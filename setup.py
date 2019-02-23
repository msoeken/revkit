from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test as TestCommand
import glob
import os
import sys

base_path = os.path.dirname(__file__)

class get_pybind_include(object):
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)

ext_modules = [
  Extension(
    '_revkit',
    glob.glob(os.path.join(base_path, 'ext', '*.cpp')) +
    glob.glob(os.path.join(base_path, 'lib/abcsat', '*.cpp')),
    include_dirs=[
      get_pybind_include(),
      get_pybind_include(user=True),
      os.path.join(base_path, 'lib', 'abcsat'),
      os.path.join(base_path, 'lib', 'caterpillar'),
      os.path.join(base_path, 'lib', 'easy'),
      os.path.join(base_path, 'lib', 'ez'),
      os.path.join(base_path, 'lib', 'fmt'),
      os.path.join(base_path, 'lib', 'glucose'),
      os.path.join(base_path, 'lib', 'kitty'),
      os.path.join(base_path, 'lib', 'lorina'),
      os.path.join(base_path, 'lib', 'mockturtle'),
      os.path.join(base_path, 'lib', 'percy'),
      os.path.join(base_path, 'lib', 'rang'),
      os.path.join(base_path, 'lib', 'sparsepp'),
      os.path.join(base_path, 'lib', 'tweedledum')
    ],
    define_macros=[
      ('FMT_HEADER_ONLY', '1'),
      ('DISABLE_NAUTY', '1'),
      ('LIN64', '1'),
      ('ABC_NAMESPACE', 'pabc'),
      ('ABC_NO_USE_READLINE', '1')
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
