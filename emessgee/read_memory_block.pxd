from .buffer_write_code cimport BufferWriteCode
from libcpp cimport bool

cdef extern from "read_memory_block.cpp":
    pass

cdef extern from "read_memory_block.h" namespace "emessgee":
    cdef cppclass ReadMemoryBlock:
        ReadMemoryBlock(char*) except +
        void destroy()
        char* read(unsigned int)
        bool is_initialized()
        bool initialize()