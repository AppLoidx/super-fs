#include "../inc/xfs/xfs.h"
#include "../inc/xfs/utils.h"
#include <alloca.h>
#include <endian.h>
#include <features.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

xfs_err_t read_xfs_from_device(xfs_t *fm, char const *device_path) {
  fm->f = fopen(device_path, "r");
  if (!fm->f)
    return XFS_ERR_DEVICE_NOT_FOUND;
  if (fread(&fm->sb, sizeof(xfs_sb_t), 1, fm->f) != 1) {
    fclose(fm->f);
    return XFS_ERR_DEVICE;
  }
  if (be32toh(fm->sb.sb_magicnum) != XFS_SB_MAGIC)
    return XFS_ERR_MAGIC;
  fm->ino_current_dir = be64toh(fm->sb.sb_rootino);
  return XFS_ERR_NONE;
}

static xfs_ino_t inode_number_(xfs_dir2_inou_t u, int is64) {
  if (is64) {
    return be64toh(*(__uint64_t *)u.i8.i);
  } else {
    return be32toh(*(__uint32_t *)u.i4.i);
  }
}

static xfs_err_t xfs_iter_extents_(xfs_t *fm, xfs_dinode_core_t *core,
                                   void *dfork, void *self,
                                   callback_t callback) {
  struct xfs_bmbt_rec *list = dfork;
  struct xfs_bmbt_irec unwrapped;
  void *data = alloca(be32toh(fm->sb.sb_blocksize));

  int is_running = 1;
  xfs_filblks_t read_blocks = 0;
  for (xfs_extnum_t i = 0; is_running && i < be32toh(core->di_nextents); ++i) {
    xfs_bmbt_disk_get_all(&list[i], &unwrapped);
    fseek(fm->f, unwrapped.br_startblock * be32toh(fm->sb.sb_blocksize),
          SEEK_SET);
    for (int j = 0; is_running && unwrapped.br_startoff <= read_blocks &&
                    j < unwrapped.br_blockcount;
         ++j) {
      if (fread(data, be32toh(fm->sb.sb_blocksize), 1, fm->f) != 1)
        return XFS_ERR_DEVICE;
      ++read_blocks;
      if (!(*callback)(self, data))
        return XFS_ERR_NONE;
    }
  }
  return XFS_ERR_NONE;
}

static xfs_err_t xfs_iter_btree_(xfs_t *fm, xfs_dinode_core_t *core,
                                 void *dfork, void *self, callback_t callback) {
  return XFS_ERR_NOT_SUPPORTED;
}

typedef xfs_err_t (*iterfun_t)(xfs_t *fm, xfs_dinode_core_t *core, void *dfork,
                               void *self, callback_t callback);

typedef struct xfs_dir_entry {
  ino_t inumber;
  char name[FILENAME_BUFLEN];
  __uint8_t namelen;
  __uint8_t ftype;
} xfs_dir_entry_t;

static xfs_err_t xfs_dir_iter_local_(xfs_t *fm, xfs_dinode_core_t *core,
                                     void *dfork, void *self,
                                     callback_t callback) {
  xfs_dir2_sf_t *sf = dfork;
  xfs_ino_t parent = inode_number_(sf->hdr.parent, sf->hdr.i8count);
  xfs_dir_entry_t entry;

  entry.namelen = 1;
  entry.name[0] = '.';
  entry.name[1] = '\0';
  entry.inumber = fm->ino_current_dir;
  entry.ftype = XFS_DIR3_FT_DIR;
  if (!(*callback)(self, &entry))
    return XFS_ERR_NONE;

  entry.namelen = 2;
  entry.name[0] = '.';
  entry.name[1] = '.';
  entry.name[2] = '\0';
  entry.inumber = parent;
  entry.ftype = XFS_DIR3_FT_DIR;
  if (!(*callback)(self, &entry))
    return XFS_ERR_NONE;

  xfs_dir2_sf_entry_t *iter =
      (xfs_dir2_sf_entry_t *)((char *)&sf->hdr.parent +
                              (sf->hdr.count ? sizeof(sf->hdr.parent.i4)
                                             : sizeof(sf->hdr.parent.i8)));

  for (int i = 0; i < sf->hdr.count || i < sf->hdr.i8count; ++i) {
    entry.namelen = iter->namelen;
    memcpy(entry.name, iter->name, entry.namelen);
    entry.name[entry.namelen] = 0;

    xfs_dir2_sf_entry_footer_t *footer =
        (xfs_dir2_sf_entry_footer_t *)((char *)iter +
                                       offsetof(xfs_dir2_sf_entry_t, name) +
                                       iter->namelen);

    entry.ftype = footer->ftype;
    entry.inumber = inode_number_(footer->inumber, 0);
    if (!(*callback)(self, &entry))
      return 0;

    iter =
        (xfs_dir2_sf_entry_t *)((char *)footer +
                                offsetof(xfs_dir2_sf_entry_footer_t, inumber) +
                                (sf->hdr.count ? sizeof(xfs_dir2_ino4_t)
                                               : sizeof(xfs_dir2_ino8_t)));
  }
  return XFS_ERR_NONE;
}

