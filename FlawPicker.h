#pragma once

#include <QtWidgets/QMainWindow>
#include <qlabel.h>

#include <qtextedit.h>
#include "ui_FlawPicker.h"

#include <qtablewidget.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qiterator.h>
#include <qtimer.h>
#include <iostream>
#include <basic_func.h>
#include <camerathread.h>
#include <pymodule.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2/opencv.hpp> 
#include <boost\filesystem.hpp>



using namespace std;
using namespace  boost::filesystem;
//**************************************************************************************************//
extern QString resource_dir;
extern QString default_dir;
extern QString default_save_dir;

extern string default_path;
extern string default_save_path;

extern string standclimg_name;
extern string standimg_name;
extern string standimg_dir;

extern int Dimgh;
extern int Dimgw;

extern int CHANNEL;


//****************************************************************************************************//

class FlawPicker : public QMainWindow
{
	Q_OBJECT

public:
	explicit FlawPicker(QWidget *parent = Q_NULLPTR);

	void initialize();
	void connections();

	//img pross unit
	void standard_file_search();
	void standardImg_pros(bool save);
	void mask_maker();
	void colume_target();
	cv::Mat minus_Standard(const cv::Mat& img);
	cv::Mat basic_pros(const cv::Mat& img,int kernel_size=5);
	cv::Mat struct_pros(const cv::Mat& img_thr, int label = 1);
		
	vector<cv::Rect> contour_pros_single(const cv::Mat& img, bool structPro = true);
	vector<cv::Rect> RectFilter(vector<cv::Rect>& rois);
	bool  finalpush(const cv::Mat& Orimg, vector<cv::Rect>& rois, bool show = true,int pause=1000);
	bool  finalpush_F(const cv::Mat& Orimg, vector<cv::Rect>& rois, bool show = true, int pause = 1000);
	//batch pross
	void batch_pros();
	int batch_load();

	// save work
	void auto_save(QString img_name, bool Q);
	void saveCrops(const cv::Mat& res, int cnt);
signals:

public slots:
	
private slots:
	void on_Open_clicked();
	void on_Save_clicked();
	void on_SaveAs_clicked();

	void on_SettingT_clicked();
	void on_Reset_clicked();
	void on_ResetPath_clicked();
	void on_ResetSavePath_clicked();
	void on_ResetStandardPath_Clicked();

	void on_ToolT_clicked();
	void on_cleanQimg_clicked();
	void on_Enlarge_clicked();
	void on_Shrink_clicked();
	void on_Renew_clicked();
	void on_forbidAll_clicked();
	void roishow_state(int);
	void roishow_state_ac();
	void roisave_state(int);
	void roisave_state_ac();

	void on_labelingT_clicked();
	void filtermode_state();

	void on_singlePros_clicked();
	void on_batchPros_clicked();

	void camera_choose();
	void Updatedevices();
	void on_camera_clicked();
	void on_cameragrab_clicked();
	void on_startCamera_clicked();
	void on_stopCamera_clicked();
	void on_pauseCamera_clicked();
	void cameraautopros_state(int state);
	void cameraautograb_state(int state);
	void on_AutoSetting_clicked();
	void on_AutoGrabSetting_clicked();
	
	void on_report_clicked();
	void on_about_clicked();
private:
	Ui::FlawPickerClass ui;
	
	
	/*UI ENBAELED SETTING when running a camera*/
	inline void set_filework_group(bool en)
	{
		ui.actionsaveT->setEnabled(en);
		ui.actionsave->setEnabled(en);
		ui.actionopenT->setEnabled(en);
		ui.actionopen->setEnabled(en);
		ui.actionsaveasT->setEnabled(en);
		ui.actionsave_as->setEnabled(en);

	}
	inline void set_pros_group(bool en)
	{
		ui.actionimgprosT->setEnabled(en);
		ui.actionlabelingT->setEnabled(en);
		ui.actionanalyzeT->setEnabled(en);			
	}
	inline void set_Sidetool_bar(bool en)
	{
		ui.ToolBarSide->setEnabled(en);
		ui.settingToolbar->setEnabled(en);
		ui.actionreset->setEnabled(en);
		ui.actionsettingT->setEnabled(en);
		ui.actiontoolsT->setEnabled(en);
	}
	inline void set_group_box(bool en)
	{
		
		ui.devicecomboBox->setEnabled(en);
		ui.updatedevice->setEnabled(en);
		ui.devicelabel->setEnabled(en);
		ui.cameraautopros->setEnabled(en);
		ui.cameraautograb->setEnabled(en);
	}
	inline void external_packed_set(bool en)
	{
		set_filework_group(en);
		set_pros_group(en);
		set_Sidetool_bar(en);
	}
	/*flags&counts*/
	int pic_count,pass_count,ng_count;
	
