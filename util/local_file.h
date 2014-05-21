#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "metadata.h"

#ifndef LOCALFILE_H
#define LOCALFILE_H

typedef struct LocalFile
{
    /**
     * File's path.
     */
    char path[500];

    /**
     * File's metadata.
     */
    Metadata *md;
    
} LocalFile;


typedef struct ListLocalFile
{
    LocalFile entry;

    struct ListLocalFile *next;

} ListLocalFile;

LocalFile* new_localfile(char *path, Metadata *md);
LocalFile* file_hotload(char *path);

void addLocalFile(LocalFile toAdd, ListLocalFile* entries[]);

#endif /* LOCALFILE_H */