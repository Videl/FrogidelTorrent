#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util/constants.h"
#include "util/metadata.h"
#include "server.h"
#include "publish.h"

Entry publish(struct sockaddr_in client_addr, int clilen)
{
	int publish_socket = 0;
	struct sockaddr_in publish_addr;
	char buffer[100] = "";
	Metadata metadataFile;
	Entry result;

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
	publish_addr.sin_port = 0;

	if(bind(publish_socket, (struct sockaddr*) &publish_addr, sizeof(publish_addr)) < 0)
	{
		perror("Error while binding the publish port on the server\n");
		exit(EXIT_FAILURE);
	}

	/*
	* Sending the ready response whith the new communication port
	*/
	strcpy(buffer, "PUBLISH_READY");
	if(sendto(publish_socket, buffer, (size_t) strlen(buffer), 0,
			  (struct sockaddr*) &client_addr, clilen) < 0)
	{
		perror("Error while sending the ready response to a publish request\n");
		exit(EXIT_FAILURE);
	}

	/*
	* Receiving metadata from client	
	*/
	if(recvfrom(publish_socket, &metadataFile, (size_t) sizeof(metadataFile), 0,
				(struct sockaddr *) &client_addr, (socklen_t *) &clilen) < 0)
	{
		perror("Error while receiving the metadata file\n");
		exit(EXIT_FAILURE);
	}

	/*
	* Sending the publish ack
	*/
	strcpy(buffer, "PUBLISH_ACK");
	if(sendto(publish_socket, buffer, (size_t) strlen(buffer), 0, 
			  (struct sockaddr*) &client_addr, clilen) < 0)
	{
		perror("Error while sending the publish ack\n");
		exit(EXIT_FAILURE);
	}

	/*
	* Close the socket and the function and return the metadata
	*/
	close(publish_socket);

	result.pair_address = client_addr;
	result.metadata = metadataFile;

	return result;
}