const fs = require('fs/promises');

let loaded = false;
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
 * @param {Map<string, IPacket>} map
 */
async function readPacketTypesIntoMap(categoryName, direction, map) {
    let dirName = `${__dirname}/packets/${categoryName}/${direction}`;
    let files = await fs.readdir(dirName);
    for (const file of files) {
        let packetType = require(`${dirName}/${file}`);
        packetType.category = categoryName;
        packetType.direction = direction;
        map.set(packetType.name, packetType);
    }
}

async function loadPacketTypes() {
    if (!loaded) {
        await readPacketTypesIntoMap('host', 'command', host_command);
        await readPacketTypesIntoMap('host', 'response', host_response);
        await readPacketTypesIntoMap('control', 'command', control_command);
        await readPacketTypesIntoMap('control', 'response', control_response);
        loaded = true;
    }
}

/**
 * @typedef {Object} DuplexPacketTypes
 * @property {Map<string, IPacket>} command
 * @property {Map<string, IPacket>} response
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
