#include "RoutingProtocolImpl.h"

RoutingProtocolImpl::RoutingProtocolImpl(Node *n) : RoutingProtocol(n) {
    sys = n;
}

RoutingProtocolImpl::~RoutingProtocolImpl() {
    // add your own code (if needed)
}

void RoutingProtocolImpl::init(unsigned short num_ports, unsigned short router_id, eProtocolType protocol_type) {
    // add your own code
    this->num_ports = num_ports;
    this->router_id = router_id;
    this->protocol_type = protocol_type;
    PP_check_msg = 'P';
    DV_update_msg = 'D';
    Exp_delay = 'E';
    createPingPongMessage();
    sys->set_alarm(this, 10 * 1000, (void *) PP_check_msg);
    sys->set_alarm(this, 30 * 1000, (void *) DV_update_msg);
    sys->set_alarm(this, 1000, (void *) Exp_delay);
    DVM.init(sys, router_id, num_ports, &neighbors, &ports, &forwarding_table);
}

void RoutingProtocolImpl::handle_alarm(void *data) {
    // handle type
    char type = *(char *)data;
    if (type == 'P') {
        createPingPongMessage();
        sys->set_alarm(this, 10 * 1000, (void *) PP_check_msg);
    }
    else if (type == 'D') {
        if (protocol_type == P_DV) {
            DVM.sendUpdatePacket();
        }
        sys->set_alarm(this, 30 * 1000, (void *) DV_update_msg);
    }
    else if (type == 'E') {
        if (protocol_type == P_DV) {
            DVM.refresh();
        }
        sys->set_alarm(this, 1000, (void *) Exp_delay);
    }
    else {
        cout << "wrong type" << endl;
    }
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
    auto type = getPacketType(packet);
    switch (type) {
        case DV:
            DVM.receivePacket(packet, port, size);
            break;
        case LS:
            // handle link state packet
            break;
        case PING:
        case PONG:
            handleMessage(port, packet, size);
            break;
        case DATA:
            forwardData(port, packet);
            break;
    }
}

void RoutingProtocolImpl::forwardData(unsigned short port, void *packet) {
    checkType(packet, DATA);
    auto start = (unsigned short *)packet;
    unsigned short dest_id = *(start + 3);
    if (dest_id == this->router_id) {
        delete[] (char *)packet;
        return;
    }
    if (forwarding_table.find(dest_id) == forwarding_table.end()) {
        return;
    }
    auto next_hop = forwarding_table[dest_id];
    sys->send(neighbors[next_hop].port, packet, getSize(packet));
}
