import net = require('net');
import { ZlibConnection } from '../../common/src/connection';
import { HostProxy } from './hostproxy';
import { ControlBase } from './controlbase';
import { forwardEvents } from '../../common/src/util';

export class ControlConnection extends ControlBase {
    ip: string;
    connection: ZlibConnection;

    constructor(ip: string, port: number) {
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
     */
    handlePacket(packet: any) {
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
                if (remoteHost == null) {
                    throw new Error('invalid host ID');
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
     */
    sendControlPacket(packet: any) {
        this.connection.write(Buffer.from(JSON.stringify(packet)));
    }
}
