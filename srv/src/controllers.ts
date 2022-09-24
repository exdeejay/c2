import net = require('net');
import { EventEmitter } from 'stream';
import { forwardEvents } from '../../common/src/util';
import { ZlibConnection } from '../../common/src/connection';
import { PacketTypes } from '../../common/src/protocol';

export class RemoteController extends EventEmitter {
	ip: string;
	connection: ZlibConnection;

	constructor(socket: net.Socket, public packetTypes: PacketTypes) {
		super();
		this.ip = socket.remoteAddress!;
		this.connection = new ZlibConnection(socket);
		this.connection.on('data', (data) => {
			//TODO: validate json-parsed packet
			this.handlePacket(JSON.parse(data.toString()));
		});
		forwardEvents(['close', 'error'], this.connection, this);
	}

	/**
	 * Handle command received from controller
	 */
	handlePacket(packet: any) {
		this.emit('packet', packet);
		switch (packet._ptype.name) {
			case 'relaycommand':
				this.emit('hostcommand', packet.id, packet.command);
				break;
			default:
				//TODO: handle invalid packet type
				break;
		}
	}
	
	/**
	 * Wrap error in packet for sending to host
	 */
	wrapErrorPacket(err: Error) {
		let errPacket = this.packetTypes.createPacket('control', 'response', 'servererror');
		errPacket.message = err.toString();
		return errPacket; //TODO
	}

	/**
	 * Send packet to this controller
	 */
	sendPacket(packet: any) {
		this.connection.write(Buffer.from(JSON.stringify(packet)));
	}
}


export class ControlServer extends EventEmitter {
	serverSocket: net.Server;

	constructor(host: string, port: number, public packetTypes: PacketTypes) {
		super();
		this.serverSocket = net.createServer(this.handleConnection.bind(this));
		this.serverSocket.listen(port, host);
		forwardEvents(['listening'], this.serverSocket, this);
	}

	/**
	 * @event connection
	 * @type {RemoteController}
	 */

	handleConnection(socket: net.Socket) {
		let controller = new RemoteController(socket, this.packetTypes);
		this.emit('connection', controller);
	}

	static createServer(host: string, port: number, callback: any, packetTypes: PacketTypes) {
		let controlServer = new ControlServer(host, port, packetTypes);
		if (callback !== undefined) {
			controlServer.on('listening', callback);
		}
		return controlServer;
	}
}