typedef struct xfs_dir_iter_block_self {
  xfs_t *fm;
  void *entry_self;
  callback_t entry_callback;
  xfs_err_t err;
} xfs_dir_iter_block_self_t;

static int xfs_dir_iter_block_(void *self_, void *data) {
  // iterate over the block

  xfs_dir_iter_block_self_t *self = self_;
  struct xfs_dir3_data_hdr *dir3_header = data;

  if (be32toh(dir3_header->hdr.magic) != XFS_DIR3_BLOCK_MAGIC &&
      be32toh(dir3_header->hdr.magic) != XFS_DIR3_DATA_MAGIC) {
    self->err = XFS_ERR_MAGIC;
    return 0;
  }

  xfs_dir_entry_t entry;
  xfs_dir2_data_union_t *iter = dir3_header->entries;
  while ((char *)iter + sizeof(xfs_dir2_data_union_t) <=
             (char *)data + be32toh(self->fm->sb.sb_blocksize) &&
         iter->unused.freetag != XFS_DIR2_DATA_UNUSED_FREETAG) {
    entry.namelen = iter->entry.namelen;
    memcpy(entry.name, iter->entry.name, entry.namelen);
    entry.name[iter->entry.namelen] = 0;
    entry.inumber = be64toh(iter->entry.inumber);
    entry.ftype = iter->entry.name[entry.namelen];

    if (!(*self->entry_callback)(self->entry_self, &entry)) {
      self->err = XFS_ERR_NONE;
      return 0;
    }

    size_t offset = sizeof(xfs_dir2_data_entry_t) - sizeof(char) +
                    iter->entry.namelen * sizeof(char);
    offset +=
        sizeof(ino_t) - ((offset + sizeof(ino_t) - 1) % sizeof(ino_t) + 1);
    iter = (xfs_dir2_data_union_t *)((char *)iter + offset);
  }
  return 1;
}

static xfs_err_t xfs_dir_iter_extents_(xfs_t *fm, xfs_dinode_core_t *core,
                                       void *dfork, void *self,
                                       callback_t callback) {
  xfs_dir_iter_block_self_t block_self;
  block_self.fm = fm;
  block_self.entry_self = self;
  block_self.entry_callback = callback;
  block_self.err = XFS_ERR_NONE;

  XFS_CHKTHROW(
      xfs_iter_extents_(fm, core, dfork, &block_self, xfs_dir_iter_block_));
  return block_self.err;
}

static xfs_err_t xfs_dir_iter_btree_(xfs_t *fm, xfs_dinode_core_t *core,
                                     void *dfork, void *self,
                                     callback_t callback) {
  xfs_dir_iter_block_self_t block_self;
  block_self.fm = fm;
  block_self.entry_self = self;
  block_self.entry_callback = callback;
  block_self.err = XFS_ERR_NONE;

  XFS_CHKTHROW(
      xfs_iter_btree_(fm, core, dfork, &block_self, xfs_dir_iter_block_));
  return block_self.err;
}

/**
  Use observer pattern for dir in fm
*/
xfs_err_t xfs_dir_iter_(xfs_t *fm, void *self, callback_t callback) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * fm->ino_current_dir, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  if (be16toh(di->di_magic) != XFS_DINODE_MAGIC)
    return XFS_ERR_MAGIC;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));

  iterfun_t iterfun = NULL;
  switch (di->di_format) {
  case XFS_DINODE_FMT_LOCAL:
    iterfun = xfs_dir_iter_local_;
    break;
  case XFS_DINODE_FMT_EXTENTS:
    iterfun = xfs_dir_iter_extents_;
    break;
  case XFS_DINODE_FMT_BTREE:
    iterfun = xfs_dir_iter_btree_;
    break;
  }

  if (!iterfun)
    return XFS_ERR_FORMAT;

  return iterfun(fm, di, dfork, self, callback);
}

