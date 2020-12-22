const { Server } = require('http');
const net = require('net');
const readline = require('readline');
const registry = require('./registry');
const server = require('./server');
const Server = server.Server;

async function main() {
    await registry.loadCommands();

    let sock = net.connect(6997);
    sock.on('connect', () => {
        console.log(
            `Connected to ${sock.address().address}:${sock.address().port}`
        );

        server.connectedServer = new Server(sock);

        const rl = readline.createInterface(process.stdin, process.stdout);
    
        rl.prompt();
        rl.on('line', (line) => {
            let args = line.split(' ');
            if (!(args[0] in registry.commands)) {
                console.log('Unknown command.');
            } else {
                await registry.commands[args[0]](args);
            }
            rl.prompt();
        });
    });

    sock.on('error', (err) => {
        console.err(`ERR -- error while connecting to host: ${err}`);
    });

    sock.on('timeout', () => {
        console.log('Connection timed out');
    })
}

main();
