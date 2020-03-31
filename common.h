//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_COMMON_H
#define PROJECT3_COMMON_H

#include "global.h"

struct Neighbor {
    unsigned short port;
    unsigned int cost;
};

struct Port {
    unsigned short id;
    unsigned short to;
    unsigned int cost;
    unsigned int last_update_time;
    bool is_connect;
};

struct PingPong_msg {
    ePacketType type;
    unsigned short router_id;
    unsigned int timestamp;

    PingPong_msg(ePacketType type, unsigned short router_id, unsigned int timestamp) {
        this->type = type;
        this->router_id = router_id;
        this->timestamp = timestamp;
    }

};

struct DV_Entry {
    unsigned short next_hop;
    unsigned int cost;
    unsigned int last_update_time;
};

struct Route_Entry {
    unsigned int next_hop;
    unsigned int cost;
    unsigned int last_update_time;
};

#endif //PROJECT3_COMMON_H
