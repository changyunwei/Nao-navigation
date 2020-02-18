#include "StdAfx.h"



#include "ImageFormat.h"

#include "DataManager.h"
#include "myCVFunctions.h"
#include "DIPAlgorithm.h"
#include "ImageProcessor.h"

#include "urbi\ubinary.hh"
#include "urbi\uimage.hh"

#include "opencv/cv.hpp"
#include "opencv/highgui.h"
 

using namespace cv;
using namespace urbi;

ImageProcessor::ImageProcessor(DataManager *pDataMgr)
    : QThread(/*parent*/)
    , pDataManager(pDataMgr)
    , flagThread(1)
    , enProcess(0)
    , sleepTime(0)
    , ampRBrightness(0)
    , ampGBrightness(0)
    , ampBBrightness(0)
    , faceX(0)
    , faceY(0)
    , faceZ(0)
    , alpha(1)
    , enInvert(false)
    , enFltrErode(false)
    , enFltrDilate(false)
    , enFltrScribble(false)
    , enFltr4ColorPoints(false)
    , enFltrGFTT(false)
    , enFltrSubPixel(false)
    , enFltr4PHoughCircles(false)
    , enFltrColorGFTT(false)
    , enFltrHoughGFTT(false)
    , enDoorDetection(false)
{
    dipAlgorithm = new DIPAlgorithm();

 
}

ImageProcessor::~ImageProcessor()
{
    // make the thread return
    flagThread = 0;
    quit();
    wait();

    delete dipAlgorithm;

}

void ImageProcessor::stopThread()
{
    flagThread = 0;
    quit();
}

void ImageProcessor::start ( Priority priority )
{
    flagThread = 1;
    QThread::start();
}

void ImageProcessor::setInterval(int slpT)
{
    sleepTime = slpT;
    return;
}

void ImageProcessor::setProcessing(bool en)
{
    enProcess = en;
	return;
}

void ImageProcessor::run()
{
    qDebug() << "About to process!";

    while(flagThread)
    {
        // Fetch a raw frame
        UBinary *b_img = NULL;
        b_img = pDataManager->fetchRawFrame();

        // If succeed to fetch
        if ( b_img != NULL )
        {

			UImage *rawUImg = &(b_img->image);

			// if put the processed button
			if ( enProcess )
            {
                // Convert to IplImage for processing
                IplImage* iplImg = NULL;
                iplImg = compressedUImage2IplImage(rawUImg);

                /*
                 * Qt::QImage routine to decompression
                 */
                //QImage qImg;
                //bool qimgerr = qImg.loadFromData((uchar*)rawUImg->data, rawUImg->size, "JPEG");
                //QImage qqimg = qImg.rgbSwapped();
                //IplImage* iplImg = QImage2Ipl(qqimg);

                // Succeed to convert
                if (iplImg != NULL)
                {
                    /*!
                    *  Filters to implement real-time processing
                    */
                    ImageProcessing(iplImg);

                    // Store processed images into DataManager
                    pDataManager->storeProcessedFrame(iplImg);

                    // Show processing frame rate on the image
                   // showText(iplImg, "fps: ", procFps, 500, 470); 

                    // Send signals to UI to display processed images
                    Q_EMIT sigDispProcessedFrame(iplImg);
                }  
            }
            // if want to show the raw image
			else
			{
				// Emit raw image to display
				Q_EMIT sigDispRawFrame(rawUImg);
			}
        }  

        // default fps 25
        msleep(sleepTime);
    }  
    qDebug() << "Image Processing has stopped!";
    exec();
    return;
}

/*!
 *  Filters selection
 */
