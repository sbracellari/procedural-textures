#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_POINTS 100

float mindist = INFINITY;
float maxdist = 0;

// these are global for now but i guess they don't need to be
int xcoords[NUM_POINTS], ycoords[NUM_POINTS];
float distances[IMAGE_SIZE][IMAGE_SIZE], colors[IMAGE_SIZE][IMAGE_SIZE];
float colors[IMAGE_SIZE][IMAGE_SIZE];

// generate some random data as a starting point
void generate_data() {
  srand(time(0));
  for (int i = 0; i < NUM_POINTS; i++) {
    xcoords[i] = rand() % IMAGE_SIZE;
    ycoords[i] = rand() % IMAGE_SIZE;
  }
}

// for any given point (x,y), look at a number of surrounding
// points (NUM_POINTS) and calculate the distance to the closest one
// this is pretty inefficient as it stands but that might lend itself
// well to the parallelization that'll take place later on
float distance() {
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      mindist = INFINITY;
      for (int i = 0; i < NUM_POINTS; i++) {
        float distance = sqrt(pow(xcoords[i] - x, 2) + pow(ycoords[i] - y, 2));
        if (distance < mindist) {
          mindist = distance;
        }
      }

      distances[x][y] = mindist;
    }
  }
}

// basically we just want to convert each value in distances
// to a meaningful color value. as points get closer together,
// they get darker
void color_convert() {
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      if (distances[x][y] < mindist) {
        mindist = distances[x][y];
      } else if (distances[x][y] > maxdist) {
        maxdist = distances[x][y];
      }

      colors[x][y] = (distances[x][y] - mindist) / (maxdist - mindist);
    }
  }
}

int main() {
  FILE *temp = fopen("cells.txt", "w");

  generate_data();
  distance();
  color_convert();

  // write data to a file for graphing purposes
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      fprintf(temp, "%f\n", colors[x][y]);
    }
  }
  printf("Generated Image\n");
  temp = fopen("cells.txt", "r");
  Image x = load_image(temp);
  printf("Loaded image\n");
  matrix_transform_image(&x);
  printf("Transformed Image\n");
  write_image(fopen("cells2.txt", "w"), &x);
  // printf("%f", x.data[0][0]);

  return 0;
}