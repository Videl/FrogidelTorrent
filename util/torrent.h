#ifndef TORRENT_H
#define TORRENT_H

typedef struct Torrent
{
    /*
    * Pair address
    */
    char pair_address[100];

    /*
    * File metadata
    */
    Metadata metadata;
} Torrent;

#endif