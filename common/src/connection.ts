import zlib = require('zlib');
import { Socket } from 'net';
import EventEmitter = require('events');

export class ZlibConnection extends EventEmitter {
    _socket: Socket;
    buffer: Buffer;
    nextPacketSize: number;

    constructor(socket: Socket) {
        super();
        this._socket = socket;
        this.buffer = null;
        this.nextPacketSize = -1;
        this._socket.on('data', this.handleData.bind(this));
    }

    socket(): Socket {
        return this._socket;
    }

    override on(eventName: string, listener: any) {
        if (eventName == 'data') {
            super.on(eventName, listener);
        } else {
            this._socket.on(eventName, listener);
        }
    }

    /**
     * @event data
     * @type {Buffer}
     */
    
    handleData(data: Buffer) {
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
     */
    write(data: Buffer) {
        let compressed = zlib.deflateSync(data);
        let lengthBuf = Buffer.alloc(4);
        lengthBuf.writeUInt32BE(compressed.length);
        let finalBuf = Buffer.concat([lengthBuf, compressed]);
        this._socket.write(finalBuf);
    }
}
