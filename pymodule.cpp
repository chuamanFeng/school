#include <pymodule.h>
#include <qstring.h>

pymodule::pymodule()
{		
	Tinitialize();
}
void pymodule::Tinitialize()
{
	Py_SetPythonHome(L"python35");
	Py_Initialize();
	init_numpy();
	if (!Py_IsInitialized())
	{
		return;
	}
	PyEval_InitThreads();
	if (module_loader()==0)
	{
		QMessageBox::critical(NULL, "ERROR", "Filter Function initialize Failed");
		Py_Finalize();
		return;
	}
	clear_all();

}
bool pymodule::run_filter(std::vector<cv::Mat>& img_v,CvSize sz)
{
	result.clear();
	if (!img_v.empty())
	{
		if (sz.height == 0 || sz.width == 0)
		{
			sz.height =img_v[0].rows;
			sz.width = img_v[0].cols;
		}
		
		gstate = PyGILState_Ensure();
		pFunc = PyObject_GetAttrString(_module, model_sumon);
		for (int i = 0; i < img_v.size(); i++)
		{						
			result_py = PyEval_CallObject(pFunc, mat2py(img_v[i],sz));
			PyArg_Parse(result_py, "i", &res_c); 
			if (res_c == 1)
			{
				result.push_back(i);
			}
		}
		PyGILState_Release(gstate);
		if (!result.empty())
		{
			return false;
		}
	}
	return true;
}
pymodule::~pymodule()
{	
	
	Py_Finalize();
}