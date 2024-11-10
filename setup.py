from setuptools import setup, Extension, find_packages
import pybind11
import sys

extra_compile_args = [
    '-O3',
    '-march=native',
    '-ffast-math',
]

if sys.platform == 'win32':
    extra_compile_args = ['/O2', '/arch:AVX2']
    extra_link_args = []
else:
    extra_link_args = []

ext_modules = [
    Extension(
        "lightbinpack.cpp.ffd",
        ["lightbinpack/cpp/ffd.cpp"],
        include_dirs=[pybind11.get_include()],
        language='c++',
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),
]

setup(
    name="lightbinpack",
    version="0.0.1",
    author="TechxGenus",
    description="A lightweight library for solving bin packing problems",
    url="https://github.com/TechxGenus/LightBinPack",
    packages=find_packages(),
    package_data={
        'lightbinpack': ['cpp/*.cpp'],
    },
    ext_modules=ext_modules,
    python_requires=">=3.6",
    install_requires=[
        "pybind11>=2.6.0",
    ],
)