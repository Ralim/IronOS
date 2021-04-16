import ctypes
import functools
import os
from pathlib import Path

HERE = Path(__file__).resolve().parent


@functools.lru_cache(maxsize=None)
def _liblzfx():
    so_path = os.path.join(HERE, "../source/Objects/host/lzfx/liblzfx.so")
    liblzfx = ctypes.cdll.LoadLibrary(so_path)
    return liblzfx


@functools.lru_cache(maxsize=None)
def _fn_lzfx_compress():
    """Returns the lzfx_compress C function.
    ::

        /*  Buffer-to buffer compression.

            Supply pre-allocated input and output buffers via ibuf and obuf, and
            their size in bytes via ilen and olen.  Buffers may not overlap.

            On success, the function returns a non-negative value and the argument
            olen contains the compressed size in bytes.  On failure, a negative
            value is returned and olen is not modified.
        */
        int lzfx_compress(const void* ibuf, unsigned int ilen,
                                void* obuf, unsigned int *olen);
    """

    fn = _liblzfx().lzfx_compress
    fn.argtype = [
        ctypes.c_char_p,
        ctypes.c_uint,
        ctypes.c_char_p,
        ctypes.POINTER(ctypes.c_uint),
    ]
    fn.restype = ctypes.c_int
    return fn


def compress(data: bytes) -> bytes:
    """Returns a bytes object of the lzfx-compressed data."""

    fn_compress = _fn_lzfx_compress()

    output_buffer_len = len(data) + 8

    ibuf = data
    ilen = len(ibuf)
    obuf = ctypes.create_string_buffer(output_buffer_len)
    olen = ctypes.c_uint(output_buffer_len)

    res = fn_compress(ibuf, ilen, obuf, ctypes.byref(olen))

    if res < 0:
        raise LzfxError(res)
    else:
        return bytes(obuf[: olen.value])  # type: ignore


class LzfxError(Exception):
    """Exception raised for lzfx compression or decompression error.

    Attributes:
        error_code -- The source error code, which is a negative integer
        error_name -- The constant name of the error
        message -- explanation of the error
    """

    # define LZFX_ESIZE      -1      /* Output buffer too small */
    # define LZFX_ECORRUPT   -2      /* Invalid data for decompression */
    # define LZFX_EARGS      -3      /* Arguments invalid (NULL) */

    def __init__(self, error_code):
        self.error_code = error_code
        if error_code == -1:
            self.error_name = "LZFX_ESIZE"
            self.message = "Output buffer too small"
        elif error_code == -2:
            self.error_name = "LZFX_ECORRUPT"
            self.message = "Invalid data for decompression"
        elif error_code == -3:
            self.error_name = "LZFX_EARGS"
            self.message = "Arguments invalid (NULL)"
        else:
            self.error_name = "UNKNOWN"
            self.message = "Unknown error"
