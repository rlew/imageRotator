#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "a2methods.h"
#include "uarray2.h"
#include "pnm.h"
#include "mem.h"
#include "math.h"

struct RGB {
    int R;
    int G;
    int B;
};

void applySum(int row, int col, void* elem, void*cl) {
    struct RGB* sum = (struct RGB*)cl;
    struct Pnm_rgb* pixel = (struct Pnm_rgb*)elem;
    sum->R += pixel->red;
    sum->G += pixel->green;
    sum->B += pixel->blue;
}

int main(int argc, char *argv[]) {
    Pnm_ppm image1;
    Pnm_ppm image2;
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);
    assert(argc == 3);
    FILE* fp = fopen(argv[1], "r");
    assert(fp);
    TRY
        image1 = Pnm_ppmread(fp, methods);
    EXCEPT(Pnm_Badformat)
        fprintf(stderr, "Badly formatted file.\n");
        exit(1);
    END_TRY;
    if(fp != stdin) {
        fflush(fp);
    }
    fp = fopen(argv[2], "r");
    assert(fp);
    TRY
        image2 = Pnm_ppmread(fp, methods);
    EXCEPT(Pnm_Badformat)
        fprintf(stderr, "Badly formatted file.\n");
        exit(1);
    END_TRY;
    if(fp != stdin) {
        fclose(fp);
    }
    
    struct RGB image1cl = { 0, 0, 0 };
    struct RGB image2cl = { 0, 0, 0 };
    int width, height;

    if(abs(image1->height - image2->height) <= 1) {
        if (image1->height > image2->height) {
            height = image2->height;
        }
        else {
            height = image1->height;
        }
    }
    else {
        fprintf(stderr, "Height doesn't match.\n");
        exit(1);
    }
    if(abs(image1->width - image2->width) > 1) {
        if (image1->width > image2->width) {
            height = image2->width;
        }
        else {
            height = image1->width;
        }
    }
    else {
        fprintf(stderr, "Height doesn't match.\n");
        exit(1);
    }

    for(int r = 0; r < image1->height; r++) {
        for(int c = 0; c < image1->width; c++) {
            // summing each rgb
        }
    }
    //divide
    //sqrt
    //return
            
}
