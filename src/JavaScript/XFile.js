/* ******************************************************************************** */ 
/* MIT License                                                                      */
/*                                                                                  */
/* Copyright (c) 2023 Koray Üstündağ                                                */
/*                                                                                  */
/* Permission is hereby granted, free of charge, to any person obtaining a copy     */
/* of this software and associated documentation files (the "Software"), to deal    */
/* in the Software without restriction, including without limitation the rights     */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/* copies of the Software, and to permit persons to whom the Software is            */
/* furnished to do so, subject to the following conditions:                         */
/*                                                                                  */
/* The above copyright notice and this permission notice shall be included in all   */
/* copies or substantial portions of the Software.                                  */
/*                                                                                  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    */
/* SOFTWARE.                                                                        */
/* ******************************************************************************** */

class XFile {
  static HEADER_XFILE = new Uint8Array([0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08]);
  static EOF_XFILE = new Uint8Array([0x08, 0x00, 0x01, 0x5A, 0x0F, 0x0B, 0x07, 0x2A]);

  static compareByteArrays(array1, array2) {
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

  static checkIsXFile(path) {
    const fs = require('fs');

    if (!fs.existsSync(path)) {
      return false;
    }

    const fileSize = fs.statSync(path).size;

    // Return mismatch if file size is less than the combined length of HEADER and EOF
    if (fileSize < XFile.HEADER_XFILE.length + XFile.EOF_XFILE.length) {
      return false;
    }

    const bufferHeader = new Uint8Array(XFile.HEADER_XFILE.length);
    const bufferEOF = new Uint8Array(XFile.EOF_XFILE.length);

    // Read HEADER
    const fileHandle = fs.openSync(path, 'r');
    fs.readSync(fileHandle, bufferHeader, 0, XFile.HEADER_XFILE.length, 0);

    // Read EOF
    fs.readSync(fileHandle, bufferEOF, 0, XFile.EOF_XFILE.length, fileSize - XFile.EOF_XFILE.length);

    // Compare HEADER and EOF
    const isXFile = XFile.compareByteArrays(bufferHeader, XFile.HEADER_XFILE) &&
      XFile.compareByteArrays(bufferEOF, XFile.EOF_XFILE);

    fs.closeSync(fileHandle);

    return isXFile;
  }

  static readAllBytes(path) {
    const fs = require('fs');

    if (!fs.existsSync(path)) {
      throw new Error('File Not Found!');
    }

    if (!XFile.checkIsXFile(path)) {
      throw new Error(`File: "${path}" is not XFile`);
    }

    const fileSize = fs.statSync(path).size;
    const fileBytes = new Uint8Array(fileSize - (XFile.HEADER_XFILE.length + XFile.EOF_XFILE.length));

    // Skip HEADER
    const fileHandle = fs.openSync(path, 'r');
    fs.readSync(fileHandle, fileBytes, 0, fileBytes.length, XFile.HEADER_XFILE.length);

    fs.closeSync(fileHandle);

    return fileBytes;
  }

  static writeAllBytes(path, bytes) {
    const fs = require('fs');

    const fileHandle = fs.openSync(path, 'w');

    // Write HEADER
    fs.writeSync(fileHandle, XFile.HEADER_XFILE, 0, XFile.HEADER_XFILE.length);

    // Write Data
    fs.writeSync(fileHandle, bytes, 0, bytes.length);

    // Write EOF
    fs.writeSync(fileHandle, XFile.EOF_XFILE, 0, XFile.EOF_XFILE.length);

    fs.closeSync(fileHandle);
  }

  static append(path, bytes) {
    const fs = require('fs');

    if (!fs.existsSync(path)) {
      throw new Error('File Not Found!');
    }

    if (!XFile.checkIsXFile(path)) {
      throw new Error(`File: "${path}" is not XFile`);
    }

    const fileHandle = fs.openSync(path, 'r+');
    const fileSize = fs.statSync(path).size;
    const dataEndPosition = fileSize - XFile.EOF_XFILE.length;

    // Move the file pointer to the end of the data (right before the EOF)
    fs.ftruncateSync(fileHandle, dataEndPosition);

    // Append the data
    fs.writeSync(fileHandle, bytes, 0, bytes.length, null);

    // Write EOF
    fs.writeSync(fileHandle, XFile.EOF_XFILE, 0, XFile.EOF_XFILE.length, null);

    fs.closeSync(fileHandle);
  }
}
