//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_COMMON_H
#define PROJECT3_COMMON_H

#include "global.h"

typedef pair<unsigned short, unsigned short> PacketPair;

vector<PacketPair>& parsePacketPairs(void *start, int size) {
    vector<PacketPair> pairs;
    for (int i = 0; i < size; i++) {
        unsigned short first = ntohs(*((unsigned short*) start + i * 2));
        unsigned short second = ntohs(*((unsigned short*) start + i * 2 + 1));
        auto pair = PacketPair(first, second);
        pairs.push_back(pair);
    }
    return pairs;
}


struct Neighbor {
    unsigned short port;
    unsigned int cost;

    Neighbor(unsigned short port, unsigned int cost) : port(port), cost(cost) {}
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

    DV_Entry(unsigned short nextHop, unsigned int cost, unsigned int lastUpdateTime) : next_hop(nextHop), cost(cost),
                                                                                       last_update_time(
                                                                                               lastUpdateTime) {}
};


#endif //PROJECT3_COMMON_H
