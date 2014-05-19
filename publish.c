#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util/constants.h"
#include "publish.h"

void publish(struct sockaddr_in client_addr)
{
	int publish_socket = 0;
	struct sockaddr_in publish_addr;

	/**
	* Open the dialog socket
	*/
	if((publish_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error while setting up the publishing socket\n");
		exit(EXIT_FAILURE);
	}

	/*
	* Setting up the dialog address
	*/
	memset(&publish_addr, 0, sizeof(publish_addr));
	publish_addr.sin_family = AF_INET;
	publish_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	publish_addr.sin_port = htons(PUBLISH_PORT_ON_SERV);

	if(bind(publish_socket, (struct sockaddr*) &publish_addr, sizeof(publish_addr)) < 0)
	{
		perror("Error while binding the publish port on the server\n");
		exit(EXIT_FAILURE);
	}

	/*
	* 
	*/	

	/*
	* Close the socket and end the fonction
	*/
	close(publish_socket);
}