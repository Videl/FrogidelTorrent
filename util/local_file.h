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

LocalFile* new_localfile(char *path, Metadata *md);
LocalFile* file_hotload(char *path);

#endif /* LOCALFILE_H */