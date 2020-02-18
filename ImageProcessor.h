#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QtGui>
#include <QThread>


class DIPAlgorithm;
class DataManager;
typedef struct _IplImage IplImage;
namespace urbi
{
    class UImage;
}


class ImageProcessor : public QThread
{
    Q_OBJECT

public:
    //ImageProcessor(/*QObject *parent*/);
    ImageProcessor(DataManager *pDataMgr=NULL);
    ~ImageProcessor();

    // Thread operation
    void run();

    // Execute a set of filters
    void ImageProcessing(IplImage *frm);

    // 
    void setProcessing(bool en);

    bool calculateViewpoint(int numLandmark, double **xyz);

    /************************************************************************/
    /* Basic DIP                                                            */
    /************************************************************************/
    // Invert color of the image
    void invert(IplImage *img);
    // Change brightness of the image
    void brightness(IplImage *img, int ampR, int ampG, int ampB);
    // Change contrast of the image
    void contrast(IplImage *img, float alpha);

public Q_SLOTS:
    // Overloaded function
    void start ( Priority priority = InheritPriority );

    // Receive signal to end this QThread
    void stopThread();

    // Set sleep time of the thread
    void setInterval(int slpT);

    // Receive signals from UI to set RGB brightness
    void setRBrightness(int);
    void setGBrightness(int);
    void setBBrightness(int);
    // Receive signal from UI to set contrast (alpha)

    void setAlpha(float a);

    void faceFeature(IplImage *img);

    /*!
    *  Filters Slots
    */
    void enableInvert(bool isEnable);
    void enableDilate(bool isEnable);
    void enableErode(bool isEnable);
    void enableScribble(bool isEnable);
    void enable4ColorPoints(bool isEnable);
    void enableGFTT(bool isEnable);
    void enableSubPixel(bool isEnable);
    void enable4PHoughCircles(bool isEnable);
    void enable4PColorGFTT(bool isEnable);
    void enable4PHoughGFTT(bool isEnable);
    void enableDoorDetection(bool isEnable);

Q_SIGNALS:
    // Send signals to UI to display raw/original images
    void sigDispRawFrame(urbi::UImage*);
    // Send signals to UI to display processed images
    void sigDispProcessedFrame(IplImage*);

    void sigFrameForCalibration(IplImage*);

    void sigProcInterval(int);

private:
    // Pointer to the external image storage
    DataManager *pDataManager;

    // 
    DIPAlgorithm *dipAlgorithm;

    // define the image getting fps
    int sleepTime;

    // Control flag of this QThread
    bool flagThread;

    //
    bool enProcess;

    // Store RGB brightness and contrast values
    int ampRBrightness;
    int ampGBrightness;
    int ampBBrightness;
    float alpha;

    // Filters enable
    bool enInvert;
    bool enFltrErode;
    bool enFltrDilate;
    bool enFltrScribble;
    bool enFltr4ColorPoints;
    bool enFltrGFTT;
    bool enFltrSubPixel;
    bool enFltr4PHoughCircles;
    bool enFltrColorGFTT;
    bool enFltrHoughGFTT;
    bool enDoorDetection;

public:
    // Face position
    float faceX;
    float faceY;
    float faceZ;
    
};

#endif // IMAGEPROCESSOR_H
