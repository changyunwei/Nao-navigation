#include "stdafx.h"
#include <QKeyEvent>
#include "DataManager.h"
#include "ImageFormat.h"
#include "CamCaliSampling.h"
#include "CameraCalibration.h"

#include "opencv/cv.hpp"

using namespace cv;

CameraCalibration::CameraCalibration(QWidget *parent)
    : QWidget(parent)
    , ccs(NULL)
{
    ui.setupUi(this);

    setFocusPolicy(Qt::StrongFocus);

    ccs = new CamCaliSampling();
    ccs->setCamCaliWindow(this);

    QObject::connect( ccs, SIGNAL(sigRawFrame(IplImage*)),                 this, SLOT(onsigDispRawVideo(IplImage*)) );
    QObject::connect( ccs, SIGNAL(sigUndistortedFrame(IplImage*)),         this, SLOT(onsigDispUndisortedVideo(IplImage*)) );
    QObject::connect( ccs, SIGNAL(sigCaliProgress(int)),                   this, SLOT(onsigCaliProgress(int)) );
    QObject::connect( ccs, SIGNAL(sigSamplingFinished()),                  this, SLOT(enableCalibration()) );
    QObject::connect( ccs, SIGNAL(sigCalibrationFinished(CvMat*, CvMat*)), this, SLOT(enableTestCaliResult(CvMat*, CvMat*)) );

    // If the thread is already running, this function does nothing.
    ccs->start();
}

CameraCalibration::~CameraCalibration()
{
    delete ccs;
}

void CameraCalibration::setCaliFrameSource(DataManager *pDM)
{
    ccs->setDataManager(pDM);
    return;
}

void CameraCalibration::showVideo(QLabel *lb, IplImage* iplImg)
{
    if ( iplImg )
    {
        // Convert to QImage for display
        QImage qImg = Ipl2QImage(iplImg);

        cvReleaseImage(&iplImg);

        // Scale the image size to the widget
        int w = lb->width();
        int h = lb->height();
        QPixmap qPix = QPixmap::fromImage(qImg.scaled(w, h),Qt::AutoColor);

        // Display the image
        lb->setPixmap(qPix);
    }

    return;
}

void CameraCalibration::onsigDispRawVideo(IplImage* iplImg)
{
    ui.spbImgW->setValue( iplImg->width );
    ui.spbImgH->setValue( iplImg->height );
    showVideo(ui.lbCalibration, iplImg);
    return;
}

void CameraCalibration::onsigDispUndisortedVideo(IplImage* iplImg)
{
    showVideo(ui.lbUndistorted, iplImg);
    return;
}

void CameraCalibration::onsigCaliProgress(int pg)
{
    ui.pgbSuccessView->setValue(pg);
    return;
}

void CameraCalibration::pbOnClickSample()
{    
    // Read the input
    ccs->numberCrossInWidth  = ui.spbNumCrossW->value();
    ccs->numberCrossInHeight = ui.spbNumCrossH->value();
    ccs->numCaliView         = ui.spbNumCaliView->value();
    ui.pgbSuccessView->setMaximum(ccs->numCaliView);

    ccs->sampling();

    return;
}

void CameraCalibration::pbOnClickCalibration()
{
    ccs->calibration();
    ui.pbCalibration->setEnabled(false);
    return;
}

void CameraCalibration::pbOnClickTestCaliResult(bool b)
{
    ccs->testCaliResult(b);

    return;
}

void CameraCalibration::pbOnClickAcceptGoodView()
{
    ccs->acceptGoodView();
    return;
}

void CameraCalibration::enableCalibration()
{
    ui.pbCalibration->setEnabled(true);
    return;
}

void CameraCalibration::enableTestCaliResult(CvMat* cm, CvMat* dc)
{
    ui.pbTest->setEnabled(true);

    ui.dsbFx->setValue   ( CV_MAT_ELEM( *cm, double, 0, 0 ) );
    ui.dsbCM12->setValue ( CV_MAT_ELEM( *cm, double, 0, 1 ) );
    ui.dsbCx->setValue   ( CV_MAT_ELEM( *cm, double, 0, 2 ) );
    ui.dsbCM21->setValue ( CV_MAT_ELEM( *cm, double, 1, 0 ) );
    ui.dsbFy->setValue   ( CV_MAT_ELEM( *cm, double, 1, 1 ) );
    ui.dsbCy->setValue   ( CV_MAT_ELEM( *cm, double, 1, 2 ) );
    ui.dsbCM31->setValue ( CV_MAT_ELEM( *cm, double, 2, 0 ) );
    ui.dsbCM32->setValue ( CV_MAT_ELEM( *cm, double, 2, 1 ) );
    ui.dsbHomoW->setValue( CV_MAT_ELEM( *cm, double, 2, 2 ) );
    ui.dsbK1->setValue   ( CV_MAT_ELEM( *dc, double, 0, 0 ) );
    ui.dsbK2->setValue   ( CV_MAT_ELEM( *dc, double, 1, 0 ) );
    ui.dsbP1->setValue   ( CV_MAT_ELEM( *dc, double, 2, 0 ) );
    ui.dsbP2->setValue   ( CV_MAT_ELEM( *dc, double, 3, 0 ) );
    ui.dsbK3->setValue   ( CV_MAT_ELEM( *dc, double, 4, 0 ) );
    return;
}

void CameraCalibration::keyPressEvent( QKeyEvent * event )
{
    if ( event->text() == QString(" ") )
    {
        ccs->acceptGoodView();
    }
    return;
}