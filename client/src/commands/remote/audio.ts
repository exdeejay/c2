const Speaker = require('speaker');

/**
 * @type Speaker
 */
let spk = new Speaker({
	channels: 1,
	bitDepth: 16,
	sampleRate: 16000,
});

export = function (commands) {
	commands['audio'] = audio;
}

async function audio(server, args) {
    if (args.length < 2) {
        console.log(`Usage: ${args[0]} <start|stop|save>`);
        return;
    }

	let packet = server.commandPacket('audiocommand');
	let status;
	switch (args[1]) {
		case 'start':
			packet.command = 0;
			status = await server.sendHostCommand(packet);
			if (status == 0) {
				server.registerHostListener(audioHandler);
				console.log('Audio server started');
			}
			return;
		case 'stop':
			packet.command = 1;
			status = await server.sendHostCommand(packet);
			server.removeHostListener(audioHandler);
			if (status == 0) {
				console.log('Audio server stopped');
			}
			return;
		case 'list':
			packet.command = 2;
			await server.sendHostCommand(packet);
			return;
		case 'save':
			return;
		default:
			console.log(`Usage: ${args[0]} <start|stop|list>`);
	}
}


function audioHandler(packet) {
	if (packet._ptype.name == 'audio') {
		spk.write(Buffer.from(packet.data, 'base64'));
	}
}