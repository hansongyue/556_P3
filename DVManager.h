//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DVMANAGER_H
#define PROJECT3_DVMANAGER_H

#include <strings.h>
#include "common.h"
#include "Node.h"
#include <unordered_set>


class DVManager {
public:
    Node *sys; // To store Node object; used to access GSR9999 interfaces
    unsigned short router_id;
    unsigned short num_ports;
    unordered_map<unsigned short, Neighbor> *neighbors;
    unordered_map<unsigned short, Port> *ports;
    unordered_map<unsigned short, DV_Entry> *DV_table;
    unordered_map<unsigned short, unsigned short>* forwarding_table;

    void init(Node *sys, unsigned short routerId, unsigned short numPorts,
              unordered_map<unsigned short, Neighbor> *neighbors, unordered_map<unsigned short, Port> *ports,
              unordered_map<unsigned short, unsigned short> *forwardingTable) {
        this->sys = sys;
        this->num_ports = numPorts;
        this->neighbors = neighbors;
        this->ports = ports;
        this->forwarding_table = forwardingTable;
        this->DV_table = new unordered_map<unsigned short, DV_Entry>();
    }

    static vector<PacketPair> parseDVUpdatePacket(void* packet) {
        auto start_pos = (unsigned short*) packet;
        unsigned short size = (ntohs(*(start_pos + 1)) - 8) / 4;
        return parsePacketPairs(start_pos + 2, size + 1);
    }


    void refresh() {
        vector<PacketPair> pairs;
        for (auto & port : *ports) {
            if (!port.second.is_connect || sys->time() - port.second.last_update_time <= 15 * 1000) {
                continue;
            }
            port.second.cost = 0;
            port.second.is_connect = false;
            auto disconnectedNeighbor = port.second.to;
            if (neighbors->find(disconnectedNeighbor) != neighbors->end()) {
                neighbors->erase(disconnectedNeighbor);
            }
            removeInvalidEntries(pairs, disconnectedNeighbor);
        }
    }

    void createNeighborIfNotExist(unsigned short neighbor_id, unsigned short port, vector<PacketPair> pairs) {
        if (neighbors->find(neighbor_id) != neighbors->end()) {
            return;
        }
        int size = pairs.size();
        for (int i = 1; i < size; i++) {
            if (pairs[i].first == router_id) {
                Neighbor neighbor(port, pairs[i].second);
                (*neighbors)[neighbor_id] = neighbor;
                break;
            }
        }
    }

    void updateDVEntry(unsigned short dest_id, unsigned short cost, unsigned short next_hop) {
        auto it = DV_table->find(dest_id);
        it->second.cost = cost;
        it->second.next_hop = next_hop;
        it->second.last_update_time = sys->time();
        (*forwarding_table)[dest_id, it->second.next_hop];
    }

    DV_Entry buildDVEntry(unsigned short dest_id, unsigned short cost, unsigned short next_hop) {
        DV_Entry entry(next_hop, cost, sys->time());
        (*DV_table)[dest_id] = entry;
        (*forwarding_table)[dest_id] = next_hop;
        return entry;
    }

    DV_Entry removeDVEntry(unsigned short dest_id) {
        auto entry = DV_table->find(dest_id)->second;
        DV_table->erase(dest_id);
        forwarding_table->erase(dest_id);
        return entry;
    }

    void removeInvalidEntries(vector<PacketPair> pairs, unsigned int disconnected_neighbor) {
        // remove outdated entries and entries take disconnected neighbor as next hop
        unordered_set<unsigned short> ids_to_remove;
        for (auto & it : *DV_table) {
            if (it.second.next_hop != disconnected_neighbor
                && sys->time() - it.second.last_update_time <= 45 * 1000) {
                continue;
            }
            if (neighbors->find(it.first) == neighbors->end()) { // only remove those node that aren't neighbors
                ids_to_remove.emplace(it.first);
                pairs.emplace_back(it.first, INFINITY_COST);
                continue;
            }
            updateDVEntry(it.first, neighbors->find(it.first)->second.cost, it.first);
            pairs.emplace_back(it.first, it.second.cost);
        }
        for (auto &id: ids_to_remove) {
            removeDVEntry(id);
        }
    }

