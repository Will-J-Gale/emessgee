import os
import atexit
from uuid import UUID
from struct import unpack

from emessgee.memory_block import MemoryBlock
from emessgee.exceptions import MMapFileExistsButNotYetTruncatedError
from emessgee.constants import (
    TMP_FOLDER, WRITING_FLAG_INDEX, HEADER_START, HEADER_LENGTH, STRUCT_FORMAT,
    ID_BYTES_ENDIEN, INVALID_ID, MAX_SANITY_LOOPS
)

class Subscriber:
    def __init__(self, topic:str):
        self._topic = topic
        self._topic_file = os.path.join(TMP_FOLDER, topic)
        self._memory_block = None
        self._last_received_id = None
        self._create_memory_block()
         
        atexit.register(self.close)
    
    def _create_memory_block(self):
        if(os.path.exists(self._topic_file)):
            for _ in range(MAX_SANITY_LOOPS):
                try:
                    self._memory_block = MemoryBlock(self._topic)
                    break
                except MMapFileExistsButNotYetTruncatedError:
                    continue

            if(self._memory_block is None):
                return False

            return True

        return False

    def recv(self):
        if(not self._memory_block and not self._create_memory_block()):
            return None

        self._wait_for_writing()
        index, data_size, message_id = self._read_header() 
        data = self._memory_block.read(index, data_size)

        if(message_id == INVALID_ID or message_id == self._last_received_id):
            return None
        
        self._last_received_id = message_id
        return data

    def _read_header(self):
        if(self._memory_block is None):
            return None

        header_bytes = self._memory_block.read(HEADER_START, HEADER_LENGTH)
        index, data_size, id_bytes = unpack(STRUCT_FORMAT, header_bytes)
        message_id = UUID(int=int.from_bytes(id_bytes, ID_BYTES_ENDIEN))

        return index, data_size, message_id

    def _wait_for_writing(self):
        if(self._memory_block is None):
            return

        for _ in range(MAX_SANITY_LOOPS):
            if(self._memory_block.read(WRITING_FLAG_INDEX) == False):
                break

    def close(self):
        if(self._memory_block is not None):
            self._memory_block.close()