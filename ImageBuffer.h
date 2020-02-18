#pragma once
#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QtGui>
#include <QObject>

typedef struct _IplImage IplImage;
namespace cv
{
    class Mat;
}

class ImageBuffer : public QObject
{
    Q_OBJECT

public:
    ImageBuffer();
    ~ImageBuffer();

    // C type
    IplImage* getIplImage();
    // C++ type
    cv::Mat getMatImage();
    // C type
    int addIplImage(IplImage*);
    // C++ type
    bool addMatImage(const cv::Mat);

    bool getMode();

public slots:
    void setMode(bool md);

signals:
    void queueSize(int);

private:
    // Storage for frames
    // C type
    QQueue<IplImage*> imgIplBuffer;
    // C++ type
    QQueue<cv::Mat> imgMatBuffer;

    /*!
     *  For real-time processing
     *  Queue mode or single frame mode
     *   This guarantee only one frame is 
     *   stored in the buffer.
     */
    bool mode;
};

#endif // IMAGEBUFFER_H
