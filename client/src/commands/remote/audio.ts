import Speaker = require('speaker');
import { Packet } from '../../../../common/src/protocol';
import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';

/**
 * @type Speaker
 */
let spk: Speaker;

export = function (commands: CommandList) {
	// commands['audio'] = audio;
	// spk = new Speaker({
	// 	channels: 1,
	// 	bitDepth: 16,
	// 	sampleRate: 16000,
	// });
}

async function audio(server: ControlServer, args: string[]) {
    if (args.length < 2) {
        console.log(`Usage: ${args[0]} <start|stop|save>`);
        return;
    }

	// let packet = server.commandPacket('audiocommand');
	// switch (args[1]) {
	// 	case 'start': {
	// 			packet.command = 0;
	// 			let status = await server.sendHostCommand(packet);
	// 			if (status == 0) {
	// 				server.registerHostListener(audioHandler);
	// 				console.log('Audio server started');
	// 			}
	// 			return;
	// 		}
	// 	case 'stop': {
	// 			packet.command = 1;
	// 			let status = await server.sendHostCommand(packet);
	// 			server.removeHostListener(audioHandler);
	// 			if (status == 0) {
	// 				console.log('Audio server stopped');
	// 			}
	// 			return;
	// 		}
	// 	case 'list': {
	// 			packet.command = 2;
	// 			await server.sendHostCommand(packet);
	// 			return;
	// 		}
	// 	case 'save':
	// 		return;
	// 	default:
	// 		console.log(`Usage: ${args[0]} <start|stop|list>`);
	// }
}


function audioHandler(packet: Packet) {
	if (packet._ptype.name == 'audio') {
		spk.write(packet.data);
	}
}
