const { EventEmitter } = require('events');
const { createPacket } = require('../common/packet');

class ControlBase extends EventEmitter {
    constructor() {
        super();
        this.hostsList = new Map();
        this.nextHostID = 0;
        this.on('connection', (host) => {
            let hostID = this.nextHostID++;
            this.hostsList.set(hostID, host);
            host.on('close', () => {
                this.hostsList.delete(hostID);
            });
        });
    }

    /**
     * Abort any command currently waiting for a host response
     */
    abortCurrentCommand() {
        let abortPkt = createPacket('host', 'response', 'retcode');
        abortPkt.retcode = -999;
        this.emit('packet', abortPkt);
    }

    /**
     * Alias to create host command packet.
     * @param {string} name
     */
    commandPacket(name) {
        return createPacket('host', 'command', name);
    }

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
        if (!this.hostsList.has(hostID)) {
            throw new Error('invalid host ID');
        }
        let host = this.hostsList.get(hostID);
        return new Promise((resolve) => {
            let handler = (responsePkt) => {
                if (responsePkt._ptype.name == 'retcode') {
                    host.removeListener('packet', handler);
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
            host.on('packet', handler);
            host.sendPacket(packet);
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
        return await this.sendCommandToHost(0, packet, callback);
    }

    registerHostListener(callback, hostID) {
        if (hostID === undefined) {
            hostID = 0;
        }
        //TODO: bounds checking
        this.hostsList.get(hostID).on('packet', callback);
    }

    removeHostListener(callback, hostID) {
        if (hostID === undefined) {
            hostID = 0;
        }
        //TODO: bounds checking
        this.hostsList.get(hostID).removeListener('packet', callback);
    }
}

exports.ControlBase = ControlBase;
