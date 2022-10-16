#include "image.h"
#include <stdio.h>
#include <stdlib.h>

Image new_image() {
  Image img;
  img.data = malloc(sizeof(float[IMAGE_SIZE][IMAGE_SIZE]));
  img.xcoords = malloc(sizeof(float[NUM_POINTS]));
  img.ycoords = malloc(sizeof(float[NUM_POINTS]));

  return img;
}

Image clone_image(const Image *original) {
  Image out;
  out.data = malloc(sizeof(float[IMAGE_SIZE][IMAGE_SIZE]));
  out.xcoords = malloc(sizeof(float[NUM_POINTS]));
  out.ycoords = malloc(sizeof(float[NUM_POINTS]));

  for (int x = 0; x < NUM_POINTS; x++) {
    out.xcoords[x] = original->xcoords[x];
    out.ycoords[x] = original->ycoords[x];
  }

  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      out.data[x][y] = original->data[x][y];
    }
  }

  return out;
}

void write_image(FILE *f, FILE *xcoord, FILE *ycoord, const Image *img) {
  for (int x = 0; x < NUM_POINTS; x++) {
    fprintf(xcoord, "%i\n", img->xcoords[x]);
    fprintf(ycoord, "%i\n", img->ycoords[x]);
  }

  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      fprintf(f, "%f\n", img->data[x][y]);
    }
  }
}

Image load_image(FILE *f, FILE *xcoord, FILE *ycoord) {
  if (f == NULL || xcoord == NULL || ycoord == NULL) {
    exit(1);
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  Image img = new_image();
  int counter = 0;
  while ((read = getline(&line, &len, f)) != -1) {
    float pt = strtof(line, NULL);
    img.data[counter / IMAGE_SIZE][counter % IMAGE_SIZE] = pt;
    counter++;
  }

  char *linex = NULL;
  size_t lenx = 0;
  ssize_t readx;
  int counterx = 0;
  while ((readx = getline(&linex, &lenx, xcoord)) != -1) {
    int ptx = strtof(linex, NULL);
    img.xcoords[counterx];
    counterx++;
  }

  char *liney = NULL;
  size_t leny = 0;
  ssize_t ready;
  int countery = 0;
  while ((ready = getline(&liney, &leny, ycoord)) != -1) {
    int pty = strtof(liney, NULL);
    img.ycoords[countery];
    countery++;
  }

  return img;
}