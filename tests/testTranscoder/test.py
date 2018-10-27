'''
Created on 24 Oct 2018

@author: julianporter
'''


import pcm2mp3
import os


TRANSCODE='transcode'
CLASS_BASED='classBased'



class Converter(object):
    
    def __init__(self,name,mode,quality=5,bitrate=16):
        self.name=name
        self.infile=f'{name}.wav'
        self.outfile=f'{name}.mp3'
        self.quality=quality
        self.bitrate=bitrate
        self.mode=mode
        
    def rm(self):
        try:
            os.remove(self.outfile)
        except Exception as e:
            print(f"Couldn't remove ${self.outfile}: {e}")
            
    def check(self):
        try:
            result=pcm2mp3.mp3Check(self.outfile)
            print(f'Check result is {result}')
            return result
        except Exception as e:
            raise Exception(f"Error while checking: {e}")

            
    def transcode(self,data):
        return pcm2mp3.transcode(data,quality=self.quality,bitrate=self.bitrate)
    
    def classBased(self,data):
        pcm=pcm2mp3.PCM(data)
        mp3=pcm2mp3.MP3(pcm,quality=self.quality,rate=self.bitrate)
        return mp3.data()
        
            
    def convert(self):
        try:
            with open(self.infile,'rb') as infile:
                dataIn=infile.read()
                action=getattr(self,self.mode)
                dataOut=action(dataIn)
                with open(self.outfile,'wb') as outfile:
                    outfile.write(dataOut)
        except Exception as e:
            raise Exception(f'Error while checking {e}')
            
    def __call__(self):
        try:
            print(f'Test with file {self.name} mode {self.mode} Q {self.quality} B {self.bitrate}')
            self.rm()
            self.convert()
            out=self.check()
            print(f'Check result is {out}')
            return out
        except Exception as e:
            print(f'{e}')
            return False
        
        
if __name__ == '__main__':
    from sys import argv

    kl=Converter(argv[1],argv[2],quality=5,bitrate=int(argv[3]))
    kl.rm()
    kl.convert()
    out=kl.check()
    print(f'Check result is {out}')
        

            

            
        
                
                
