const { createPacket } = require("../../../lib/packet")
const server = require('../../server');

module.exports = async function (args) {
    let packet = createPacket('host', 'command', 'listfiles');
    console.log(await server.connectedServer.sendHostCommand(packet));
}