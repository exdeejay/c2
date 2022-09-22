const { createPacket } = require("../../../lib/packet");
const server = require('../../server');

module.exports = async function (args) {
    let packet = createPacket('host', 'command', 'listfiles');
    let resp = await server.connectedServer.sendHostCommand(packet);
    console.log(resp.stdout);
}