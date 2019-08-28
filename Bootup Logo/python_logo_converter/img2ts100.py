#!/usr/bin/env python
# coding=utf-8
from __future__ import division
import os
import sys


try:
    from PIL import Image, ImageOps
except ImportError as error:
    raise ImportError("{}: {} requres Python Imaging Library (PIL). "
                      "Install with `pip` or OS-specific package "
                      "management tool."
                      .format(error, sys.argv[0]))

VERSION_STRING = '0.01'

LCD_WIDTH       = 96
LCD_HEIGHT      = 16
LCD_NUM_BYTES   = LCD_WIDTH * LCD_HEIGHT // 8
LCD_PADDED_SIZE = 1024

INTELHEX_DATA_RECORD                    = 0x00
INTELHEX_END_OF_FILE_RECORD             = 0x01
INTELHEX_EXTENDED_LINEAR_ADDRESS_RECORD = 0x04
INTELHEX_BYTES_PER_LINE                 = 16
INTELHEX_MINIMUM_SIZE                   = 4096


def split16(word):
    """return high and low byte of 16-bit word value as tuple"""
    return (word >> 8) & 0xff, word & 0xff


def intel_hex_line(record_type, offset, data):
    """generate a line of data in Intel hex format"""
    # length, address offset, record type
    record_length = len(data)
    yield ':{:02X}{:04X}{:02X}'.format(record_length, offset, record_type)

    # data
    for byte in data:
        yield "{:02X}".format(byte)

    # compute and write checksum (now using unix style line endings for DFU3.45 compatibility
    yield "{:02X}\n".format((((sum(data,                   # sum data ...
                                     record_length           # ... and other ...
                                     + sum(split16(offset))  # ... fields ...
                                     + record_type)          # ... on line
                                 & 0xff)                     # low 8 bits
                                ^ 0xff)                      # two's ...
                               + 1)                          # ... complement
                              & 0xff)                        # low 8 bits


def intel_hex(file, bytes_, start_address=0x0):
    """write block of data in Intel hex format"""
    def write(generator):
        file.write(''.join(generator))

    if len(bytes_) % INTELHEX_BYTES_PER_LINE != 0:
        raise ValueError("Program error: Size of LCD data is not evenly divisible by {}"
                         .format(INTELHEX_BYTES_PER_LINE))

    address_lo =  start_address        & 0xffff
    address_hi = (start_address >> 16) & 0xffff

    write(intel_hex_line(INTELHEX_EXTENDED_LINEAR_ADDRESS_RECORD, 0,
                               split16(address_hi)))

    size_written = 0
    while size_written < INTELHEX_MINIMUM_SIZE:
        offset = address_lo
        for line_start in range(0, len(bytes_), INTELHEX_BYTES_PER_LINE):
            write(intel_hex_line(INTELHEX_DATA_RECORD, offset,
                                 bytes_[line_start:line_start + INTELHEX_BYTES_PER_LINE]))
            size_written += INTELHEX_BYTES_PER_LINE
            if size_written >= INTELHEX_MINIMUM_SIZE:
                break
            offset += INTELHEX_BYTES_PER_LINE

    write(intel_hex_line(INTELHEX_END_OF_FILE_RECORD, 0, ()))


