//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DVMANAGER_H
#define PROJECT3_DVMANAGER_H

#include "common.h"


class DVManager {
public:
    unsigned short router_id;
    unordered_map<unsigned short, Neighbor> neighbors;
    unordered_map<unsigned short, Port> ports;
    unordered_map<unsigned short, unordered_map<unsigned short, DVEntry>> DV_table; // DV_table is a matrix -> <dest ip, <neighbor ip, DV entry>>
    unordered_map<unsigned short, DVEntry> forwarding_table;

    void refresh();

    void parsePairs() {

    }

    void receivePacket(void* packet, unsigned short port, int size) {
        char* start_pos = (char*) packet;
        unsigned short source_id = ntohs(*((unsigned short *) packet + 2));
    }
};


#endif //PROJECT3_DVMANAGER_H
