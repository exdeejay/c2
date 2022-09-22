#include "packetconnection.h"
#include <vector>
#include <exception>
#include <zlib.h>
#include "packet.h"
#include "field.h"
#include <iostream>
using namespace std;

void PacketConnection::connect() {
    conn->connect();
}

SerializedPacket PacketConnection::read_packet() {
    while (true) {
        if (compressed_len == -1 && buf.size() >= 4) {
            compressed_len = Field<uint32_t>::parse_field(buf.cbegin(), buf.cend());
        }
        if (compressed_len != -1 && buf.size() >= compressed_len + 4) {
            unique_ptr<vector<char>> deflated = make_unique<vector<char>>();
            char deflateBuf[1024];

            z_stream stream;
            stream.zalloc = Z_NULL;
            stream.zfree = Z_NULL;
            stream.opaque = Z_NULL;
            stream.avail_in = 0;
            stream.next_in = Z_NULL;
            inflateInit(&stream);

            stream.avail_in = compressed_len;
            stream.next_in = (Bytef*) buf.data() + 4;
            bool valid = true;
            unsigned char packet_type = 0;
            do {
                stream.avail_out = sizeof(deflateBuf);
                stream.next_out = (Bytef*) deflateBuf;
                int ret = inflate(&stream, Z_FINISH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    valid = false;
                    break;
                }

                int type_offset = 0;
                if (deflated->size() == 0 && stream.avail_out != sizeof(deflateBuf)) {
                    packet_type = deflateBuf[0];
                    type_offset = 1;
                }
                deflated->insert(deflated->end(), deflateBuf + type_offset, deflateBuf + (sizeof(deflateBuf) - stream.avail_out));
            } while (stream.avail_out == 0);
            inflateEnd(&stream);


            buf.erase(buf.begin(), buf.begin() + compressed_len + 4);
            compressed_len = -1;
            if (!valid) {
                throw new exception("invalid zlib");
            }
            return SerializedPacket(packet_type, move(deflated));
        }
        char pbuf[1024];
        int bytes = conn->read(pbuf, sizeof(pbuf));
        buf.insert(buf.end(), pbuf, pbuf + bytes);
    }
}

void PacketConnection::write_packet_sync(const SerializedPacket& spkt) {
    vector<char> data;
    data.push_back(spkt.type);
    data.insert(data.end(), spkt.data->begin(), spkt.data->end());
	vector<char> out;
    out.resize(4);

	char buf[1024];
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	deflateInit(&stream, Z_DEFAULT_COMPRESSION);
	stream.avail_in = data.size();
	stream.next_in = (Bytef*) data.data();
	do {
		stream.avail_out = sizeof(buf);
		stream.next_out = (Bytef*) buf;
		deflate(&stream, Z_FINISH);
		out.insert(out.end(), buf, buf + (sizeof(buf) - stream.avail_out));
	} while (stream.avail_out == 0);
	deflateEnd(&stream);

	unsigned int size = byteswap32(out.size() - 4);
	out[0] = size & 0xFF;
	out[1] = (size >> 8) & 0xFF;
	out[2] = (size >> 16) & 0xFF;
	out[3] = (size >> 24) & 0xFF;

	lock_guard<mutex> guard(writeMutex);
    conn->write(out.data(), out.size());
}
