//
// Created by Zequn Jiang on 3/30/20.
//

#ifndef PROJECT3_DETECTNEIGHBOR_H
#define PROJECT3_DETECTNEIGHBOR_H

#include "Node.h"

class Neighbor {
    Node *sys;
    unsigned short port;
    int cost;
};

void createMessage();

void handleMessage();

#endif //PROJECT3_DETECTNEIGHBOR_H
