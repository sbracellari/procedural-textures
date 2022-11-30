// The image.h file defines the Image struct
// while also giving other files access to 
// Image creation/modificaton functions. 

#include <stdio.h>

// Image size in pixels
#define IMAGE_SIZE 512
#define NUM_POINTS 100

// Image structure definition
typedef struct {
  float (*data)[IMAGE_SIZE];
} Image;

Image new_image();

Image rand_image();

Image clone_image(const Image *original);

void write_image(FILE *f, const Image *img);

Image load_image(FILE *f);

void swap_image(Image *img1, Image *img2);