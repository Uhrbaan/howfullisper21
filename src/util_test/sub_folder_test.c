#include <stdbool.h>
#include <stdio.h>

void useful_function(bool is_useful) {
    printf("You called a use%s function.\n", (is_useful) ? "ful" : "less");
}