#include "XFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned char HEADER_XFILE[8] = { 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08 };
const unsigned char EOF_XFILE[8] = { 0x08, 0x00, 0x01, 0x5A, 0x0F, 0x0B, 0x07, 0x2A };

bool CompareByteArrays(const unsigned char* array1, const unsigned char* array2, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (array1[i] != array2[i])
        {
            return false;
        }
    }

    return true;
}

bool CheckIsXFile(const char* path)
{
    bool returnValue = false;
    FILE* sourceFile = fopen(path, "rb");

    if (!sourceFile)
    {
        return returnValue;
    }

    fseek(sourceFile, 0, SEEK_END);
    long fileSize = ftell(sourceFile);

    /* Return mismatch if file size is less than the combined length of HEADER and EOF */
    if (fileSize < sizeof(HEADER_XFILE) + sizeof(EOF_XFILE))
    {
        fclose(sourceFile);
        return returnValue;
    }

    unsigned char bufferHeader[sizeof(HEADER_XFILE)];
    unsigned char bufferEOF[sizeof(EOF_XFILE)];

    /* Read HEADER */
    fseek(sourceFile, 0, SEEK_SET);
    fread(bufferHeader, sizeof(HEADER_XFILE), 1, sourceFile);

    /* Read EOF */
    fseek(sourceFile, -sizeof(EOF_XFILE), SEEK_END);
    fread(bufferEOF, sizeof(EOF_XFILE), 1, sourceFile);

    /* Compare HEADER and EOF */
    returnValue = CompareByteArrays(bufferHeader, HEADER_XFILE, sizeof(HEADER_XFILE)) &&
        CompareByteArrays(bufferEOF, EOF_XFILE, sizeof(EOF_XFILE));

    fclose(sourceFile);
    return returnValue;
}

unsigned char* ReadAllBytes(const char* path, int* fileSize)
{
    FILE* sourceFile = fopen(path, "rb");

    if (!sourceFile)
    {
        *fileSize = 0;
        return NULL;
    }

    fseek(sourceFile, 0, SEEK_END);
    *fileSize = ftell(sourceFile);

    /* Skip HEADER */
    fseek(sourceFile, sizeof(HEADER_XFILE), SEEK_SET);

    /* Read Data */
    unsigned char* fileBytes = malloc(*fileSize - (sizeof(HEADER_XFILE) + sizeof(EOF_XFILE)));
    if (!fileBytes)
    {
        fclose(sourceFile);
        return NULL;
    }

    fread(fileBytes, *fileSize - (sizeof(HEADER_XFILE) + sizeof(EOF_XFILE)), 1, sourceFile);

    fclose(sourceFile);
    return fileBytes;
}


void WriteAllBytes(const char* path, const unsigned char* bytes, int length)
{
    FILE* destinationFile = fopen(path, "wb");

    /* Write HEADER */
    fwrite(HEADER_XFILE, sizeof(HEADER_XFILE), 1, destinationFile);

    /* Write Data */
    fwrite(bytes, length, 1, destinationFile);

    /* Write EOF */
    fwrite(EOF_XFILE, sizeof(EOF_XFILE), 1, destinationFile);

    fclose(destinationFile);
}

void Append(const char* path, const unsigned char* bytes, int length)
{
    FILE* destinationFile = fopen(path, "r+b");

    if (!destinationFile)
    {
        return;
    }

    /* Move the file pointer to the end of the data (right before the EOF) */
    fseek(destinationFile, -sizeof(EOF_XFILE), SEEK_END);

    /* Append the data */
    fwrite(bytes, length, 1, destinationFile);

    /* Write EOF */
    fwrite(EOF_XFILE, sizeof(EOF_XFILE), 1, destinationFile);

    fclose(destinationFile);
}
