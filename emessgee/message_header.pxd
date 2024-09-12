cdef extern from "message_header.h" namespace "emessgee":
    cdef int MESSAGE_HEADER_SIZE

    cdef cppclass MessageHeader:
        @staticmethod
        MessageHeader* from_bytes(unsigned char* data) except +
        unsigned char* to_bytes() except +
        unsigned int message_index
        unsigned int message_size
        unsigned int message_id
        