#include <stdio.h>
#include <stdlib.h>
#include "util/metadata.h"

int main(int argc, char *argv[])
{
	printf("Hello world\n");
	char *chr = "Test test test!";

	Metadata* test = new_metadata(chr);

	printf("Name: %s\n", test->md_name);
	free_metadata(test);
}
