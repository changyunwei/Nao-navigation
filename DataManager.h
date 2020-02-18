#pragma once
#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "boost/circular_buffer.hpp"
//#include <QObject>

/*
namespace boost
{
    template <class T, class Alloc = BOOST_CB_DEFAULT_ALLOCATOR(T)>
    class circular_buffer;
}
*/
namespace urbi
{
    class UBinary;
}
typedef struct _IplImage IplImage;



#define BUF_LEN 50

class DataManager /*: public QObject*/
{
    /*Q_OBJECT*/

private:
    // Raw frames
    // Only store the pointers, DONT'T store the data.
    boost::circular_buffer<urbi::UBinary*> rawVideoFrame;

    // Frames processed
    // Only store the pointers, DONT'T store the data.
    boost::circular_buffer<IplImage*> procVideoFrame;

    // Circular buffers for joint/sensor values
    // 0 for left sonar
    // 1 for right sonar
    boost::circular_buffer<double> JSValue[10];

public:
    DataManager(void);
    ~DataManager(void);

    /*!
     *  must refer to input frame, otherwise no deep copy
     */
    // Raw frames operation
    bool storeRawFrame(urbi::UBinary &inFrame);
    urbi::UBinary* fetchRawFrame();
    urbi::UBinary* fetchLatestRawFrame();
    IplImage* fetchLatestIplFrame();

    // Processed frames operation
    bool storeProcessedFrame(IplImage *inFrame);
    IplImage* fetchProcessedFrame();

    // num: notation for sensor/joint
    // 1: left sonar
    // 2: right sonar
    bool storeValue(int num, double val);
    double fetchValue(int num);

    void saveLatestFrame();
};

#endif