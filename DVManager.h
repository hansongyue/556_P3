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
    unordered_map<unsigned short, DVEntry> DV_table;


    void refresh();

    vector<PacketPair> parseDVUpdatePacket(void* packet) {
        unsigned short* start_pos = (unsigned short*) packet;
        unsigned short size = *(start_pos + 1);
        return parsePacketPairs(start_pos + 2, size + 1);
    }

    void receivePacket(void* packet, unsigned short port, int size) {
        auto pairs = parseDVUpdatePacket(packet);
        unsigned short source_id = pairs[0].first;
    }
};


#endif //PROJECT3_DVMANAGER_H
