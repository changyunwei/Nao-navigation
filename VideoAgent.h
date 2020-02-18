#pragma once
#ifndef VIDEOAGENT_H
#define VIDEOAGENT_H

#include <QtGui>
#include <QObject>

class UrbiMessenger;
class DataManager;
class ImageProcessor;
class ImageBuffer;

class VideoAgent : public QObject
{
    Q_OBJECT

public:
    VideoAgent();
    ~VideoAgent();

    QImage getImage();

public:
    // UClient Wrapper
    UrbiMessenger *UMsger;

    // Store raw UImage
    DataManager *DataMgr;

    // Process
    ImageProcessor *imgProcessor;

    // Store processed image
    ImageBuffer *imgBuffer;
};

#endif // VIDEOAGENT_H
