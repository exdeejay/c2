#include "controller.h"
#include <memory>
#include "packet.h"
#include "simplecommand.h"
using namespace std;

void Controller::loop() {
	while (true) {
		SerializedPacket spkt = conn->read_packet();
		consume_packet(spkt);
	}
}
