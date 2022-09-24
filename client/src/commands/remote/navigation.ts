import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

const COMMAND_LS = 0;
const COMMAND_CD = 1;
const COMMAND_PWD = 2;
const COMMAND_RM = 3;

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
