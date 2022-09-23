import fs = require('fs/promises');

let loaded = false;
export const commands = {};

async function readCommandsIntoArray(categoryName: string, arr: any) {
    let dirName = `${__dirname}/commands/${categoryName}`;
    let files = await fs.readdir(dirName);
    for (let file of files) {
        require(`${dirName}/${file}`)(arr);
    }
}

export async function loadCommands() {
    if (!loaded) {
        await readCommandsIntoArray('local', commands);
        await readCommandsIntoArray('remote', commands);
        loaded = true;
    }
}