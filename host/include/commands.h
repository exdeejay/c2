#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>

#include "controller.h"


void listFiles(Controller* controller);
void handleCommand(std::vector<char>& buf);

#endif