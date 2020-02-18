#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include <QWidget>
#include <QThread>
#include "ui_CameraCalibration.h"

class DataManager;
class CamCaliSampling;
typedef struct _IplImage IplImage;
struct CvMat;

class CameraCalibration : public QWidget
{
    Q_OBJECT

public:
    CameraCalibration(QWidget *parent = 0);
    ~CameraCalibration();

    void setCaliFrameSource(DataManager *pDM);

private: 
    void run();
    void showVideo(QLabel *lb, IplImage* iplImg);

public Q_SLOTS:
    void pbOnClickSample();
    void pbOnClickCalibration();
    void pbOnClickTestCaliResult(bool b);
    void pbOnClickAcceptGoodView();

    void onsigDispRawVideo(IplImage* iplImg);
    void onsigDispUndisortedVideo(IplImage* iplImg);
    void onsigCaliProgress(int pg);

    void enableCalibration();
    void enableTestCaliResult(CvMat*, CvMat*);

protected:
    void keyPressEvent( QKeyEvent * event );

private:
    Ui::CameraCalibration ui;

    // Make CamCaliSampling instance able to access private members of this class
    friend class CamCaliSampling;

    CamCaliSampling *ccs;

};

#endif // CAMERACALIBRATION_H
