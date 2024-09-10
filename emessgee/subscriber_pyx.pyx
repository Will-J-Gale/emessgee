# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from libcpp.vector cimport vector
from libcpp.string cimport string

import numpy as np

from .subscriber cimport Subscriber as cppSubscriber
from .read_result cimport ReadResult as cppReadResult
from .read_result_pyx import _ReadResult

cdef class Subscriber:
    cdef cppSubscriber* cpp_subscriber 

    def __cinit__(self, list[str] topics):
        cdef vector[string] cpp_topics = [topic.encode() for topic in topics]
        self.cpp_subscriber = new cppSubscriber(cpp_topics)
    
    def __dealloc__(self):
        del self.cpp_subscriber
    
    def recv(self, str topic):
        cdef cppReadResult result = self.cpp_subscriber.recv(topic.encode())
        cdef char[:] data_view
        if(result.valid):
            data_view = <char[:result.size]>(result.data)
            data = np.asarray(data_view)
        else:
            data = None

        return _ReadResult(
            data,
            result.size,
            result.valid
        )
    
    def close(self):
        self.cpp_subscriber.close()