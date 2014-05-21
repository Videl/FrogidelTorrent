#ifndef PUBLISH_H
#define PUBLISH_H

void publish(struct sockaddr_in client_addr, int clilen, int *fileCounter);

#endif