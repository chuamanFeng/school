#pragma once

#include <qmessagebox.h>
#include <Python.h>
#include <cv.h>
#include<numpy\core\include\numpy\arrayobject.h>
#include <QThread>
#include <basic_func.h>

using namespace std;

extern const char* modelpyname;
extern const char* model_sumon;

class pymodule : public QThread
{	
public:
	pymodule();
	void Tinitialize();
	~pymodule();


	bool run_filter(vector<cv::Mat>&imgs,CvSize sz=CvSize(0,0));

	PyObject*pFunc = NULL;
	PyObject*pArg = NULL;
	PyObject* _module = NULL;
	PyObject* result_py = NULL;
	int res_c = -1;

	PyGILState_STATE gstate;
	std::vector<int> result;

private:
	inline int init_numpy()
	{
		import_array();
	}
	inline int module_loader()
	{						
		_module = PyImport_ImportModule(modelpyname);			
		if (_module==NULL)
		{
			return 0;
		}
	    return  1;
	}
	inline void clear_all()
	{
		PyObject*pFunc = NULL;
		PyObject*pArg = NULL;
		PyObject* result_py = NULL;
		res_c = -1;
	}
	
	inline PyObject* mat2py(const cv::Mat &img_m,CvSize sz)
	{	
		cv::Mat img = graytr(img_m);			
		int p = 0;				
		unsigned char*data = (unsigned char*)malloc(sizeof(uchar) * sz.width * sz.height);
		for (int i = 0; i< sz.height; i++)
		{
			for (int j = 0; j < sz.width; j++)
			{		
				data[p] = img.at<uchar>(j, i);				
				p++;
			}
		}
		npy_intp Dims[2] = { sz.height ,sz.width }; 
		PyObject*PyArray = PyArray_SimpleNewFromData(2, Dims, NPY_UBYTE ,data);
		
		PyObject*ArgArray = PyTuple_New(1);
		PyTuple_SetItem(ArgArray, 0, PyArray);
	
		return ArgArray;
	}
	inline cv::Mat graytr(const cv::Mat &img_m)
	{
		cv::Mat img = img_m;
		if (img.channels() != 1)
		{
			if (img.channels() == 4)
			{
				cvtColor(img, img, CV_RGBA2RGB);
			}
			cvtColor(img, img, CV_BGR2GRAY);
		}		
		return img;
	}

};

