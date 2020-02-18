#include "StdAfx.h"
#include <QtCore/qmath.h>
#include "RobotIndicator.h"

RobotIndicator::RobotIndicator()
    : posX(0)
    , posY(0)
    , theta(0)
    , Forwarding(0)
    , Backwarding(0)
    , Leftsiding(0)
    , Rightsiding(0)
{

}

RobotIndicator::~RobotIndicator()
{

}

void RobotIndicator::run()
{
    while ( Forwarding || Backwarding || Leftsiding || Rightsiding )
    {
        if (Forwarding)
        {
            moveForward();
        }
        if (Backwarding)
        {
            moveBackward();
        }
        if (Leftsiding)
        {
            moveLeft();
        }
        if (Rightsiding)
        {
            moveRight();
        }

        msleep(500);
    }

    exec();
}

void RobotIndicator::stopMoving()
{
    // make the thread return
    Forwarding = 0;
    Backwarding = 0;
    Leftsiding = 0;
    Rightsiding = 0;
    quit();
    wait();
}

void RobotIndicator::moveX(qreal x)
{
    posX += x;
}
void RobotIndicator::moveY(qreal y)
{
    posY -= y;
}

void RobotIndicator::turn(qreal t)
{
    theta += t;
}

void RobotIndicator::moveForward()
{
    qreal v = PI*theta/180;
    qreal x = STEP*qSin(v);
    qreal y = STEP*qCos(v);
    moveX(x);
    moveY(y);
    Q_EMIT updateMap(posX, posY, theta);
}

void RobotIndicator::moveBackward()
{
    qreal v = PI*theta/180;
    qreal x = STEP*qSin(v+PI);
    qreal y = STEP*qCos(v+PI);
    moveX(x);
    moveY(y);
    Q_EMIT updateMap(posX, posY, theta);
}

void RobotIndicator::moveLeft()
{
    qreal v = PI*theta/180;
    qreal x = STEP*qSin(v-PI/2);
    qreal y = STEP*qCos(v-PI/2);
    moveX(x);
    moveY(y);
    Q_EMIT updateMap(posX, posY, theta);
}

void RobotIndicator::moveRight()
{
    qreal v = PI*theta/180;
    qreal x = STEP*qSin(v+PI/2);
    qreal y = STEP*qCos(v+PI/2);
    moveX(x);
    moveY(y);
    Q_EMIT updateMap(posX, posY, theta);
}

void RobotIndicator::movingForward()
{
    Forwarding = 1;
    start();
}
void RobotIndicator::movingBackward()
{
    Backwarding = 1;
    start();
}
void RobotIndicator::movingLeft()
{
    Leftsiding = 1;
    start();
}
void RobotIndicator::movingRight()
{
    Rightsiding = 1;
    start();
}

void RobotIndicator::turnLeft()
{
    turn(-PALSTANCE);
    Q_EMIT updateMap(posX, posY, theta);
}

void RobotIndicator::turnRight()
{
    turn(PALSTANCE);
    Q_EMIT updateMap(posX, posY, theta);
}