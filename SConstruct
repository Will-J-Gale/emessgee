import os
import subprocess
import sys
import sysconfig
import platform
import numpy as np

import SCons.Errors

SCons.Warnings.warningAsException(True)

AddOption('--build_tests',
          action='store_true',
          help='Build test programs')

env = Environment(
  CCFLAGS=[
    "-g",
    "-fPIC",
    "-O2",
    "-Wunused",
    "-Werror",
    "-Wshadow",
    "-Wno-unknown-warning-option",
    "-Wno-inconsistent-missing-override",
    "-Wno-c99-designator",
    "-Wno-reorder-init-list",
    "-Wno-vla-cxx-extension",
  ],

  CPPPATH=["#emessgee"],
  CC='clang',
  CXX='clang++',
  CFLAGS=["-std=gnu11"],
  CXXFLAGS=["-std=c++1z"],
  COMPILATIONDB_USE_ABSPATH=True,
)

emessgee_src = Glob("#emessgee/emessgee/*.cpp")
emessgee = env.StaticLibrary("#emessgee/emessgee", emessgee_src)
Export("emessgee")

#@TODO
# if(GetOption("build_tests")):
#   test_env = env.Clone()
#   test_env["CPPPATH"] += ['#test/include']
#   test_src = Glob("#test/src/*.cpp")
#   test_env.Program("#test/test", test_src, LIBS=[emessgee])

