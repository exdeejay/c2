const { EventEmitter } = require('events');
const { createPacket } = require('../common/packet');

class HostProxy extends EventEmitter {
    constructor(remoteID, ip, control) {
        super();
        this.remoteID = remoteID;
        this.ip = ip;
        this.control = control;
    }

    /**
     * Send packet to this host
     * @param {*} packet
     */
    sendPacket(packet) {
        let wrappedPkt = createPacket('control', 'command', 'relaycommand');
        wrappedPkt.id = this.remoteID;
        wrappedPkt.command = packet;
        this.control.sendControlPacket(wrappedPkt);
    }
}

exports.HostProxy = HostProxy;
