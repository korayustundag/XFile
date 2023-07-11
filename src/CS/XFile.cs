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

using System;
using System.IO;

namespace libXFile
{
    /// <summary>
    /// Provides utility methods for working with XFiles.
    /// </summary>
    public static class XFile
    {
        /// <summary>
        /// Gets the header bytes that identify an XFile.
        /// </summary>
        public static byte[] HEADER_XFILE
        {
            get
            {
                return new byte[8] { 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08 };
            }
        }

        /// <summary>
        /// Gets the end of file bytes that mark the end of an XFile.
        /// </summary>
        public static byte[] EOF_XFILE
        {
            get
            {
                return new byte[8] { 0x08, 0x00, 0x01, 0x5A, 0x0F, 0x0B, 0x07, 0x2A };
            }
        }

        private static bool CompareByteArrays(byte[] array1, byte[] array2)
        {
            if (array1.Length != array2.Length)
            {
                return false;
            }

            for (int i = 0; i < array1.Length; i++)
            {
                if (array1[i] != array2[i])
                {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// Checks if the file at the specified path is an XFile.
        /// </summary>
        /// <param name="path">The path of the file to check.</param>
        /// <returns>
        ///   <c>true</c> if the file is an X file; otherwise, <c>false</c>.
        /// </returns>
        public static bool CheckIsXFile(string path)
        {
            bool returnValue = false;
            
            if (!File.Exists(path))
            {
                return returnValue;
            }

            using (FileStream sourceFileStream = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                long fileSize = sourceFileStream.Length;

                /* Return mismatch if file size is less than the combined length of HEADER and EOF */
                if (fileSize < HEADER_XFILE.Length + EOF_XFILE.Length)
                {
                    sourceFileStream.Close();
                    return returnValue;
                }

                byte[] bufferHeader = new byte[HEADER_XFILE.Length];
                byte[] bufferEOF = new byte[EOF_XFILE.Length];

                /* Read HEADER */
                sourceFileStream.Read(bufferHeader, 0, HEADER_XFILE.Length);

                /* Read EOF */
                sourceFileStream.Seek(-EOF_XFILE.Length, SeekOrigin.End);
                sourceFileStream.Read(bufferEOF, 0, EOF_XFILE.Length);

                /* Compare HEADER and EOF */
                returnValue = (CompareByteArrays(bufferHeader, HEADER_XFILE) && CompareByteArrays(bufferEOF, EOF_XFILE));

                /* Close and release the Source File */
                sourceFileStream.Close();
                sourceFileStream.Dispose();
            }

            return returnValue;
        }

        /// <summary>
        /// Reads all bytes of a file, excluding <c>HEADER</c> and <c>EOF</c>.
        /// </summary>
        /// <param name="path">The path of the file to read.</param>
        /// <returns>An array of bytes containing the data read from the file.</returns>
        /// <exception cref="SourceFileIsNotXFileException">Thrown if the file is not XFile..</exception>
        /// <exception cref="FileNotFoundException">Thrown if the file is not found...</exception>
        public static byte[] ReadAllBytes(string path)
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("File Not Found!", path);
            }

            if (!CheckIsXFile(path))
            {
                throw new SourceFileIsNotXFileException("File: \"" + Path.GetFullPath(path) + "\" is not XFile");
            }

            byte[] fileBytes;

            using (FileStream sourceFileStream = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                long fileSize = sourceFileStream.Length;

                fileBytes = new byte[fileSize - (HEADER_XFILE.Length + EOF_XFILE.Length)];

                /* Skip HEADER */
                sourceFileStream.Seek(HEADER_XFILE.Length, SeekOrigin.Begin);

                /* Read Data */
                sourceFileStream.Read(fileBytes, 0, fileBytes.Length);

                /* Skip EOF */
                sourceFileStream.Seek(-(EOF_XFILE.Length), SeekOrigin.End);

                /* Close and release the Destination File */
                sourceFileStream.Close();
                sourceFileStream.Dispose();
            }
            return fileBytes;
        }

        /// <summary>
        /// Writes the byte array with a HEADER at the beginning, EOF at the end, and data in between to the specified file.
        /// </summary>
        /// <param name="path">The file to write to.</param>
        /// <param name="bytes">The bytes to write to the file.</param>
        public static void WriteAllBytes(string path, byte[] bytes)
        {
            using (FileStream destinationFileStream = new FileStream(path, FileMode.Create, FileAccess.Write))
            {
                /* Write HEADER */
                destinationFileStream.Write(HEADER_XFILE, 0, HEADER_XFILE.Length);

                /* Write Data */
                destinationFileStream.Write(bytes, 0, bytes.Length);

                /* Write EOF */
                destinationFileStream.Write(EOF_XFILE, 0, EOF_XFILE.Length);

                /* Close and release the Destination File */
                destinationFileStream.Close();
                destinationFileStream.Dispose();
            }
        }

        /// <summary>
        /// Appends the byte array data to the existing XFile.
        /// </summary>
        /// <param name="path">The path of the file to append data to.</param>
        /// <param name="bytes">The byte array containing the data to be appended.</param>
        /// <exception cref="SourceFileIsNotXFileException">Thrown if the file is not XFile..</exception>
        /// <exception cref="FileNotFoundException">Thrown if the file is not found...</exception>
        public static void Append(string path, byte[] bytes)
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("File Not Found!", path);
            }

            if (!CheckIsXFile(path))
            {
                throw new SourceFileIsNotXFileException("File: \"" + Path.GetFullPath(path) + "\" is not XFile");
            }

            using (FileStream destinationFileStream = new FileStream(path, FileMode.Open, FileAccess.Write))
            {
                /* Move the file pointer to the end of the data (right before the EOF) */
                long dataEndPosition = destinationFileStream.Length - EOF_XFILE.Length;
                destinationFileStream.Seek(dataEndPosition, SeekOrigin.Begin);

                /* Append the data */
                destinationFileStream.Write(bytes, 0, bytes.Length);

                destinationFileStream.Write(EOF_XFILE, 0, EOF_XFILE.Length);

                /* Close and release the Destination File */
                destinationFileStream.Close();
                destinationFileStream.Dispose();
            }
        }
    }
}