def img2hex(input_filename,
            output_file,
            preview_filename=None,
            threshold=128,
            dither=False,
            negative=False):
    """
    Convert 'input_filename' image file into Intel hex format with data
        formatted for display on TS100 LCD and file object.
    Input image is converted from color or grayscale to black-and-white,
        and resized to fit TS100 LCD screen as necessary.
    Optionally write resized/thresholded/black-and-white preview image
        to file specified by name.
    Optional `threshold' argument 8 bit value; grayscale pixels greater than
        this become 1 (white) in output, less than become 0 (black).
    Unless optional `dither', in which case PIL grayscale-to-black/white
        dithering algorithm used.
    Optional `negative' inverts black/white regardless of input image type
        or other options.
    """

    try:
        image = Image.open(input_filename)
    except BaseException as e:
        raise IOError("error reading image file \"{}\": {}".format(input_filename, e))

    # convert to luminance
    # do even if already black/white because PIL can't invert 1-bit so
    #   can't just pass thru in case --negative flag
    # also resizing works better in luminance than black/white
    # also no information loss converting black/white to grayscale
    if image.mode != 'L':
        image = image.convert('L')

    if image.size != (LCD_WIDTH, LCD_HEIGHT):
        image = image.resize((LCD_WIDTH, LCD_HEIGHT), Image.BICUBIC)

    if negative:
        image = ImageOps.invert(image)
        threshold = 255 - threshold  # have to invert threshold

    if dither:
        image = image.convert('1')
    else:
        image = image.point(lambda pixel: 0 if pixel < threshold else 1, '1')

    if preview_filename:
        image.save(preview_filename)

    ''' DEBUG
    for row in range(LCD_HEIGHT):
        for column in range(LCD_WIDTH):
            if image.getpixel((column, row)): sys.stderr.write('1')
            else:                             sys.stderr.write('0')
        sys.stderr.write('\n')
    '''

    # pad to this size (also will be repeated in output Intel hex file)
    data = [0] * LCD_PADDED_SIZE

    # magic/undocumented/required header in endian-reverse byte order
    data[0] = 0x55
    data[1] = 0xAA
    data[2] = 0x0D
    data[3] = 0xF0

    # convert to TS100 LCD format
    for ndx in range(LCD_WIDTH * 16 // 8):
        bottom_half_offset = 0 if ndx < LCD_WIDTH else 8
        byte = 0
        for y in range(8):
            if image.getpixel((ndx % LCD_WIDTH, y + bottom_half_offset)):
                byte |= 1 << y
        # store in endian-reversed byte order
        data[4 + ndx + (1 if ndx % 2 == 0 else -1)] = byte

    intel_hex(output_file, data, 0x0800F800)


def parse_commandline():
    parser = argparse.ArgumentParser(
                formatter_class=argparse.ArgumentDefaultsHelpFormatter,
                description="Convert image file for display on TS100 LCD "
                            "at startup")

    def zero_to_255(text):
        value = int(text)
        if not 0 <= value <= 255:
            raise argparse.ArgumentTypeError("must be integer from 0 to 255 ")
        return value

    parser.add_argument('input_filename',
                        help="input image file")

    parser.add_argument('output_filename',
                        help="output Intel hex file")

    parser.add_argument('-p', '--preview',
                        help="filename of image preview (same data as "
                             "Intel hex file, as will appear on TS100 LCD)")

    parser.add_argument('-n', '--negative',
                        action='store_true',
                        help="photo negative: exchange black and white "
                              "in output")

    parser.add_argument('-t', '--threshold',
                        type=zero_to_255,
                        default=128,
                        help="0 to 255: gray (or color converted to gray) "
                             "above this becomes white, below becomes black; "
                             "ignored if using --dither")

    parser.add_argument('-d', '--dither',
                        action='store_true',
                        help="use dithering (speckling) to convert gray or "
                             "color to black and white")

    parser.add_argument('-f', '--force',
                        action='store_true',
                        help="force overwriting of existing files")

    parser.add_argument('-v', '--version',
                        action='version',
                        version="%(prog)s version " + VERSION_STRING,
                        help="print version info")

    return parser.parse_args()


if __name__ == "__main__":
    import argparse
    args = parse_commandline()

    if os.path.exists(args.output_filename) and not args.force:
        sys.stderr.write("Won't overwrite existing file \"{}\" (use --force "
                         "option to override)\n"
                         .format(args.output_filename))
        sys.exit(1)

    if args.preview and os.path.exists(args.preview) and not args.force:
        sys.stderr.write("Won't overwrite existing file \"{}\" (use --force "
                         "option to override)\n"
                         .format(args.preview))
        sys.exit(1)

    try:
        with open(args.output_filename, 'w', newline='\r\n') as output:
            img2hex(args.input_filename,
                    output,
                    args.preview,
                    args.threshold,
                    args.dither,
                    args.negative)
    except BaseException as error:
        sys.stderr.write("Error converting file: {}\n".format(error))
        sys.exit(1)
