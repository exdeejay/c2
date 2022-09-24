import fs = require('fs');
import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';

export = function (commands: CommandList) {
	commands['screenshot'] = screenshot;
}

async function screenshot(server: ControlServer, args: string[]) {
    let packet = server.commandPacket('screenshot');
    await server.sendHostCommand(packet, async (response) => {
		if (response._ptype.name == 'buffer') {
			fs.writeFileSync('out.png', Buffer.from(response.data, 'base64'));
			console.log('Wrote image to out.png');
		}
	});
}