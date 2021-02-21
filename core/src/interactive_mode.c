
#include<stdio.h>

#include "../inc/interactive_mode.h"
#include "../../cli/inc/interactive.h"


int startInteractiveMode() {
  fputs("Starting interactive_mode\n", stderr);

  char * input = readInput();

  fprintf(stderr, "You input is %s\n", input);

  return 0;
}
