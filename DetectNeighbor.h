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
        else {
            Neighbor neighbor {port, neighbor_id, RTT};
            neighbors[neighbor_id] = neighbor;
        }

        // check if local DV changes
        if (old_RTT != RTT) { // has local changes
            unsigned int diff = RTT - old_RTT;
            for (auto entry : dvManager.DV_table) {
                if (entry.first == neighbor_id && RTT < dvManager.DV_table[neighbor_id].cost) {  //目的地就是改变的邻居节点
                    entry.second.next_hop = neighbor_id;
                    entry.second.cost = RTT;
                }
                else if (entry.second.next_hop == neighbor_id) { // 目的地不是改变的邻居节点，但需要跳转经过改变的邻居节点
                    unsigned int new_cost = entry.second.cost + diff;
                    if (neighbors.count(entry.first) && neighbors[entry.first].cost < new_cost) { // 目的地是其他的邻居节点
                        entry.second.next_hop = entry.first;
                        entry.second.cost = neighbors[entry.first].cost;
                    }
                    else if (!neighbors.count(entry.first)) { // 目的地不是邻居节点，更新cost
                        entry.second.cost = new_cost;
                    }
                }
                entry.second.last_update_time = sys->time();
            }
        }
        else { // cost (RTT) doesn't change
            for (auto entry : dvManager.DV_table) {
                if (entry.first == neighbor_id || entry.second.next_hop == neighbor_id) {
                    entry.second.last_update_time = sys->time();
                }
            }
        }


    }
    else {

    }
}




#endif //PROJECT3_DETECTNEIGHBOR_H
