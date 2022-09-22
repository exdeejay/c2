const { SSL_OP_SSLEAY_080_CLIENT_DH_BUG } = require('constants');
const net = require('net');
const { loadPacketTypes } = require('../lib/protocol');
const registry = require('./registry');
const { readline, parseArgs } = require('./readline');
const Server = require('./server');

const SRV_HOST = '127.0.0.1';
const SRV_PORT = 5678;

async function main() {
    await loadPacketTypes();
    await registry.loadCommands();

    let sock = net.connect(SRV_PORT, SRV_HOST);
    sock.on('connect', async () => {
        console.log(`Connected to ${sock.remoteAddress}:${sock.remotePort}`);

        let server = new Server(sock);

        if (process.stdin.isTTY) {
            while (true) {
                process.stdout.write('> ');
                let line = await readline();
                let args = parseArgs(line);
                if (!(args[0] in registry.commands)) {
                    console.log('Unknown command.');
                } else {
                    await registry.commands[args[0]](server, args);
                }
            }
        }
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
