cdef extern from "metadata.h" namespace "emessgee":
    cdef int METADATA_SIZE

    cdef cppclass Metadata:
        @staticmethod
        Metadata* from_bytes(char* data) except +
        char* to_bytes()
        unsigned int queue_size
        bint block_ready
        bint writing