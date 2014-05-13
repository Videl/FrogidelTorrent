#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metadata.h"

Metadata *new_metadata(char name[])
{
	Metadata *new_md = malloc(sizeof(Metadata));
	strcpy(new_md->md_name, name);

	return new_md;
}


void free_metadata(Metadata *md)
{
	free(md);
}