#include "stdafx.h"
#include "CameraCalibration.h"
#include "DataManager.h"
#include "ImageFormat.h"

#include "urbi\ubinary.hh"
#include "urbi\uimage.hh"
#include "urbi\uconversion.hh"
#include "CamCaliSampling.h"

#include "opencv/cv.hpp"
#include "opencv/highgui.h"

using namespace cv;
using namespace urbi;

CamCaliSampling::CamCaliSampling(QObject *parent)
    : QThread(parent)
    , pDataMgr(NULL)
    , pCCWindow(NULL)
    , imgPts(NULL)
    , objPts(NULL)
    , cntPts(NULL)
    , intrinsicMatrix(NULL)
    , distortionCoeffs(NULL)
    , imgW(0)
    , imgH(0)
    , numberCross(0)
    , numberCrossInWidth(0)
    , numberCrossInHeight(0)
    , numCaliView(0)
    , successes(0)
    , flagCamCaliThread(0)
    , sectionSampling(0)
    , sectionTest(0)
{
    goodView = new QSemaphore(0);

    intrinsicMatrix  = cvCreateMat( 3, 3, CV_64FC1 );
    distortionCoeffs = cvCreateMat( 5, 1, CV_64FC1 );
}

CamCaliSampling::~CamCaliSampling()
{
    flagCamCaliThread = 0;
    quit();
    wait();

    delete goodView;

    cvReleaseMat(&intrinsicMatrix);
    cvReleaseMat(&distortionCoeffs);
}

void CamCaliSampling::setDataManager(DataManager *pDM)
{
    pDataMgr = pDM;
}

void CamCaliSampling::setCamCaliWindow(CameraCalibration *cc)
{
    pCCWindow = cc;
}

IplImage* CamCaliSampling::getCalibrationFrame()
{
    // Fetch a raw frame
    UBinary *b_img = NULL;
    IplImage* iplImg = NULL;

    b_img = pDataMgr->fetchRawFrame();

    // If succeed to fetch
    if ( b_img != NULL )
    {
        //qDebug() << "Read out image ready!";

        UImage *rawUImg = &(b_img->image);

        // Convert to IplImage for processing
        iplImg = compressedUImage2IplImage(rawUImg);
    }

    return iplImg;
}

void CamCaliSampling::start( Priority priority )
{
    flagCamCaliThread = 1;

    QThread::start(priority);
}


