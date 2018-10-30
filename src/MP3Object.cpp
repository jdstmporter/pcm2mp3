/*
 * MP3Object.cpp
 *
 *  Created on: 1 May 2018
 *      Author: julianporter
 */

#include "MP3Object.hpp"
#include "PCMObject.hpp"
#include "ID3Object.hpp"


using namespace pylame::mp3;

#ifdef DEBUG
#define debug(ARGS) std::cerr << ARGS << std::endl
#else
#define debug(ARGS) ()
#endif


PyObject *MP3_new(PyTypeObject *type,PyObject *args,PyObject *keywords) {
	auto self = (PyMP3 *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->mp3 = mp3_t();
	}
	return (PyObject *)self;
}

// Release

void MP3_dealloc(PyMP3 *self) {
	try {
		debug("MP3: Deallocating " << self->mp3);
		debug("    Freeing object");
		auto p=(PyObject *)self;
		p->ob_type->tp_free(p);
	}
	catch(std::exception &e) {
		debug("    Something went wrong " << e.what());
	}
	debug("    Deallocated");
}

// Initialisation




int MP3_init(PyMP3 *self,PyObject *args,PyObject *keywords) {

	if(!PyTuple_Check(args))  {
		PyErr_SetString(PyExc_OSError,"Arguments are not a tuple");
		return -1;
	}
	auto n=PyTuple_Size(args);
	if(n==0) {
		PyErr_SetString(PyExc_OSError,"Too few positional arguments (wants one or two)");
		return -1;
	}
	if(n>2) {
		PyErr_SetString(PyExc_OSError,"Too few / many positional arguments (wants one or two)");
		return -1;
	}
	PyPCM *pcm=(PyPCM *)PyTuple_GetItem(args,0);
	PyID3 *id=(n==2) ? (PyID3 *)PyTuple_GetItem(args,1) : nullptr;

	try {
		if (!pcm->pcm) {
			//std::cerr << "    Error: No data" << std::endl;
			PyErr_SetString(PyExc_OSError, "No data in PCM object");
			return -1;
		}
		id3_t id3=(id!=nullptr) ? id->id3 : std::make_shared<pylame::id3::ID3Header>();
		auto mp3=std::make_shared<pylame::MP3File>(*id3);
		//std::cerr << "    About to transcode" << std::endl;
		mp3->transcode(pcm->pcm);
		//std::cerr << "    Transcoding completed" << std::endl;
		self->mp3=mp3;
		return 0;
	}
	catch(std::exception &e) {
		//std::cerr << "    Error: Error while loading file and transcoding" << std::endl;
		PyErr_SetString(PyExc_OSError,e.what());
		return -1;
	}
}

PyObject *MP3_sampleRate(PyMP3 *self,PyObject *args,PyObject *keywords) {
	if(!self->mp3) {
		PyErr_SetString(PyExc_OSError,"No data in MP3 object");
		return nullptr;
	}
	return PyLong_FromUnsignedLong((unsigned long)self->mp3->getRate());
}
PyObject *MP3_duration(PyMP3 *self,PyObject *args,PyObject *keywords) {
	if(!self->mp3) {
		PyErr_SetString(PyExc_OSError,"No data in MP3 object");
		return nullptr;
	}
	return PyFloat_FromDouble(self->mp3->getDuration());
}

Py_ssize_t MP3_len(PyMP3 *self) {
	if(!self->mp3) {
		PyErr_SetString(PyExc_OSError,"No data in MP3 object");
		return -1;
	}
	return self->mp3->size();
}

PyObject *MP3_data(PyMP3 *self,PyObject *args,PyObject *keywords) {
	if(!self->mp3) {
			PyErr_SetString(PyExc_OSError,"No data in MP3 object");
			return nullptr;
	}
	return PyByteArray_FromStringAndSize(reinterpret_cast<const char *>(self->mp3->bytes()),self->mp3->size());
}

/* Buffer protocol */

