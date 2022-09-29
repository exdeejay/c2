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
#include "ringbuf.h"

class ControllerImpl {
public:
    ControllerImpl(std::string host, uint16_t port) : host(host), port(port) {
        audioBuf.resize(audioBufSize);
        size_t ringbuf_obj_size;
        ringbuf_get_sizes(1, &ringbuf_obj_size, NULL);
        audioBufState = (ringbuf_t*) new char[ringbuf_obj_size];
        ringbuf_setup(audioBufState, 1, audioBufSize);
        audioBufWorker = ringbuf_register(audioBufState, 0);
    }
    ~ControllerImpl() {
        ringbuf_unregister(audioBufState, audioBufWorker);
        delete[] audioBufState;
    }

    static const int audioBufSize = 0x1000;

    std::string host;
    uint16_t port;
    std::unique_ptr<PacketConnection> conn;
    std::unordered_map<packettype_t, std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>> registry;
    std::multimap<packettype_t, std::function<bool(Controller&, Packet&)>> handlers;
    ringbuf_t* audioBufState;
    ringbuf_worker_t* audioBufWorker;
    std::vector<uint8_t> audioBuf;
};

#endif