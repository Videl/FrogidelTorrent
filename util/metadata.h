#include <stdio.h>
#include <stdlib.h>

#ifndef METADATA_H
#define METADATA_H

typedef struct Metadata
{
    /**
    * File's name.
    */
    char md_name[255];

    /**
    * File's extension.
    */
    char md_extension[10];

    /**
    * List of keywords to describe the file.
    */
    char md_keywords[255];

    /**
    * Hash of the content. We use SHA-1 which generates a
    * 160-bit hash value rendered as 40 digits long in
    * hexadecimal.
    */
    char md_hash[41];
} Metadata;

Metadata *new_metadata(char name[], char extension[], char keywords[], char hash[]);
void free_metadata(Metadata *md);
Metadata *create_metadata_from_path(char pathFile[1000]);

#endif /* METADATA_H */