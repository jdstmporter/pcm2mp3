/*
 * PCMObject.cpp
 *
 *  Created on: 1 May 2018
 *      Author: julianporter
 */

#include "PCMObject.hpp"
#include "../pcm2mp3-cpp/src/transcode/WAVFile.hpp"
#include "../pcm2mp3-cpp/src/transcode/AIFFFile.hpp"
#include "../pcm2mp3-cpp/src/transcode/transcoder.hpp"

using namespace pylame::pcm;

PyObject *PCM_new(PyTypeObject *type,PyObject *args,PyObject *keywords) {
	auto self = (PyPCM *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->pcm = file_t();
	}
	return (PyObject *)self;
}

// Release

void PCM_dealloc(PyPCM *self) {
	try {
		//std::cerr << "PCM: Deallocating " << (self->pcm) << std::endl;
		//std::cerr << "    Freeing object" << std::endl;
		auto p=(PyObject *)self;
		p->ob_type->tp_free(p);
	}
	catch(std::exception &e) {
		//std::cerr << "    Something went wrong " << e.what() << std::endl;
	}
	//std::cerr << "    Deallocated" << std::endl;
}

// Initialisation



int PCM_init(PyPCM *self,PyObject *args,PyObject *keywords) {
	Py_buffer buffer;

	if(!PyArg_ParseTuple(args,"y*",&buffer)) {
		PyErr_SetString(PyExc_TypeError,"No data buffer provided in PCM constructor");
		return -1;
	}
	try {
		char *orig=(char *)buffer.buf;
		pylame::data_t data(orig,orig+buffer.len);
		file_t pcm;
		if(WAVFile::isInstance(data)) {
			pcm=std::static_pointer_cast<PCMFile,WAVFile>(std::make_shared<WAVFile>(data));
		}
		else if(AIFFFile::isInstance(data)) {
			pcm=std::static_pointer_cast<PCMFile,AIFFFile>(std::make_shared<AIFFFile>(data));
		}
		if(!pcm) {
			PyErr_SetString(PyExc_OSError,"Cannot initialise PCM object: unknown file type");
			return -1;
		}
		self->pcm=pcm;
		PyBuffer_Release(&buffer);
		return 0;
	}
	catch(std::exception &e) {
		PyErr_SetString(PyExc_OSError,e.what());
		return -1;
	}
}

PyObject *PCM_sampleRate(PyPCM *self,PyObject *args,PyObject *keywords) {
	if(!self->pcm) {
		PyErr_SetString(PyExc_OSError,"No data in PCM object");
		return nullptr;
	}
	return PyLong_FromUnsignedLong((unsigned long)self->pcm->samplesPerSecond());
}

PyObject *PCM_nChannels(PyPCM *self,PyObject *args,PyObject *keywords) {
	if(!self->pcm) {
		PyErr_SetString(PyExc_OSError,"No data in PCM object");
		return nullptr;
	}
	return PyLong_FromUnsignedLong((unsigned long)self->pcm->nChans());
}

PyObject *PCM_nSamples(PyPCM *self,PyObject *args,PyObject *keywords) {
	if(!self->pcm) {
		PyErr_SetString(PyExc_OSError,"No data in PCM object");
		return nullptr;
	}
	return PyLong_FromUnsignedLong((unsigned long)self->pcm->samplesPerChannel());
}
PyObject *PCM_duration(PyPCM *self,PyObject *args,PyObject *keywords) {
	if(!self->pcm) {
		PyErr_SetString(PyExc_OSError,"No data in PCM object");
		return nullptr;
	}
	return PyFloat_FromDouble(self->pcm->duration());
}
PyObject *PCM_format(PyPCM *self,PyObject *args,PyObject *keywords) {
	if(!self->pcm) {
		PyErr_SetString(PyExc_OSError,"No data in PCM object");
		return nullptr;
	}
	std::stringstream s;
	s << self->pcm->fileType() << " " << self->pcm->sampleFormat();
	auto st=s.str();
	return PyUnicode_DecodeUTF8(st.c_str(),st.length(),nullptr);
}
Py_ssize_t PCM_len(PyPCM *self) {
	if(!self->pcm) {
			PyErr_SetString(PyExc_OSError,"No data in PCM object");
			return -1;
	}
	return (Py_ssize_t)self->pcm->size();
}

