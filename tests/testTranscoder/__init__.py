from .test import Converter
from nose.tools import eq_

def action(f,mode,rate):
    if f=='test' and rate==32 :
        expected=False
    else:
        expected=True
    
    tester=Converter(f,mode,quality=5,bitrate=rate)
    result=tester()
    eq_(expected,result)

def testConversion(): 
    for f in ['test', 'sample']:
        for mode in ['transcode','classBased']:
            for bitrate in [16,32,64]:
                yield action, f, mode, bitrate 