//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_COMMON_H
#define PROJECT3_COMMON_H

#include "global.h"

typedef pair<unsigned short, unsigned short> PacketPair;

vector<PacketPair>& parsePacketPairs(void *start, int size);

ePacketType getPacketType(void *packet);

unsigned short getSize(void *packet);

void checkType(void *packet, ePacketType type);

struct Neighbor {
    unsigned short port;
    unsigned short cost;

    Neighbor() {}

    Neighbor(unsigned short port, unsigned short cost) : port(port), cost(cost) {}
};

struct Port {
    unsigned short id;
    unsigned short to;
    unsigned int cost;
    unsigned int last_update_time;
    bool is_connect;
};

struct DV_Entry {
    unsigned short next_hop;
    unsigned int cost;
    unsigned int last_update_time;

    DV_Entry() {
        cout << "init empty dv entry" << endl;
    }

    DV_Entry(unsigned short nextHop, unsigned int cost, unsigned int lastUpdateTime) : next_hop(nextHop), cost(cost),
                                                                                       last_update_time(
                                                                                               lastUpdateTime) {}
};


#endif //PROJECT3_COMMON_H
