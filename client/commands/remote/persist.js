module.exports = function (commands) {
    commands['persist'] = persist;
};

async function persist(server, args) {
	let packet = server.commandPacket('persist');
	let ret = await server.sendHostCommand(packet);
	if (ret != 0) {
		console.error(`ERROR: Something went wrong (${ret})`);
	} else {
		console.log('Persistence established.');
	}
}