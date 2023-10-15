import os
import mmap
from typing import Union

from emessgee.constants import TMP_FOLDER, DEFAULT_BUFFER_SIZE
from emessgee.exceptions import (
    PublisherAlreadyExistsError, ErrorMessages,
    DataNotBytesOrStringError, MemoryBlockIsReadOnlyError,
    MMapFileExistsButNotYetTruncatedError
)

class MemoryBlock:
    def __init__(self, name:str, size:int = DEFAULT_BUFFER_SIZE, create=False):
        self._name = name
        self._filepath = os.path.join(TMP_FOLDER, name)
        self._read_only = not create

        if(create and os.path.exists(self._filepath)):
            raise PublisherAlreadyExistsError(ErrorMessages.PUBLISHER_ALREADY_EXISTS)

        flags = os.O_CREAT | os.O_RDWR if create else os.O_RDWR
        self._file_descriptor = os.open(self._filepath, flags)

        if(create): os.truncate(self._file_descriptor, size)

        try:
            self._buffer = mmap.mmap(self._file_descriptor, 0, mmap.MAP_SHARED)
            self._size = size
        except ValueError:
            raise MMapFileExistsButNotYetTruncatedError

    def get_buffer(self):
        return self._buffer
    
    def get_buffer_size(self):
        return self._size

    def read(self, index:int, size:int=1):
        return self._buffer[index:index+size]
    
    def write(self, index:int, data:Union[bytes, str]):
        if(self._read_only):
            raise MemoryBlockIsReadOnlyError(ErrorMessages.MEMORY_BLOCK_IS_READ_ONLY)

        if(type(data) not in [bytes, str]):
            raise DataNotBytesOrStringError(
                ErrorMessages.DATA_NOT_BYTES.format(type=type(data))
            )
        
        data_bytes = data if isinstance(data, bytes) else data.encode()

        end = index + len(data_bytes)
        self._buffer[index:end] = data_bytes

    def write_flag(self, index:int, state:bool):
        if(self._read_only):
            raise MemoryBlockIsReadOnlyError(ErrorMessages.MEMORY_BLOCK_IS_READ_ONLY)

        self._buffer[index] = int(state)

    def close(self):
        self._buffer.close()
        if(os.path.exists(self._filepath)):
            os.remove(self._filepath)




