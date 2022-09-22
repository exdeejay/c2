const { createPacket, logOutput } = require("../../../lib/packet");
const server = require('../../server');
const fs = require('fs');
const PNG = require('pngjs').PNG;


module.exports = async function (args) {
    let packet = createPacket('host', 'command', 'screenshot');
    await server.connectedServer.sendHostCommand(packet, async (response) => {
		fs.writeFileSync('out.png', response.data);
		console.log('Wrote image to out.png');
	});
}