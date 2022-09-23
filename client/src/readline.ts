let buf: string[];
let idx: number;
let historyIdx;
let history = [];

function writeChars(char: string, num: number) {
    for (let i = 0; i < num; i++) {
        process.stdout.write(char);
    }
}

function jumpTo(newIndex: number) {
    if (idx != newIndex) {
        process.stdout.moveCursor(newIndex - idx, 0);
        idx = newIndex;
    }
}

function replaceLine(newLine: string) {
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

export async function readline(): Promise<string> {
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
                            process.stdout.write(buf.slice(idx, buf.length).join(''));
                            process.stdout.write(' ');
                            process.stdout.moveCursor(idx - buf.length - 1, 0);
                        } else {
                            process.stdout.write(' \b');
                        }
                    }
                    break;
                case '\x03':
                    process.stdout.write('\n');
                    process.stdin.setRawMode(false);
                    process.stdin.removeListener('data', rd);
                    resolve('');
                    return;
                case '\x1b[A': //up
                    if (historyIdx + 1 < history.length) {
                        historyIdx++;
                        replaceLine(history[history.length - historyIdx - 1]);
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
                        process.stdout.write(buf.slice(idx, buf.length).join(''));
                        process.stdout.moveCursor(idx - buf.length, 0);
                    }
            }
        }
        process.stdin.on('data', rd);
    });
}

export function parseArgs(line: string): string[] {
    let args = [];
    let current = [];
    let escaped = false;
    let inquotes = false;
    for (let i = 0; i < line.length; i++) {
        let c = line[i];
        if (!escaped && c == '\\') {
            escaped = true;
        } else {
            if (escaped) {
                // in escape sequence
                if (['"', "'", '\\'].includes(c)) {
                    current.push(c);
                } else if (c == 'n') {
                    // \n
                    current.push('\n');
                } else if (c == 'r') {
                    // \r
                    current.push('\r');
                } else if (c == 't') {
                    // \t
                    current.push('\t');
                } else if (c == 'x') {
                    // \xNN
                    if (i + 2 < line.length) {
                        // TODO
                        current.push(String.fromCharCode(parseInt(`${line[i + 1]}${line[i + 2]}`, 16)));
                        i += 2;
                    } else {
                        current.push('\\x');
                    }
                } else {
                    current.push('\\');
                    current.push(c);
                }
            } else {
                // normal operation
                if (c == ' ' && !inquotes) {
                    if (current.length > 0) {
                        args.push(current.join(''));
                        current = [];
                    }
                } else if (c == '"') {
                    inquotes = !inquotes;
                } else if (c == "'") {
                    inquotes = !inquotes;
                } else {
                    current.push(c);
                }
            }
            escaped = false;
        }
    }
    if (current.length != 0) {
        args.push(current.join(''));
    }
    return args;
}
