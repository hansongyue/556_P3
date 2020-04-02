//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DETECTNEIGHBOR_H
#define PROJECT3_DETECTNEIGHBOR_H

#include "Node.h"
#include "common.h"
#include "RoutingProtocolImpl.h"

int PingPong_size = 12;

void RoutingProtocolImpl :: createPingPongMessage() {
    int size = 8 + 4; // header + timestamp
    for (unsigned int i = 0; i < num_ports; i++) {
        char* Ping_pkt = (char *)malloc(sizeof(char) * size);
        *Ping_pkt = htons(PING);
        // reserve 1 byte
        *(unsigned short *)(Ping_pkt + 2) = htons(size); // size
        *(unsigned short *)(Ping_pkt + 4) = htons(router_id); // source
        *(unsigned short *)(Ping_pkt + 6) = htons(ports[i].to); // target
        *(unsigned int *)(Ping_pkt + 8) = htonl(sys->time());
        sys->send(i, Ping_pkt, PingPong_size);
    }
}

void RoutingProtocolImpl :: handleMessage(unsigned short port, void *packet, unsigned short size) {
    char *data = (char *)packet;
    char type = ntohs(*data);
    if (type == PING) {
        *data = htons(PONG);
        unsigned short target_id = *(unsigned short *)(data + 4);
        *(unsigned short *)(data + 4) = htons(router_id);
        *(unsigned short *)(data + 6) = htons(target_id);
        *(unsigned int *)(data + 8) = htonl(sys->time());
        sys->send(port, data, PingPong_size);
    }
    else if (type == PONG) {
        unsigned int cur_time = sys->time();
        unsigned int timestamp = ntohl(*(unsigned int *)(data + 8));
        unsigned int RTT = cur_time - timestamp;
        unsigned short neighbor_id = ntohs(*(unsigned short *)(data + 4));
        // update ports table
        ports[port].to = neighbor_id;
        ports[port].cost = RTT;
        ports[port].last_update_time = cur_time;
        bool is_connect = ports[port].is_connect;
        ports[port].is_connect = true;
        // update direct neighbors table
        if (neighbors.count(neighbor_id)) { // already connected before, just update cost
            neighbors[neighbor_id].port = port;
            unsigned int old_RTT =  neighbors[neighbor_id].cost;
            neighbors[neighbor_id].cost = RTT;
            unsigned int RTT_diff = RTT - old_RTT;
            if (RTT_diff) {
                for (auto entry : *DVM.DV_table) {
                    if (entry.second.next_hop == neighbor_id) { // is a next_hop of some destinations
                        unsigned int new_RTT = entry.second.cost + RTT_diff;
                        if (neighbors.count(entry.first) && neighbors[entry.first].cost < new_RTT) { // now a direct neighbor is better
                            entry.second.next_hop = entry.first;
                            entry.second.cost = neighbors[entry.first].cost;
                            (*DVM.forwarding_table)[entry.first] = entry.first;
                        }
                        else { // otherwise, use current route and just update cost
                            entry.second.cost += RTT_diff; // may not best route anymore if cost increases
                        }
                        entry.second.last_update_time = sys->time();
                    }
                    else if (entry.first == neighbor_id && RTT < (*DVM.DV_table)[neighbor_id].cost) { // is a direct neighbor destination
                        entry.second.next_hop = neighbor_id;
                        entry.second.cost = RTT;
                        (*DVM.forwarding_table)[neighbor_id] = neighbor_id;
                        entry.second.last_update_time = sys->time();
                    }
                }
                DVM.sendUpdatePacket();
            }
            else {
                for (auto entry : *DVM.DV_table) {
                    if (entry.second.next_hop == neighbor_id || entry.first == neighbor_id) {
                        entry.second.last_update_time = sys->time();
                    }
                }
            }
        }
        else { // find a new neighbor or re-connect
            Neighbor neighbor { port, RTT };
            neighbors[neighbor_id] = neighbor;
            if (DVM.DV_table->count(neighbor_id)) { // re-connect a neighbor, may change best route for itself
                for (auto entry : *DVM.DV_table) {
                    if (entry.first == neighbor_id && RTT < entry.second.cost) {
                        entry.second.next_hop = entry.first;
                        entry.second.cost = RTT;
                    }
                }
            }
            else { // new neighbor (first time appear)
                (*DVM.DV_table)[neighbor_id] = { neighbor_id, RTT, sys->time() };
            }
            DVM.sendUpdatePacket();
        }
    }
}




#endif //PROJECT3_DETECTNEIGHBOR_H
