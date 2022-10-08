#include <stdio.h>
#define IMAGE_SIZE 512

typedef struct {
  float (*data)[IMAGE_SIZE];
} Image;

Image new_image();

Image clone_image(const Image *original);

void write_image(FILE *f, const Image *img);

Image load_image(FILE *f);

/// @brief A simple transform mapping existing pixels to new destinations.
/// Appears to arrange data into roughly grid shaped patterns.
/// @param img A pointer to a heap allocated Image.
void matrix_transform_image(Image *img);