module.exports = function (commands) {
    commands['persist'] = persist;
};

async function persist(server, args) {
	let packet = server.commandPacket('persist');
	if (args.length == 1) {
		packet.regkey = 'c2';
	} else {
		packet.regkey = args.slice(1).join(' ');
	}
	let ret = await server.sendHostCommand(packet);
	if (ret != 0) {
		console.error(`ERROR: Something went wrong (${ret})`);
	} else {
		console.log('Persistence established.');
	}
}