from libcpp.string cimport string

from .buffer_write_code cimport BufferWriteCode

cdef extern from "params.cpp":
    pass

cdef extern from "params.h" namespace "emessgee":
    cdef cppclass Params:
        Params() except +
        BufferWriteCode write_int(char*, int) except +
        BufferWriteCode write_float(char*, float) except +
        BufferWriteCode write_double(char*, double) except +
        BufferWriteCode write_bool(char*, bool) except +
        BufferWriteCode write_string(char*, char*) except +
        void close() except +
        int read_int(char*) except +
        float read_float(char*) except +
        double read_double(char*) except +
        bint read_bool(char*) except +
        string read_string(char*) except +