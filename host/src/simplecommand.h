#ifndef SIMPLECOMMAND_H
#define SIMPLECOMMAND_H

#include <memory>
#include <vector>
#include <typeinfo>
#include "controller.h"
#include "packet.h"


namespace c2 {
    template<class...> struct FnCaller;
    template<class T, class... Args> struct FnCaller<T, Args...> {
        template<class... Accum> static int call_fn(
            std::vector<char>& data,
            void* fn,
            Controller& ctrl,
            Accum... accumulated_args
        ) {
            return FnCaller<Args...>::call_fn(data, fn, ctrl, accumulated_args..., parse_field<T>(data));
        }
    };
    template<> struct FnCaller<> {
        template<class... Accum> static int call_fn(
            std::vector<char>& data,
            void* fn,
            Controller& ctrl,
            Accum... accumulated_args
        ) {
            int(*proc)(Accum...) = (int(*)(Controller&, Accum...)) fn;
            return proc(ctrl, accumulated_args...);
        }
    };
}

template<class... Args>
class CommandPacket : public Packet {
public:
    CommandPacket(int(*proc)(Args...)) : proc(proc) {

    }

    static std::unique_ptr<Packet> build(
        unsigned char type,
        int(*proc)(Controller&, Args...),
        Controller& ctrl,
        std::vector<char>& data
    ) {
        return std::make_unique<CommandPacket>();
        c2::FnCaller<Args...>::call_fn(data, (void*) proc, ctrl);
    }

private:
    int(*proc)(Args...);
};

template<class... Args> void register_command(unsigned char type, std::unique_ptr<Packet>(*command)(Args...)) {
    Packet::register_type(type, std::bind(CommandPacket::build, type, command, _1, _2));
}

#endif