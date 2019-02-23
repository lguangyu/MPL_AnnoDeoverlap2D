#include <vector>
#include <Python.h>
#define NPY_NO_DEPRECATED_API 14 /* suppres annoying warnings */
#include <numpy/ndarrayobject.h>
#include "clip_box.h"
#include "ode_deoverlap.h"


void _ndarray2d_to_flat_vector(PyArrayObject* src, std::vector< double >& dest)
{
	npy_intp size = PyArray_SIZE(src);
	double* data = reinterpret_cast< double* >(PyArray_DATA(src));
	dest.resize(size);
	for (std::vector< double >::iterator i = dest.begin();
		i != dest.end(); i++)
		(*i) = *(data++);
	return;
}


void _ndarray1d_to_cbox(PyArrayObject* src, ClipBox& dest)
{
	double* data = reinterpret_cast< double* >(PyArray_DATA(src));
	dest.x_min = data[0];
	dest.x_max = data[1];
	dest.y_min = data[2];
	dest.y_max = data[3];
	return;
}


void _update_ndarray2d_from_vector(const std::vector< double >& src, PyArrayObject* dest)
{
	double* data = reinterpret_cast< double* >(PyArray_DATA(dest));
	for (std::vector< double >::const_iterator i = src.begin();
		i != src.end(); i++)
		*(data++) = (*i);
	return;
}


PyObject* _raise(PyObject* except_t, const char* _msg)
{
	PyErr_SetString(except_t, _msg);
	return NULL;
}


PyObject* wrap_ode_deoverlap(PyObject* self, PyObject* args, PyObject* kwargs)
{
	PyArrayObject* t_xys_arr = nullptr;
	PyArrayObject* t_whs_arr = nullptr;
	PyArrayObject* a_xys_arr = nullptr;
	PyArrayObject* cbox_arr = nullptr;
	double t_repel;
	double a_repel;
	double a_attract;
	static char* kwlist[] = {"text_xys", "text_dims", "anno_xys", "clip_box",
		"text_repel", "anno_repel", "anno_attract", NULL};
	static char* _arg_format = "OOOOddd|";
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, _arg_format, kwlist,
		reinterpret_cast< PyObject** >(&t_xys_arr),
		reinterpret_cast< PyObject** >(&t_whs_arr),
		reinterpret_cast< PyObject** >(&a_xys_arr),
		reinterpret_cast< PyObject** >(&cbox_arr),
		&t_repel,
		&a_repel,
		&a_attract))
		return NULL;
	/* check instance type (PyArray_Type) before using numpy C API */
	if (!PyObject_TypeCheck(reinterpret_cast< PyObject* >(t_xys_arr),
		&PyArray_Type))
		return _raise(PyExc_TypeError,
			"'text_xys' must be of type 'ndarray' with non-zero length");
	if (!PyObject_TypeCheck(reinterpret_cast< PyObject* >(t_whs_arr),
		&PyArray_Type))
		return _raise(PyExc_TypeError,
			"'text_dims' must be of type 'ndarray' with non-zero length");
	if (!PyObject_TypeCheck(reinterpret_cast< PyObject* >(a_xys_arr),
		&PyArray_Type))
		return _raise(PyExc_TypeError,
			"'anno_xys' must be of type 'ndarray' with non-zero length");
	if (!PyObject_TypeCheck(reinterpret_cast< PyObject* >(cbox_arr),
		&PyArray_Type))
		return _raise(PyExc_TypeError,
			"'clip_box' must be of type 'ndarray' with non-zero length");
	/* check shape of arrays */
	if ((PyArray_TYPE(t_xys_arr) != NPY_FLOAT64) ||
		(PyArray_TYPE(t_whs_arr) != NPY_FLOAT64) ||
		(PyArray_TYPE(a_xys_arr) != NPY_FLOAT64) ||
		(PyArray_NDIM(t_xys_arr) != 2) ||
		(PyArray_NDIM(t_whs_arr) != 2) ||
		(PyArray_NDIM(a_xys_arr) != 2) ||
		(PyArray_DIM(t_xys_arr, 0) != PyArray_DIM(t_whs_arr, 0)) ||
		(PyArray_DIM(a_xys_arr, 0) != PyArray_DIM(t_whs_arr, 0)) ||
		(PyArray_DIM(t_xys_arr, 1) != 2) ||
		(PyArray_DIM(t_xys_arr, 1) != PyArray_DIM(t_whs_arr, 1)) ||
		(PyArray_DIM(a_xys_arr, 1) != PyArray_DIM(t_whs_arr, 1)))
		return _raise(PyExc_ValueError,
			"'text_xys', 'text_dims' and 'anno_xys' must be <float64> array with identical shape (N, 2)");
	if ((PyArray_NDIM(cbox_arr) != 1) ||
		(PyArray_DIM(cbox_arr, 0) != 4))
		return _raise(PyExc_ValueError,
			"'clip_box', must have shape (4, )");
	/* parse values into vectors */
	std::vector< double > t_xys, t_whs, a_xys;
	ClipBox cbox;
	_ndarray2d_to_flat_vector(t_xys_arr, t_xys);
	_ndarray2d_to_flat_vector(t_whs_arr, t_whs);
	_ndarray2d_to_flat_vector(a_xys_arr, a_xys);
	_ndarray1d_to_cbox(cbox_arr, cbox);
	/* solve with ode */
	ode_deoverlap(t_xys, t_whs, a_xys, cbox,
		t_repel, a_repel, a_attract);
	/* update to pos_arr */
	_update_ndarray2d_from_vector(t_xys, t_xys_arr);
	Py_RETURN_NONE;
}


/* python module interface */
static PyMethodDef __module_meth[] = {
	{"ode_deoverlap", (PyCFunction) wrap_ode_deoverlap,
		METH_VARARGS | METH_KEYWORDS, "deoverlap texts with ODE"},
	{NULL, NULL, 0, NULL},
};

static struct PyModuleDef __module_def = {
	PyModuleDef_HEAD_INIT,
	"deoverlap C/C++ routines", /* module name */
	NULL, /* module doc string, NULL safe */
	-1,
	__module_meth,
};

extern "C"
{

PyMODINIT_FUNC PyInit__deoverlap_croutine(void)
{
	import_array();
	return PyModule_Create(&__module_def);
}

}; /* end of extern "C" */
