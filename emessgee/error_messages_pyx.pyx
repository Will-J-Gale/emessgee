# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .error_messages cimport FILE_ALREADY_EXISTS as cpp_FILE_ALREADY_EXISTS
from .error_messages cimport FAILED_TO_CREATE_MMAP as cpp_FAILED_TO_CREATE_MMAP
from .error_messages cimport FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK as cpp_FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK
from .error_messages cimport FAILED_TO_DESTROY_READ_MEMORY_BLOCK as cpp_FAILED_TO_DESTROY_READ_MEMORY_BLOCK
from .error_messages cimport FAILED_TO_GENERATE_UNIQUE_ID as cpp_FAILED_TO_GENERATE_UNIQUE_ID

def FILE_ALREADY_EXISTS():
    return cpp_FILE_ALREADY_EXISTS.decode("utf8")

def FAILED_TO_CREATE_MMAP():
    return cpp_FAILED_TO_CREATE_MMAP.decode("utf8")

def FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK():
    return cpp_FAILED_TO_DESTROY_WRITE_MEMORY_BLOCK.decode("utf8")

def FAILED_TO_DESTROY_READ_MEMORY_BLOCK():
    return cpp_FAILED_TO_DESTROY_READ_MEMORY_BLOCK.decode("utf8")

def FAILED_TO_GENERATE_UNIQUE_ID():
    return cpp_FAILED_TO_GENERATE_UNIQUE_ID.decode("utf8")