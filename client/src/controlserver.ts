import { EventEmitter } from 'events';
import { Packet, PacketTypes } from 'c2lib';
import { forwardEvents } from 'c2lib';
import { Host } from './host';
import * as net from 'node:net';

interface ControlServerOptions {
    ip?: string;
    port?: number;
}

export declare interface ControlServer {
    on(event: 'connection', listener: (incomingHost: Host) => void): this;
    on(event: 'listening', listener: () => void): this;
}

export class ControlServer extends EventEmitter {
    serverSocket: net.Server;
    ip: string;
    port: number;
    hostsList: Map<number, Host>;
    currentHost: Host | null = null;
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

            host.on('packet', (packet) => {
                switch (packet._ptype.name) {
                    case 'out':
                        console.log(packet.out);
                        break;
                    case 'err':
                        console.error(packet.err);
                        break;
                }
            });
            if (this.currentHost === null) {
                this.currentHost = host;
            }
            host.on('close', () => {
                console.log(`${host.ip} disconnected`);
                if (this.currentHost === host) {
                    this.currentHost = null;
                }
                this.hostsList.delete(hostID);
            });
            host.on('error', (err) => {
                console.log(`Host error: ${err.name}: ${err.message}`);
            });
            this.emit('connection', host);
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
     * Alias to create host command packet.
     */
    commandPacket(name: string) {
        return this.packetTypes.createPacket('host', 'command', name);
    }

    /**
     * Sends packet to the currently selected host.
     * See `sendCommandToHost()` for more details.
     */
    async sendHostCommand(packet: Packet): Promise<number> {
        if (this.currentHost === null) {
            throw new Error('no current host');
        }
        this.currentHost.sendPacket(packet);
        return await this.ret();
    }

    async waitForPacket(type: string): Promise<Packet> {
        if (this.currentHost === null) {
            throw new Error('no current host');
        }
        return await this.currentHost.waitForPacket(type);
    }

    async ret(): Promise<number> {
        if (this.currentHost === null) {
            throw new Error('no current host');
        }
        return (await this.currentHost.waitForPacket('retcode')).code;
    }

}