void CamCaliSampling::run()
{
    // Overall loop
    while ( flagCamCaliThread )
    {
        // Sampling section
        if ( sectionSampling )
        {
	        CvSize totalCross = cvSize( numberCrossInWidth, numberCrossInHeight );
	
	        CvPoint2D32f* crosses = new CvPoint2D32f[ numberCross ];
	        int cntCorsses;
	
	        int step = 0;
	        int frame = 0;
	        successes = 0;
	
	        // Read out the first video frame
	        IplImage *image = NULL;
	        while ( image == NULL )
	        {
	            image = getCalibrationFrame();
	        }
	
	        // Get the size of the image for further use
	        CvSize sizeImg = cvGetSize( image );
	        imgW = sizeImg.width;
	        imgH = sizeImg.height;
	
	        // For subpixel
	        IplImage *grayImg = cvCreateImage(sizeImg,8,1); 
	
	        // CAPTURE CORNER VIEWS LOOP UNTIL WE¡¯VE GOT "NUMBER_CALIBRATION_VIEW"
	        // SUCCESSFUL CAPTURES (ALL CORNERS ON THE BOARD ARE FOUND)
	        //
	        do
	        {
	            // Find chessboard corners
	            int found = cvFindChessboardCorners(
	                image, totalCross, crosses, &cntCorsses,
	                CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
	
	            qDebug() << "Found:" << found;
	
	            // If total number of crosses were found 
	            if ( found )
	            {
	                // Get Subpixel accuracy on those corners
	                cvCvtColor(image, grayImg, CV_BGR2GRAY);
	                cvFindCornerSubPix( grayImg, crosses, cntCorsses, cvSize(11,11), cvSize(-1,-1),
	                    cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ) );
	
	                // Store for save to file
	                IplImage *imgToSave = cvCloneImage(image);
	
	                // Draw the "subpixeled" crosses
	                cvDrawChessboardCorners(image, totalCross, crosses, cntCorsses, found);
	
	                // Deep copy before sending
	                IplImage *rawDispImg = cvCloneImage(image);
	                Q_EMIT sigRawFrame(rawDispImg);
	
	                // Ask for user input if it is a good view
	                bool gv = goodView->tryAcquire(1);
	                if ( gv )
	                {
	                    // If we got a good board, add it to our data
                        // This "if" is optional
	                    if( cntCorsses == numberCross ) 
	                    {
	                        step = successes*numberCross;
	                        for( int i=step, j=0; j<numberCross; ++i,++j ) 
	                        {
	                            CV_MAT_ELEM( *imgPts, double, i, 0 ) = crosses[j].x;
	                            CV_MAT_ELEM( *imgPts, double, i, 1 ) = crosses[j].y;
	                            CV_MAT_ELEM( *objPts, double, i, 0 ) = j/numberCrossInWidth;
	                            CV_MAT_ELEM( *objPts, double, i, 1 ) = j%numberCrossInWidth;
	                            CV_MAT_ELEM( *objPts, double, i, 2 ) = 0.0f;
	                        }
	                        CV_MAT_ELEM(*cntPts, int, successes,0) = numberCross;
	                        successes++;
	
	                        QString filename = QString("calibration/%1.jpg").arg(successes);
	                        cvSaveImage( filename.toStdString().data(), imgToSave );
	                        cvReleaseImage(&imgToSave);
	
	                        // UI display
	                        Q_EMIT sigCaliProgress(successes);
	                    }
	                    else
	                    {
	                        qDebug() << "Impossible to reach here!";
	                    }
	                }  // if ( gv )
	
	            } // if ( found )
	            else
	            {
	                // Draw crosses found
	                cvDrawChessboardCorners(image, totalCross, crosses, cntCorsses, found);
	
	                // Deep copy before sending
	                IplImage *rawDispImg = cvCloneImage(image);
	                Q_EMIT sigRawFrame(rawDispImg);
	            }
	
	            cvReleaseImage(&image);
	            image = NULL;
	            //Get next image
	            while ( image == NULL )
	            {
	                image = getCalibrationFrame();
	            }
	        } while ( successes < numCaliView );
	
	        cvReleaseImage(&image);
	        cvReleaseImage(&grayImg);
            delete crosses;

            Q_EMIT sigSamplingFinished();
            sectionSampling = 0;
        } 

        // Test section
        else if ( sectionTest )
        {
            // EXAMPLE OF LOADING THESE MATRICES BACK IN:
            CvMat *intrinsic = (CvMat*)cvLoad("calibration/NaoCamIntrinsics.xml");
            CvMat *distortion = (CvMat*)cvLoad("calibration/NaoCamDistortion.xml");

            // Get image for test
            IplImage *image = NULL;
            while ( image == NULL )
            {
                image = getCalibrationFrame();
            }

            // Build the undistort map that we will use for all
            // subsequent frames.
            //
            IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
            IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
            cvInitUndistortMap(
                intrinsic,
                distortion,
                mapx,
                mapy
                );

            // Just run the camera to the screen, now showing the raw and
            // the undistorted image.
            //
            do 
            {
                // Show raw image
                // Deep copy before sending
                IplImage *rawDispImg = cvCloneImage(image);
                Q_EMIT sigRawFrame(rawDispImg);

                IplImage *t = cvCloneImage(image);
                cvRemap( t, image, mapx, mapy ); // Undistort image
                cvReleaseImage(&t);

                // Show corrected image
                // Deep copy before sending
                IplImage *undistortedDispImg = cvCloneImage(image);
                Q_EMIT sigUndistortedFrame(undistortedDispImg);

                cvReleaseImage(&image);
                image = NULL;
                //Get next image
                while ( image == NULL )
                {
                    image = getCalibrationFrame();
                }
            } while( sectionTest );

            cvReleaseImage(&image);
            cvReleaseImage(&mapx);
            cvReleaseImage(&mapy);
            cvReleaseMat(&intrinsic);
            cvReleaseMat(&distortion);

            qDebug() << "Test over!";

            sectionTest = 0;
        }
    }

    return;
}

