#include<stdio.h>
#include<string.h>

#include "../../cli/inc/interactive.h"
#include "../inc/interactive_mode.h"
#include "../inc/device.h"

int main(int argc, char *argv[]) {
  

  if (argc != 2) {
    fputs("Invalid command-line arguments!", stderr);
    return 1;
  }

  if (!strcmp(argv[1], "info")) {
    fputs("Start application in info mode\n", stderr);
    print_devices(stderr);
  } else if (!strcmp(argv[1], "start")) {
    return startInteractiveMode();

  } else {
    fputs("Invalid command-line argument. Please refer to \"super-fs help\" command", stderr);
    return 1;
  }


  return 0;
}
