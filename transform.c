#include "transform.h"
#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// const int LUCAS1 = 11;
const int LUCAS1 = 29;
// const int LUCAS2 = 47;
const int LUCAS2 = 199;

typedef struct {
  int x, y;
  float value;
} Score;

int score_comp(const void *elem1, const void *elem2) {
  Score f = *((Score *)elem1);
  Score s = *((Score *)elem2);
  if (f.value > s.value)
    return 1;
  if (f.value < s.value)
    return -1;
  return 0;
}

Points top_points(Image *img) {
  Points ps = init_points();
  Score best_col[IMAGE_SIZE];
  for (int i = 0; i < IMAGE_SIZE; ++i) {
    Score best = {.x = i, .y = 0, .value = img->data[i][0]};
    for (int j = 0; j < IMAGE_SIZE; ++j) {
      if (best.value < img->data[i][j]) {
        best = (Score){.x = i, .y = j, .value = img->data[i][j]};
      }
    }
    best_col[i] = best;
  }
  qsort(&best_col, IMAGE_SIZE, sizeof(Score), score_comp);
  for (int i = 0; i < NUM_POINTS; ++i) {
    ps.xcoords[i] = best_col[i].x;
    ps.ycoords[i] = best_col[i].y;
  }
  return ps;
}

Points all_top_points(Image *img) {
  Points ps = init_points();
  Score scores[IMAGE_SIZE * IMAGE_SIZE];
  for (int i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i) {
    int x = i % IMAGE_SIZE;
    int y = i / IMAGE_SIZE;
    Score score = {.x = x, .y = y, .value = img->data[y][x]};
    scores[i] = score;
  }
  qsort(&scores, IMAGE_SIZE * IMAGE_SIZE, sizeof(Score), score_comp);
  for (int i = 0; i < NUM_POINTS; ++i) {
    ps.xcoords[i] = scores[i].x;
    ps.ycoords[i] = scores[i].y;
  }
  return ps;
}

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
      img->data[dest.x][dest.y] += backup.data[x][y];
    }
  }
  free(backup.data);
}

Points init_points() {
  Points ps = {.xcoords = malloc(sizeof(float[NUM_POINTS])),
               .ycoords = malloc(sizeof(float[NUM_POINTS]))};
  return ps;
}

// generate some random data as a starting points
Points generate_points() {
  Points ps = init_points();
  srand(time(NULL));
  for (int i = 0; i < NUM_POINTS; i++) {
    ps.xcoords[i] = rand() % IMAGE_SIZE;
    ps.ycoords[i] = rand() % IMAGE_SIZE;
  }
  return ps;
}

Points generate_points2() {
  Image img = rand_image();
  Points top = top_points(&img);
  free(img.data);
  return top;
}

Points generate_points3(Image *img) {
  Points top = all_top_points(img);
  return top;
}

// for any given point (x,y), look at a number of surrounding
// points (NUM_POINTS) and calculate the distance to the closest one
// this is pretty inefficient as it stands but that might lend itself
// well to the parallelization that'll take place later on
void distance(Image *restrict img) {
  // Todo less stupid way of using an image to fetch random points
  Points ps = generate_points3(img);
  float(*data)[IMAGE_SIZE] = (img->data);
  float mindist = INFINITY;
#pragma acc kernels
  {
    for (int x = 0; x < IMAGE_SIZE; x++) {
      for (int y = 0; y < IMAGE_SIZE; y++) {
        mindist = INFINITY;
#pragma acc loop reduction(min : mindist)
        for (int i = 0; i < NUM_POINTS; i++) {
          float x_dist = (ps.xcoords[i] - x) * (ps.xcoords[i] - x);
          float y_dist = (ps.ycoords[i] - y) * (ps.ycoords[i] - y);
          float distance = sqrt(x_dist + y_dist);
          mindist = fmin(distance, mindist);
        }

        data[x][y] = mindist;
      }
    }
    free(ps.xcoords);
    free(ps.ycoords);
  }
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
    }
  }
  for (int x = 0; x < IMAGE_SIZE; x++) {
    for (int y = 0; y < IMAGE_SIZE; y++) {
      img->data[x][y] = (img->data[x][y] - mindist) / (maxdist - mindist);
    }
  }
}

int neighbor_fires(Image *img, int src_x, int src_y) {
  int count = 0;
  for (int dest_x = src_x - 1; dest_x <= src_x + 1; ++dest_x) {
    for (int dest_y = src_y - 1; dest_y <= src_y + 1; ++dest_y) {
      if (dest_y < 0) {
        dest_y = IMAGE_SIZE - 1;
      }
      if (dest_x < 0) {
        dest_x = IMAGE_SIZE - 1;
      }
      // if (dest_y < 0 || dest_y >= IMAGE_SIZE || dest_x < 0 ||
      //     dest_x >= IMAGE_SIZE) {
      //   continue;
      // }
      if (img->data[dest_x % IMAGE_SIZE][dest_y % IMAGE_SIZE] == 0.0) {
        count++;
      }
    }
  }
  return count;
}

