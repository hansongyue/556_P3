//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DVMANAGER_H
#define PROJECT3_DVMANAGER_H

#include "common.h"
#include "Node.h"


class DVManager {
public:
    Node *sys; // To store Node object; used to access GSR9999 interfaces
    unsigned short router_id;
    unsigned short num_ports;
    unordered_map<unsigned short, Neighbor> *neighbors;
    unordered_map<unsigned short, Port> *ports;
    unordered_map<unsigned short, DV_Entry> *DV_table;
    unordered_map<unsigned short, unsigned short>* forwarding_table;


    void refresh();

    vector<PacketPair> parseDVUpdatePacket(void* packet) {
        unsigned short* start_pos = (unsigned short*) packet;
        unsigned short size = *(start_pos + 1);
        return parsePacketPairs(start_pos + 2, size + 1);
    }

    void createNeighborIfNotExist(unsigned short neighbor_id, unsigned short port, vector<PacketPair> pairs) {
        int size = pairs.size();
        if (neighbors->find(neighbor_id) == neighbors->end()) {
            for (int i = 1; i < size; i++) {
                if (pairs[i].first == router_id) {
                    Neighbor neighbor{port, pairs[i].second};
                    (*neighbors)[neighbor_id] = neighbor;
                    break;
                }
            }
        }
    }

    void updateCost() {

    }

    void receivePacket(void* packet, unsigned short port, int size) {
        auto pairs = parseDVUpdatePacket(packet);
        unsigned short source_id = pairs[0].first;
        createNeighborIfNotExist(source_id, port, pairs);
        vector<PacketPair> cur_packet;
        unsigned short source_cost = DV_table->find(source_id)->second.cost;
        for (int i = 1; i < size; i++) {
            unsigned short dest_id = pairs[i].first;
            unsigned short cost = pairs[i].second;
            if (DV_table->find(dest_id) == DV_table->end()) {
                if (cost == INFINITY_COST || dest_id == router_id) {
                    continue;
                }
                // create new cell
                DV_Entry entry;
                entry.next_hop = source_id;
                entry.cost = cost + source_cost;
                entry.last_update_time = sys->time();
                (*DV_table)[dest_id] = entry;
                PacketPair pair(dest_id, entry.cost);
                cur_packet.push_back(pair);
                (*forwarding_table)[dest_id] = source_id;
                continue;
            }
            auto it = DV_table->find(dest_id);
            bool updated = false;
            if (cost == INFINITY_COST) {
                // poison reversed : if source_id --(next hop: router_id)-> dest_id, cost = INF
                if (source_id == it->second.next_hop) {
                    continue;
                }
                continue;
            }
            if (neighbors->find(dest_id) != neighbors->end()) {
                // if dest is a direct neighbor, set its cost firstly
                it->second.cost = neighbors->find(dest_id)->second.cost;
                it->second.next_hop = dest_id;
                it->second.last_update_time = sys->time();
                PacketPair pair(dest_id, it->second.cost);
                cur_packet.push_back(pair);
                (*forwarding_table)[dest_id, it->second.next_hop];
            }
            if (it->second.cost > cost + source_cost) {
                it->second.cost = cost + source_cost;
                it->second.next_hop = source_id;
                it->second.last_update_time = sys->time();
                PacketPair pair(dest_id, it->second.cost);
                cur_packet.push_back(pair);
                (*forwarding_table)[dest_id, it->second.next_hop];
            }
        }
    }

    void sendUpdatePacket(vector<PacketPair>* pairs) {

    }

    void sendUpdatePacket(unordered_map<unsigned short, DV_Entry> DV_table) {
        unsigned int size = DV_table.size() * 4 + 8; // in bytes
        char * DV_update_pkt = (char *) malloc(size * sizeof(char));
        for (unsigned short i = 0; i < num_ports; i++) {
            *DV_update_pkt = DV; //type, 1 byte
            // reserve 1 byte
            *(unsigned short *)(DV_update_pkt + 2) = size;
            *(unsigned short *)(DV_update_pkt + 4) = router_id;
            *(unsigned short *)(DV_update_pkt + 6) = (*ports)[i].to;
            int bytes_count = 8;
            for (auto it : DV_table) {
                *(unsigned short *)(DV_update_pkt + bytes_count) = it.first;
                bytes_count += 2;
                *(unsigned short *)(DV_update_pkt + bytes_count) = it.second.cost;
                bytes_count += 2;
            }
            sys->send(i, DV_update_pkt, size);
        }
    }
};

#endif //PROJECT3_DVMANAGER_H
