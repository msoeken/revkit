from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test as TestCommand
import glob
import os
import sys

base_path = os.path.dirname(__file__)

def pybind_include():
  import pybind11
  return pybind11.get_include()

ext_modules = [
  Extension(
    '_revkit',
    glob.glob(os.path.join(base_path, 'ext', '*.cpp')),
    include_dirs=[
      pybind_include(),
      os.path.join(base_path, 'lib', 'easy'),
      os.path.join(base_path, 'lib', 'fmt'),
      os.path.join(base_path, 'lib', 'glucose'),
      os.path.join(base_path, 'lib', 'kitty'),
      os.path.join(base_path, 'lib', 'tweedledum')
    ],
    define_macros=[
      ('FMT_HEADER_ONLY', '1')
    ],
    language='c++'
  )
]

class BuildExt(build_ext):
  def build_extensions(self):
    ct = self.compiler.compiler_type
    opts = []
    if ct == 'unix':
      opts.append('-std=c++17')
      opts.append('-Wno-unknown-pragmas')
    else:
      opts.append('/std:c++17')
    for ext in self.extensions:
      ext.extra_compile_args = opts
    build_ext.build_extensions(self)

class PyTest(TestCommand):
  def run_tests(self):
    import pytest
    errno = pytest.main()
    sys.exit(errno)

setup(
  name='revkit',
  version='3.1.0',
  author='Mathias Soeken',
  author_email='mathias.soeken@epfl.ch',
  ext_modules=ext_modules,
  cmdclass={
    'build_ext': BuildExt,
    'test': PyTest
  },
  packages=['revkit', 'revkit.export'],
  zip_safe=False,
  install_requires=['pybind11>=2.2'],
  tests_require=['pytest']
)
