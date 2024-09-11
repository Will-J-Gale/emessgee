from libcpp.vector cimport vector
from libcpp.string cimport string

from .read_memory_queue cimport ReadMemoryQueue
from .read_result cimport ReadResult

cdef extern from "subscriber.cpp":
    pass

cdef extern from "subscriber.h" namespace "emessgee":
    cdef cppclass Subscriber:
        Subscriber(vector[string]) except +
        ReadResult recv(string topic) except +
        void close() except +