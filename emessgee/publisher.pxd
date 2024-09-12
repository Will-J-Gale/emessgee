from libcpp.vector cimport vector
from libcpp.string cimport string

from .write_memory_queue cimport WriteMemoryQueue
from .buffer_write_code cimport BufferWriteCode

cdef extern from "publisher.cpp":
    pass

cdef extern from "publisher.h" namespace "emessgee":
    cdef cppclass Publisher:
        Publisher(vector[string], size_t, size_t) except +
        BufferWriteCode send(char*, unsigned char*, size_t) except +
        void close() except +