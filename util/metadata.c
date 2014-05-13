#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metadata.h"

Metadata *new_metadata(char name[], char extension[], char keywords[], char hash[])
{
	Metadata *new_md = malloc(sizeof(Metadata));

	/** Copy name */
	strcpy(new_md->md_name, name);

	/** Copy extension */
	strcpy(new_md->md_extension, extension);

	/** Copy keywords */

	/** Copy hash */
	strcpy(new_md->md_hash, hash);

	return new_md;
}

void free_metadata(Metadata *md)
{
	free(md);
}

Metadata *create_metadata_from_path(char pathFile[1000])
{
	char *file_name;
	char *extension;
	char hash[41];

	/** Name discovery */
	char *slash = strrchr(pathFile, '/');
    if (!slash || slash == pathFile) 
	{
		file_name = pathFile;
	}
    else 
	{
		file_name = slash+1;
	}

	/** Extension discovery */
	char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name)
	{
		extension = "";
	}
    else 
	{
		extension = dot+1;
	}

	/** Keywords discovery */
	// TODO
	
	/** Hash discovery */
	// TODO
	// 
	// First way: use openssl/sha.h and do it manually
	// Second way (easier): use shell command 'sha1sum' with popen.
	strcpy(hash, "9bdbaa76cde709f2d0ad1089feb098ce1263208d");
	hash[40] = '\0';
	
	Metadata *md = new_metadata(file_name, extension, "", hash);
	return md;
}