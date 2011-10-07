#include "uarray.h"
#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"

#define T UArray2_T

struct T{
    int width;
    int height;
    UArray_T array;
};

int UArray2_height(T twoDArray) {
    return twoDArray->height;
}

int UArray2_width(T twoDArray) {
    return twoDArray->width;
}

int UArray2_size(T twoDArray) {
    return UArray_size(twoDArray->array);
}

T UArray2_new(int height, int width, int size) {
    T twoDArray = malloc(sizeof(*twoDArray));
    int length = height * width;
    twoDArray->array = UArray_new(length, size);
    twoDArray->height = height;
    twoDArray->width = width;
    return twoDArray;
}

void UArray2_free(T twoDArray) {
    UArray_free(&twoDArray->array);
    free(twoDArray);
}

void* UArray2_at(T twoDArray, int row, int col) {
    int loc = row * UArray2_width(twoDArray) + col;
    return UArray_at(twoDArray->array, loc); 
}

void UArray2_map_row_major(T twoDArray, void apply(int row, int col, void* elem, 
    void* cl), void* cl){
    for(int r=0; r < UArray2_height(twoDArray); r++) {
        for(int c=0; c < UArray2_width(twoDArray); c++) {
            apply(r, c, UArray2_at(twoDArray, r, c), cl);
        }
    }
}

void UArray2_map_col_major(T twoDArray, void apply(int row, int col, void* elem, 
    void* cl), void* cl) {
    for(int c=0; c < UArray2_width(twoDArray); c++){
        for(int r=0; r < UArray2_height(twoDArray); r++){
            apply(r, c, UArray2_at(twoDArray, r, c), cl);
        }
    }
}

#undef T
