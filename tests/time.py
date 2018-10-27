'''
Created on 5 Dec 2017

@author: julianporter
'''

from pcm2mp3 import transcode
from time import perf_counter
from sys import argv


class TestResult(object):
    def __init__(self,rate=0,quality=0,nTests=0,time=0,length=0):
        self.rate=rate
        self.quality=quality
        self.nTests=nTests
        self.time=time
        self.length=length

    def __str__(self):
        return "{}, {}, {}, {}, {}, {}".format(self.rate,self.quality,self.nTests,self.time,self.mean,self.length)
    
    @property
    def mean(self):
        return self.time/self.nTests


class TimeTest(object):
    
    def __init__(self,infile,nTests=100,rate=[8,24,32,48,64,96,128,192],quality=[2,5]):
        with open(infile,"rb") as f:
            self.infile=f.read()
    
        self.nTests=nTests
        self.rate=rate
        self.quality=quality
        self.results=[]
        
    def _test(self,rate,quality):
        count=0
        total=0
        nbytes=0
        for _ in range(self.nTests):
            try:
                start=perf_counter()
                o=transcode(self.infile,bitrate=rate,quality=quality)
                duration=perf_counter()-start
                total+=duration
                nbytes+=len(o)
                count+=1
            except Exception as e:
                print(e)
        return TestResult(rate=rate,quality=quality,nTests=count,time=total,length=nbytes/count)
        
               
    def run(self):
        for rate in self.rate:
            for quality in self.quality:
                print("Testing rate={}, quality={}".format(rate,quality))
                result=self._test(rate,quality)
                self.results.append(result)
                
    def __iter__(self):    
        return iter(self.results)
    
if __name__=='__main__':
    n=int(argv[1])
    t=TimeTest('test.wav',n)
    t.run()
    lines=[str(r) for r in t]
    lines.insert(0, 'rate,quality,nTests,time,mean,length')
    for l in lines: print(l)
    if len(argv)>2:
        lines=[l+'\n' for l in lines]
        with open(argv[2],'w') as f:
            f.writelines(lines)
        

