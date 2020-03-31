//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DETECTNEIGHBOR_H
#define PROJECT3_DETECTNEIGHBOR_H

#include "Node.h"
#include "RoutingProtocolImpl.h"

struct Neighbor {
    unsigned short port;
    unsigned short router_id;
    unsigned int cost;
};

struct Port {
    unsigned short id;
    unsigned short to;
    unsigned int last_update_time;
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

int PingPong_size = sizeof(PingPong_msg);

void RoutingProtocolImpl :: createPingPongMessage() {
    for (auto i = 0; i < num_ports; i++) {
        auto *PingPong_pkt = new PingPong_msg(PING, router_id, sys->time());
        sys->send(i, (char *) PingPong_pkt, PingPong_size);
    }
}

void RoutingProtocolImpl :: handleMessage(unsigned short port, void *packet, unsigned short size) {
    PingPong_msg msg = *(PingPong_msg *)packet;
    if (msg.type == PING) {
        msg.type = PONG;
        msg.router_id = router_id;
        msg.timestamp = sys->time();
        sys->send(port, (char *) &msg, PingPong_size);
    }
    else if (msg.type == PONG) {
        unsigned int cur_time = sys->time();
        unsigned int rtt = cur_time - msg.timestamp;
        // update ports table
        ports[port].to = msg.router_id;
        ports[port].last_update_time = cur_time;
        // update direct neighbors table
        if (neighbors.count(msg.router_id)) {
            neighbors[msg.router_id].port = port;
            neighbors[msg.router_id].router_id = msg.router_id;
            neighbors[msg.router_id].cost = rtt;
        }
        else {
            Neighbor neighbor {port, msg.router_id, rtt};
            neighbors[msg.router_id] = neighbor;
        }


    }
    else {

    }
}




#endif //PROJECT3_DETECTNEIGHBOR_H
