#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DIM 500
#define NUM_POINTS 100

float mindist = INFINITY;
float maxdist = 0;

// these are global for now but i guess they don't need to be
int xcoords[NUM_POINTS], ycoords[NUM_POINTS];
float distances[DIM][DIM], colors[DIM][DIM];
float colors[DIM][DIM];

// generate some random data as a starting point
void generate_data() {
  srand(time(0));
  for(int i = 0; i < NUM_POINTS; i++) {
    xcoords[i] = rand() % DIM;
    ycoords[i] = rand() % DIM;
  } 
}

// for any given point (x,y), look at a number of surrounding
// points (NUM_POINTS) and calculate the distance to the closest one
// this is pretty inefficient as it stands but that might lend itself
// well to the parallelization that'll take place later on
float distance() {
  for (int x = 0; x < DIM; x++) {
    for (int y = 0; y < DIM; y++ ) {
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
  for(int x = 0; x < DIM; x++) {
    for(int y = 0; y < DIM; y++) {
      if (distances[x][y] < mindist) {
        mindist = distances[x][y];
      } else if (distances[x][y] > maxdist) { 
        maxdist = distances[x][y]; 
      }

      colors[x][y] = (distances[x][y] - mindist)/(maxdist-mindist);
    }
  } 
}
  
int main() {
  FILE *temp = fopen("cells.txt", "w");
  
  generate_data();
  distance();
  color_convert();

  // write data to a file for graphing purposes
  for(int x = 0; x < DIM; x++) {
    for(int y = 0; y < DIM; y++) {
      fprintf(temp, "%f\n", colors[x][y]);
    }
  }

  return 0;
}