const { createPacket } = require('../../../lib/packet');
const server = require('../../server');
const fs = require('fs/promises');
const path = require('path');

module.exports = async function (args) {
    if (args.length < 2 || args.length > 3) {
        console.log(`Usage: ${args[0]} <local file> [remote path]`);
        return;
    }

    try {
        let packet = createPacket('host', 'command', 'uploadfile');
        packet.contents = await fs.readFile(args[1]);
        if (args.length == 3) {
            packet.path = args[2];
        } else {
            packet.path = path.basename(args[1]);
        }
        let ret = await server.connectedServer.sendHostCommand(packet);
        if (ret == 0) {
            console.log(`${args[1]} -> ${packet.path}`);
        } else {
            console.error('ERROR: could not write file');
        }
    } catch (err) {
        console.error(err.message);
    }
};
