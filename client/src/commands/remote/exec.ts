import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['exec'] = exec;
	commands['shell'] = shell;
	commands['start'] = start;
};

async function exec(server: ControlServer, args: string[]) {
	if (args.length < 2) {
		console.log(`Usage: ${args[0]} <program> [args]`);
	}
	let packet = server.commandPacket('exec');
	packet.cmd = args.slice(1).join(' ');
	packet.wait = true;
	await server.sendHostCommand(packet);
}

async function shell(server: ControlServer, args: string[]) {
	await exec(server, ['exec', 'cmd', '/c'].concat(args.slice(1)));
}

async function start(server: ControlServer, args: string[]) {
	if (args.length < 2) {
		console.log(`Usage: ${args[0]} <program> [args]`);
	}
	let packet = server.commandPacket('exec');
	packet.cmd = args.slice(1).join(' ');
	packet.wait = false;
	await server.sendHostCommand(packet);
}
