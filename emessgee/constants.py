import os
import atexit
import shutil
from uuid import UUID 

TMP_FOLDER = "/tmp/emessgee"
DEFAULT_BUFFER_SIZE = int(1e3)
ID_LEN = 16
WRITING_FLAG_INDEX = 0
HEADER_LENGTH = 4 + 4 + ID_LEN
HEADER_START = 1
HEADER_END = HEADER_START + HEADER_LENGTH
RESERVED_BYTES = 1 + HEADER_LENGTH # BOOL + INT + INT + UUID bytes
STRUCT_FORMAT = f">II{ID_LEN}s"
ID_BYTES_ENDIEN = "big"
INVALID_ID = UUID(int=0)
INVALID_INDEX = 0
INVALID_SIZE = 0
MAX_SANITY_LOOPS = 10000

def __remove_tmp_dir():
    if(os.path.exists(TMP_FOLDER)):
        shutil.rmtree(TMP_FOLDER)

os.makedirs(TMP_FOLDER, exist_ok=True)
atexit.register(__remove_tmp_dir)