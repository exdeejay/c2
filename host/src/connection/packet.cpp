#include "packet.h"

// using namespace std;


// unique_ptr<Packet> Packet::parse(const SerializedPacket& spkt) {
// 	auto pktctr = registry.find(spkt.type);
// 	if (pktctr == registry.end()) {
// 		throw new exception("bad packet");
// 	}
// 	return pktctr->second(*spkt.data.get());
// }