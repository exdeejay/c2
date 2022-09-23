#ifndef SIMPLECOMMAND_H
#define SIMPLECOMMAND_H

#include <vector>
#include <memory>
#include <functional>
#include <cstdint>
#include <utility>
#include "controller.h"
#include "packet.h"
#include "field.h"


namespace c2 {
    template<class...> struct FnCaller;
    template<class T, class... Args> struct FnCaller<T, Args...> {
    // public:
    //     static std::function<int(Controller&)> bind_fn(
    //         const std::vector<char>& data,
    //         int(*fn)(Controller&, T, Args...)
    //     ) {
    //         return FnCaller<Args...>::bind_fn(data, (void*)fn);
    //     }
    // private:
        template<class... Accum> static std::function<int(Controller&)> bind_fn(
            std::vector<char>::const_iterator data,
            const std::vector<char>::const_iterator& end,
            void* fn,
            Accum... accumulated_args
        ) {
            return FnCaller<Args...>::bind_fn(data, end, fn, accumulated_args..., Field<T>::parse_field(data, end));
        }
    };
    template<> struct FnCaller<> {
    // public:
    //     static std::function<int(Controller&)> bind_fn(
    //         const std::vector<char>& data,
    //         int(*fn)(Controller&)
    //     ) {
    //         return FnCaller<>::bind_fn(data, (void*)fn);
    //     }
    // private:
        template<class... Accum> static std::function<int(Controller&)> bind_fn(
            std::vector<char>::const_iterator data,
            const std::vector<char>::const_iterator& end,
            void* fn,
            Accum... accumulated_args
        ) {
            int(*proc)(Controller&, Accum...) = (int(*)(Controller&, Accum...)) fn;
            return std::bind(proc, std::placeholders::_1, accumulated_args...);
        }
    };
}

template<class... Args>
class CommandPacket : public Packet {
public:
    CommandPacket(packettype_t type, int(*proc)(Controller&, Args...), const std::vector<uint8_t>& data) : Packet(type) {
        bound_proc = c2::FnCaller<Args...>::bind_fn(data.cbegin(), data.cend(), proc);
    }

    int execute(Controller& ctrl) {
        return bound_proc(ctrl);
    }

    SerializedPacket serialize() const {
        //TODO: implement this
        return SerializedPacket(_type, std::vector<uint8_t>>());
    }

    static std::pair<
        std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)>,
        std::function<bool(Controller&, Packet&)>
    > build(packettype_t type, int(*proc)(Controller&, Args...)) {
        auto builder = [type, proc](const std::vector<uint8_t>& data) {
            return std::make_unique<CommandPacket>(type, proc, data);
        };
        auto handler = [](Controller& ctrl, Packet& pkt) {
            CommandPacket& cmdpkt = dynamic_cast<CommandPacket&>(pkt);
            ctrl.ret(cmdpkt.execute(ctrl));
            return true;
        };
        return std::make_pair(builder, handler);
    }

private:
    std::function<int(Controller&)> bound_proc;
};



#endif