#ifndef IMAGE_INCLUDED
#define IMAGE_INCLUDED
#include <stdio.h>
#include <stdint.h>
#include <a2methods.h>

typedef struct Components {
    // points to the different structs that have different elem comps
    void* comps;
} *Components;

typedef struct Image {
    unsigned width, height, denominator;
    A2Methods_UArray2 pixels; //2D array of elements of type struct Components
    struct A2Methods_T *methods; // used to operate on pixels
} *Image;

Image Image_new(unsigned width, unsigned height, unsigned denominator, 
    A2Methods_T methods, int size);

/* Frees corresponding pixels NOT methods in pixmap of components */
void Image_free(Image img);

#endif
