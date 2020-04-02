#include "RoutingProtocolImpl.h"

RoutingProtocolImpl::RoutingProtocolImpl(Node *n) : RoutingProtocol(n) {
    sys = n;
    // add your own code
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
    sys->set_alarm(this, 10 * 1000, (void *) PP_check_msg);
    sys->set_alarm(this, 30 * 1000, (void *) DV_update_msg);
    sys->set_alarm(this, 1000, (void *) Exp_delay);
}

void RoutingProtocolImpl::handle_alarm(void *data) {
    // handle type
    char type = *(char *)data;
    if (type == 'P') {
        sys->set_alarm(this, 10 * 1000, (void *) PP_check_msg);
    }
    else if (type == 'D') {
        sys->set_alarm(this, 30 * 1000, (void *) DV_update_msg);
    }
    else if (type == 'E') {
        sys->set_alarm(this, 1000, (void *) Exp_delay);
    }
    else {
        cout << "wrong type" << endl;
    }
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
    cout << "receive data: " << (char *)packet << endl;
    // add your own code
}

// add more of your own code
