import { describe, expect, test } from '@jest/globals';
import { ByteType, CharType, IntType, UintType } from '../src/datatypes';

describe('utils', () => {
    test('protocol', () => {
        expect(CharType.parse(Buffer.from([244]))).toStrictEqual([1, -12]);
        expect(ByteType.parse(Buffer.from([244]))).toStrictEqual([1, 244]);
        expect(IntType.parse(Buffer.from([0x80, 0, 0, 0xc]))).toStrictEqual([4, -2147483636]);
        expect(UintType.parse(Buffer.from([0x80, 0, 0, 0xc]))).toStrictEqual([4, 0x8000000c]);
    });
});