const { packet_types } = require('./protocol');

/**
 * @param {string} category
 * @param {string} direction
 * @param {string} name
 */
function createPacket(category, direction, name) {
    if (!(category in packet_types)) {
        throw new Error('invalid packet category');
    }
    if (!(direction in packet_types[category])) {
        throw new Error('invalid packet direction');
    }

    if (!packet_types[category][direction].has(name)) {
        throw new Error('invalid packet name');
    }

    return { _ptype: packet_types[category][direction].get(name) };
}

/**
 * @param {Buffer} buffer
 * @param {number} offset
 * @param {Object} packetObj
 * @param {string} type
 */
function parseField(buffer, offset, packetObj, field, type) {
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
            let ret = parseField(buffer, offset, packetObj, field, 'buf');
            packetObj[field] = packetObj[field].toString();
            return ret;

        case 'buf':
            let size = buffer.readUInt32BE(offset);
            packetObj[field] = buffer.slice(offset + 4, offset + 4 + size);
            return size + 4;
    }
}

/**
 * @param {Buffer} buffer
 * @param {string} category
 * @param {string} direction
 */
function parsePacket(category, direction, buffer) {
    if (!(category in packet_types)) {
        throw new Error('invalid packet category');
    }
    if (!(direction in packet_types[category])) {
        throw new Error('invalid packet direction');
    }
    if (buffer == null || buffer.length == 0) {
        throw new Error('empty buffer');
    }

    let offset = 0;
    let commandNum = buffer.readUInt8(offset);
    offset += 1;

    let packetType = null;
    for (let [key, val] of packet_types[category][direction]) {
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
        offset += parseField(
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

/**
 * @param {number[]} bytesArr
 * @param {*} obj
 * @param {string} type
 */
function serializeField(bytesArr, obj, type) {
    let buf = null;
    let len;

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
            len.writeUInt32BE(obj.length);
            buf = Buffer.concat([len, obj]);
            break;
    }
    
    for (let i = 0; i < buf.length; i++) {
        bytesArr.push(buf[i]);
    }
}

/**
 * @param {Buffer} buffer
 * @param {string} category
 * @param {string} direction
 */
function serializePacket(packet) {
    let bytesArr = [ packet._ptype.index ];
    for (let field in packet._ptype.data) {
        serializeField(bytesArr, packet[field], packet._ptype.data[field]);
    }

    return Buffer.from(bytesArr);
}

exports.createPacket = createPacket;
exports.parsePacket = parsePacket;
exports.serializePacket = serializePacket;
