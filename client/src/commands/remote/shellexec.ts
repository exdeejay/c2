import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';

export = function (commands: CommandList) {
	commands['openurl'] = openUrl;
}

async function openUrl(server: ControlServer, args: string[]) {
    if (args.length != 2) {
        console.log(`Usage: ${args[0]} <url>`);
        return;
    }
    let packet = server.commandPacket('shellexec');
    packet.verb = 'open';
    packet.file = args[1];
    await server.sendHostCommand(packet);
}