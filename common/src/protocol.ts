import fs = require('fs/promises');

let loaded = false;
const host_command = new Map();
const host_response = new Map();
const control_command = new Map();
const control_response = new Map();

type Category = 'host' | 'control';
type Direction = 'command' | 'response';

export interface Packet {
    [key: string]: any;
    _ptype: PacketType;
}

export interface PacketType {
    index: number;
    name: string;
    category: Category;
    direction: Direction;
    data: { [key: string]: string };
}

interface DuplexPacketTypes {
    command: Map<string, PacketType>;
    response: Map<string, PacketType>;
}

interface HostControlPacketTypes {
    host: DuplexPacketTypes;
    control: DuplexPacketTypes;
}

export class PacketTypes {
    loaded: boolean;
    packetDir: string;
    packetTypes: HostControlPacketTypes;

    constructor(packetDir: string = `${__dirname}/packets`) {
        this.loaded = false;
        this.packetDir = packetDir;
        this.packetTypes = {
            host: {
                command: new Map(),
                response: new Map(),
            },
            control: {
                command: new Map(),
                response: new Map(),
            },
        };
    }

    async load() {
        await this.readPacketTypesIntoMap('host', 'command', this.packetTypes.host.command);
        await this.readPacketTypesIntoMap('host', 'response', this.packetTypes.host.response);
        await this.readPacketTypesIntoMap('control', 'command', this.packetTypes.control.command);
        await this.readPacketTypesIntoMap('control', 'response', this.packetTypes.control.response);
        loaded = true;
    }

    async readPacketTypesIntoMap(category: Category, direction: Direction, map: Map<string, PacketType>) {
        let dirName = `${this.packetDir}/${category}/${direction}`;
        let files = await fs.readdir(dirName);
        for (const file of files) {
            let packetType: PacketType = require(`${dirName}/${file}`);
            packetType.category = category;
            packetType.direction = direction
            map.set(packetType.name, packetType);
        }
    }

    createPacket(category: Category, direction: Direction, name: string): Packet {
        if (!(category in this.packetTypes)) {
            throw new Error('invalid packet category');
        }
        if (!(direction in this.packetTypes[category])) {
            throw new Error('invalid packet direction');
        }
    
        let ptypeOrUndefined = this.packetTypes[category][direction].get(name);
        if (ptypeOrUndefined === undefined) {
            throw new Error('invalid packet name');
        }
        let ptype = ptypeOrUndefined;
        return { _ptype: ptype };
    }
    
    static parseField(buffer: Buffer, offset: number, packetObj: any, field: string, type: string): any {
        if (type.startsWith('[') && type.endsWith(']')) {
            let startOffset = offset;
            let subtype = type.slice(1, type.length - 1);
            let length = buffer.readUInt32BE(offset);
            offset += 4;
            let arr = [];
            for (let i = 0; i < length; i++) {
                let parsed = { '0': null };
                offset += PacketTypes.parseField(buffer, offset, parsed, '0', subtype);
                arr.push(parsed['0']);
            }
            packetObj[field] = arr;
            return offset - startOffset;
        } else {
            switch (type) {
                case 'char':
                    packetObj[field] = buffer.readInt8(offset);
                    return 1;
    
                case 'byte':
                    packetObj[field] = buffer.readUInt8(offset);
                    return 1;
    
                case 'int':
                    packetObj[field] = buffer.readInt32BE(offset);
                    return 4;
    
                case 'uint':
                    packetObj[field] = buffer.readUInt32BE(offset);
                    return 4;
    
                case 'str':
                    let strsize = buffer.readUInt32BE(offset);
                    packetObj[field] = buffer
                        .slice(offset + 4, offset + 4 + strsize)
                        .toString();
                    return strsize + 4;
    
                case 'buf':
                    let size = buffer.readUInt32BE(offset);
                    packetObj[field] = buffer
                        .slice(offset + 4, offset + 4 + size)
                        .toString('base64');
                    return size + 4;
            }
        }
    }
    
    parsePacket(category: Category, direction: Direction, buffer: Buffer): Packet {
        if (!(category in this.packetTypes)) {
            throw new Error('invalid packet category');
        }
        if (!(direction in this.packetTypes[category])) {
            throw new Error('invalid packet direction');
        }
        if (buffer == null || buffer.length == 0) {
            throw new Error('empty buffer');
        }
    
        let offset = 0;
        let commandNum = buffer.readUInt8(offset);
        offset += 1;
    
        let packetType = null;
        for (let [key, val] of this.packetTypes[category][direction]) {
            if (val.index == commandNum) {
                packetType = val;
                break;
            }
        }
        if (packetType == null) {
            throw new Error('command not specified');
        }
    
        let packet: Packet = {
            _ptype: packetType,
        };
    
        for (let field in packetType.data) {
            offset += PacketTypes.parseField(
                buffer,
                offset,
                packet,
                field,
                packetType.data[field]
            );
        }
    
        if (offset != buffer.length) {
            throw "parsed packet didn't match buffer length";
        }
    
        return packet;
    }
    
    static serializeField(bytesArr: number[], obj: any, type: string) {
        let buf = null;
        let len;
    
        if (type.startsWith('[') && type.endsWith(']')) {
            let subtype = type.slice(1, type.length - 1);
            buf = Buffer.alloc(4);
            buf.writeUInt32BE(obj.length);
            for (let i = 0; i < buf.length; i++) {
                bytesArr.push(buf[i]);
            }
            for (let item of obj) {
                PacketTypes.serializeField(bytesArr, item, subtype);
            }
        } else {
            switch (type) {
                case 'char':
                    buf = Buffer.alloc(1);
                    buf.writeInt8(obj);
                    break;
    
                case 'byte':
                    buf = Buffer.alloc(1);
                    buf.writeUInt8(obj);
                    break;
    
                case 'int':
                    buf = Buffer.alloc(4);
                    buf.writeInt32BE(obj);
                    break;
    
                case 'uint':
                    buf = Buffer.alloc(4);
                    buf.writeUInt32BE(obj);
                    break;
    
                case 'str':
                    len = Buffer.alloc(4);
                    len.writeUInt32BE(obj.length);
                    buf = Buffer.concat([len, Buffer.from(obj)]);
                    break;
    
                case 'buf':
                    len = Buffer.alloc(4);
                    let unb64 = Buffer.from(obj, 'base64');
                    len.writeUInt32BE(unb64.length);
                    buf = Buffer.concat([len, unb64]);
                    break;
    
                default:
                    throw new Error('invalid packet type');
            }
            for (let i = 0; i < buf.length; i++) {
                bytesArr.push(buf[i]);
            }
        }
    }
    
    serializePacket(packet: any): Buffer {
        let bytesArr = [packet._ptype.index];
        for (let field in packet._ptype.data) {
            PacketTypes.serializeField(bytesArr, packet[field], packet._ptype.data[field]);
        }
    
        return Buffer.from(bytesArr);
    }
}
