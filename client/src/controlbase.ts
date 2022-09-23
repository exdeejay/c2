import { EventEmitter } from 'events';
import { createPacket } from '../../common/src/packet';
import { Host } from '../../srv/src/hosts';

export class ControlBase extends EventEmitter {
    hostsList: Map<number, Host>;
    nextHostID: number;

    constructor() {
        super();
        this.hostsList = new Map();
        this.nextHostID = 0;
        this.on('connection', (host) => {
            let hostID = this.nextHostID++;
            this.hostsList.set(hostID, host);
            host.on('close', () => {
                this.hostsList.delete(hostID);
            });
        });
    }

    /**
     * Abort any command currently waiting for a host response
     */
    abortCurrentCommand() {
        let abortPkt = createPacket('host', 'response', 'retcode');
        abortPkt.retcode = -999;
        this.emit('packet', abortPkt);
    }

    /**
     * Alias to create host command packet.
     */
    commandPacket(name: string) {
        return createPacket('host', 'command', name);
    }

    /**
     * Sends packet as a command to a given host.
     *
     * The passed-in callback co-handles any packets that arrive during the command,
     * and it is deregistered once a retcode is received. If it is null, out and err
     * packets are handled automatically.
     */
    async sendCommandToHost(hostID: number, packet: any, callback: any): Promise<number> {
        let hostOrUndefined = this.hostsList.get(hostID);
        if (hostOrUndefined === undefined) {
            throw new Error('invalid host ID');
        }
        let host = hostOrUndefined;
        return new Promise((resolve) => {
            let handler = (responsePkt: any) => {
                if (responsePkt._ptype.name == 'retcode') {
                    host.removeListener('packet', handler);
                    resolve(responsePkt.code);
                } else {
                    if (callback == null) {
                        if (responsePkt._ptype.name == 'out') {
                            process.stdout.write(responsePkt.out);
                        } else if (responsePkt._ptype.name == 'err') {
                            console.error(`ERROR: ${responsePkt.err}`);
                        }
                    } else {
                        callback(responsePkt);
                    }
                }
            };
            host.on('packet', handler);
            host.sendPacket(packet);
        });
    }

    /**
     * Sends packet to the currently selected host.
     * See `sendCommandToHost()` for more details.
     */
    async sendHostCommand(packet: any, callback: any): Promise<number> {
        return await this.sendCommandToHost(0, packet, callback);
    }

    registerHostListener(callback: any, hostID: number) {
        if (hostID === undefined) {
            hostID = 0;
        }
        let hostOrUndefined = this.hostsList.get(hostID)
        if (hostOrUndefined === undefined) {
            throw new Error('invalid host ID');
        }
        let host = hostOrUndefined;
        host.on('packet', callback);
    }

    removeHostListener(callback: any, hostID: number) {
        if (hostID === undefined) {
            hostID = 0;
        }
        let hostOrUndefined = this.hostsList.get(hostID)
        if (hostOrUndefined === undefined) {
            throw new Error('invalid host ID');
        }
        let host = hostOrUndefined;
        host.removeListener('packet', callback);
    }
}
