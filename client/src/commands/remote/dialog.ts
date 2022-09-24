import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['say'] = say;
    commands['ask'] = ask;
};

const MB_OK = 0x0;
const MB_YESNO = 0x4;
const MB_TASKMODAL = 0x2000;
const MB_SETFOREGROUND = 0x10000;
const MB_TOPMOST = 0x40000;
const MB_SERVICE_NOTIFICATION = 0x200000;

const selectionMap: { [key: number]: string | undefined } = {
	1: "OK",
	2: "Cancel",
	3: "Abort",
	6: "Yes",
	7: "No"
} as const;

async function say(server: ControlServer, args: string[]) {
	if (args.length < 2) {
		console.log(`Usage: ${args[0]} <message>`);
		return;
	}
	let packet = server.commandPacket('dialog');
	packet.type = MB_OK | MB_TASKMODAL;
	packet.message = args.slice(1).join(' ');
	packet.title = "???";
	let ret = await server.sendHostCommand(packet);
	if (ret == 0 || selectionMap[ret] === undefined) {
		console.error(`ERROR: Something went wrong (${ret})`);
		return;
	}
	console.log(`User pressed ${selectionMap[ret]}.`);
}

async function ask(server: ControlServer, args: string[]) {
	if (args.length < 2) {
		console.log(`Usage: ${args[0]} <message>`);
		return;
	}
	let packet = server.commandPacket('dialog');
	packet.type = MB_YESNO | MB_TASKMODAL;
	packet.message = args.slice(1).join(' ');
	packet.title = "???";
	let ret = await server.sendHostCommand(packet);
	if (ret == 0 || selectionMap[ret] === undefined) {
		console.error(`ERROR: Something went wrong (${ret})`);
		return;
	}
	console.log(`User pressed ${selectionMap[ret]}.`);
}