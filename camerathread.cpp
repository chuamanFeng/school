#include "camerathread.h"
#include <QDebug>

CameraThread::CameraThread()
{
	formatConverter.OutputPixelFormat = PixelType_Mono8;	
	IsStopped = false;
}

void CameraThread::initial()
{	
	PylonInitialize();
	m_pTlFactory = &CTlFactory::GetInstance();
	m_myDevices.clear();
	m_pTlFactory->EnumerateDevices(m_myDevices);
	curr_camera = NULL;
	if (m_myDevices.empty())
	{
		
		return;
	}
	m_myCameras.Initialize((size_t)m_myDevices.size());
	string str;
	for (size_t i = 0; i < m_myCameras.GetSize(); ++i)
	{
		m_myCameras[i].Attach(m_pTlFactory->CreateDevice(m_myDevices[i]));
		str = (string)m_myCameras[i].GetDeviceInfo().GetSerialNumber();
	
		serial_num.push_back(QString::fromStdString(str));
		str.clear();
	}
	QMessageBox::information(NULL, "Notice:", QString("%1").arg(m_myDevices.size()) + " camera linked");
	m_bIsStartGrab = false;
	
}
cv::Mat CameraThread::startRunning()
{	
	if(m_bIsStartGrab && curr_camera->IsGrabbing())
	{
		
		curr_camera->RetrieveResult(5000, m_ptrGrabResult, TimeoutHandling_ThrowException);

		if (m_ptrGrabResult->GrabSucceeded())
		{
			assert(m_ptrGrabResult->GetWidth() == Dimgw && m_ptrGrabResult->GetHeight() == Dimgh);
			formatConverter.Convert(pylonImage, m_ptrGrabResult);
			out = cv::Mat(Dimgh, Dimgw, CV_8UC1, (uint8_t *)pylonImage.GetBuffer());
		
		}
		else
		{
			curr_camera->StopGrabbing();
			curr_camera->Close();
			return cv::Mat::zeros(Dimgh, Dimgw, CV_8UC1);
		}
		
	}

	return out;
}
cv::Mat CameraThread::grab()
{
	if (curr_camera->IsGrabbing())
	{
		try
		{
			curr_camera->RetrieveResult(5000, m_ptrGrabResult, TimeoutHandling_ThrowException);
			if (m_ptrGrabResult->GrabSucceeded())
			{
				assert(m_ptrGrabResult->GetWidth() == Dimgw && m_ptrGrabResult->GetHeight() == Dimgh);				
				formatConverter.Convert(pylonImage, m_ptrGrabResult);								
				out = cv::Mat(Dimgh, Dimgw, CV_8UC1, (uint8_t *)pylonImage.GetBuffer());
				
			}
			else
			{
				curr_camera->StopGrabbing();
				curr_camera->Close();
				return cv::Mat::zeros(Dimgh, Dimgw, CV_8UC1);
			}
		}
		catch (GenICam::GenericException& e)
		{
			curr_camera->StopGrabbing();
			curr_camera->Close();

			return cv::Mat::zeros(Dimgh, Dimgw, CV_8UC1);
		}

	}
	curr_camera->StopGrabbing();
	curr_camera->Close();

	return out;

}
bool CameraThread::checkCurrCam()
{
	if (curr_camera == NULL)
	{
		QMessageBox::information(NULL, "ERROR", "None of the devices has been chosen");
		return false;
	}
	if (!curr_camera->IsOpen())
	{
		curr_camera->Open();
	}
	CHeartbeatHelper headbeatHelper(*curr_camera);
	headbeatHelper.SetValue(1000);
	
	curr_camera->StartGrabbing(GrabStrategy_LatestImageOnly);
	m_bIsStartGrab = true;

	return true;
}
void CameraThread::stop()
{
	m_bIsStartGrab = FALSE;

	m_ptrGrabResult.Release();
	Sleep(200);

	if (m_myCameras.IsOpen())
		m_myCameras.Close();
	IsStopped = true;
}
void CameraThread::updateDevice()
{
	IsStopped = false;
	initial();
}