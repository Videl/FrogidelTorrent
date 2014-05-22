#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <time.h>
#include <sys/timeb.h>
#include <netdb.h>
#include <strings.h>
#include <pthread.h>
#include "sharer.h"
#include "util/metadata.h"
#include "util/usermenu.h"
#include "util/constants.h"
#include "util/local_file.h"
#include "util/torrent.h"

int main(int argc, char *argv[])
{
	char user_addr_server[30];
	int stop = 1;
    int i = 0;
    pthread_t thread_server;
    int iret;

    iret = pthread_create( &thread_server, NULL, low_energy_server_run, (void*) NULL);
    if(iret)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n", iret);
        exit(EXIT_FAILURE);
    }


    /** Network files UDP (Central server) */
    int serverSocket;
    struct sockaddr_in serv_addr;

    /** Hashmap of local files */
    ListLocalFile *local_files_list[16];

    for (i = 0; i < 16; i++)
    {
        local_files_list[i] = NULL;
    }

	printf("Welcome! You have launched a pair!\n");
	printf("Please write the central server's address ip: ");

    read_line(user_addr_server, sizeof(user_addr_server), stdin);

	printf("Central server's address ip: %s\n", user_addr_server);
    setup_publish_server(&serv_addr, &serverSocket, user_addr_server);

	
	while(stop)
	{
		printf("\n\n");
		printf("What would you like to do?\n");

		char *usermenu1[4] = {"Change the central server's address ip.", "Publish a new file.", "Search for a file", "See the file you published"};
		int choice = print_user_menu(usermenu1, 4);

		switch(choice)
		{
			case 0:
				printf("New central server's address: ");
                read_line(user_addr_server, sizeof(user_addr_server), stdin);

                printf("Central server's address ip: %s\n", user_addr_server);
                setup_publish_server(&serv_addr, &serverSocket, user_addr_server);

				break;
			case 1:
				printf("Publish a new file.\n");
                publish_new_file(&serverSocket, serv_addr, local_files_list);

				break;
			case 2:
				printf("Search for a file.\n");
                search_for_a_file(&serverSocket, serv_addr);

				break;
            case 3:
                printf("List of published files:\n");
                int i;
                for (i = 0; i < 16; i++)
                {
                    ListLocalFile *mlf = local_files_list[i];
                    if (mlf != NULL)
                    {
                        while(mlf->next != NULL)
                        {
                            printf(ANSI_COLOR_CYAN ANTISPACER"%s\n"ANSI_COLOR_RESET, mlf->entry.path);
                            mlf = mlf->next;
                        }
                        printf(ANSI_COLOR_CYAN ANTISPACER"%s\n"ANSI_COLOR_RESET, mlf->entry.path);
                    }
                }

                break;
			case 666:
				printf("Bye!\n");
				stop = 0;
				break;
			case 654:
				printf("Canceled.\n");
				break;
			default:
				printf("Sorry, we couldn't manage your choice.\n");
				break;
		}

        fflush(stdin);
	}

    printf("Exiting, waiting for TCP server to end.\n");
    pthread_join(thread_server, NULL);

	return EXIT_SUCCESS;
}

void publish_new_file(
    int *serverSocket, 
    struct sockaddr_in serv_addr, 
    ListLocalFile **listOfLocalFiles)
{
    int stop = 1;
    while(stop)
    {
        printf("\n\n");
        printf("Please insert the file's absolute path:\n");
        char file[500];
        char keywords[255];
        int n;
        socklen_t len = sizeof(serv_addr);
        char sendbuf[1500];

        printf(ANSI_COLOR_BLUE SPACER ANSI_COLOR_RESET);
        read_line(file, sizeof(file), stdin);

        LocalFile *fl = file_hotload(file);

        if (fl)
        {
            // <<< PUBLISH THE FILE TO SERVER >>>
            
            n = sendto(
                *serverSocket, 
                "PUBLISH", 
                strlen("PUBLISH"), 
                0, 
                (struct sockaddr *) &serv_addr, 
                sizeof(serv_addr)
            );

            if (n != strlen("PUBLISH"))
            {
                perror("No server accross the network! Can't start publishing!\n");
                //exit (1);
            }
            else
            {
                // We need the new port
                struct sockaddr_in publish_server;

                n = recvfrom(*serverSocket, sendbuf, sizeof(sendbuf)-1,0, 
                    (struct sockaddr *) &publish_server, &len);

                if (n < 0)
                {
                    perror("Error when following the publishing protocol.\n");
                    //exit(1);
                }
                else
                {
                    // printf("Received %s !\n.", sendbuf);
                    if (strcmp(sendbuf, "PUBLISH_READY") == 0)
                    {
                        n = sendto(
                            *serverSocket, 
                            (void *)(fl->md), 
                            (size_t) sizeof(*(fl->md)), 
                            0, 
                            (struct sockaddr *) &publish_server, 
                            sizeof(publish_server)
                        );
                        // TODO : test if error.

                        n = recvfrom(
                            *serverSocket, 
                            sendbuf, 
                            sizeof(sendbuf)-1,
                            0, 
                            (struct sockaddr *) &publish_server, 
                            &len
                        );

                        sendbuf[n] = '\0';
                        
                        if (strcmp(sendbuf, "PUBLISH_ACK") == 0)
                        {
                            printf(ANSI_COLOR_CYAN ANTISPACER "Publishing of file is complete.\n"ANSI_COLOR_RESET);
                            addLocalFile(*fl, listOfLocalFiles);
                        }
                        else
                        {
                            printf("Publishing of file FAILED.\n");
                            printf("Received: %s, %d\n", sendbuf, (int) strlen(sendbuf));
                        }
                    }
                }
            }
        }
        else
        {
            printf("Your file `%s` has not been found.\n", file);
        }

        char *usermenu_continue[1] = {"Continue"};
        int choice = print_user_menu(usermenu_continue, 1);
        switch(choice)
        {
            case 654:
                stop = 0;
                break;
            case 666:
                exit(0);
                break;

            default:
                // do nothing, let's continue
                break;
        }
    }
}

