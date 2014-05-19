#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usermenu.h"


int print_user_menu(char **usermenu, int number_of_choices)
{
	int i = 0;
	int choice = 666;
	
	for(i = 0; i < number_of_choices; i++)
	{
		printf("%s[%d] %s\n", SPACER, i, usermenu[i]);
	}
	printf("%s[666] Cancel\n", SPACER);

	printf("Your choice: ");
	scanf("%d", &choice);
	fflush(stdin);

	return choice;
}