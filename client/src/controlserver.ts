import { EventEmitter } from 'events';
import { Packet, PacketTypes } from 'c2lib';
import { forwardEvents } from 'c2lib';
import { Host } from './hosts';
import * as net from 'node:net';

interface ControlServerOptions {
    ip?: string;
    port?: number;
}

export declare interface ControlServer {
    on(event: 'connection', listener: (incomingHost: Host) => void): this;
    on(event: 'listening', listener: () => void): this;
    on(event: 'packet', listener: (packet: Packet) => void): this;
}

export class ControlServer extends EventEmitter {
    serverSocket: net.Server;
    ip: string;
    port: number;
    hostsList: Map<number, Host>;
    #nextHostID = 0;

    constructor(public packetTypes: PacketTypes, options: ControlServerOptions) {
        super();
        const defaultOpts = {
            ip: '0.0.0.0',
            port: 6996
        };
        let opts = { ...defaultOpts, ...options };
        this.ip = opts.ip;
        this.port = opts.port;
        this.hostsList = new Map();

        this.serverSocket = net.createServer((socket) => {
            let host = new Host(socket, this);
            let hostID = this.#nextHostID++;
            this.hostsList.set(hostID, host);
            host.on('close', () => {
                this.hostsList.delete(hostID);
            });
            this.emit('connection', host);
        });
        this.on('packet', (packet) => {
            switch (packet._ptype.name) {
                case 'out':
                    console.log(packet.out);
                case 'err':
                    console.error(packet.err);
            }
        });
        
        forwardEvents(['listening'], this.serverSocket, this);
    }

    listen() {
        return new Promise(resolve => {
            this.serverSocket.on('listening', resolve);
            this.serverSocket.listen(this.port, this.ip);
        });
    }

    /**
     * Abort any command currently waiting for a host response
     */
    abortCurrentCommand() {
        let abortPkt = this.packetTypes.createPacket('host', 'response', 'retcode');
        abortPkt.retcode = -999;
        this.emit('packet', abortPkt);
    }

    /**
     * Alias to create host command packet.
     */
    commandPacket(name: string) {
        return this.packetTypes.createPacket('host', 'command', name);
    }

    /**
     * Sends packet as a command to a given host.
     *
     * The passed-in callback co-handles any packets that arrive during the command,
     * and it is deregistered once a retcode is received. If it is null, out and err
     * packets are handled automatically.
     */
    async sendCommandToHost(hostID: number, packet: Packet): Promise<number> {
        let hostOrUndefined = this.hostsList.get(hostID);
        if (hostOrUndefined === undefined) {
            throw new Error('invalid host ID');
        }
        let host = hostOrUndefined;
        host.sendPacket(packet);
        return await this.ret();
    }

    /**
     * Sends packet to the currently selected host.
     * See `sendCommandToHost()` for more details.
     */
    async sendHostCommand(packet: Packet): Promise<number> {
        return await this.sendCommandToHost(0, packet);
    }

    waitForPacket(type: string): Promise<Packet> {
        return new Promise((resolve) => {
            let handler = (packet: Packet) => {
                if (packet._ptype.name === type) {
                    this.removeListener('packet', handler);
                    resolve(packet);
                }
            };
            this.on('packet', handler);
        });
    }

    ret(): Promise<number> {
        return this.waitForPacket('ret');
    }

    registerHostListener(callback: (packet: Packet) => void, hostID = 0) {
        let hostOrUndefined = this.hostsList.get(hostID)
        if (hostOrUndefined === undefined) {
            throw new Error('invalid host ID');
        }
        let host = hostOrUndefined;
        host.on('packet', callback);
    }

    removeHostListener(callback: (packet: Packet) => void, hostID = 0) {
        let hostOrUndefined = this.hostsList.get(hostID)
        if (hostOrUndefined === undefined) {
            throw new Error('invalid host ID');
        }
        let host = hostOrUndefined;
        host.removeListener('packet', callback);
    }

}
