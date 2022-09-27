export class DataType<T> {
    constructor(
        public name: string,
        public parse: (buf: Buffer) => [number, T],
        public serialize: (data: T) => Buffer
    ) {}
}

export * from './primitive';
export * from './filedata';