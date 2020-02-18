/*************************************************
 * GUI main window
 *  triggered by "QGuiThread"
 *************************************************/

#include "stdafx.h"
#include "USpy.h"
#include "UVideo.h"
#include "DataManager.h"
#include "ImageProcessor.h"
#include "ImageFormat.h"

#include "urbi/uimage.hh"
#include <QImage>
#include "WizofOz.h"
#include "CameraCalibration.h"
#include "RobotIndicator.h"
#include "naourbiqtremoteclient.h"
#include "myCVFunctions.h"
#include "synchronization.h"

#include "opencv/cv.hpp"

using namespace urbi;

NaoUrbiQtRemoteClient::NaoUrbiQtRemoteClient(USpy *pUS)
	: QMainWindow(0,0)
    , pUSpy(pUS)
    , pUVideo(NULL)
    , isCnctd(0)
    , cbdSonarL(1)
    , cbdSonarR(2)
    , dataMgr(NULL)
    , imgProcessor(NULL)
    , WoZCC(NULL)
    , CamCali(NULL)
    , inertialX(0)
    , inertialY(0)
    , inertialTheta(0)
	, procIntv(0)
	, procCount(0)
	, procFps(0)
{
	ui.setupUi(this);

    int mainwinW = 1400;
    int mainwinH = 900;
    ui.mainScrollAreaWidgetContents->setMinimumSize(mainwinW,mainwinH);
    // Only QMainWindow can do like this
    this->setCentralWidget(ui.mainScrollArea);


    // Default Host and Port
  //  ui.leHost->setText("0");
  //  ui.lePort->setText("0");

    // Create Data Storage
    dataMgr = new DataManager();
    // Create Image Processing Thread
    imgProcessor = new ImageProcessor(dataMgr);

    // Set reference pointer to USpy
    pUSpy->setDataStorage(dataMgr);
    pUVideo = UVideo::gpUVideo;
    pUVideo->setDataStorage(dataMgr);

    // Initial the WizofOZ
    WoZCC = new WoZControlClient();
    WoZCC->setUrbiMessenger(pUSpy);
    ui.pbWoZ->setEnabled(true);

    // Set Canvas mapping
	ui.Canvas->setUrbiMessenger(pUSpy);
	ui.Canvas->setCommunicationMessenger(ui.Communication);

	// Set Communication Mapping
	ui.Communication->setUrbiMessenger(pUSpy);


	// 
    robotInd = new RobotIndicator();

    /**************************************************************
     * Set signal to slot mapping
     **************************************************************/
    // Send by image processing thread, before processing
    connect( imgProcessor, SIGNAL(sigDispRawFrame(urbi::UImage*)), 
                     this, SLOT(onsigDispRawFrames(urbi::UImage*)) );
    connect( imgProcessor, SIGNAL(sigDispProcessedFrame(IplImage*)), 
                     this, SLOT(onsigDispProcessedFrames(IplImage*)) );

    connect( pUSpy, SIGNAL(sigSonarL(float)), 
              this, SLOT(onsigDispSonarL(float)) );
    connect( pUSpy, SIGNAL(sigSonarR(float)), 
              this, SLOT(onsigDispSonarR(float)) );
    connect( pUSpy, SIGNAL(sigFacePos(float, float, float)), 
              this, SLOT(onsigDispFacePos(float, float, float)) );

    connect( pUSpy, SIGNAL(sigRobotPosition(float, float, float)), 
              this, SLOT(onsigDispRobotPosition(float, float, float)) );

   // connect(ui.Communication, SIGNAL(executeNavigation), ui.Canvas, SLOT(onExecuteNavigation()));

    // Robot -> Canvas
    // Note that the signal and slots parameters must not contain any variable names, only the type. 
    QObject::connect( robotInd, SIGNAL(updateMap(qreal, qreal, qreal)), 
        ui.Canvas, SLOT(updatePaint(qreal, qreal, qreal)) );

    QObject::connect( ui.pbPathPlanning, SIGNAL(clicked()), this, SLOT(pbOnClickPathPlanning()) );
	QObject::connect( ui.pbFaceLM,       SIGNAL(clicked()), this, SLOT(pbOnClickFaceLM()) );
    QObject::connect( ui.pbSaveImage,    SIGNAL(clicked()), this, SLOT(pbOnClickSaveImage()) );

	QObject::connect( ui.pb0, SIGNAL(clicked()), this, SLOT(pbOnClickTurnHeadYaw0()) );
	QObject::connect( ui.pb45, SIGNAL(clicked()), this, SLOT(pbOnClickTurnHeadYaw45()) );
	QObject::connect( ui.pb90, SIGNAL(clicked()), this, SLOT(pbOnClickTurnHeadYaw90()) );
	QObject::connect( ui.pbn45, SIGNAL(clicked()), this, SLOT(pbOnClickTurnHeadYawn45()) );
	QObject::connect( ui.pbn90, SIGNAL(clicked()), this, SLOT(pbOnClickTurnHeadYawn90()) );

    QObject::connect( ui.pbSeeDoor, SIGNAL(clicked()), this, SLOT(pbOnClickSeeDoor()) );

}

