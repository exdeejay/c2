const fs = require('fs/promises');

const commands = {};

async function readCommandsIntoArray(categoryName, arr) {
    let dirName = `${__dirname}/commands/${categoryName}`;
    let files = await fs.readdir(dirName);
    for (let file of files) {
        require(`${dirName}/${file}`)(commands);
    }
}

async function loadCommands() {
    await readCommandsIntoArray('local', commands);
    await readCommandsIntoArray('remote', commands);
}

exports.commands = commands;
exports.loadCommands = loadCommands;
