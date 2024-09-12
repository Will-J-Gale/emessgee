from .read_result cimport ReadResult
from .read_memory_block cimport ReadMemoryBlock

cdef extern from "read_memory_queue.cpp":
    pass

cdef extern from "read_memory_queue.h" namespace "emessgee":
    cdef cppclass ReadMemoryQueue:
        ReadMemoryQueue(char*) except +
        ReadResult read() except +
        void close() except +
        bint is_initialized() except +