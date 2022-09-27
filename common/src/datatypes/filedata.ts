import { DataType } from "../protocol";
import { BigUintType, StringType, UintType } from "./primitive";

export interface FileData {
    name: string;
    attrs: number;
    creationTime: Date;
    lastAccessTime: Date;
    lastWriteTime: Date;
    size: bigint;
}

export const FileDataType = new DataType(
    'file',
    function (buf: Buffer): [number, FileData] {
        let partialFile: Partial<FileData> = {};
        let totalSize = 0;
        let size;
        let val;
        [size, val] = StringType.parse(buf);
        partialFile.name = val;
        buf = buf.subarray(size);
        totalSize += size;
        [size, val] = UintType.parse(buf);
        partialFile.attrs = val;
        buf = buf.subarray(size);
        totalSize += size;
        [size, val] = BigUintType.parse(buf);
        partialFile.creationTime = new Date(Number(val));
        buf = buf.subarray(size);
        totalSize += size;
        [size, val] = BigUintType.parse(buf);
        partialFile.lastAccessTime = new Date(Number(val));
        buf = buf.subarray(size);
        totalSize += size;
        [size, val] = BigUintType.parse(buf);
        partialFile.lastWriteTime = new Date(Number(val));
        buf = buf.subarray(size);
        totalSize += size;
        [size, val] = BigUintType.parse(buf);
        partialFile.size = val;
        totalSize += size;
        return [totalSize, partialFile as FileData];
    },
    function (data: FileData): Buffer {
        let buf = Buffer.from([]);
        buf = Buffer.concat([buf, StringType.serialize(data.name)]);
        buf = Buffer.concat([buf, UintType.serialize(data.attrs)]);
        buf = Buffer.concat([buf, BigUintType.serialize(BigInt(data.creationTime.valueOf()))]);
        buf = Buffer.concat([buf, BigUintType.serialize(BigInt(data.lastAccessTime.valueOf()))]);
        buf = Buffer.concat([buf, BigUintType.serialize(BigInt(data.lastWriteTime.valueOf()))]);
        buf = Buffer.concat([buf, BigUintType.serialize(data.size)]);
        return buf;
    }
);