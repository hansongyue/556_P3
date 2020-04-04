//
// Created by Zequn Jiang on 4/3/20.
//

#ifndef PROJECT3_LSMANAGER_H
#define PROJECT3_LSMANAGER_H


#include "Node.h"
#include "common.h"

class LSManager {
public:
    Node *sys; // To store Node object; used to access GSR9999 interfaces
    unsigned short router_id;
    unsigned short num_ports;
    unordered_map<unsigned short, Neighbor> *neighbors;
    unordered_map<unsigned short, Port> *ports;
    unordered_map<unsigned short, unsigned short> *forwarding_table;

    void init(Node *sys, unsigned short routerId, unsigned short numPorts,
              unordered_map<unsigned short, Neighbor> *neighbors, unordered_map<unsigned short, Port> *ports,
              unordered_map<unsigned short, unsigned short> *forwardingTable) {
        this->sys = sys;
        this->router_id = routerId;
        this->num_ports = numPorts;
        this->neighbors = neighbors;
        this->ports = ports;
        this->forwarding_table = forwardingTable;
    }
}

#endif //PROJECT3_LSMANAGER_H