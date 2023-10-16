import os
import atexit
from uuid import uuid4, UUID
from typing import Union
from collections import deque

from emessgee.memory_block import WriteMemoryBlock, ReadMemoryBlock
from emessgee.header import header_to_bytes, header_from_bytes
from emessgee.constants import (
    ReservedIndexes, HEADER_LENGTH, HEADER_START, INVALID_ID, INVALID_INDEX,
    INVALID_SIZE, DEFAULT_BUFFER_SIZE, MAX_SANITY_LOOPS, TMP_FOLDER
)
from emessgee.exceptions import (
    DataNotBytesOrStringError, DataTooLargeError, ErrorMessages,
    FailedCreatingMmapBufferError
)

class WriteMemoryQueue:
    def __init__(self, name, buffer_size:int = DEFAULT_BUFFER_SIZE, queue_size:int = 1):
        self._name = name
        self._buffer_size = buffer_size
        self._queue_size = max(1, queue_size)
        self._header_length = HEADER_LENGTH * queue_size
        extened_buffer_size = len(ReservedIndexes) + self._header_length + buffer_size
        self._memory_block = WriteMemoryBlock(name, extened_buffer_size)
        self._data_start = len(ReservedIndexes) + self._header_length
        self._write_index = self._data_start
        self._queue_index = 0
        
        for i in range(self._queue_size):
            self._write_header(i, INVALID_INDEX, INVALID_SIZE, INVALID_ID)

        self._memory_block.write_byte(ReservedIndexes.QUEUE_SIZE.value, self._queue_size)
        self._memory_block.write_byte(ReservedIndexes.BLOCK_READY.value, 1)

        atexit.register(self.close)
    
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
    
    def close(self):
        self._memory_block.close()

    def write(self, data:Union[bytes, str]):
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
        message_index = self._write_data(data_bytes)
        self._write_header(self._queue_index, message_index, message_size, uuid4())
        self._queue_index = (self._queue_index + 1) % self._queue_size
        self._end_write()

class ReadMemoryQueue:
    def __init__(self, name):
        self._name = name
        self._memory_filepath = os.path.join(TMP_FOLDER, name)
        self._memory_block = None
        self._queue_size = None
        self._queue_index = 0
        self._read_message_ids = None
        self._create_memory_block()
    
    def _create_memory_block(self):
        if(os.path.exists(self._memory_filepath)):
            for _ in range(MAX_SANITY_LOOPS):
                try:
                    memory_block = ReadMemoryBlock(self._name)
                    block_ready = ord(memory_block.read(ReservedIndexes.BLOCK_READY.value))

                    if(not block_ready):
                        memory_block.close()
                        continue

                    self._queue_size = ord(memory_block.read(ReservedIndexes.QUEUE_SIZE.value))
                    self._read_message_ids = deque(maxlen=self._queue_size)
                    self._memory_block = memory_block
                    break

                except FailedCreatingMmapBufferError:
                    continue

            if(self._memory_block is None):
                return False

            return True

        return False
    
    def read(self):
        if(self._memory_block is None and not self._create_memory_block()):
            return None

        self._wait_for_writing()
        index, data_size, message_id = self._read_header(self._queue_index) 

        if(message_id == INVALID_ID or message_id in self._read_message_ids):
            return None
        
        self._queue_index = (self._queue_index + 1) % self._queue_size
        data = self._memory_block.read(index, data_size)
        self._read_message_ids.append(message_id)
        return data

    def _read_header(self, queue_index):
        if(self._memory_block is None):
            return None

        header_index = HEADER_START + (queue_index * HEADER_LENGTH)
        header_bytes = self._memory_block.read(header_index, HEADER_LENGTH)
        return header_from_bytes(header_bytes)
    

    def _wait_for_writing(self):
        if(self._memory_block is None):
            return

        for _ in range(MAX_SANITY_LOOPS):
            if(ord(self._memory_block.read(ReservedIndexes.WRITING.value)) == False):
                break

    def close(self):
        if(self._memory_block is not None):
            self._memory_block.close()