int neighbor_forests(Image *img, int src_x, int src_y) {
  int count = 0;
  for (int dest_x = src_x - 1; dest_x <= src_x + 1; ++dest_x) {
    for (int dest_y = src_y - 1; dest_y <= src_y + 1; ++dest_y) {
      if (dest_y < 0 || dest_y >= IMAGE_SIZE || dest_x < 0 ||
          dest_x >= IMAGE_SIZE) {
        continue;
      }
      if (img->data[dest_x][dest_y] == 1.0) {
        count++;
      }
    }
  }
  return count;
}

void automata_step(Image *img) {
  const float FIRE = 0.0;
  const float FOREST = 1.0;
  const float BARREN = 2.0;
  Image prev = clone_image(img);
  for (int i = 0; i < IMAGE_SIZE; ++i) {
    for (int j = 0; j < IMAGE_SIZE; ++j) {
      float val = prev.data[i][j];
      if (val == FIRE) {
        img->data[i][j] = BARREN;
      } else if (val == FOREST) {
        int neighbors = neighbor_fires(img, i, j);
        if (neighbors > 0 || rand() % IMAGE_SIZE * 4 == 0) {
          img->data[i][j] = FIRE;
        }
      } else {
        if (rand() % 1000 == 0 && neighbor_forests(img, i, j) < 5) {
          img->data[i][j] = FOREST;
        }
      }
    }
  }
  free(prev.data);
}

float rand_float() {
  float x = (float)rand() / ((float)RAND_MAX);
  return x;
}

void celluar_automata(Image *img) {
  color_convert(img);
  const int NUM_STEPS = 32;
  for (int i = 0; i < IMAGE_SIZE; ++i) {
    for (int j = 0; j < IMAGE_SIZE; ++j) {
      if (img->data[i][j] > rand_float()) {
        img->data[i][j] = 1.0;
      } else {
        img->data[i][j] = 2.0;
      }
    }
  }
  for (int step = 0; step < NUM_STEPS; step++) {
    automata_step(img);
  }
  for (int i = 0; i < IMAGE_SIZE; ++i) {
    for (int j = 0; j < IMAGE_SIZE; ++j) {
      if (img->data[i][j] == 0) {
        img->data[i][j] = 2.0;
      }
      if (img->data[i][j] == 2.0) {
        if (neighbor_forests(img, i, j) > 5) {
          img->data[i][j] = 1.0;
        }
      }
    }
  }
}

void identity(Image *img) {}

Image generate_image(Transform *pre, int pre_count, Transform *post,
                     int post_count) {
  // Todo different random?
  Image img = rand_image();
  for (int i = 0; i < pre_count; ++i) {
    (*pre)(&img);
    pre++;
  }
  distance(&img);
  for (int i = 0; i < post_count; ++i) {
    (*post)(&img);
    post++;
  }

  color_convert(&img);
  printf("Generated Image\n");
  return img;
}

void generate_files(int do_io) {
  Transform pre[1];
  pre[0] = &identity;
  Transform all_posts[4];
  all_posts[0] = &identity;
  all_posts[1] = &folding;
  all_posts[2] = &celluar_automata;
  all_posts[3] = &matrix_transform_image;
  Transform post[3];
  post[0] = &folding;
  post[1] = &folding;
  post[2] = &celluar_automata;

  int img_id = 0;

  int bit_string = 0;
  int cellular_count = 0;
  while (bit_string < (1 << 4)) {
    for (int i = 0; i < 3; i++) {
      post[i] = &identity;
    }
    cellular_count = 0;
    int bs = bit_string;
    printf("%d\n", bit_string);
    int post_idx = 0;
    while (bs != 0) {
      int bottom = bs & 0x1;
      switch (bottom) {
      case 0:
        post[post_idx++] = &folding;
        break;
      case 1:
        post[post_idx++] = &celluar_automata;
        cellular_count += 1;
        break;
      default:
        post[post_idx++] = &identity;
        break;
      }
      if (cellular_count > 1) {
        goto END;
      }
      bs = bs >> 1;
    }
    char fname[20];
    sprintf(fname, "img/%d", img_id);

    Image img = generate_image(pre, 1, post, 3);

    if (do_io) {
      write_image(fopen(fname, "w"), &img);
    }
    img_id++;
  END:
    bit_string++;
  }
}