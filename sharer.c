#include <stdio.h>
#include <stdlib.h>
#include "util/metadata.h"
#include "util/usermenu.h"

void publish_new_file();

int main(int argc, char *argv[])
{
	char addr_server[30];
	int stop = 1;

	printf("Welcome! You have launched a pair!\n");
	printf("Please write the central server's address ip: ");

    read_line(addr_server, sizeof(addr_server), stdin);

	printf("Central server's address ip: %s\n", addr_server);

	
	while(stop)
	{
		printf("\n\n");
		printf("What would you like to do?\n");

		char *usermenu1[4] = {"Change the central server's address ip.", "Publish a new file.", "Search for a file", "Exit"};
		int choice = print_user_menu(usermenu1, 4);

		switch(choice)
		{
			case 0:
				printf("New central server's address: ");
                read_line(addr_server, sizeof(addr_server), stdin);

                printf("Central server's address ip: %s\n", addr_server);

				break;
			case 1:
				printf("Publish a new file.\n");
                publish_new_file();

				break;
			case 2:
				printf("Search for a file.\n");
				break;
			case 3:
				printf("Bye!\n");
				stop = 0;
				break;
			case 666:
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

void publish_new_file()
{
    int stop = 1;
    while(stop)
    {
        printf("\n\n");
        printf("Please insert the file's absolute path:\n");
        char file[500];
        
        read_line(file, sizeof(file), stdin);

        // TODO: PUBLISH THE FILE TO SERVER

        char *usermenu_continue[1] = {"Continue"};
        int choice = print_user_menu(usermenu_continue, 1);
        switch(choice)
        {
            case 666:
                stop = 0;
                break;
            default:
                // do nothing, let's continue
                break;
        }
    }
}