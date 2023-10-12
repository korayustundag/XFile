import * as fs from 'fs';

class XFile {
    private header_xfile: Uint8Array;

    constructor(magicBytes?: number[]) {
        if (magicBytes) {
            if (magicBytes.length !== 8) {
                throw new Error('Magic bytes must be 8 bytes long.');
            }
            this.header_xfile = new Uint8Array(magicBytes);
        } else {
            this.header_xfile = new Uint8Array([0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08]);
        }
    }

    getMagicBytes(): number[] {
        return Array.from(this.header_xfile);
    }

    private static compareByteArrays(array1: Uint8Array, array2: Uint8Array): boolean {
        if (array1.length !== array2.length) {
            return false;
        }

        for (let i = 0; i < array1.length; i++) {
            if (array1[i] !== array2[i]) {
                return false;
            }
        }

        return true;
    }

    checkIsXFile(path: string): boolean {
        if (!fs.existsSync(path)) {
            return false;
        }

        const bufferHeader = new Uint8Array(8);
        const sourceFileStream = fs.openSync(path, 'r');
        fs.readSync(sourceFileStream, bufferHeader, 0, 8, 0);
        fs.closeSync(sourceFileStream);

        return XFile.compareByteArrays(bufferHeader, this.header_xfile);
    }

    writeAllBytes(path: string, bytes: Uint8Array): void {
        const destinationFileStream = fs.openSync(path, 'w');
        fs.writeSync(destinationFileStream, this.header_xfile, 0, 8);
        fs.writeSync(destinationFileStream, bytes, 0, bytes.length);
        fs.closeSync(destinationFileStream);
    }

    append(path: string, bytes: Uint8Array): void {
        const destinationFileStream = fs.openSync(path, 'a');
        fs.writeSync(destinationFileStream, bytes, 0, bytes.length);
        fs.closeSync(destinationFileStream);
    }

    writeAllText(path: string, contents: string, encoding: BufferEncoding = 'utf-8'): void {
        const buffer = Buffer.from(contents, encoding);
        this.writeAllBytes(path, new Uint8Array(buffer));
    }

    writeAllLines(path: string, lines: string[]): void {
        const contents = lines.join('\n');
        this.writeAllText(path, contents);
    }

    readAllText(path: string, encoding: BufferEncoding = 'utf-8'): string {
        const bufferHeader = new Uint8Array(8);
        const sourceFileStream = fs.openSync(path, 'r');
        fs.readSync(sourceFileStream, bufferHeader, 0, 8, 0);
        const fileSize = fs.statSync(path).size - 8;
        const bufferContent = new Uint8Array(fileSize);
        fs.readSync(sourceFileStream, bufferContent, 0, fileSize, 8);
        fs.closeSync(sourceFileStream);
        return Buffer.from(bufferContent).toString(encoding);
    }

    readAllLines(path: string, encoding: BufferEncoding = 'utf-8'): string[] {
        const text = this.readAllText(path, encoding);
        return text.split('\n');
    }
}

const xfile = new XFile();
const xfile_custom = new XFile([0x1A, 0x0F, 0x0E, 0x0B, 0xA5, 0x10, 0x00, 0x80]);

// Use the XFile functions here

console.log('Magic Bytes:', xfile.getMagicBytes());
console.log('Custom Magic Bytes:', xfile_custom.getMagicBytes());
