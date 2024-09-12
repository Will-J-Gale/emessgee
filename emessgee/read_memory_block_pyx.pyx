# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .read_memory_block cimport ReadMemoryBlock as cppReadMemoryBlock

cdef class ReadMemoryBlock:
    cdef cppReadMemoryBlock* cpp_read_memory_block

    def __cinit__(self, name):
        self.cpp_read_memory_block = new cppReadMemoryBlock(name.encode())
    
    def __dealloc__(self):
        if(self.cpp_read_memory_block != NULL):
            self.destroy()
            del self.cpp_read_memory_block
    
    def destroy(self):
        self.cpp_read_memory_block.destroy()
    
    def is_initialized(self):
        return self.cpp_read_memory_block.is_initialized()

    def initialize(self):
        return self.cpp_read_memory_block.initialize()
    
    def read(self, unsigned int index):
        return self.cpp_read_memory_block.read(index)


