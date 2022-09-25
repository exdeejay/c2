export interface FileData {
    name: string;
    attrs: number;
    creationTime: Date;
    lastAccessTime: Date;
    lastWriteTime: Date;
    size: bigint;
}