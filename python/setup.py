from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
from distutils import ccompiler
from distutils import cmd
from distutils import unixccompiler
from Cython.Build import cythonize
import string
import platform
import sys
import os

#holder for ext_modules variable
ext_modules = None
#execute following only if building extensions and started from makefile
if(sys.argv[1] == "build_ext" and os.environ.has_key("CFLAGS")):
  #determine arch of current os
  if(platform.system()=='Linux'):
      arch = 'linux'
  elif(platform.system()=='Darwin'):
      arch = 'macos'
      os.environ["CC"] = "g++"  
  else:
    arch = platform.system()

  #variable to hold all the libraries used in code
  libraries = ["prim"]#,"boost_thread","boost_system"]

  #searches for boost_serialization or boost_serialization-mt libraries
  #if neither is found terminates compilation
  compiler = ccompiler.new_compiler()    
  list_of_cflags = os.environ["CFLAGS"].strip(string.whitespace).split(" ")
  for flag in list_of_cflags:
    if(flag.startswith("-L")):
      libdir = flag.lstrip("-L")
      compiler.set_library_dirs([libdir])
      if(compiler.find_library_file(compiler.library_dirs, "boost_serialization")):
        libraries.append("boost_serialization")
        break
      elif(compiler.find_library_file(compiler.library_dirs, "boost_serialization-mt")):
        libraries.append("boost_serialization-mt")
        break
  else:
    print("BOOST serialization library not found")
    print("Terminating...")
    sys.exit(0)

  ext_modules=[
      Extension("interface",
                ["interface.pyx","wrapper.cpp"],
                # include_dirs=["kodiak/src/", "/"],
                # extra_objects=["kodiak/lib/"+arch+"/libKodiak.a",],
                include_dirs=["../src", "/"],
                extra_objects=["../lib/"+arch+"/libKodiak.a",],
                libraries=libraries,
                extra_link_args = [],
                extra_compile_args = [os.environ["CFLAGS"]],
                language='c++',) # Unix-like specific
  ]


setup(
  name = "python-interface-to-kodiak",
  description = "Userfriendly interface to Kodiak library",
  cmdclass = {"build_ext": build_ext},
  ext_modules = ext_modules
)
