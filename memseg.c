#include <stdlib.h>
#include "memseg.h"
#include <stdio.h>
#include <mem.h>

/*
 * Increase the available set of IDs in unmappedIDs and sets the corresponding
 * IDs in mappedIDs to NULL
 */
static void resizeMem(Mem* memorySegments) {
    for(UM_Word i = (UM_Word)Seq_length(memorySegments->mappedIDs); 
        i < (UM_Word)Seq_length(memorySegments->mappedIDs) * 2; i++) {
        UM_Word* value;
        NEW(value);
        *value = i;
        Seq_addhi(memorySegments->unmappedIDs, value);
        Seq_put(memorySegments->mappedIDs, i, NULL);
    }

}

/*
 * Allocates memory for the mapped and unmapped IDs in memory that can be used
 * to created mapped memory segments
 */
void instantiateMem(Mem* mem, int length) {
    mem->mappedIDs = Seq_new(length);
    mem->unmappedIDs = Seq_new(length);
    
    for(UM_Word i = 0; i < (UM_Word)length; i++) {
        UM_Word* value;
        NEW(value);
        *value = i;
        Seq_addhi(mem->mappedIDs, NULL);
        Seq_addhi(mem->unmappedIDs, value);
    }
}

/*
 * Maps a segment in memory by marking an ID as in use and allocating a
 * segment of memory of the specified length. Returns the index of the mapped
 * segment
 */
UArray_T mapSegment(Mem* memorySegments, UM_Word ID, int length) {
    if(Seq_length(memorySegments->unmappedIDs) == 0){
        resizeMem(memorySegments);
    }

    UArray_T segment = UArray_new(length, sizeof(UM_Word));
    
    // Initializing each UM_Word in the memory segment to 0
    for(UM_Word i = 0; i < (UM_Word)length; i++) {
        UM_Word* elem = UArray_at(segment, i);
        *elem = 0;
    }

    UM_Word* availableID = (UM_Word*)Seq_put(memorySegments->unmappedIDs, ID,
                                             NULL);
    UM_Word index = *availableID;
    FREE(availableID);
    Seq_put(memorySegments->mappedIDs, index, segment);

    return segment;
}

/*
 * Returns an ID to the pool of available IDs and frees all associated memory
 * with the given ID
 */
void unmapSegment(Mem* memorySegments, UM_Word index) {
    UArray_T segmentID = Seq_get(memorySegments->mappedIDs, index);
    UArray_free(&segmentID);
    Seq_put(memorySegments->mappedIDs, index, NULL);
    Seq_put(memorySegments->unmappedIDs, index, &index);
}

/*
 * Returns the memory segment stored at the specified ID and offset
 */
UM_Word segmentedLoad(Mem* memorySegments, UM_Word ID, int offset){
  return *(UM_Word*)UArray_at((UArray_T)Seq_get(memorySegments->mappedIDs,
           ID), offset);
}

/*
 * Stores the value passed at the specified index and offset in the memory
 * segments
 */
void segmentedStore(Mem* memorySegments, UM_Word ID, int offset, UM_Word
                       value){
    UM_Word* word = UArray_at((UArray_T)Seq_get(memorySegments->mappedIDs, ID),
                              offset);
    *word = value;
}

UArray_T getIndex(Mem* memorySegments, UM_Word ID){
    return (UArray_T)Seq_get(memorySegments->mappedIDs, ID);
}

UArray_T segmentCopy(Mem* memorySegments, UM_Word ID){
    UArray_T segment = Seq_get(memorySegments->mappedIDs, ID);
    UArray_T copy = UArray_new(UArray_length(segment), sizeof(UM_Word));
    for(int i = 0; i < UArray_length(segment); i++){
        UM_Word* value = UArray_at(copy, i);
        *value = *(UM_Word*)UArray_at(segment, i);
    }
    return copy;
}

void freeMem(Mem* memorySegments) {
    while(Seq_length(memorySegments->mappedIDs) != 0) {
        UArray_T seg = Seq_remlo(memorySegments->mappedIDs);
        if(seg != NULL) {
            UArray_free(&seg);
        }
    }
    while(Seq_length(memorySegments->unmappedIDs) != 0) {
        UM_Word* ID = Seq_remlo(memorySegments->unmappedIDs);
        FREE(ID);
    }
    Seq_free(&memorySegments->mappedIDs);
    Seq_free(&memorySegments->unmappedIDs);
    FREE(memorySegments);
}