PyObject *PCM_transcode(PyPCM *self, PyObject *args, PyObject *keywords) {
	if (!self->pcm) {
		PyErr_SetString(PyExc_OSError, "No data in PCM object");
		return nullptr;
	}
	unsigned bitRate = 64;
	unsigned quality = 5;
	if(!PyArg_ParseTuple(args,"|II",&bitRate,&quality)) {
		PyErr_SetString(PyExc_TypeError,"Bad arguments");
		return nullptr;
	}
	try {
		pylame::Transcode transcode(self->pcm, quality,bitRate);
		auto out = Py_BuildValue("y#", transcode.ptr(), transcode.size());
		return out;
	} catch (std::exception &e) {
		PyErr_SetString(PyExc_OSError, e.what());
		return nullptr;
	}
}





PyMethodDef PyPCM_methods[] = {
		{"sampleRate",(PyCFunction)PCM_sampleRate,METH_NOARGS,"Get the number of samples per second"},
		{"nChannels",(PyCFunction)PCM_nChannels,METH_NOARGS,"Get the number of channels"},
		{"nSamples",(PyCFunction)PCM_nSamples,METH_NOARGS,"Get the number of samples per channel"},
		{"duration",(PyCFunction)PCM_duration,METH_NOARGS,"Get the approximate duration of the data in seconds"},
		{"format",(PyCFunction)PCM_format,METH_NOARGS,"Get a structured string describing the data format"},
		{"transcode",(PyCFunction)PCM_transcode,METH_VARARGS,"Transcode to MP3"},
		{NULL}
};

PyMemberDef PyPCM_members[] = {{
				(char *)"_pcm",
				T_OBJECT_EX,
				offsetof(PyPCM,pcm),
				READONLY,
				(char *)"opaque data object"},
				{NULL}  /* Sentinel */
};

PySequenceMethods PyPCM_sequence = {
		(lenfunc)PCM_len,			/* sq_length */
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

const char *PyPCM_Name="pcm2mp3.PCM";
const char *PyPCM_Docstr="Container for PCM data";


static PyTypeObject PyPCM_Type = {
		PyVarObject_HEAD_INIT(NULL,0)
		(char *)PyPCM_Name,             		/*tp_name*/
		sizeof(PyPCM), 			/*tp_basicsize*/
		0,                         	/*tp_itemsize*/
		(destructor)PCM_dealloc,		/*tp_dealloc*/
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
		PyPCM_Docstr,								/* tp_doc */
		0,   								/* tp_traverse */
		0,           						/* tp_clear */
		0,		               				/* tp_richcompare */
		0,		               				/* tp_weaklistoffset */
		0,		               				/* tp_iter */
		0,		               				/* tp_iternext */

		PyPCM_methods,             		/* tp_methods */
		PyPCM_members,             		/* tp_members */
		0,                         			/* tp_getset */
		0,                         			/* tp_base */
		0,                         			/* tp_dict */
		0,                         			/* tp_descr_get */
		0,                         			/* tp_descr_set */
		0,                         			/* tp_dictoffset */
		(initproc)PCM_init,      		/* tp_init */
		0,                         			/* tp_alloc */
		(newfunc)PCM_new,                 		/* tp_new */
};

bool PCMManager::isReady() { return PyType_Ready(&PyPCM_Type)>=0; }
void PCMManager::inc() { Py_INCREF(&PyPCM_Type); }
void PCMManager::add(PyObject *m,const char *name) {
	PyModule_AddObject(m,name,(PyObject *)&PyPCM_Type);
}
PyPCM * PCMManager::make() {
	return (PyPCM *)_PyObject_New(&PyPCM_Type);
}






