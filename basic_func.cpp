
#include "basic_func.h"

using namespace cv;

Mat QImage2cvMat(QImage image)
{
	Mat mat;

	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

QImage cvMat2QImage(const Mat& mat)
{

	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);

		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}

		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}

	else if (mat.type() == CV_8UC3)
	{

		const uchar *pSrc = (const uchar*)mat.data;

		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{

		const uchar *pSrc = (const uchar*)mat.data;

		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{

		return QImage();
	}
}
Mat IplImage2Mat(IplImage* img)
{
	Mat Img;
	if (img != NULL)
	{
		Img = cvarrToMat(img);
	}
	else
	{
		Img = Mat::zeros(Dimgw, Dimgh, CV_8UC1);
	}
	return Img;

}
IplImage* Mat2IplImage(Mat img)
{

	IplImage* Img = &IplImage(img);

	return Img;

}
QString char2qstring(char* name)
{
	QString nn = QString(QLatin1String(name));
	return nn;
}

char* qstring2char(QString str)
{

	QByteArray ba = str.toLatin1();
	char *mm = ba.data();
	return mm;
}
string pathPros(string pathr)
{
	string::size_type pos = 0;


	while ((pos = pathr.find_first_of('\\', pos)) != string::npos)
	{
		pathr.insert(pos, "\\");
		pos = pos + 2;
	}
	return pathr;
}

char*  getcwd()
{
	char* buffer;
	buffer = _getcwd(NULL, 0);
	return buffer;
}
wchar_t* widen(const string& str)
{
	wostringstream wstm;
	const ctype<wchar_t>& ctfacet =
		use_facet< ctype<wchar_t> >(wstm.getloc());
	for (size_t i = 0; i<str.size(); ++i)
		wstm << ctfacet.widen(str[i]);
	
	return wstr2wchar(wstm.str());
}
wchar_t* wstr2wchar(const wstring& wstr)
{
	wchar_t * wcstr = (wchar_t *)wstr.c_str();
	return wcstr;
}
string narrow(const wstring& str)
{
	ostringstream stm;
	const ctype<char>& ctfacet =
		use_facet< ctype<char> >(stm.getloc());
	for (size_t i = 0; i<str.size(); ++i)
		stm << ctfacet.narrow(str[i], 0);
	return stm.str();
}
