#include "stdafx.h"
#include "USpy.h"
#include <QMessageBox>
#include "RobotState.h"
#include "wozcontrolclient.h"

WoZControlClient::WoZControlClient(QWidget *parent)
    : QWidget(parent)
    , pUSpy(NULL)
    , robotState(NULL)
{
    wozui.setupUi(this);

    robotState = new RobotState();
}

WoZControlClient::~WoZControlClient()
{
    delete robotState;
}

void WoZControlClient::setUrbiMessenger(USpy *pUS)
{
    pUSpy = pUS;
    return;
}

void WoZControlClient::pbOnClickSendUrbiScript()
{
    // Get the urbiscript text
    QString qsUrbiScript = ui.teUrbiScriptInput->toPlainText();
    // Must store the return value which is temporal
    QByteArray qbaUrbiScript = qsUrbiScript.toLatin1();
    const char* UrbiScript = qbaUrbiScript.data();

    // Debug
    qDebug() << UrbiScript;

    // Don't put the string in ""
    // Send the script
    pUSpy->send(UrbiScript);

    return;
}

//////////////////////////////////////////////////////////////////////////
void WoZControlClient::pbOnClickStopAllBehavior()
{
    pUSpy->send("BehaviorManager.stopAllBehaviors();");
}

//////////////////////////////////////////////////////////////////////////
void WoZControlClient::pbOnClickWaveHello()
{
    pUSpy->send("BehaviorManager.runBehavior(\"WaveHello\") & tts.say(\"Hello! Can you see me?\") | Leds.randomEyes(3);");
}

void WoZControlClient::pbOnClickElevator()
{
	pUSpy->send("tts.say(\"I am Nao. Welcome to Man Machine Interaction group. I will guide you to our experience lab. Please follow me.\"),");
}

void WoZControlClient::pbOnClickWalkSpeech()
{
	//pUSpy->send("tts.say(\"I am guiding you to our New Experience Lab. Many experiments are carried out in the lab.\"),");
	pUSpy->send("tts.say(\"I am humanoid robot, I am good at interacting with people. There are several researchers working with me, they will tell you something about the project, you can see the details on the poster.\"),");
}

void WoZControlClient::pbOnClickDoor()
{
	pUSpy->send("tts.say(\"I see the door of the experience lab. My friends there are working on negotiation and virtual reality. They will explain the details.\"),");
}

void WoZControlClient::pbOnClickInteraction()
{
	//pUSpy->send("BehaviorManager.runBehavior(\"interaction\"),");
	//pUSpy->send("BehaviorManager.runBehavior(\"surprise\"), sleep(4s); tts.say(\"Thank you for coming.\"),");
	pUSpy->send("BehaviorManager.runBehavior(\"guess_sport\"),");
}
//////////////////////////////////////////////////////////////////////////

void WoZControlClient::pbOnClickForward()
{
    pUSpy->send("motion.walkTo(0.1, 0, 0);");
}

void WoZControlClient::pbOnClickBackward()
{
    pUSpy->send("motion.walkTo(-0.1, 0, 0);");
}

void WoZControlClient::pbOnClickLeft()
{
    pUSpy->send("motion.walkTo(0, 0.1, 0);");
}

void WoZControlClient::pbOnClickRight()
{
    pUSpy->send("motion.walkTo(0, -0.1, 0);");
}

void WoZControlClient::setWalkVelocity(float x, float y, float theta, float f)
{
    wozui.leVX->setText( tr("%1").arg(x) );
    wozui.leVY->setText( tr("%1").arg(y) );
    wozui.leVTheta->setText( tr("%1").arg(theta) );
    wozui.leVF->setText( tr("%1").arg(f) );

    QString cmd = QString("motion.setWalkTargetVelocity(%1,%2,%3,%4);").arg(x).arg(y).arg(theta).arg(f);
    pUSpy->send(cmd.toStdString().data());
}

void WoZControlClient::pbOnClickTurnLeft(bool b)
{
    if ( b )
    {
	    robotState->theta = ui.dsbVTheta->value();
        ui.pbTurnRight->setChecked(false);
    } 
    else
    {
        robotState->theta = 0;
    }
    
    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency);
}

void WoZControlClient::pbOnClickTurnRight(bool b)
{
    if ( b )
    {
	    robotState->theta = -wozui.dsbVTheta->value();
        wozui.pbTurnLeft->setChecked(false);
    }
    else
    {
        robotState->theta = 0;
    }
    
    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency);
}

void WoZControlClient::pbOnClickStop()
{
    robotState->theta = 0;
    robotState->vx = 0;
    robotState->vy = 0;
    robotState->frequency = 0;

    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency);
    //pUSpy->send("motion.killWalk();");

    // Pop up all the buttons
    wozui.pbForwarding->setChecked(false);
    wozui.pbBackwarding->setChecked(false);
    wozui.pbLeftsiding->setChecked(false);
    wozui.pbRightsiding->setChecked(false);
}

void WoZControlClient::pbOnClickForwarding(bool b)
{
    if ( b )
    {
        robotState->vx = wozui.dsbVX->value();
        robotState->frequency = wozui.dsbVF->value();
        wozui.pbBackwarding->setChecked(false);
    }
    else 
    {
        robotState->vx = 0;
    }
    
    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency);
}

