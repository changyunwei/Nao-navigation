#ifndef MORTECARLO_H
#define MORTECARLO_H

#include <QtGui/QMainWindow>
#include "ui_WizofOz.h"

class USpy;
class RobotState;

class WoZControlClient : public QMainWindow
{
    Q_OBJECT

public:
    WoZControlClient(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~WoZControlClient();

    void setUrbiMessenger(USpy *pUS);

    void setWalkVelocity(float x, float y, float theta, float f);

    float setHeadYaw(float val);
    float setHeadPitch(float val);

private:
    Ui::WoZControlClientClass ui;

    USpy *pUSpy;

    RobotState *robotState;

public Q_SLOTS:
    void pbOnClickStopAllBehavior();

    void pbOnClickWaveHello();
	void pbOnClickElevator();
	void pbOnClickWalkSpeech();
	void pbOnClickDoor();
    void pbOnClickInteraction();

    void pbOnClickSendUrbiScript();

    void pbOnClickForward();
    void pbOnClickBackward();
    void pbOnClickLeft();
    void pbOnClickRight();
    void pbOnClickStop();

    void pbOnClickForwarding(bool);
    void pbOnClickBackwarding(bool);
    void pbOnClickLeftsiding(bool);
    void pbOnClickRightsiding(bool);
    void pbOnClickTurnLeft(bool);
    void pbOnClickTurnRight(bool);

    void leOnTextChangedHeadPitch(QString);
    void leOnTextChangedHeadYaw(QString);
    void vsldOnValueChangedHeadPitch(int iHP);
    void hsldOnValueChangedHeadYaw(int iHY);

    void pbOnClickRstHeadYaw();
    void pbOnClickRstHeadPitch();
};

#endif // MORTECARLO_H
