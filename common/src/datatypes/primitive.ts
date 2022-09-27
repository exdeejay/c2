import { DataType } from ".";

export const CharType = new DataType(
    'char',
    function (buf: Buffer): [number, number] {
        return [1, buf.readInt8()];
    },
    function (data: number): Buffer {
        let buf = Buffer.alloc(1);
        buf.writeInt8(data);
        return buf;
    }
);

export const ByteType = new DataType(
    'byte',
    function (buf: Buffer): [number, number] {
        return [1, buf.readUint8()];
    },
    function (data: number): Buffer {
        let buf = Buffer.alloc(1);
        buf.writeUint8(data);
        return buf;
    }
);

export const BooleanType = new DataType(
    'bool',
    function (buf: Buffer): [number, boolean] {
        return [1, buf.readUint8() != 0];
    },
    function (data: boolean): Buffer {
        let buf = Buffer.alloc(1);
        buf.writeUint8(data ? 1 : 0);
        return buf;
    }
);

export const IntType = new DataType(
    'int',
    function (buf: Buffer): [number, number] {
        return [4, buf.readInt32BE()];
    },
    function (data: number): Buffer {
        let buf = Buffer.alloc(4);
        buf.writeInt32BE(data);
        return buf;
    }
);

export const UintType = new DataType(
    'uint',
    function (buf: Buffer): [number, number] {
        return [4, buf.readUint32BE()];
    },
    function (data: number): Buffer {
        let buf = Buffer.alloc(4);
        buf.writeUint32BE(data);
        return buf;
    }
);

export const BigUintType = new DataType(
    'biguint',
    function (buf: Buffer): [number, bigint] {
        return [8, buf.readBigUint64BE()];
    },
    function (data: bigint): Buffer {
        let buf = Buffer.alloc(8);
        buf.writeBigUint64BE(data);
        return buf;
    }
);

export const StringType = new DataType(
    'str',
    function (buf: Buffer): [number, string] {
        let size = buf.readUInt32BE();
        let str = buf.subarray(4, 4 + size).toString();
        return [size + 4, str];
    },
    function (data: string): Buffer {
        let len = Buffer.alloc(4);
        len.writeUInt32BE(data.length);
        let buf = Buffer.concat([len, Buffer.from(data)]);
        return buf;
    }
);

export const BufferType = new DataType(
    'buf',
    function (buf: Buffer): [number, Buffer] {
        let size = buf.readUInt32BE();
        let data = Buffer.from(buf.subarray(4, 4 + size));
        return [size + 4, data];
    },
    function (data: Buffer): Buffer {
        let len = Buffer.alloc(4);
        len.writeUInt32BE(data.length);
        let buf = Buffer.concat([len, data]);
        return buf;
    }
);
