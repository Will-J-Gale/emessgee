# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from libcpp.vector cimport vector
from libcpp.string cimport string

from .publisher cimport Publisher as cppPublisher

cdef class Publisher:
    cdef cppPublisher* cpp_publisher 

    def __cinit__(self, list[str] topics, size_t buffer_size, size_t queue_size):
        cdef vector[string] cpp_topics = [topic.encode() for topic in topics]
        self.cpp_publisher = new cppPublisher(cpp_topics, buffer_size, queue_size)
    
    def __dealloc__(self):
        del self.cpp_publisher
    
    def send(self, str topic, bytes data):
        size = len(data)
        self.cpp_publisher.send(topic.encode(), <char*> data, size)
    
    def close(self):
        self.cpp_publisher.close()