from .buffer_write_code cimport BufferWriteCode

cdef extern from "write_memory_block.cpp":
    pass

cdef extern from "write_memory_block.h" namespace "emessgee":
    cdef cppclass WriteMemoryBlock:
        WriteMemoryBlock(char*, size_t) except +
        void destroy() except +
        BufferWriteCode write(unsigned int, unsigned char*, size_t) except +
        unsigned char* read(size_t) except +