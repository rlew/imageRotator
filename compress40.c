#include <stdio.h>
#include "pnm.h"
#include "uarray2.h"
#include "a2plain.h"
#include "a2methods.h"
#include "assert.h"
#include "except.h"
#include <stdlib.h>
#include "mem.h"
#include "compress40.h"

struct Closure {
  A2Methods_T methods;
  unsigned denominator;
};

struct Pnm_rgbScaled {
  float red;
  float green;
  float blue;
};

  /* Scale to integers using large apply*/
static void applyRGBToIntScale(int col, int row, A2Methods_UArray2 array,
                                void *elem, void *cl){
  struct Closure *mycl = (struct Closure*)cl;
  struct Pnm_rgbScaled *curpix = (struct Pnm_rgbScaled*)elem;
  struct Pnm_rgb *descaled;
  unsigned denominator = mycl->denominator;
  descaled->red = (unsigned)(curpix->red * denominator);
  descaled->blue = (unsigned)(curpix->blue * denominator);
  descaled->green = (unsigned)(curpix->green * denominator);
  FREE(curpix);
  struct Pnm_rgb *temp = mycl->methods->at(array, col, row);
  temp = descaled;
}
  /* Scale to floats using large apply*/
static void applyRGBToFloatScale(int col, int row, A2Methods_UArray2 array,
                                void *elem, void *cl){
  struct Closure *mycl = (struct Closure*)cl;
  struct Pnm_rgb *curpix = (struct Pnm_rgb*)elem;
  struct Pnm_rgbScaled *scaled;
  unsigned denominator = mycl->denominator;
  scaled->red = (float)curpix->red / (float)denominator;
  scaled->blue = (float)curpix->blue / (float)denominator;
  scaled->green = (float)curpix->green / (float)denominator;
  FREE(curpix);
  struct Pnm_rgbScaled *temp = mycl->methods->at(array, col, row);
  temp = scaled;
}

/*
static void applyRGBScale(void *elem, void *cl){
  struct Pnm_rgb *curpix = (struct Pnm_rgb*)elem;
  unsigned denominator = *(unsigned*)cl;
  curpix->red /= denominator;
  curpix->blue /= denominator;
  curpix->green /= denominator;
}*/

void compress40(FILE *input){
    Pnm_ppm image;
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);
    TRY
        image = Pnm_ppmread(input, methods);
    EXCEPT(Pnm_Badformat)
        fprintf(stderr, "Badly formatted file.\n");
        exit(1);
    END_TRY;

    if(!(image->width % 2)){
      image->width -= 1;

    }
    if(!(image->height % 2)){
      image->height -= 1;
    }
    struct Closure mycl = {methods, image->denominator};
    methods->map_row_major(image->pixels, applyRGBToFloatScale, &mycl);
    methods->map_row_major(image->pixels, applyRGBToIntScale, &mycl);
    Pnm_ppmwrite(stdout, image);
}


void decompress40(FILE *input) {
    (void) input;
    printf("FUCKKKKKKKKKK\n");
}
