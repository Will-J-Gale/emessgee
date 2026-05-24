Import('env')

base_libs = []

# build ion_test
env["CPPPATH"] += ["#emessgee"]
emessgee_src = Glob("#emessgee/emessgee/*.cpp")
emessgee = env.StaticLibrary("#emessgee/emessgee", emessgee_src, LIBS=base_libs)
Export("emessgee")