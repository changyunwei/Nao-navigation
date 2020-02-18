#ifndef WOZCONTROLCLIENT_H
#define WOZCONTROLCLIENT_H

#include <QtGui/QWidget>
#include "ui_wozcontrolclient.h"


class USpy;
class RobotState;

class WoZControlClient : public QWidget
{
    Q_OBJECT

public:
    WoZControlClient(QWidget *parent = 0);
    ~WoZControlClient();

   void setUrbiMessenger(USpy *pUS);

    void setWalkVelocity(float x, float y, float theta, float f);

    float setHeadYaw(float val);
    float setHeadPitch(float val);

private:

    Ui::WoZControlClient wozui;

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

#endif // WOZCONTROLCLIENT_H
