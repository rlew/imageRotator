#include <assert.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <mem.h>
#include "image.h"
/*
Image Image_new(unsigned width, unsigned height, unsigned denominator, 
    A2Methods_T methods, int size) {
    assert(width > 0 && height > 0 && denominator > 0 && size > 0);
    assert(methods);
    struct Image img;
    img.width = width;
    img.height = height;
    img.denominator = denominator;
    img.methods = &methods;
    img.pixels = img.methods->new(width, height, size);
    return &img;
}

void Image_free(Image img) {
    img->methods->free(img->pixels);
}*/
