import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
	commands['args'] = args;
}

function args(server: ControlServer, argv: string[]) {
	for (let a of argv) {
		console.log(a);
	}
}
