#include "stdafx.h"

#include <vector>

#include "myCVFunctions.h"
#include "Intrinsics.h"
#include "DIPAlgorithm.h"

#include "SiftRansac/xform.h"

#include "opencv/cv.hpp"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/ml.h"

using namespace cv;


DIPAlgorithm::DIPAlgorithm(void)
    : imgScribble(NULL)
{
    NaoCam = new CameraParameters(640, 480, 0.0022, NaoCamera, 640, 480);
    NaoCam->setCurRes(640, 480);
    NaoCam->getCameraParameters(&intrinsics, &distCoeffs);
    NaoCam->getCameraParameters(camParaMat, distCoeffMat);

    // Read objects coordinates from files
    objPoints = (CvMat*) cvLoad( "data/Square.xml" );
    //objPoints = (CvMat*) cvLoad( "data/Hexagon.xml" );
    //objPoints = (CvMat*) cvLoad( "data/Octagon.xml" );
    printCvMat("Object Coordinates", objPoints);
}

DIPAlgorithm::~DIPAlgorithm(void)
{
    // Clean up
    cvReleaseImage(&imgScribble);

    cvReleaseMat(&objPoints);

    delete NaoCam;
    NaoCam = NULL;

    cvReleaseMat(&intrinsics);
    cvReleaseMat(&distCoeffs);
    camParaMat.release();
    distCoeffMat.release();
}


void DIPAlgorithm::calibrateImage( IplImage *iplImg )
{
    if ( iplImg == NULL )
    {
        return;
    }

    // Build the undistort map that we will use for all
    // subsequent frames.
    // The function cvUndistort2() does one after the other in a single call.
    IplImage* mapx = cvCreateImage( cvGetSize(iplImg), IPL_DEPTH_32F, 1 );
    IplImage* mapy = cvCreateImage( cvGetSize(iplImg), IPL_DEPTH_32F, 1 );
    cvInitUndistortMap( intrinsics, distCoeffs, mapx, mapy );

    // Just run the camera to the screen, now showing the raw and
    // the undistorted image.
    IplImage *t = cvCloneImage(iplImg);
    cvRemap( t, iplImg, mapx, mapy ); // Undistort image

    cvReleaseImage(&t);
    cvReleaseImage(&mapx);
    cvReleaseImage(&mapy);

    return;
}


/*!
 *  To achieve this thresholding, we’ll be using the HSV colour space, 
 *  instead of the more common RGB colour space. In HSV, each “tint” of 
 *  colour is assigned a particular number (the Hue). The “amount” of 
 *  colour is assigned another number (the Saturation) and the brightness 
 *  of the colour is assigned another number (the Intensity or Value).
 */
IplImage* DIPAlgorithm::GetThresholdedImage(IplImage* img)
{
    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);

    // create a new image that will hold the thresholded image (which will be returned)
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

    /*! 
    *  Actual thresholding
    *  Here, imgHSV is the reference image. 
    *  And the two cvScalars represent the lower and upper bound of values that are yellowish in colour. 
    *  (These bounds should work in almost all conditions. If they don’t, try experimenting with the last two values).
    *  B G R
    */
    // Yellow
    cvInRangeS(imgHSV, cvScalar(20, 100, 100), cvScalar(30, 255, 255), imgThreshed);
    // Red
    //cvInRangeS(imgHSV, cvScalar(0, 100, 100), cvScalar(10, 255, 255), imgThreshed);
    //cvInRangeS(imgHSV, cvScalar(170, 100, 100), cvScalar(180, 255, 255), imgThreshed);
    // Blue
    //cvInRangeS(imgHSV, cvScalar(115, 100, 100), cvScalar(125, 255, 255), imgThreshed);
    // Green
    //cvInRangeS(imgHSV, cvScalar(55, 100, 100), cvScalar(65, 255, 255), imgThreshed);

    cvReleaseImage(&imgHSV);

    return imgThreshed;
}

