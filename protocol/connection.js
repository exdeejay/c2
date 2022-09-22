const zlib = require('zlib');
const { Socket } = require('net');
const EventEmitter = require('events');

class ZlibConnection extends EventEmitter {
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
     * @event data
     * @type {Buffer}
     */
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
                let compressed = data.slice(0, this.nextPacketSize + 4);
                compressed = compressed.slice(4);
                data = data.slice(this.nextPacketSize + 4);
                let decompressed = zlib.inflateSync(compressed);
                this.emit('data', decompressed);
                this.nextPacketSize = -1;
            } else {
                this.buffer = Buffer.from(data);
                break;
            }
        }
    }

    /**
     * Write data to connection
     * @param {Buffer} data
     */
    write(data) {
        let compressed = zlib.deflateSync(data);
        let lengthBuf = Buffer.alloc(4);
        lengthBuf.writeUInt32BE(compressed.length);
        let finalBuf = Buffer.concat([lengthBuf, compressed]);
        this.socket.write(finalBuf);
    }
}

exports.ZlibConnection = ZlibConnection;
