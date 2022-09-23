import net = require('net');
import { EventEmitter } from 'stream';
import { forwardEvents } from '../../common/src/util';
import { ZlibConnection } from '../../common/src/connection';

export class Controller extends EventEmitter {
    ip: string | undefined;
    connection: ZlibConnection;

    constructor(socket: net.Socket) {
        super();
        this.ip = socket.remoteAddress;
        this.connection = new ZlibConnection(socket);
        this.connection.on('data', (data) => {
            //TODO: validate json-parsed packet
            this.handlePacket(JSON.parse(data.toString()));
        });
        forwardEvents(['close', 'error'], this.connection, this);
    }

    /**
     * Handle command received from controller
     */
    handlePacket(packet: any) {
        this.emit('packet', packet);
        switch (packet._ptype.name) {
            case 'relaycommand':
                this.emit('hostcommand', packet.id, packet.command);
                break;
            default:
                //TODO: handle invalid packet type
                break;
        }
    }

    /**
     * Send packet to this controller
     */
    sendPacket(packet: any) {
        this.connection.write(Buffer.from(JSON.stringify(packet)));
    }
}

/**
 * Wrap error in packet for sending to host
 */
function wrapErrorPacket(err: Error) {
    let errPacket = createPacket('control', 'response', 'servererror');
    errPacket.message = err.toString();
    return errPacket; //TODO
}

export class ControlServer extends EventEmitter {
    serverSocket: net.Server;

    constructor(host: string, port: number) {
        super();
        this.serverSocket = net.createServer(this.handleConnection.bind(this));
        this.serverSocket.listen(port, host);
        forwardEvents(['listening'], this.serverSocket, this);
    }

    /**
     * @event connection
     * @type {Controller}
     */

    handleConnection(socket: net.Socket) {
        let controller = new Controller(socket);
        this.emit('connection', controller);
    }
}

export function createServer(host: string, port: number, callback: any) {
    let controlServer = new ControlServer(host, port);
    if (callback !== undefined) {
        controlServer.on('listening', callback);
    }
    return controlServer;
}
