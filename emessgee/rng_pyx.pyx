# distutils: language = c++

from .rng cimport RNG as cppRNG

cdef class RNG:
    cdef cppRNG c_rng

    def genreate(self):
        return self.c_rng.generate()