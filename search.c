#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util/metadata.h"
#include "util/torrent.h"
#include "server.h"
#include "util/hash.h"

void search(ListEntry *entries[], struct sockaddr_in client_addr, int clilen)
{
	int search_socket = 0;
	struct sockaddr_in search_addr;
	char buffer[255] = "";
	int result_counter = 0, hash = 0;
	ListEntry *path;

	struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

	/*
	* Open the search socket
	*/
	if((search_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error while setting up the search socket\n");
		return;
	}

	if(setsockopt(search_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        // error("setsockopt failed\n");        
        // Who cares
    }

    if (setsockopt (search_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        // error("setsockopt failed\n");        
        // Who cares
    }

	/*
	* Setting up the search address
	*/
	memset(&search_addr, 0, sizeof(search_addr));
	search_addr.sin_family = AF_INET;
	search_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	search_addr.sin_port = 0;

	if(bind(search_socket, (struct sockaddr*) &search_addr, sizeof(search_addr)))
	{
		perror("Error while binding the search port on the server\n");
		close(search_socket);
		return;
	}

	/*
	* Sending the ready response with the new communicatino port
	*/
	strcpy(buffer, "SEARCH_READY");
	if(sendto(search_socket, buffer, (size_t) strlen(buffer), 0,
			  (struct sockaddr*) &client_addr, clilen) < 0)
	{
		perror("Error while sending the ready response to a search request\n");
		close(search_socket);
		return;
	}

	/*
	* Receiving the search keyword from client
	*/
	if(recvfrom(search_socket, buffer, (size_t) sizeof(buffer), 0,
				(struct sockaddr*) &client_addr, (socklen_t *) &clilen)  < 0)
	{
		perror("Error while receiving the search keyword from the client\n");
		close(search_socket);
		return;
	}

	/*
	* Do the searching
	*/
	hash = hashWord(buffer, strlen(buffer));
	if(entries[hash] != NULL)
	{
		path = entries[hash];

		do
		{
			if(strstr(path->torrent.metadata.md_keywords, buffer) != NULL)
				result_counter++;

			path = path->next;

		} while(path != NULL);
	}

	else
	{
		result_counter = 0;
	}

	/*
	* Send the number
	*/
	if(sendto(search_socket, &result_counter, (size_t) sizeof(result_counter), 0,
			  (struct sockaddr*) &client_addr, clilen) < 0)
	{
		perror("Error while sending the search result number\n");
		close(search_socket);
		return;
	}

	/*
	* Send the results one by one
	*/
	if(result_counter != 0)
	{
		path = entries[hash];

		do
		{
			if(strstr(path->torrent.metadata.md_keywords, buffer) != NULL)
			{
				if(sendto(search_socket, &path->torrent, sizeof(path->torrent), 0,
					(struct sockaddr*) &client_addr, clilen) < 0)
				{
					perror("Error while sending a result to the client\n");
					close(search_socket);
					return;
				}
			}

			path = path->next;

		} while(path != NULL);
	}

	close(search_socket);
}