void NaoUrbiQtRemoteClient::pbOnClickSeeDoor()
{
	pUSpy->send("BehaviorManager.runBehavior(\"happy\") | tts.say(\"May I come in?\");");
    return;
}

void NaoUrbiQtRemoteClient::pbOnClickTurnHeadYaw0()
{
	// 0 degree 1.5s
	QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(0).arg(1.5);
	pUSpy->send( s.toStdString().data() );
}
void NaoUrbiQtRemoteClient::pbOnClickTurnHeadYaw45()
{
	// 45 degree 1.5s
	QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(PI/4).arg(1.5);
	pUSpy->send( s.toStdString().data() );
}
void NaoUrbiQtRemoteClient::pbOnClickTurnHeadYaw90()
{
	// 90 degree 1.5s
	QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(PI/2).arg(1.5);
	pUSpy->send( s.toStdString().data() );
}

void NaoUrbiQtRemoteClient::pbOnClickTurnHeadYawn45()
{
	// 45 degree 1.5s
	QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(-PI/4).arg(1.5);
	pUSpy->send( s.toStdString().data() );
}
void NaoUrbiQtRemoteClient::pbOnClickTurnHeadYawn90()
{
	// 90 degree 1.5s
	QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(-PI/2).arg(1.5);
	pUSpy->send( s.toStdString().data() );
}


NaoUrbiQtRemoteClient::~NaoUrbiQtRemoteClient()
{
    // release memory
    if ( CamCali )
        delete CamCali;

    delete WoZCC;
    delete imgProcessor;
    delete robotInd;
    delete dataMgr;
}

void NaoUrbiQtRemoteClient::closeEvent(QCloseEvent *event)
{
    pUVideo->subscribeImage(false);

    /* you can prevent the window from closing by calling ignore() on all events.*/
    //event->ignore();
    event->accept();
}

void NaoUrbiQtRemoteClient::MotorEnable(bool en, const char* motorname)
{
    if ( en )
    {
        // Turn on head motors
        QString s = QString("%1.load=1;wall(\"Turn %1 motors on!\");").arg(motorname);
        pUSpy->send(s.toStdString().data());
    } 
    else
    {
        // Turn off head motors
        QString s = QString("%1.load=0;wall(\"Turn %1 motors off!\");").arg(motorname);
        pUSpy->send(s.toStdString().data());
    }
}

void NaoUrbiQtRemoteClient::pbOnClickSaveImage()
{
    dataMgr->saveLatestFrame();
}

void NaoUrbiQtRemoteClient::pbOnClickMotorEnableHead(bool en)
{
    MotorEnable(en, "head");
}

void NaoUrbiQtRemoteClient::pbOnClickMotorEnableLeg(bool en)
{
    MotorEnable(en, "leg");
}

void NaoUrbiQtRemoteClient::pbOnClickMotorEnableArm(bool en)
{
    MotorEnable(en, "arm");
}

void NaoUrbiQtRemoteClient::pbOnClickWoZ()
{
    WoZCC->show();
    qDebug() << "WizofOz control GUI opened!";
}

void NaoUrbiQtRemoteClient::pbOnDispSonar()
{
    // Left sonar
    //UMsger->subscribeJSValue("ALMemory.getData(\"Device/SubDeviceList/US/Left/Sensor/Value\");", &cbdSonarL);

    // Right sonar
    //UMsger->subscribeJSValue("ALMemory.getData(\"Device/SubDeviceList/US/Right/Sensor/Value\");", &cbdSonarR);

    return;
}

void NaoUrbiQtRemoteClient::pbOnClickTurnOnCamera(bool b)
{
    if ( b )
    {
        pUVideo->subscribeImage(true);
	    imgProcessor->start();
		procTime.start();

		//enable image processing
		ui.pbTurnOnImageProcessing->setEnabled(true);
    } 
    else
    {
        imgProcessor->stopThread();
        pUVideo->subscribeImage(false);
		
		ui.cameraFps->setText("CAMERA OFF");
		//disable image processing
        ui.pbTurnOnImageProcessing->setEnabled(false);
    }

    return;
}

