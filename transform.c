#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int LUCAS1 = 11;
const int LUCAS2 = 47;

typedef void (*Transform)(Image *);

typedef struct {
  int x, y;
} Index2D;

Index2D matrix_transform_sq(int x, int y, int dim) {
  Index2D out;
  out.x = (LUCAS1 * x) % dim;
  out.y = (LUCAS2 * y) % dim;
  return out;
}

/// @brief A simple transform mapping existing pixels to new destinations.
/// Appears to arrange data into roughly grid shaped patterns.
/// @param img A pointer to a heap allocated Image.
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

typedef struct {
  int(*xcoords), (*ycoords);
} Points;

// generate some random data as a starting points
Points generate_points() {
  Points ps = {.xcoords = malloc(sizeof(float[NUM_POINTS])),
               .ycoords = malloc(sizeof(float[NUM_POINTS]))};
  srand(time(NULL));
  for (int i = 0; i < NUM_POINTS; i++) {
    ps.xcoords[i] = rand() % IMAGE_SIZE;
    ps.ycoords[i] = rand() % IMAGE_SIZE;
  }
  return ps;
}

// for any given point (x,y), look at a number of surrounding
// points (NUM_POINTS) and calculate the distance to the closest one
// this is pretty inefficient as it stands but that might lend itself
// well to the parallelization that'll take place later on
void distance(Image *img) {
  Points ps = generate_points();
  float mindist = INFINITY;
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      mindist = INFINITY;
      for (int i = 0; i < NUM_POINTS; i++) {
        float distance =
            sqrt(pow(ps.xcoords[i] - x, 2) + pow(ps.ycoords[i] - y, 2));
        if (distance < mindist) {
          mindist = distance;
        }
      }

      img->data[x][y] = mindist;
    }
  }
  free(ps.xcoords);
  free(ps.ycoords);
}

// The folding technique is used here
// Fold each value 3 times
// There are two triple nested for loops that will
// be parallelized later on for improved performance
void folding(Image *img) {
  float scale_A[IMAGE_SIZE][IMAGE_SIZE];
  float fold_A[IMAGE_SIZE][IMAGE_SIZE];

  float temp_A[IMAGE_SIZE][IMAGE_SIZE];

  // temp value for data so it doesnt overwrite, maybe add clone?
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      temp_A[x][y] = img->data[x][y];
    }
  }

  for (int nfolds = 0; nfolds < 3; nfolds++) {
    float min_A = temp_A[0][0];
    float max_A = temp_A[0][0];
    for (int x = 0; x < IMAGE_SIZE; x++) {
      for (int y = 0; y < IMAGE_SIZE; y++) {
        if (min_A > temp_A[x][y]) {
          min_A = temp_A[x][y];
        } else if (max_A < temp_A[x][y]) {
          max_A = temp_A[x][y];
        }
      }
    }
    max_A = max_A - min_A;

    for (int x = 0; x < IMAGE_SIZE; x++) {
      for (int y = 0; y < IMAGE_SIZE; y++) {
        if (nfolds == 0) {
          img->data[x][y] = temp_A[x][y];
        }
        scale_A[x][y] = img->data[x][y] - min_A;
        fold_A[x][y] = max_A / (nfolds + 1);
        img->data[x][y] = fabs(scale_A[x][y] - fold_A[x][y]);
      }
    }
  }
}

// basically we just want to convert each value in distances
// to a meaningful color value. as points get closer together,
// they get darker
void color_convert(Image *img) {
  float mindist = INFINITY;
  float maxdist = 0;
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      if (img->data[x][y] < mindist) {
        mindist = img->data[x][y];
      } else if (img->data[x][y] > maxdist) {
        maxdist = img->data[x][y];
      }

      img->data[x][y] = (img->data[x][y] - mindist) / (maxdist - mindist);
    }
  }
}

void write_graph(Transform t, const char *fname) {
  Image img = new_image();

  distance(&img);

  t(&img);
  color_convert(&img);
  printf("Generated Image\n");

  // matrix_transform_image(&img);
  // printf("Transformed Image\n");
  write_image(fopen(fname, "w"), &img);
}

void write_graph_file() {
  Transform t[2];
  t[0] = &folding;
  t[1] = &matrix_transform_image;
  for (int img_id = 0; img_id < 2; img_id++) {
    char fname[20];
    sprintf(fname, "img/%d", img_id);
    write_graph(t[img_id], fname);
  }
}