#include "total.h"
#include <Python.h>
#include <vector>

using namespace std;

/*
 * Implements an example function.
 */
PyDoc_STRVAR(get_notes_example_doc, "wrap_get_notes(obj)\
\
Example function");

// ��ʼ��FFT�ı���
PyObject* wrap_setup_variables(PyObject* self, PyObject* train_data_all) {
	if (PyTuple_Size(train_data_all) == 0) {
		printf("No input parameters. only set parameters.\n");
		setup_variables();
	}
	else {
		//��ȡ���ݼ���trade data�Ľṹ����������BA��Ӧ��������Ƶ�ʷ�ֵ�ṹ���Լ�������train data��ÿһ����±�����ʵƵ��/Ƶ�ʱ�����������������
		PyObject* train_data_ori[6];
		int train_data_len;
		double sample_ratio;
		if (!PyArg_ParseTuple(train_data_all, "OOOOOOid", &train_data_ori[0], &train_data_ori[1], &train_data_ori[2], &train_data_ori[3], &train_data_ori[4], &train_data_ori[5], &train_data_len, &sample_ratio)) {
			PyErr_SetString(PyExc_TypeError, "input list is not correct");
			return NULL;
		}
		vector<double> train_data[6];
		for (int t = 0; t <= 5; t++) {
			train_data[t].reserve(train_data_len);
			for (int t0 = 0; t0 <= train_data_len - 1; t0++) {
				PyObject* tmp = PyList_GetItem(train_data_ori[t], t0);
				double value = PyFloat_AsDouble(tmp);
				train_data[t].push_back(value);
			}
		}
		//���ݻ�ȡ��������������ÿ��ѵ�������ķ�ֵ
		setup_variables();
		setup_train_data_peaks(train_data, sample_ratio);
	}
	Py_RETURN_NONE;
}

// �����������Ƶ��ȷ����������������output_type��ʾ��������ͣ�0Ϊֱ�����FFTת�������1Ϊ���ÿ��������Ӧ��Ƶ�׷�ֵ��2Ϊ�������ȷ�Ͻ����
PyObject *wrap_get_notes(PyObject *self, PyObject *wave0) {
	// 
	vector<int> wave_input;
	wave_input.reserve(4096);
	// ��ȡ�������Ƶ�ź�
	PyObject* wave1;
	int output_type_value;
	if (!PyArg_ParseTuple(wave0, "Oi", &wave1, &output_type_value)) {
		PyErr_SetString(PyExc_TypeError, "Input wave must be a list");
		return NULL;
	}
	int wave_len = PyList_Size(wave1);
	if (wave_len != 2048 && wave_len != 4096) {
		PyErr_SetString(PyExc_ValueError, "Input wave must have length of 2048 or 4096");
		return NULL;
	}
	for (int t = 0; t <= wave_len - 1; t++) {
		PyObject* tmp = PyList_GetItem(wave1, t);
		int value = PyLong_AsLong(tmp);
		wave_input.push_back(value);
	}
	// ���������Ƶ�źż���FFT
	vector<double> freq_data = fft(wave_input, wave_len);
	// �����������������FFT������д������
	if (output_type_value == 0) {
		// ֱ�����FFTת�����
		PyObject* res_data = PyList_New(0);
		for (int t = 0; t <= wave_len / 2 - 1; t++) {
			PyList_Append(res_data, PyFloat_FromDouble(freq_data[t]));
		}
		return res_data;
	}
	else if (output_type_value == 1) {
		// ������ÿ��������Ӧ��Ƶ�׷�ֵ
		vector<double> freq_data_by_note = get_freq_data_by_note(freq_data, wave_len);
		PyObject* res_data = PyList_New(0);
		for (int t = 0; t <= 48; t++) {
			PyList_Append(res_data, PyFloat_FromDouble(freq_data_by_note[t]));
		}
		return res_data;
	}
	else if (output_type_value == 2) {
		// �������ȷ�Ͻ��
		double sample_ratio = SAMPLE_RATE * 1.0 / wave_len;
		vector<int> note_indexes = get_note_indexes(freq_data, sample_ratio);
		PyObject* res_data = PyList_New(0);
		if (note_indexes.size() >= 1) {
			for (int t = 0; t < note_indexes.size(); t++) {
				PyList_Append(res_data, PyLong_FromLong(note_indexes[t]));
			}
		}
		return res_data;
	}
	else {
		PyErr_SetString(PyExc_ValueError, "output type must be 0, 1 or 2");
		return NULL;
	}
}

/*
 * List of functions to add to get_notes in exec_get_notes().
 */
static PyMethodDef get_notes_functions[] = {
	{ "setup_vars", (PyCFunction)wrap_setup_variables, METH_VARARGS | METH_KEYWORDS, get_notes_example_doc },
	{ "parse_note", (PyCFunction)wrap_get_notes, METH_VARARGS | METH_KEYWORDS, get_notes_example_doc },
    { NULL, NULL, 0, NULL } /* marks end of array */
};

/*
 * Initialize get_notes. May be called multiple times, so avoid
 * using static state.
 */
int exec_get_notes(PyObject *module) {
    PyModule_AddFunctions(module, get_notes_functions);

    PyModule_AddStringConstant(module, "__author__", "WHL");
    PyModule_AddStringConstant(module, "__version__", "1.0.0");
    PyModule_AddIntConstant(module, "year", 2021);

    return 0; /* success */
}

/*
 * Documentation for get_notes.
 */
PyDoc_STRVAR(get_notes_doc, "The get_notes module");


static PyModuleDef_Slot get_notes_slots[] = {
    { Py_mod_exec, exec_get_notes },
    { 0, NULL }
};

static PyModuleDef get_notes_def = {
    PyModuleDef_HEAD_INIT,
    "get_notes",
	get_notes_doc,
    0,              /* m_size */
    NULL,           /* m_methods */
	get_notes_slots,
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_get_notes() {
    return PyModuleDef_Init(&get_notes_def);
}
