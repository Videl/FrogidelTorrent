#include <stdio.h>
#include <stdlib.h>
#include "util/metadata.h"

int main(int argc, char *argv[])
{
	printf("Hello world\n");

	Metadata *dm = create_metadata_from_path("zer/main.c");

	free_metadata(dm);

	return 0;
}
