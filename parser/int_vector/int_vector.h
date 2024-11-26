#ifndef INT_VECTOR_H
#define INT_VECTOR_H

#include <stdlib.h>
typedef struct {
    size_t size;
    int* buff;
} IntVector ;

void IntVector_push(IntVector* this, int val);

void IntVector_destroy(IntVector* this);

int* IntVector_get(IntVector* this, size_t i);

IntVector IntVector_create();

#endif