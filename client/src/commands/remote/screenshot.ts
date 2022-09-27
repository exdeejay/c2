import fs = require('fs');
import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';

export = function (commands: CommandList) {
	commands['screenshot'] = screenshot;
}

async function screenshot(server: ControlServer, args: string[]) {
    let packet = server.commandPacket('screenshot');
    await server.sendHostCommand(packet);
	let response = await server.waitForPacket('buffer');
	fs.writeFileSync('out.png', response.data);
	console.log('Wrote image to out.png');
}