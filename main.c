#include <stdio.h>
#include <stdlib.h>
#include "util/metadata.h"
#include "util/usermenu.h"

int main(int argc, char *argv[])
{
	printf("Hello world\n");

	Metadata *dm = create_metadata_from_path("zer/main.c");

	free_metadata(dm);

    char buff[10];

    read_line(buff, 10, stdin);

    printf ("OK [%s]\n", buff);
    printf ("OK [%d]\n", atoi(buff)+1);

	return 0;
}
