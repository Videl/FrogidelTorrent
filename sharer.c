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
#include <unistd.h>
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

    /** Network files UDP (Central server) */
    int serverSocket;
    struct sockaddr_in serv_addr;

    /** Hashmap of local files */
    ListLocalFile *local_files_list[16];

    iret = pthread_create(
        &thread_server, 
        NULL, 
        low_energy_server_run, 
        (void*) local_files_list
    );

    if(iret)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n", iret);
        exit(EXIT_FAILURE);
    }

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

        printf("Please insert the file's tags (comma separated):\n");
        read_line(keywords, sizeof(file), stdin);

        LocalFile *fl = file_hotload(file, keywords);

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

                    sendbuf[n] = '\0';

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

                    else
                    {
                        perror("Error when following the publishing protocol.\n");
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
        // Receive the number of results
        int loops = 0;
        Torrent *result;

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

            if (n < 0)
            {
                perror("Error when following the search protocol: ");
            }
            else
            {
                recv_buffer[n] = '\0';

                if(strcmp(recv_buffer, "SEARCH_READY") != 0)
                    perror("Error when following the search protocol : ");

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
                        n = recvfrom(
                            *serverSocket, 
                            &loops, 
                            sizeof(loops),
                            0,
                            (struct sockaddr *) &search_server, 
                            &len
                        );

                        if (n < 0 || loops <= 0)
                        {
                            printf("No search results.\n");
                        }
                        else
                        {
                            // Prepare a loop with as much loops as needed
                            result = malloc(loops*sizeof(Torrent));
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
                                    printf(ANSI_COLOR_CYAN ANTISPACER "%s\n" ANSI_COLOR_RESET, result[i].metadata.md_name);
                                }
                            }
                        }
                    }
                }
            }
        }

        if(loops > 0)
        {
            char *usermenu_continue[2] = {"Download a file", "New search"};
            int choice = print_user_menu(usermenu_continue, 2);
            switch(choice)
            {
                case 654:
                    stop = 0;
                    break;
                case 666:
                    exit(0);
                    break;
                case 0:
                    download_file(result, loops);
                    break;
                default:
                    // cleaning then let's continue
                    free(result); 
                    loops = 0;

                    break;
            }
        }
        else
        {
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


void download_file(Torrent *results, int no_result)
{
    int i = 0;

    for(i = 0; i < no_result; i++)
    {

        printf(ANSI_COLOR_CYAN ANTISPACER "[%d] %s\n" ANSI_COLOR_RESET, i, results[i].metadata.md_name);
    }

    printf("Choose which file to download: ");
    int choice = get_integer_input();

    printf(ANSI_COLOR_RED SPACER "STARTING DOWNLOAD of %s ### <-" ANSI_COLOR_RESET, results[choice].metadata.md_name);
    printf("\n");

    int dialogSocket = 0;
    //int clilen = 0;
    struct sockaddr_in serv_addr;


    // Distant address set to the socket
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr = results[choice].pair_address;
    serv_addr.sin_port = htons(PORT_ON_PAIR);
    
    dialogSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(dialogSocket < 0)
    {
        perror("Error while opening the listening socket\n");
        exit(EXIT_FAILURE);
    }

    else
    {
        struct timeval timeout;      
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (setsockopt (dialogSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
        {
            // error("setsockopt failed\n");        
            // Who cares
        }

        if (setsockopt (dialogSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
        {
            // error("setsockopt failed\n");        
            // Who cares
        }

        if(connect(dialogSocket, (struct sockaddr*) &serv_addr, (socklen_t) sizeof(serv_addr)) < 0)
        {
            perror("Error connecting to pair\n");
            exit(EXIT_FAILURE);
        }

        char request[46];
        sprintf(request, "GET %s", results[choice].metadata.md_hash);
        send(dialogSocket, 
            request, 
            strlen(request), 
            0
        );

        printf("Creating file %s.\n", results[choice].metadata.md_name);
        char buffer[8192];
        FILE *input_file = fopen(results[choice].metadata.md_name, "a");

        while (1)
        {
            // Read data into buffer.  We may not have enough to 
            // fill up buffer, so we
            // store how many bytes were actually read in bytes_read.
            int bytes_read = read(dialogSocket, buffer, sizeof(buffer));
            printf("Read %d bytes.\n", bytes_read);

            if (bytes_read == 0) // We're done reading from the file
                break;

            if (bytes_read < 0) {
                // handle errors
            }

            // You need a loop for the write, because not all of 
            // the data may be written
            // in one call; write will return how many bytes 
            // were written. p keeps
            // track of where in the buffer we are, while we 
            // decrement bytes_read
            // to keep track of how many bytes are left to write.
            void *p = buffer;
            while (bytes_read > 0) 
            {
                int bytes_written = write(fileno(input_file), p, bytes_read);
                if (bytes_written <= 0) 
                {
                    // handle errors
                }
                bytes_read -= bytes_written;
                p += bytes_written;
            }
        }

        fclose(input_file);
        printf("Closing file %s.\n", results[choice].metadata.md_name);
    }
}

void *low_energy_server_run(void * list)
{
    // printf("\n");
    // printf("LOW ENERGY SERVER BOOTING!!!\n");
    // printf("\n");
    ListLocalFile **localfiles = (ListLocalFile**) list;

    int sockfd, newsockfd;
    struct sockaddr_in  low_energy_serv_addr, cli_addr;
    socklen_t low_energy_clilen;
    int tab_clients[FD_SETSIZE];
    fd_set rset, pset;
    int maxfdp1, nbfd, i, sockcli, datasent;
    char msg[45];
    for(i = 0; i < FD_SETSIZE; i++)
    {
        tab_clients[i] = -1;
    }

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) <0) {
        perror("Socket creating error.\n");
        exit(2);
    }
    maxfdp1 = sockfd + 1;

    FD_ZERO(&rset);
    FD_ZERO(&pset);
    FD_SET(sockfd, &rset);

    memset((char*) &low_energy_serv_addr,0, sizeof(low_energy_serv_addr));
    low_energy_serv_addr.sin_family = PF_INET;
    low_energy_serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    low_energy_serv_addr.sin_port = htons(PORT_ON_PAIR);

    if (bind(sockfd,(struct sockaddr *)&low_energy_serv_addr, sizeof(low_energy_serv_addr) ) <0) {
        perror ("File sharer server problem (bind): ");
        exit (1);
    }

    /* Paramètrer le nombre de connexion "pending" */
    if (listen(sockfd, SOMAXCONN) <0) {
        perror ("File sharer server problem (listen): ");
        exit (1);
    }

    while (1) 
    {
        pset = rset; // ou FD_CPY(&rset, &pset);
        nbfd = select(maxfdp1, &pset, NULL, NULL, NULL);

        if(FD_ISSET(sockfd, &pset))
        {
            //printf("New connection.\n");

            low_energy_clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  &low_energy_clilen);
            if (newsockfd < 0) {
                perror("File sharer server problem (accept): ");
                exit (1);
            }

            // Recherche de la place libre
            i = 0;
            while( (i < FD_SETSIZE) && (tab_clients[i] >= 0)) i++;
            if (i == FD_SETSIZE) 
            {
                perror("File sharer server problem (No room for client):");
                exit(1);
            }

            // Adds the new client in the tab.
            tab_clients[i] = newsockfd;
            FD_SET(newsockfd, &rset);

            if(newsockfd >= maxfdp1)
            {
                maxfdp1 = newsockfd + 1;
                printf("Client accepted.\n");
            }
            nbfd--;
        }

        i = 0;
        while ((nbfd > 0) && (i < FD_SETSIZE))
        {
            if(((sockcli = tab_clients[i]) >= 0) && (FD_ISSET(sockcli, &pset)))
            {
                memset( (char*) msg, 0, sizeof(msg) );
                if ((datasent = read(sockcli, msg, sizeof(msg)-1)) < 0)  {
                    perror ("<<< ERROR RECEIVING DATA >>>");
                    exit (3);
                }
                msg[datasent] = '\0';

                /* Parse msg to get the SHA */
                char sha[41];
                int j = 0;
                while(msg[j++] != ' ') strcpy(sha, &msg[j+1]);
                sha[40] = '\0';
                // printf("SHA (%d): '%s'\n", nbfd, sha);

                /* Look for file and send it! */
                LocalFile* data = searchForSha(sha, localfiles);
                if(data != NULL)
                {
                    // Sending the file
                    // printf("File path: %s\n", data->md->md_name);
                    char buffer[8192];
                    FILE *input_file = fopen(data->md->md_name, "rb");

                    while (1)
                    {
                        // Read data into buffer.  We may not have enough to 
                        // fill up buffer, so we
                        // store how many bytes were actually read in bytes_read.
                        int bytes_read = read(
                            fileno(input_file), 
                            buffer, 
                            sizeof(buffer)
                        );
                        if (bytes_read == 0) // We're done reading from the file
                            break;

                        if (bytes_read < 0) {
                            // handle errors
                        }

                        // You need a loop for the write, because not all of 
                        // the data may be written
                        // in one call; write will return how many bytes 
                        // were written. p keeps
                        // track of where in the buffer we are, while we 
                        // decrement bytes_read
                        // to keep track of how many bytes are left to write.
                        void *p = buffer;
                        while (bytes_read > 0) 
                        {
                            int bytes_written = send(sockcli, p, bytes_read, 0);
                            if (bytes_written <= 0) 
                            {
                                // handle errors
                            }
                            bytes_read -= bytes_written;
                            p += bytes_written;
                        }
                    }

                    fclose(input_file);
                }

                if (datasent == 0)
                {
                    printf("Déconnexion\n");
                    FD_CLR(sockcli, &rset);
                    close(sockcli);
                    tab_clients[i] = -1;
                }
                nbfd--;
            }
            i++;
        }
    }

    close(sockfd);  
}