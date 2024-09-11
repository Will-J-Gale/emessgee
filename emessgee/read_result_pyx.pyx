# cython: language_level = 3
# distutils: language = c++

from .read_result cimport ReadResult as cppReadResult

cdef class ReadResult:
    cdef cppReadResult c_read_result

class _ReadResult:
    def __init__(self, data, size, valid):
        self.data = data
        self.size = size
        self.valid = valid