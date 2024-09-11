# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

import numpy as np

from .read_memory_queue cimport ReadMemoryQueue as cppReadMemoryQueue
from .read_result cimport ReadResult as cppReadResult

class _ReadResult:
    def __init__(self, data, size, valid):
        self.data = data
        self.size = size
        self.valid = valid

cdef class ReadMemoryQueue:
    cdef cppReadMemoryQueue* cpp_read_memory_queue

    def __cinit__(self, str name):
        self.cpp_read_memory_queue = new cppReadMemoryQueue(name.encode())
    
    def __dealloc__(self):
        del self.cpp_read_memory_queue
    
    def read(self):
        cdef cppReadResult result = self.cpp_read_memory_queue.read()
        cdef unsigned char[:] data_view = <unsigned char[:result.size]>(result.data)

        return _ReadResult(
            np.asarray(data_view),
            result.size,
            result.valid
        )

    def close(self):
        self.cpp_read_memory_queue.close()
    
    def is_initialized(self):
        return self.cpp_read_memory_queue.is_initialized()