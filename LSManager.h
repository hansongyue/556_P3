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
    unordered_map<unsigned short, unordered_map<unsigned short, unsigned short>> *LS_table;
    vector<unsigned short> destinations;
    unordered_map<unsigned short, unsigned int> *seq_map;
    unordered_map<unsigned short, unsigned short> *forwarding_table;
    unsigned int seq_num = 0;
    unsigned short num_nodes = 0;

    void init(Node *sys, unsigned short routerId, unsigned short numPorts,
              unordered_map<unsigned short, Neighbor> *neighbors, unordered_map<unsigned short, Port> *ports,
              unordered_map<unsigned short, unsigned short> *forwardingTable) {
        this->sys = sys;
        this->router_id = routerId;
        this->num_ports = numPorts;
        this->neighbors = neighbors;
        this->ports = ports;
        this->forwarding_table = forwardingTable;

        (*LS_table)[router_id] = { { router_id, 0} };
    }

    void floodLSP() {
        unsigned int size = neighbors->size() * 4 + 12; // in bytes
        for (unsigned short i = 0; i < num_ports; i++) {
            if (!(*ports)[i].is_connect) {
                continue;
            }
            char *msg = (char *)malloc(size * sizeof(char));
            *msg = (unsigned char)LS;
            auto packet = (unsigned short *)msg;
            *(packet + 1) = htons(size);
            *(packet + 2) = htons(router_id);
            *(int *)(packet + 4) = htonl(seq_num);
            seq_num++;
            int cnt = 0;
            for (auto it : *neighbors) {
                unsigned short neighbor_id = it.first;
                unsigned short cost = it.second.cost;
                *(packet + 6 + cnt) = htons(neighbor_id);
                cnt++;
                *(packet + 6 + cnt) = htons(cost);
                cnt++;
            }
            sys->send(i, msg, size);
        }
    }

    void recvLSP(void *data, unsigned int size) {
        unsigned short *pkt = (unsigned short *)data;
        unsigned short source_id = ntohs(*(pkt + 2));
        unsigned int seq = ntohl(*(int *)(pkt + 4));
        if (seq_map->count(source_id) && (*seq_map)[source_id] <= seq) {
            return ;
        }
        else { // not in map or seq is larger
            (*seq_map)[source_id] = seq;
            vector<pair<unsigned short, unsigned short>> path_pairs;
            int count = 0;
            unsigned int num_pairs = (size - 12) / 4;
            for (unsigned int i = 0; i < num_pairs; i++) {
                unsigned short neighbor_id = ntohs(*(pkt + count + 6));
                count++;
                unsigned short cost = ntohs(*(pkt + count + 6));
                count++;
                path_pairs.push_back({ neighbor_id, cost });
            }
            updateGraph(source_id, path_pairs);
        }
    }

    unordered_map<unsigned short, unsigned short> insertNodeHelper(unsigned short new_dest) {
        num_nodes++;
        destinations.push_back(new_dest);
        unordered_map<unsigned short, unsigned short> old_node;
        for (auto it : destinations) {
            if (it != new_dest) {
                old_node[it] = USHRT_MAX;
                (*LS_table)[it][new_dest] = USHRT_MAX;
            }
            else
                old_node[it] = 0;
        }
        return old_node;
    }

    void updateGraph(unsigned short source_id, const vector<pair<unsigned short, unsigned short>> &path_pairs) {
        if (LS_table->count(source_id)) {
            // do nothing?
        }
        else {
            LS_table->insert({ source_id, insertNodeHelper(source_id) }); // insert a new node
        }
        for (auto it : path_pairs) {
            if (LS_table->count(it.first)) {
                (*LS_table)[source_id][it.first] = it.second;
                (*LS_table)[it.first][source_id] = it.second;
            }
            else {
                LS_table->insert({ it.first, insertNodeHelper(it.first) });
                (*LS_table)[source_id][it.first] = it.second;
                (*LS_table)[it.first][source_id] = it.second;
            }
        }
    }


    void Dijkstra() {

    }

};




#endif //PROJECT3_LSMANAGER_H
