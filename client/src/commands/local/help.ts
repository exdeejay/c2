import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

let commandMap: CommandList;

export = function (commands: CommandList) {
    commands['help'] = help;
	commandMap = commands;
};

function help(server: ControlServer, args: string[]) {
	console.log('List of commands:');
    for (let c in commandMap) {
		console.log(`  ${c}`);
	}
}
