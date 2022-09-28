import { describe, expect, test } from '@jest/globals';
import { ByteType, CharType, IntType, UintType } from '../src/datatypes';
import { PacketTypes } from '../src/protocol';

describe('utils', () => {
    test('datatypes', () => {
        expect(CharType.parse(Buffer.from([244]))).toStrictEqual([1, -12]);
        expect(CharType.parse(CharType.serialize(-12))).toStrictEqual([1, -12]);
        expect(ByteType.parse(Buffer.from([244]))).toStrictEqual([1, 244]);
        expect(ByteType.parse(ByteType.serialize(244))).toStrictEqual([1, 244]);
        expect(IntType.parse(Buffer.from([0x80, 0, 0, 0xc]))).toStrictEqual([4, -2147483636]);
        expect(IntType.parse(IntType.serialize(-2147483636))).toStrictEqual([4, -2147483636]);
        expect(UintType.parse(Buffer.from([0x80, 0, 0, 0xc]))).toStrictEqual([4, 0x8000000c]);
        expect(UintType.parse(UintType.serialize(0x8000000c))).toStrictEqual([4, 0x8000000c]);
    });

    test('packet loading', async () => {
        const packetTypes = new PacketTypes();
        expect(packetTypes.loaded).toBe(false);
        expect(packetTypes.dataTypes.size).not.toBe(0);
        expect(packetTypes.packetTypes.control.command.size).toBe(0);
        expect(packetTypes.packetTypes.control.response.size).toBe(0);
        expect(packetTypes.packetTypes.host.command.size).toBe(0);
        expect(packetTypes.packetTypes.host.response.size).toBe(0);

        await packetTypes.load();
        expect(packetTypes.loaded).toBe(true);
        expect(packetTypes.dataTypes.size).not.toBe(0);
        expect(packetTypes.packetTypes.control.command.size).not.toBe(0);
        expect(packetTypes.packetTypes.control.response.size).not.toBe(0);
        expect(packetTypes.packetTypes.host.command.size).not.toBe(0);
        expect(packetTypes.packetTypes.host.response.size).not.toBe(0);
    });

    test('packet parsing', async () => {
        const packetTypes = new PacketTypes();
        await packetTypes.load();
        let packet;

        packet = packetTypes.parsePacket('host', 'response', Buffer.from([0x00, 0x01]));
        expect(packet._ptype.name).toBe('retcode');
        expect(packet.code).toBeDefined();
        expect(packet.code).toBe(1);

        packet = packetTypes.parsePacket('host', 'response', Buffer.from([
            0x03,
            0x00, 0x00, 0x00, 0x05,
            0x06, 0x07, 0x08, 0x09, 0x0a
        ]));
        expect(packet._ptype.name).toBe('buffer');
        expect(packet.data).toBeDefined();
        expect(packet.data).toEqual(Buffer.from([6, 7, 8, 9, 10]));
    });
});