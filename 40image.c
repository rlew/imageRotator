#include "string.h"
#include "compress40.h"
#include <stdio.h>
#include "pnm.h"
#include "uarray2.h"
#include "a2plain.h"
#include "a2methods.h"
#include "assert.h"
#include "except.h"
#include <stdlib.h>
#include "mem.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

struct Closure {
  A2Methods_T methods;
  unsigned denominator;
  A2Methods_UArray2 scaledArray;
};

struct Pnm_rgbScaled {
  float red;
  float green;
  float blue;
};

  /* Scale to integers using large apply*/
/*static void applyRGBToIntScale(int col, int row, A2Methods_UArray2 array,
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
}*/
  /* Scale to floats using large apply*/
static void applyRGBToFloatScale(int col, int row, A2Methods_UArray2 array,
                                A2Methods_Object *elem, void *cl){
  (void) array;
  struct Closure *mycl = cl;
  struct Pnm_rgb *curpix = elem;
  struct Pnm_rgbScaled *scaled;
  NEW(scaled);
  unsigned denominator = mycl->denominator;
  scaled->red = (float)(curpix->red) / (float)denominator;
  scaled->blue = (float)(curpix->blue) / (float)denominator;
  scaled->green = (float)(curpix->green) / (float)denominator;
  struct Pnm_rgbScaled* temp = mycl->methods->at(mycl->scaledArray, col, row);
  *temp = *scaled;
}

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
    A2Methods_UArray2 scaledArray = methods->new(image->width, image->height, sizeof(struct Pnm_rgbScaled));
    struct Closure mycl = {methods, image->denominator, scaledArray};
    methods->map_row_major(image->pixels, applyRGBToFloatScale, &mycl);
    methods->free(scaledArray);
    Pnm_ppmfree(&image);
    //methods->map_row_major(image->pixels, applyRGBToIntScale, &mycl);
    //Pnm_ppmwrite(stdout, image);
}


void decompress40(FILE *input) {
    (void) input;
    printf("FUCKKKKKKKKKK\n");
}

int main(int argc, char *argv[]) {
  int i;
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-c")) {
      compress_or_decompress = compress40;
    } else if (!strcmp(argv[i], "-d")) {
      compress_or_decompress = decompress40;
    } else if (*argv[i] == '-') {
      fprintf(stderr, "%s: unknown option '%s'\n", argv[0], argv[i]);
      exit(1);
    } else if (argc - i > 2) {
      fprintf(stderr, "Usage: %s -d [filename]\n"
                      "       %s -c [filename]\n", argv[0], argv[0]);
      exit(1);
    } else {
      break;
    }
  }
  assert(argc - i <= 1); // at most one file on command line
  if (i < argc) {
    FILE* fp = fopen(argv[i], "r");
    assert(fp);
    compress_or_decompress(fp);
    fclose(fp);
  } else {
    compress_or_decompress(stdin);
  }
}
