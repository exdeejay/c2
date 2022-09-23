export = function (commands) {
	commands['args'] = args;
}

function args(server, argv) {
	for (let a of argv) {
		console.log(a);
	}
}
