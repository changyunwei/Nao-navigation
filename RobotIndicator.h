#ifndef ROBOTINDICATOR_H
#define ROBOTINDICATOR_H

#include <QThread>

#define PI         3.14159265359
#define STEP       5
#define PALSTANCE  5

class RobotIndicator : public QThread
{
    Q_OBJECT

Q_SIGNALS:
    void updateMap(qreal x, qreal y, qreal theta);

public:
    RobotIndicator();
    ~RobotIndicator();

private:
    void run();



public Q_SLOTS:
    void moveX(qreal x);
    void moveY(qreal y);

    void moveForward();
    void moveBackward();
    void moveLeft();
    void moveRight();

    void movingForward();
    void movingBackward();
    void movingLeft();
    void movingRight();

    void turn(qreal theta);
    void turnLeft();
    void turnRight();

    void stopMoving();

public:
    qreal posX;
    qreal posY;
    qreal theta;

    bool Forwarding;
    bool Backwarding;
    bool Leftsiding;
    bool Rightsiding;
};

#endif // ROBOTINDICATOR_H
