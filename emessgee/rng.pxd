cdef extern from "rng.cpp":
    pass

cdef extern from "rng.h" namespace "emessgee":
    cdef cppclass RNG:
        @staticmethod
        int generate() except +