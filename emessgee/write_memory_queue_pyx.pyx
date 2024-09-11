# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .write_memory_queue cimport WriteMemoryQueue as cppWriteMemoryQueue
from .write_memory_block cimport WriteMemoryBlock as cppWriteMemoryBlock

cdef class WriteMemoryQueue:
    cdef cppWriteMemoryQueue* cpp_write_memory_queue

    def __cinit__(self, str name, size_t buffer_size, size_t queue_size):
        self.cpp_write_memory_queue = new cppWriteMemoryQueue(name.encode(), buffer_size, queue_size)
    
    def __dealloc__(self):
        del self.cpp_write_memory_queue
    
    def write(self, bytes data):
        size = len(data)
        return self.cpp_write_memory_queue.write(<unsigned char*> data, size)
    
    def close(self):
        self.cpp_write_memory_queue.close()