const { packet_types } = require('./protocol');

/**
 * @param {string} category
 * @param {string} direction
 * @param {string} name
 */
function createPacket(category, direction, name) {
    if (!(category in packet_types)) {
        throw 'invalid packet category';
    }
    if (!(direction in packet_types[category])) {
        throw 'invalid packet direction';
    }


    let type = packet_types[category][direction].find(
        (pkt) => pkt.name == name
    );
    if (type == null) {
        throw 'invalid packet name';
    }

    return { type };
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
        throw 'invalid packet category';
    }
    if (!(direction in packet_types[category])) {
        throw 'invalid packet direction';
    }
    if (buffer == null || buffer.length == 0) {
        throw 'empty buffer';
    }

    let offset = 0;
    let commandNum = buffer.readUInt8(offset);
    offset += 1;
    if (commandNum >= packet_types[category][direction].length) {
        throw 'command not specified';
    }

    let packetType = packet_types[category][direction][commandNum];
    let packet = {
        type: packetType,
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
function serializePacket(category, direction, packet) {
    if (!(category in packet_types)) {
        throw 'invalid packet category';
    }
    if (!(direction in packet_types[category])) {
        throw 'invalid packet direction';
    }

    let bytesArr = [ packet.type.index ];
    for (let field in packet.type.data) {
        serializeField(bytesArr, packet[field], packet.type.data[field]);
    }

    return Buffer.from(bytesArr);
}

exports.createPacket = createPacket;
exports.parsePacket = parsePacket;
exports.serializePacket = serializePacket;
