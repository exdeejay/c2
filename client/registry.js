const fs = require('fs/promises');

const commands = {};

async function readCommandsIntoArray(categoryName, arr) {
    let prevDir = process.cwd();
    process.chdir('client');
    let dirName = `./commands/${categoryName}`;
    let files = await fs.readdir(dirName);
    for (let file of files) {
        let command = require(`${dirName}/${file}`);
        command.category = categoryName;
        let commandName = file.slice(0, file.lastIndexOf('.'));
        commands[commandName] = command;
    }
    process.chdir(prevDir);
}

async function loadCommands() {
    await readCommandsIntoArray('local', commands)
    await readCommandsIntoArray('remote', commands);
}

exports.commands = commands;
exports.loadCommands = loadCommands;
