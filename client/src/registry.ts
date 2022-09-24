import fs = require('fs/promises');

type CommandFunc = (control: any, args: any[]) => void;

export interface CommandList {
    [key: string]: CommandFunc;
};

export class CommandRegistry {
    loaded: boolean;
    commands: CommandList;

    constructor(public dirName = `${__dirname}/commands`) {
        this.loaded = false;
        this.commands = {};
    }

    async readCommandsIntoArray(categoryName: string, arr: CommandList) {
        let dirName = `${this.dirName}/${categoryName}`;
        let files = await fs.readdir(dirName);
        for (let file of files) {
            require(`${dirName}/${file}`)(arr);
        }
    }

    async loadCommands() {
        if (!this.loaded) {
            await this.readCommandsIntoArray('local', this.commands);
            await this.readCommandsIntoArray('remote', this.commands);
            this.loaded = true;
        }
    }
}
