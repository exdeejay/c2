#ifndef CONTROLLER_IMPL_H
#define CONTROLLER_IMPL_H

#include <cstdint>
#include <map>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

class ControllerImpl {
public:
    std::unordered_map<uint8_t, std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>> registry;
    std::multimap<uint8_t, std::function<bool(Controller&, Packet&)>> handlers;
};

#endif