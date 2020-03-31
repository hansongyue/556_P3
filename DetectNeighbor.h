//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DETECTNEIGHBOR_H
#define PROJECT3_DETECTNEIGHBOR_H

#include "Node.h"
#include "common.h"
#include "RoutingProtocolImpl.h"

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
        unsigned int RTT = cur_time - msg.timestamp;
        unsigned short neighbor_id = msg.router_id;
        // update ports table
        ports[port].to = neighbor_id;
        unsigned int old_RTT = ports[port].cost;
        ports[port].cost = RTT;
        ports[port].last_update_time = cur_time;
        // update direct neighbors table
        if (neighbors.count(neighbor_id)) {
            neighbors[neighbor_id].port = port;
            neighbors[neighbor_id].cost = RTT;
        }
        else { // find a new neighbor
            Neighbor neighbor { port, RTT };
            neighbors[neighbor_id] = neighbor;
        }
    }
}




#endif //PROJECT3_DETECTNEIGHBOR_H
