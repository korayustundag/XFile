/**
 * @file XFile.c
 * @brief XFile Operations
 *
 * This file contains a set of functions for working with XFile, which is a data structure
 * that includes an 8-byte header and provides operations such as creation, destruction,
 * file writing, file reading, and header comparison.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 /**
  * @struct XFile
  * @brief XFile data structure
  *
  * This structure represents an XFile, which includes an 8-byte header.
  */
typedef struct {
    unsigned char header_xfile[8];
} XFile;

/**
 * @brief Create an XFile with the default header.
 *
 * This function allocates memory for an XFile structure and initializes it with a default header.
 *
 * @return A pointer to the created XFile.
 * @note The caller is responsible for freeing the allocated memory using destroy_xfile.
 */
XFile* create_xfile() {
    XFile* xfile = (XFile*)malloc(sizeof(XFile));
    if (xfile == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    unsigned char default_header[] = { 0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08 };
    memcpy(xfile->header_xfile, default_header, sizeof(default_header));
    return xfile;
}

/**
 * @brief Create an XFile with custom magic bytes.
 *
 * This function allocates memory for an XFile structure and initializes it with the provided magic bytes.
 *
 * @param magic_bytes The custom magic bytes for the XFile.
 * @return A pointer to the created XFile.
 * @note The caller is responsible for freeing the allocated memory using destroy_xfile.
 */
XFile* create_xfile_with_magic_bytes(unsigned char* magic_bytes) {
    if (magic_bytes == NULL) {
        fprintf(stderr, "Magic bytes cannot be NULL.\n");
        exit(1);
    }
    XFile* xfile = (XFile*)malloc(sizeof(XFile));
    if (xfile == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    memcpy(xfile->header_xfile, magic_bytes, 8);
    return xfile;
}

/**
 * @brief Destroy an XFile.
 *
 * This function frees the memory allocated for an XFile structure.
 *
 * @param xfile The XFile to be destroyed.
 */
void destroy_xfile(XFile* xfile) {
    if (xfile != NULL) {
        free(xfile);
    }
}

/**
 * @brief Compare two byte arrays for equality.
 *
 * This function compares two byte arrays for equality up to a specified length.
 *
 * @param array1 The first byte array.
 * @param array2 The second byte array.
 * @param length The length up to which the arrays will be compared.
 * @return 1 if the arrays are equal, 0 otherwise.
 */
int compare_byte_arrays(const unsigned char* array1, const unsigned char* array2, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (array1[i] != array2[i]) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Check if a file is an XFile by comparing its header.
 *
 * This function checks if a file at the specified path is an XFile by comparing its header with
 * the expected XFile header.
 *
 * @param xfile The XFile header to compare against.
 * @param path The path to the file to check.
 * @return 1 if the file is an XFile, 0 otherwise.
 */
int check_is_xfile(const XFile* xfile, const char* path) {
    if (xfile == NULL || path == NULL) {
        return 0;
    }
    FILE* source_file = fopen(path, "rb");
    if (source_file == NULL) {
        return 0;
    }

    fseek(source_file, 0, SEEK_END);
    long file_size = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);

    if (file_size < 8) {
        fclose(source_file);
        return 0;
    }

    unsigned char buffer_header[8];
    fread(buffer_header, sizeof(buffer_header), 1, source_file);

    int result = compare_byte_arrays(buffer_header, xfile->header_xfile, 8);

    fclose(source_file);
    return result;
}

/**
 * @brief Write a byte array to a file, including the XFile header.
 *
 * This function writes a byte array to the specified file, including the XFile header at the beginning
 * of the file. If the file does not exist, it will be created.
 *
 * @param xfile The XFile header to include.
 * @param path The path to the file to write the byte array to.
 * @param bytes The byte array to be written to the file.
 * @param size The size of the byte array.
 */
void write_all_bytes(const XFile* xfile, const char* path, const unsigned char* bytes, size_t size) {
    if (xfile == NULL || path == NULL || bytes == NULL) {
        return;
    }

    FILE* destination_file = fopen(path, "wb");
    if (destination_file == NULL) {
        fprintf(stderr, "Failed to open the destination file for writing.\n");
        exit(1);
    }

    fwrite(xfile->header_xfile, sizeof(xfile->header_xfile), 1, destination_file);
    fwrite(bytes, size, 1, destination_file);

    fclose(destination_file);
}

/**
 * @brief Append a byte array to the end of a file.
 *
 * This function appends a byte array to the end of the specified file. If the file does not exist,
 * it will be created, and the byte array will be added to the existing content at the end of the file.
 *
 * @param path The path to the file to which the byte array will be appended.
 * @param bytes The byte array to be appended to the file.
 * @param size The size of the byte array.
 */
void append(const char* path, const unsigned char* bytes, size_t size) {
    if (path == NULL || bytes == NULL) {
        return;
    }

    FILE* destination_file = fopen(path, "ab");
    if (destination_file == NULL) {
        fprintf(stderr, "Failed to open the destination file for append.\n");
        exit(1);
    }

    fwrite(bytes, size, 1, destination_file);

    fclose(destination_file);
}

/**
 * @brief Write text to a file using UTF-8 encoding, including the XFile header.
 *
 * This function encodes the specified text string using UTF-8 encoding and writes it to the file
 * at the given path, including the XFile header at the beginning of the file. If the file does not
 * exist, it will be created.
 *
 * @param xfile The XFile header to include.
 * @param path The path to the file to write the text to.
 * @param contents The text string to be written to the file.
 */
void write_all_text(const XFile* xfile, const char* path, const char* contents) {
    if (xfile == NULL || path == NULL || contents == NULL) {
        return;
    }

    size_t contents_length = strlen(contents);
    unsigned char* bytes = (unsigned char*)contents;

    write_all_bytes(xfile, path, bytes, contents_length);
}

/**
 * @brief Read the text content of a file, excluding the XFile header.
 *
 * This function reads the text content of the file located at the given path, using UTF-8 encoding,
 * and skips the XFile header at the beginning of the file. If the file does not exist, a
 * FileNotFoundException is thrown.
 *
 * @param xfile The XFile header to skip.
 * @param path The path to the file to read the text from.
 * @return The text content of the file, excluding the XFile header.
 */
char* read_all_text(const XFile* xfile, const char* path) {
    if (xfile == NULL || path == NULL) {
        return NULL;
    }

    FILE* source_file = fopen(path, "rb");
    if (source_file == NULL) {
        fprintf(stderr, "Failed to open the source file for reading.\n");
        exit(1);
    }

    fseek(source_file, 8, SEEK_SET);
    fseek(source_file, 0, SEEK_END);
    long file_size = ftell(source_file);
    fseek(source_file, 8, SEEK_SET);

    char* file_contents = (char*)malloc(file_size - 8);
    if (file_contents == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    fread(file_contents, file_size - 8, 1, source_file);
    fclose(source_file);

    return file_contents;
}

// Example
int main() {
    XFile* xfile = create_xfile();
    unsigned char custom_header[] = { 0x1A, 0x0F, 0x0E, 0x0B, 0xA5, 0x10, 0x00, 0x80 };
    XFile* xfile_custom = create_xfile_with_magic_bytes(custom_header);

    write_all_text(xfile, "default.dat", "This is test1!");
    write_all_text(xfile_custom, "custom.dat", "This is test2!");

    destroy_xfile(xfile);
    destroy_xfile(xfile_custom);

    return 0;
}