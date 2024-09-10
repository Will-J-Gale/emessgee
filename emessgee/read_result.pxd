cdef extern from "read_result.h" namespace "emessgee":
    cdef struct ReadResult:
        char* data
        int size
        bint valid