cdef extern from "metadata.h" namespace "emessgee":
    cdef int METADATA_SIZE

    cdef cppclass Metadata:
        @staticmethod
        Metadata* from_bytes(unsigned char* data) except +
        unsigned char* to_bytes() except +
        unsigned int queue_size 
        bint block_ready
        bint writing