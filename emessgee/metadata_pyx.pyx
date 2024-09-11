# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .metadata cimport Metadata as cppMetadata, METADATA_SIZE as cpp_METADATA_SIZE

def METADATA_SIZE():
    return cpp_METADATA_SIZE

cdef class Metadata:
    cdef cppMetadata* cpp_metadata

    def __cinit__(self):
        self.cpp_metadata = new cppMetadata()
    
    @staticmethod
    def from_bytes(bytes data):
        cdef cppMetadata* instance = cppMetadata.from_bytes(<unsigned char*> data)
        raise NotImplementedError()
        return None
    
    def to_bytes(self):
        return self.cpp_metadata.to_bytes()

    @property
    def queue_size(self):
        return self.cpp_metadata.queue_size
    
    @property.setter
    def queue_size(self, size_t size):
        self.cpp_metadata.queue_size = size

    @property
    def block_ready(self):
        return self.cpp_metadata.block_ready
    
    @property.setter
    def queue_size(self, bint ready):
        self.cpp_metadata.block_ready = ready

    @property
    def writing(self):
        return self.cpp_metadata.writing
    
    @property.setter
    def queue_size(self, bint writing):
        self.cpp_metadata.writing = writing