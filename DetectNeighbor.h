//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DETECTNEIGHBOR_H
#define PROJECT3_DETECTNEIGHBOR_H

#include "Node.h"
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
        // update ports table
        ports[port].to = msg.router_id;
        unsigned int old_RTT = ports[port].cost;
        ports[port].cost = RTT;
        ports[port].last_update_time = cur_time;
        // update direct neighbors table
        if (neighbors.count(msg.router_id)) {
            neighbors[msg.router_id].port = port;
            neighbors[msg.router_id].cost = RTT;
        }
        else {
            Neighbor neighbor {port, msg.router_id, RTT};
            neighbors[msg.router_id] = neighbor;
        }

        // check if local DV changes
        if (old_RTT != RTT) { // has local changes
            unsigned int diff = RTT - old_RTT;
            for (auto entry : dvManager.DV_table) {
                if (entry.second.next_hop == msg.router_id) { //
                    unsigned int new_cost = entry.second.cost + diff;
                    if (neighbors.count(entry.first)) { // dest is a neighbor
                        if (neighbors[entry.first].cost < new_cost) {
                            entry.second.next_hop = entry.first;
                            entry.second.cost = new_cost;
                        }
                    }
                    else {
                        entry.second.cost = new_cost;
                    }
                }
                else {

                }
            }
        }
        else { // cost (RTT) doesn't change

        }


    }
    else {

    }
}




#endif //PROJECT3_DETECTNEIGHBOR_H
