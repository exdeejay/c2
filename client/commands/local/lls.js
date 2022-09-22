const fs = require('fs/promises');

module.exports = async function (args) {
	let path = '.';
	if (args.length > 1) {
		path = args[1];
	}
    let files = await fs.readdir(path);
	for (let f of files) {
		console.log(f);
	}
}