	bool curr_done;
	bool filteropen;
	/*about paths*/
	char* origin_cwdir;
	string img_path;
	QString img_pathQ;

	string roi_path;
	QString roi_pathQ;
	QString last_save_path;
	QString last_open_path;
	
	string stand_dir;

	/*for filter*/
	cv::Point3i imgsize;
	double max_area, min_area;
	int min_dist;
	CvSize finalsize;

	inline void imgsizei(const cv::Mat& img)
	{
		imgsize.x = img.cols;
		imgsize.y = img.rows;
		imgsize.z = imgsize.x*imgsize.y;
	};
	inline void area_rangei(const cv::Point3i& imgsiz)
	{
		int short_edge = min(imgsiz.x, imgsiz.y);
		min_area = int(short_edge / 100),
		max_area = int(short_edge / 15);
	};
	inline void finalsizei(int maxarea)
	{
		int edg = int(maxarea*0.8);
		finalsize = CvSize(edg*1.2, edg*1.2);
		min_dist = edg*4;
	};
	inline void update_filterdata(const cv::Mat& img)
	{
		imgsizei(img);
		area_rangei(imgsize);
		finalsizei(max_area);

	}

	/*process&display*/
	QImage imgQ;
	QImage bkgd,ng,pass,nosig;
	cv::Mat imgM;
	cv::Mat img_curr;
	int img_channel;
	QString displaying_path;	
	vector<cv::Mat> batch_img;	
	vector<cv::Rect> newrr;
	vector<cv::Mat> roi_temper;
	QEventLoop eventloop;
	QPalette pe;

