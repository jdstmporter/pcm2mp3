#!/usr/bin/python3
'''
Created on 3 May 2018

@author: julianporter
'''

import pcm2mp3
import traceback
import sys
import re
import os



if __name__ == '__main__':
    TRANSCODE=re.match('^[tT]',sys.argv[1]) if len(sys.argv)>1 else 0

    try:
        os.remove('test.mp3')
    except Exception as e:
        print(f"Couldn't remove test.mp3: {e}")
        
    try:
        
        if TRANSCODE:
            print('Using transcode')
            with open('test.wav','rb') as infile:
                print('Starting transcoding')
                out=pcm2mp3.transcode(infile.read(),quality=5,bitrate=16)
                print('Transcoding completed')
                with open('test.mp3','wb') as outfile:
                    outfile.write(out)
        else:
            print('Using MP3 Object')
            with open('test.wav','rb') as infile:
                pcm=pcm2mp3.PCM(infile.read())
                mp3=pcm2mp3.MP3(pcm,quality=5,rate=16)
                with open('test.mp3','wb') as outfile:
                    outfile.write(mp3.data())
        print('Successful conversion')
        
        result=pcm2mp3.mp3Check('test.mp3')
        if result:
            print('Output file verifies as MP3: success')
        else:
            print('Output file is not a valid MP3: failure')
            
    except:
        ety,ev,etr= sys.exc_info()
        print(f"There was an error : {ety}, {ev}")
        stack=traceback.extract_tb(etr)
        for line in stack:
            print(f'{line.filename} : {line.lineno} - {line.name} : {line.line}')
            
    finally:
        print('Completion')
  
        

        
        