#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "controller.h"
#include "field.h"
using namespace std;


int downloadFile(Controller& ctrl, const string path) {
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

int uploadFile(Controller& ctrl, const string path, vector<uint8_t> data) {
	vector<uint8_t> filebuf = Field<vector<uint8_t>>::parse_field(data.begin(), data.end());
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