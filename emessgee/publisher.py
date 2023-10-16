import atexit
from struct import pack
from typing import Union
from uuid import uuid4, UUID

from emessgee.memory_block import MemoryBlock
from emessgee.exceptions import (
    DataNotBytesOrStringError, DataTooLargeError, ErrorMessages
)
from emessgee.constants import (
    DEFAULT_BUFFER_SIZE, WRITING_FLAG_INDEX, HEADER_START, 
    RESERVED_BYTES,STRUCT_FORMAT, INVALID_ID, INVALID_INDEX, INVALID_SIZE
)

class Publisher:
    def __init__(self, topic:str, buffer_size:int = DEFAULT_BUFFER_SIZE):
        self._topic = topic
        self._memory_block = MemoryBlock(topic, buffer_size+RESERVED_BYTES, create=True)
        self._write_index = RESERVED_BYTES
        self._buffer_size = self._memory_block.get_buffer_size() - RESERVED_BYTES

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
        start_index = self._write_data(data_bytes)
        self._write_header(start_index, message_size, uuid4())
        self._end_write()

    def close(self):
        self._memory_block.close()

    def _write_header(self, index:int, message_size:int, message_id:UUID):
        header = pack(STRUCT_FORMAT, index, message_size, message_id.bytes)
        self._memory_block.write(HEADER_START, header)
    
    def _write_data(self, data:bytes):
        if(self._write_index + len(data) >= self._buffer_size):
            self._write_index = RESERVED_BYTES

        start_index = self._write_index
        self._memory_block.write(start_index, data)
        self._write_index += len(data)
        return start_index
    
    def _begin_write(self):
        self._memory_block.write_flag(WRITING_FLAG_INDEX, True)

    def _end_write(self):
        self._memory_block.write_flag(WRITING_FLAG_INDEX, False)