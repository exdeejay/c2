const fs = require('fs/promises');
const internal = require('stream');

const host_command = new Map();
const host_response = new Map();
const control_command = new Map();
const control_response = new Map();

/**
 * @typedef {Object} IPacket
 * @property {number} index
 * @property {string} name
 * @property {string} category
 * @property {string} direction
 * @property {Object} data
 */

/**
 * @param {string} categoryName
 * @param {string} direction
 * @param {string} dirName
 * @param {Map<number, IPacket>} arr
 */
async function readPacketTypesIntoArray(categoryName, direction, arr) {
    let dirName = `${__dirname}/packets/${categoryName}/${direction}`;
    let files = await fs.readdir(dirName);
    for (const file of files) {
        let packetType = require(`${dirName}/${file}`);
        packetType.category = categoryName;
        packetType.direction = direction;
        arr.set(packetType.index, packetType);
    }
}

async function loadPacketTypes() {
    await readPacketTypesIntoArray(
        'host',
        'command',
        host_command
    );
    await readPacketTypesIntoArray(
        'host',
        'response',
        host_response
    );
    await readPacketTypesIntoArray(
        'control',
        'command',
        control_command
    );
    await readPacketTypesIntoArray(
        'control',
        'response',
        control_response
    );
}

/**
 * @typedef {Object} DuplexPacketTypes
 * @property {Map<number, IPacket>} command
 * @property {Map<number, IPacket>} response
 */

/**
 * @typedef {Object} HostControlPacketTypes
 * @property {DuplexPacketTypes} host
 * @property {DuplexPacketTypes} control
 */

/** @type {HostControlPacketTypes} */
exports.packet_types = {
    host: {
        command: host_command,
        response: host_response,
    },
    control: {
        command: control_command,
        response: control_response,
    },
};
exports.loadPacketTypes = loadPacketTypes;
