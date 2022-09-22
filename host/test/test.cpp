#include <fstream>
#include <iostream>
#include <snappy.h>
#include <string>
#include <vector>

using namespace std;

template <typename T = uint64_t> T decodeVarint(char *input, size_t *offset) {
	T ret = 0;
	size_t i;
	for (i = 0; i < sizeof(T); i++) {
		ret |= (input[i] & 127) << (7 * i);
		if (!(input[i] & 128)) {
			i++;
			break;
		}
	}
	*offset += i;
	return ret;
}

int main(int argc, char *argv[]) {
	cout << "running test: " << endl;

	ifstream file("002987.ldb", ios::binary | ios::ate);
	if (!file) {
		cerr << "could not read file" << endl;
		return 1;
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
		string keyDelta(out.begin() + offset,
						out.begin() + offset + unsharedBytes);
		offset += unsharedBytes;
		int blockOffset = decodeVarint(&out[offset], &offset);
		int blockSize = decodeVarint(&out[offset], &offset);

		blockOffsets.push_back(blockOffset);
		blockSizes.push_back(blockSize);
	}

	bool res =
		snappy::Uncompress(bytes.data() + blockOffsets[3], blockSizes[3], &out);
	if (!res) {
		cout << "!!!corrupted!!!" << endl;
	}

	offset = 0;
	string lastKey;
	while (offset < out.size()) {
		int sharedBytes = decodeVarint<uint32_t>(&out[offset], &offset);
		int unsharedBytes = decodeVarint<uint32_t>(&out[offset], &offset);
		int valueLength = decodeVarint<uint32_t>(&out[offset], &offset);
		cout << "shared bytes: " << sharedBytes << endl;
		cout << "unshared bytes: " << unsharedBytes << endl;
		cout << "value length: " << valueLength << endl;
		string keyDelta(out.begin() + offset, out.begin() + offset + unsharedBytes);
		if (sharedBytes > 0) {
			keyDelta.insert(keyDelta.begin(), lastKey.begin(), lastKey.begin() + sharedBytes);
		}
		cout << "key: " << keyDelta << endl;
		lastKey = keyDelta;
		offset += unsharedBytes;

		if (valueLength > 0) {
			string val(out.begin() + offset + 1, out.begin() + offset + valueLength);
			offset += valueLength;
			cout << "value: " << val << endl;
		}

		cout << endl;
	}
}