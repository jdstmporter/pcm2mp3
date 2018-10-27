/*
 * MP3TestObject.hpp
 *
 *  Created on: 22 Oct 2018
 *      Author: julianporter
 */

#ifndef LIB_MP3TESTOBJECT_HPP_
#define LIB_MP3TESTOBJECT_HPP_

#include "PyHeader.hpp"
#include "../pcm2mp3-cpp/src/info/MP3.hpp"
#include "../pcm2mp3-cpp/src/info/MP3File.hpp"
#include "../pcm2mp3-cpp/src/info/MP3Frame.hpp"
#include "../pcm2mp3-cpp/src/info/Test.hpp"

using test_t = std::shared_ptr<mp3::MP3TestResult>;

typedef struct {
    PyObject_HEAD
    test_t test;
} PyMP3Test;


PyObject *MP3Test_new(PyTypeObject *type,PyObject *args,PyObject *keywords);
void MP3Test_dealloc(PyMP3Test *self);
int MP3Test_init(PyMP3Test *self,PyObject *args,PyObject *keywords);

PyObject * MP3Test_mpegType(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_sampleRate(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_bitRate(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_duration(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_nSamples(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_nFrames(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_isGood(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_message(PyMP3Test *self,PyObject *args,PyObject *keywords);
PyObject * MP3Test_log(PyMP3Test *self,PyObject *args,PyObject *keywords);



class MP3TestManager {
public:

	 MP3TestManager() {};

	 void prepare();
	 bool isReady();
	 void inc();
	 void add(PyObject *m,const char *name);
	 PyMP3Test * make();


};

#endif /* LIB_MP3TESTOBJECT_HPP_ */