void WoZControlClient::pbOnClickBackwarding(bool b)
{
    if ( b )
    {
        robotState->vx = -wozui.dsbVX->value();
        robotState->frequency = wozui.dsbVF->value();
        wozui.pbForwarding->setChecked(false);
    }
    else
    {
        robotState->vx = 0;
    }
    
    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency); 
}

void WoZControlClient::pbOnClickLeftsiding(bool b)
{
    if ( b )
    {
        robotState->vy = wozui.dsbVY->value();
        robotState->frequency = wozui.dsbVF->value();
        wozui.pbRightsiding->setChecked(false);
    }
    else
    {
        robotState->vy = 0;
    }
    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency); 
}

void WoZControlClient::pbOnClickRightsiding(bool b)
{
    if (b)
    {
        robotState->vy = -wozui.dsbVY->value();
        robotState->frequency = wozui.dsbVF->value();
        wozui.pbLeftsiding->setChecked(false);
    }
    else
    {
        robotState->vy = 0;
    }
    setWalkVelocity(robotState->vx, robotState->vy, robotState->theta, robotState->frequency); 
}




/*/////////////////////////////////////////////////////////////////////////
//      Control Head
//
    void QLineEdit::textEdited ( const QString & text ) [signal]
    This signal is emitted whenever the text is edited. The text argument is the next text.
    Unlike textChanged(), this signal is not emitted when the text is changed programmatically, for example, by calling setText().
/////////////////////////////////////////////////////////////////////////*/
/*
 * In Urbi, headPitch.val varies from -0.67 ~ 0.51
 */
float WoZControlClient::setHeadPitch(float val)
{
    // Range check
    if ( val > 0.51 )   //  29.5 degree -> radian
    {
        val = 0.51;
    }
    if ( val < -0.67 )  // -38.5 degree -> radian
    {
        val = -0.67;
    }

    // Time  0 ~ 3.54 seconds
    float t = 2 * qAbs( val - robotState->headPitch );
    qDebug() << "SetHeadPitchTime:" << t << "s";

    // Send Command
    //QString s = QString("headPitch.val=%1 time:%2s;").arg(val).arg(t);
    //QString s = QString( "motion.angleInterpolation(\"HeadPitch\", %1, %2, 1), tts.say(\"head pitch.\");" ).arg(val).arg(t);
    QString s = QString( "motion.angleInterpolation(\"HeadPitch\", %1, %2, 1);" ).arg(val).arg(t);
    pUSpy->send( s.toStdString().data() );

    // Store current value
    robotState->headPitch = val;

    return val;
}

void WoZControlClient::pbOnClickRstHeadPitch()
{
    setHeadPitch(0);

    // ui display
    wozui.leHeadPitch->setText(tr("%1").arg(0));
    wozui.vsldHeadPitch->setSliderPosition(0);
}

void WoZControlClient::leOnTextChangedHeadPitch(QString sHP)
{
    float fHP = sHP.toFloat();

    // calculate and send command
    float finalFHP = setHeadPitch(fHP);

    // ui display
    wozui.leHeadPitch->setText(tr("%1").arg(finalFHP));
    wozui.vsldHeadPitch->setSliderPosition(finalFHP*1000.0);
}

void WoZControlClient::vsldOnValueChangedHeadPitch(int iHP)
{
    float fHP = iHP/1000.0;

    float finalFHP = setHeadPitch(fHP);

    // ui display
    wozui.leHeadPitch->setText(tr("%1").arg(finalFHP));
}

/*
 * In Urbi, headYaw.val varies from -2.079~2.079
 */
float WoZControlClient::setHeadYaw(float val)
{
    // Range check
    if ( val > 2.0 )   //  119.5 degree -> radian
    {
        val = 2.0;
    }
    if ( val < -2.0 )  // -119.5 degree -> radian
    {
        val = -2.0;
    }

    // Time  0 ~ 4 seconds
    float t = qAbs( val - robotState->headYaw );
    qDebug() << "SetHeadYawTime:" << t << "s";

    // Send command
    //QString s = QString("headYaw.val=%1 time:%2s;").arg(val).arg(t);
    QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(-val).arg(t);
    pUSpy->send( s.toStdString().data() );

    // Store current value
    robotState->headYaw = val;

    return val;
}

void WoZControlClient::pbOnClickRstHeadYaw()
{
    setHeadYaw(0);

    // ui display
    wozui.leHeadYaw->setText(tr("%1").arg(0));
    wozui.hsldHeadYaw->setSliderPosition(0);
}

void WoZControlClient::leOnTextChangedHeadYaw(QString sHY)
{
    float fHY = sHY.toFloat();

    float finalFHY = setHeadYaw(fHY);

    // ui display
    wozui.leHeadYaw->setText(tr("%1").arg(finalFHY));
    wozui.hsldHeadYaw->setSliderPosition(finalFHY*1000.0);
}

void WoZControlClient::hsldOnValueChangedHeadYaw(int iHY)
{
    float fHY = iHY/1000.0;

    float finalFHY = setHeadYaw(fHY);

    // ui display
    wozui.leHeadYaw->setText(tr("%1").arg(finalFHY));
}