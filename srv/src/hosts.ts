import net = require('net');
import { ZlibConnection } from '../../common/src/connection';
import { EventEmitter } from 'stream';
import { forwardEvents } from '../../common/src/util';
import { PacketTypes } from '../../common/src/protocol';

export class Host extends EventEmitter {
    ip: string;
    connection: ZlibConnection;

    constructor(socket: net.Socket, public packetTypes: PacketTypes) {
        super();
        this.ip = socket.remoteAddress!;
        this.connection = new ZlibConnection(socket);
        this.connection.on('data', (data) => {
            let packet = this.packetTypes.parsePacket('host', 'response', data);
            this.emit('packet', packet);
        });
        forwardEvents(['close', 'error'], this.connection, this);
    }

    /**
     * Send packet to this host
     */
    sendPacket(packet: any) {
        this.connection.write(this.packetTypes.serializePacket(packet));
    }
}

export class HostServer extends EventEmitter {
    serverSocket: net.Server;

    constructor(host: string, port: number, public packetTypes: PacketTypes) {
        super();
        this.serverSocket = net.createServer((socket) => {
            let host = new Host(socket, this.packetTypes);
            this.emit('connection', host);
        });
        this.serverSocket.listen(port, host);
        forwardEvents(['listening'], this.serverSocket, this);
    }

    static createServer(host: string, port: number, callback: any, packetTypes: PacketTypes) {
        let hostServer = new HostServer(host, port, packetTypes);
        if (callback !== undefined) {
            hostServer.on('listening', callback);
        }
        return hostServer;
    }
}
