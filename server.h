#ifndef SERVER_H
#define SERVER_H

typedef struct ListEntry
{
	/**
	* The entry in the list entry
	*/
	Torrent torrent;

	/*
	* A pointer on the next element
	*/
	struct ListEntry *next;
} ListEntry;

void addEntry(Torrent toAdd, ListEntry *entries[]);
int hashWord(char *word, int length);
void server_interruption(int sig);

#endif