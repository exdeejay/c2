const Server = require('./server');

/**
 * @type {Array}
 */
let buf;
/**
 * @type {number}
 */
let idx;
let historyIdx;
let history = [];

/**
 * @param {string} char
 * @param {number} num
 */
function writeChars(char, num) {
    for (let i = 0; i < num; i++) {
        process.stdout.write(char);
    }
}

/**
 * @param {number} newIndex
 */
function jumpTo(newIndex) {
    if (idx != newIndex) {
        process.stdout.moveCursor(newIndex - idx, 0);
        idx = newIndex;
    }
}

function replaceLine(newLine) {
    let oldLen = buf.length;
    jumpTo(0);
    buf = newLine.split('');
    for (let c of buf) {
        process.stdout.write(c);
    }
    if (oldLen > buf.length) {
        writeChars(' ', oldLen - buf.length);
		writeChars('\b', oldLen - buf.length);
    }
	idx = buf.length;
}

async function readline() {
    process.stdin.setRawMode(true);
    historyIdx = -1;
    idx = 0;
    buf = [];
    return new Promise((resolve) => {
        function rd(char) {
            switch (char.toString()) {
                case '\r\n':
                case '\r':
                case '\n':
                    process.stdout.write('\n');
                    process.stdin.setRawMode(false);
                    process.stdin.removeListener('data', rd);
                    let line = buf.join('');
                    if (history.length == 0 || history[history.length - 1] != line) {
                        history.push(line);
                    }
                    resolve(line);
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
                    Server.connectedServer.close();
                    process.exit(0);
                case '\x1b[A': //up
                    if (historyIdx + 1 < history.length) {
                        historyIdx++;
                        replaceLine(history[history.length - historyIdx - 1])
                    }
                    break;
                case '\x1b[B': //down
                    if (historyIdx - 1 >= -1) {
                        historyIdx--;
						if (historyIdx == -1) {
							replaceLine('');
						} else {
							replaceLine(history[history.length - historyIdx - 1]);
						}
                    }
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
                        process.stdout.write(
                            buf.slice(idx, buf.length).join('')
                        );
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
        if (w.length != 0) {
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
    }
    return args;
}

exports.readline = readline;
exports.parseArgs = parseArgs;