void DIPAlgorithm::Scribbling(IplImage *frame)
{
    // If this is the first frame, we need to initialize it
    if (imgScribble == NULL)
    {
        /*!
        *  DON'T use ~d.lib, no zero initialization!
        */
        imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);
    }

    // Holds the yellow thresholded image (yellow = white, rest = black)
    IplImage* imgYellowThresh = GetThresholdedImage(frame);

    /*!
     *  NOTE: here assume that there will be only one yellow object on screen.
     *        If you have multiple objects, this code wont work.
     *  Calculate the moments to estimate the position of the ball
     *  (The moments may then be used then to calculate the gravity center of the shape, its
     *   area, main axises and various shape characteristics including 7 Hu invariants.)
     *  用矩算质心，如果不是单一物体，就算不出了。
     */
    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(imgYellowThresh, moments, 1);

    // The actual moment values
    double moment10 = cvGetSpatialMoment(moments, 1, 0);
    double moment01 = cvGetSpatialMoment(moments, 0, 1);
    double area     = cvGetCentralMoment(moments, 0, 0);

    // Holding the last and current ball positions
    static int posX = 0;
    static int posY = 0;

    int lastX = posX;
    int lastY = posY;

    posX = moment10/area;
    posY = moment01/area;

    // Print it out for debugging purposes
    _RPT2(_CRT_WARN, "position (%d,%d)\n", posX, posY);

    // We want to draw a line only if its a valid position
    if(lastX>0 && lastY>0 && posX>0 && posY>0)
    {
        // Draw a yellow line from the previous point to the current point
        cvLine(imgScribble, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,255,255), 5);
    }

    // Add the scribbling image and the frame...
    cvAdd(frame, imgScribble, frame);

    // Release the thresholded image+moments... we need no memory leaks.. please
    cvReleaseImage(&imgYellowThresh);
    delete moments;

    return;
}


/*!
 *  Four color points algorithm
 *  Get the geometric center of certain area colored between [lower, upper] in "img".
 */
CvPoint DIPAlgorithm::catchColorPoint(IplImage* img, CvScalar lower, CvScalar upper)
{
    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);

    // create a new image that will hold the thresholded image (which will be returned)
    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

    cvInRangeS(imgHSV, lower, upper, imgThreshed);

    CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(imgThreshed, moments, 1);

    // The actual moment values
    double moment10 = cvGetSpatialMoment(moments, 1, 0);
    double moment01 = cvGetSpatialMoment(moments, 0, 1);
    double area     = cvGetCentralMoment(moments, 0, 0);

    // Holding the last and current ball positions
    int posX = moment10/area;
    int posY = moment01/area;

    // Release the thresholded image+moments... we need no memory leaks.. please
    cvReleaseImage(&imgHSV);
    cvReleaseImage(&imgThreshed);
    delete moments;

    return cvPoint(posX, posY);
}

void DIPAlgorithm::showFourColorPoints(IplImage* img)
{
    CvPoint yellowPoint = catchColorPoint(img, cvScalar(20, 100, 100),  cvScalar(30, 255, 255));
    CvPoint redPoint    = catchColorPoint(img, cvScalar(0, 100, 100),   cvScalar(10, 255, 255));
    CvPoint greenPoint  = catchColorPoint(img, cvScalar(55, 100, 100),  cvScalar(65, 255, 255));
    CvPoint bluePoint   = catchColorPoint(img, cvScalar(115, 100, 100), cvScalar(125, 255, 255));

    IplImage *imgScribble = cvCreateImage(cvGetSize(img), 8, 3);
    cvLine(imgScribble, yellowPoint, bluePoint,   cvScalar(0,255,255), 5);
    cvLine(imgScribble, redPoint,    greenPoint,  cvScalar(0,255,255), 5);
    cvLine(imgScribble, greenPoint,  yellowPoint, cvScalar(0,255,255), 5);
    cvLine(imgScribble, bluePoint,   redPoint,    cvScalar(0,255,255), 5);

    // Add the scribbling image and the frame...
    cvAdd(img, imgScribble, img);

    cvReleaseImage(&imgScribble);
}

/*!
 *  Hough circles
 */
void DIPAlgorithm::trackHoughCircles(IplImage* img, CvSeq** results)
{
    IplImage* gimg = cvCreateImage(cvGetSize(img), 8, 1);
    // Grayscale
    cvCvtColor(img, gimg, CV_BGR2GRAY);

    CvMemStorage* storage = cvCreateMemStorage(0);
    cvSmooth(gimg, gimg, CV_GAUSSIAN, 5, 5 );
    if (results == NULL)
    {
        CvSeq* res = 0;
        results = &res;
    }
    *results = cvHoughCircles(gimg, storage, CV_HOUGH_GRADIENT, 2, gimg->width/10);
    //cvZero(img);
    for( int i = 0; i < (*results)->total; i++ )
    {
        float* p = (float*) cvGetSeqElem( *results, i );
        CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
        // thickness = -1, filled circles
        cvCircle(img, pt, cvRound( p[2] ), CV_RGB(0xff,0xff,0xff)/*, -1*/);
        cvLine(img, pt, pt, CV_RGB(255,0,0), 5);
    }

    cvReleaseImage(&gimg);
}

