#include <stdio.h>
#include <stdlib.h>
#include "util/metadata.h"
#include "util/usermenu.h"

int main(int argc, char *argv[])
{
	char addr_server[30];
	int stop = 1;

	printf("Welcome! You have launched a pair!\n");
	printf("Please write the central server's address ip: ");
	scanf("%s", addr_server);

	printf("Central server's address ip: %s\n", addr_server);

	
	while(stop)
	{
		printf("\n\n");
		printf("What would you like to do?\n");

		char *usermenu1[4] = {"Change the central server's address ip.", "Manage your file publishment", "Search for a file", "Exit"};
		int choice = print_user_menu(usermenu1, 4);

		switch(choice)
		{
			case 0:
				printf("Edit central server's address.\n");
				break;
			case 1:
				printf("Manage file publishment.\n");
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
		}
	}

	return 0;
}