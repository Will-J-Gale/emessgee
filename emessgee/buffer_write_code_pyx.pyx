# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .buffer_write_code cimport BufferWriteCode as cppBufferWriteCode

cdef class BufferWriteCode:
    SUCCESS = cppBufferWriteCode.SUCCESS
    BUFFER_NULLPTR = cppBufferWriteCode.BUFFER_NULLPTR
    INDEX_TOO_LARGE = cppBufferWriteCode.INDEX_TOO_LARGE
    BUFFER_CLOSED = cppBufferWriteCode.BUFFER_CLOSED
    DATA_TOO_LARGE = cppBufferWriteCode.DATA_TOO_LARGE




