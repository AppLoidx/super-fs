#ifndef _XFS_H_
#define _XFS_H_

#include "xfs_types.h"

/* 8-битная длина файла -- максимум 256 символов */
#define FILENAME_BUFLEN 256

typedef struct xfs {
  FILE *f;
  xfs_sb_t sb;
  xfs_ino_t ino_current_dir;
} xfs_t;

typedef enum xfs_err {
  XFS_ERR_NONE = 0,
  XFS_ERR_NOT_SUPPORTED,
  XFS_ERR_DEVICE_NOT_FOUND,
  XFS_ERR_DEVICE,
  XFS_ERR_OUT_DEVICE,
  XFS_ERR_MAGIC,
  XFS_ERR_FORMAT,
  XFS_ERR_FILENAME_NOT_FOUND,
  XFS_ERR_NOT_A_DIRECTORY,
  XFS_ERR_INVALID_FILE,
} xfs_err_t;

#define XFS_CHKTHROW(action)                                                \
  do {                                                                         \
    xfs_err_t xfs_err##__LINE__ = action;                                \
    if (xfs_err##__LINE__ != XFS_ERR_NONE)                               \
      return xfs_err##__LINE__;                                             \
  } while (0)

xfs_err_t read_xfs_from_device(xfs_t *fm, char const *device_path);
xfs_err_t xfs_sample(xfs_t *fm);
xfs_err_t xfs_ls(xfs_t *fm);
xfs_err_t xfs_cd(xfs_t *fm, char const *dirname, size_t dirname_size);
xfs_err_t xfs_cp(xfs_t *fm, char const *from, char const *to);
xfs_err_t xfs_dog(xfs_t *fm, char const *filename, size_t filename_size);


#endif