// From User Manual, doesn't work!
//void DIPAlgorithm::FourPointsHoughCircles(IplImage* img)
//{ 
//    Mat mimg(img);
//    Mat gmimg(img);
//
//    // Grayscale
//    cvtColor(mimg, gmimg, CV_BGR2GRAY);
//
//    // smooth it, otherwise a lot of false circles may be detected
//    GaussianBlur( gmimg, gmimg, Size(9, 9), 2, 2 );
//
//    // output
//    vector<Vec3f> circles;
//
//    HoughCircles(gmimg, circles, CV_HOUGH_GRADIENT,
//        2, gmimg.rows/4, 200, 100 );
//
//    for( size_t i = 0; i < circles.size(); i++ )
//    {
//        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//        int radius = cvRound(circles[i][2]);
//        // draw the circle center
//        cvCircle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
//        // draw the circle outline
//        cvCircle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
//    }
//
//    //cvReleaseImage(&gimg);
//
//    return;
//}


/*!
* cvGoodFeaturesToTrack -> Determines strong corners on an image.
*  image:        The source 8-bit or floating-point 32-bit, single-channel image
*  eigImage:     Temporary floating-point 32-bit image, the same size as image 
*  tempImage:    Another temporary image, the same size and format as eigImage
*  corners:      Output parameter; detected corners
*  cornerCount:  Output parameter; number of detected corners
*  qualityLevel: Multiplier for the max/min eigenvalue; specifies the minimal accepted quality of image corners
*  minDistance:  Limit, specifying the minimum possible distance between the returned corners; 
*                 Euclidian distance is used
*  mask:         Region of interest. The function selects points either in the specified region or in the whole
*                 image if the mask is NULL
*  blockSize:    Size of the averaging block, passed to the underlying 
*                 cvCornerMinEigenVal or cvCornerHarris used by the function
*  useHarris:    If nonzero, Harris operator ( cvCornerHarris) is used instead of default cvCornerMinEigenVal
*  k:            Free parameter of Harris detector; used only if (useHarris! = 0)
*/
void DIPAlgorithm::cornerGFTT(IplImage* img, int &count, CvPoint2D32f* corners, CvArr *mask)
{
    // Convert it into grayscale
    IplImage* imgGrayscale = cvCreateImage(cvGetSize(img), 8, 1);
    cvCvtColor(img, imgGrayscale, CV_BGR2GRAY);

    // Create temporary images required by cvGoodFeaturesToTrack
    IplImage* imgTemp = cvCreateImage(cvGetSize(img), 32, 1);
    IplImage* imgEigen = cvCreateImage(cvGetSize(img), 32, 1);

    // Create the array
    bool noOutput = 0;
    // No output
    if (corners == NULL)
    {
        noOutput = 1;
        // Internally create the array
        corners = new CvPoint2D32f[count];
    }

    // Find corners
    cvGoodFeaturesToTrack(imgGrayscale, imgEigen, imgTemp, corners, &count, 0.1, 100, mask, 3, 0, 0);

    // Mark these corners on the original image
    for(int i=0;i<count;i++)
    {
        // cvLine needs int points
        CvPoint pt = cvPoint(corners[i].x, corners[i].y);
        cvLine(img, pt, pt, CV_RGB(255,0,0), 5);

        // Print the serial number and integer corner
        qDebug("GFTT: %d - (%f,%f)", i, corners[i].x, corners[i].y);
    }

    // Print the number of corners
    qDebug("GFTT End! Count: %d", count);

    cvReleaseImage(&imgGrayscale);
    cvReleaseImage(&imgEigen);
    cvReleaseImage(&imgTemp);

    if (noOutput)
        delete [] corners;

    return;
}

/*!
*  SubPixel
*/
void DIPAlgorithm::cornerSubPixel(IplImage* img)
{    
    // Restore a grayscale image
    IplImage* imgGrayscale = cvCreateImage(cvGetSize(img), 8, 1);
    cvCvtColor(img, imgGrayscale, CV_BGR2GRAY);

    int count = 100;
    CvPoint2D32f *corners = new CvPoint2D32f[count];

    // Do GFTT first
    cornerGFTT(img, count, corners);

    // Refine corners
    cvFindCornerSubPix(imgGrayscale, corners, count, cvSize(5, 5), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 100, 0.000001));
    for(int i=0;i<count;i++)
    {
        // Print the serial number and the refined floating point corners
        qDebug("SubPixel: %d - (%f,%f)", i, corners[i].x, corners[i].y);
    }

    qDebug("SubPixel End!");

    cvReleaseImage(&imgGrayscale);
    delete [] corners;

    return;
}

