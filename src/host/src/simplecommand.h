#ifndef SIMPLECOMMAND_H
#define SIMPLECOMMAND_H

#include <memory>
#include <vector>
#include <typeinfo>
#include <functional>
#include "controller.h"
#include "packet.h"
#include "field.h"


namespace c2 {
    template<class...> class FnCaller;
    template<class T, class... Args> class FnCaller<T, Args...> {
    public:
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
    template<> class FnCaller<> {
    public:
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
    CommandPacket(unsigned char type, int(*proc)(Controller&, Args...), const std::vector<char>& data) : Packet(type) {
        bound_proc = c2::FnCaller<Args...>::bind_fn(data.cbegin(), data.cend(), proc);
    }

    int execute(Controller& ctrl) {
        return bound_proc(ctrl);
    }

    SerializedPacket serialize() const {
        //TODO: implement this
        return SerializedPacket(_type, std::move(std::make_unique<std::vector<char>>()));
    }

    static void build(
        unsigned char type,
        int(*proc)(Controller&, Args...)
    ) {
        Packet::register_type(type, [type, proc](const std::vector<char>& data) {
            return std::make_unique<CommandPacket>(type, proc, data);
        });
        Packet::register_handler(type, [](Controller& ctrl, Packet& pkt) {
            CommandPacket& cmdpkt = dynamic_cast<CommandPacket&>(pkt);
            ctrl.ret(cmdpkt.execute(ctrl));
            return true;
        });
    }

private:
    std::function<int(Controller&)> bound_proc;
};

template<class... Args> void register_command(unsigned char type, int(*command)(Controller&, Args...)) {
    CommandPacket<Args...>::build(type, command);
}

#endif