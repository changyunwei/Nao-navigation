#pragma once
#ifndef DIP_ALGORITHM
#define DIP_ALGORITHM

typedef _IplImage IplImage;
typedef void CvArr;
struct CvMat;
struct CvScalar;
struct CvPoint2D32f;
struct CvSeq;
struct CvPoint;

class CameraParameters;
class SiftRansac;

class DIPAlgorithm
{
public:
    DIPAlgorithm(void);
    ~DIPAlgorithm(void);

    void calibrateImage( IplImage *iplImg );

    /*! 
     * Color detection
     */
    void Scribbling(IplImage *frame);
    IplImage* GetThresholdedImage(IplImage* img);
    // Four points distance detection
    CvPoint catchColorPoint(IplImage* img, CvScalar lower, CvScalar upper);
    void showFourColorPoints(IplImage* img);
    /*! 
    * Corner detection
    */
    void cornerGFTT(IplImage* img, int &count, CvPoint2D32f* corners=NULL, CvArr *mask=NULL);
    void cornerSubPixel(IplImage* img);
    // Hough Circles
    void trackHoughCircles(IplImage* img, CvSeq** results=NULL);
    // Color tracking + GFTT
    void cornerColorGFTT(IplImage* img);
    // Hough Circles + GFTT
    void cornerHoughGFTT(IplImage* img);
    // Draw convex hull of points
    int drawConvexHull(IplImage* img, CvPoint2D32f *vertex, int count);

    // Door Detection
    void doorDetection(IplImage *frame_come);

    bool getViewpoint(IplImage *capturedImage, double **xyz, int numLandmark );

private:
    // This image holds the "scribble" data...
    // the tracked positions of the ball
    IplImage* imgScribble;

    SiftRansac *sr;

    //!!! These 2 can be merged !!!
    // Viewpoint from extrinsics
    CameraParameters *NaoCam;
    // Camera parameters
    CvMat* intrinsics;
    CvMat* distCoeffs;
    cv::Mat camParaMat;
    cv::Mat distCoeffMat;

    //////////////////////////////////////////////////////////////////////////
    // Create the objects matrix
    CvMat *objPoints;
    //////////////////////////////////////////////////////////////////////////
};


#endif  //#ifndef DIP_ALGORITHM
