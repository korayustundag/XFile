#include "XFile.h"

const std::array<uint8_t, 8> XFile::HEADER = { 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08 };
const std::array<uint8_t, 8> XFile::EOF_XFILE = { 0x08, 0x00, 0x01, 0x5A, 0x0F, 0x0B, 0x07, 0x2A };

template <size_t N>
bool XFile::CompareByteArrays(const std::vector<uint8_t>& array1, const std::array<uint8_t, N>& array2)
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

bool XFile::CheckIsXFile(const std::string& path)
{
    bool returnValue = false;

    std::ifstream sourceFileStream(path, std::ios::binary);
    if (!sourceFileStream)
    {
        return returnValue;
    }

    sourceFileStream.seekg(0, std::ios::end);
    std::streampos fileSize = sourceFileStream.tellg();

    /* Return mismatch if file size is less than the combined length of HEADER and EOF */
    if (fileSize < HEADER.size() + EOF_XFILE.size())
    {
        sourceFileStream.close();
        return returnValue;
    }

    std::vector<uint8_t> bufferHeader(HEADER.size());
    std::vector<uint8_t> bufferEOF(EOF_XFILE.size());

    /* Read HEADER */
    sourceFileStream.seekg(0, std::ios::beg);
    sourceFileStream.read(reinterpret_cast<char*>(bufferHeader.data()), bufferHeader.size());

    /* Read EOF */
    sourceFileStream.seekg(-static_cast<std::streamoff>(EOF_XFILE.size()), std::ios::end);
    sourceFileStream.read(reinterpret_cast<char*>(bufferEOF.data()), bufferEOF.size());

    /* Compare HEADER and EOF */
    returnValue = (CompareByteArrays(bufferHeader, HEADER) && CompareByteArrays(bufferEOF, EOF_XFILE));

    /* Close and release the Source File */
    sourceFileStream.close();

    return returnValue;
}

std::vector<uint8_t> XFile::ReadAllBytes(const std::string& path)
{
    std::ifstream sourceFileStream(path, std::ios::binary);
    if (!sourceFileStream)
    {
        throw std::runtime_error("File Not Found! Path: " + path);
    }

    if (!CheckIsXFile(path))
    {
        throw SourceFileIsNotXFileException("File: \"" + path + "\" is not XFile");
    }

    sourceFileStream.seekg(0, std::ios::end);
    std::streampos fileSize = sourceFileStream.tellg();

    std::vector<uint8_t> fileBytes(static_cast<size_t>(fileSize) - (HEADER.size() + EOF_XFILE.size()));

    /* Skip HEADER */
    sourceFileStream.seekg(HEADER.size(), std::ios::beg);

    /* Read Data */
    sourceFileStream.read(reinterpret_cast<char*>(fileBytes.data()), fileBytes.size());

    /* Skip EOF */
    sourceFileStream.seekg(-static_cast<std::streamoff>(EOF_XFILE.size()), std::ios::end);

    /* Close and release the Destination File */
    sourceFileStream.close();

    return fileBytes;
}

void XFile::WriteAllBytes(const std::string& path, const std::vector<uint8_t>& bytes)
{
    std::ofstream destinationFileStream(path, std::ios::binary);
    if (!destinationFileStream)
    {
        throw std::runtime_error("Failed to create file! Path: " + path);
    }

    /* Write HEADER */
    destinationFileStream.write(reinterpret_cast<const char*>(HEADER.data()), HEADER.size());

    /* Write Data */
    destinationFileStream.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

    /* Write EOF */
    destinationFileStream.write(reinterpret_cast<const char*>(EOF_XFILE.data()), EOF_XFILE.size());

    /* Close and release the Destination File */
    destinationFileStream.close();
}

void XFile::Append(const std::string& path, const std::vector<uint8_t>& bytes)
{
    std::ofstream destinationFileStream(path, std::ios::binary | std::ios::in | std::ios::out);
    if (!destinationFileStream)
    {
        throw std::runtime_error("File Not Found! Path: " + path);
    }

    if (!CheckIsXFile(path))
    {
        throw SourceFileIsNotXFileException("File: \"" + path + "\" is not XFile");
    }

    /* Move the file pointer to the end of the data (right before the EOF) */
    destinationFileStream.seekp(-static_cast<std::streamoff>(EOF_XFILE.size()), std::ios::end);

    /* Append the data */
    destinationFileStream.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

    destinationFileStream.write(reinterpret_cast<const char*>(EOF_XFILE.data()), EOF_XFILE.size());

    /* Close and release the Destination File */
    destinationFileStream.close();
}
