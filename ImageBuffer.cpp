#include "StdAfx.h"
#include "DataManager.h"
#include <highgui.h>
#include "ImageBuffer.h"

using namespace cv;

/*! 
 *  Semaphores for synchronously access to ImageBuffer
 *   of Capture Thread and Process Thread.
 *  It seems that QSemaphore cannot be a class member.
 */
QSemaphore availableFrames;

ImageBuffer::ImageBuffer()
    : QObject()
    , mode(false)
{

}

ImageBuffer::~ImageBuffer()
{
    // Clean up
    while(imgIplBuffer.size())
    {
        IplImage* iplImg = imgIplBuffer.dequeue();
        cvReleaseImage(&iplImg);
    }
}

int ImageBuffer::addIplImage(IplImage* iplImg)
{
    /*!
     *  In mono-frame mode, insert data only when buffer is empty!
     */
    if (mode)
    {
        if (!imgIplBuffer.isEmpty())
        {
            return -1;
        }
    }

	/*!
     *  Fully copy the input IplImage, which will allocate a memory.
     */
	IplImage* temp = cvCloneImage(iplImg);
	/*!
     *  Add image to queue
     *  !!NOTICE!! The datum stored in Queue are actually the pointers.
     */
	imgIplBuffer.enqueue(temp);

    // QSemaphore
    availableFrames.release();
	
    int s = imgIplBuffer.size();
	qDebug("New image added - size: %d", s);

    emit queueSize(s);

    return s;
}

bool ImageBuffer::addMatImage(const Mat mImg)
{
    imgMatBuffer.enqueue(mImg);

    qDebug("New image added - size: %d", imgMatBuffer.size());

    return 0;
}

IplImage* ImageBuffer::getIplImage()
{
    // QSemaphore
    availableFrames.acquire();

    IplImage* img = NULL;

	qDebug("Before get image - size: %d", imgIplBuffer.size());
	
	if (!imgIplBuffer.isEmpty())
	{
	    img = imgIplBuffer.dequeue();
        //qDebug() << "-----" << img;
	}
	
    return img;
}

Mat ImageBuffer::getMatImage()
{
    Mat img = imgMatBuffer.dequeue();

    return img;
}

void ImageBuffer::setMode(bool md)
{
    mode = md;
}

bool ImageBuffer::getMode()
{
    return mode;
}