void NaoUrbiQtRemoteClient::pbOnClickTurnOnInertial(bool b)
{
    if ( b )
    {
        pUSpy->subscribeInertial(true);
        pUSpy->refInertialCnt = 0;
    } 
    else
    {
        pUSpy->subscribeInertial(false);
    }
}


void NaoUrbiQtRemoteClient::pbOnClickFaceTrack(bool b)
{
    if ( b )
    {
        pUSpy->trackFace(true);
    } 
    else
    {
        pUSpy->trackFace(false);
    }
}

void NaoUrbiQtRemoteClient::pbOnClickSoundTrack(bool b)
{
    if ( b )
    {
        pUSpy->send("BehaviorManager.runBehavior(\"SoundTracker\"),");
    } 
    else
    {
        pUSpy->send("BehaviorManager.stopBehavior(\"SoundTracker\"),");
    }
}


void NaoUrbiQtRemoteClient::pbOnClickTurnOnProcessing(bool en)
{
    imgProcessor->setProcessing(en);

    return;
}

// Signal->Slot
void NaoUrbiQtRemoteClient::onsigDispRawFrames(urbi::UImage* uimg)
{
    if (uimg != NULL)
    {
        // Get the pointer to UBinary to display
        // UBinary *b_img = dataMgr->fetchRawFrame();

        // create a QImage by existing data
        QImage qImg;
	    bool qimgerr = qImg.loadFromData((uchar*)uimg->data, uimg->size, "JPEG");

        // clean up
        // delete uimg->data;

        // delete the frame in buffer
        //b_img->clear();
        
        // Save to a file if necessary
	    //qImg->save("qimg.jpeg");

        // Scale the image size to the widget
	    int w = ui.cameraProcessed->width();
	    int h = ui.cameraProcessed->height();
	    QPixmap qPix = QPixmap::fromImage(qImg.scaled(w, h),Qt::AutoColor);

        // Display the image
	    ui.cameraProcessed->setPixmap(qPix);

		//calculation fps
		procCount++;
		if (procCount >= 8)
		{
			procCount=0;
			procIntv = procTime.restart();;
			procFps = 8000/(float)procIntv;
			ui.cameraFps->setText(tr("Fps of Original Image:%1").arg(procFps));
		}
    }

    return;
}

// Signal->Slot
void NaoUrbiQtRemoteClient::onsigDispProcessedFrames(IplImage* iplImg)
{
    // Convert to QImage for display
    QImage qImg = Ipl2QImage(iplImg);


    //qDebug() << "QImage conversion succeed! w: " << qImg.width() << "h: " << qImg.height();

    // Clear the IplImage
    cvReleaseImage(&iplImg);

    //qImage.loadFromData(rawUImg->data, rawUImg->size, "JPEG");

    // Scale the image size to the widget
    int w = ui.cameraProcessed->width();
    int h = ui.cameraProcessed->height();

    /*!
    * Assigns the given pixmap to this pixmap and returns a reference to this pixmap
    * It is a DEEP copy!
    */
    QPixmap qPix = QPixmap::fromImage(qImg.scaled(w, h),Qt::AutoColor);
    ui.cameraProcessed->setPixmap(qPix);

    // Fps calculation
	procCount++;
	if (procCount >= 8)
	{
		procCount=0;
		procIntv = procTime.restart();;
		procFps = 8000/(float)procIntv;
		ui.cameraFps->setText(tr("Fps of Processed Image:%1").arg(procFps));
	}


    return;
}

// Signal->Slot
void NaoUrbiQtRemoteClient::onsigDispSonarL(float sl)
{
    ui.lbSonarLDisp->setText( tr("%1").arg(sl) );

    return;
}
// Signal->Slot
void NaoUrbiQtRemoteClient::onsigDispSonarR(float sr)
{
    ui.lbSonarRDisp->setText( tr("%1").arg(sr) );

    return;
}

// Signal->Slot
void NaoUrbiQtRemoteClient::onsigDispFacePos(float x, float y, float z)
{
    imgProcessor->faceX = x;
    imgProcessor->faceY = y;
    imgProcessor->faceZ = z;
    QString s = QString("(%1, %2, %3)").arg(x).arg(y).arg(z);
    qDebug() << "Face position:" << s;
    ui.leFacePosition->setText(s);
    return;
}

