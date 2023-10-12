using System;
using System.IO;
using System.Text;

namespace libXFile
{
    /// <summary>
    /// Represents a class for working with XFiles, which are binary files with a specific header.
    /// </summary>
    public class XFile
    {
        private readonly byte[] HEADER_XFILE;

        /// <summary>
        /// Initializes a new instance of the XFile class with a default header.
        /// </summary>
        /// <remarks>
        /// Default header: 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08
        /// </remarks>
        public XFile()
        {
            HEADER_XFILE = new byte[8] { 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08 };
        }

        /// <summary>
        /// Initializes a new instance of the XFile class with a custom header specified by magicBytes.
        /// </summary>
        /// <param name="magicBytes">The custom header to use for XFile identification.</param>
        /// <exception cref="ArgumentNullException">Thrown when magicBytes is null.</exception>
        public XFile(byte[] magicBytes)
        {
            if (magicBytes != null)
            {
                HEADER_XFILE = magicBytes;
            }
            else
            {
                throw new ArgumentNullException(nameof(magicBytes));
            }
        }

        private bool CompareByteArrays(byte[] array1, byte[] array2)
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
        /// Gets the magic bytes used for XFile identification.
        /// </summary>
        /// <returns>The magic bytes as a byte array.</returns>
        public byte[] GetMagicBytes()
        {
            return HEADER_XFILE;
        }

        /// <summary>
        /// Checks if a file at the specified path is an XFile by comparing its header with the expected header.
        /// </summary>
        /// <param name="path">The path to the file to check.</param>
        /// <returns>True if the file is an XFile; otherwise, false.</returns>
        public bool CheckIsXFile(string path)
        {
            bool returnValue = false;
            if (!File.Exists(path))
            {
                return returnValue;
            }
            using (FileStream sourceFileStream = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                long fileSize = sourceFileStream.Length;
                if (fileSize < HEADER_XFILE.Length)
                {
                    sourceFileStream.Close();
                    return returnValue;
                }
                byte[] bufferHeader = new byte[HEADER_XFILE.Length];
                sourceFileStream.Read(bufferHeader, 0, HEADER_XFILE.Length);
                returnValue = CompareByteArrays(bufferHeader, HEADER_XFILE);
                sourceFileStream.Close();
                sourceFileStream.Dispose();
            }
            return returnValue;
        }

        /// <summary>
        /// Writes a byte array to the specified file, including the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to write the byte array to.</param>
        /// <param name="bytes">The byte array to be written to the file.</param>
        /// <remarks>
        /// This method writes the specified byte array to the file at the given path. It includes the XFile header at the beginning of the file. If the file does not exist, it will be created. If the file already exists, its contents will be overwritten.
        /// </remarks>
        /// <exception cref="Exception">Thrown when an error occurs during the file write operation.</exception>
        public void WriteAllBytes(string path, byte[] bytes)
        {
            try
            {
                using (FileStream destinationFileStream = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write))
                {
                    destinationFileStream.Write(HEADER_XFILE, 0, HEADER_XFILE.Length);
                    destinationFileStream.Write(bytes, 0, bytes.Length);
                    destinationFileStream.Close();
                    destinationFileStream.Dispose();
                }
            }
            catch (Exception)
            {
                throw;
            }
        }

        /// <summary>
        /// Appends a byte array to the end of the specified file.
        /// </summary>
        /// <param name="path">The path to the file to which the byte array will be appended.</param>
        /// <param name="bytes">The byte array to be appended to the file.</param>
        /// <remarks>
        /// This method appends the specified byte array to the end of the file located at the given path. If the file does not exist, it will be created, and the byte array will be written to it. If the file already exists, the byte array will be added to the existing content at the end of the file.
        /// </remarks>
        /// <exception cref="Exception">Thrown when an error occurs during the file append operation.</exception>
        public void Append(string path, byte[] bytes)
        {
            try
            {
                using (FileStream destinationFileStream = new FileStream(path, FileMode.Open, FileAccess.Write))
                {
                    destinationFileStream.Seek(destinationFileStream.Length, SeekOrigin.Begin);
                    destinationFileStream.Write(bytes, 0, bytes.Length);
                    destinationFileStream.Close();
                    destinationFileStream.Dispose();
                }
            }
            catch (Exception)
            {
                throw;
            }
        }

        /// <summary>
        /// Writes a text string to the specified file using UTF-8 encoding, including the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to write the text to.</param>
        /// <param name="contents">The text string to be written to the file.</param>
        /// <remarks>
        /// This method encodes the specified text string using UTF-8 encoding and writes it to the file at the given path, including the XFile header at the beginning of the file. If the file does not exist, it will be created. If the file already exists, its contents will be overwritten.
        /// </remarks>
        /// <exception cref="Exception">Thrown when an error occurs during the file write operation.</exception>
        public void WriteAllText(string path, string contents)
        {
            WriteAllBytes(path, Encoding.UTF8.GetBytes(contents));
        }