static int dir_entry_observer_call__(void *self, void *data) {
  xfs_dir_entry_t *entry = data;
  switch (entry->ftype) {
  case XFS_DIR3_FT_DIR:
    fputs("[dir 📁 ] \t", stdout);
    break;
  case XFS_DIR3_FT_REG_FILE:
    fputs("[file 📝 ] \t", stdout);
    break;
  default:
    fputs("[] \t\t", stdout);
    break;
  }
  puts(entry->name);
  return 1;
}

xfs_err_t xfs_ls(xfs_t *fm) {
  return xfs_dir_iter_(fm, NULL, dir_entry_observer_call__);
}

typedef struct xfs_dir_entry_find_callback_self {
  xfs_dir_entry_t *expected;
  xfs_err_t err;
} xfs_dir_entry_find_callback_self_t;

static int xfs_dir_entry_find_callback_(void *self_, void *data) {
  xfs_dir_entry_find_callback_self_t *self = self_;
  xfs_dir_entry_t *entry = data;
  if (entry->namelen == self->expected->namelen &&
      memcmp(entry->name, self->expected->name, entry->namelen) == 0) {
    self->expected->ftype = entry->ftype;
    self->expected->inumber = entry->inumber;
    return 0;
  }
  return 1;
}

xfs_err_t xfs_find_entry_(xfs_t *fm, xfs_dir_entry_t *expected) {
  xfs_dir_entry_find_callback_self_t self;
  self.expected = expected;
  self.err = XFS_ERR_NONE;
  XFS_CHKTHROW(xfs_dir_iter_(fm, &self, xfs_dir_entry_find_callback_));
  return self.err;
}

xfs_err_t xfs_cd(xfs_t *fm, char const *dirname, size_t dirname_size) {
  if (dirname_size == 1 && dirname[0] == '/') {
    fm->ino_current_dir = be64toh(fm->sb.sb_rootino);
    return XFS_ERR_NONE;
  }
  xfs_dir_entry_t dir;
  memcpy(dir.name, dirname, dirname_size);
  dir.namelen = dirname_size;
  XFS_CHKTHROW(xfs_find_entry_(fm, &dir));
  if (dir.ftype != XFS_DIR3_FT_DIR)
    return XFS_ERR_NOT_A_DIRECTORY;
  fm->ino_current_dir = dir.inumber;
  return XFS_ERR_NONE;
}

typedef struct xfs_cp_file_block_self {
  __uint32_t blocksize;
  FILE *stream;
  xfs_err_t err;
} xfs_cp_file_block_self_t;

static int xfs_cp_file_block__(void *self_, void *data) {
  xfs_cp_file_block_self_t *self = self_;
  if (fwrite(data, self->blocksize, 1, self->stream) != 1) {
    self->err = XFS_ERR_OUT_DEVICE;
    return 0;
  }
  self->err = XFS_ERR_NONE;
  return 1;
}

static xfs_err_t xfs_cp_file__(xfs_t *fm, xfs_dir_entry_t *dir_entry) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * dir_entry->inumber, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  if (be16toh(di->di_magic) != XFS_DINODE_MAGIC)
    return XFS_ERR_MAGIC;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));

  iterfun_t iterfun = NULL;
  switch (di->di_format) {
    case XFS_DINODE_FMT_EXTENTS:
      iterfun = xfs_iter_extents_;
      break;
    case XFS_DINODE_FMT_BTREE:
      iterfun = xfs_iter_btree_;
      break;
  }

  if (!iterfun)
    return XFS_ERR_FORMAT;

  FILE *f = fopen(dir_entry->name, "wb");
  if (!f)
    return XFS_ERR_OUT_DEVICE;
  xfs_cp_file_block_self_t self;
  self.blocksize = be32toh(fm->sb.sb_blocksize);
  self.stream = f;
  self.err = XFS_ERR_NONE;
  xfs_err_t iter_err = iterfun(fm, di, dfork, &self, xfs_cp_file_block__);
  fclose(f);
  XFS_CHKTHROW(iter_err);
  return self.err;
}

static int is_self_or_parent_(char const *name) {
  return strcmp(".", name) == 0 || strcmp("..", name) == 0;
}

static xfs_err_t xfs_cp_entry_(xfs_t *fm, xfs_dir_entry_t *what);

typedef struct xfs_cp_dir_entry_callback_self {
  xfs_t *fm;
  xfs_err_t err;
} xfs_cp_dir_entry_callback_self_t;

