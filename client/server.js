const EventEmitter = require('events');
const { packet_types } = require('../protocol/protocol');
const { createPacket } = require('../protocol/packet');
const { PacketConnection } = require('../protocol/connection');


class Server extends EventEmitter {
    constructor(socket) {
        super();
        this.connection = new PacketConnection(socket);
        this.connection.on('packet', (data) => {
            this.handleResponse(JSON.parse(data.toString()));
        });
        socket.on('close', () => {
            console.log('Connection to host closed.');
        });
        Server.connectedServer = this; //TODO: this sucks
    }

    /**
     * Close server connection.
     */
    close() {
        this.connection.socket.destroy();
    }

    /**
     * Abort any command currently waiting for a host response
     */
    abort() {
        let abortPkt = createPacket('host', 'response', 'retcode');
        abortPkt.retcode = -999;
        this.emit('hostpacket', abortPkt);
    }

    /**
     * Handle response packets from server
     * @param {*} packet
     */
    handleResponse(packet) {
        switch (packet['_ptype'].name) {
            case 'servererror':
                console.log(`ERROR: Server encountered error (${packet.message})`);
                this.abort();
                break;
            case 'newpwn':
                console.log(`\n[!] Pwned ${packet.ip}`);
                break;
            case 'hostresponse':
                this.emit('hostpacket', packet.data);
                break;
            default:
                //TODO: handle invalid packet type
                break;
        }
    }

    /**
     * Alias to create host command packet.
     * @param {string} name 
     */
    commandPacket(name) {
        return createPacket('host', 'command', name);
    }

    /**
     * @callback hostCmdCallback
     * @param {*} response
     */
    /** 
     * Sends packet as a command to a given host.
     * 
     * The passed-in callback co-handles any packets that arrive during the command,
     * and it is deregistered once a retcode is received. If it is null, out and err
     * packets are handled automatically.
     * 
     * @param {number} hostID
     * @param {*} packet
     * @param {hostCmdCallback} callback 
     * @returns {number} retcode from host
     */
    async sendCommandToHost(hostID, packet, callback) {
        let wrappedPkt = createPacket('control', 'command', 'relaycommand');
        wrappedPkt.id = hostID;
        wrappedPkt.command = packet;
        
        return new Promise((resolve) => {
            let handler = (responsePkt) => {
                if (responsePkt._ptype.name == 'retcode') {
                    this.removeListener('hostpacket', handler);
                    resolve(responsePkt.code);
                } else {
                    if (callback == null) {
                        if (responsePkt._ptype.name == 'out') {
                            process.stdout.write(responsePkt.out);
                        } else if (responsePkt._ptype.name == 'err') {
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

    /** 
     * Sends packet to the currently selected host.
     * See `sendCommandToHost()` for more details.
     * 
     * @param {*} packet
     * @param {hostCmdCallback} callback 
     * @returns {number} retcode from host
     */
    async sendHostCommand(packet, callback) {
        await this.sendCommandToHost(0, packet, callback);
    }

    /**
     * Send packet to this server.
     * @param {*} packet
     */
    sendCommand(packet) {
        this.connection.write(JSON.stringify(packet));
    }
}

module.exports = Server;
