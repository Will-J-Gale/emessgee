cdef extern from "buffer_write_code.h" namespace "emessgee":
    cdef enum BufferWriteCode:
        SUCCESS,
        BUFFER_NULLPTR,
        INDEX_TOO_LARGE,
        BUFFER_CLOSED,
        DATA_TOO_LARGE