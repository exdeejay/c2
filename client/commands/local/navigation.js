const fs = require('fs/promises');

module.exports = function (commands) {
    commands['lls'] = lls;
	commands['lcd'] = lcd;
	commands['lpwd'] = lpwd;
};

async function lls(server, args) {
    let path = '.';
    if (args.length > 1) {
        path = args[1];
    }
    let files = await fs.readdir(path);
    for (let f of files) {
        console.log(f);
    }
}

function lcd(server, args) {
	if (args.length != 2) {
		console.log(`Usage: ${args[0]} <dir>`);
	}
    process.chdir(args[1]);
}

function lpwd(server, args) {
	console.log(process.cwd());
}
