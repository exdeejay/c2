import fs = require('fs/promises');
import { FileDataType } from './datatypes/filedata';
import { BigUintType, BufferType, ByteType, CharType, IntType, StringType, UintType } from './datatypes/primitive';

type Category = 'host' | 'control';
type Direction = 'command' | 'response';

export class DataType<T> {
    constructor(
        public name: string,
        public parse: (buf: Buffer) => [number, T],
        public serialize: (data: T) => Buffer
    ) {}
}

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
    dataTypes: Map<string, DataType<any>>;

    constructor(packetDir: string = `${__dirname}/../packets`) {
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
        this.dataTypes = new Map([
            CharType,
            ByteType,
            IntType,
            UintType,
            BigUintType,
            StringType,
            BufferType,
            FileDataType,
        ].map(t => [t.name, t]));
    }

    async load() {
        await this.readPacketTypesIntoMap('host', 'command', this.packetTypes.host.command);
        await this.readPacketTypesIntoMap('host', 'response', this.packetTypes.host.response);
        await this.readPacketTypesIntoMap('control', 'command', this.packetTypes.control.command);
        await this.readPacketTypesIntoMap('control', 'response', this.packetTypes.control.response);
        this.loaded = true;
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
        buffer = buffer.subarray(1);
    
        let packetType = null;
        for (let val of this.packetTypes[category][direction].values()) {
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
    
        for (let [name, type] of Object.entries(packetType.data)) {
            let [size, value] = this.parseField(buffer, type);
            packet[name] = value;
            buffer = buffer.subarray(size);
        }
    
        if (offset != buffer.length) {
            throw "parsed packet didn't match buffer length";
        }
    
        return packet;
    }

    parseField(buf: Buffer, type: string): [number, any] {
        if (type.startsWith('[') && type.endsWith(']')) {
            type = type.slice(1, -1);
            let totalSize = 0;
            let [size, length] = UintType.parse(buf);
            buf = buf.subarray(size);
            totalSize += size;

            let objects = [];
            for (let i = 0; i < length; i++) {
                let [size, val] = this.parseField(buf, type);
                buf = buf.subarray(size);
                totalSize += size;
                objects.push(val);
            }

            return [totalSize, objects];
        } else {
            let parser = this.dataTypes.get(type);
            if (parser === undefined) {
                throw new Error(`invalid type: ${type}`);
            }
            return parser!.parse(buf);
        }
    }
    
    serializePacket(packet: any): Buffer {
        let outBuf = Buffer.from([packet._ptype.index]);
        for (let [name, type] of Object.entries<string>(packet._ptype.data)) {
            outBuf = Buffer.concat([outBuf, this.serializeField(packet[name], type)]);
        }
        return outBuf;
    }

    serializeField(data: any, type: string): Buffer {
        if (type.startsWith('[') && type.endsWith(']')) {
            if (!Array.isArray(data)) {
                throw new Error(`expecting array on type: ${type}`);
            }
            type = type.slice(1, -1);
            let outBuf = UintType.serialize(data.length);
            for (let elem of data) {
                outBuf = Buffer.concat([outBuf, this.serializeField(elem, type)]);
            }
            return outBuf;
        } else {
            let serializer = this.dataTypes.get(type);
            if (serializer === undefined) {
                throw new Error(`invalid type: ${type}`);
            }
            //TODO: this is very inefficient lol
            return serializer!.serialize(data);
        }
    }
}
