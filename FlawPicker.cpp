#include "FlawPicker.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qinputdialog.h>


using namespace cv;

FlawPicker::FlawPicker(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);	

	ui.groupBox->setEnabled(false);
	ui.actioncameraauto->setEnabled(false);
	ui.actionautograb->setEnabled(false);

	this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint);	
	bkgd=ui.picshow->pixmap()->toImage();	
	nosig = ui.result->pixmap()->toImage();

	initialize();
	connections();	
}
void FlawPicker::connections()
{
	connect(ui.actionopen, SIGNAL(triggered()), this, SLOT(on_Open_clicked()));
	connect(ui.actionopenT, SIGNAL(triggered()), this, SLOT(on_Open_clicked()));

	connect(ui.actionsave_as, SIGNAL(triggered()), this, SLOT(on_SaveAs_clicked()));
	connect(ui.actionsaveasT, SIGNAL(triggered()), this, SLOT(on_SaveAs_clicked()));

	connect(ui.actionsave, SIGNAL(triggered()), this, SLOT(on_Save_clicked()));
	connect(ui.actionsaveT, SIGNAL(triggered()), this, SLOT(on_Save_clicked()));

	connect(ui.actionsettingT, SIGNAL(triggered()), this, SLOT(on_SettingT_clicked()));
	connect(ui.actionreset, SIGNAL(triggered()), this, SLOT(on_Reset_clicked()));
	connect(ui.actionresetT2, SIGNAL(triggered()), this, SLOT(on_Reset_clicked()));
	 
	connect(ui.actionPathSetting, SIGNAL(triggered()), this, SLOT(on_ResetPath_clicked()));
	connect(ui.actionopenpath, SIGNAL(triggered()), this, SLOT(on_ResetPath_clicked()));
	connect(ui.actionSavePath, SIGNAL(triggered()), this, SLOT(on_ResetSavePath_clicked()));
	connect(ui.actionsavepath, SIGNAL(triggered()), this, SLOT(on_ResetSavePath_clicked()));
	connect(ui.actionStandardPath, SIGNAL(triggered()), this, SLOT(on_ResetStandardPath_Clicked()));
	connect(ui.actionstandardpath, SIGNAL(triggered()), this, SLOT(on_ResetStandardPath_Clicked()));

	connect(ui.actiontoolsT, SIGNAL(triggered()), this, SLOT(on_ToolT_clicked()));
	connect(ui.actionclearImg, SIGNAL(triggered()), this, SLOT(on_cleanQimg_clicked()));
	connect(ui.actionenlarge, SIGNAL(triggered()), this, SLOT(on_Enlarge_clicked()));
	connect(ui.actionshrink, SIGNAL(triggered()), this, SLOT(on_Shrink_clicked()));
	connect(ui.actionrenew, SIGNAL(triggered()), this, SLOT(on_Renew_clicked()));

	connect(ui.checkboxshowroi, SIGNAL(stateChanged(int)), this, SLOT(roishow_state(int)));
	connect(ui.actionshowroi, SIGNAL(triggered()), this, SLOT(roishow_state_ac()));
	connect(ui.actionsaverois, SIGNAL(triggered()), this, SLOT(roisave_state_ac()));
	connect(ui.checkboxsaveroi, SIGNAL(stateChanged(int)), this, SLOT(roisave_state(int)));

	connect(ui.actionlabelingT, SIGNAL(triggered()), this, SLOT(on_labelingT_clicked()));
	connect(ui.actionlabelfilter, SIGNAL(triggered()), this, SLOT(filtermode_state()));

	connect(ui.actionstopAll, SIGNAL(triggered()), this, SLOT(on_forbidAll_clicked()));

	connect(ui.actionimgprosT, SIGNAL(triggered()), this, SLOT(on_singlePros_clicked()));
	connect(ui.actionanalyzeT, SIGNAL(triggered()), this, SLOT(on_batchPros_clicked()));

	connect(ui.devicecomboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_camera_clicked(camera_choose())));
	connect(ui.updatedevice, SIGNAL(currentIndexChanged(int)), this, SLOT(on_camera_clicked(Updatedevices())));
	connect(ui.actioncameraT, SIGNAL(triggered()), this, SLOT(on_camera_clicked()));
	connect(ui.actiongrabT, SIGNAL(triggered()), this, SLOT(on_cameragrab_clicked()));
	connect(ui.start, SIGNAL(clicked()), this, SLOT(on_startCamera_clicked()));
	connect(ui.stopcamera, SIGNAL(clicked()), this, SLOT(on_stopCamera_clicked()));
	connect(ui.pause, SIGNAL(clicked()), this, SLOT(on_pauseCamera_clicked()));
	connect(ui.autosetting, SIGNAL(clicked()), this, SLOT(on_AutoSetting_clicked()));
	connect(ui.autograbsetting, SIGNAL(clicked()), this, SLOT(on_AutoGrabSetting_clicked()));

	connect(ui.cameraautopros, SIGNAL(stateChanged(int)), this, SLOT(cameraautopros_state(int)));
	connect(ui.cameraautograb, SIGNAL(stateChanged(int)), this, SLOT(cameraautograb_state(int)));

	connect(ui.actionreport, SIGNAL(triggered()), this, SLOT(on_report_clicked()));
	connect(ui.actionaboutT, SIGNAL(triggered()), this, SLOT(on_about_clicked()));
}
void FlawPicker::initialize()
{	
	ui.FlawNumber->display("0:0");	
	imgsize.x = Dimgw;
	imgsize.y = Dimgh;
	imgsize.z = Dimgw * Dimgh;
	area_rangei(imgsize);
	finalsizei(max_area);

	filteropen = true;
	ui.actionlabelfilter->setChecked(true);

	curr_done=false;
	StructelementA = getStructuringElement(MORPH_ELLIPSE, Size(7,7));//ok
	StructelementB = getStructuringElement(MORPH_CROSS, Size(5,5));  //ok

	origin_cwdir = getcwd();
	pic_count=pass_count=ng_count = 0;	
	ui.actionautosave->setChecked(false);
	ui.actionsaverois->setChecked(false);
	
	img_path = default_path;
	roi_path = default_save_path;
	last_save_path=default_save_dir;
	last_open_path=default_dir;
	img_pathQ = default_dir;
	roi_pathQ = default_save_dir;
	stand_dir = standimg_dir;
	
	img_channel = CHANNEL;

	cameraParm_init();
	tableunitInit();

	colume_target();
	standard_mean.release();
	standard_mean = imread(standimg_name);
	standard_mean = grayCheck(standard_mean);
	if (!standard_mean.empty())
	{
		QMessageBox::StandardButton que = QMessageBox::question(NULL, "Notice", "There is an old standard average image.Willing to use it? ");
		if (que == QMessageBox::Yes)
		{			
			mask_maker();		
			return;
		}		
		else
		{
			standard_mean.release();
			system("del st.bmp");
		}				
	}
	
	
	standard_file_search();
	if (standard_mean.empty())
	{
		QMessageBox::warning(NULL, "Warning", "failed to prepare a standard average image sample.You may need to check /change the path of standard image pack later before operating functions.");

		return;
	}	
	
}
//*******************************************************************************************************slots
/*test*/
void FlawPicker::on_about_clicked()
{
	on_Open_clicked();
	Mat temp=grayCheck(img_curr);
	Mat maskrect= Mat::zeros(temp.size(), CV_8UC1);	
	Mat maskrect_r = Mat::zeros(temp.size(), CV_8UC1);
	int short_edge = min(img_curr.rows ,img_curr.cols);
	blur(temp ,temp ,Size(5,5));
	Sobel(temp,temp, CV_8U, 1, 0, 3, 1, 0);
	threshold(temp, temp,0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//displayMatResult(temp);
	vector<Vec4i> lines;
	HoughLinesP(temp ,lines, 1, CV_PI /2, 50, short_edge /3.6,short_edge /135);
	int x0 = 0, y0 = 0, cnt = 0;
	if (!lines.empty())
	{				
		vector<Vec4i>::iterator it = lines.begin();
		while (it != lines.end())
		{
			if (distanceCalc(Point((*it)[0], (*it)[1]), Point((*it)[2], (*it)[3])) < 400)
			{
				x0 += (*it)[0] + (*it)[2];
				y0 += (*it)[1] + (*it)[3];
				cnt++;
			}
			++it;
		}
		y0 = y0 / cnt / 2;
		x0 = x0 / cnt / 2;
	}
	
	Rect centerRec = Rect(x0-short_edge/10,y0-short_edge/6,short_edge/5,short_edge/3);
	rectangle(maskrect, centerRec , Scalar(255), 3);
	floodFill(maskrect, Point(x0,y0), Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	floodFill(maskrect_r, Point(0,0), Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	maskrect_r -= maskrect;
	//for (size_t i = 0; i < lines.size(); i++)//
	// {
	//	float rho = lines[i][0], theta = lines[i][1];
	//	
	//	Point pt1, pt2;
	//	pt1 = Point(lines[i][0], lines[i][1]);
	//	pt2 = Point(lines[i][2], lines[i][3]);	
	//	line(maskrect ,pt1, pt2, Scalar(255,255, 255), 3, CV_AA);
	//}
	cvtColor(img_curr, img_curr, CV_RGB2GRAY);
	displayMatResult(img_curr-maskrect_r);
}
/*File Works*/
void FlawPicker::on_Open_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open image"), last_open_path, tr("img(*.JPG *.BMP)"));
	

	if (filename.isEmpty())
	{
		return;
	}
	displaying_path=filename;
	last_open_path = filename;
	imgQ.load(filename);

	img_curr.release();	
	img_curr = imread(filename.toStdString());	
	img_curr.convertTo(img_curr, standard_mean.type());
	showresult(-1);
	showsignal(0);
	if (curr_done)
	{		
		curr_done = false;
	}
	if (!ui.actionimgprosT->isEnabled())
	{
		ui.actionimgprosT->setEnabled(true);
	}
	
}
void FlawPicker::on_Save_clicked()
{
	if (imgQ.isNull())
	{
		return;
	}
	imgQ.save(displaying_path);
}
void FlawPicker::on_SaveAs_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save image"), last_save_path, tr("img(*.JPG *.BMP)"));

	if (filename.isEmpty() || imgQ.isNull())
	{
		return;
	}
	imgQ.save(filename);
	last_save_path = filename;
}
/*Path& Reset Works*/
void FlawPicker::on_SettingT_clicked()
{
	bool state = ui.settingToolbar->isEnabled();
	ui.settingToolbar->setEnabled(!state);
}
void FlawPicker::on_Reset_clicked()
{
	QMessageBox::StandardButton que = QMessageBox::question(NULL, "question", "Sure to reset?");
	if (que == QMessageBox::Yes)
	{		
		all_clean_up();
		initialize();
		if (ui.groupBox->isEnabled())
		{
			on_stopCamera_clicked();
		}		
		
	
		QMessageBox::StandardButton que = QMessageBox::question(NULL, "NOTICE", "Reset the standard image path to default?");
		if (que == QMessageBox::No)
		{
			on_ResetStandardPath_Clicked();

		}

	}
}
void FlawPicker::on_ResetPath_clicked()
{
	QString pathname = QFileDialog::getExistingDirectory(NULL, "DefaultSourcePath", img_pathQ);
	if (pathname.isEmpty())
	{
		return;
	}
	img_path = pathname.toStdString();
	img_pathQ = pathname;
	QMessageBox::information(NULL, "NOTICE", "changed default source path to " + pathname);
	return;

}
void FlawPicker::on_ResetSavePath_clicked()
{
	QString pathname = QFileDialog::getExistingDirectory(NULL, "DefaultSavePath", roi_pathQ);
	if (pathname.isEmpty())
	{
		return;
	}
	roi_path = pathname.toStdString();
	roi_pathQ= pathname;
	QMessageBox::information(NULL, "NOTICE", "changed default saving path to " + pathname);
	return;

}
void FlawPicker::on_ResetStandardPath_Clicked()
{
	QString pathname = QFileDialog::getExistingDirectory(NULL, "DefaultStandardSorcesPath", QString::fromStdString(stand_dir));
	if (pathname.isEmpty())
	{
		return;
	}
	stand_dir = pathname.toStdString();
	
	
	QMessageBox::StandardButton que = QMessageBox::question(NULL, "NOTICE:changed default Standard Sorces path to " + pathname,
		                                                    "Reset the standard imageData?");
	if (que == QMessageBox::Yes)
	{
		standard_mean.release();
		system("del st.bmp");
		standardImg_pros(true);		
	}
	
	return;

}
/*Label Works*/
void FlawPicker::on_ToolT_clicked()
{
	bool state = ui.ToolBarSide->isEnabled();
	ui.ToolBarSide->setEnabled(!state);
}
void FlawPicker::on_cleanQimg_clicked()
{
	if (!img_curr.empty())
	{
		img_curr.release();
	}
	ui.picshow->clear();
	curr_done = false;
	showresult(-1);
	clearRoisAndLabels();
}
void FlawPicker::on_Enlarge_clicked()
{
	QRect geo=ui.picshow->geometry();
			
	int h = geo.height() * 1.02;
	int w = geo.width() * 1.02;
	int x = geo.left() - 0.5*(w - geo.width());
	int y = geo.top() - 0.5*(h - geo.height());

	if (x<70||y<30||h+y>470||w>840)
	{
		return;
	}
	QRect newone(x,y,w,h);
	ui.picshow->setGeometry(newone);
	ui.result->setGeometry(QRect(x,y, ui.result->width(),ui.result->height()));
	if(!imgQ.isNull())
	{
		ui.picshow->clear();
		ui.picshow->setPixmap(QPixmap::fromImage(imgQ));
		ui.picshow->show();
	}	
}
void FlawPicker::on_Shrink_clicked()
{
	QRect geo = ui.picshow->geometry();
		
	int h = geo.height()/1.02;
	int w = geo.width()/1.02;
	int x = geo.left() - 0.5*(w-geo.width());
	int y = geo.top() - 0.5*(h-geo.height());

	if (h<275 || w<490)
	{
		return;
	}

	QRect newone(x, y, w, h);
	ui.picshow->setGeometry(newone);
	ui.result->setGeometry(QRect(x,y, ui.result->width(), ui.result->height()));
	if (!imgQ.isNull())
	{
		ui.picshow->clear();
		ui.picshow->setPixmap(QPixmap::fromImage(imgQ));
		ui.picshow->show();
	}
}
void FlawPicker::on_Renew_clicked()
{
	if (!imgQ.isNull())
	{
		if (!displaying_path.isEmpty())
		{
			imgQ.load(displaying_path);
			img_curr.release();
			img_curr = QImage2cvMat(imgQ);

			showsignal();			
		}
		else
		{
			ui.picshow->clear();
			img_curr.release();
		}
	}
	else
	{
		displaying_path.clear();
		img_curr.release();
	}
	curr_done = false;
	showresult(-1);
}
/*groupBOX ROI*/
void FlawPicker::roishow_state(int state)
{
	if (ui.actionshowroi->isChecked() != state)
	{
		ui.actionshowroi->setChecked(state);
	}
	if (state == Qt::Checked)
	{	
		if (!roi_temper.empty())
		{
			int tpsz=min(int(roi_temper.size()),11);
			for (int j = 0;j < tpsz;j++)
			{
				QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(j + 1));
				cr->clear();
				cr->setPixmap(QPixmap::fromImage(cvMat2QImage(roi_temper[j])));
				cr->show();
			}
		}

	}
	else
	{
		for (int i = 0;i < 11;i++)
		{
			QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(i + 1));
			cr->clear();
		}
	
	}

}
void FlawPicker::roishow_state_ac()
{
	int state = ui.actionshowroi->isChecked();
	if (ui.checkboxshowroi->isChecked() != state)
	{
		ui.checkboxshowroi->setChecked(state);
	}
}
void  FlawPicker::roisave_state(int state)
{
	if (ui.actionsaverois->isChecked() != state)
	{
		ui.actionsaverois->setChecked(state);
	}
		
}
void FlawPicker::roisave_state_ac()
{
	int state = ui.actionsaverois->isChecked();
	if (ui.checkboxsaveroi->isChecked() != state)
	{
		ui.checkboxsaveroi->setChecked(state);
	}	
}
/*label filter*/
void FlawPicker::on_labelingT_clicked()
{	
	if (roi_temper.empty())
	{
		return;
	}
	filterlable();
}
void FlawPicker::filtermode_state()
{
	
	if (ui.actionlabelfilter->isChecked())
	{
		filteropen = true;
		QMessageBox::information(NULL,"Notice","Intellegent mode ON");
	}
	else
	{
		filteropen = false;
		QMessageBox::information(NULL, "Notice", "Intellegent mode OFF");
	}
}
/*Stop all*/
void FlawPicker::on_forbidAll_clicked()
{
	ui.settingToolbar->setEnabled(false);
	ui.ToolBarSide->setEnabled(false);
}
/*Pros Works*/
void FlawPicker::on_singlePros_clicked()
{
	if (!displaying_path.isEmpty())
	{
		img_curr = imread(displaying_path.toStdString());
	}
	if (img_curr.empty()||curr_done)
	{	
		
		on_Open_clicked();	
		if (img_curr.empty())
		{
			return;
		}
	}	
	if (filteropen)
	{
		finalpush_F(img_curr, contour_pros_single(img_curr, true), true);
	}
	else if (!filteropen)
	{
		finalpush(img_curr, contour_pros_single(img_curr, true), true);
		curr_done = true;
	}
	
	
}
void FlawPicker::on_batchPros_clicked()
{
	QString pathname = QFileDialog::getExistingDirectory(NULL, "Batch image Path", QString::fromStdString(default_path));
	if (pathname.isEmpty())
	{
		QMessageBox::StandardButton que = QMessageBox::question(NULL, "Notice", "Path invalid.Willing to Load from default path?");
		if (que == QMessageBox::No)
		{
			return;
		}
	}
	img_path = pathname.toStdString();	
	batch_pros();
	img_curr.release();

	
}
/*camera works*/
void FlawPicker::camera_choose()
{	
	startedcam = false;
	camera_t.stop();
	camera_t.curr_camera = NULL;

	camera_t.curr_camera = &(camera_t.m_myCameras[ui.devicecomboBox->currentIndex()]);
	ui.devicecomboBox->setEditText(ui.devicecomboBox->currentText());
	ui.devicecomboBox->hidePopup();
}
void FlawPicker::Updatedevices()
{
	int cam_num = ui.devicecomboBox->count();
	for (int i=cam_num; i>0; i--)
	{
		ui.devicecomboBox->removeItem(i);
	}
	camera_t.updateDevice();
}
void FlawPicker::on_camera_clicked()
{
	camera_t.initial();	
	
	if (camera_t.serial_num.empty())
	{
		QMessageBox::critical(NULL,"Error","No device exist");
		return;
	}
	
	int cnt = camera_t.serial_num.size();

	ui.groupBox->setEnabled(true);
	ui.actioncameraauto->setEnabled(true);
	ui.actionautograb->setEnabled(true);
	ui.devicecomboBox->setEditable(true);
	
	for(int i=0;i<cnt;i++)
	{		
		ui.devicecomboBox->insertItem(i, camera_t.serial_num[i]);
		
	}	
	cameraParm_init();
	camera_t.curr_camera= &(camera_t.m_myCameras[ui.devicecomboBox->currentIndex()]);
}
void FlawPicker::on_cameragrab_clicked()
{	
	if (!startedcam)
	{
		if (!camera_t.checkCurrCam())
		{
			return;
		}
	}

	img_curr.release();
	img_curr=camera_t.grab();
	displayMatResult(img_curr,25);
	startedcam = false;
	on_SaveAs_clicked();
	
}
void FlawPicker::on_startCamera_clicked()
{	
	if (!startedcam)
	{
		startedcam = true;
		external_packed_set(false);
		set_group_box(false);
	}	
	int mode = camera_mode_check();	
	if (camera_t.checkCurrCam())
	{
		while (startedcam)
		{
			img_curr.release();
			img_curr = camera_t.startRunning();
			if (mode == 2)
			{	
				bool res;
				if (filteropen)
				{
					res = finalpush_F(img_curr, contour_pros_single(img_curr, true), true);
				}
				else
				{
					res = finalpush(img_curr, contour_pros_single(img_curr, true), true);
				}
				if (res)
				{
					pass_cnt++;
				}
				else
				{
					ng_cnt++;
				}
				QTimer::singleShot(autopause, &eventloopC, SLOT(quit()));
				eventloopC.exec();
				total_cnt++;
			}
			else
			{
				displayMatResult(img_curr, 25);
				if (mode == 1)
				{
					saveGrabs();					
					QTimer::singleShot(autopausegrab, &eventloopC, SLOT(quit()));
					eventloopC.exec();
				}
			}
		}
	}
	else
	{
		startedcam = false;
		QMessageBox::critical(NULL,"Error","START Failed/Error occured in running");
		
	}
	chdir(origin_cwdir);
}
void FlawPicker::on_stopCamera_clicked()
{
	if (startedcam)
	{
		startedcam = false;
		
		
	}	
	camera_t.stop();
	int cam_num = ui.devicecomboBox->count();
	for (int i = cam_num-1; i>=0; i--)
	{
		ui.devicecomboBox->removeItem(i);
	}
	ui.picshow->clear();
	ui.picshow->setPixmap(QPixmap::fromImage(bkgd));
	ui.groupBox->setEnabled(false);
	ui.actioncameraauto->setEnabled(false);
	if (!ui.actionopenT->isEnabled())
	{
		external_packed_set(true);
	}
	img_curr.release();
}
void FlawPicker::on_pauseCamera_clicked()
{
	if (startedcam)
	{
		startedcam = false;
		external_packed_set(true);
		set_group_box(true);
	}
	camera_t.curr_camera->StopGrabbing();
	camera_t.m_bIsStartGrab = false;
}
void FlawPicker::cameraautopros_state(int state)
{
	if (state == Qt::Checked)
	{
		ui.actioncameraauto->setChecked(true);
		autopros = true;
		if (ui.cameraautograb->isEnabled())
		{
			if(ui.cameraautograb->isChecked())
			{ 
				ui.cameraautograb->setChecked(false);
			}			
			ui.cameraautograb->setEnabled(false);
			ui.actionautograb->setEnabled(false);
			autograb = false;
		}		
	}
	else
	{
		ui.actioncameraauto->setChecked(false);
		autopros = false;
		if (!ui.cameraautograb->isEnabled())
		{			
			ui.actionautograb->setEnabled(true);
			ui.cameraautograb->setEnabled(true);
		}
		for (int j = 0;j < 11;j++)
		{
			QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(j + 1));
			cr->clear();
		}
		ui.FlawNumber->display(QString("%1:").arg(pic_count+1) + QString("%1").arg(0));

	}
}
void FlawPicker::cameraautograb_state(int state)
{
	if (state == Qt::Checked)
	{
		ui.actionautograb->setChecked(true);
		autograb = true;
		if (ui.cameraautopros->isEnabled())
		{
			if (ui.cameraautopros->isChecked())
			{
				ui.cameraautopros->setChecked(false);
			}			
			ui.cameraautopros->setEnabled(false);
			ui.actioncameraauto->setEnabled(false);
			autopros = false;
		}
	}
	else
	{
		ui.actionautograb->setChecked(false);
		autograb = false;
		if (!ui.cameraautopros->isEnabled())
		{
			ui.actioncameraauto->setEnabled(true);
			ui.cameraautopros->setEnabled(true);
		}
	}
}
void FlawPicker::on_AutoSetting_clicked()
{
	bool ok = false;
	int pause = QInputDialog::getInt(NULL,"Process Frequency", "enter an integer(500~3000ms/pic)", autopause,500,3000,100,&ok);
	if (ok && pause>=500&&pause<=3000)
	{
		autopause = pause;
	}
	else
	{
		autopause = 1000;	
	}
	QMessageBox::information(NULL, "Notice", "set process frequency :"+QString("%1").arg(autopause)+"ms/pic");
	autosetted = true;
}
void FlawPicker::on_AutoGrabSetting_clicked()
{
	bool ok = false;
	int pause = QInputDialog::getInt(NULL, "Grab Frequency", "enter an integer(1000~5000ms/pic)", autopause, 1000, 5000, 100, &ok);
	if (ok && pause >= 1000 && pause <= 5000)
	{
		autopausegrab = pause;
	}

	QMessageBox::information(NULL, "Notice", "set grab frequency :" + QString("%1").arg(autopause) + "ms/pic");

	QString pathname = QFileDialog::getExistingDirectory(NULL, "SavePath", autograbpath);
	if (!pathname.isEmpty())
	{
		autograbpath = pathname;
	}
	QMessageBox::information(NULL, "NOTICE", "save path:" + pathname);
	autograbsetted = true;
}

