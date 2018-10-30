/*
 * MP3Object.hpp
 *
 *  Created on: 1 May 2018
 *      Author: julianporter
 */

#ifndef MP3OBJECT_HPP_
#define MP3OBJECT_HPP_

#include "PyHeader.hpp"
#include "../pcm2mp3-cpp/src/transcode/PCMFile.hpp"
#include "../pcm2mp3-cpp/src/transcode/MP3File.hpp"
#include "../pcm2mp3-cpp/src/transcode/MP3Data.hpp"

using mp3_t = std::shared_ptr<pylame::MP3File>;

typedef struct {
    PyObject_HEAD
    mp3_t mp3;
} PyMP3;




 PyObject *MP3_new(PyTypeObject *type,PyObject *args,PyObject *keywords);
 void MP3_dealloc(PyMP3 *self);
 int MP3_init(PyMP3 *self,PyObject *args,PyObject *keywords);
 PyObject * MP3_sampleRate(PyMP3 *self,PyObject *args,PyObject *keywords);
 PyObject * MP3_duration(PyMP3 *self,PyObject *args,PyObject *keywords);
 Py_ssize_t PCM_len(PyMP3 *self);

 PyObject * MP3_data(PyMP3 *self,PyObject *args,PyObject *keywords);

 int MP3_getBuffer(PyMP3 *self, Py_buffer *view, int flags);



 class MP3Manager {
  public:


 	 MP3Manager() {};

 	 void prepare();
 	 bool isReady();
 	 void inc();
 	 void add(PyObject *m,const char *name);
 	 PyMP3 * make();




  };

#endif /* MP3OBJECT_HPP_ */