/*!
*  Color tracking + GFTT
*/
void DIPAlgorithm::cornerColorGFTT(IplImage* img)
{
    // find the four colored points
    CvPoint yellowPoint = catchColorPoint(img, cvScalar(20, 100, 100),  cvScalar(30, 255, 255));
    CvPoint redPoint    = catchColorPoint(img, cvScalar(0, 100, 100),   cvScalar(10, 255, 255));
    CvPoint greenPoint  = catchColorPoint(img, cvScalar(55, 100, 100),  cvScalar(65, 255, 255));
    CvPoint bluePoint   = catchColorPoint(img, cvScalar(115, 100, 100), cvScalar(125, 255, 255));

    /*!
    *  Build the mask for GFTT
    */
    IplImage *mask = cvCreateImage(cvGetSize(img), 8, 1);
    cvZero(mask);

    int rad = 15;
    // The function draws a simple or filled circle with a given center and radius.
    // thickness positive no fill, otherwise filled
    cvCircle(mask, yellowPoint, rad, cvScalar(255,255,255), -1);
    cvCircle(mask, redPoint,    rad, cvScalar(255,255,255), -1);
    cvCircle(mask, greenPoint,  rad, cvScalar(255,255,255), -1);
    cvCircle(mask, bluePoint,   rad, cvScalar(255,255,255), -1);

    // Do GFTT with mask!
    int count = 100;
    CvPoint2D32f *corners = new CvPoint2D32f[count];
    cornerGFTT(img, count, corners, mask);

    // For debug, display the mask
    // !should do this after GFTT!
    cvCircle(img,  yellowPoint, rad, cvScalar(255,255,255));
    cvCircle(img,  redPoint,    rad, cvScalar(255,255,255));
    cvCircle(img,  greenPoint,  rad, cvScalar(255,255,255));
    cvCircle(img,  bluePoint,   rad, cvScalar(255,255,255));

    drawConvexHull(img, corners, count);

    cvReleaseImage(&mask);

    return;
}

/*!
 *  Hough circles + GFTT
 */
void DIPAlgorithm::cornerHoughGFTT(IplImage* img)
{
    qDebug("Hough GFTT start!");

    CvSeq *houghResults = NULL;
    trackHoughCircles(img, &houghResults);

    IplImage *mask = cvCreateImage(cvGetSize(img), 8, 1);
    cvZero(mask);

    for( int i = 0; i < houghResults->total; i++ )
    {
        float* p = (float*) cvGetSeqElem( houghResults, i );
        CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
        //cvCircle(img, pt, cvRound( p[2] ), CV_RGB(0xff,0xff,0xff));
        cvCircle(mask, pt, 10, CV_RGB(0xff,0xff,0xff), -1);
    }

    int count = 100;
    CvPoint2D32f *corners = new CvPoint2D32f[count];
    cornerGFTT(img, count, corners, mask);

    // Mark these corners on the original image
    for(int i=0;i<count;i++)
    {
        CvPoint pt = cvPoint(corners[i].x, corners[i].y);
        cvCircle(img, pt, 10, cvScalar(255,255,255));
    }

    // Draw Convex Hull of the points
    drawConvexHull(img, corners, count);

    qDebug("Hough GFTT end!");

    cvReleaseImage(&mask);
    delete [] corners;

    return;
}

/*!
*  Draw Convex Hull of given Points series
*    using Sklansky’s algorithm
*  core function:
*    CvSeq* cvConvexHull2(
*        const CvArr* input,
*        void* storage=NULL,
*        int orientation=CV CLOCKWISE,
*        int return points=0 ); 
*/
int DIPAlgorithm::drawConvexHull(IplImage* img, CvPoint2D32f *vertex, int count)
{
    if (count <= 0)
        return 0;

    // Create the matrix
    int* hull = (int*)malloc( count * sizeof(hull[0]));
    /*! type The type of the matrix elements in the form 
    *      CV_<bit depth><S|U|F>C<number of channels>, 
    *   where S=signed, U=unsigned, F=float. For example, CV_8UC1 means the
    *   elements are 8-bit unsigned and the there is 1 channel, and CV 32SC2 means the elements
    *   are 32-bit signed and there are 2 channels.
    */
    CvMat point_mat = cvMat( 1, count, CV_32FC2, vertex );
    CvMat hull_mat = cvMat( 1, count, CV_32SC1, hull );

    //qDebug("probe: 1\n");

    cvConvexHull2( &point_mat, &hull_mat, CV_CLOCKWISE, 0 );
    //err cvConvexHull2( vertex, 0, CV_CLOCKWISE, 0 );

    //qDebug("probe: 2\n");

    int hullcount = hull_mat.cols;

    // Starting point
    CvPoint pt0 = cvPointFrom32f(vertex[hull[hullcount-1]]);

    // vertex sequence dependent
    //cvPolyLine(img, (CvPoint**)&corners, &i, 1, 1, CV_RGB(255,255,0));

    for (int i=0; i<hullcount; i++)
    {
        CvPoint pt = cvPointFrom32f(vertex[hull[i]]);
        cvLine( img, pt0, pt, CV_RGB( 255, 255, 0 ));
        pt0 = pt;
        qDebug("%d : (%d, %d)", i, pt.x, pt.y);
    }

    free(hull);

    return hullcount;
}


