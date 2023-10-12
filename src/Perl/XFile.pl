use strict;
use warnings;
use IO::File;

package XFile;

sub new {
    my ($class, $magic_bytes) = @_;

    my $self = {
        header_xfile => $magic_bytes ? $magic_bytes : [0x2A, 0x07, 0x0B, 0x0F, 0x5A, 0x01, 0x00, 0x08]
    };

    bless $self, $class;
    return $self;
}

sub compare_byte_arrays {
    my ($array1, $array2) = @_;

    return @$array1 ~~ @$array2;  # Smart match operator for array comparison
}

sub check_is_xfile {
    my ($self, $path) = @_;

    return 0 if !-e $path;

    my $source_file = IO::File->new($path, "r");
    my $buffer_header;
    $source_file->read($buffer_header, 8);

    return $self->compare_byte_arrays($buffer_header, $self->{header_xfile});
}

sub write_all_bytes {
    my ($self, $path, $bytes) = @_;

    my $destination_file = IO::File->new($path, "w");
    $destination_file->binmode(":raw");
    $destination_file->print(pack('C*', @{$self->{header_xfile}});
    $destination_file->print($bytes);
    $destination_file->close;
}

sub append {
    my ($self, $path, $bytes) = @_;

    my $destination_file = IO::File->new($path, "a");
    $destination_file->binmode(":raw");
    $destination_file->print($bytes);
    $destination_file->close;
}

sub write_all_text {
    my ($self, $path, $contents, $encoding) = @_;

    $self->write_all_bytes($path, $contents);
}

sub write_all_lines {
    my ($self, $path, $lines) = @_;

    my $contents = join("\n", @$lines);
    $self->write_all_text($path, $contents);
}

sub read_all_text {
    my ($self, $path) = @_;

    my $source_file = IO::File->new($path, "r");
    my $buffer_header;
    $source_file->read($buffer_header, 8);

    my $contents = do { local $/; $source_file->getline };

    return $contents;
}

sub read_all_lines {
    my ($self, $path) = @_;

    my $text = $self->read_all_text($path);
    my @lines = split(/\n/, $text);

    return \@lines;
}

1;

package main;

my $xfile = XFile->new();
my $xfile_custom = XFile->new([0x1A, 0x0F, 0x0E, 0x0B, 0xA5, 0x10, 0x00, 0x80]);

# Use the XFile functions here

print "Magic Bytes: " . join(", ", @{$xfile->get_magic_bytes()}) . "\n";
print "Custom Magic Bytes: " . join(", ", @{$xfile_custom->get_magic_bytes()}) . "\n";

