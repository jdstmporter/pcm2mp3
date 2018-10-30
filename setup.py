#!/usr/bin/python3
'''
Created on 1 Dec 2017

@author: julianporter
'''
from setuptools import setup, Extension
from setuptools.config import read_configuration
import utils
import os
from collections import namedtuple
from sys import exit


checker=utils.CheckCompiler('-std=c++14')
if not checker.run():
    print("Cannot build pcm2mp3 unless compiler supports -std=c++14")
    exit(1)

for lib in ['mp3lame','tag']:
    checker=utils.CheckLibrary(lib)
    if not checker.run():
        print(f'Cannot build pcm2mp3 unless lib{lib} is installed and on the compiler path')
        exit(1)


configuration=read_configuration('setup.cfg')
metadata=configuration['metadata']

def sourceFilesIn(folder,exclude=[]):
    try:
        items=os.listdir(folder)
        return [os.path.join(folder,item) for item in items if item.endswith('.cpp') and item not in exclude]
    except:
        return []




Version = namedtuple('Version',['major','minor','maintenance'])
def processVersion():
    version=metadata['version']
    parts=version.split('.')
    if len(parts)<3: parts.extend([0,0,0])
    return Version(*(parts[:3]))

def makeExtension(module,src):
    #print("Making {} with {}".format(module,src))
    
    v=processVersion()
    mv=f'"{v.major}.{v.minor}.{v.maintenance}"'
    return Extension(module,
                    define_macros = [('MAJOR_VERSION', v.major),
                                     ('MINOR_VERSION', v.minor),
                                     ('MAINTENANCE_VERSION', v.maintenance),
                                     ('MODULE_VERSION', mv)],
                    sources = src,
                    language = 'c++',
                    include_dirs=['/usr/include'],
                    libraries = ['mp3lame','tag'],
                    library_dirs = ['/usr/lib/x86_64-linux-gnu'])

src=[]
src.extend(sourceFilesIn('src'))
src.extend(sourceFilesIn('pcm2mp3-cpp/src/info',['CRC16.cpp','info.cpp']))
src.extend(sourceFilesIn('pcm2mp3-cpp/src/transcode',['transcode.cpp']))
src.extend(sourceFilesIn('pcm2mp3-cpp/src/id3',['main.cpp']))

coder = makeExtension('pcm2mp3',src)

with open('README.rst') as readme:
    longDescription = readme.read()

setup (
    cmdclass = {'cleaner' : utils.Cleaner },
    ext_modules = [coder],
    long_description = longDescription,
    test_suite = 'nose.collector' 
)
