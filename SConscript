Import('env', "envCython")

#CPP
emessgee_src = [f for f in Glob("emessgee/*.cpp") if not str(f).endswith("_pyx.cpp")]
emessgee = env.StaticLibrary("emessgee", emessgee_src)

#Python
inc = [Dir('.')]

cython_src = Glob("emessgee/*_pyx.pyx")
for pyx_path in cython_src:
  program_path = str(pyx_path.abspath).replace(".pyx", ".so")
  envCython.Program(program_path, pyx_path.abspath, LIBS=envCython["LIBS"], CPPPATH=envCython['CPPPATH'] + inc)

Export("emessgee")