int MP3_getBuffer(PyMP3 *self, Py_buffer *view, int flags) {
	if(!self->mp3) {
			PyErr_SetString(PyExc_BufferError,"No data in MP3 object");
			view->obj=nullptr;
			return -1;
	}
	if(flags&PyBUF_WRITABLE) {
		PyErr_SetString(PyExc_BufferError,"MP3 Object is read-only");
		return -1;
	}
	if(flags&(PyBUF_INDIRECT|PyBUF_STRIDES|PyBUF_ND)) {
		PyErr_SetString(PyExc_BufferError,"MP3 Object is always contiguous and linear");
		return -1;
	}
	auto buf=reinterpret_cast<const char *>(self->mp3->bytes());
	auto len=self->mp3->size();
	if(!PyBuffer_FillInfo(view,(PyObject *)self,(void *)buf,(long)len,1,flags)) {
		PyErr_SetString(PyExc_BufferError,"Cannot initialise MP3 Buffer");
		return -1;
	}
	view->obj=(PyObject *)self;
	Py_INCREF(view->obj);
	return 0;
}





PyMethodDef PyMP3_methods[] = {
		{"sampleRate",(PyCFunction)MP3_sampleRate,METH_NOARGS,"Get the number of samples per second"},
		{"duration",(PyCFunction)MP3_duration,METH_NOARGS,"Get the approximate duration of the data in seconds"},
		{"data",(PyCFunction)MP3_data,METH_NOARGS,"Get the MP3 file data"},
		{NULL}
};

PyMemberDef PyMP3_members[] = {{
				(char *)"_mp3",
				T_OBJECT_EX,
				offsetof(PyMP3,mp3),
				READONLY,
				(char *)"opaque data object"},
				{NULL}  /* Sentinel */
};

PySequenceMethods PyMP3_sequence = {
		(lenfunc)MP3_len,			/* sq_length */
		0,								/* sq_concat */
		0,								/* sq_repeat */
		0,								/* sq_item */
		0,								/* sq_slice */
		0,								/* sq_ass_item */
		0,								/* sq_ass_slice */
		0,								/* sq_contains */
		0,								/* sq_inplace_concat */
		0,								/* sq_inplace_repeat */
};

PyBufferProcs PyMP3_buffer = {
		(getbufferproc) MP3_getBuffer,
		0
};

const char *PyMP3_Name="pcm2mp3.MP3";
const char *PyMP3_Docstr="Container / transcoder for MP3 data";


static PyTypeObject PyMP3_Type = {
		PyVarObject_HEAD_INIT(NULL,0)
		(char *)PyMP3_Name,             		/*tp_name*/
		sizeof(PyMP3), 			/*tp_basicsize*/
		0,                         	/*tp_itemsize*/
		(destructor)MP3_dealloc,		/*tp_dealloc*/
		0,                         			/*tp_print*/
		0,                         			/*tp_getattr*/
		0,                         			/*tp_setattr*/
		0,                         			/*tp_compare*/
		0,                         			/*tp_repr*/
		0,                         			/*tp_as_number*/
		0,                  /*tp_as_sequence*/
		0,                         			/*tp_as_mapping*/
		0,                         			/*tp_hash */
		0,                         			/*tp_call*/
		0,                         			/*tp_str*/
		0,                         			/*tp_getattro*/
		0,                         			/*tp_setattro*/
		0,                         			/*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT, 				/*tp_flags*/
		PyMP3_Docstr,								/* tp_doc */
		0,   								/* tp_traverse */
		0,           						/* tp_clear */
		0,		               				/* tp_richcompare */
		0,		               				/* tp_weaklistoffset */
		0,		               				/* tp_iter */
		0,		               				/* tp_iternext */

		PyMP3_methods,             		/* tp_methods */
		PyMP3_members,             		/* tp_members */
		0,                         			/* tp_getset */
		0,                         			/* tp_base */
		0,                         			/* tp_dict */
		0,                         			/* tp_descr_get */
		0,                         			/* tp_descr_set */
		0,                         			/* tp_dictoffset */
		(initproc)MP3_init,      		/* tp_init */
		0,                         			/* tp_alloc */
		(newfunc)MP3_new,                 		/* tp_new */
};



void MP3Manager::prepare() {
	PyMP3_Type.tp_as_sequence=&PyMP3_sequence;
	PyMP3_Type.tp_as_buffer=&PyMP3_buffer;
}
bool MP3Manager::isReady() { return PyType_Ready(&PyMP3_Type)>=0; }
void MP3Manager::inc() { Py_INCREF(&PyMP3_Type); }
void MP3Manager::add(PyObject *m,const char *name) {
	PyModule_AddObject(m,name,(PyObject *)&PyMP3_Type);
}
PyMP3 * MP3Manager::make() {
	return (PyMP3 *)_PyObject_New(&PyMP3_Type);
}







