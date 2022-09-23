import net = require('net');
import { ZlibConnection } from '../../common/src/connection';
import { EventEmitter } from 'stream';
import { forwardEvents } from '../../common/src/util';

export class Host extends EventEmitter {
    ip: string | undefined;
    connection: ZlibConnection;

    constructor(socket: net.Socket) {
        super();
        this.ip = socket.remoteAddress;
        this.connection = new ZlibConnection(socket);
        this.connection.on('data', (data) => {
            let packet = parsePacket('host', 'response', data);
            this.emit('packet', packet);
        });
        forwardEvents(['close', 'error'], this.connection, this);
    }

    /**
     * Send packet to this host
     */
    sendPacket(packet: any) {
        this.connection.write(serializePacket(packet));
    }
}

export class HostServer extends EventEmitter {
    serverSocket: net.Server;

    constructor(host: string, port: number) {
        super();
        this.serverSocket = net.createServer((socket) => {
            let host = new Host(socket);
            this.emit('connection', host);
        });
        this.serverSocket.listen(port, host);
        forwardEvents(['listening'], this.serverSocket, this);
    }
}

export function createServer(host: string, port: number, callback: any) {
    let hostServer = new HostServer(host, port);
    if (callback !== undefined) {
        hostServer.on('listening', callback);
    }
    return hostServer;
}
