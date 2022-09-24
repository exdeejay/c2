import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['exec'] = exec;
	commands['start'] = start;
};

async function exec(server: ControlServer, args: string[]) {
	if (args.length < 2) {
		console.log(`Usage: ${args[0]} <program> [args]`);
	}
	let packet = server.commandPacket('exec');
	packet.cmd = args.slice(1).join(' ');
	packet.wait = 1;
	await server.sendHostCommand(packet);
}

async function start(server: ControlServer, args: string[]) {
	if (args.length < 2) {
		console.log(`Usage: ${args[0]} <program> [args]`);
	}
	let packet = server.commandPacket('exec');
	packet.cmd = args.slice(1).join(' ');
	packet.wait = 0;
	await server.sendHostCommand(packet);
}
