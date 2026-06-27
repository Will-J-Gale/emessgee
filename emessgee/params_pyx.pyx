# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from libcpp.string cimport string
from libcpp.vector cimport vector
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
    
    def write_bool(self, str key, bool data):
        return self.cpp_params.write_bool(key.encode(), data)
    
    def write_string(self, str key, str data):
        return self.cpp_params.write_string(key.encode(), data.encode())
    
    def write_bytes(self, str key, bytes data):
        size = len(data)
        return self.cpp_params.write_bytes(key.encode(), data, size)
    
    def write_string_list(self, str key, list[string] data):
        cdef vector[string] cpp_data
        for d in data:
            if(isinstance(d, str)):
                cpp_data.push_back(d.encode())
            else:
                cpp_data.push_back(d)
                
        return self.cpp_params.write_string_list(key.encode(), cpp_data)
    
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
    
    def read_bytes(self, str key, int size):
        dst = bytes(size)
        self.cpp_params.read_bytes(key.encode(), dst, size)
        return dst
    
    def read_string_list(self, str key):
        return self.cpp_params.read_string_list(key.encode())
    
    def delete_key(self, str key):
        return self.cpp_params.delete_key(key.encode())
    
    def check_key(self, str key):
        return self.cpp_params.check_key(key.encode())
        
    def close(self):
        self.cpp_params.close()