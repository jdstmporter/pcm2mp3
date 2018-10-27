'''
Created on 2 Dec 2017

@author: julianporter
'''
from setuptools import Command
import shutil
import os

class Cleaner(Command) :
    
    
    description='Perform a true deep clean, removing distributions, builds and egg-info'
    
    user_options=[]
    
    def __init__(self,dist,**kwargs):
        super(Cleaner,self).__init__(dist,**kwargs)
        
    def initialize_options(self):
        self.directories=[]
        self.files=[]
    
    def finalize_options(self):
        self.directories=['build','dist','wav2mp3.egg-info']
        self.files=[]
    
    def run(self):
        for directory in self.directories:
            try:
                shutil.rmtree(directory)
                print("{} deleted".format(directory))
            except FileNotFoundError:
                print("{} does not exist, so not deleted...".format(directory))
            except Exception as e:
                print("{} : {}".format(e.__class__.__name__,e))
        for file in self.files:
            try:
                os.remove(file)
                print("{} deleted".format(directory))
            except FileNotFoundError:
                print("{} does not exist, so not deleted...".format(directory))
            except Exception as e:
                print("{} : {}".format(e.__class__.__name__,e))
            
        
        