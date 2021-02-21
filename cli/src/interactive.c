#include<stdio.h>
#include<stdlib.h>
#include "../inc/interactive.h"


char * readInput() {
  char* input = malloc(sizeof(char) * INPUT_BUF_SIZE);
  fgets(input, INPUT_BUF_SIZE, stdin);
  return input;
}
