const net = require('net');
const { ZlibConnection } = require('../common/connection');
const { HostProxy } = require('./hostproxy');
const { ControlBase } = require('./controlbase');
const { forwardEvents } = require('../common/util');

class ControlConnection extends ControlBase {
    /**
     * @param {string} host
     * @param {number} port
     */
    constructor(ip, port) {
        super();
        this.ip = ip;
        this.connection = new ZlibConnection(net.connect(port, ip));
        this.connection.on('data', (data) => {
            this.handlePacket(JSON.parse(data.toString()));
        });
        forwardEvents(['close', 'error', 'timeout', 'connect'], this.connection, this);
    }

    /**
     * Handle response packets from server
     * @param {*} packet
     */
    handlePacket(packet) {
        this.emit('controlpacket', packet);
        switch (packet['_ptype'].name) {
            case 'servererror':
                console.log(`ERROR: Server encountered error (${packet.message})`);
                this.abortCurrentCommand();
                //this.emit('error', packet.message);
                break;
            case 'newpwn':
                let host = new HostProxy(packet.id, packet.ip, this);
                this.emit('connection', host);
                break;
            case 'hostresponse':
                //TODO: check host packet bounds
                let remoteHost = null;
                for (let h of this.hostsList.values()) {
                    if (h.remoteID == packet.id) {
                        remoteHost = h;
                        break;
                    }
                }
                remoteHost.emit('packet', packet.data);
                break;
            default:
                //TODO: handle invalid packet type
                break;
        }
    }

    /**
     * Send packet to this connection.
     * @param {*} packet
     */
    sendControlPacket(packet) {
        this.connection.write(JSON.stringify(packet));
    }
}

exports.ControlConnection = ControlConnection;
