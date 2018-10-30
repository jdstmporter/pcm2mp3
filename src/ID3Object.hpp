/*
 * ID3Object.hpp
 *
 *  Created on: 29 Oct 2018
 *      Author: julianporter
 */

#ifndef SRC_ID3OBJECT_HPP_
#define SRC_ID3OBJECT_HPP_

#include "PyHeader.hpp"
#include "../pcm2mp3-cpp/src/transcode/ID3Header.hpp"

using id3_t = std::shared_ptr<pylame::id3::ID3Header>;

typedef struct {
	PyObject_HEAD;
	id3_t id3;
} PyID3;

PyObject *ID3_new(PyTypeObject *type,PyObject *args,PyObject *keywords);
 void ID3_dealloc(PyID3 *self);
 int ID3_init(PyID3 *self,PyObject *args,PyObject *keywords);
 PyObject * ID3_data(PyID3 *self,PyObject *args,PyObject *keywords);
 int ID3_set(PyID3 *self,PyObject *key,PyObject *value);

 int ID3_getBuffer(PyID3 *self, Py_buffer *view, int flags);

 bool check_ID3(PyObject *o);

 class ID3Manager {
  public:

 	 ID3Manager() {};

 	 void prepare();
 	 bool isReady();
 	 void inc();
 	 void add(PyObject *m,const char *name);
 	 PyID3 * make();




  };

#endif /* SRC_ID3OBJECT_HPP_ */
