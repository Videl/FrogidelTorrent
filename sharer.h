#include <stdio.h>
#include <stdlib.h>
#include "util/local_file.h"
#include "util/torrent.h"

#ifndef SHARER_H
#define SHARER_H

void publish_new_file(
    int *serverSocket, 
    struct sockaddr_in serv_addr, 
    ListLocalFile **listOfLocalFiles);
void search_for_a_file(
    int *serverSocket, 
    struct sockaddr_in serv_addr);
void *low_energy_server_run();
void setup_publish_server(struct sockaddr_in *serv_addr, 
                          int *serverSocket, 
                          char *ip_addr);

void download_file(Torrent *results, int no_result);

#endif /* SHARER_H */