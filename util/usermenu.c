#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usermenu.h"
#include "constants.h"

/**
 * Safely ask a user's some information.
 * 
 * @param  usermenu          The array of questions.
 * @param  number_of_choices The numbers of questions in the array.
 * @return                   The choice the user took.
 */
int print_user_menu(char **usermenu, int number_of_choices)
{
	int i = 0;
	
	for(i = 0; i < number_of_choices; i++)
	{
		printf(ANSI_COLOR_GREEN SPACER ANSI_COLOR_RESET "[%d] %s\n", i, 
            usermenu[i]);
	}
    printf(ANSI_COLOR_GREEN SPACER ANSI_COLOR_RESET  
        ANSI_COLOR_YELLOW "[654] Cancel\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_GREEN SPACER ANSI_COLOR_RESET 
        ANSI_COLOR_RED "[666] Exit\n" ANSI_COLOR_RESET);

	printf("Your choice: ");
    int choice = get_integer_input();

	return choice;
}

int get_integer_input()
{
    int choice = 666;

    char buff[5];

    read_line(buff, sizeof(char)*5, stdin);
    choice = atoi(buff);
    
    return choice;
}

/**
 * http://home.datacomm.ch/t_wolf/tw/c/getting_input.html
 * @param  buf    [description]
 * @param  length [description]
 * @param  f      [description]
 * @return        [description]
 */
char *read_line (char *buf, size_t length, FILE *f)
  /**** Read at most 'length'-1 characters from the file 'f' into
        'buf' and zero-terminate this character sequence. If the
        line contains more characters, discard the rest.
   */
{
  char *p = fgets (buf, length, f);

  if (p) {
    size_t last = strlen (buf) - 1;

    if (buf[last] == '\n') {
      /**** Discard the trailing newline */
      buf[last] = '\0';
    } else {
      /**** There's no newline in the buffer, therefore there must be
            more characters on that line: discard them!
       */
      fscanf (f, "%*[^\n]");
      /**** And also discard the newline... */
      (void) fgetc (f);
    } /* end if */
  } /* end if */
  return p;
} /* end read_line */