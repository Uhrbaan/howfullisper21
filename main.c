#include <stdio.h>
#include <stdbool.h>

#include "test_adder.h"
#include "util_test/sub_folder_test.h"

int main(int argc, const char *argv[]) {
	printf("Hello world!\n");
	printf("1+2 = %d\n", add(1, 2));
	useful_function(false);
	return 0;
}