void ImageProcessor::ImageProcessing(IplImage *frame)
{
	// dipAlgorithm->getVPFE(frame);
	//faceFeature(frame_come);
	/*
	dipAlgorithm->brightness(frame, ampRBrightness, ampGBrightness, ampBBrightness);
	dipAlgorithm->contrast(frame, alpha);



	if (enInvert)
	invert(frame);
	if (enFltrErode)
	cvErode(frame, frame, 0, 2);
	if (enFltrDilate)
	cvDilate(frame, frame, 0, 2);
	if (enFltrScribble)
	dipAlgorithm->Scribbling(frame);
	if (enFltr4ColorPoints)
	dipAlgorithm->showFourColorPoints(frame);
	if (enFltrGFTT)
	{
	int count = 100;
	dipAlgorithm->cornerGFTT(frame, count);
	}
	if (enFltrSubPixel)
	dipAlgorithm->cornerSubPixel(frame);
	if (enFltr4PHoughCircles)
	dipAlgorithm->trackHoughCircles(frame);
	if (enFltrColorGFTT)
	dipAlgorithm->cornerColorGFTT(frame);
	if (enFltrHoughGFTT)
	dipAlgorithm->cornerHoughGFTT(frame);
    if (enDoorDetection)
    dipAlgorithm->doorDetection(frame);
	*/

	return;
}



void ImageProcessor::faceFeature(IplImage *img)
{
    cvDrawRect(img, cvPoint(faceY-10, faceZ-10), cvPoint(faceY+10, faceZ+10), cvScalar(255,0,0));
}




/*!
 *  Brightness
 */
void ImageProcessor::brightness(IplImage *img, int ampR, int ampG, int ampB)
{
    // B, G, R
    cvAddS(img, cvScalar(ampB,ampG,ampR), img);
    return;
}
void ImageProcessor::setRBrightness(int R)
{
    ampRBrightness = R;
    return;
}
void ImageProcessor::setGBrightness(int G)
{
    ampGBrightness = G;
    return;
}
void ImageProcessor::setBBrightness(int B)
{
    ampBBrightness = B;
    return;
}

/*!
 *  Contrast
 */
void ImageProcessor::contrast(IplImage *img, float alpha)
{
    //
    cvScale(img, img, alpha);
    return;
}

void ImageProcessor::setAlpha(float a)
{
    alpha = a;
    return;
}

/*!
 *  Invert
 */
void ImageProcessor::invert(IplImage *img)
{  
    cvNot(img, img);
    return;
}





void ImageProcessor::enableInvert(bool isEnable)
{
    enInvert = isEnable;
    return;
}

void ImageProcessor::enableDilate(bool isEnable)
{
    enFltrDilate = isEnable;
    return;
}

void ImageProcessor::enableErode(bool isEnable)
{
    enFltrErode = isEnable;
    return;
}

void ImageProcessor::enableScribble(bool isEnable)
{
    enFltrScribble = isEnable;
    return;
}

void ImageProcessor::enable4ColorPoints(bool isEnable)
{
    enFltr4ColorPoints = isEnable;
    return;
}

void ImageProcessor::enableGFTT(bool isEnable)
{
    enFltrGFTT = isEnable;
    return;
}

void ImageProcessor::enableSubPixel(bool isEnable)
{
    enFltrSubPixel = isEnable;
    return;
}

void ImageProcessor::enable4PHoughCircles(bool isEnable)
{
    enFltr4PHoughCircles = isEnable;
    return;
}

void ImageProcessor::enable4PColorGFTT(bool isEnable)
{
    enFltrColorGFTT = isEnable;
    return;
}

void ImageProcessor::enable4PHoughGFTT(bool isEnable)
{
    enFltrHoughGFTT = isEnable;
    return;
}

void ImageProcessor::enableDoorDetection(bool isEnable)
{
    enDoorDetection = isEnable;
    return;
}


bool ImageProcessor::calculateViewpoint(int numLandmark, double **xyz)
{
    bool b = 1;
    //////////////////////////////////////////////////////////////////////////
    IplImage *iplImg = pDataManager->fetchLatestIplFrame();
    if ( iplImg )
    {     
        // Undistort the image
        dipAlgorithm->calibrateImage(iplImg);

        //////////////////////////////////////////////////////////////////////////
        b = dipAlgorithm->getViewpoint(iplImg, xyz, numLandmark);
        //////////////////////////////////////////////////////////////////////////

        IplImage *iplImgDisp = cvCloneImage(iplImg);
        Q_EMIT sigDispProcessedFrame(iplImgDisp);
        cvReleaseImage(&iplImg);
    }

    return b;
}