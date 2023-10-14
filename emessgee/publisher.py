import os
import mmap
import atexit
from struct import pack
from typing import Union
from uuid import uuid4, UUID

from emessgee.exceptions import (
    DataNotBytesOrStringError, DataTooLargeError, PublisherAlreadyExistsError,
    ErrorMessages,
)
from emessgee.constants import (
    TMP_FOLDER, DEFAULT_BUFFER_SIZE, WRITING_FLAG_INDEX, HEADER_START, 
    HEADER_END, RESERVED_BYTES,STRUCT_FORMAT, INVALID_ID, INVALID_INDEX, INVALID_SIZE
)

class Publisher:
    def __init__(self, topic:str, buffer_size:int = DEFAULT_BUFFER_SIZE):
        self._topic = topic
        self._topic_filepath = os.path.join(TMP_FOLDER, topic)

        if(os.path.exists(self._topic_filepath)):
            raise PublisherAlreadyExistsError(
                ErrorMessages.PUBLISHER_ALREADY_EXISTS.format(topic=topic)
            )

        self._file_descriptor = os.open(self._topic_filepath, os.O_CREAT | os.O_RDWR)
        self._write_index = RESERVED_BYTES
        os.truncate(self._file_descriptor, buffer_size + RESERVED_BYTES)
        self._buffer = mmap.mmap(self._file_descriptor, 0, mmap.MAP_SHARED)
        self._buffer_size = buffer_size

        self._write_header(INVALID_INDEX, INVALID_SIZE, INVALID_ID)
        atexit.register(self.close)
    
    def send(self, data:Union[bytes, str]):
        if(type(data) not in [bytes, str]):
            raise DataNotBytesOrStringError(
                ErrorMessages.DATA_NOT_BYTES.format(type=type(data))
            )

        data_bytes = data if isinstance(data, bytes) else data.encode()
        message_size = len(data_bytes)
        
        if(message_size > self._buffer_size):
            raise DataTooLargeError(
                ErrorMessages.DATA_TOO_LARGE.format(
                    data_size=message_size, buffer_size=self._buffer_size
                )
            )

        self._begin_write()
        start_index = self._write_data(data_bytes, message_size)
        self._write_header(start_index, message_size, uuid4())
        self._end_write()

    def close(self):
        self._buffer.close()
        if(os.path.exists(self._topic_filepath)):
            os.remove(self._topic_filepath) 

    def _write_header(self, index:int, message_size:int, message_id:UUID):
        header = pack(STRUCT_FORMAT, index, message_size, message_id.bytes)
        self._buffer[HEADER_START:HEADER_END] = header
    
    def _write_data(self, data:bytes, size:int):
        end = self._write_index + size

        if(end >= self._buffer_size):
            self._write_index = RESERVED_BYTES
            end = self._write_index + size

        start_index = self._write_index
        self._buffer[start_index:end] = data
        self._write_index = end
        return start_index
    
    def _begin_write(self):
        self._buffer[WRITING_FLAG_INDEX] = 1

    def _end_write(self):
        self._buffer[WRITING_FLAG_INDEX] = 0