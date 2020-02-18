/*!********************************************************
 *  For dealing with OpenCV, the parameters of the camera
 *   should be firstly calculated carefully.
 *  <<Learning OpenCV>> Chapter 11. Calibration is highly
 *   recommanded to read.
 *  For each camera used, an instance can be created to store
 *   the parameters.
 *  --- Junchao Xu, MMI, EWI, TUDelft / 2011.03.30
 **********************************************************/
#pragma once
#ifndef INTRINSICS_H
#define INTRINSICS_H

struct CvMat;

// Camera selection
enum CameraType
{
    VAIO_Webcam    = 0,
    HiDef5M_Webcam = 1,
    NaoCamera      = 2
};

const std::string CamParaStr[3][2] = { "VAIO_WC_INTINSIC",     "VAIO_WC_DISTORTION", 
                                       "HIDEF5M_WC_INTRINSIC", "HIDEF5M_WC_DISTORTION",
                                       "NAO_CAM_INTRINSIC",    "NAO_CAM_DISTORTION"    };

class CameraParameters
{
private:
    // Camera Type
    CameraType camType;

    static std::string camera_name[3][2];

    // Intrinsic, physical parameters
    int ccdFullResW;  // pixel unit
    int ccdFullResH;  // pixel unit
    float pixelSize;  // mm
    float ccdW;       // mm
    float ccdH;       // mm

    int caliResW;     // pixel unit
    int caliResH;     // pixel unit
    float caliPixelSize; // mm

    float focusX;     // mm
    float focusY;     // mm

    // Camera Matrices
    CvMat *intrinsics;
    CvMat *distCoeffs;
    cv::Mat camParaMat;
    cv::Mat distCoeffMat;

    // Variable, keep the ratio when scaling
    bool isCurSet;
    int curResW;
    int curResH;
    float curPixelSize;

public:
    CameraParameters(int cFRW, int cFRH, float pS, CameraType ct, int caliRW, int caliRH);
    //CameraParameters(const CameraParameters & cp);
    ~CameraParameters();

    void showParameters();

    float setCurRes(int w, int h);

    bool getCameraParameters( CvMat **intrinsic, CvMat **distCoeff );
    bool getCameraParameters( cv::Mat &cp, cv::Mat &dc );

    bool getCurStat(int &w, int &h, float &curPS);

    void getFocus(float &fx, float &fy);

};

#endif