#!python3

"""
The FNIRSI HS-02 firmware files are `.bin` files with a header lead in.
It appears the header is 16 bytes:
- "bin\0" [4 bytes]
- sector size of the file [4 bytes]
- 0 [4 bytes]
- 0 [4 bytes]

There appears to be no checksum on the file (byte modding strings apply directly).
If I were to guess, that might be what the last two 0's are for
"""

import struct
import sys


def add_header_to_file(input_filename, output_filename=None):
    """
    Add a 16-byte header to the beginning of a file.
    Header format:
    - "bin\0" [4 bytes]
    - sector size of the file [4 bytes]
    - 0 [4 bytes]
    - 0 [4 bytes]
    """
    if output_filename is None:
        output_filename = input_filename.replace(".bin", "_with_header.bin")

    try:
        # Read the original file
        with open(input_filename, "rb") as infile:
            original_data = infile.read()

        # Get file size for the header
        file_size = len(original_data)
        # Pad up to 2k sector size
        file_size += 2048 - (file_size % 2048)
        # Create the 16-byte header
        header = b"bin\0"  # 4 bytes
        header += struct.pack("<I", file_size)  # 4 bytes, little-endian unsigned int
        header += struct.pack("<I", 0)  # 4 bytes of zeros
        header += struct.pack("<I", 0)  # 4 bytes of zeros

        # Write header + original data to output file
        with open(output_filename, "wb") as outfile:
            outfile.write(header)
            outfile.write(original_data)

        print(f"Successfully added header to {input_filename}")
        print(f"Output saved as: {output_filename}")

    except FileNotFoundError:
        print(f"Error: File '{input_filename}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error processing file: {e}")
        sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input_file> [output_file]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    add_header_to_file(input_file, output_file)
