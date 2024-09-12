# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .write_memory_block cimport WriteMemoryBlock as cppWriteMemoryBlock

cdef class WriteMemoryBlock:
    cdef cppWriteMemoryBlock* cpp_write_memory_block

    def __cinit__(self, name, size_t buffer_size):
        self.cpp_write_memory_block = new cppWriteMemoryBlock(name.encode(), buffer_size)

    def __dealloc__(self):
        if(self.cpp_write_memory_block != NULL):
            del self.cpp_write_memory_block

    def destroy(self):
        self.cpp_write_memory_block.destroy()
    
    def write(self, int index, bytes data):
        size = len(data)
        return self.cpp_write_memory_block.write(index, <unsigned char*>data, size)

    def read(self, index):
        return self.cpp_write_memory_block.read(index)