const EventEmitter = require('events');
const { packet_types } = require('../lib/protocol');
const { serializePacket, parsePacket } = require('../lib/packet');
const { PacketConnection } = require('../lib/connection');
const { once, emit } = require('process');

class Server extends EventEmitter {
    constructor(socket) {
        super();
        this.connection = new PacketConnection(socket);
        this.connection.on('packet', (data) => {
            try {
                let packet = parsePacket('control', 'response', data);
                this.handleResponse(packet);
            } catch (err) {
                console.error(err);
            }
        });
        socket.on('close', () => {
            console.log('Connection to host closed.');
        });
    }

    close() {
        this.connection.socket.destroy();
    }

    handleResponse(packet) {
        if (packet.type.name == 'hostresponse') {
            this.emit('hostpacket', parsePacket('host', 'response', packet.data));
        }
    }


    /**
     * @callback hostCmdCallback
     * @param {*} response
     */
    /** 
     * @param {*} packet 
     * @param {hostCmdCallback} callback 
     * @returns 
     */
    async sendHostCommand(packet, callback) {
        let wrappedPkt = {
            type: packet_types.control.command.find(
                (pkt) => pkt != null && pkt.name == 'relaycommand'
            ),
            id: 0, // TODO: fill in host id
            command: JSON.stringify(packet),
        };
        
        return new Promise((resolve) => {
            let handler = (responsePkt) => {
                if (responsePkt.type.name == 'retcode') {
                    this.removeListener('hostpacket', handler);
                    resolve(responsePkt.code);
                } else {
                    if (callback == null) {
                        if (responsePkt.type.name == 'out') {
                            console.log(responsePkt.out);
                        } else if (responsePkt.type.name == 'err') {
                            console.error(`ERROR: ${responsePkt.err}`);
                        }
                    } else {
                        callback(responsePkt);
                    }
                }
            };
            this.on('hostpacket', handler);
            this.sendCommand(wrappedPkt);
        });
    }

    sendCommand(packet) {
        let buf = serializePacket('control', 'command', packet);
        this.connection.socket.write(buf);
    }
}

/**
 * @type Server
 */
exports.connectedServer = null;
exports.Server = Server;
