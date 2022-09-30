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
#include "controller.h"
#include "plugin.h"
#include "command.h"

class ControllerImpl {
public:
    ControllerImpl(std::string host, uint16_t port) : host(host), port(port) {}

    std::string host;
    uint16_t port;
    std::unique_ptr<PacketConnection> conn;
    std::vector<std::unique_ptr<Plugin>> plugins;
    std::vector<std::unique_ptr<CommandBuilder>> commands;
    std::unordered_map<packettype_t, std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>> registry;
    std::multimap<packettype_t, std::function<bool(Controller&, Packet&)>> handlers;
};

#endif