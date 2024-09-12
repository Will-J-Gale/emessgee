cdef extern from "constants.h" namespace "emessgee":
    cdef const char[] TMP_FOLDER
    cdef unsigned int DEFAULT_BUFFER_SIZE 
    cdef unsigned int DEFAULT_QUEUE_SIZE
    cdef unsigned int ID_LEN
    cdef unsigned int INVALID_ID
    cdef unsigned int INVALID_INDEX
    cdef unsigned int MAX_SANITY_LOOPS