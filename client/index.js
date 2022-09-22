const net = require('net');
const readline = require('readline');
const { loadPacketTypes } = require('../lib/protocol');
const registry = require('./registry');
const Server = require('./server');

const SRV_HOST = '127.0.0.1';
const SRV_PORT = 5678;

async function main() {
    await loadPacketTypes();
    await registry.loadCommands();

    let sock = net.connect(SRV_PORT, SRV_HOST);
    sock.on('connect', () => {
        console.log(`Connected to ${sock.remoteAddress}:${sock.remotePort}`);

        let server = new Server(sock);

        const rl = readline.createInterface(process.stdin, process.stdout);
        rl.prompt();
        rl.on('line', async (line) => {
            let args = line.split(' ');
            if (!(args[0] in registry.commands)) {
                console.log('Unknown command.');
            } else {
                await registry.commands[args[0]](server, args);
            }
            rl.prompt();
        });
    });

    sock.on('error', (err) => {
        console.error(`ERROR: could not connect to host: ${err}`);
        process.exit(1);
    });

    sock.on('timeout', () => {
        console.log('Connection timed out');
    });
}

main();
