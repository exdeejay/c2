const { forwardEvents } = require('../common/util');
const hosts = require('../srv/hosts');
const { ControlBase } = require('./controlbase');

class ControlLocal extends ControlBase {
    /**
     * @param {string} host
     * @param {number} port
     */
    constructor(host, port) {
        super();
        this.hostServer = hosts.createServer(host, port);
        forwardEvents(['listening', 'connection'], this.hostServer, this);
    }
}

function createServer(host, port, callback) {
    let localServer = new ControlLocal(host, port);
    if (callback !== undefined) {
        localServer.on('listening', callback);
    }
    return localServer;
}

exports.createServer = createServer;
exports.ControlLocal = ControlLocal;
