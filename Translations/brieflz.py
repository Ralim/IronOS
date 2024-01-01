import ctypes
import functools
import os
from pathlib import Path

HERE = Path(__file__).resolve().parent


@functools.lru_cache(maxsize=None)
def _libbrieflz():
    so_path = os.path.join(HERE, "../source/Objects/host/brieflz/libbrieflz.so")
    libbrieflz = ctypes.cdll.LoadLibrary(so_path)
    return libbrieflz


@functools.lru_cache(maxsize=None)
def _fn_blz_max_packed_size():
    """Returns the blz_max_packed_size C function.
    ::

        /**
        * Get bound on compressed data size.
        *
        * @see blz_pack
        *
        * @param src_size number of bytes to compress
        * @return maximum size of compressed data
        */
        BLZ_API size_t
        blz_max_packed_size(size_t src_size);
    """

    fn = _libbrieflz().blz_max_packed_size
    fn.argtype = [
        ctypes.c_size_t,
    ]
    fn.restype = ctypes.c_size_t
    return fn


def blz_max_packed_size(src_size: int) -> int:
    """Get bound on compressed data size."""
    fn_blz_max_packed_size = _fn_blz_max_packed_size()
    return int(fn_blz_max_packed_size(src_size))


@functools.lru_cache(maxsize=None)
def _fn_blz_workmem_size_level():
    """Returns the blz_workmem_size_level C function.
    ::

        /**
        * Get required size of `workmem` buffer.
        *
        * @see blz_pack_level
        *
        * @param src_size number of bytes to compress
        * @param level compression level
        * @return required size in bytes of `workmem` buffer
        */
        BLZ_API size_t
        blz_workmem_size_level(size_t src_size, int level);
    """

    fn = _libbrieflz().blz_workmem_size_level
    fn.argtype = [
        ctypes.c_size_t,
        ctypes.c_int,
    ]
    fn.restype = ctypes.c_size_t
    return fn


def blz_workmem_size_level(src_size: int, level: int) -> int:
    """Get required size of `workmem` buffer."""
    fn_blz_workmem_size_level = _fn_blz_workmem_size_level()
    return int(fn_blz_workmem_size_level(src_size, level))


@functools.lru_cache(maxsize=None)
def _fn_blz_pack_level():
    """Returns the blz_pack_level C function.
    ::

        /**
        * Compress `src_size` bytes of data from `src` to `dst`.
        *
        * Compression levels between 1 and 9 offer a trade-off between
        * time/space and ratio. Level 10 is optimal but very slow.
        *
        * @param src pointer to data
        * @param dst pointer to where to place compressed data
        * @param src_size number of bytes to compress
        * @param workmem pointer to memory for temporary use
        * @param level compression level
        * @return size of compressed data
        */
        BLZ_API unsigned long
        blz_pack_level(const void *src, void *dst, unsigned long src_size,
                       void *workmem, int level);
    """

    fn = _libbrieflz().blz_pack_level
    fn.argtype = [
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_ulong,
        ctypes.c_char_p,
        ctypes.c_int,
    ]
    fn.restype = ctypes.c_ulong
    return fn


def compress(data: bytes) -> bytes:
    """Returns a bytes object of the brieflz-compressed data."""

    fn_blz_pack_level = _fn_blz_pack_level()

    output_buffer_len = blz_max_packed_size(len(data))

    src = data
    dst = ctypes.create_string_buffer(output_buffer_len)
    src_size = len(src)
    workmem = ctypes.create_string_buffer(blz_workmem_size_level(len(data), 10))
    level = 10

    res = fn_blz_pack_level(src, dst, src_size, workmem, level)

    if res == 0:
        raise BriefLZError()
    else:
        return bytes(dst[:res])  # type: ignore


@functools.lru_cache(maxsize=None)
def _fn_blz_depack_srcsize():
    """Returns the blz_depack_srcsize C function.
    ::

        /**
        * Decompress `src_size` bytes of data from `src` to `dst`.
        *
        * This function is unsafe. If the provided data is malformed, it may
        * read more than `src_size` from the `src` buffer.
        *
        * @param src pointer to compressed data
        * @param dst pointer to where to place decompressed data
        * @param src_size size of the compressed data
        * @return size of decompressed data
        */
        BLZ_API unsigned long
        blz_depack_srcsize(const void *src, void *dst, unsigned long src_size);
    """

    fn = _libbrieflz().blz_depack_srcsize
    fn.argtype = [
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_ulong,
    ]
    fn.restype = ctypes.c_ulong
    return fn


def depack_srcsize(data: bytes, expected_depack_size: int) -> bytes:
    """Returns a bytes object of the uncompressed data."""

    fn_blz_depack_srcsize = _fn_blz_depack_srcsize()

    output_buffer_len = expected_depack_size * 2

    src = data
    dst = ctypes.create_string_buffer(output_buffer_len)
    src_size = len(src)

    res = fn_blz_depack_srcsize(src, dst, src_size)

    if res == 0:
        raise BriefLZError()
    else:
        return bytes(dst[:res])  # type: ignore


class BriefLZError(Exception):
    """Exception raised for brieflz compression or decompression error."""

    def __init__(self):
        pass