	cv::Mat StructelementA, StructelementB;
	cv::Mat standard_mean;
	cv::Mat maskcenter, maskcenterb, maskcenterb_r;
	cv::Point maskcenterPt, mask_earPt1, mask_earPt2;
	int mask_radius, mask_earl, mask_earh;	
	cv::Mat standard_colume;
	cv::Rect colume_locate;
	inline bool checkPath(const string &strFilePath)
	{			
		if (!exists(strFilePath))
		{
			return false;
		}
		return true;
	}
	inline cv::Mat basic_prepare(const cv::Mat& img,int blurSize=5)
	{
		if (blurSize > 0)
		{
			blur(img, img, cv::Size(blurSize, blurSize));		}
	
		Sobel(img, img, CV_8U, 1, 0, 3, 1, 0);
		threshold(img,img, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
		return img;
	}
	inline cv::Mat grayCheck(const cv::Mat& img)
	{
		cv::Mat img_m=img;
		if (img.channels() != 1)
		{
			if (img.channels() == 4)
			{
				cvtColor(img_m , img_m, CV_RGBA2RGB);
			}
			cvtColor(img_m, img_m, CV_RGB2GRAY);
		}
		
		return  img_m;
	}	
	inline bool contour_filter(double area, int distance1, int distance2 = -1)
	{
		if (area < min_area || area > max_area)
		{
			return false;
		}
		if (distance2 == -1)
		{
			if (distance1 > mask_radius)
			{
				return false;
			}

		}
		else if (distance2 != -1)
		{
			if (distance1>mask_earh&&distance2>mask_earh)
			{
				return false;
			}
			if (distance1<mask_earl || distance2<mask_earl)
			{
				return false;
			}
		}
		return true;
	}
	inline int distanceCalc(cv::Point pt1, cv::Point pt2)
	{
		return  sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));
	}
	inline int distanceCalc(const cv::Rect& R1, const cv::Rect& R2)
	{
		return  sqrt(
			(R1.x + R1.width*0.5 - R2.x - R2.width*0.5)*(R1.x + R1.width*0.5 - R2.x - R2.width*0.5) +
			(R1.y + R1.height*0.5 - R2.y - R2.height*0.5)*(R1.y + R1.height*0.5 - R2.y - R2.height*0.5)
		);
	}
	inline cv::Rect trans2finalsize(const cv::Rect& orRect)
	{
		int newx = max(int(orRect.x + orRect.width*0.5 - 0.5*finalsize.width), 0);
		int newy = max(int(orRect.y + orRect.height*0.5 - 0.5*finalsize.height), 0);
		newx = min(newx, imgsize.x - finalsize.width);
		newy = min(newy, imgsize.y - finalsize.height);

		return cv::Rect(newx, newy, finalsize.width, finalsize.height);
	}	

	inline void displayMatResult(const cv::Mat& Orimg,int pause=1000)
	{
		imgQ = cvMat2QImage(Orimg);
		showsignal(pause);
	};	
	inline void painting(const cv::Mat& Orimg, vector<cv::Rect>& rois)
	{
		imgQ = cvMat2QImage(Orimg);
		if (rois.empty())
		{
			showsignal(0);
			return;
		}		

		QPainter painter(&imgQ);
		painter.setPen(QPen(QColor(255, 130, 130), 4, Qt::SolidLine));

		vector<cv::Rect>::iterator it = rois.begin();
		while (it != rois.end())
		{
			painter.drawRect(it->x, it->y, it->width, it->height);
			++it;
		}

		showsignal(0);

	}	
	inline void showsignal(int pause = 1000)
	{
		ui.picshow->clear();
		ui.picshow->setPixmap(QPixmap::fromImage(imgQ));
		ui.picshow->show();

		if (pause > 0)
		{			
			QTimer::singleShot(pause, &eventloop, SLOT(quit()));
			eventloop.exec();
		}
	}
	inline void showresult(int cnt)
	{
		ui.result->clear();
		
		if (cnt >0)
		{
			if (ng.isNull())
			{
				ng.load(resource_dir+"ng.png");
			}
			ui.result->setPixmap(QPixmap::fromImage(ng));
		}
		else if (cnt == 0)
		{
			if (pass.isNull())
			{
				pass.load(resource_dir+"Pass.png");
			}
			ui.result->setPixmap(QPixmap::fromImage(pass));
		}
		else
		{
			ui.result->setPixmap(QPixmap::fromImage(nosig));
		}
	}
	inline void showresult(bool res)
	{
		if (!res)
		{
			if (ng.isNull())
			{
				ng.load(resource_dir + "ng.png");
			}
			ui.result->setPixmap(QPixmap::fromImage(ng));
		}
		else 
		{
			if (pass.isNull())
			{
				pass.load(resource_dir + "Pass.png");
			}
			ui.result->setPixmap(QPixmap::fromImage(pass));
		}
	
	}
	inline void show_rois_labels(bool res,int cnt)
	{
		QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(cnt + 1));
		QLabel *crt = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabelt_%1").arg(cnt + 1));
		cr->setPixmap(QPixmap::fromImage(cvMat2QImage(roi_temper[cnt])));
		if (res)
		{	
			pe.setColor(QPalette::WindowText, Qt::red);
			crt->setPalette(pe);
			crt->setText("NG");
			return;
		}	
		pe.setColor(QPalette::WindowText, Qt::white);
		crt->setPalette(pe);
		crt->setText("Pass");				
	}
	//clean job
	inline void clearRoisAndLabels()
	{	
		for (int i = 0;i < 11;i++)
		{
			QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(i+1));
			cr->clear();			
			QLabel *crt = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabelt_%1").arg(i+1));
			crt->clear();				
		}
	}
	inline void all_clean_up()
	{
		clearRoisAndLabels();
		img_curr.release();		
		showresult(-1);
		ui.picshow->clear();
		ui.picshow->setPixmap(QPixmap::fromImage(bkgd));
		curr_done = false;
		displaying_path.clear();
		batch_img.clear();
		roi_temper.clear();
		pic_count = 0;
	}

	/*camera works*/
	CameraThread camera_t;
	int total_cnt, pass_cnt, ng_cnt;
	bool startedcam;
	bool autopros;
	bool autosetted;
	bool autograb;
	bool autograbsetted;
	QString autograbpath;
	int autopause;
	int autopausegrab;
	int autograbcount;
	QEventLoop eventloopC;
	inline void cameraParm_init()
	{
		total_cnt = pass_cnt = ng_cnt = 0;		
		autosetted = false;
		autograbsetted = false;
		autopause = 1000;
		autopausegrab = 3000;
		autograbcount = 0;
		autograbpath = default_save_dir;
	}
	inline int camera_mode_check()
	{
		{
			autopros = ui.actioncameraauto->isChecked();
			autograb = ui.actionautograb->isChecked();
			if (autopros)
			{
				if (!autosetted)
				{
					on_AutoSetting_clicked();
				}
				autosetted = true;				
				return 2;
			}
			if (autograb)
			{
				if (!autograbsetted)
				{
					on_AutoGrabSetting_clicked();
				}
				autograbsetted = true;
				chdir((autograbpath.toStdString()).c_str());
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	inline void saveGrabs()
	{
		char* s = new char;
		sprintf(s, "%d.bmp", autograbcount);	
		imwrite(s, img_curr);
		autograbcount++;
		s = NULL;		
		delete s;
		ui.FlawNumber->display(QString("%1:").arg(autograbcount) +"0");
		
	}
	/*module works*/
	pymodule ModelFilter;
	inline bool resFilter()
	{
		bool resF= ModelFilter.run_filter(roi_temper, finalsize);
		if (resF)
		{
			roi_temper.clear();
			newrr.clear();
			return resF;
		}
		vector<cv::Mat> F_rois;
		vector<cv::Rect> F_rects;
		for (int i = 0; i < ModelFilter.result.size(); i++)
		{
			F_rois.push_back(roi_temper[ModelFilter.result[i]]);
			F_rects.push_back(newrr[ModelFilter.result[i]]);
		}
		
		roi_temper.clear();
		roi_temper.insert(roi_temper.begin(), F_rois.begin(), F_rois.end());
		newrr.clear();
		newrr.insert(newrr.begin(), F_rects.begin(), F_rects.end());

		return resF;
	}
	inline void filterlable()
	{
		clearRoisAndLabels();

		bool res=ModelFilter.run_filter(roi_temper, finalsize);
		vector<int>::iterator it;
		for (int i = 0; i < roi_temper.size(); i++)
		{		
			it = std::find(ModelFilter.result.begin(), ModelFilter.result.end(), i);
			show_rois_labels((it!= ModelFilter.result.end()), i);
		}
		ui.FlawNumber->display(QString("%1:").arg(pic_count) + QString("%1").arg(ModelFilter.result.size()));
		showresult(res);
	}
	/*analyze report*/
	QTableWidget* table;
	QTableWidgetItem *total_c, *pass_c, *ng_c;
	QTableWidgetItem *mode, *frequency, *pathsave,*saved;
	inline void stateUpdate()
	{		
		if (autopros)
		{
			if (filteropen)
			{
				mode->setText("AUTOPROS(filter: ON)");
			}
			else
			{
				mode->setText("AUTOPROS(filter: OFF)");
			}
			
			frequency->setText(QString("%1 ms/pic").arg(autopause));
			pathsave->setText(QString::fromStdString(roi_path));
			if (ui.actionautosave->isChecked())
			{
				saved->setText("Y");
			}
			else
			{
				saved->setText("N");
			}
		}
		else if(autograb)
		{
			mode->setText("AUTOGRAB");
			frequency->setText(QString("%1 ms/pic").arg(autopausegrab));
			saved->setText("Y");
			pathsave->setText(QString::fromStdString(autograbpath.toStdString()));
		}
		else
		{
			mode->setText("STATIC_FREE");
			frequency->setText("null");
			saved->setText("null");
			pathsave->setText(QString::fromStdString(roi_path));
		}
		
		
	}
	inline void countUpdate()
	{		
		if (autopros)
		{
			total_c->setText(QString("%1").arg(total_cnt));
			pass_c->setText(QString("%1").arg(pass_cnt));
			ng_c->setText(QString("%1").arg(ng_cnt));
		}
		else if (autograb)
		{
			total_c->setText(QString("%1").arg(autograbcount));
			pass_c->setText("null");
			ng_c->setText("null");
		}
		else
		{
			total_c->setText(QString("%1").arg(pic_count));
			pass_c->setText(QString("%1").arg(pass_count));
			ng_c->setText(QString("%1").arg(ng_count));
		}

	}
	inline void tableunitInit()
	{
		table = new QTableWidget(7, 1);		
		QStringList headerLabels;
		headerLabels << "Value/Count";
		table->setHorizontalHeaderLabels(headerLabels);
		table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

		QStringList rowLabels;
		rowLabels << "Total" << "Passed" << "NG" << "Process Frequency" << "Process Mode" << "Result Saved" << "Save path";
		table->setVerticalHeaderLabels(rowLabels);

		mode = new QTableWidgetItem();
		frequency = new QTableWidgetItem();
		pathsave = new QTableWidgetItem();
		saved = new QTableWidgetItem();
		total_c = new QTableWidgetItem();
		pass_c = new QTableWidgetItem();
		ng_c =new QTableWidgetItem();
		stateUpdate();
		countUpdate();
	}
	inline void table_update()
	{						
		stateUpdate();
		countUpdate();

		table->setItem(0, 0, total_c);
		table->setItem(1, 0, pass_c);
		table->setItem(2, 0, ng_c);
		table->setItem(3, 0, mode);
		table->setItem(4, 0, frequency);
		table->setItem(5, 0, saved);
		table->setItem(6, 0, pathsave);

		table->show();
	}
	/**/
};
