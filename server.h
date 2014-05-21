#ifndef SERVER_H
#define SERVER_H

typedef struct Entry
{
	/*
	* Identifier of the project
	*/
	int id;

	/*
	* Pair address
	*/
	char pair_address[100];

	/*
	* File metadata
	*/
	Metadata medata;
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


void server_interruption(int sig);

#endif