#include "packetconnection.h"
#include <vector>
#include <exception>
#include <zlib.h>
#include "packet.h"
#include "util.h"
using namespace std;

void PacketConnection::connect() {
    conn.get()->connect();
}

unique_ptr<SerializedPacket> PacketConnection::read_packet() {
    while (true) {
        if (compressed_len == -1 && buf.size() >= 4) {
            compressed_len = get<uint32_t>(buf.data());
        }
        if (compressed_len != -1 && buf.size() >= compressed_len + 4) {
            vector<char>* deflated = new vector<char>;
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
            return make_unique<SerializedPacket>(packet_type, unique_ptr<vector<char>>(deflated));
        }
        char pbuf[1024];
        int bytes = conn->read(pbuf, sizeof(pbuf));
        buf.insert(buf.end(), pbuf, pbuf + bytes);
    }
}

void PacketConnection::write_packet_sync(const SerializedPacket& spkt) {
	lock_guard<mutex> guard(writeMutex);
    conn->write((const char*) &spkt.type, sizeof(SerializedPacket::type));
    conn->write(spkt.data.get()->data(), spkt.data.get()->size());
}
