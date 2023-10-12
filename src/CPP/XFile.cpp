/**
 * @file XFile.cpp
 * @brief XFile Operations
 *
 * This file contains a C++ class, XFile, designed for working with files that have an XFile header.
 * It provides operations for creating XFile objects, comparing headers, writing and reading files,
 * and working with file content.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <cstring>

 /**
  * @class XFile
  * @brief XFile class for file operations
  *
  * The XFile class provides various operations for working with files that have an XFile header.
  */
class XFile
{
private:
    std::vector<uint8_t> HEADER_XFILE = { 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08 };

    /**
     * @brief Compare two byte arrays for equality.
     *
     * This private method compares two byte arrays for equality up to their sizes.
     *
     * @param array1 The first byte array.
     * @param array2 The second byte array.
     * @return True if the arrays are equal, false otherwise.
     */
    bool CompareByteArrays(const std::vector<uint8_t>& array1, const std::vector<uint8_t>& array2)
    {
        if (array1.size() != array2.size())
        {
            return false;
        }

        for (size_t i = 0; i < array1.size(); i++)
        {
            if (array1[i] != array2[i])
            {
                return false;
            }
        }

        return true;
    }

public:
    /**
     * @brief Default constructor for the XFile class.
     */
    XFile() {}

    /**
     * @brief Constructor for the XFile class with custom magic bytes.
     *
     * This constructor initializes an XFile with custom magic bytes provided as a vector.
     *
     * @param magicBytes The custom magic bytes for the XFile.
     * @throw std::invalid_argument If magicBytes is empty.
     */
    XFile(const std::vector<uint8_t>& magicBytes)
    {
        if (!magicBytes.empty())
        {
            HEADER_XFILE = magicBytes;
        }
        else
        {
            throw std::invalid_argument("magicBytes cannot be empty.");
        }
    }

    /**
     * @brief Get the magic bytes of the XFile header.
     *
     * This method returns the magic bytes that make up the XFile header.
     *
     * @return The XFile header as a vector of bytes.
     */
    std::vector<uint8_t> GetMagicBytes()
    {
        return HEADER_XFILE;
    }

    /**
     * @brief Check if a file is an XFile by comparing its header.
     *
     * This method checks if a file at the specified path is an XFile by comparing its header with
     * the expected XFile header.
     *
     * @param path The path to the file to check.
     * @return True if the file is an XFile, false otherwise.
     */
    bool CheckIsXFile(const std::string& path)
    {
        bool returnValue = false;
        if (!std::ifstream(path))
        {
            return returnValue;
        }
        std::ifstream sourceFileStream(path, std::ios::in | std::ios::binary);

        sourceFileStream.seekg(0, std::ios::end);
        std::streampos fileSize = sourceFileStream.tellg();
        std::streampos headerSize = HEADER_XFILE.size();
        sourceFileStream.seekg(0, std::ios::beg);

        if (fileSize < headerSize)
        {
            return returnValue;
        }

        std::vector<uint8_t> bufferHeader(HEADER_XFILE.size());
        sourceFileStream.read(reinterpret_cast<char*>(bufferHeader.data()), HEADER_XFILE.size());

        returnValue = CompareByteArrays(bufferHeader, HEADER_XFILE);
        return returnValue;
    }

    /**
     * @brief Write a byte array to a file, including the XFile header.
     *
     * This method writes a byte array to the specified file, including the XFile header at the beginning
     * of the file. If the file does not exist, it will be created.
     *
     * @param path The path to the file to write the byte array to.
     * @param bytes The byte array to be written to the file.
     */
    void WriteAllBytes(const std::string& path, const std::vector<uint8_t>& bytes)
    {
        std::ofstream destinationFileStream(path, std::ios::out | std::ios::binary);
        destinationFileStream.write(reinterpret_cast<const char*>(HEADER_XFILE.data()), HEADER_XFILE.size());
        destinationFileStream.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }

    /**
     * @brief Append a byte array to the end of a file.
     *
     * This method appends a byte array to the end of the specified file. If the file does not exist,
     * it will be created, and the byte array will be added to the existing content at the end of the file.
     *
     * @param path The path to the file to which the byte array will be appended.
     * @param bytes The byte array to be appended to the file.
     */
    void Append(const std::string& path, const std::vector<uint8_t>& bytes)
    {
        std::ofstream destinationFileStream(path, std::ios::out | std::ios::binary | std::ios::app);
        destinationFileStream.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    }

