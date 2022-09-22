const net = require('net');
const yargs = require('yargs');
const { loadPacketTypes } = require('../protocol/protocol');
const { handleIncomingHost } = require('./host');
const { handleIncomingController } = require('./control');

const DEFAULT_HOST = '0.0.0.0';
const DEFAULT_PORT = 6996;
const DEFAULT_CONTROL_HOST = '127.0.0.1';
const DEFAULT_CONTROL_PORT = 35768;

async function main() {
    let argv = yargs(process.argv.slice(2))
        .usage(
            'Usage: $0 [-h ip] [-p port] [--control-host ip] [--control-host port]'
        )
        .default('host', DEFAULT_HOST)
        .alias('host', 'h')
        .default('port', DEFAULT_PORT)
        .alias('port', 'p')
        .default('control-host', DEFAULT_CONTROL_HOST)
        .default('control-port', DEFAULT_CONTROL_PORT)
        .help('help').argv;

    await loadPacketTypes();

    const server = net.createServer(handleIncomingHost);
    const controlServer = net.createServer(handleIncomingController);

    server.listen(argv.port, argv.host, () => {
        console.log(
            `Listening for incoming connections at ${argv.host}:${argv.port}`
        );
    });
    controlServer.listen(argv['control-port'], argv['control-host'], () => {
        console.log(
            `[>] Connect to control server at ${argv['control-host']}:${argv['control-port']}`
        );
    });
}

main();