//Signal->slot
void NaoUrbiQtRemoteClient::onsigDispRobotPosition(float x, float y, float z)
{
    // UI display
    ui.leRobotX->setText(tr("%1").arg(x));
    ui.leRobotY->setText(tr("%1").arg(y));
	ui.leRobotTheta->setText(tr("%1").arg(z));

	// m   m   rad
    ui.Canvas->updateInertial(x, y, z);

    return;
}

void NaoUrbiQtRemoteClient::pbOnClickCorrect2DCoordinates()
{

 //   double *relativePosXYZ = new double[4]();
 //   relativePosXYZ[3] = 1;

	//bool b=1;
	//int lmNum = 0;

	//switch (ui.Canvas->step)
	//{
	//case 1:
 //       // Relative position in Camera Coordinates
 //       b = imgProcessor->calculateViewpoint( 10, &relativePosXYZ );
	//	lmNum = 10;
	//	break;
	//case 2:
	//	// Relative position in Camera Coordinates
	//	b=imgProcessor->calculateViewpoint( 4, &relativePosXYZ );
	//	lmNum = 4;
	//	if (b)
	//	{
	//		b=imgProcessor->calculateViewpoint( 9, &relativePosXYZ );
	//		lmNum = 9;
	//	}
	//	if (b)
	//	{
	//		b=imgProcessor->calculateViewpoint( 1, &relativePosXYZ );
	//		lmNum = 1;
	//	}
	//	break;
	//case 3:
	//	// Relative position in Camera Coordinates
	//	b=imgProcessor->calculateViewpoint( 2, &relativePosXYZ );
	//	lmNum = 2;
	//	if (b)
	//	{
	//		b=imgProcessor->calculateViewpoint( 3, &relativePosXYZ );
	//		lmNum = 3;
	//	}

	//	break;
	//case 4:
	//	// Relative position in Camera Coordinates
	//	b=imgProcessor->calculateViewpoint( 11, &relativePosXYZ );
	//	lmNum = 11;
	//	if (b)
	//	{
	//		b=imgProcessor->calculateViewpoint( 12, &relativePosXYZ );
	//		lmNum = 12;
	//	}

	//	break;
	//case 5:
	//	// Relative position in Camera Coordinates
	//	b=imgProcessor->calculateViewpoint( 6, &relativePosXYZ );
	//	lmNum = 6;
	//	if (b)
	//	{
	//		b=imgProcessor->calculateViewpoint( 7, &relativePosXYZ );
	//		lmNum = 7;
	//	}

	//	break;
	//case 6:
	//	// Relative position in Camera Coordinates
	//	b=imgProcessor->calculateViewpoint( 5, &relativePosXYZ );
	//	lmNum = 5;
	//	if (b)
	//	{
	//		b=imgProcessor->calculateViewpoint( 8, &relativePosXYZ );
	//		lmNum = 8;
	//	}
	//	break;
	//default:
	//	break;
	//}

	//if ( b )
	//{
	//	return;
	//}

 //   qDebug("Viewpoint: (%f, %f, %f)mm", relativePosXYZ[0], relativePosXYZ[1], relativePosXYZ[2]);

 //   // Absolute distance between landmark and viewpoint
 //   double vectorLen = qSqrt( relativePosXYZ[0]*relativePosXYZ[0] + relativePosXYZ[1]*relativePosXYZ[1] + relativePosXYZ[2]*relativePosXYZ[2] );
 //   qDebug("Vector Length: %f mm", vectorLen);

 //   // Read out the head angles
 //   double *headAngles = new double[2]();
 //   bool bb = pUSpy->getHeadAngles(&headAngles);
 //   qDebug() << "Yaw:" << headAngles[0] << "Pitch:" << headAngles[1];

 //   // 
 //   double hy = 180 * headAngles[0] / PI;
 //   ui.Canvas->relocateRobot( lmNum, relativePosXYZ, hy );


 //   delete relativePosXYZ;
 //   delete headAngles;
 //   headAngles = NULL;
 //   return;
}

void NaoUrbiQtRemoteClient::pbOnClickCalibration()
{
    CamCali = new CameraCalibration();
    CamCali->setCaliFrameSource(dataMgr);
    CamCali->show();
    // pUVideo->subscribeImage(true);
    return;
}

void NaoUrbiQtRemoteClient::pbOnClickFaceLM()
{
    ui.Canvas->faceLandmark();
}

void NaoUrbiQtRemoteClient::pbOnClickPathPlanning()
{
	// new path planning
	ui.Canvas->updateStartEnd();
	ui.Canvas->pGridMap->findNewPath();
	ui.Canvas->QWidget::update();
	ui.Canvas->execution(TRUE);
}