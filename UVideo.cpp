#include "stdafx.h"
#include "DataManager.h"
#include "UVideo.h"

using namespace urbi;

/*static*/ UVideo* UVideo::gpUVideo = NULL;

UStart(UVideo);

UVideo::UVideo(const std::string& s)
    : UObject(s)
    , QObject(0)
    , imageSrc(NULL)
{
    /*
     * The constructor only registers init, 
     *  so that our default instance servo does nothing, 
     *  and can only be used to create new instances.
     */
    UBindFunction(UVideo, init);

}

UVideo::~UVideo(void)
{
    delete imageSrc;
}

int UVideo::init()
{
    UBindFunction(UVideo, setImageSrc);
    qDebug() << "UVideo bind setImageSrc.";

    //Q_EMIT 
    gpUVideo = this;

    return 0;
}

void UVideo::setDataStorage(DataManager *pdm)
{
    if ( pdm != NULL )
    {
        pDataStorage = pdm;
    }
    else
    {
        qDebug() << "Invalid Data Storage Handle!";
    }

    return;
}

void UVideo::subscribeImage(bool b)
{
    if ( b )
    {
		UVideo::send("camera.load=1;");
        //UNotifyChange( *imageSrc, &USpy::onImage );

        //qDebug() << "Set Image Notification.";
    }
    else
    {
		UVideo::send("camera.load=0;");
        //imageSrc->unnotify();
        //qDebug() << "Cancel Image Notification.";
    }
}

void UVideo::setImageSrc(UVar& image)
{
    if( imageSrc )
        imageSrc->unnotify();

    // Store the image source
    imageSrc = new UVar(image);
    imageSrc->useRTP(true);
    qDebug() << "Image source set!";

    UNotifyChange( *imageSrc, &UVideo::onImage );
    qDebug() << "Set Image Notification.";
}

void UVideo::onImage(UVar& img)
{
    UBinary b_img = img;

    // Store image frame in data storage
    pDataStorage->storeRawFrame(b_img);
}