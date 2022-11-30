// The image.c file contains functions to initialize, fill, clone
// write, load, and swap an image. This file relies on the Image struct 
// defined in the image.h file. These functions are utilized in transform.c
// when transforming created images. 

#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initializes an image using the Image structure
Image new_image() {
  Image img;
  img.data = malloc(sizeof(float[IMAGE_SIZE][IMAGE_SIZE]));

  return img;
}

// Initializes an image and fills it with random values
Image rand_image() {
  Image img = new_image();
  srand(time(NULL));
  for (int i = 0; i < IMAGE_SIZE; ++i) {
    for (int j = 0; j < IMAGE_SIZE; ++j) {
      img.data[i][j] = (float)rand() / (float)(RAND_MAX);
    }
  }
  return img;
}

// Makes a copy of an image and can be used if you don't want
// modifications to impact the original image
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

// Writes the image out to a file to be viewed via
// the jupyter notebook
void write_image(FILE *f, const Image *img) {
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      fprintf(f, "%f\n", img->data[x][y]);
    }
  }
}

// Loads an image from a file
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

// Swaps data for img1 and img2
void swap_image(Image *img1, Image *img2) {
  Image tmp = *img1;
  *img1 = *img2;
  *img2 = tmp;
}