void DIPAlgorithm::doorDetection(IplImage *frame_come)
{
    try
    {
        CvSVM svm = CvSVM();
        svm.load( "SVM_DATA.xml" );
        // set 3 features as a part of the feature vector
        int verticleLineNum = 0;
        int horizontalLineNum = 0;
        int clineLineNum = 0;
        //creat feature vector to save and pass values to tst_mat for testing
        vector<float> featureVectorTemp;
        featureVectorTemp.clear();
        //creat a one-line-matrix tst_mat for testing
        CvMat *tst_mat = 0;
        tst_mat = cvCreateMat( 1, 7, CV_32FC1 );

        //prepare variables for Canny and Hough Transform
        IplImage* tst = 0;
        tst = cvCreateImage( cvSize(frame_come->width,frame_come->height), 8, 1 );
        IplImage *tst_dst = 0;
        tst_dst = cvCreateImage( cvSize(frame_come->width,frame_come->height), 8, 1 );
        CvMemStorage* storage_tst = cvCreateMemStorage(0);
        CvSeq* lines_tst = 0;

        IplImage* frame = 0;
        frame = cvCreateImage( cvSize(frame_come->width,frame_come->height), 8, 3 );
        frame = cvCloneImage(frame_come);

        cvCvtColor( frame, tst, CV_RGB2GRAY );	
        cvCanny( tst, tst_dst, 50, 200, 3 );
        lines_tst = cvHoughLines2( tst_dst, storage_tst, CV_HOUGH_STANDARD, 1, CV_PI/180, 50, 0, 0 );

        for( int kk = 0; kk < MIN(lines_tst->total,100); kk++ )
        {
            float* line = (float*)cvGetSeqElem(lines_tst,kk);
            float rho = line[0];
            float theta = line[1];
            if ( (0 < theta&& theta < 0.3)||(2.84 < theta && theta < 3.14))
                verticleLineNum++;
            else
            {
                if ( 1.42 < theta && theta < 1.72)
                {horizontalLineNum++;}
                else clineLineNum++;
            }

            featureVectorTemp.push_back((float)lines_tst->total);
            featureVectorTemp.push_back((float)verticleLineNum);
            featureVectorTemp.push_back((float)horizontalLineNum);
            featureVectorTemp.push_back((float)clineLineNum);
            featureVectorTemp.push_back(float(verticleLineNum)/float(MIN(lines_tst->total,100)));
            featureVectorTemp.push_back(float(horizontalLineNum)/float(MIN(lines_tst->total,100)));
            featureVectorTemp.push_back(float(clineLineNum)/float(MIN(lines_tst->total,100)));

            verticleLineNum = 0;
            horizontalLineNum = 0;
            clineLineNum = 0;

            for (int kk=0; kk != featureVectorTemp.size(); kk++)
            {
                cvmSet(tst_mat,0, kk, featureVectorTemp[kk]);
            }
            featureVectorTemp.clear();			
            //predict
            int ret = svm.predict( tst_mat );
            if(ret == 0)
                printf("corridor \n");
            if(ret ==1)
                printf("Door \n");
        }
        cvReleaseImage( &frame );
        cvReleaseImage( &tst );
        cvReleaseImage( &tst_dst );
        cvReleaseMat( &tst_mat);
    }
    catch (const Exception exc)
    {
        throw exc;
    }

    return;
}

bool DIPAlgorithm::getViewpoint(IplImage *capturedImage, double **xyz, int numLandmark )
{
    sr = new SiftRansac();

    bool b = sr->relative_position2( capturedImage, xyz, numLandmark );

    delete sr;

    return b;
}