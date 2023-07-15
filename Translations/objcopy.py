import os
import subprocess
import tempfile


if "OBJCOPY" in os.environ:
    OBJCOPY = os.environ["OBJCOPY"]
else:
    OBJCOPY = "objcopy"


def get_binary_from_obj(objfile_path: str, section_name: str) -> bytes:
    tmpfd, tmpfile = tempfile.mkstemp()
    result = subprocess.run(
        [OBJCOPY, "-O", "binary", "-j", section_name, objfile_path, tmpfile]
    )
    result.check_returncode()
    with open(tmpfd, "rb") as f:
        bin: bytes = f.read()
    os.remove(tmpfile)
    return bin


def cpp_var_to_section_name(var_name: str) -> str:
    return f".rodata._ZL{len(var_name)}{var_name}"
