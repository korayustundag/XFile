import os
import struct

class XFile:
    def __init__(self, magic_bytes=None):
        if magic_bytes is None:
            self.header_xfile = bytes([0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08])
        else:
            if len(magic_bytes) != 8:
                raise ValueError("Magic bytes must be 8 bytes long.")
            self.header_xfile = bytes(magic_bytes)

    def get_magic_bytes(self):
        return list(self.header_xfile)

    @staticmethod
    def compare_byte_arrays(array1, array2):
        return array1 == array2

    def check_is_xfile(self, path):
        if not os.path.exists(path):
            return False

        with open(path, "rb") as source_file:
            buffer_header = source_file.read(8)
            return self.compare_byte_arrays(buffer_header, self.header_xfile)

    def write_all_bytes(self, path, bytes):
        with open(path, "wb") as destination_file:
            destination_file.write(self.header_xfile)
            destination_file.write(bytes)

    def append(self, path, bytes):
        with open(path, "ab") as destination_file:
            destination_file.write(bytes)

    def write_all_text(self, path, contents, encoding="utf-8"):
        self.write_all_bytes(path, contents.encode(encoding))

    def write_all_lines(self, path, lines):
        contents = "\n".join(lines)
        self.write_all_text(path, contents)

    def read_all_text(self, path, encoding="utf-8"):
        with open(path, "rb") as source_file:
            source_file.seek(8)
            return source_file.read().decode(encoding)

    def read_all_lines(self, path, encoding="utf-8"):
        text = self.read_all_text(path, encoding)
        return text.splitlines()

if __name__ == "__main__":
    xfile = XFile()
    xfile_custom = XFile([0x1A, 0x0F, 0x0E, 0x0B, 0xA5, 0x10, 0x00, 0x80])

    # Use the XFile functions here

    print(xfile.get_magic_bytes())
    print(xfile_custom.get_magic_bytes())
