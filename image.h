#include <stdio.h>

#define IMAGE_SIZE 512
#define NUM_POINTS 100

// Image structure
typedef struct {
  float (*data)[IMAGE_SIZE];
  int (*xcoords), (*ycoords);
} Image;

Image new_image();

Image clone_image(const Image *original);

void write_image(FILE *f, FILE *xcoord, FILE *ycoord, const Image *img);

Image load_image(FILE *f, FILE *xcoord, FILE *ycoord);

void write_graph_file();