void search_for_a_file(
    int *serverSocket, 
    struct sockaddr_in serv_addr)
{
    int stop = 1;
    char recv_buffer[1500];

    while(stop)
    {
        printf("\n");
        printf("Please insert one keyword to search for: \n");
        char keyword[500];
        int i = 0;
        int n;
        socklen_t len = sizeof(serv_addr);

        printf(ANSI_COLOR_BLUE SPACER ANSI_COLOR_RESET);
        read_line(keyword, sizeof(keyword), stdin);


        // <<< BEGIN SEARCH >>>
        
        // Begin search
        n = sendto(
            *serverSocket, 
            "SEARCH", 
            strlen("SEARCH"), 
            0,
            (struct sockaddr *) &serv_addr, 
            sizeof(serv_addr)
        );

        if (n != strlen("SEARCH"))
        {
            perror("No server accross the network! Can't start searching!\n");
            //exit (1);
        }
        else
        {
            // Receive the port
            struct sockaddr_in search_server;
            n = recvfrom(
                *serverSocket, 
                recv_buffer, 
                sizeof(recv_buffer)-1,
                0,
                (struct sockaddr *) &search_server, 
                &len
            );

            if (n < 0 || strcmp(recv_buffer, "SEARCH_READY") != 0)
            {
                perror("Error when following the search protocol: ");
            }
            else
            {
                // Use the port to send the keyword
                n = sendto(
                    *serverSocket, 
                    (void *) keyword, 
                    (size_t) strlen(keyword), 
                    0, 
                    (struct sockaddr *) &search_server, 
                    sizeof(search_server)
                );

                if (n != strlen(keyword))
                {
                    perror("No server accross the network! Can't start searching!\n");
                    //exit (1);
                }
                else
                {
                    // Receive the number of results
                    char noData[20];
                    int loops = 0;

                    n = recvfrom(
                        *serverSocket, 
                        noData, 
                        sizeof(noData),
                        0,
                        (struct sockaddr *) &search_server, 
                        &len
                    );
                    loops = atoi(noData);

                    if (n < 0 || loops <= 0)
                    {
                        printf("No search results.\n");
                    }
                    else
                    {
                        // Prepare a loop with as much loops as needed
                        Torrent *result = 
                            malloc(loops*sizeof(Torrent));
                        if(result == NULL)
                        {
                            printf("Error when allocating memory for the search result. Exiting.\n");
                            exit(1);
                        }
                        else
                        {
                            for(i = 0; i < loops; i++)
                            {
                                recvfrom(
                                    *serverSocket,
                                    (void *)&result[i], 
                                    (size_t) sizeof(result[i]),
                                    0,
                                    (struct sockaddr *) &search_server,
                                    &len
                                );
                                printf(SPACER "%s\n", result[i].metadata.md_name);
                            }

                            // TODO Send bye message to search server
                        }       
                    }
                }
            }
        }
        
        char *usermenu_continue[1] = {"Continue"};
        int choice = print_user_menu(usermenu_continue, 1);
        switch(choice)
        {
            case 654:
                stop = 0;
                break;
            case 666:
                exit(0);
                break;

            default:
                // do nothing, let's continue
                break;
        }
    }
}

void setup_publish_server(struct sockaddr_in *serv_addr, int *serverSocket, char *ip_addr)
{
    printf("Connecting to server "ANSI_COLOR_CYAN"%s"ANSI_COLOR_RESET"...\n", ip_addr);

    memset((char *) serv_addr, 0, sizeof(&serv_addr));
    serv_addr->sin_family = PF_INET;
    serv_addr->sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr->sin_port = htons(PORT_ON_SERV);

    if ((*serverSocket = socket(PF_INET, SOCK_DGRAM, 0)) <0)
    {
        perror ("Socket error.");
        exit (1);
    }

    struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt (*serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        // error("setsockopt failed\n");        
        // Who cares
    }

    if (setsockopt (*serverSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        // error("setsockopt failed\n");        
        // Who cares
    }
}


void *low_energy_server_run(/* void * truc */)
{
    printf("Coucou, je suis un thread!");
    while(1) { }
}