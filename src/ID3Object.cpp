/*
 * ID3Object.cpp
 *
 *  Created on: 29 Oct 2018
 *      Author: julianporter
 */

#include "ID3Object.hpp"

#ifdef DEBUG
#define debug(ARGS) std::cerr << ARGS << std::endl
#else
#define debug(ARGS) ()
#endif

using namespace pylame;

PyObject *ID3_new(PyTypeObject *type,PyObject *args,PyObject *keywords) {
	auto self = (PyID3 *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->id3 = id3_t();
	}
	return (PyObject *)self;
}

// Release


void ID3_dealloc(PyID3 *self) {
	try {
		debug("ID3: Deallocating " << self->id3);
		debug("    Freeing object");
		auto p=(PyObject *)self;
		p->ob_type->tp_free(p);
	}
	catch(std::exception &e) {
		debug("    Something went wrong " << e.what());
	}
	debug("    Deallocated");
}

int ID3_init(PyID3 *self,PyObject *args,PyObject *keywords) {
	if(!PyTuple_Check(args))  {
			PyErr_SetString(PyExc_OSError,"Arguments are not a tuple");
			return -1;
		}
	id3::ID3Version version=id3::ID3Version::ID3v2;
	auto n=PyTuple_Size(args);
	if(n==1) {
		try {
			auto v=(unsigned)toLong(PyTuple_GetItem(args,0));
			version=static_cast<id3::ID3Version>(v);
		}
		catch(...) {
			version=id3::ID3Version::ID3v2;
		}
	}
	else if(n>1) {
		PyErr_SetString(PyExc_OSError,"Too many positional arguments (wants one or zero)");
		return -1;
	}

	self->id3=std::make_shared<id3::ID3Header>(version);
	return 0;

}

int ID3_set(PyID3 *self,PyObject *key,PyObject *value) {
	if(!self->id3) {
		PyErr_SetString(PyExc_OSError,"No data in ID3 object");
		return -1;
	}
	try {
		auto k=toLong(key);
		id3::ID3Field field=static_cast<id3::ID3Field>((unsigned)k);
		if(PyFloat_Check(value)) self->id3->set(field,toDouble(value));
		else if(PyUnicode_Check(value)) self->id3->set(field,toString(value));
		return 0;
	}
	catch(std::exception &e) {
		PyErr_SetString(PyExc_OSError,e.what());
		return -1;
	}
}

PyObject * ID3_data(PyID3 *self,PyObject *args,PyObject *keywords) {
	if(!self->id3) {
		PyErr_SetString(PyExc_OSError,"No data in ID3 object");
		return nullptr;
	}
	try {
		auto data=self->id3->publish();
		return PyByteArray_FromStringAndSize(reinterpret_cast<const char *>(data.data()),data.size());
	}
	catch(std::exception &e) {
			//std::cerr << "    Error: Error while loading file and transcoding" << std::endl;
			PyErr_SetString(PyExc_OSError,e.what());
			return nullptr;
		}
}

int ID3_getBuffer(PyID3 *self, Py_buffer *view, int flags) {
	if(!self->id3) {
			PyErr_SetString(PyExc_BufferError,"No data in ID3 object");
			view->obj=nullptr;
			return -1;
	}
	if(flags&PyBUF_WRITABLE) {
		PyErr_SetString(PyExc_BufferError,"ID3 Object is read-only");
		return -1;
	}
	if(flags&(PyBUF_INDIRECT|PyBUF_STRIDES|PyBUF_ND)) {
		PyErr_SetString(PyExc_BufferError,"ID3 Object is always contiguous and linear");
		return -1;
	}
	auto data=self->id3->publish();
	auto buf=reinterpret_cast<const char *>(data.data());
	auto len=data.size();
	if(!PyBuffer_FillInfo(view,(PyObject *)self,(void *)buf,(long)len,1,flags)) {
		PyErr_SetString(PyExc_BufferError,"Cannot initialise ID3 Buffer");
		return -1;
	}
	view->obj=(PyObject *)self;
	Py_INCREF(view->obj);
	return 0;
}


