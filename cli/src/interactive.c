#include<stdio.h>
#include<stdlib.h>
#include "../inc/interactive.h"


char * readInput() {
  fputs("Krasavchik?> ", stdout);
  char *cmd;
  scanf("%ms", &cmd);

  return cmd;
}
