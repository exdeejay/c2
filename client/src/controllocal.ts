import { forwardEvents } from '../../common/src/util';
import hosts = require('../../srv/src/hosts');
import { ControlBase } from './controlbase';

export class ControlLocal extends ControlBase {
    hostServer: hosts.HostServer;

    constructor(host: string, port: number) {
        super();
        this.hostServer = hosts.createServer(host, port);
        forwardEvents(['listening', 'connection'], this.hostServer, this);
    }
}

export function createServer(host: string, port: number, callback: any): ControlLocal {
    let localServer = new ControlLocal(host, port);
    if (callback !== undefined) {
        localServer.on('listening', callback);
    }
    return localServer;
}