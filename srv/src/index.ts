import yargs = require('yargs');
import { PacketTypes } from '../../common/src/protocol';
import { CombinedServer } from './combinedserver';

async function main() {
    let argv = await yargs(process.argv.slice(2))
        .usage('Usage: $0 [-h ip] [-p port] [--control-host ip] [--control-host port]')
        .default('host', '0.0.0.0')
        .alias('host', 'h')
        .default('port', 6996)
        .alias('port', 'p')
        .default('control-host', '127.0.0.1')
        .default('control-port', 35768)
        .help('help').argv;

    const packetTypes = new PacketTypes();
    await packetTypes.load();

    let combinedServer = new CombinedServer(packetTypes, {
        host: argv.host,
        port: argv.port,
        controlHost: argv['control-host'],
        controlPort: argv['control-port']
    });
    
    combinedServer.run();
}

main();
