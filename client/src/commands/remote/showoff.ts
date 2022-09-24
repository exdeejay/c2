import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['showoff'] = showoff;
};

async function showoff(server: ControlServer, args: string[]) {
	let packet = server.commandPacket('showoff');
	packet.type = 0;
	await server.sendHostCommand(packet);
}
