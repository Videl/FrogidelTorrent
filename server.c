#include <stdio.h>
#include <stdlib.h>
#include <sting.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "util/metadata.h"
#include "util/usermenu.h"
#include "util/constants.h"

int main(int argc, char *argv[])
{
	int listening_socket = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	int clilen = sizeof(client_addr);


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

	if(bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
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

	}


	return EXIT_SUCCESS;
}