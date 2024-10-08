from .rng cimport RNG
from .buffer_write_code cimport BufferWriteCode
from .write_memory_block cimport WriteMemoryBlock

cdef extern from "write_memory_queue.cpp":
    pass

cdef extern from "write_memory_queue.h" namespace "emessgee":
    cdef cppclass WriteMemoryQueue:
        WriteMemoryQueue(char*, size_t, size_t) except +
        BufferWriteCode write(unsigned char*, size_t) except +
        void close() except +
        # WriteMemoryBlock* get_write_block() #@TODO