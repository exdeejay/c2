const { createPacket } = require('../../../lib/packet');
const server = require('../../server');
const fs = require('fs/promises');
const Speaker = require('speaker');

/**
 * @type Speaker
 */
let spk = new Speaker({
	channels: 1,
	sampleRate: 16000,
});

module.exports = async function (args) {
    if (args.length < 2) {
        console.log(`Usage: ${args[0]} <start|stop|save>`);
        return;
    }

	let packet = createPacket('host', 'command', 'audiocontrol');
	let status;
	switch (args[1]) {
		case 'start':
			packet.command = 0;
			status = await server.connectedServer.sendHostCommand(packet);
			if (status == 0) {
				server.connectedServer.on('hostpacket', audioHandler);
				console.log('Audio server started');
			}
			return;
		case 'stop':
			packet.command = 1;
			status = await server.connectedServer.sendHostCommand(packet);
			server.connectedServer.removeListener('hostpacket', audioHandler);
			if (status == 0) {
				console.log('Audio server stopped');
			}
			return;
		case 'list':
			packet.command = 2;
			await server.connectedServer.sendHostCommand(packet);
			return;
		case 'save':
			return;
		default:
			console.log(`Usage: ${args[0]} <start|stop|save>`);
	}
}


function audioHandler(packet) {
	if (packet.type.name == 'audio') {
		spk.write(packet.data);
	}
}
