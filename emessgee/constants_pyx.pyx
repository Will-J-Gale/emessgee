# cython: language_level = 3
# distutils: language = c++
# distutils: extra_compile_args = -std=c++2a

from .constants cimport TMP_FOLDER as cpp_TMP_FOLDER
from .constants cimport DEFAULT_BUFFER_SIZE as cpp_DEFAULT_BUFFER_SIZE
from .constants cimport DEFAULT_QUEUE_SIZE as cpp_DEFAULT_QUEUE_SIZE
from .constants cimport ID_LEN as cpp_ID_LEN
from .constants cimport INVALID_ID as cpp_INVALID_ID
from .constants cimport INVALID_INDEX as cpp_INVALID_INDEX
from .constants cimport MAX_SANITY_LOOPS as cpp_MAX_SANITY_LOOPS

def TMP_FOLDER():
    return cpp_TMP_FOLDER.decode("utf8")

def DEFAULT_BUFFER_SIZE():
    return cpp_DEFAULT_BUFFER_SIZE.decode("utf8")

def DEFAULT_QUEUE_SIZE():
    return cpp_DEFAULT_QUEUE_SIZE.decode("utf8")

def ID_LEN():
    return cpp_ID_LEN.decode("utf8")

def INVALID_ID():
    return cpp_INVALID_ID.decode("utf8")

def INVALID_INDEX():
    return cpp_INVALID_INDEX.decode("utf8")

def MAX_SANITY_LOOPS():
    return cpp_MAX_SANITY_LOOPS.decode("utf8")

