#include "commands.h"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;


int downloadFile(Controller& ctrl, const string path) {
	ifstream file(path, ios::binary | ios::ate);
	if (!file) {
		return -1;
	}
	streamsize size = file.tellg();
	file.seekg(0, ios::beg);

	vector<char> buf(size);
	file.read(buf.data(), size);
	if (!file) {
		return -2;
	}
	ctrl.send_buffer(buf);
	return 0;
}

int uploadFile(Controller& ctrl, const string path, vector<char> data) {
	vector<char> filebuf = parse_field<vector<char>>(data.begin(), data.end());
	ofstream file(path, ios::binary);
	if (!file) {
		return -1;
	}
	file.write(filebuf.data(), filebuf.size());
	if (!file) {
		return -2;
	}
	return 0;
}