    void receivePacket(void* packet, unsigned short port, int size) {
        checkType(packet, DV);
        auto pairs = parseDVUpdatePacket(packet);
        unsigned short source_id = pairs[0].first;
        createNeighborIfNotExist(source_id, port, pairs);
        vector<PacketPair> cur_packet;
        unsigned short source_cost = DV_table->find(source_id)->second.cost;
        for (int i = 1; i < size; i++) {
            unsigned short dest_id = pairs[i].first;
            unsigned short cost = pairs[i].second;
            if (DV_table->find(dest_id) == DV_table->end()) { // dest_id currently unreachable
                if (cost == INFINITY_COST || dest_id == router_id) {
                    continue;
                }
                // create new cell as now we can reach dest_id via source_id
                auto entry = buildDVEntry(source_id, cost + source_cost, sys->time());
                cur_packet.emplace_back(dest_id, entry.cost);
                continue;
            }
            auto it = DV_table->find(dest_id);
            if (cost == INFINITY_COST) {
                // poison reversed : if source_id --(next hop: router_id)-> dest_id, then cost = INF
                if (source_id != it->second.next_hop) { // router_id have a way to dest_id even source_id don't
                    cur_packet.emplace_back(dest_id, it->second.cost);
                    continue;
                }
                if (neighbors->find(dest_id) == neighbors->end()) { // link is broken
                    removeDVEntry(dest_id);
                    cur_packet.emplace_back(dest_id, INFINITY_COST);
                    continue;
                }
                // if dest_id is direct neighbor
                if (neighbors->find(dest_id) != neighbors->end()) {
                    // if dest is a direct neighbor, set its cost firstly
                    auto new_cost = neighbors->find(dest_id)->second.cost;
                    updateDVEntry(dest_id, new_cost, dest_id);
                    cur_packet.emplace_back(dest_id, new_cost);
                }
                continue;
            }
            if (neighbors->find(dest_id) != neighbors->end()) {
                // if dest is a direct neighbor, set its cost firstly
                auto new_cost = neighbors->find(dest_id)->second.cost;
                updateDVEntry(dest_id, new_cost, dest_id);
                cur_packet.emplace_back(dest_id, new_cost);
            }
            if (it->second.cost > cost + source_cost) {
                updateDVEntry(dest_id, cost + source_cost, source_id);
                cur_packet.emplace_back(dest_id, cost + source_cost);
            }
        }
        if (cur_packet.size() > 0) {
            sendUpdatePacket(&cur_packet);
        }
    }

    void sendUpdatePacket(vector<PacketPair>* pairs) {
        unsigned int size = (*DV_table).size() * 4 + 8; // in bytes
        char *msg = new char[size * sizeof(char)];
        bzero(msg, size * sizeof(char));
        unsigned short first_short = DV;
        auto packet = (unsigned short *)msg;
        *(packet) = htons(first_short);
        *(packet + 1) = htons(size);
        *(packet + 2) = htons(router_id);
        for (unsigned short i = 0; i < num_ports; i++) {
            if (!(*ports)[i].is_connect) {
                continue;
            }
            *(packet + 3) = htons((*ports)[i].to);
            int cnt = 0;
            for (auto & pair : *pairs) {
                *(packet + 4 + cnt++) = htons(pair.first);
                *(packet + 4 + cnt++) = htons(pair.second);
            }
            sys->send(i, msg, size);
        }
    }

    void sendUpdatePacket() {
        vector<PacketPair> pairs;
        for (auto & it : *DV_table) {
            pairs.emplace_back(it.first, it.second.cost);
        }
        sendUpdatePacket(&pairs);
    }
};

#endif //PROJECT3_DVMANAGER_H
