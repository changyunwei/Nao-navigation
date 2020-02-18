#include "stdafx.h"

#include "myCVFunctions.h"
#include "Intrinsics.h"

#include "opencv/cv.hpp"
#include "opencv2/core/core.hpp"

/*!
 *  cFRW:   camera full resolution width
 *  cFRH:   camera full resolution height
 *  pS:     pixel size
 *  ct:     camera type
 *  caliRW: resolution width during calibration
 *  caliRH: resolution height during calibration
 */
CameraParameters::CameraParameters(int cFRW, int cFRH, float pS, CameraType ct, int caliRW, int caliRH)
    : camType(ct)
    , ccdFullResW(cFRW)
    , ccdFullResH(cFRH)
    , pixelSize(pS)
    , caliResW(caliRW)
    , caliResH(caliRH)
    , isCurSet(false)
{
    // Dimension of CCD in mm
    ccdW = ccdFullResW*pixelSize;
    ccdH = ccdFullResH*pixelSize;

    // Read intrinsic/distortion parameters from 
    intrinsics = (CvMat*)cvLoad( "data/camera_parameters.xml", NULL, (char*)CamParaStr[ct][0].c_str() );
    distCoeffs = (CvMat*)cvLoad( "data/camera_parameters.xml", NULL, (char*)CamParaStr[ct][1].c_str() );

    // Covent
    cv::FileStorage fs("data/camera_parameters.xml", cv::FileStorage::READ);
    fs["NAO_CAM_INTRINSIC"] >> camParaMat;
    fs["NAO_CAM_DISTORTION"] >> distCoeffMat;
    //printMatDouble("Camera Parameters", camParaMat);
    //printMatDouble("Distortion Coeffects", distCoeffMat);
    fs.release();

    // Focus length
    caliPixelSize = pixelSize*ccdFullResW/caliRW;
    focusX = CV_MAT_ELEM(*intrinsics, float, 0, 0)*caliPixelSize;
    focusY = CV_MAT_ELEM(*intrinsics, float, 1, 1)*caliPixelSize;

    qDebug() << "Camera parameters object created!";
    showParameters();
};

//CameraParameters::CameraParameters(const CameraParameters & cp)
//{
//    
//}

CameraParameters::~CameraParameters()
{
    if ( intrinsics != NULL )
    {
	    cvReleaseMat(&intrinsics);
	    intrinsics = NULL;
    }
    if ( distCoeffs != NULL )
    {
	    cvReleaseMat(&distCoeffs);
        distCoeffs = NULL;
    }

    camParaMat.release();
    distCoeffMat.release();
};

void CameraParameters::showParameters()
{
    qDebug() << "Camera Type:" << camType;
    qDebug() << "***** CCD Properties *****";
    qDebug() << "CCD Full Resolution:" << ccdFullResW << "x" << ccdFullResH;
    qDebug() << "Physical Pixel Size:" << pixelSize << "mm";    
    qDebug() << "CCD Physical Size:"   << ccdW << "x" << ccdH << "mm";
    qDebug() << "***** Calibration Configuration *****";
    qDebug() << "Calibration Resolution:" << caliResW << "x" << caliResH;    
    qDebug() << "Physical Pixel Size:" << caliPixelSize << "mm";
    qDebug() << "Focus Length fx:" << focusX << "mm  fy:" << focusY << "mm \n";

    printCvMat("Intrinsics", intrinsics);
    printCvMat("Distortion", distCoeffs);

    if ( isCurSet )
    {
        qDebug() << "Current Resolution:" << curResW << "x" << curResH;    
        qDebug() << "current Pixel Size:" << curPixelSize << "mm";
    }
    else
    {
        qDebug() << "!WARNING: Current Resolution hasn't been set yet!";
    }

    return;
};

float CameraParameters::setCurRes(int w, int h)
{
    isCurSet = 1;
    curResW = w;
    curResH = h;
    curPixelSize = pixelSize*ccdFullResW/curResW;
    return curPixelSize;
};

bool CameraParameters::getCameraParameters(CvMat **intrinsic, CvMat **distCoeff)
{
    *intrinsic = cvCloneMat(intrinsics);
    *distCoeff = cvCloneMat(distCoeffs);
    return 0;
};

bool CameraParameters::getCameraParameters( cv::Mat &cp, cv::Mat &dc )
{
    cp = camParaMat;
    dc = distCoeffMat;
    return 0;
};

bool CameraParameters::getCurStat(int &w, int &h, float &curPS)
{
    if ( isCurSet )
    {
        w = curResW;
        h = curResH;
        curPS = curPixelSize;
        return 0;
    }
    else
    {
        qDebug() << "!ERROR: Current Resolution hasn't been set yet!";
        return 1;
    }
};

void CameraParameters::getFocus(float &fx, float &fy)
{
    fx = focusX;
    fy = focusY;
    return;
};