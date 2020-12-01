#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define WRONG_FORMAT -1
#define STRUCTURE_ERROR -2
#define QDBMP_ERROR -3


struct MY_BMP_HEADER {
    unsigned short type;
    unsigned int size;
    unsigned int pixel_array_offset;
    int width;
    int height;
    unsigned short bits_per_pixel;
    unsigned int image_size;
    unsigned int horizontal_resolution;
    unsigned int vertical_resolution;
    unsigned int number_of_colors;
    unsigned int number_of_important_colors;
};