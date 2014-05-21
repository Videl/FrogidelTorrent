#include <stdio.h>
#include <stdlib.h>
#include "util/local_file.h"

#ifndef SHARER_H
#define SHARER_H

void publish_new_file(
    int *serverSocket, 
    struct sockaddr_in serv_addr, 
    ListLocalFile **listOfLocalFiles);
void search_for_a_file();
void low_energy_server_run();
void setup_publish_server(struct sockaddr_in *serv_addr, 
                          int *serverSocket, 
                          char *ip_addr);

#endif /* SHARER_H */