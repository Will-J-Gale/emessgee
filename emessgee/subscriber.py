import os
import mmap
import atexit
from uuid import UUID
from struct import unpack

from emessgee.exceptions import InfiniteLoopDetectedError, ErrorMessages
from emessgee.constants import (
    TMP_FOLDER, WRITING_FLAG_INDEX, HEADER_START, HEADER_END, STRUCT_FORMAT,
    ID_BYTES_ENDIEN, INVALID_ID, MAX_SANITY_LOOPS
)

class Subscriber:
    def __init__(self, topic:str):
        self._topic = topic
        self._topic_file = os.path.join(TMP_FOLDER, topic)
        self._file_descriptor = None
        self._buffer = None
        self._last_received_id = None
        self._create_buffer()
         
        atexit.register(self.close)
    
    def _create_buffer(self):
        if(os.path.exists(self._topic_file)):
            self._file_descriptor = os.open(self._topic_file, os.O_CREAT | os.O_RDWR)
            for _ in range(MAX_SANITY_LOOPS):
                try:
                    self._buffer = mmap.mmap(self._file_descriptor, 0, mmap.MAP_SHARED)
                    break
                except ValueError: 
                    #File exists but file not yet truncated
                    continue
                    
            if(self._buffer is None):
                raise InfiniteLoopDetectedError(ErrorMessages.INFINITE_LOOP)

            return True
        
        return False

    def recv(self):
        if(not self._buffer and not self._create_buffer()):
            return None

        self._wait_for_writing()
        index, data_size, message_id = self._read_header() 
        data = self._read(index, data_size)

        if(message_id == INVALID_ID or message_id == self._last_received_id):
            return None
        
        self._last_received_id = message_id
        return data

    def _read(self, index, size):
        if(self._buffer is None):
            return None

        end = index + size
        data = self._buffer[index:end]
        return data
    
    def _read_header(self):
        if(self._buffer is None):
            return None

        header_bytes = self._buffer[HEADER_START:HEADER_END]
        index, data_size, id_bytes = unpack(STRUCT_FORMAT, header_bytes)
        message_id = UUID(int=int.from_bytes(id_bytes, ID_BYTES_ENDIEN))

        return index, data_size, message_id

    def _wait_for_writing(self):
        if(self._buffer is None):
            return

        for _ in range(MAX_SANITY_LOOPS):
            if(self._buffer[WRITING_FLAG_INDEX] == False):
                break

    def close(self):
        if(self._buffer is not None):
            self._buffer.close()