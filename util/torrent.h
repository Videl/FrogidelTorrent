#ifndef TORRENT_H
#define TORRENT_H

typedef struct Torrent
{
    /*
    * Pair address
    */
    struct sockaddr_in pair_address;

    /*
    * File metadata
    */
    Metadata metadata;
} Torrent;

#endif