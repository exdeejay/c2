#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>
#include <functional>
#include <type_traits>
#include "field.h"
#include "packet.h"
#include "controller.h"

class Controller;

template<class...> struct FnCaller;
template<class T, class... Args> struct FnCaller<T, Args...> {
    template<class... Accum> static std::function<int(Controller&)> bind_fn(
        std::vector<uint8_t>::const_iterator data,
        const std::vector<uint8_t>::const_iterator& end,
        std::function<int(Controller&, Accum..., T, Args...)> fn,
        Accum... accumulated_args
    ) {
        return FnCaller<Args...>::template bind_fn<Accum..., T>(data, end, fn, accumulated_args..., Field<T>::parse_field(data, end));
    }
    static std::function<int(Controller&)> bind(
        std::vector<uint8_t>::const_iterator data,
        const std::vector<uint8_t>::const_iterator& end,
        std::function<int(Controller&, T, Args...)> fn
    ) {
        return FnCaller<Args...>::template bind_fn<T>(data, end, fn, Field<T>::parse_field(data, end));
    }
};
template<> struct FnCaller<> {
    template<class... Accum> static std::function<int(Controller&)> bind_fn(
        std::vector<uint8_t>::const_iterator data,
        const std::vector<uint8_t>::const_iterator& end,
        std::function<int(Controller&, Accum...)> fn,
        Accum... accumulated_args
    ) {
        return std::bind(fn, std::placeholders::_1, accumulated_args...);
    }

    static std::function<int(Controller&)> bind(
        std::vector<uint8_t>::const_iterator data,
        const std::vector<uint8_t>::const_iterator& end,
        std::function<int(Controller&)> fn
    ) {
        return fn;
    }
};

class CommandBuilder {
public:
    CommandBuilder(packettype_t type) : _type(type) {}
    virtual ~CommandBuilder() {}
    virtual std::pair<std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>, std::function<bool(Controller&, Packet&)>> build() = 0;
    packettype_t type() { return _type; }
private:
    packettype_t _type;
};

template<packettype_t typenum, class... Args>
class Command : public CommandBuilder {
public:
    Command() : CommandBuilder(typenum) {}

    class CommandPacket : public Packet {
    public:
        CommandPacket(Command* command, const std::vector<uint8_t>& data) : Packet(typenum) {
            std::function<int(Controller&, Args...)> proc = [command](Controller& ctrl, Args... args) {
                return command->execute(ctrl, args...);
            };
            bound_proc = FnCaller<Args...>::bind(data.cbegin(), data.cend(), proc);
        }

        int execute(Controller& ctrl) {
            return bound_proc(ctrl);
        }

        SerializedPacket serialize() const {
            //TODO: implement this
            return SerializedPacket(typenum, {});
        }

    private:
        std::function<int(Controller&)> bound_proc;
    };

    virtual int execute(Controller&, Args...) = 0;

    std::pair<std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>, std::function<bool(Controller&, Packet&)>> build() override {
        auto builder = [this](const std::vector<uint8_t>& data) {
            return std::make_unique<CommandPacket>(this, data);
        };
        auto handler = [](Controller& ctrl, Packet& pkt) {
            CommandPacket& cmdpkt = dynamic_cast<CommandPacket&>(pkt);
            ctrl.ret(cmdpkt.execute(ctrl));
            return true;
        };
        return std::make_pair(builder, handler);
    }
};

#endif