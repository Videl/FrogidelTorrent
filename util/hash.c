#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/sha.h>
#include "hash.h"

int hashWord(char *word, int length)
{
	int i = 0, result = 0;

	for(i = 0 ; i < length ; i++)
	{
		result += (int)pow(word[i], length - 1 - i);
	}

	return result % 100;
}

char* sha1_on_file(char *filename)
{
	FILE *fp = NULL;
	unsigned char temp[SHA_DIGEST_LENGTH];
	char buff[32768] = {0};
	char *checksum = NULL;
	SHA_CTX context;
	size_t resultRead = 0;
	int i = 0;

	// Allocate file
	fp = fopen(filename, "rb");
	if(!fp)
	{
		perror("File not found\n");
		return NULL;
	}

	// Allocate the checksum
	checksum = (char*) malloc(sizeof(char) * 41);
	if(!checksum)
	{
		perror("Error while allocating the checksum in memory\n");
		return NULL;
	}

	// SHA1 file checksum
	SHA1_Init(&context);
	do
	{
		resultRead = fread(buff, sizeof(char), 32768, fp);
		SHA1_Update(&context, buff, resultRead);
	} while(resultRead != 0);


	fclose(fp);

	SHA1_Final(temp, &context);

	for(i = 0 ; i < SHA_DIGEST_LENGTH ; i++)
	{
		sprintf(checksum+(2*i), "%02X", 0xff & temp[i]);
	}
	checksum[40] = '\0';

	return checksum;
}
