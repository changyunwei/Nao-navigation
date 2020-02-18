#include "StdAfx.h"

#include "UrbiMessenger.h"
#include "DataManager.h"
#include "ImageBuffer.h"
#include "ImageProcessor.h"

#include "cv.h"

#include "Conversion2QImage.h"
#include "videoAgent.h"


/*! 
 *  Semaphores for synchronously access to ImageBuffer
 *   of Capture Thread and Process Thread.
 *  It seems that QSemaphore cannot be a class member.
 */
//QSemaphore availableFrames;

VideoAgent::VideoAgent()
    : QObject()
{
    // Store processed image
    imgBuffer = new ImageBuffer();
    // Store raw UImage
    DataMgr = new DataManager();

    UMsger = new UrbiMessenger(DataMgr);
    imgProcessor = new ImageProcessor(imgBuffer, DataMgr);

    //connect(&DataMgr, SIGNAL(vdFrameReady(unsigned short)), &imgProcessor, SLOT(Process(unsigned short)));
}

VideoAgent::~VideoAgent()
{
    // Clean up
    delete imgProcessor;
    delete UMsger;
    delete imgBuffer;
    delete DataMgr;
    /*!
     * Warning: Deleting a QObject while pending events are waiting to be delivered can cause a crash. 
     *  You must not delete the QObject directly if it exists in a different thread 
     *   than the one currently executing. 
     *  Use deleteLater() instead, which will cause the event loop to delete the object 
     *   after all pending events have been delivered to it.
     *  !!Note!! that entering and leaving a new event loop (e.g., by opening a modal dialog) 
     *   will not perform the deferred deletion; for the object to be deleted, 
     *   the control must return to the event loop from which deleteLater() was called.
     *  Note: It is safe to call this function more than once; 
     *   when the first deferred deletion event is delivered, 
     *   any pending events for the object are removed from the event queue.
     */
    //imgProcessor->deleteLater();
    //camCapAgent->deleteLater();
}

QImage VideoAgent::getImage()
{
    IplImage* img = imgBuffer->getIplImage();
    QImage qImg = Ipl2QImage(img);

    return qImg;
}