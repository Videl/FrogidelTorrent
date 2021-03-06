#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "metadata.h"

Metadata *new_metadata(char name[], char extension[], char keywords[], char hash[])
{
	Metadata *new_md = malloc(sizeof(Metadata));

	/** Copy name */
	strcpy(new_md->md_name, name);

	/** Copy extension */
	strcpy(new_md->md_extension, extension);

	/** Copy keywords */
	strcpy(new_md->md_keywords, keywords);

	/** Copy hash */
	strcpy(new_md->md_hash, hash);

	return new_md;
}

void free_metadata(Metadata *md)
{
	free(md);
}

/**
 * Create a new Metadata struct filled with every information
 * obtainable.
 *  * Filename
 *  * Extension
 *  * Some keywords (video, image, executable, etc)
 *  * The (SHA1) hash
 *
 * The file is supposed to be checked for validity too (TODO).
 *
 * Side-effect: returns a new Metadata struct.
 */
Metadata *create_metadata_from_path(char pathFile[1000], char keywords[255])
{
	char *file_name;
	char *extension;
	char hash[41];

    if (file_exists(pathFile))
    {
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

    	
    	/** Hash discovery */
    	strcpy(hash, sha1_on_file(pathFile));
    	 
    	// First way: use openssl/sha.h and do it manually
    	// Second way (easier): use shell command 'sha1sum' with popen.
    	hash[40] = '\0';
    	
    	Metadata *md = new_metadata(file_name, extension, keywords, hash);
    	return md;
    }
    else
    {
        return 0;
    }
}

/**
 * Adds a keyword to a Metadata struct.
 * 
 * Side-effect: Metadata `md` gets the keyword corresponding to
 * 				`keyword` added to its list `md->md_keywords`.
 */
void add_keyword_to_metadata(Metadata *md, char keyword[255])
{
	
}

/**
 * Remove a keyword from a Metadata struct.
 * `keyword` contains the name of the keyword to remove.
 *
 * Side-effect: Metadata `md` gets the keyword corresponding to
 * 				`keyword` removed from its list `md->md_keywords`.
 * 				If it is not found, nothing is changed.
 */
void remove_keyword_to_metadata(Metadata *md, char keyword[255])
{
	
}

/**
 * Adds a metadata struct to the list of metadata.
 */
void add_metadata_to_list()
{

}

/**
 * Remove a metadata struct to the list of metadata.
 */
void remove_metadata_from_list()
{

}


int file_exists( char * filename)
{
    FILE * file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}