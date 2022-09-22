#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>

#include "controller.h"

// navigation
int pwd(Controller* controller);
int changeDir(Controller* controller, const std::string dir);
int listFiles(Controller* controller, const std::string dir);

// screen
int screenshot(Controller* controller);

#endif