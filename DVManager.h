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


    void refresh();

    void parsePairs() {

    }

    void receivePacket(void* packet, unsigned short port, int size) {
        char* start_pos = (char*) packet;
        unsigned short source_id = ntohs(*((unsigned short *) packet + 2));
    }
};


#endif //PROJECT3_DVMANAGER_H