        /// <summary>
        /// Writes a text string to the specified file using the specified encoding, including the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to write the text to.</param>
        /// <param name="contents">The text string to be written to the file.</param>
        /// <param name="encoding">The encoding to use for writing the text to the file.</param>
        /// <remarks>
        /// This method encodes the specified text string using the provided encoding and writes it to the file at the given path, including the XFile header at the beginning of the file. If the file does not exist, it will be created. If the file already exists, its contents will be overwritten.
        /// </remarks>
        /// <exception cref="Exception">Thrown when an error occurs during the file write operation.</exception>
        public void WriteAllText(string path, string contents, Encoding encoding)
        {
            WriteAllBytes(path, encoding.GetBytes(contents));
        }

        /// <summary>
        /// Writes an array of text lines to the specified file, separated by newlines and including the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to write the text lines to.</param>
        /// <param name="lines">An array of text lines to be written to the file.</param>
        /// <remarks>
        /// This method joins the array of text lines using newline characters and writes the resulting text to the file at the given path, including the XFile header at the beginning of the file. If the file does not exist, it will be created. If the file already exists, its contents will be overwritten.
        /// </remarks>
        /// <exception cref="Exception">Thrown when an error occurs during the file write operation.</exception>
        public void WriteAllLines(string path, string[] lines)
        {
            string contents = string.Join(Environment.NewLine, lines);
            WriteAllText(path, contents);
        }

        /// <summary>
        /// Reads the text content of the specified file using UTF-8 encoding, skipping the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to read the text from.</param>
        /// <returns>The text content of the file, excluding the XFile header.</returns>
        /// <remarks>
        /// This method reads the text content of the file located at the given path, using UTF-8 encoding, and skips the XFile header at the beginning of the file. If the file does not exist, a FileNotFoundException is thrown.
        /// </remarks>
        /// <exception cref="FileNotFoundException">Thrown when the file is not found.</exception>
        public string ReadAllText(string path)
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("File Not Found!", path);
            }
            string fileText;
            using (StreamReader reader = new StreamReader(path, Encoding.UTF8))
            {
                reader.BaseStream.Seek(HEADER_XFILE.Length, SeekOrigin.Begin);
                fileText = reader.ReadToEnd();
            }
            return fileText;
        }

        /// <summary>
        /// Reads the text content of the specified file using the specified encoding, skipping the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to read the text from.</param>
        /// <param name="encoding">The encoding to use for reading the text from the file.</param>
        /// <returns>The text content of the file, excluding the XFile header.</returns>
        /// <remarks>
        /// This method reads the text content of the file located at the given path, using the specified encoding, and skips the XFile header at the beginning of the file. If the file does not exist, a FileNotFoundException is thrown.
        /// </remarks>
        /// <exception cref="FileNotFoundException">Thrown when the file is not found.</exception>
        public string ReadAllText(string path, Encoding encoding)
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("File Not Found!", path);
            }
            string fileText;
            using (StreamReader reader = new StreamReader(path, encoding))
            {
                reader.BaseStream.Seek(HEADER_XFILE.Length, SeekOrigin.Begin);
                fileText = reader.ReadToEnd();
            }
            return fileText;
        }

        /// <summary>
        /// Reads the text lines of the specified file using UTF-8 encoding, skipping the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to read the text lines from.</param>
        /// <returns>An array of text lines from the file, excluding the XFile header.</returns>
        /// <remarks>
        /// This method reads the text lines of the file located at the given path, using UTF-8 encoding, and skips the XFile header at the beginning of the file. If the file does not exist, a FileNotFoundException is thrown.
        /// </remarks>
        /// <exception cref="FileNotFoundException">Thrown when the file is not found.</exception>
        public string[] ReadAllLines(string path)
        {
            string contents = ReadAllText(path);
            return contents.Split(new string[] { Environment.NewLine }, StringSplitOptions.None);
        }

        /// <summary>
        /// Reads the binary content of the specified file, excluding the XFile header.
        /// </summary>
        /// <param name="path">The path to the file to read the binary content from.</param>
        /// <returns>The binary content of the file, excluding the XFile header.</returns>
        /// <remarks>
        /// This method reads the binary content of the file located at the given path, excluding the XFile header at the beginning of the file. If the file does not exist, a FileNotFoundException is thrown.
        /// </remarks>
        /// <exception cref="FileNotFoundException">Thrown when the file is not found.</exception>
        public byte[] ReadAllBytes(string path)
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("File Not Found!", path);
            }
            byte[] fileBytes;
            using (FileStream sourceFileStream = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                long fileSize = sourceFileStream.Length;
                fileBytes = new byte[fileSize - HEADER_XFILE.Length];
                sourceFileStream.Seek(HEADER_XFILE.Length, SeekOrigin.Begin);
                sourceFileStream.Read(fileBytes, 0, fileBytes.Length);
                sourceFileStream.Close();
                sourceFileStream.Dispose();
            }
            return fileBytes;
        }
    }
}
