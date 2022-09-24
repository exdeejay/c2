#include "packetconnection.h"
#include <vector>
#include <exception>
#include <iostream>
#include <cstdint>

#include <zlib.h>
#include "packet.h"
#include "field.h"
#include "packetconnectionimpl.h"

using namespace std;

PacketConnection::PacketConnection(string host, uint16_t port) : impl(make_unique<PacketConnectionImpl>(host, port)) {}

PacketConnection::~PacketConnection() = default;

void PacketConnection::connect() {
    impl->conn.connect();
}

SerializedPacket PacketConnection::read_packet_sync() {
    while (true) {
        if (impl->compressed_len == -1 && impl->buf.size() >= 4) {
            impl->compressed_len = Field<uint32_t>::parse_field(impl->buf.cbegin(), impl->buf.cend());
        }
        if (impl->compressed_len != -1 && impl->buf.size() >= impl->compressed_len + 4) {
            vector<uint8_t> deflated;
            uint8_t deflateBuf[1024];

            z_stream stream;
            stream.zalloc = Z_NULL;
            stream.zfree = Z_NULL;
            stream.opaque = Z_NULL;
            stream.avail_in = 0;
            stream.next_in = Z_NULL;
            inflateInit(&stream);

            stream.avail_in = impl->compressed_len;
            stream.next_in = (Bytef*) impl->buf.data() + 4;
            bool valid = true;
            uint8_t packet_type = 0;
            do {
                stream.avail_out = sizeof(deflateBuf);
                stream.next_out = (Bytef*) deflateBuf;
                int ret = inflate(&stream, Z_FINISH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    valid = false;
                    break;
                }

                int type_offset = 0;
                if (deflated.size() == 0 && stream.avail_out != sizeof(deflateBuf)) {
                    packet_type = deflateBuf[0];
                    type_offset = 1;
                }
                deflated.insert(deflated.end(), deflateBuf + type_offset, deflateBuf + (sizeof(deflateBuf) - stream.avail_out));
            } while (stream.avail_out == 0);
            inflateEnd(&stream);


            impl->buf.erase(impl->buf.begin(), impl->buf.begin() + impl->compressed_len + 4);
            impl->compressed_len = -1;
            if (!valid) {
                throw new exception("invalid zlib");
            }
            return SerializedPacket(packet_type, move(deflated));
        }
        uint8_t pbuf[1024];
        int bytes = impl->conn.read(pbuf, sizeof(pbuf));
        impl->buf.insert(impl->buf.end(), pbuf, pbuf + bytes);
    }
}

void PacketConnection::write_packet_sync(const SerializedPacket& spkt) {
    vector<uint8_t> data;
    data.push_back(spkt.type);
    data.insert(data.end(), spkt.data.begin(), spkt.data.end());
	vector<uint8_t> out;
    out.resize(4);

	uint8_t buf[1024];
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

	uint32_t size = byteswap32(out.size() - 4);
	out[0] = size & 0xFF;
	out[1] = (size >> 8) & 0xFF;
	out[2] = (size >> 16) & 0xFF;
	out[3] = (size >> 24) & 0xFF;

	lock_guard<mutex> guard(impl->writeMutex);
    impl->conn.write(out.data(), out.size());
}