static int xfs_cp_dir_entry_callback_(void *self_, void *data) {
  xfs_cp_dir_entry_callback_self_t *self = self_;
  xfs_dir_entry_t *entry = data;
  if (is_self_or_parent_(entry->name))
    return 1;
  self->err = xfs_cp_entry_(self->fm, entry);
  return self->err == XFS_ERR_NONE;
}

static xfs_err_t xfs_cp_dir_(xfs_t *fm, xfs_dir_entry_t *what) {
  char *old_path = getcwd(NULL, 0);
  if (!is_self_or_parent_(what->name)) {
    if (mkdir(what->name, 0777) || chdir(what->name)) {
      free(old_path);
      return XFS_ERR_OUT_DEVICE;
    }
  }
  xfs_ino_t old = fm->ino_current_dir;
  fm->ino_current_dir = what->inumber;

  xfs_cp_dir_entry_callback_self_t self;
  self.fm = fm;
  self.err = XFS_ERR_NONE;
  xfs_dir_iter_(fm, &self, xfs_cp_dir_entry_callback_);

  fm->ino_current_dir = old;
  if (!chdir(old_path)) {
    free(old_path);
    return XFS_ERR_OUT_DEVICE;
  }
  free(old_path);
  return self.err;
}

static xfs_err_t xfs_cp_entry_(xfs_t *fm, xfs_dir_entry_t *what) {
  switch (what->ftype) {
  case XFS_DIR3_FT_REG_FILE:
    xfs_cp_file__(fm, what);
    break;
  case XFS_DIR3_FT_DIR:
    xfs_cp_dir_(fm, what);
    break;
  default:
    return XFS_ERR_NOT_SUPPORTED;
  }
  return XFS_ERR_NONE;
}

xfs_err_t xfs_cp(xfs_t *fm, char const *from, char const *to) {
  char *old_path = getcwd(NULL, 0);
  if (!is_self_or_parent_(to)) {
    if (mkdir(to, 0777) || chdir(to)) {
      free(old_path);
      return XFS_ERR_OUT_DEVICE;
    }
  }

  xfs_dir_entry_t entry;
  entry.namelen = strlen(from);
  strcpy(entry.name, from);
  xfs_find_entry_(fm, &entry);
  xfs_cp_entry_(fm, &entry);

  if (chdir(old_path)) {
    free(old_path);
    return XFS_ERR_OUT_DEVICE;
  }
  free(old_path);
  return XFS_ERR_NONE;
}


static xfs_err_t xfs_iter_over_file_blocks_(xfs_t *fm, xfs_dir_entry_t *dir_entry, void *self, callback_t callback) {
  __uint16_t inodesize = be16toh(fm->sb.sb_inodesize);
  void *inode_info = alloca(inodesize);
  fseek(fm->f, inodesize * dir_entry->inumber, SEEK_SET);
  if (fread(inode_info, inodesize, 1, fm->f) != 1)
    return XFS_ERR_DEVICE;
  xfs_dinode_core_t *di = inode_info;
  if (be16toh(di->di_magic) != XFS_DINODE_MAGIC)
    return XFS_ERR_MAGIC;
  void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));

  iterfun_t iterfun = NULL;
  switch (di->di_format) {
  case XFS_DINODE_FMT_EXTENTS:
    iterfun = xfs_iter_extents_;
    break;
  case XFS_DINODE_FMT_BTREE:
    iterfun = xfs_iter_btree_;
    break;
  }

  if (!iterfun)
    return XFS_ERR_FORMAT;

  xfs_err_t iter_err = iterfun(fm, di, dfork, self, callback);
  return iter_err;
  XFS_CHKTHROW(iter_err);
}


// cat analog :)
xfs_err_t xfs_dog(xfs_t *fm, char const *filename, size_t filename_size) {
  xfs_dir_entry_t file;
  memcpy(file.name, filename, filename_size);
  file.namelen = filename_size;
  XFS_CHKTHROW(xfs_find_entry_(fm, &file));

  if (file.ftype != XFS_DIR3_FT_REG_FILE)
    return XFS_ERR_INVALID_FILE;

  xfs_cp_file_block_self_t this;
  this.blocksize = be32toh(fm->sb.sb_blocksize);
  this.stream = stdout; // output
  this.err = XFS_ERR_NONE;
  
  xfs_iter_over_file_blocks_(fm, &file, &this, xfs_cp_file_block__);
  return this.err;
}
