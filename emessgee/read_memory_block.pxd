from .buffer_write_code cimport BufferWriteCode
from libcpp cimport bool

cdef extern from "read_memory_block.cpp":
    pass

cdef extern from "read_memory_block.h" namespace "emessgee":
    cdef cppclass ReadMemoryBlock:
        ReadMemoryBlock(char*) except +
        void destroy()  except +
        unsigned char* read(unsigned int) except +
        bool is_initialized() except +
        bool initialize() except +