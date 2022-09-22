const net = require('net');
const { EventEmitter } = require('stream');
const { forwardEvents } = require('../common/util');
const { ZlibConnection } = require('../common/connection');
const { createPacket } = require('../common/packet');

class Controller extends EventEmitter {
    /**
     * @param {net.Socket} socket
     */
    constructor(socket) {
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
     * @param {*} packet
     */
    handlePacket(packet) {
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
     * @param {*} packet
     */
    sendPacket(packet) {
        this.connection.write(JSON.stringify(packet));
    }
}

/**
 * Wrap error in packet for sending to host
 * @param {Error} err
 */
function wrapErrorPacket(err) {
    let errPacket = createPacket('control', 'response', 'servererror');
    errPacket.message = err.toString();
    return errPacket; //TODO
}

class ControlServer extends EventEmitter {
    constructor(host, port) {
        super();
        this.serverSocket = net.createServer(this.handleConnection.bind(this));
        this.serverSocket.listen(port, host);
        forwardEvents(['listening'], this.serverSocket, this);
    }

    /**
     * @event connection
     * @type {Controller}
     */
    /**
     * @param {net.Socket} socket
     */
    handleConnection(socket) {
        let controller = new Controller(socket);
        this.emit('connection', controller);
    }
}

function createServer(host, port, callback) {
    let controlServer = new ControlServer(host, port);
    if (callback !== undefined) {
        controlServer.on('listening', callback);
    }
    return controlServer;
}

exports.createServer = createServer;
exports.ControlServer = ControlServer;
exports.Controller = Controller;
