#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef USERMENU_H
#define USERMENU_H

#define OK              0
#define NO_INPUT        1
#define TOO_LONG        2

int print_user_menu(char **usermenu, int number_of_choices);
char *read_line (char *buf, size_t length, FILE *f);
int get_integer_input();

#endif /* METADATA_H */