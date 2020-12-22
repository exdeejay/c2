const EventEmitter = require('events');
const { packet_types } = require('../lib/protocol');
const { serializePacket, parsePacket } = require('../lib/packet');
const { PacketConnection } = require('../lib/connection');
const { once, emit } = require('process');

class Server extends EventEmitter {
    constructor(socket) {
        this.connection = new PacketConnection(socket);
        this.connection.on('packet', (data) => {
            let packet = parsePacket('control', 'response', data);
            this.handleResponse(packet);
        });
        socket.on('close', () => {
            console.log('Connection to host closed.');
        });
    }

    handleResponse(packet) {
        if (packet.type.name == 'hostresponse') {
            emit('hostresponse', packet);
        }
    }

    async sendHostCommand(packet) {
        let wrappedPkt = {
            type: packet_types.control.command.find(
                (pkt) => pkt.name == 'relaycommand'
            ),
            data: JSON.stringify(packet),
        };
        return new Promise((resolve) => {
            sendCommand(wrappedPkt).then((response) => {
                resolve(response.data);
            });
        });
    }

    async sendCommand(packet) {
        let buf = serializePacket('control', 'command', packet);
        return new Promise((resolve) => {
            once('hostresponse', resolve);
            this.connection.socket.write(buf);
        });
    }
}

/**
 * @type Server
 */
exports.connectedServer = null;
