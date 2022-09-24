"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.PacketTypes = void 0;
const fs = require("fs/promises");
class PacketTypes {
    constructor(packetDir = `${__dirname}/packets`) {
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
    load() {
        return __awaiter(this, void 0, void 0, function* () {
            yield this.readPacketTypesIntoMap('host', 'command', this.packetTypes.host.command);
            yield this.readPacketTypesIntoMap('host', 'response', this.packetTypes.host.response);
            yield this.readPacketTypesIntoMap('control', 'command', this.packetTypes.control.command);
            yield this.readPacketTypesIntoMap('control', 'response', this.packetTypes.control.response);
            this.loaded = true;
        });
    }
    readPacketTypesIntoMap(category, direction, map) {
        return __awaiter(this, void 0, void 0, function* () {
            let dirName = `${this.packetDir}/${category}/${direction}`;
            let files = yield fs.readdir(dirName);
            for (const file of files) {
                let packetType = require(`${dirName}/${file}`);
                packetType.category = category;
                packetType.direction = direction;
                map.set(packetType.name, packetType);
            }
        });
    }
    createPacket(category, direction, name) {
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
    static parseField(buffer, offset, packetObj, field, type) {
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
        }
        else {
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
    parsePacket(category, direction, buffer) {
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
        let packet = {
            _ptype: packetType,
        };
        for (let field in packetType.data) {
            offset += PacketTypes.parseField(buffer, offset, packet, field, packetType.data[field]);
        }
        if (offset != buffer.length) {
            throw "parsed packet didn't match buffer length";
        }
        return packet;
    }
    static serializeField(bytesArr, obj, type) {
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
        }
        else {
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
    serializePacket(packet) {
        let bytesArr = [packet._ptype.index];
        for (let field in packet._ptype.data) {
            PacketTypes.serializeField(bytesArr, packet[field], packet._ptype.data[field]);
        }
        return Buffer.from(bytesArr);
    }
}
exports.PacketTypes = PacketTypes;
