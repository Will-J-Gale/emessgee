# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from libcpp.string cimport string
from .params cimport Params as cppParams
from .constants cimport PARAMS_BUFFER_SIZE as cpp_PARAMS_BUFFER_SIZE
from .buffer_write_code cimport BufferWriteCode

class _Param:
    def __init__(self, valid, code, value):
        self.valid = valid
        self.code = code
        self.value = value

cdef class Params:
    cdef cppParams* cpp_params 

    def __cinit__(self):
        if(self.cpp_params == NULL):
            self.cpp_params = new cppParams();

    def __dealloc__(self):
        if(self.cpp_params != NULL):
            self.cpp_params.close()
            del self.cpp_params
        
    def write_int(self, str key, int data):
        return self.cpp_params.write_int(key.encode(), data)
    
    def write_float(self, str key, float data):
        return self.cpp_params.write_float(key.encode(), data)
    
    def write_double(self, str key, double data):
        return self.cpp_params.write_double(key.encode(), data)
    
    def write_bool(self, str key, bint data):
        return self.cpp_params.write_bool(key.encode(), data)
    
    def write_string(self, str key, str data):
        return self.cpp_params.write_string(key.encode(), data.encode())
    
    def read_int(self, str key):
        return self.cpp_params.read_int(key.encode())
    
    def read_float(self, str key):
        return self.cpp_params.read_float(key.encode())
    
    def read_double(self, str key):
        return self.cpp_params.read_double(key.encode())
    
    def read_bool(self, str key):
        return self.cpp_params.read_bool(key.encode())
    
    def read_string(self, str key):
        return self.cpp_params.read_string(key.encode()).decode("utf8")
    
    def close(self):
        self.cpp_params.close()