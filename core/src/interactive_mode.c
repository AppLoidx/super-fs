
#include<stdio.h>

#include "../inc/interactive_mode.h"
#include "../../cli/inc/interactive.h"


int startInteractiveMode(char * fs_file) {
  fputs("Starting interactive_mode\n", stderr);
  

  fprintf(stderr, "File name is %s\n", fs_file);
  char * input = readInput();

  fprintf(stderr, "You input is %s\n", input);

  return 0;
}
