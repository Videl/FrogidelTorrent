#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <math.h>
#include "util/metadata.h"
#include "util/usermenu.h"
#include "util/constants.h"
#include "util/torrent.h"
#include "util/hash.h"
#include "server.h"
#include "publish.h"
#include "search.h"

int main(int argc, char *argv[])
{
	// Network variable
	int listening_socket = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	int clilen = sizeof(client_addr), n = 0;
	Torrent toPublish;

	// Var
	int i = 0;

	// Metadata files
	ListEntry *entries[100];

	for(i = 0 ; i < 100 ; i++)
	{
		entries[i] = NULL;
	}

	// System variable
	struct sigaction new, old;

	// Buffers variable
	char instructions_buffer[INSTRUCTIONS_BUFFER_SIZE];

	/*
	* Signal interruption action
	*/
	memset(&new, 0, sizeof(new));
	new.sa_handler = server_interruption;
	sigaction(SIGINT, &new, &old);


	/**
	* First open the listening socket
	*/
	if((listening_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error while setting up the listening socket\n");
		exit(EXIT_FAILURE);
	}

	/**
	* Binding the address and port
	*/
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT_ON_SERV);

	if(bind(listening_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Error while binding the server port\n");
		exit(EXIT_FAILURE);	
	}

	printf("Welcome, you've launched the Frogidel Torrent server.\n");
	printf("Server is running.\n");

	/*
	* Treatment loop
	*/
	while(1)
	{
		if((n = recvfrom(listening_socket, instructions_buffer, (size_t) INSTRUCTIONS_BUFFER_SIZE,
					0, (struct sockaddr *)&client_addr, (socklen_t *) &clilen)) < 0)
		{
			perror("Error while receiving on the listening socket\n");
			exit(EXIT_FAILURE);
		}

		instructions_buffer[n] = '\0';

		//Publish handling
		if(strcmp(instructions_buffer, "PUBLISH") == 0)
		{
			toPublish = publish(client_addr, clilen);
			if(strcmp(toPublish.metadata.md_name, "ERROR") == 0)
				printf("Fail to PUBLISH data\n");

			else
			{
				addEntry(toPublish, entries);
				printf("File received\n");
			}
		}

		//Search handler
		else if(strcmp(instructions_buffer, "SEARCH") == 0)
		{
			search(entries, client_addr, clilen);
		}
	}

	return EXIT_SUCCESS;
}

void addEntry(Torrent toAdd, ListEntry* entries[])
{
	int i = 0, j = 0, hash = 0;
	char *temp = NULL;
	ListEntry *entryToAdd = NULL;
	ListEntry *path = NULL;

	entryToAdd = (ListEntry*) malloc(sizeof(ListEntry));
	if(!entryToAdd)
	{
		printf("Error while creating the list entry in memory\n");
		exit(EXIT_FAILURE);
	}

	entryToAdd->torrent = toAdd;
	entryToAdd->next = NULL;


	temp = (char*) malloc(sizeof(char) * (strlen(toAdd.metadata.md_keywords) + 1));
	if(!temp)
	{
		printf("Error while creating the list entry in memory\n");
		exit(EXIT_FAILURE);
	}

	//Add first keyword
	i = 0;
	j = 0;
	while(toAdd.metadata.md_keywords[i] != '\0')
	{
		// Determine the string
		while(toAdd.metadata.md_keywords[i] != ',' && toAdd.metadata.md_keywords[i] != '\0')
		{
			temp[j] = toAdd.metadata.md_keywords[i];
			i++;
			j++;
			printf("%d\t%d\n", i, j);
		}

		// Hash it and add the entry
		temp[j] = '\0';
		hash = hashWord(temp, j);

		printf("Added\n");
		printf("%d\n", hash);

		if(entries[hash] == NULL)
			entries[hash] = entryToAdd;

		else
		{
			j = 0;
			path = entries[hash];
			while(path->next != NULL)
				path = path->next;

			path->next = entryToAdd;
		}

		if(toAdd.metadata.md_keywords[i] != '\0')
		{
			i++;
			j = 0;
		}
	}

}


void server_save_torrents(ListEntry *entries[])
{
	FILE *fp = NULL;


	fp = fopen("server_torrents", "w");
	if(!fp)
	{
		perror("Error while saving the server torrents\n");
		exit(EXIT_FAILURE);
	}



	fclose(fp);
}

void server_load_torrents(ListEntry *entries[])
{
	FILE *fp = NULL;

	fp = fopen("server_torrents", "r");
	if(!fp)
	{
		perror("Error while loading the server torrents\n");
		exit(EXIT_FAILURE);
	}
}

void server_interruption(int sig)
{
	printf("\n\nSaving server torrents\n\n");

	printf("\n\nEnding Frogidel Torrent Server\n\n");
	exit(EXIT_SUCCESS);
}