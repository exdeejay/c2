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

// screen
int screenshot(Controller* ctrl);

// audio
int audioControl(Controller* ctrl, AudioCommand cmd);

#endif