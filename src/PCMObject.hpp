/*
 * PCMObject.hpp
 *
 *  Created on: 1 May 2018
 *      Author: julianporter
 */

#ifndef LIB_PCMOBJECT_HPP_
#define LIB_PCMOBJECT_HPP_

#include "PyHeader.hpp"
#include "../pcm2mp3-cpp/src/transcode/PCMFile.hpp"



typedef struct {
    PyObject_HEAD
    pylame::pcm::file_t pcm;
} PyPCM;

 PyObject *PCM_new(PyTypeObject *type,PyObject *args,PyObject *keywords);
 void PCM_dealloc(PyPCM *self);
 int PCM_init(PyPCM *self,PyObject *args,PyObject *keywords);
 PyObject * PCM_sampleRate(PyPCM *self,PyObject *args,PyObject *keywords);
 PyObject * PCM_nChannels(PyPCM *self,PyObject *args,PyObject *keywords);
 PyObject * PCM_nSamples(PyPCM *self,PyObject *args,PyObject *keywords);
 PyObject * PCM_duration(PyPCM *self,PyObject *args,PyObject *keywords);
 PyObject * PCM_format(PyPCM *self,PyObject *args,PyObject *keywords);
 Py_ssize_t PCM_len(PyPCM *self);
 PyObject * PCM_transcode(PyPCM *self, PyObject *args, PyObject *keywords);

 class PCMManager {
  public:
 	 PCMManager() {};

 	 bool isReady();
 	 void inc();
 	 void add(PyObject *m,const char *name);
 	 PyPCM * make();




  };




#endif /* LIB_PCMOBJECT_HPP_ */
