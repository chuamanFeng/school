#pragma once
#ifndef THREAD_H
#define THREAD_H

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigEInstantcamera.h>
#include <pylon/gige/BaslerGigEInstantCameraArray.h>
#include <pylon/PylonGUI.h>

#include "cv.h"
#include <qmessagebox.h>
#include <QThread>
#include <iostream>


using namespace std;
using namespace Pylon;
using namespace GenApi;
using namespace GenICam;
typedef Pylon::CBaslerGigEInstantCamera Camera_t;

extern int Dimgh;
extern int Dimgw;

class CameraThread : public QThread
{
	Q_OBJECT
public:
	    CameraThread();
		void initial();
		cv::Mat startRunning();
		cv::Mat grab();
		void stop();
		void updateDevice();
		bool checkCurrCam();
public:
	/*camera work*/
	DeviceInfoList_t m_myDevices;
	CTlFactory* m_pTlFactory;
	CBaslerGigEInstantCameraArray m_myCameras;
	Camera_t* curr_camera;

	vector<QString> serial_num;
	BOOL m_bIsStartGrab;
	CPylonImage pylonImage;
	CImageFormatConverter formatConverter;

	CGrabResultPtr m_ptrGrabResult;
	cv::Mat out;
private:
	
	volatile bool IsStopped;
	
	
};

#endif // THREAD_H
class CHeartbeatHelper
{
public:
	explicit CHeartbeatHelper(CInstantCamera& camera)
		: m_pHeartbeatTimeout(NULL)
	{
		// m_pHeartbeatTimeout may be NULL  
		m_pHeartbeatTimeout = camera.GetTLNodeMap().GetNode("HeartbeatTimeout");
	}

	bool SetValue(int64_t NewValue)
	{
		// Do nothing if no heartbeat feature is available.  
		if (!m_pHeartbeatTimeout.IsValid())
			return false;

		// Apply the increment and cut off invalid values if neccessary.  
		int64_t correctedValue = NewValue - (NewValue % m_pHeartbeatTimeout->GetInc());

		m_pHeartbeatTimeout->SetValue(correctedValue);
		return true;
	}

	bool SetMax()
	{
		// Do nothing if no heartbeat feature is available.  
		if (!m_pHeartbeatTimeout.IsValid())
			return false;

		int64_t maxVal = m_pHeartbeatTimeout->GetMax();
		return SetValue(maxVal);
	}

protected:
	GenApi::CIntegerPtr m_pHeartbeatTimeout; // Pointer to the node, will be NULL if no node exists.  
};
