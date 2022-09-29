#ifndef COMMAND_H
#define COMMAND_H

#include "packet.h"

template<class...> struct FnCaller;
template<class T, class... Args> struct FnCaller<T, Args...> {
    template<class... Accum> static std::function<int(Controller&)> bind_fn(std::vector<uint8_t>::const_iterator data, const std::vector<uint8_t>::const_iterator& end, void* fn, Accum... accumulated_args) {
        return FnCaller<Args...>::bind_fn(data, end, fn, accumulated_args..., Field<T>::parse_field(data, end));
    }
};
template<> struct FnCaller<> {
    template<class... Accum> static std::function<int(Controller&)> bind_fn(
        std::vector<uint8_t>::const_iterator data,
        const std::vector<uint8_t>::const_iterator& end,
        void* fn,
        Accum... accumulated_args
    ) {
        int(*proc)(Controller&, Accum...) = (int(*)(Controller&, Accum...)) fn;
        return std::bind(proc, std::placeholders::_1, accumulated_args...);
    }
};

class CommandBuilder {
    virtual std::pair<std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>, std::function<bool(Controller&, Packet&)>> build() = 0;
};

template<packettype_t type, class... Args>
class Command : public CommandBuilder {
public:
    class CommandPacket : public Packet {
        CommandPacket(Command* command, const std::vector<uint8_t>& data) : Packet(type) {
            auto proc = [command](Controller& ctrl, Args... args) {
                command->execute(ctrl, ...args);
            };
            bound_proc = FnCaller<Args...>::bind_fn(data.cbegin(), data.cend(), proc);
        }

        int execute(Controller& ctrl) {
            return bound_proc(ctrl);
        }

        SerializedPacket serialize() const {
            //TODO: implement this
            return SerializedPacket(type, {});
        }

        private:
            std::function<int(Controller&)> bound_proc;
    };

    virtual int execute(Controller&, Args...) = 0;

    std::pair<std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>, std::function<bool(Controller&, Packet&)>> build() override {
        auto builder = [this, proc](const std::vector<uint8_t>& data) {
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