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
#include "util/metadata.h"
#include "util/usermenu.h"
#include "util/constants.h"
#include "util/local_file.h"

void publish_new_file(int *serverSocket, struct sockaddr_in serv_addr);
void search_for_a_file();
void low_energy_server_run();
void setup_publish_server(struct sockaddr_in *serv_addr, int *serverSocket, char *ip_addr);

int main(int argc, char *argv[])
{
	char user_addr_server[30];
	int stop = 1;

    /** Network files UDP (Central server) */
    int serverSocket;
    struct sockaddr_in serv_addr;

	printf("Welcome! You have launched a pair!\n");
	printf("Please write the central server's address ip: ");

    read_line(user_addr_server, sizeof(user_addr_server), stdin);

	printf("Central server's address ip: %s\n", user_addr_server);
    setup_publish_server(&serv_addr, &serverSocket, user_addr_server);

	
	while(stop)
	{
		printf("\n\n");
		printf("What would you like to do?\n");

		char *usermenu1[4] = {"Change the central server's address ip.", "Publish a new file.", "Search for a file"};
		int choice = print_user_menu(usermenu1, 3);

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
                publish_new_file(&serverSocket, serv_addr);

				break;
			case 2:
				printf("Search for a file.\n");
                search_for_a_file();

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

	return EXIT_SUCCESS;
}

void publish_new_file(int *serverSocket, struct sockaddr_in serv_addr)
{
    int stop = 1;
    while(stop)
    {
        printf("\n\n");
        printf("Please insert the file's absolute path:\n");
        char file[500];
        int n;
        socklen_t len = sizeof(serv_addr);
        char sendbuf[1500];

        printf(ANSI_COLOR_BLUE SPACER ANSI_COLOR_RESET);
        read_line(file, sizeof(file), stdin);

        LocalFile *fl = file_hotload(file);

        if (fl)
        {
            // <<< PUBLISH THE FILE TO SERVER >>>
            
            n = sendto(*serverSocket, "PUBLISH", strlen("PUBLISH"), 0, 
                (struct sockaddr *) &serv_addr, sizeof(serv_addr));

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
                    exit(1);
                }
                else
                {
                    printf("Received %s !\n.", sendbuf);
                    n = sendto(
                        *serverSocket, 
                        (void *)(fl->md), 
                        (size_t) sizeof(*(fl->md)), 
                        0, 
                        (struct sockaddr *) &publish_server, 
                        sizeof(publish_server)
                    );
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

void search_for_a_file()
{
    int stop = 1;
    while(stop)
    {
        printf("\n");
        printf("Please insert one keyword to search for: ");
        char file[500];

        read_line(file, sizeof(file), stdin);

        // TODO: MAKE THE SEARCH

        // char *usermenu_continue[1] = {"Continue"};
        // int choice = print_user_menu(usermenu_continue, 1);
        // switch(choice)
        // {
        //     case 666:
        //         stop = 0;
        //         break;
        //     default:
        //         // do nothing, let's continue
        //         break;
        // }
    }
}

void low_energy_server_run()
{

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
    timeout.tv_sec = 10;
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