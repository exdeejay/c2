const fs = require('fs');

module.exports = function (commands) {
	commands['screenshot'] = screenshot;
}

async function screenshot(server, args) {
    let packet = server.commandPacket('screenshot');
    await server.sendHostCommand(packet, async (response) => {
		if (response._ptype.name == 'buffer') {
			fs.writeFileSync('out.png', Buffer.from(response.data, 'base64'));
			console.log('Wrote image to out.png');
		}
	});
}