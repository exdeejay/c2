const { createPacket, logOutput } = require("../../../lib/packet");
const server = require('../../server');


module.exports = async function (args) {
    let packet = createPacket('host', 'command', 'pwd');
    await server.connectedServer.sendHostCommand(packet);
}