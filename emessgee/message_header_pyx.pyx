# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .message_header cimport MessageHeader as cppMessageHeader, MESSAGE_HEADER_SIZE as cpp_MESSAGE_HEADER_SIZE

def MESSAGE_HEADER_SIZE():
    return cpp_MESSAGE_HEADER_SIZE

cdef class MessageHeader:
    cdef cppMessageHeader* c_message_header
    
    def __cinit__(self):
        self.c_message_header = new cppMessageHeader()
    
    def __dealloc__(self):
        del self.c_message_header
     
    @property
    def message_index(self):
        return self.c_message_header.message_index

    @message_index.setter
    def message_index(self, unsigned int index):
        self.c_message_header.message_index = index 

    @property
    def message_size(self):
        return self.c_message_header.message_size
    @message_size.setter
    def message_size(self, size_t size):
        self.c_message_header.message_size = size 

    @property
    def message_id(self):
        return self.c_message_header.message_id
    @message_id.setter
    def message_id(self, size_t id):
        self.c_message_header.message_size = id 

    @staticmethod
    def from_bytes(bytes data):
        cdef cppMessageHeader* instance = cppMessageHeader.from_bytes(<unsigned char*> data)
        raise NotImplementedError()
        return None
    
    def to_bytes(self):
        return self.c_message_header.to_bytes()