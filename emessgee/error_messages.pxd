cdef extern from "error_messages.h" namespace "emessgee":
    cdef const char[] FILE_ALREADY_EXISTS
    cdef const char[] FAILED_TO_CREATE_MMAP
    cdef const char[] FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK
    cdef const char[] FAILED_TO_DESTROY_READ_MEMORY_BLOCK
    cdef const char[] FAILED_TO_GENERATE_UNIQUE_ID