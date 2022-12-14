// The cells.c file is the main.c file for
// this project. When compiled, this is the
// file to run. 

#include "image.h"
#include "transform.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define true 1
#define false 0

int main() {
  long start = clock();
  int do_io = true;
  generate_files(do_io);
  double duration = (double)(clock() - start) / (CLOCKS_PER_SEC / 1000);
  printf("Total Time Taken: %f ms\n", duration);
  return 0;
}