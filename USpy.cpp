#include "stdafx.h"
#include "QGuiThread.h"
#include "DataManager.h"
#include "USpy.h"

using namespace urbi;

UStart(USpy);

USpy::USpy(const std::string& s)
    : UObject(s)
    , QObject(0)
    , GuiThread(NULL)
    , sonarLSrc(NULL)
    , sonarRSrc(NULL)
    , facePosSrc(NULL)
	, robotpositionSrc(NULL)
    , updateHTM(0)
    , updateHeadYaw(0)
    , updateHeadPitch(0)
    , refInertialCnt(0)
    , refInertialX(0)
    , refInertialY(0)
    , refInertialT(0)
{
    /*
     * The constructor only registers init, 
     *  so that our default instance servo does nothing, 
     *  and can only be used to create new instances.
     */
    UBindFunction(USpy, init);
    
    //homoTM = new UList();
}

USpy::~USpy()
{
    if ( GuiThread )
        delete GuiThread;

    delete sonarLSrc;
    delete sonarRSrc;
    delete facePosSrc;
    delete robotpositionSrc;
}

int USpy::init()
{

    UBindFunction(USpy, setSonarLSrc);
    UBindFunction(USpy, setSonarRSrc);
    UBindFunction(USpy, setFacePosSrc);
    UBindFunction(USpy, setRobotPositionSrc);
	//active walkTo
    UBindFunction(USpy, activeWalkTo);	

    UBindVar(USpy, HomoTransformMatrix);
    UNotifyChange(HomoTransformMatrix, &USpy::onHTMUpdated);

    UBindVar(USpy, ipHeadYaw);
    UNotifyChange(ipHeadYaw, &USpy::onHeadYawUpdated);
    UBindVar(USpy, ipHeadPitch);
    UNotifyChange(ipHeadPitch, &USpy::onHeadPitchUpdated);

    // It doesn't work!
    //QObject::connect(GuiThread, SIGNAL(finished()), this, SLOT(clearUObjects()));
    //QObject::connect(GuiThread, SIGNAL(terminated()), this, SLOT(clearUObjects()));

    // Create GUI thread and show main window
    GuiThread = new QGuiThread();
    GuiThread->setUSpy(this);
    GuiThread->start();

    // Immediately return after "GuiThread->start()"
    return 0;
}

void USpy::onHTMUpdated(UVar& htm)
{
    homoTM = UList(htm);
    //UList homoTM = UList(HomoTransformMatrix);
    updateHTM = 1;
    return;
}

void USpy::onHeadYawUpdated(UVar& hy)
{
    angleHeadYaw = UList(hy);
    updateHeadYaw = 1;
    return;
}

void USpy::onHeadPitchUpdated(UVar& hp)
{
    angleHeadPitch = UList(hp);
    updateHeadPitch = 1;
    return;
}

void USpy::setDataStorage(DataManager *pdm)
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

void USpy::setFacePosSrc(UVar& fp)
{
    if ( facePosSrc )
        facePosSrc->unnotify();

    // Store the facePos source
    facePosSrc = new UVar(fp);

    UNotifyChange( *facePosSrc, &USpy::onFacePos );

    qDebug() << "Set FacePos Notification.";
}

void USpy::onFacePos(UVar& fp)
{
    UList ulfp = UList(fp);

    float x = ulfp.array[0]->val;
    float y = ulfp.array[1]->val;
    float z = ulfp.array[2]->val;

    Q_EMIT sigFacePos(x,y,z);
}

void USpy::trackFace(bool b)
{
    if ( b )
    {
        send("FaceTracker.startTracker(),facetracker.unfreeze;");
    } 
    else
    {
        send("facetracker.freeze;FaceTracker.stopTracker(),");
    }
}

void USpy::setSonarLSrc(UVar& sl)
{    
    if ( sonarLSrc )
        sonarLSrc->unnotify();

    // Store the sonarL source
    sonarLSrc = new UVar(sl);

    UNotifyChange( *sonarLSrc, &USpy::onSonarL );

    qDebug() << "Set SonarL Notification.";
}

void USpy::onSonarL(UVar& sl)
{
    float snl = (ufloat)sl;
    Q_EMIT sigSonarL(snl);
}

void USpy::setSonarRSrc(UVar& sr)
{
    if ( sonarRSrc )
        sonarRSrc->unnotify();

    // Store the sonarL source
    sonarRSrc = new UVar(sr);

    UNotifyChange( *sonarRSrc, &USpy::onSonarR );

    qDebug() << "Set SonarR Notification.";
}

void USpy::onSonarR(UVar& sr)
{
    float snr = (ufloat)sr;
    Q_EMIT sigSonarR(snr);
}


/**
 * get the robot position from odometry
 */
void USpy::setRobotPositionSrc(UVar& xyz)
{
	if ( robotpositionSrc )
		robotpositionSrc->unnotify();

	// Store the facePos source
	robotpositionSrc = new UVar(xyz);

	UNotifyChange( *robotpositionSrc, &USpy::onRobotPosition);
}

void USpy::onRobotPosition(UVar& xyz)
{
	UList ulfp = UList(xyz);

    float x = ulfp.array[0]->val;
    float y = ulfp.array[1]->val;
    float t = ulfp.array[2]->val;

    //Q_EMIT sigRobotPosition(x,y,t);

    if ( refInertialCnt < 5 )
    {	
        refInertialX += x;
        refInertialY += y;
        //refInertialT += t;
        refInertialCnt++;
        if ( refInertialCnt == 5 )
        {
            refInertialX /= 5;
            refInertialY /= 5;
            //refInertialT /= 5;
        }
    }
    else
    {
        float x_ = x - refInertialX;
        float y_ = y - refInertialY;
        //float t_ = t - refInertialT;

	    Q_EMIT sigRobotPosition(x_,y_,t);
    }
}

void USpy::subscribeInertial(bool b)
{
    if ( b )
    {
		USpy::send("inertial.unfreeze;");
    } 
    else
    {
		USpy::send("inertial.freeze;");
    }
}

void USpy::activeWalkTo()
{
	Q_EMIT activeNewWalkPath();
}

/*
bool USpy::getTransformationMatrix()
{
    // Get Transformation Matrix
    while (!updateHTM)
    {
        pUSpy->send("updateHTM();");
        // ms
        Sleep(300);
    }
    updateHTM = 0;
}
*/

bool USpy::getHeadAngles(double **angles)
{
    int failures = 0;
    // Get Head Pitch and Yaw Angles
    while ( (!updateHeadYaw) || (!updateHeadPitch) )
    {
        UObject::send("updateHeadAngles();");
        // ms
        Sleep(500);
        failures++;
        if ( failures > 5)
        {
            return 1;
        }
    }

    updateHeadYaw = 0;
    updateHeadPitch = 0;

    if ( (*angles) != NULL )
    { 
        (*angles)[0] = angleHeadYaw.array[0]->val;
        (*angles)[1] = angleHeadPitch.array[0]->val;
    }

    return 0;
}

