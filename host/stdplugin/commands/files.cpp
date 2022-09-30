#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "controller.h"
#include "commands.h"
#include "field.h"
using namespace std;


int DownloadFileCommand::execute(Controller& ctrl, string path) {
	ifstream file(path, ios::binary | ios::ate);
	if (!file) {
		return -1;
	}
	auto size = file.tellg();
	file.seekg(0, ios::beg);

	vector<uint8_t> buf(size);
	file.read((char*) buf.data(), size);
	if (!file) {
		return -2;
	}
	ctrl.send_buffer(buf);
	return 0;
}

int UploadFileCommand::execute(Controller& ctrl, string path, vector<uint8_t> filebuf) {
	ofstream file(path, ios::binary);
	if (!file) {
		return -1;
	}
	file.write((char*) filebuf.data(), filebuf.size());
	if (!file) {
		return -2;
	}
	return 0;
}