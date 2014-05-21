#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include "util/metadata.h"
#include "util/usermenu.h"
#include "util/constants.h"
#include "server.h"
#include "publish.h"

int main(int argc, char *argv[])
{
	// Network variable
	int listening_socket = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	int clilen = sizeof(client_addr);
	int fileCounter = 0;

	//Metadata files
	ListEntry *entries[100];

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
		if(recvfrom(listening_socket, instructions_buffer, (size_t) INSTRUCTIONS_BUFFER_SIZE,
					0, (struct sockaddr *)&client_addr, (socklen_t *) &clilen) < 0)
		{
			perror("Error while receiving on the listening socket\n");
			exit(EXIT_FAILURE);
		}

		//Publish handling
		if(strcmp(instructions_buffer, "PUBLISH") == 0)
		{
			publish(client_addr, clilen, &fileCounter);
		}

		//Search handler
		else if(strcmp(instructions_buffer, "SEARCH") == 0)
		{

		}
	}


	return EXIT_SUCCESS;
}

void server_interruption(int sig)
{
	printf("\n\nEnding Frogidel Torrent Server\n");
	exit(EXIT_SUCCESS);
}