#include "image.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int x;
  int y;
} Index2D;

Index2D matrix_transform_sq(int x, int y, int dim) {
  const int LUCAS1 = 11;
  const int LUCAS2 = 47;
  Index2D out;
  out.x = (LUCAS1 * x) % dim;
  out.y = (LUCAS2 * y) % dim;
  return out;
}

void matrix_transform_image(Image *img) {
  // Avoid operating on the data in place
  Image backup = clone_image(img);
  for (int x = 0; x < IMAGE_SIZE; ++x) {
    for (int y = 0; y < IMAGE_SIZE; ++y) {
      Index2D dest = matrix_transform_sq(x, y, IMAGE_SIZE);
      img->data[dest.x][dest.y] = backup.data[x][y];
    }
  }
  free(backup.data);
}

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