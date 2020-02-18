#ifndef NAOURBIQTREMOTECLIENT_H
#define NAOURBIQTREMOTECLIENT_H

#include <QtGui/QMainWindow>
#include <QTime>
#include "ui_naourbiqtremoteclient.h"


typedef unsigned char BYTE;
typedef struct _IplImage IplImage;
class DataManager;
class ImageProcessor;
class USpy;
class UVideo;

namespace urbi
{
    class UImage;
    class UList;
}

class WoZControlClient;
class CameraCalibration;
class RobotIndicator;

class NaoUrbiQtRemoteClient : public QMainWindow
{
	Q_OBJECT

public:
	NaoUrbiQtRemoteClient(USpy *us);
	~NaoUrbiQtRemoteClient();

    void MotorEnable(bool en, const char* motorname);

private Q_SLOTS:
    // UI Slots
    void pbOnClickMotorEnableArm(bool en);
    void pbOnClickMotorEnableHead(bool en);
    void pbOnClickMotorEnableLeg(bool en);

    void pbOnDispSonar();
    void pbOnClickTurnOnCamera(bool);
    void pbOnClickTurnOnProcessing(bool);
    void pbOnClickFaceTrack(bool);
    void pbOnClickSoundTrack(bool);
    void pbOnClickTurnOnInertial(bool);

    void pbOnClickSaveImage();

    void pbOnClickCorrect2DCoordinates();
	void pbOnClickFaceLM();
	void pbOnClickPathPlanning();

    void pbOnClickWoZ();
    void pbOnClickCalibration();

    // Signal->Slot
    void onsigDispRawFrames(urbi::UImage*);
    void onsigDispProcessedFrames(IplImage*);
    void onsigDispSonarL(float sl);
    void onsigDispSonarR(float sr);
    void onsigDispFacePos(float, float, float);
    void onsigDispRobotPosition(float, float,float);


    void closeEvent(QCloseEvent *event);

	void pbOnClickTurnHeadYaw0();
	void pbOnClickTurnHeadYaw45();
	void pbOnClickTurnHeadYaw90();
	void pbOnClickTurnHeadYawn45();
	void pbOnClickTurnHeadYawn90();
    void pbOnClickSeeDoor();
private:
    // Real UI
	Ui::NaoUrbiQtRemoteClientClass ui;

    USpy *pUSpy;

    UVideo *pUVideo;

    // UI
    WoZControlClient *WoZCC;
    CameraCalibration *CamCali;

    // Data storage
    DataManager *dataMgr;

    // Image processing
    ImageProcessor *imgProcessor;

    //
    RobotIndicator *robotInd; 

    bool isCnctd;

    // Notation for sonar
    // to distinct callback value
    // 1 for left sonar
    // 2 for right sonar
    int cbdSonarL;
    int cbdSonarR;

    float inertialX;
    float inertialY;
    float inertialTheta;

	//image fps calculation
	QTime procTime;
	int procIntv;
	int procCount;
	float procFps;
};

#endif // NAOURBIQTREMOTECLIENT_H
