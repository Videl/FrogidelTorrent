#include <stdlib.h>

int hashWord(char *word, int length)
{
	int i = 0, result = 0;

	for(i = 0 ; i < length ; i++)
	{
		result += (int)pow(word[i], length - 1 - i);
	}

	return result % 100;
}