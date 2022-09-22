const fs = require('fs');

module.exports = function (commands) {
	commands['screenshot'] = screenshot;
}

async function screenshot(server, args) {
    let packet = server.commandPacket('screenshot');
    await server.sendHostCommand(packet, async (response) => {
		if (response.type.name == 'buffer') {
			fs.writeFileSync('out.png', response.data);
			console.log('Wrote image to out.png');
		}
	});
}