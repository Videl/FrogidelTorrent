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

LocalFile* file_hotload(char *path, char keywords[255])
{
    Metadata *md = create_metadata_from_path(path, keywords);
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

void addLocalFile(LocalFile toAdd, ListLocalFile* entries[])
{
    ListLocalFile *entryToAdd = NULL;
    ListLocalFile *path = NULL;
    char c = toAdd.md->md_hash[0];
    int listLookup = (int)strtol(&c, NULL, 16);

    entryToAdd = (ListLocalFile*) malloc(sizeof(ListLocalFile));
    if(!entryToAdd)
    {
        printf("Error while creating the list entry in memory\n");
        exit(EXIT_FAILURE);
    }

    entryToAdd->entry = toAdd;
    entryToAdd->next = NULL;

    if(entries[listLookup] == NULL)
    {
        entries[listLookup] = entryToAdd;
    }
    else
    {
        path = entries[listLookup];
        while(path->next != NULL)
        {
            path = path->next;
        }
        path->next = entryToAdd;
    }
}

LocalFile* searchForSha(char *sha, ListLocalFile** entries)
{
    ListLocalFile *path = NULL;
    char c = sha[0];
    int listLookup = (int) strtol(&c, NULL, 16);
    printf("AAA\n");

    if(entries == NULL)
    {
        printf("T'es nul Pierre\n");
        return NULL;
    }

    if(entries[listLookup] == NULL)
    {
        return NULL;
    }
    else
    {
        path = entries[listLookup];
        printf("%s\n", path->entry.md->md_hash);
        if(strcmp(path->entry.md->md_hash, sha) == 0)
        {
            return &(path->entry);
        }

        while(path->next != NULL)
        {
            path = path->next;
            // printf("%s\n", path->entry.md->md_hash);
            if(strcmp(path->entry.md->md_hash, sha) == 0)
            {
                return &(path->entry);
            }
        }
    }

    return NULL;
}