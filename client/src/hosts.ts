import net = require('net');
import { ZlibConnection } from 'c2lib';
import { EventEmitter } from 'stream';
import { forwardEvents } from 'c2lib';
import { Packet } from 'c2lib';
import { ControlServer } from './controlserver';

export class Host extends EventEmitter {
    ip: string;
    connection: ZlibConnection;

    constructor(socket: net.Socket, public controlServer: ControlServer) {
        super();
        this.ip = socket.remoteAddress!;
        this.connection = new ZlibConnection(socket);
        this.connection.on('data', (data) => {
            let packet = this.controlServer.packetTypes.parsePacket('host', 'response', data);
            this.emit('packet', packet);
        });
        forwardEvents(['close', 'error'], this.connection, this);
    }

    /**
     * Send packet to this host
     */
    sendPacket(packet: Packet) {
        this.connection.write(this.controlServer.packetTypes.serializePacket(packet));
    }
}
