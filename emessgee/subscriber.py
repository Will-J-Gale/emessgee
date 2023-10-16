import os
import atexit
from collections import deque

from emessgee.memory_block import MemoryBlock
from emessgee.header import header_from_bytes
from emessgee.exceptions import MMapFileExistsButNotYetTruncatedError
from emessgee.constants import (
    TMP_FOLDER, HEADER_START, HEADER_LENGTH, INVALID_ID,
    MAX_SANITY_LOOPS, ReservedIndexes
)

class Subscriber:
    def __init__(self, topic:str):
        self._topic = topic
        self._topic_file = os.path.join(TMP_FOLDER, topic)
        self._memory_block = None
        self._last_received_id = None
        self._queue_size = None
        self._read_message_ids = None
        self._queue_index = 0
        self._create_memory_block()
         
        atexit.register(self.close)
    
    def _create_memory_block(self):
        if(os.path.exists(self._topic_file)):
            for _ in range(MAX_SANITY_LOOPS):
                try:
                    memory_block = MemoryBlock(self._topic)
                    block_ready = ord(memory_block.read(ReservedIndexes.BLOCK_READY.value))

                    if(not block_ready):
                        continue

                    self._queue_size = ord(memory_block.read(ReservedIndexes.QUEUE_SIZE.value))
                    self._read_message_ids = deque(maxlen=self._queue_size)
                    self._memory_block = memory_block
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
            if(self._memory_block.read(ReservedIndexes.WRITING.value) == False):
                break

    def close(self):
        if(self._memory_block is not None):
            self._memory_block.close()