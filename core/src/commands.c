#include "../inc/commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WRAP_ERR__(type)                                                       \
  case type:                                                                   \
    fputs(#type "\n", stderr);                                                 \
    break;

static void wrap_err__(xfs_err_t code) {
  switch (code) {
  case XFS_ERR_NONE:
    break;
  case XFS_ERR_INVALID_FILE:
    fputs("This is not a file, duuude!\n", stderr);
    break;
  case XFS_ERR_NOT_A_DIRECTORY:
    fputs("Do you really think this is a dir? Maa-an, see \'ls\' command!\n",
          stderr);
    break;
    WRAP_ERR__(XFS_ERR_FILENAME_NOT_FOUND)
    WRAP_ERR__(XFS_ERR_NOT_SUPPORTED)
    WRAP_ERR__(XFS_ERR_DEVICE_NOT_FOUND)
    WRAP_ERR__(XFS_ERR_DEVICE)
    WRAP_ERR__(XFS_ERR_OUT_DEVICE)
    WRAP_ERR__(XFS_ERR_MAGIC)
    WRAP_ERR__(XFS_ERR_FORMAT)
  }
}

static int ls_cmd(xfs_t *fm) {
  wrap_err__(xfs_ls(fm));
  return 1;
}

static int dog_cmd(xfs_t *fm) {
  char *filename;
  scanf("%ms", &filename);
  wrap_err__(xfs_dog(fm, filename, strlen(filename)));

  return 1;
}

static int cd_cmd(xfs_t *fm) {
  char *path;
  scanf("%ms", &path);
  wrap_err__(xfs_cd(fm, path, strlen(path)));
  free(path);
  return 1;
}

static int cp_cmd(xfs_t *fm) {
  char *from, *to;
  scanf("%ms %ms", &from, &to);
  wrap_err__(xfs_cp(fm, from, to));
  free(from);
  free(to);
  return 1;
}

static int pwd_cmd(xfs_t *fm) {
  wrap_err__(xfs_pwd(fm));
  return 1;
}

static int help_cmd() {
  fputs("ls, cd, cp, exit, help, dog", stderr);
  return 1;
}

/**
@cmd - command to execute
@fm - file manager
*/
int resolve(char *cmd, xfs_t *fm) {
  int res = 1;
  if (strcmp("ls", cmd) == 0)
    res = ls_cmd(fm);
  else if (strcmp("pwd", cmd) == 0) {
    res = pwd_cmd(fm);
  } else if (strcmp("cd", cmd) == 0)
    res = cd_cmd(fm);
  else if (strcmp("cp", cmd) == 0)
    res = cp_cmd(fm);
  // next part of cp command will be executed inside cp_cmd
  else if (strcmp("exit", cmd) == 0)
    res = 0;
  else if (strcmp("help", cmd) == 0)
    res = help_cmd();
  else if (strcmp("dog", cmd) == 0) {
    res = dog_cmd(fm);
  } else
    printf("Unknown command '%s'. Use help command\n", cmd);

  // parse end of command
  scanf("%*[^\n]");
  free(cmd);
  return res;
}
