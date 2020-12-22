const { Socket } = require('net');
const EventEmitter = require('events');

class PacketConnection extends EventEmitter {
    /**
     * @param {Socket} socket
     */
    constructor(socket) {
        this.socket = socket;
        this.buffer = null;
        this.socket.on('data', this.handleData);
    }

    /**
     * @param {Buffer} data
     */
    handleData(data) {
        if (this.buffer) {
            data = Buffer.concat([this.buffer, data]);
            this.buffer = null;
        }
        while (data.length > 0) {
            if (data.length >= 4 && this.nextPacketSize == -1) {
                this.nextPacketSize = data.readUInt32BE();
                data = data.slice(4);
            }
            if (data.length - 4 >= this.nextPacketSize) {
                let packet = Buffer.from(data, 0, this.nextPacketSize);
                data = data.slice(this.nextPacketSize);
                this.emit('packet', packet);
                this.nextPacketSize = -1;
            } else {
                this.buffer = Buffer.from(data);
                break;
            }
        }
    }
}

exports.PacketConnection = PacketConnection;
