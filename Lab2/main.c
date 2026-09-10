#include <stdio.h>
#include "array.h"


int main(void) {
    Array a = array_new(3);  // liten kapasitet med vilje

    for (long i = 0; i < 10; i++) {
        array_insertBack(&a, i);
        printf("Inserted %ld, ", i);
        array_print(a);
    }

    array_destroy(a);
    return 0;
}