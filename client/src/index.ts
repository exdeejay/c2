import yargs = require('yargs');
import { CommandRegistry } from './registry'; './registry';
import { readline, parseArgs } from './readline';
import { ControlConnection } from './controlconnection';
import { ControlLocal } from './controllocal';
import { PacketTypes } from '../../common/src/protocol';
import { ControlBase } from './controlbase';

async function main() {
    let argv = await yargs(process.argv.slice(2))
        .usage('Usage: $0 [-h ip] [-p port]')
        .demandOption('host')
        .string('host')
        .alias('host', 'h')
        .default('port', 35768)
        .number('port')
        .alias('port', 'p')
        .help('help').argv;
    
    const packetTypes = new PacketTypes();
    await packetTypes.load();
    const registry = new CommandRegistry();
    await registry.loadCommands();

    let local = argv.host === undefined;
    let control: ControlBase | null = null;
    if (local) {
        let controlLocal = new ControlLocal('0.0.0.0', 6997);
        control = controlLocal;
        await new Promise((resolve) => {
            controlLocal.on('listening', resolve);
        });
        console.log(`Local control server started at 0.0.0.0:6997`);
    } else {
        let controlRemote = new ControlConnection(argv.host, argv.port);
        control = controlRemote;
        try {
            await new Promise((resolve, reject) => {
                controlRemote.on('connect', resolve);
                controlRemote.on('error', reject);
                controlRemote.on('timeout', reject);
            });
            controlRemote.on('error', (err) => {
                console.error(`ERROR: Error occurred with remote server (${err})`);
            });
            controlRemote.on('close', () => {
                console.error('Server connection closed.');
                process.exit(0);
            });
            console.log(
                `Connected to ${controlRemote.ip}:${controlRemote.connection.socket().remotePort}`
            );
        } catch (err) {
            console.error(`ERROR: could not connect to server (${err})`);
            process.exit(1);
        }
    }
    let resolvedControl = control!;

    resolvedControl.on('connection', (host) => {
        console.log(`\n[!] Pwned ${host.ip}`);
    });

    process.on('SIGINT', () => {
        resolvedControl.abortCurrentCommand();
    });

    if (process.stdin.isTTY) {
        while (true) {
            process.stdout.write('> ');
            let line = await readline();
            if (line == '') {
                continue;
            }
            let args = parseArgs(line);
            if (!(args[0] in registry.commands)) {
                console.log('Unknown command.');
            } else {
                await registry.commands[args[0]](resolvedControl, args);
            }
        }
    }
    process.exit(0);
}

main();
