export = function (commands) {
    commands['showoff'] = showoff;
};

async function showoff(server, args) {
	let packet = server.commandPacket('showoff');
	packet.type = 0;
	await server.sendHostCommand(packet);
}
