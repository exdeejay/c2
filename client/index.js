const net = require('net');
const { loadPacketTypes } = require('../lib/protocol');
const registry = require('./registry');
const Server = require('./server');

const SRV_HOST = '127.0.0.1';
const SRV_PORT = 5678;

async function readline() {
    process.stdin.setRawMode(true);
    let idx = 0;
    let buf = [];
    return new Promise((resolve) => {
        function rd(char) {
            switch (char.toString()) {
                case '\r\n':
                case '\r':
                case '\n':
                    process.stdout.write('\n');
                    process.stdin.setRawMode(false);
                    process.stdin.removeListener('data', rd);
                    resolve(buf.join(''));
                    return;
                case '\b':
                    if (idx != 0) {
                        idx--;
                        buf.splice(idx, 1);
                        process.stdout.write('\b');
                        if (idx != buf.length) {
                            process.stdout.write(
                                buf.slice(idx, buf.length).join('')
                            );
                            process.stdout.write(' ');
                            process.stdout.moveCursor(idx - buf.length - 1, 0);
                        } else {
                            process.stdout.write(' \b');
                        }
                    }
                    break;
                case '\x03':
                    process.exit(0);
                case '\x1b[A': //up
                    break;
                case '\x1b[B': //down
                    break;
                case '\x1b[C': //right
                    if (idx != buf.length) {
                        process.stdout.moveCursor(1, 0);
                        idx++;
                    }
                    break;
                case '\x1b[D': //left
                    if (idx != 0) {
                        process.stdout.moveCursor(-1, 0);
                        idx--;
                    }
                    break;
                default:
                    process.stdout.write(char);
                    buf.splice(idx, 0, String(char));
                    idx++;
                    if (idx != buf.length) {
                        process.stdout.write(buf.slice(idx, buf.length).join(''));
                        process.stdout.moveCursor(idx - buf.length, 0);
                    }
            }
        }
        process.stdin.on('data', rd);
    });
}

/**
 * @param {string} line 
 * @returns array
 */
function parseArgs(line) {
    let words = line.split(' ');
    let args = [];
    let collecting = false;
    let collect = [];
    for (let w of words) {

        if (collecting) {
            collect.push(w);
            if (w.endsWith('"')) {
                args.push(collect.join(' ').slice(1, -1));
            }
            continue;
        }
        if (w.startsWith('"')) {
            collecting = true;
            collect.push(w);
            continue;
        }
        args.push(w);
    }
    return args;
}

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
