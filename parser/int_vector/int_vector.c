#include "int_vector.h"
#include <stdio.h>

void IntVector_push(IntVector* this, int val){
    this->buff = realloc(this->buff, ++this->size * sizeof(int));
    this->buff[this->size - 1] = val;
}

void IntVector_destroy(IntVector* this){
    free(this->buff);
}

IntVector IntVector_create(){
    return (IntVector){.size = 0, .buff = NULL};
}