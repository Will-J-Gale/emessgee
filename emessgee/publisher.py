import atexit
from struct import pack
from typing import Union
from uuid import uuid4, UUID

from emessgee.memory_block import MemoryBlock
from emessgee.header import header_to_bytes
from emessgee.exceptions import (
    DataNotBytesOrStringError, DataTooLargeError, ErrorMessages
)
from emessgee.constants import (
    DEFAULT_BUFFER_SIZE, HEADER_START, HEADER_LENGTH,
    INVALID_ID, INVALID_INDEX, INVALID_SIZE, ReservedIndexes
)

class Publisher:
    def __init__(
            self, 
            topic:str, 
            buffer_size:int = DEFAULT_BUFFER_SIZE,
            queue_size:int = 1):
        self._topic = topic
        self._queue_size = max(1, queue_size)
        self._queue_index = 0
        self._header_length = HEADER_LENGTH * queue_size
        extened_buffer_size = len(ReservedIndexes) + self._header_length + buffer_size
        self._memory_block = MemoryBlock(topic, extened_buffer_size, create=True)
        self._data_start = len(ReservedIndexes) + self._header_length
        self._write_index = self._data_start
        self._buffer_size = buffer_size

        for i in range(self._queue_size):
            self._write_header(i, INVALID_INDEX, INVALID_SIZE, INVALID_ID)

        self._memory_block.write_byte(ReservedIndexes.QUEUE_SIZE.value, self._queue_size)
        self._memory_block.write_byte(ReservedIndexes.BLOCK_READY.value, 1)
        atexit.register(self.close)
    
    def send(self, data:Union[bytes, str]):
        if(type(data) not in [bytes, str]):
            raise DataNotBytesOrStringError(
                ErrorMessages.DATA_NOT_BYTES.format(type=type(data))
            )

        data_bytes = data if isinstance(data, bytes) else data.encode()
        message_size = len(data_bytes)
        
        if(message_size > self._buffer_size):
            raise DataTooLargeError(ErrorMessages.DATA_TOO_LARGE.format(
                    data_size=message_size, 
                    buffer_size=self._buffer_size
                )
            )

        self._begin_write()
        start_index = self._write_data(data_bytes)
        self._write_header(self._queue_index, start_index, message_size, uuid4())
        self._queue_index = (self._queue_index + 1) % self._queue_size
        self._end_write()

    def close(self):
        self._memory_block.close()

    def _write_header(self, queue_index:int, message_index:int, message_size:int, message_id:UUID):
        header = header_to_bytes(message_index, message_size, message_id)
        header_index = HEADER_START + (HEADER_LENGTH * queue_index)
        self._memory_block.write(header_index, header)
    
    def _write_data(self, data:bytes):
        if(self._write_index + len(data) >= self._buffer_size):
            self._write_index = self._data_start

        start_index = self._write_index
        self._memory_block.write(start_index, data)
        self._write_index += len(data)
        return start_index
    
    def _begin_write(self):
        self._memory_block.write_byte(ReservedIndexes.WRITING.value, 1)

    def _end_write(self):
        self._memory_block.write_byte(ReservedIndexes.WRITING.value, 0)