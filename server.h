#ifndef SERVER_H
#define SERVER_H

typedef struct Entry
{
	/*
	* Pair address
	*/
	struct sockaddr_in pair_address;

	/*
	* File metadata
	*/
	Metadata metadata;
} Entry;


typedef struct ListEntry
{
	/**
	* The entry in the list entry
	*/
	Entry entry;

	/*
	* A pointer on the next element
	*/
	struct ListEntry *next;
} ListEntry;

void addEntry(Entry toAdd, ListEntry *entries[]);
int hashWord(char *word, int length);
void server_interruption(int sig);

#endif