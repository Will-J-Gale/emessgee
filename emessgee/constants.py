import os
import atexit
import shutil
from enum import Enum
from uuid import UUID 

class ReservedIndexes(Enum):
    BLOCK_READY = 0
    WRITING = 1
    QUEUE_SIZE = 2

TMP_FOLDER = "/tmp/emessgee"
DEFAULT_BUFFER_SIZE = int(1e3)
ID_LEN = 16
HEADER_LENGTH = 4 + 4 + ID_LEN #INT + INT + UUID bytes
HEADER_START = len(ReservedIndexes)
HEADER_FORMAT = f">II{ID_LEN}s"
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

