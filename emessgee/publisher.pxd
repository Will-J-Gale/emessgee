from libcpp.vector cimport vector
from libcpp.string cimport string

from .write_memory_queue cimport WriteMemoryQueue

cdef extern from "publisher.cpp":
    pass

cdef extern from "publisher.h" namespace "emessgee":
    cdef cppclass Publisher:
        Publisher(vector[string], size_t, size_t) except +
        void send(char*, char*, size_t)
        void close()