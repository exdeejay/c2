const net = require('net');
const { loadPacketTypes } = require('../protocol/protocol');
const { handleIncomingHost } = require('./host');
const { handleIncomingController } = require('./control');

const PORT = 6997;
const HOST = '0.0.0.0';
const CONTROL_PORT = 5678;
const CONTROL_HOST = '127.0.0.1';

async function main() {
    await loadPacketTypes();

    const server = net.createServer(handleIncomingHost);
    const controlServer = net.createServer(handleIncomingController);

    server.listen(PORT, HOST, () => {
        console.log(`Listening for incoming connections at ${HOST}:${PORT}`);
    });
    controlServer.listen(CONTROL_PORT, CONTROL_HOST, () => {
        console.log(
            `Connect to control server at ${CONTROL_HOST}:${CONTROL_PORT}`
        );
    });
}

main();
