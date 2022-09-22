module.exports = function (commands) {
    commands['exec'] = exec;
	commands['start'] = start;
};

async function exec(server, args) {
	if (args < 2) {
		console.log(`Usage: ${args[0]} <program> [args]`);
	}
	let packet = server.createPacket('exec');
	packet.cmd = args.slice(1).join(' ');
	packet.wait = 1;
	await server.sendHostCommand(packet);
}

async function start(server, args) {
	if (args < 2) {
		console.log(`Usage: ${args[0]} <program> [args]`);
	}
	let packet = server.createPacket('exec');
	packet.cmd = args.slice(1).join(' ');
	packet.wait = 0;
	await server.sendHostCommand(packet);
}