    /**
     * @brief Write text to a file using UTF-8 encoding, including the XFile header.
     *
     * This method encodes the specified text string using UTF-8 encoding and writes it to the file
     * at the given path, including the XFile header at the beginning of the file. If the file does not
     * exist, it will be created.
     *
     * @param path The path to the file to write the text to.
     * @param contents The text string to be written to the file.
     */
    void WriteAllText(const std::string& path, const std::string& contents)
    {
        WriteAllBytes(path, std::vector<uint8_t>(contents.begin(), contents.end()));
    }

    /**
     * @brief [This method is under construction] Write text to a file using a specified encoding, including the XFile header.
     *
     * This method encodes the specified text string using the provided encoding and writes it to the file
     * at the given path, including the XFile header at the beginning of the file. If the file does not exist,
     * it will be created.
     *
     * @param path The path to the file to write the text to.
     * @param contents The text string to be written to the file.
     * @param encoding The encoding to use.
     */
    void WriteAllText(const std::string& path, const std::string& contents, const std::string& encoding)
    {
        WriteAllBytes(path, std::vector<uint8_t>(contents.begin(), contents.end()));
    }

    /**
     * @brief Write multiple lines of text to a file using UTF-8 encoding, including the XFile header.
     *
     * This method writes multiple lines of text to a file, separating them with line breaks. The text
     * includes the XFile header at the beginning of the file. If the file does not exist, it will be created.
     *
     * @param path The path to the file to write the text to.
     * @param lines The lines of text to be written to the file.
     */
    void WriteAllLines(const std::string& path, const std::vector<std::string>& lines)
    {
        std::string contents = "";
        for (const std::string& line : lines)
        {
            contents += line + '\n';
        }
        WriteAllText(path, contents);
    }

    /**
     * @brief Read the text content of a file, excluding the XFile header.
     *
     * This method reads the text content of the file located at the given path, using UTF-8 encoding,
     * and skips the XFile header at the beginning of the file. If the file does not exist, an exception is thrown.
     *
     * @param path The path to the file to read the text from.
     * @return The text content of the file, excluding the XFile header.
     */
    std::string ReadAllText(const std::string& path)
    {
        if (!std::ifstream(path))
        {
            throw std::invalid_argument("File Not Found: " + path);
        }

        std::ifstream reader(path, std::ios::in | std::ios::binary);

        reader.seekg(HEADER_XFILE.size(), std::ios::beg);
        std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

        return std::string(fileData.begin(), fileData.end());
    }

    /**
     * @brief Read multiple lines of text from a file, excluding the XFile header.
     *
     * This method reads multiple lines of text from the file located at the given path, excluding the
     * XFile header. If the file does not exist, an exception is thrown.
     *
     * @param path The path to the file to read the text from.
     * @return A vector of strings, each representing a line of text from the file.
     */
    std::vector<std::string> ReadAllLines(const std::string& path)
    {
        std::string text = ReadAllText(path);
        std::vector<std::string> lines;
        size_t pos = 0;
        size_t end;
        while ((end = text.find('\n', pos)) != std::string::npos)
        {
            lines.push_back(text.substr(pos, end - pos));
            pos = end + 1;
        }
        if (pos < text.size())
        {
            lines.push_back(text.substr(pos, text.size() - pos));
        }
        return lines;
    }

    /**
     * @brief Read all bytes of a file, excluding the XFile header.
     *
     * This method reads all bytes of the file located at the given path, excluding the XFile header. If the file does not exist, an exception is thrown.
     * @param path The path to the file to read the bytes from.
     * @return A vector of bytes representing the content of the file, excluding the XFile header.
     */
    std::vector<uint8_t> ReadAllBytes(const std::string& path)
    {
        if (!std::ifstream(path))
        {
            throw std::invalid_argument("File Not Found: " + path);
        }

        std::ifstream sourceFileStream(path, std::ios::in | std::ios::binary);

        sourceFileStream.seekg(0, std::ios::end);
        std::streampos fileSize = sourceFileStream.tellg();
        std::streampos headerSize = HEADER_XFILE.size();
        sourceFileStream.seekg(HEADER_XFILE.size(), std::ios::beg);

        std::vector<uint8_t> fileBytes(static_cast<size_t>(fileSize - headerSize));
        sourceFileStream.read(reinterpret_cast<char*>(fileBytes.data()), fileBytes.size());

        return fileBytes;
    }
};

// Example
int main()
{
    // Default Header
    XFile* xf = new XFile();
    xf->WriteAllText("default.dat", "This is default test!");
    delete xf;

    // Custom Header
    std::vector<uint8_t> customHeader = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
    XFile* xf_custom = new XFile(customHeader);
    xf_custom->WriteAllText("custom.dat", "This is custom test!");
    delete xf_custom;
}
