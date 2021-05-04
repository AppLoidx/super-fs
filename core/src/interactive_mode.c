
#include<stdio.h>
#include <string.h>

#include "../inc/interactive_mode.h"
#include "../../cli/inc/interactive.h"
#include "../inc/xfs/xfs.h"
#include "../inc/commands.h"

/**
* Retuns error "xfs_err_t" if some error occurs
*/
static xfs_err_t xfs_init_from_file_to(char * fs_file, xfs_t * fm) {
  fputs("Initializing xfs fm...\n", stderr);
  xfs_err_t code;
  code = read_xfs_from_device(fm, fs_file);

  if (code == XFS_ERR_DEVICE_NOT_FOUND) {
    fputs("Device is not found\n", stderr);
  } else if (code == XFS_ERR_NOT_SUPPORTED) {
    fputs("Device is not supported\n", stderr);
  } else if (code == XFS_ERR_FORMAT) {
    fputs("Device have an invalid format\n", stderr);
  } else if (code != XFS_ERR_NONE) {
    fputs("Some error occured. Please correct your args\n", stderr);
  } else {
    fputs("Successfully initiated! Enjoy your work 😇 \n", stderr);
  }

  return code;
}


int start_interactive_mode(char * fs_file) {
  fputs("Starting interactive_mode\n", stderr);
  

  fprintf(stderr, "File name is %s\n", fs_file);

  xfs_t fm;
  xfs_init_from_file_to(fs_file, &fm);

  while (1) {
    char * input = readInput();
    if (strcmp(input, "exit") == 0) {
      fputs("Goodbye! 😛😛😛 \n", stderr);
      break;
    }
    fprintf(stderr, "You input is %s\n", input);
    resolve(input, &fm);
  }

  return 0;
}


