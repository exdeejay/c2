const yargs = require('yargs');
const { loadPacketTypes } = require('../common/protocol');
const registry = require('./registry');
const { readline, parseArgs } = require('./readline');
const { ControlConnection } = require('./controlconnection');
const { ControlLocal } = require('./controllocal');

async function main() {
    let argv = yargs(process.argv.slice(2))
        .usage('Usage: $0 [-h ip] [-p port]')
        .option('host')
        .string('host')
        .alias('host', 'h')
        .default('port', 35768)
        .number('port')
        .alias('port', 'p')
        .help('help').argv;

    await loadPacketTypes();
    await registry.loadCommands();

    let local = argv.host === undefined;
    let control = null;
    if (local) {
        control = new ControlLocal('0.0.0.0', 6997);
        await new Promise((resolve) => {
            control.on('listening', resolve);
        });
        console.log(`Local control server started at 0.0.0.0:6997`);
    } else {
        control = new ControlConnection(argv.host, argv.port);
        try {
            await new Promise((resolve, reject) => {
                control.on('connect', resolve);
                control.on('error', reject);
                control.on('timeout', reject);
            });
            control.on('error', (err) => {
                console.error(`ERROR: Error occurred with remote server (${err})`);
            });
            control.on('close', () => {
                console.error('Server connection closed.');
                process.exit(0);
            });
            console.log(
                `Connected to ${control.ip}:${control.connection.socket().remotePort}`
            );
        } catch (err) {
            console.error(`ERROR: could not connect to server (${err})`);
            process.exit(1);
        }
    }

    control.on('connection', (host) => {
        console.log(`\n[!] Pwned ${host.ip}`);
    });

    process.on('SIGINT', () => {
        control.abortCurrentCommand();
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
                await registry.commands[args[0]](control, args);
            }
        }
    }
    process.exit(0);
}

main();
