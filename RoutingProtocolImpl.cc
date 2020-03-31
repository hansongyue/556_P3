#include "RoutingProtocolImpl.h"

RoutingProtocolImpl::RoutingProtocolImpl(Node *n) : RoutingProtocol(n) {
    sys = n;
    // add your own code
}

RoutingProtocolImpl::~RoutingProtocolImpl() {
    // add your own code (if needed)
}

void RoutingProtocolImpl::init(unsigned short num_ports, unsigned short router_id, eProtocolType protocol_type) {
    this->protocol_type = protocol_type;
    this->num_ports = num_ports;
    this->router_id = router_id;
    // add your own code
}

void RoutingProtocolImpl::handle_alarm(void *data) {
    char type = ((char *)data)[0];
    // handle type
}

void RoutingProtocolImpl::recv(unsigned short port, void *packet, unsigned short size) {
    cout << "receive data: " << (char *)packet << endl;
    // add your own code
}

// add more of your own code
