const yargs = require('yargs');
const { loadPacketTypes } = require('../common/protocol');
const { CombinedServer } = require('./combinedserver');

async function main() {
    let argv = yargs(process.argv.slice(2))
        .usage('Usage: $0 [-h ip] [-p port] [--control-host ip] [--control-host port]')
        .default('host', '0.0.0.0')
        .alias('host', 'h')
        .default('port', 6996)
        .alias('port', 'p')
        .default('control-host', '127.0.0.1')
        .default('control-port', 35768)
        .help('help').argv;

    await loadPacketTypes();

    new CombinedServer(argv.host, argv.port, argv['control-host'], argv['control-port']);
}

main();
