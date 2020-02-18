#ifndef CAMCALISAMPLING_H
#define CAMCALISAMPLING_H

#include <QThread>
#include <QSemaphore>

class CameraCalibration;
class DataManager;
struct CvMat;
typedef struct _IplImage IplImage;

class CamCaliSampling : public QThread
{
    Q_OBJECT

public:
    CamCaliSampling(QObject *parent = NULL);
    ~CamCaliSampling();

    void run();

    void setCamCaliWindow(CameraCalibration *cc);

    void setDataManager(DataManager *pDM);
    IplImage* getCalibrationFrame();

    void sampling();
    void calibration();
    void testCaliResult(bool b);

    void acceptGoodView();

public Q_SLOTS:
    void start( Priority priority = InheritPriority);

Q_SIGNALS:
    void sigRawFrame(IplImage*);
    void sigUndistortedFrame(IplImage*);
    void sigCaliProgress(int);

    void sigSamplingFinished();
    void sigCalibrationFinished(CvMat*, CvMat*);

private:
    bool flagCamCaliThread;
    bool sectionSampling;
    bool sectionTest;

    CameraCalibration *pCCWindow;

    DataManager *pDataMgr;

    QSemaphore *goodView;

    CvMat* imgPts;
    CvMat* objPts;
    CvMat* cntPts;

    CvMat* intrinsicMatrix;
    CvMat* distortionCoeffs;

    int imgW;
    int imgH;

public:
    // Total number of crosses on the board
    int numberCross;
    // Number of Crosses in the Width of the Board
    int numberCrossInWidth;
    // Number of Crosses in the Height of the Board
    int numberCrossInHeight;
    // Total Number of Views of the Board in the Whole Calibration Procedure
    int numCaliView;
    // Number of Successful Sample Views
    int successes;
};

#endif // CAMCALISAMPLING_H
