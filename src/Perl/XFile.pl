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

use strict;
use warnings;
use File::Basename;

my $HEADER_XFILE = "\x2A\x07\x0B\x0F\x5A\x01\x00\x08";
my $EOF_XFILE = "\x08\x00\x01\x5A\x0F\x0B\x07\x2A";

sub compare_byte_arrays {
    my ($array1, $array2) = @_;
    return length($array1) == length($array2) && $array1 eq $array2;
}

sub check_is_xfile {
    my ($path) = @_;

    if (! -e $path) {
        return 0;
    }

    open(my $file, "<", $path) or die "Cannot open file: $!";
    binmode($file);

    my $file_size = -s $file;

    # Return mismatch if file size is less than the combined length of HEADER and EOF
    if ($file_size < length($HEADER_XFILE) + length($EOF_XFILE)) {
        close($file);
        return 0;
    }

    # Read HEADER
    my $buffer_header;
    read($file, $buffer_header, length($HEADER_XFILE));

    # Read EOF
    seek($file, -$file_size, 2);
    my $buffer_eof;
    read($file, $buffer_eof, length($EOF_XFILE));

    # Compare HEADER and EOF
    my $is_xfile = compare_byte_arrays($buffer_header, $HEADER_XFILE) &&
                   compare_byte_arrays($buffer_eof, $EOF_XFILE);

    close($file);
    return $is_xfile;
}

sub read_all_bytes {
    my ($path) = @_;

    if (! -e $path) {
        die "File Not Found!";
    }

    if (! check_is_xfile($path)) {
        die "File: \"" . File::Spec->rel2abs($path) . "\" is not XFile";
    }

    open(my $file, "<", $path) or die "Cannot open file: $!";
    binmode($file);

    my $file_size = -s $file;

    seek($file, length($HEADER_XFILE), 0);

    # Read Data
    my $file_bytes;
    read($file, $file_bytes, $file_size - (length($HEADER_XFILE) + length($EOF_XFILE)));

    close($file);
    return $file_bytes;
}

sub write_all_bytes {
    my ($path, $bytes) = @_;

    open(my $file, ">", $path) or die "Cannot open file: $!";
    binmode($file);

    # Write HEADER
    print $file $HEADER_XFILE;

    # Write Data
    print $file $bytes;

    # Write EOF
    print $file $EOF_XFILE;

    close($file);
}

sub append {
    my ($path, $bytes) = @_;

    if (! -e $path) {
        die "File Not Found!";
    }

    if (! check_is_xfile($path)) {
        die "File: \"" . File::Spec->rel2abs($path) . "\" is not XFile";
    }

    open(my $file, "+<", $path) or die "Cannot open file: $!";
    binmode($file);

    # Move the file pointer to the end of the data (right before the EOF)
    my $data_end_position = seek($file, 0, 2) - length($EOF_XFILE);

    # Append the data
    seek($file, $data_end_position, 0);
    print $file $bytes;
    print $file $EOF_XFILE;

    close($file);
}
