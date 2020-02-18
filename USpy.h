#ifndef USPY_H
#define USPY_H

#include <urbi/uobject.hh>
#include <QObject>
#include <QVector>

class QGuiThread;
class DataManager;

// QObject must be put first
class USpy : public QObject, public urbi::UObject
{
    Q_OBJECT

public:
    // The class must have a single constructor taking a string.
    USpy(const std::string& s);
    ~USpy();

    void setDataStorage(DataManager *pdm);
    
    void subscribeInertial(bool b);
    void trackFace(bool b);

    bool getHeadAngles(double **angles);

    bool updateHTM;
    urbi::UList homoTM;

    bool updateHeadYaw;
    urbi::UList angleHeadYaw;
    bool updateHeadPitch;
    urbi::UList angleHeadPitch;

    int   refInertialCnt;
    qreal refInertialX;
    qreal refInertialY;
    qreal refInertialT;

private:
    int init();

    // sonar
    void setSonarLSrc(urbi::UVar&);
    void onSonarL(urbi::UVar&);
    void setSonarRSrc(urbi::UVar&);
    void onSonarR(urbi::UVar&);

    // face position
    void setFacePosSrc(urbi::UVar& fp);
    void onFacePos(urbi::UVar& fp);

    // odometry 
    void setRobotPositionSrc(urbi::UVar& xyz);
    void onRobotPosition(urbi::UVar& xyz);

    // head position
    void onHTMUpdated(urbi::UVar& htm);
    void onHeadYawUpdated(urbi::UVar& hy);
    void onHeadPitchUpdated(urbi::UVar& hp);

	//avtive walkTo
	void activeWalkTo();

private:
    // Wrap the QThread in the UObject
    QGuiThread *GuiThread;

    // A reference to Data Storage
    DataManager *pDataStorage;


    urbi::UVar *sonarLSrc;
    urbi::UVar *sonarRSrc;
    urbi::UVar *facePosSrc;
    urbi::UVar *robotpositionSrc;

    //urbi::UVar HomoTransformMatrix;
    urbi::InputPort HomoTransformMatrix;
    urbi::InputPort ipHeadYaw;
    urbi::InputPort ipHeadPitch;

Q_SIGNALS:
    void sigSonarL(float);
    void sigSonarR(float);
    void sigFacePos(float, float, float);
	void sigRobotPosition(float,float, float);

	void activeNewWalkPath();

};

#endif // USPY_H
