const { Socket } = require('net');
const { PacketConnection } = require('../lib/connection');
const { parsePacket, serializePacket } = require('../lib/packet');
const host = require('./host');

/**
 * @type Controller[]
 */
const controllers = [];
const nonces = {};
let commandID = 0;

class Controller {
    /**
     * @param {Socket} socket
     */
    constructor(socket) {
        this.connection = new PacketConnection(socket);
        this.connection.on('packet', (data) => {
            let packet = parsePacket('control', 'command', data);
            this.handleCommand(packet);
        });
    }

    handleCommand(packet) {
        switch (packet.type.name) {
            case 'relaycommand':
                host.hosts[packet.id].sendCommand(packet.command);
                break;
        }
    }

    sendResponse(response) {
        let buffer = serializePacket('control', 'response', response);
        this.connection.write(buffer);
    }
}

/**
 * @param {Socket} socket
 */
function handleIncomingController(socket) {
    let controller = new Controller(socket);
    let index = controllers.push(controller) - 1;
    socket.on('close', () => {
        console.log(`- Controller ${index} disconnected`);
        controllers.splice(index, 1);
    });
    socket.on('error', () => {
        console.log(`! Controller ${index} forcibly exited`);
    });
    console.log(`+ Now controlling the herd from ${socket.remoteAddress}`);
}

exports.Controller = Controller;
exports.handleIncomingController = handleIncomingController;
exports.controllers = controllers;
