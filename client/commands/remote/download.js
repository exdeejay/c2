const { createPacket } = require('../../../lib/packet');
const server = require('../../server');
const fs = require('fs/promises');
const path = require('path');

module.exports = async function (args) {
    if (args.length < 2 || args.length > 3) {
        console.log(`Usage: ${args[0]} <remote file> [local path]`);
        return;
    }

    let packet = createPacket('host', 'command', 'downloadfile');
    packet.path = args[1];
    let buffer;
    let ret = await server.connectedServer.sendHostCommand(packet, response => {
        if (response.type.name == 'buffer') {
            buffer = response.data;
        }
    });
    if (ret != 0) {
        console.error('ERROR: could not read file');
        return;
    }
    let localPath;
    if (args.length == 3) {
        localPath = args[2];
    } else {
        localPath = path.basename(args[1]);
    }
    try {
        await fs.writeFile(localPath, buffer);
        console.log(`${args[1]} -> ${localPath}`);
    } catch (err) {
        console.error(err);
    }
};
