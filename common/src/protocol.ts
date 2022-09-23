import fs = require('fs/promises');

let loaded = false;
const host_command = new Map();
const host_response = new Map();
const control_command = new Map();
const control_response = new Map();

export interface IPacket {
    index: number;
    name: string;
    category: string;
    direction: string;
    data: any;
}

async function readPacketTypesIntoMap(categoryName: string, direction: string, map: Map<string, IPacket>) {
    let dirName = `${__dirname}/packets/${categoryName}/${direction}`;
    let files = await fs.readdir(dirName);
    for (const file of files) {
        let packetType = require(`${dirName}/${file}`);
        packetType.category = categoryName;
        packetType.direction = direction;
        map.set(packetType.name, packetType);
    }
}

export async function loadPacketTypes() {
    if (!loaded) {
        await readPacketTypesIntoMap('host', 'command', host_command);
        await readPacketTypesIntoMap('host', 'response', host_response);
        await readPacketTypesIntoMap('control', 'command', control_command);
        await readPacketTypesIntoMap('control', 'response', control_response);
        loaded = true;
    }
}

interface DuplexPacketTypes {
    command: Map<string, IPacket>;
    response: Map<string, IPacket>;
}

interface HostControlPacketTypes {
    host: DuplexPacketTypes;
    control: DuplexPacketTypes;
}

export const packet_types: HostControlPacketTypes = {
    host: {
        command: host_command,
        response: host_response,
    },
    control: {
        command: control_command,
        response: control_response,
    },
};
