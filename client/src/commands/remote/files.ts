import fs = require('fs/promises');
import path = require('path');
import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';

export = function (commands: CommandList) {
    commands['download'] = download;
    commands['upload'] = upload;
};

async function download(server: ControlServer, args: string[]) {
    if (args.length < 2 || args.length > 3) {
        console.log(`Usage: ${args[0]} <remote file> [local path]`);
        return;
    }

    let packet = server.commandPacket('downloadfile');
    packet.path = args[1];
    let buffer: Buffer;
    let ret = await server.sendHostCommand(packet, (response) => {
        if (response._ptype.name == 'buffer') {
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
        await fs.writeFile(localPath, buffer!);
        console.log(`${args[1]} -> ${localPath}`);
    } catch (err) {
        console.error(err);
    }
}

async function upload(server: ControlServer, args: string[]) {
    if (args.length < 2 || args.length > 3) {
        console.log(`Usage: ${args[0]} <local file> [remote path]`);
        return;
    }

    try {
        let packet = server.commandPacket('uploadfile');
        packet.contents = await fs.readFile(args[1]);
        if (args.length == 3) {
            packet.path = args[2];
        } else {
            packet.path = path.basename(args[1]);
        }
        let ret = await server.sendHostCommand(packet);
        if (ret == 0) {
            console.log(`${args[1]} -> ${packet.path}`);
        } else {
            console.error('ERROR: could not write file');
        }
    } catch (err: any) {
        console.error(err.message);
    }
}
