import { EventEmitter } from 'events';
import { createPacket } from '../../common/src/packet';

export class HostProxy extends EventEmitter {
    remoteID: number;
    ip: string;
    control: any;

    constructor(remoteID: number, ip: string, control: any) {
        super();
        this.remoteID = remoteID;
        this.ip = ip;
        this.control = control;
    }

    /**
     * Send packet to this host
     * @param {*} packet
     */
    sendPacket(packet) {
        let wrappedPkt = createPacket('control', 'command', 'relaycommand');
        wrappedPkt.id = this.remoteID;
        wrappedPkt.command = packet;
        this.control.sendControlPacket(wrappedPkt);
    }
}
