#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "local_file.h"

LocalFile* new_localfile(char *path, Metadata *md)
{
    LocalFile *lf = malloc(sizeof(LocalFile));

    /** Copy path */
    strcpy(lf->path, path);

    lf->md = md;

    return lf;
}

LocalFile* file_hotload(char *path)
{
    Metadata *md = create_metadata_from_path(path);
    if (md)
    {
        LocalFile *lf = new_localfile(path, md);
        return lf;
    }
    else
    {
        return 0;
    }
}