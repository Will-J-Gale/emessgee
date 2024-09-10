# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .read_memory_block cimport ReadMemoryBlock as cppReadMemoryBlock

cdef class ReadMemoryBlock:
    cdef cppReadMemoryBlock* c_read_memory_block

    def __cinit__(self, name):
        self.c_read_memory_block = new cppReadMemoryBlock(name.encode())
    
    def __dealloc__(self):
        del self.c_read_memory_block
    
    def destroy(self):
        self.c_read_memory_block.destroy()
    
    def is_initialized(self):
        return self.c_read_memory_block.is_initialized()

    def initialize(self):
        return self.c_read_memory_block.initialize()
    
    def read(self, unsigned int index):
        return self.c_read_memory_block.read(index)


