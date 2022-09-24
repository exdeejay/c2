import fs = require('fs/promises');
import { ControlServer } from '../../controlserver';
import { CommandList } from '../../registry';

export = function (commands: CommandList) {
    commands['lls'] = lls;
	commands['lcd'] = lcd;
	commands['lpwd'] = lpwd;
};

async function lls(server: ControlServer, args: string[]) {
    let path = '.';
    if (args.length > 1) {
        path = args[1];
    }
    let files = await fs.readdir(path);
    for (let f of files) {
        process.stdout.write(f + '    ');
    }
    console.log();
}

function lcd(server: ControlServer, args: string[]) {
	if (args.length != 2) {
		console.log(`Usage: ${args[0]} <dir>`);
	}
    process.chdir(args[1]);
}

function lpwd(server: ControlServer, args: string[]) {
	console.log(process.cwd());
}
