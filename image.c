#include "image.h"
#include <stdio.h>
#include <stdlib.h>

Image new_image() {
  Image img;
  img.data = malloc(sizeof(float[IMAGE_SIZE][IMAGE_SIZE]));

  return img;
}

Image clone_image(const Image *original) {
  Image out;
  out.data = malloc(sizeof(float[IMAGE_SIZE][IMAGE_SIZE]));

  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      out.data[x][y] = original->data[x][y];
    }
  }

  return out;
}

void write_image(FILE *f, const Image *img) {
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      fprintf(f, "%f\n", img->data[x][y]);
    }
  }
}

Image load_image(FILE *f) {
  if (f == NULL) {
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
  return img;
}