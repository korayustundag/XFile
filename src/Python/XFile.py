#####################################################################################
# MIT License                                                                       #
#                                                                                   #
# Copyright (c) 2023 Koray Üstündağ                                                 #
#                                                                                   #
# Permission is hereby granted, free of charge, to any person obtaining a copy      #
# of this software and associated documentation files (the "Software"), to deal     #
# in the Software without restriction, including without limitation the rights      #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         #
# copies of the Software, and to permit persons to whom the Software is             #
# furnished to do so, subject to the following conditions:                          #
#                                                                                   #
# The above copyright notice and this permission notice shall be included in all    #
# copies or substantial portions of the Software.                                   #
#                                                                                   #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     #
# SOFTWARE.                                                                         #
#####################################################################################

import os

class XFile:
    HEADER_XFILE = bytes([0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08])
    EOF_XFILE = bytes([0x08, 0x00, 0x01, 0x5A, 0x0F, 0x0B, 0x07, 0x2A])

    @staticmethod
    def compare_byte_arrays(array1, array2):
        return len(array1) == len(array2) and all(a == b for a, b in zip(array1, array2))

    @staticmethod
    def check_is_xfile(path):
        if not os.path.exists(path):
            return False

        with open(path, "rb") as file:
            file_size = os.path.getsize(path)

            # Return mismatch if file size is less than the combined length of HEADER and EOF
            if file_size < len(XFile.HEADER_XFILE) + len(XFile.EOF_XFILE):
                return False

            # Read HEADER
            buffer_header = file.read(len(XFile.HEADER_XFILE))

            # Read EOF
            file.seek(-len(XFile.EOF_XFILE), os.SEEK_END)
            buffer_eof = file.read(len(XFile.EOF_XFILE))

            # Compare HEADER and EOF
            return XFile.compare_byte_arrays(buffer_header, XFile.HEADER_XFILE) and \
                   XFile.compare_byte_arrays(buffer_eof, XFile.EOF_XFILE)

    @staticmethod
    def read_all_bytes(path):
        if not os.path.exists(path):
            raise FileNotFoundError("File Not Found!", path)

        if not XFile.check_is_xfile(path):
            raise Exception("File: \"" + os.path.abspath(path) + "\" is not XFile")

        with open(path, "rb") as file:
            file_size = os.path.getsize(path)

            file.seek(len(XFile.HEADER_XFILE), os.SEEK_SET)

            # Read Data
            file_bytes = file.read(file_size - (len(XFile.HEADER_XFILE) + len(XFile.EOF_XFILE)))

        return file_bytes

    @staticmethod
    def write_all_bytes(path, bytes_data):
        with open(path, "wb") as file:
            # Write HEADER
            file.write(XFile.HEADER_XFILE)

            # Write Data
            file.write(bytes_data)

            # Write EOF
            file.write(XFile.EOF_XFILE)

    @staticmethod
    def append(path, bytes_data):
        if not os.path.exists(path):
            raise FileNotFoundError("File Not Found!", path)

        if not XFile.check_is_xfile(path):
            raise Exception("File: \"" + os.path.abspath(path) + "\" is not XFile")

        with open(path, "r+b") as file:
            # Move the file pointer to the end of the data (right before the EOF)
            data_end_position = file.seek(0, os.SEEK_END) - len(XFile.EOF_XFILE)

            # Append the data
            file.seek(data_end_position, os.SEEK_SET)
            file.write(bytes_data)
            file.write(XFile.EOF_XFILE)
