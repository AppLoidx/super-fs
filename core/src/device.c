#include "../inc/device.h"
#include <stdio.h>
#include <stdlib.h>

void print_devices(FILE* file) {
  FILE *f = fopen("/proc/mounts", "r");
  if (!f)
    return;

  int line = 0;
  int successful_assigns;
  
  puts("\nDevice\tMount point\t\tFilesystem\t\tMode");
  do {
    char *device, *mountpoint, *filesystem, *mode;
    successful_assigns = fscanf(f, "%ms%ms%ms%ms%*[^\n]", &device, &mountpoint,
                                &filesystem, &mode);

    if (successful_assigns == 4) {
      printf("%s\t%s\t%s\t%s\n", device, mountpoint, filesystem, mode);
    }

    switch (successful_assigns) {
      case 4:
        free(mode);
      case 3:
        free(filesystem);
      case 2:
        free(mountpoint);
      case 1:
        free(device);
      default:
        break;
    }
  } while (successful_assigns > 0);

  fclose(f);
}
