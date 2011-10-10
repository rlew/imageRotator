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
  A2Methods_UArray2 array;
};

struct Pnm_rgbScaled {
  float red;
  float green;
  float blue;
};

  /* Scale to integers using large apply*/
static void applyRGBToIntScale(int col, int row, A2Methods_UArray2 array,
                                void *elem, void *cl){
  (void) array;
  struct Closure *mycl = cl;
  struct Pnm_rgbScaled *curpix = elem;
  struct Pnm_rgb *descaled;
  NEW(descaled);
  unsigned denominator = mycl->denominator;
  descaled->red = (unsigned)curpix->red * denominator;
  descaled->blue = (unsigned)curpix->blue * denominator;
  descaled->green = (unsigned)curpix->green * denominator;
  struct Pnm_rgb *temp = mycl->methods->at(array, col, row);
  *temp = *descaled;
}

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
  struct Pnm_rgbScaled* temp = mycl->methods->at(mycl->array, col, row);
  *temp = *scaled;
}

static Pnm_ppm trim(Pnm_ppm image){
    if(image->width % 2 != 0)
      image->width -= 1;

    if(image->height % 2 != 0)
      image->height -= 1;

    return image;
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
    image = trim(image);

    A2Methods_UArray2 scaledArray = methods->new(image->width, image->height, sizeof(struct Pnm_rgbScaled));
    struct Closure *mycl;
    NEW(mycl);
    mycl->methods = methods;
    mycl->denominator = image->denominator;
    mycl->array = scaledArray;
    methods->map_row_major(image->pixels, applyRGBToFloatScale, mycl);
    Pnm_ppm descaled;
    NEW(descaled);
    descaled->width = image->width;
    descaled->height = image->height;
    descaled->denominator = image->denominator;
    descaled->pixels = methods->new(descaled->width, descaled->height, sizeof(struct Pnm_rgb));
    descaled->methods = methods;
    struct Closure *cl;
    NEW(cl);
    cl->methods = methods;
    cl->denominator = descaled->denominator;
    cl->array = descaled->pixels;
 cd    methods->map_row_major(descaled->pixels, applyRGBToIntScale, cl);
    Pnm_ppmwrite(stdout, image);
    FREE(mycl);
    Pnm_ppmfree(&image);
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
