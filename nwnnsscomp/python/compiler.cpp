#include <Python.h>

#include "../../_NscLib/Nsc.h"
#include "../../_NwnLib/NwnStdLoader.h"

CNwnStdLoader loader;
int version = 999999;

static PyObject *compiler_init(PyObject *self, PyObject *args) {
    const char *nwndir;
    if(!PyArg_ParseTuple(args,"s",&nwndir)) {
        return NULL;
    }
    if(!loader.Initialize(nwndir)) {
        PyErr_SetString(PyExc_IOError,"NWN dir not found in compiler module");
        return NULL;
    }
    if(!NscCompilerInitialize(&loader,version,true)) {
        PyErr_SetString(PyExc_SystemError, "NWN compiler failed to initialize");
        return NULL;
    }
    loader.AddModuleHaks();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *compiler_free(PyObject *self, PyObject *args) {
    loader.Close();
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *compiler_compile(PyObject *self, PyObject *args) {
    CNwnMemoryStream out;
    CNwnMemoryStream dbg;
    
    unsigned char *data;
    int dataLength = 0;
    const char *name;
    bool optimizeFlag = true;
    
    if(!PyArg_ParseTuple(args,"s#si",&data,&dataLength,&name,&optimizeFlag)) {
        return NULL;
    }
    NscResult result = NscCompileScript(&loader,name,data,dataLength,false,version,
                                        optimizeFlag,false,&out,&dbg);
    if(result == NscResult_Failure) {
        PyErr_SetString(PyExc_SystemError, "script compilation failed");
        return NULL;
    } else if(result == NscResult_Include) {
        PyErr_Warn(PyExc_UserWarning, "script is an include file, not compiled");
        Py_INCREF(Py_None);
        return Py_None;
    }
    return Py_BuildValue("s#s#",
                         out.GetData(),out.GetPosition(),
                         dbg.GetData(),dbg.GetPosition());
}

static PyMethodDef Methods[] = {
    {"init", compiler_init, METH_VARARGS,
     "initialize compiler module"},
    {"compile", compiler_compile, METH_VARARGS,
     "compile an nss buffer"},
    {"free", compiler_free, METH_VARARGS,
     "free any resources associated with the nss compiler module"},
    {NULL,NULL,0,NULL}
};

extern "C" {
    PyMODINIT_FUNC initnsscompiler(void) {
        (void) Py_InitModule("nsscompiler",Methods);
    }
}
