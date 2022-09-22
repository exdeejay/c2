const fs = require('fs/promises');

let loaded = false;
const commands = {};

async function readCommandsIntoArray(categoryName, arr) {
    let dirName = `${__dirname}/commands/${categoryName}`;
    let files = await fs.readdir(dirName);
    for (let file of files) {
        require(`${dirName}/${file}`)(commands);
    }
}

async function loadCommands() {
    if (!loaded) {
        await readCommandsIntoArray('local', commands);
        await readCommandsIntoArray('remote', commands);
        loaded = true;
    }
}

exports.commands = commands;
exports.loadCommands = loadCommands;
