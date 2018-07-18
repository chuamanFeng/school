#pragma once

#include <iostream>
#include <sstream>
#include <qimage.h>
#include "cv.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>  
#include <direct.h>



using namespace std;

cv::Mat QImage2cvMat(QImage image);
QImage cvMat2QImage(const cv::Mat& mat);
cv::Mat IplImage2Mat(IplImage* img);
IplImage* Mat2IplImage(cv::Mat img);
QString char2qstring(char* name);
char* qstring2char(QString str);
string pathPros(string pathr);
char*  getcwd();
wchar_t* widen(const string& str);
string narrow(const wstring& str);
wchar_t* wstr2wchar(const wstring& wstr);

extern int Dimgh;
extern int Dimgw;