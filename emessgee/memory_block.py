import os
import mmap
from typing import Union

from emessgee.constants import TMP_FOLDER, DEFAULT_BUFFER_SIZE, MAX_SANITY_LOOPS
from emessgee.exceptions import (
    WriteQueueAlreadyExistsError, ErrorMessages,
    DataNotBytesOrStringError, MemoryBlockIsReadOnlyError,
    MMapFileExistsButNotYetTruncatedError, InvalidByteDataError,
    FailedCreatingMmapBufferError
)

class WriteMemoryBlock:
    def __init__(self, name:str, size:int = DEFAULT_BUFFER_SIZE):
        self._name = name
        self._filepath = os.path.join(TMP_FOLDER, name)

        if(os.path.exists(self._filepath)):
            raise WriteQueueAlreadyExistsError(
                ErrorMessages.PUBLISHER_ALREADY_EXISTS.format(topic=name)
            )

        self._file_descriptor = os.open(self._filepath, os.O_CREAT | os.O_RDWR)

        os.truncate(self._file_descriptor, size)

        self._buffer = mmap.mmap(self._file_descriptor, 0, mmap.MAP_SHARED)
        self._size = size

    def write(self, index:int, data:Union[bytes, str]):
        if(type(data) not in [bytes, str]):
            raise DataNotBytesOrStringError(
                ErrorMessages.DATA_NOT_BYTES.format(type=type(data))
            )
        
        data_bytes = data if isinstance(data, bytes) else data.encode()

        end = index + len(data_bytes)
        self._buffer[index:end] = data_bytes

    def write_byte(self, index:int, value:int):
        try:
            self._buffer[index] = value
        except ValueError as e:
            raise InvalidByteDataError(str(e))

    def close(self):
        self._buffer.close()
        if(os.path.exists(self._filepath)):
            os.remove(self._filepath)


class ReadMemoryBlock:
    def __init__(self, name:str):
        self._name = name
        self._filepath = os.path.join(TMP_FOLDER, name)
        self._file_descriptor = os.open(self._filepath, os.O_RDWR)
        self._buffer = None

        try:
            self._buffer = mmap.mmap(self._file_descriptor, 0, mmap.MAP_SHARED)
        except ValueError:
            os.close(self._file_descriptor)
            raise FailedCreatingMmapBufferError
        
    def close(self):
        self._buffer.close()
    
    def read(self, index:int, size:int=1):
        return self._buffer[index:index+size]
