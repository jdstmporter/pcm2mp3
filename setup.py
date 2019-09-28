#!/usr/bin/env python3
'''
Created on 1 Dec 2017

@author: julianporter
'''
from setuptools import setup, Extension
from setuptools.config import read_configuration
import utils
import os
import os.path
from collections import namedtuple
from sys import exit
import platform


checker=utils.CheckCompiler('-std=c++14')
if not checker.run():
    print('Cannot build pcm2mp3 unless compiler supports -std=c++14')
    exit(1)

for lib in ['mp3lame','tag']:
    checker=utils.CheckLibrary(lib)
    if not checker.run():
        print('Cannot build pcm2mp3 unless lib{0} is installed and on the compiler path'.format(lib)) 
        exit(1)


configuration=read_configuration('setup.cfg')
metadata=configuration['metadata']

def sourceFilesIn(folder,exclude=[]):
    try:
        items=os.listdir(folder)
        return [os.path.join(folder,item) for item in items if item.endswith('.cpp') and item not in exclude]
    except:
        return []

def getLibPaths(libs=[]):
    paths=[]
    OS=platform.system()
    if OS == 'Darwin':
        paths=['/usr/lib','/usr/local/lib','/opt/local/lib']
    elif OS == 'Linux':
        paths=['/usr/lib','/usr/local/lib','/usr/lib/x86_64-linux-gnu']
    elif OS == 'Windows':
        print('Windows is not supported - you\'re on your own')
    else:
        print('Unidentified OS {}'.format(OS))
    
    return [p for p in paths if os.path.exists(p)]
    
        


Version = namedtuple('Version',['major','minor','maintenance'])
def processVersion():
    version=metadata['version']
    parts=version.split('.')
    if len(parts)<3: parts.extend([0,0,0])
    return Version(*(parts[:3]))

def makeExtension(module,src):
    #print("Making {} with {}".format(module,src))
    
    v=processVersion()
    mv='"{0}.{1}.{2}"'.format(v.major,v.minor,v.maintenance)
    libs=['mp3lame','tag']
    libpaths=getLibPaths(libs)
    return Extension(module,
                    define_macros = [('MAJOR_VERSION', v.major),
                                     ('MINOR_VERSION', v.minor),
                                     ('MAINTENANCE_VERSION', v.maintenance),
                                     ('MODULE_VERSION', mv)],
                    sources = src,
                    language = 'c++14',
                    include_dirs=['/usr/include'],
                    libraries = libs,
                    library_dirs = libpaths)

src=[]
src.extend(sourceFilesIn('src'))
src.extend(sourceFilesIn('pcm2mp3-cpp/src/info',['CRC16.cpp','info.cpp']))
src.extend(sourceFilesIn('pcm2mp3-cpp/src/transcode',['transcode.cpp']))
# src.extend(sourceFilesIn('pcm2mp3-cpp/src/id3',['main.cpp']))

coder = makeExtension('pcm2mp3',src)

with open('README.rst') as readme:
    longDescription = readme.read()

setup (
    cmdclass = {'cleaner' : utils.Cleaner },
    ext_modules = [coder],
    long_description = longDescription,
    test_suite = 'nose.collector' 
)
