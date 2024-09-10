# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .write_memory_block cimport WriteMemoryBlock as cppWriteMemoryBlock

cdef class WriteMemoryBlock:
    cdef cppWriteMemoryBlock* c_write_memory_block

    def __cinit__(self, name, size_t buffer_size):
        self.c_write_memory_block = new cppWriteMemoryBlock(name.encode(), buffer_size)

    def __dealloc__(self):
        del self.c_write_memory_block

    def destroy(self):
        self.c_write_memory_block.destroy()
    
    def write(self, int index, bytes data):
        size = len(data)
        return self.c_write_memory_block.write(index, <char*>data, size)

    def read(self, index):
        return self.c_write_memory_block.read(index)

cdef class _WriteMemoryBlockWrapper:
    cdef cppWriteMemoryBlock* cpp_obj

    @staticmethod
    cdef from_ptr(cppWriteMemoryBlock* ptr):
        cdef _WriteMemoryBlockWrapper wrapper = _WriteMemoryBlockWrapper.__new__(_WriteMemoryBlockWrapper)
        wrapper.cpp_obj = ptr
        return wrapper
    