void CamCaliSampling::sampling()
{
    numberCross = numberCrossInWidth * numberCrossInHeight;

    // ALLOCATE STORAGE
    imgPts = cvCreateMat( numCaliView * numberCross, 2, CV_64FC1 );
    objPts = cvCreateMat( numCaliView * numberCross, 3, CV_64FC1 );
    cntPts = cvCreateMat( numCaliView, 1, CV_32SC1 );

    flagCamCaliThread = 1;
    sectionTest = 0;
    sectionSampling = 1;

    return;
}

void CamCaliSampling::calibration()
{
    // ALLOCATE MATRICES ACCORDING TO HOW MANY CHESSBOARDS FOUND
    // The sampling process could be terminated halfway!
    CvMat* fndObjPts = cvCreateMat( successes*numberCross, 3, CV_64FC1 );
    CvMat* fndImgPts = cvCreateMat( successes*numberCross, 2, CV_64FC1 );
    CvMat* fndCntPts = cvCreateMat( successes, 1, CV_32SC1 );

    /* 
     * TRANSFER THE POINTS INTO THE CORRECT SIZE MATRICES
     * Below, we write out the details in the next two loops. 
     * We could instead have written:
     *   imgPts->rows = objPts->rows = successes * numberCross; 
     *   cntPts->rows = successes;
     */
    for(int i = 0; i<successes*numberCross; ++i) 
    {
        CV_MAT_ELEM( *fndImgPts, double, i, 0 ) = CV_MAT_ELEM( *imgPts, double, i, 0 );
        CV_MAT_ELEM( *fndImgPts, double, i, 1 ) = CV_MAT_ELEM( *imgPts, double, i, 1 );
        CV_MAT_ELEM( *fndObjPts, double, i, 0 ) = CV_MAT_ELEM( *objPts, double, i, 0 );
        CV_MAT_ELEM( *fndObjPts, double, i, 1 ) = CV_MAT_ELEM( *objPts, double, i, 1 );
        CV_MAT_ELEM( *fndObjPts, double, i, 2 ) = CV_MAT_ELEM( *objPts, double, i, 2 );
    }

    for(int i=0; i<successes; ++i)
    {
        // These are all the same number
        CV_MAT_ELEM( *fndCntPts, int, i, 0 ) = CV_MAT_ELEM( *cntPts, int, i, 0 );
    }

    cvReleaseMat(&objPts);
    cvReleaseMat(&imgPts);
    cvReleaseMat(&cntPts);

    // At this point we have all of the chessboard corners we need.
    // Initialize the intrinsic matrix such that the two focal
    // lengths have a ratio of 1.0
    //
    CV_MAT_ELEM( *intrinsicMatrix, double, 0, 0 ) = 1.0f;
    CV_MAT_ELEM( *intrinsicMatrix, double, 1, 1 ) = 1.0f;

    // CALIBRATE THE CAMERA!
    cvCalibrateCamera2(
        fndObjPts, fndImgPts,
        fndCntPts, cvSize( imgW, imgH ),
        intrinsicMatrix, distortionCoeffs,
        NULL, NULL,0 //CV_CALIB_FIX_ASPECT_RATIO
        );

    cvReleaseMat(&fndObjPts);
    cvReleaseMat(&fndImgPts);
    cvReleaseMat(&fndCntPts);

    // SAVE THE INTRINSICS AND DISTORTIONS
    cvSave("calibration/NaoCamIntrinsics.xml", intrinsicMatrix);
    cvSave("calibration/NaoCamDistortion.xml", distortionCoeffs);

    Q_EMIT sigCalibrationFinished(intrinsicMatrix, distortionCoeffs);

    return;
}

void CamCaliSampling::testCaliResult(bool b)
{
    if (b)
    {
        flagCamCaliThread = 1;
        sectionSampling = 0;
        sectionTest = 1;
    } 
    else
    {
        sectionTest = 0;
    }

    return;
}

void CamCaliSampling::acceptGoodView()
{
    if ( !goodView->available() )
    { 
        goodView->release(1);
    }

    return;
}