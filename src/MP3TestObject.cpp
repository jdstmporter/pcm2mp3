/*
 * MP3TestObject.cpp
 *
 *  Created on: 22 Oct 2018
 *      Author: julianporter
 */

#include "MP3TestObject.hpp"

using namespace mp3;



PyObject *MP3Test_new(PyTypeObject *type,PyObject *args,PyObject *keywords) {
	auto self = (PyMP3Test *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->test = test_t();
	}
	return (PyObject *)self;
}

// Release

void MP3Test_dealloc(PyMP3Test *self) {
	try {
		auto p=(PyObject *)self;
		p->ob_type->tp_free(p);
	}
	catch(std::exception &e) {
		//std::cerr << "    Something went wrong " << e.what();
	}
}



int MP3Test_init(PyMP3Test *self,PyObject *args,PyObject *keywords) {

	std::string filename;
	bool verbose = false;

	//std::cerr << "    About to parse positional arguments" << std::endl;

	if(!PyTuple_Check(args))  {
		PyErr_SetString(PyExc_OSError,"Arguments are not a tuple");
		return -1;
	}
	if(PyTuple_Size(args)!=1) {
		PyErr_SetString(PyExc_OSError,"Too few / many positional arguments (wants one)");
		return -1;
	}
	filename=toString(PyTuple_GetItem(args,0));

	//std::cerr << "    Completed parsing positional arguments" << std::endl;

	//std::cerr << "    About to parse keyword arguments" << std::endl;
	if(!PyDict_Check(keywords)) {
		PyErr_SetString(PyExc_OSError,"Keywords are not a dictionary");
		return -1;
	}

	std::map<std::string,PyObject*> kwargs;
	Py_ssize_t pos=0;
	PyObject *key, *value;
	while(PyDict_Next(keywords,&pos,&key,&value)) {
		std::string kv=toString(key);
		kwargs[kv]=value;
	}
	//std::for_each(kwargs.begin(),kwargs.end(),[](auto kv) { std::cerr << kv.first << std::endl;});
	try { verbose=toBool(kwargs.at("verbose")); } catch(...) {}

	//std::cerr << "Filename : " << filename << " Verbose: " << verbose << std::endl;

	try {
		MP3Test test(filename);
		test.parse(verbose);
		self->test=test();
		return 0;
	}
	catch(std::exception &e) {
		//std::cerr << "    Error: Error while loading file and parsing" << std::endl;
		PyErr_SetString(PyExc_OSError,e.what());
		return -1;
	}
}

PyObject *MP3Test_sampleRate(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	return fromUInt(self->test->samplerate);
}
PyObject *MP3Test_bitRate(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	return fromUInt(self->test->bitrate);
}
PyObject *MP3Test_duration(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	return PyFloat_FromDouble(self->test->duration);
}
PyObject *MP3Test_nSamples(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	return fromUInt(self->test->nsamples);
}
PyObject *MP3Test_nFrames(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	return fromUInt(self->test->nframes);
}
PyObject *MP3Test_message(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	return fromString(self->test->message.c_str());
}
PyObject *MP3Test_mpegType(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	if(self->test->spec.isGood()) {
		PyObject *o[3];
		for(auto i=0;i<3;i++) {
			auto s=self->test->spec.str(i);
			o[i]=fromString(s);
		}
		return PyTuple_Pack(3,o[0],o[1],o[2]);
	}
	else {
		Py_RETURN_NONE;
	}
}
PyObject *MP3Test_isGood(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	if(self->test->isGood()) { Py_RETURN_TRUE; }
	else { Py_RETURN_FALSE; }
}
PyObject *MP3Test_log(PyMP3Test *self,PyObject *args,PyObject *keywords) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	auto log=PyList_New(self->test->log.size());
	auto count=0;
	for(auto it=self->test->log.begin();it!=self->test->log.end();it++) {
		auto s=*it;
		PyList_SetItem(log,count++,fromString(s));
	}
	return log;
}

PyObject *MP3Test_repr(PyMP3Test *self) {
	if(!self->test) {
		PyErr_SetString(PyExc_OSError,"No data in test object");
		return nullptr;
	}
	std::stringstream s;
	s << self->test;
	return fromString(s.str());
}



PyMethodDef PyMP3Test_methods[] = {
		{"sampleRate",(PyCFunction)MP3Test_sampleRate,METH_NOARGS,"Get the number of samples per second"},
		{"bitRate",(PyCFunction)MP3Test_bitRate,METH_NOARGS,"Get the number of bits per second"},
		{"nSamples",(PyCFunction)MP3Test_nSamples,METH_NOARGS,"Get the number of samples per channel"},
		{"duration",(PyCFunction)MP3Test_duration,METH_NOARGS,"Get the approximate duration of the data in seconds"},
		{"mpegType",(PyCFunction)MP3Test_mpegType,METH_NOARGS,"Get a tuple describing the MPEG format"},
		{"log",(PyCFunction)MP3Test_log,METH_VARARGS,"Verbose output"},
		{"isGood",(PyCFunction)MP3Test_isGood,METH_NOARGS,"Is i a good MP3 file"},
		{"message",(PyCFunction)MP3Test_message,METH_NOARGS,"Brief diagnostic message"},
		{"nFrames",(PyCFunction)MP3Test_nFrames,METH_NOARGS,"Get the number of frames in the file"},
		{NULL}
};

PyMemberDef PyMP3Test_members[] = {{
				(char *)"_test",
				T_OBJECT_EX,
				offsetof(PyMP3Test,test),
				READONLY,
				(char *)"opaque data object"},
				{NULL}  /* Sentinel */
};



const char *PyMP3Test_Name="pcm2mp3.MP3Check";
const char *PyMP3Test_Docstr="Check MP3 File";


static PyTypeObject PyMP3Test_Type = {
		PyVarObject_HEAD_INIT(NULL,0)
		(char *)PyMP3Test_Name,             		/*tp_name*/
		sizeof(PyMP3Test), 			/*tp_basicsize*/
		0,                         	/*tp_itemsize*/
		(destructor)MP3Test_dealloc,		/*tp_dealloc*/
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
		PyMP3Test_Docstr,								/* tp_doc */
		0,   								/* tp_traverse */
		0,           						/* tp_clear */
		0,		               				/* tp_richcompare */
		0,		               				/* tp_weaklistoffset */
		0,		               				/* tp_iter */
		0,		               				/* tp_iternext */

		PyMP3Test_methods,             		/* tp_methods */
		PyMP3Test_members,             		/* tp_members */
		0,                         			/* tp_getset */
		0,                         			/* tp_base */
		0,                         			/* tp_dict */
		0,                         			/* tp_descr_get */
		0,                         			/* tp_descr_set */
		0,                         			/* tp_dictoffset */
		(initproc)MP3Test_init,      		/* tp_init */
		0,                         			/* tp_alloc */
		(newfunc)MP3Test_new,                 		/* tp_new */
};



bool MP3TestManager::isReady() { return PyType_Ready(&PyMP3Test_Type)>=0; }
void MP3TestManager::inc() { Py_INCREF(&PyMP3Test_Type); }
void MP3TestManager::add(PyObject *m,const char *name) {
	PyModule_AddObject(m,name,(PyObject *)&PyMP3Test_Type);
}
PyMP3Test * MP3TestManager::make() {
	return (PyMP3Test *)_PyObject_New(&PyMP3Test_Type);
}



