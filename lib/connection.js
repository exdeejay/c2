const { Socket } = require('net');
const EventEmitter = require('events');

class PacketConnection extends EventEmitter {
    /**
     * @param {Socket} socket
     */
    constructor(socket) {
        super();
        this.socket = socket;
        this.buffer = null;
        this.nextPacketSize = -1;
        this.socket.on('data', this.handleData.bind(this));
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
            }

            if (data.length >= this.nextPacketSize + 4) {
                let packet = data.slice(0, this.nextPacketSize + 4);
                data = data.slice(this.nextPacketSize + 4);
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
