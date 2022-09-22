const { createPacket } = require("../../../lib/packet");
const server = require('../../server');


module.exports = async function (args) {
    if (args.length > 2) {
        console.log(`Usage: ${args[0]} [dir]`);
        return;
    }

    let packet = createPacket('host', 'command', 'listfiles');
    if (args.length == 2) {
        packet.dir = args[1];
    } else {
        packet.dir = '';
    }
    await server.connectedServer.sendHostCommand(packet);
}
