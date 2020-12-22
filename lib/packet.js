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
        case 'int':
            packetObj[field] = buffer.readInt32BE(offset);
            return 4;

        case 'str':
            let ret = parseField('buf', offset, packetObj, field, type);
            packetObj[field] = packetObj[field].toString();
            return ret;

        case 'buf':
            let size = buffer.readUInt32BE(offset);
            packetObj[field] = Buffer.from(buffer, offset + 4, size);
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
    if (buffer == null || buffer.length < 4) {
        throw 'too small';
    }

    let offset = 0;
    let size = buffer.readUInt32BE();
    offset += 4;
    if (buffer.length - 4 != size) {
        throw 'wrong size';
    }

    let commandNum = buffer.readUInt8(offset);
    offset += 1;
    if (commandNum >= packet_types[category][direction]) {
        throw 'command not specified';
    }

    let packetType = packet_types[category][direction];
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
    switch (type) {
        case 'int':
            buf = Buffer.alloc(4);
            buf.writeInt32BE(obj);
            break;

        case 'str':
            let len = Buffer.alloc(4 + obj.length);
            len.writeUInt32BE(obj.length);
            buf = Buffer.concat([len, Buffer.from(obj)]);
            break;

        case 'buf':
            let len = Buffer.alloc(4 + obj.length);
            len.writeUInt32BE(obj.length);
            buf = Buffer.concat([len, obj]);
            break;
    }
    for (let b of buf) {
        bytesArr.push(b);
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

    let bytesArr = [];
    for (let field in packet.type.data) {
        serializeField(bytesArr, packet[field], packet.type.data[field]);
    }

    let len = Buffer.alloc(4);
    len.writeUInt32BE(bytesArr.length);
    let buffer = Buffer.from([len, Buffer.from(bytesArr)]);
    return buffer;
}

exports.createPacket = createPacket;
exports.parsePacket = parsePacket;
exports.serializePacket = serializePacket;