PyMethodDef PyID3_methods[] = {
		{"data",(PyCFunction)ID3_data,METH_NOARGS,"Get the ID3 tag block"},
		{NULL}
};

PyMemberDef PyID3_members[] = {{
				(char *)"_id3",
				T_OBJECT_EX,
				offsetof(PyID3,id3),
				READONLY,
				(char *)"opaque data object"},
				{NULL}  /* Sentinel */
};

PyBufferProcs PyID3_buffer = {
		(getbufferproc) ID3_getBuffer,
		0
};

PyMappingMethods PyID3_mapping = {
		0,
		0,
		(objobjargproc) ID3_set
};

const char *PyID3_Name="pcm2mp3.ID3";
const char *PyID3_Docstr="ID3 tag container";

static PyTypeObject PyID3_Type = {
		PyVarObject_HEAD_INIT(NULL,0)
		(char *)PyID3_Name,             		/*tp_name*/
		sizeof(PyID3), 			/*tp_basicsize*/
		0,                         	/*tp_itemsize*/
		(destructor)ID3_dealloc,		/*tp_dealloc*/
		0,                         			/*tp_print*/
		0,                         			/*tp_getattr*/
		0,                         			/*tp_setattr*/
		0,                         			/*tp_compare*/
		0,                         			/*tp_repr*/
		0,                         			/*tp_as_number*/
		0,                  /*tp_as_sequence*/
		&PyID3_mapping,                         			/*tp_as_mapping*/
		0,                         			/*tp_hash */
		0,                         			/*tp_call*/
		0,                         			/*tp_str*/
		0,                         			/*tp_getattro*/
		0,                         			/*tp_setattro*/
		0,                         			/*tp_as_buffer*/
		Py_TPFLAGS_DEFAULT, 				/*tp_flags*/
		PyID3_Docstr,								/* tp_doc */
		0,   								/* tp_traverse */
		0,           						/* tp_clear */
		0,		               				/* tp_richcompare */
		0,		               				/* tp_weaklistoffset */
		0,		               				/* tp_iter */
		0,		               				/* tp_iternext */

		PyID3_methods,             		/* tp_methods */
		PyID3_members,             		/* tp_members */
		0,                         			/* tp_getset */
		0,                         			/* tp_base */
		0,                         			/* tp_dict */
		0,                         			/* tp_descr_get */
		0,                         			/* tp_descr_set */
		0,                         			/* tp_dictoffset */
		(initproc)ID3_init,      		/* tp_init */
		0,                         			/* tp_alloc */
		(newfunc)ID3_new,                 		/* tp_new */
};

bool check_ID3(PyObject *o) { return o->ob_type==&PyID3_Type; }

static const std::map<id3::ID3Version,std::string> id3versions = {
		{ id3::ID3Version::ID3v1 , "ID3v1" },
		{ id3::ID3Version::ID3v2 , "ID3v2" },
		{ id3::ID3Version::ID3v1v2 , "ID3v1_and_v2" }
};


void ID3Manager::prepare() {
	auto d=PyDict_New();
	for(auto it=id3::Metadata::id3str.begin();it!=id3::Metadata::id3str.end();it++) {
		PyDict_SetItemString(d,it->second.toCString(),PyLong_FromLong(static_cast<unsigned>(it->first)));
	}
	for(auto it=id3versions.begin();it!=id3versions.end();it++) {
			PyDict_SetItemString(d,it->second.c_str(),PyLong_FromLong(static_cast<unsigned>(it->first)));
	}
	Py_INCREF(d);
	PyID3_Type.tp_dict=d;
	PyID3_Type.tp_as_buffer=&PyID3_buffer;
}
bool ID3Manager::isReady() { return PyType_Ready(&PyID3_Type)>=0; }
void ID3Manager::inc() { Py_INCREF(&PyID3_Type); }
void ID3Manager::add(PyObject *m,const char *name) {
	PyModule_AddObject(m,name,(PyObject *)&PyID3_Type);
}
PyID3 * ID3Manager::make() {
	return (PyID3 *)_PyObject_New(&PyID3_Type);
}




