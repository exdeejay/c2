#ifndef CONTROLLER_IMPL_H
#define CONTROLLER_IMPL_H

#include <cstdint>
#include <map>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include "packet.h"
#include "packetconnection.h"

class ControllerImpl {
public:
    ControllerImpl(std::string host, uint16_t port) : conn(host, port) {}

    PacketConnection conn;
    std::unordered_map<packettype_t, std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>> registry;
    std::multimap<packettype_t, std::function<bool(Controller&, Packet&)>> handlers;
};

#endif