void FlawPicker::on_report_clicked()
{
	table_update();
}
//*****************************************************************************************************img pros unit

void FlawPicker::standard_file_search()
{	
	standard_mean.release();		
	standardImg_pros(true);		
}
void FlawPicker::standardImg_pros(bool save)
{
	if (stand_dir.empty())
	{
		return;
	}
	if (!checkPath(stand_dir))
	{
		QMessageBox::critical(NULL,"ERROR","Default standard_picture path doesen't exist");
		return;
	}
	vector<Mat> tempv;
	
	for (directory_iterator i(stand_dir), end_iter; i != end_iter; i++)
	{
		string filename = stand_dir + "\\" + i->path().filename().string();
		Mat temp = imread(filename, 1);
		temp = grayCheck(temp);
		tempv.push_back(temp);

	}

	int cnt = tempv.size();
	if (cnt > 0)
	{
		vector<Mat>::iterator it = tempv.begin();
		standard_mean = *it / cnt;
		it++;
		while (it != tempv.end())
		{
			standard_mean += *it / cnt;
			++it;
		}
	}
	
	if (standard_mean.cols != imgsize.x)
	{
		update_filterdata(standard_mean);
	}

	if (save)
	{
		imwrite("st.bmp", standard_mean);
	}
	
	
	mask_maker();
	tempv.clear();

}
void FlawPicker::mask_maker()
{
	if (standard_mean.empty())
	{
		standard_file_search();

	}
	assert(!standard_mean.empty());
	Mat temp;
	standard_mean.copyTo(temp);
	int short_edg = min(temp.rows, temp.cols);
	//--------------------------------------------------------clear
	maskcenterPt = Point(0, 0);
	mask_earPt1 = Point(0, 0);
	mask_earPt2 = Point(0, 0);
	mask_radius = mask_earl = mask_earh = 0;
	maskcenter.release();
	maskcenterb.release();
	maskcenterb_r.release();

	//--------------------------------------------------------copy & convertion
	Mat mask = Mat::zeros(temp.size(), CV_8UC1);
	Mat maskb = Mat::zeros(temp.size(), CV_8UC1);
	if (temp.type() != CV_8UC1)
	{
		cvtColor(temp, temp, CV_BGR2GRAY);
	}

	vector<Vec3f> circles;
	int max_radius = 0;
	//------------------------------------center medium---------------------------------------//	
	Point cent(0, 0);
	HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 2, short_edg / 20, 100, 150, short_edg / 3.5, short_edg / 2.7);
	if (circles.empty())
	{
		QMessageBox::warning(NULL, "Warning", "Mask loading failed.This may caused by bad quality standard background pictures.Please change/grab a new group of background pictures later");
		return;
	}
	for (int i = 0;i < circles.size();i++)
	{
		Point seed;
		seed.x = circles[i][0];
		seed.y = circles[i][1];
		cent.x += seed.x;
		cent.y += seed.y;
		max_radius = max(int(circles[i][2]), max_radius);

	}
	cent.x = cent.x / circles.size();
	cent.y = cent.y / circles.size();
	circle(mask, cent, max_radius * 1.1, Scalar(255), 3);
	floodFill(mask, cent, Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);

	//--------------------------------------center big---------------------------------------//
	circles.clear();
	max_radius = 0;
	HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 2, 100, 100, 150, short_edg / 2.2, short_edg / 1.8);
	if (circles.empty())
	{
		QMessageBox::warning(NULL, "Warning", "Mask loading failed.This may caused by bad quality standard background pictures.Please change/grab a new group of background pictures later");
		return;
	}
	for (int i = 0;i < circles.size();i++)
	{
		Point seed;
		seed.x = circles[i][0];
		seed.y = circles[i][1];
		maskcenterPt.x += seed.x;
		maskcenterPt.y += seed.y;
		max_radius = max(int(circles[i][2]), max_radius);
	}
	maskcenterPt.x = maskcenterPt.x / circles.size();
	maskcenterPt.y = maskcenterPt.y / circles.size();
	mask_radius = max_radius-min_dist/10;
	
	circle(maskb, maskcenterPt, max_radius * 1.05, Scalar(255), 3);
	floodFill(maskb, maskcenterPt, Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	maskb.copyTo(maskcenterb);
	maskcenterb_r = Mat::ones(maskcenterb.size(), maskcenterb.type()) * 255 - maskcenterb;

	//---------------------------------------side double-------------------------------------//
	circles.clear();
	max_radius = 0;
	int ctl = 0, ctr = 0;
	HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 2, 1000, 100, 150, short_edg / 25, short_edg / 17);//ok
	if (circles.size()<2)
	{
		QMessageBox::warning(NULL, "Warning", "Mask loading failed.This may caused by bad quality standard background pictures.Please change/grab a new group of background pictures later");
		return;
	}	
	for (int i = 0;i < circles.size();i++)
	{
		Point seed;
		seed.x = circles[i][0];
		seed.y = circles[i][1];
		if (seed.x > maskcenterPt.x)//right
		{
			mask_earPt1.x += seed.x;
			mask_earPt1.y += seed.y;
			ctr++;
		}
		else//left
		{
			mask_earPt2.x += seed.x;
			mask_earPt2.y += seed.y;
			ctl++;
		}
		max_radius = max(int(circles[i][2]), max_radius);

	}
	ctl = max(ctl, 1);
	ctr = max(ctr, 1);

	mask_earPt1.x = mask_earPt1.x / ctr;
	mask_earPt1.y = mask_earPt1.y / ctr;
	mask_earPt2.x = mask_earPt2.x / ctl;
	mask_earPt2.y = mask_earPt2.y / ctl;
	mask_earl = max_radius*1.25;        //ok
	mask_earh = max_radius*1.85;        //ok

	circle(mask, mask_earPt1, max_radius * 1.20, Scalar(255), 3);
	floodFill(mask, mask_earPt1, Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	circle(mask, mask_earPt2, max_radius * 1.20, Scalar(255), 3);
	floodFill(mask, mask_earPt2, Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	Rect leftdiff = Rect(0,0, int(mask_earPt2.x - mask_earh), short_edg);
	Rect rightdiff = Rect(Point(min(mask_earPt1.x+ mask_earh,Dimgw),0),Point(Dimgw,Dimgh));
	rectangle(mask, leftdiff, Scalar(255), 3);	
	floodFill(mask,Point(leftdiff.x+0.5*leftdiff.width, leftdiff.y + 0.5*leftdiff.height), Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	rectangle(mask, rightdiff, Scalar(255), 3);
	floodFill(mask, Point(rightdiff.x + 0.5*rightdiff.width, rightdiff.y + 0.5*rightdiff.height), Scalar(255), NULL, cvScalarAll(0), cvScalarAll(0), CV_FLOODFILL_FIXED_RANGE);
	mask.copyTo(maskcenter);

}
void FlawPicker::colume_target()
{
	standard_colume.release();
	standard_colume=imread("cl_st.bmp");

	if (standard_colume.empty())
	{
		QMessageBox::critical(NULL,"ERROR","Colume target picture not found.Please put one under the working path of this programe.");
		return;
	}
	Mat temp = grayCheck(standard_colume);
	
	int short_edge = min(temp.rows, temp.cols);
	temp = basic_prepare(temp);

	vector<Vec4i> lines;
	HoughLinesP(temp, lines, 1, CV_PI / 2, 50, short_edge / 3.6, short_edge / 135);
	int x0 = 0, y0 = 0, cnt = 0;
	if (!lines.empty())
	{
		vector<Vec4i>::iterator it = lines.begin();
		while (it != lines.end())
		{
			if (distanceCalc(Point((*it)[0], (*it)[1]), Point((*it)[2], (*it)[3])) < 400)
			{
				x0 += (*it)[0] + (*it)[2];
				y0 += (*it)[1] + (*it)[3];
				cnt++;
			}
			++it;
		}
		y0 = y0 / cnt / 2;
		x0 = x0 / cnt / 2;
		colume_locate = Rect(x0 - short_edge / 10, y0 - short_edge / 6, short_edge / 5, short_edge / 3);
	}
	else
	{
		QMessageBox::critical(NULL,"ERROR","Faild to locate the colume target.Try replacing the current colume picture with one of better quality");
		return;
	}
}

Mat FlawPicker::minus_Standard(const Mat& img)
{

	if (standard_mean.empty())
	{
		assert(!stand_dir.empty());
		standard_file_search();

	}
	Mat out;
	
	out = Mat(img.rows, img.cols, img.type());
	absdiff(img, standard_mean, out);
	return out;

}
Mat FlawPicker::basic_pros(const Mat& img, int kernel_size)
{
	
	Mat img_m=basic_prepare(img,kernel_size);
	
	img_m = img_m - maskcenter;
	return img_m;
}
Mat FlawPicker::struct_pros(const Mat& img_thr, int label)
{
	Mat* element;
	if (label == 0)
	{
		element = &StructelementA;
	}
	else if (label == 1)
	{
		element = &StructelementB;
	}

	Mat temp;
	morphologyEx(img_thr, temp, MORPH_TOPHAT, *element);
	
	return temp;
}
vector<Rect>  FlawPicker::contour_pros_single(const Mat& img_m, bool structPro)
{
	assert(!img_m.empty());
	Mat img = grayCheck(img_m);
	Mat* temp = new Mat;
	*temp = minus_Standard(img);	

	Mat center, side;
	temp->copyTo(center);
	temp->copyTo(side);
	side = basic_pros(side, -1) - maskcenterb;
	center = basic_pros(center,5) - maskcenterb_r;
	
	
	vector<Rect> out;
	vector<Mat> contourscenter, contoursside;

	if (structPro)
	{
		vector<Mat> contour1, contour2;

		for (int k = 0;k < 2;k++)
		{
			findContours(struct_pros(center, k), contour1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			findContours(struct_pros(side, k), contour2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			
			contoursside.insert(contoursside.end(), contour2.begin(), contour2.end());
			contourscenter.insert(contourscenter.end(), contour1.begin(), contour1.end());
			contour1.clear();
			contour2.clear();
		}
	}
	else if (!structPro)
	{
		findContours(center, contourscenter, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		findContours(side, contoursside, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	}
	vector<Mat>::iterator it = contourscenter.begin();
	vector<Mat>::iterator its = contoursside.begin();
	while (it != contourscenter.end())
	{
		RotatedRect roi = minAreaRect(*it);
		double area = contourArea(*it);
		int distance = distanceCalc(roi.center, maskcenterPt);
		if (contour_filter(area, distance))
		{

			out.push_back(boundingRect(*it));

		}
		++it;
	}
	while (its != contoursside.end())
	{
		RotatedRect roi = minAreaRect(*its);
		double area = contourArea(*its);
		int distance1 = distanceCalc(roi.center, mask_earPt1);
		int distance2 = distanceCalc(roi.center, mask_earPt2);
		if (contour_filter(area, distance1, distance2))
		{

			out.push_back(boundingRect(*its));

		}
		++its;
	}
	temp->release();
	temp = NULL;
	delete temp;

	return out;
}
vector<Rect> FlawPicker::RectFilter(vector<Rect>& rois)
{

	vector<Rect> clusters;
	vector<Rect> finalsizeout,out;

	vector<Rect> temp1;
	temp1.insert(temp1.end(), rois.begin(), rois.end());
	vector<Rect>::iterator it = temp1.begin();

	while (it != temp1.end())
	{
		vector<Rect>::iterator it1 = temp1.begin();
		vector<Point> temp3;
		temp3.push_back((*it).tl());
		temp3.push_back((*it).br());
		double dist;

		while (it1 != temp1.end())	
		{
			dist = distanceCalc(*it, *it1);
				
			if (dist < min_dist&&dist>0)
			{

				temp3.push_back((*it1).tl());
				temp3.push_back((*it1).br());
				it1 = temp1.erase(it1);
				if (temp1.empty() || it1 == temp1.end())
				{

					break;
				}

			}
			else if (dist>=min_dist || dist <= 0.0)
			{
				++it1;
			}

		}
		
		clusters.push_back(boundingRect(temp3));
		
		++it;
	}

	vector<Rect>::iterator it2 = clusters.begin();	
	Rect R1, R2;
	int fnsz;
	while (it2 != clusters.end())
	{		
		R1 = trans2finalsize(*it2);
		finalsizeout.push_back(R1);
		it2 = clusters.erase(it2);
		vector<Rect>::iterator it3 = clusters.begin();
		int dist;
		while(it3!=clusters.end())
		{
			R2 = trans2finalsize(*it3);
			it3 = clusters.erase(it3);
			dist = distanceCalc(R1.tl(), R2.tl());
			if ( dist>=0.5*finalsize.width)
			{
				int dist_b,cnt=0;
				fnsz = finalsizeout.size();
				for (cnt = 0;cnt < fnsz;cnt++)
				{
					dist_b = distanceCalc(finalsizeout[cnt].tl(), R2.tl());
					if (dist_b < 0.5*finalsize.width)
					{
						break;
					}
				}
				if (cnt == fnsz)
				{
					finalsizeout.push_back(R2);
				}
			}					
		}
		it2 = clusters.begin();
	}
	
	return finalsizeout;
}
bool  FlawPicker::finalpush(const Mat& Orimg, vector<Rect>& rois, bool show,int pause)
{		
	roi_temper.clear();
	newrr.clear();
	newrr = RectFilter(rois);
	vector<Rect>::iterator it = newrr.begin();
	clearRoisAndLabels();

	int cnt = 0;
	pic_count++;
	chdir(roi_path.c_str());	
	while (it != newrr.end())
	{

		Mat temp = Mat(Orimg, *it);

		roi_temper.push_back(temp);
		if (ui.actionsaverois->isChecked())
		{
			saveCrops(temp, cnt);

		}
		if (ui.actionshowroi->isChecked())
		{
			if (cnt < 11)
			{
				QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(cnt + 1));			
				cr->setPixmap(QPixmap::fromImage(cvMat2QImage(temp)));				
				cr->show();
			}
			
		}
		++it;
		cnt++;
	}	
	if (show)
	{
		painting(Orimg, newrr);	
	}
	if (ui.actionautosave->isChecked())
	{
		saveCrops(QImage2cvMat(imgQ), cnt++);
	}
	ui.FlawNumber->display(QString("%1:").arg(pic_count)+ QString("%1").arg(cnt));	
	showresult(cnt);
	QTimer::singleShot(pause, &eventloop, SLOT(quit()));
	eventloop.exec();
	chdir(origin_cwdir);
	curr_done = true;
	if (cnt == 0)
	{
		pass_count++;
		return true;
	}
	ng_count++;
	return false;

}
bool  FlawPicker::finalpush_F(const Mat& Orimg, vector<Rect>& rois, bool show, int pause)
{
	roi_temper.clear();
	newrr.clear();
	newrr = RectFilter(rois);
	vector<Rect>::iterator it = newrr.begin();
	clearRoisAndLabels();

	
	pic_count++;
	chdir(roi_path.c_str());
	while (it != newrr.end())
	{
		Mat temp0 = Mat(Orimg, *it);
		roi_temper.push_back(temp0);
		++it;
	}
	bool resF = resFilter();

	int cnt= 0;
	for (cnt = 0; cnt< roi_temper.size(); cnt++)
	{
		Mat temp = roi_temper[cnt];
		if (ui.actionsaverois->isChecked())
		{
			saveCrops(temp, cnt);

		}
		if (ui.actionshowroi->isChecked())
		{
			if (cnt< 11)
			{
				QLabel *cr = QMainWindow::centralWidget()->findChild<QLabel *>(QString("roilabel_%1").arg(cnt + 1));
				cr->setPixmap(QPixmap::fromImage(cvMat2QImage(temp)));
				cr->show();
			}

		}				
	}
	
		
	if (show)
	{
		painting(Orimg, newrr);
	}
	if (ui.actionautosave->isChecked())
	{
		saveCrops(QImage2cvMat(imgQ), cnt++);
	}
	ui.FlawNumber->display(QString("%1:").arg(pic_count) + QString("%1").arg(cnt));
	showresult(cnt);
	QTimer::singleShot(pause, &eventloop, SLOT(quit()));
	eventloop.exec();
	chdir(origin_cwdir);
	curr_done = true;
	if (resF)
	{
		pass_count++;		
	}
	
	return resF;

}
//*****************************************************************************************************batch img pros

int FlawPicker::batch_load()
{
	if (img_path.empty())
	{
		img_path = default_path;
		img_pathQ = default_dir;
	}
	batch_img.clear();
	for (directory_iterator i(img_path), end_iter; i != end_iter; i++)
	{
		string filename = img_path + "\\" + i->path().filename().string();
		Mat temp = imread(filename, 1);
		if (!temp.empty())
		{
			
			temp.convertTo(temp, standard_mean.type());
			batch_img.push_back(temp);
		}

	}
	int img_num = batch_img.size();
	QString sz = QString("%1").arg(img_num);
	QMessageBox::information(NULL, "NOTICE", sz + "IMAGES WERE LOADED");
	return img_num;
}
void FlawPicker::batch_pros()
{
	if (!batch_img.empty())
	{
		batch_img.clear();
	}
	if (batch_load() == 0)
	{
		QMessageBox::critical(NULL,"Error","No image file exist under current path");
		return;
	}
	
	vector<Mat>::iterator it = batch_img.begin();
	while (it != batch_img.end())
	{
		if (filteropen)
		{
			finalpush_F(*it, contour_pros_single(*it, true), true);
		}
		else
		{
			finalpush(*it, contour_pros_single(*it, true), true);
		}
		++it;
	}
	QMessageBox::information(NULL, "NOTICE", "FINISHED");
	img_curr.release();
}

//*****************************************************************************************************basic functions
void FlawPicker::auto_save(QString img_name, bool Q)
{
	if (roi_pathQ.isEmpty())
	{
		roi_pathQ = default_save_dir;
		roi_path = default_save_path;
	}
	if (Q)
	{
		if (imgQ.isNull())
		{
			return;
		}
		imgQ.save(roi_pathQ + img_name + ".BMP");
	}
	else
	{
		if (imgM.empty())
		{
			return;
		}
		QString IMG_NAME = roi_pathQ + img_name + ".bmp";
		string fileAsSave = IMG_NAME.toStdString();
		imwrite(fileAsSave, imgM);

	}

}
void FlawPicker::saveCrops(const Mat& res, int cnt)
{
	
	char* s = new char;
	sprintf(s, "%d.bmp", cnt);
	char* s1 = new char;
	sprintf(s1, "%d", pic_count);
	
	
	
	char* C = (char*)new char(strlen(s1) + strlen(s));

	strcpy(C, s1);
	strcat(C, s);
	
	imwrite( C,res);

      delete C;
	delete s;
	delete s1;
	s = NULL;
	s1 = NULL;
	C = NULL;
	

}
