let commandMap;

module.exports = function (commands) {
    commands['help'] = help;
	commandMap = commands;
};

function help(server, args) {
	console.log('List of commands:');
    for (let c in commandMap) {
		console.log(`  ${c}`);
	}
}
