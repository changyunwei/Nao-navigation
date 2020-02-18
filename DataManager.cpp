#include "stdafx.h"

#include <urbi/uimage.hh>
#include <urbi/ubinary.hh>

#define QT_NO_DEBUG_OUTPUT
#include <QDebug>
#include "DataManager.h"
#include "ImageFormat.h"

#include "opencv/cv.hpp"
#include "opencv/highgui.h"

using namespace urbi;

DataManager::DataManager(void)
{
    // Create the buffer for sensor values
    JSValue[0].set_capacity(BUF_LEN);
    JSValue[1].set_capacity(BUF_LEN);

    // Create the buffer for raw video frames
    rawVideoFrame.set_capacity(BUF_LEN);

    // Create the buffer for processed video frames
    procVideoFrame.set_capacity(BUF_LEN);
}

DataManager::~DataManager(void)
{
    // deallocate the memory
    int i=0;
    for ( i=0; i<rawVideoFrame.size(); i++ )
    {
        UBinary* ub = rawVideoFrame[i];
        ub->clear();
        
    }
    rawVideoFrame.clear();
    for( i=0; i<procVideoFrame.size(); i++ )
    {
        IplImage* iplimg = procVideoFrame[0];
        cvReleaseImage(&iplimg);
    }
    procVideoFrame.clear();
}

bool DataManager::storeRawFrame(UBinary &inFrame)
{
    if (!rawVideoFrame.full())
    {
        // Deep copy
        UBinary *b_img = new UBinary(inFrame);

	    rawVideoFrame.push_back(b_img);

        //qDebug() << "New frame! Current size: " << rawVideoFrame.size();
	
	    return true;
    } 
    else
    {
        //qDebug() << "Raw frame buffer full!";
        return false;
    }
}

UBinary* DataManager::fetchRawFrame()
{   
    if (!rawVideoFrame.empty())
    {
        // Get the front value
        UBinary* b_img = rawVideoFrame[0];

        // For debug
        //UBinary* b_img1 = rawVideoFrame[1];
        //UBinary* b_img2 = rawVideoFrame[2];
        //UBinary* b_img3 = rawVideoFrame[3];

        rawVideoFrame.pop_front();

        //qDebug() << "Get a raw frame! Current size: " << rawVideoFrame.size();

        return b_img;
    }
    else
    {
        //qDebug() << "Raw frame buffer Empty!";
        return NULL;
    }
}

UBinary* DataManager::fetchLatestRawFrame()
{   
    if (!rawVideoFrame.empty())
    {
        // Get the front value
        UBinary* b_img = rawVideoFrame.back();

        //qDebug() << "Get a raw frame! Current size: " << rawVideoFrame.size();

        return b_img;
    }
    else
    {
        //qDebug() << "Raw frame buffer Empty!";
        return NULL;
    }
}

IplImage* DataManager::fetchLatestIplFrame()
{
    UBinary *b_img = fetchLatestRawFrame();
    IplImage* iplImg = NULL;
    if ( b_img )
    {
        // Deep Copy
        UImage *rawUImgDisp = &(b_img->image);

        // Convert to IplImage for processing
        iplImg = compressedUImage2IplImage(rawUImgDisp);
    }
    return iplImg;
}

bool DataManager::storeProcessedFrame(IplImage *inFrame)
{
    if (!rawVideoFrame.full())
    {
        IplImage *iplImg = cvCloneImage(inFrame);

        procVideoFrame.push_back(iplImg);

        // For debug
        //if ( procVideoFrame.size() >= 8 )
        //{
        //    IplImage* procImg1 = procVideoFrame[1];
        //    IplImage* procImg2 = procVideoFrame[2];
        //    IplImage* procImg3 = procVideoFrame[3];
        //    IplImage* procImg4 = procVideoFrame[4];
        //    IplImage* procImg5 = procVideoFrame[5];
        //    IplImage* procImg6 = procVideoFrame[6];
        //}

        //qDebug() << "New frame! Current size: " << procVideoFrame.size();

        return true;
    } 
    else
    {
        //qDebug() << "Processed frame buffer full!";
        return false;
    }
}

IplImage* DataManager::fetchProcessedFrame()
{    
    if (!procVideoFrame.empty())
    {
        // Get the front value
        IplImage* procImg = procVideoFrame[0];

        // For debug
        //if ( procVideoFrame.size() >= 5 )
        //{
	       // IplImage* procImg1 = procVideoFrame[1];
	       // IplImage* procImg2 = procVideoFrame[2];
	       // IplImage* procImg3 = procVideoFrame[3];
        //}

        procVideoFrame.pop_front();

        //qDebug() << "Get a processed frame! Current size: " << procVideoFrame.size();

        return procImg;
    }
    else
    {
        //qDebug() << "Processed frame buffer Empty!";
        return NULL;
    }
}



bool DataManager::storeValue(int num, double val)
{
    // store value at the back, if not full
    if (!JSValue[num-1].full())
    {
        JSValue[num-1].push_back(val);
        
        // send signal to GUI to display
        //Q_EMIT vdValueReady(num);

        return 0;
    }
    else
    {
        qDebug("circular_buffer %d full!\n", num);
        return 1;
    }
}

double DataManager::fetchValue(int num)
{
    // Get the front value
    double JSVal = JSValue[num-1][0];
    if (!JSValue[num-1].empty())
    {
        JSValue[num-1].pop_front();
        return JSVal;
    }
    else
    {
        qDebug("circular_buffer %d empty!\n", num);
        return 0;
    }
}


void DataManager::saveLatestFrame()
{
    IplImage *iplImg = fetchLatestIplFrame();
    
    cvSaveImage("savedFrames/sample.jpg", iplImg);

    return;
}
