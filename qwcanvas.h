#ifndef QWCANVAS_H
#define QWCANVAS_H

#include <QWidget>
#include <QPainter>
#include "ui_qwcanvas.h"
#include "GridMap.h"
#include "USpy.h"
#include "netcommunicatioin.h"

#define NAO_PI 3.1415

//class RobotBearing
//{
//public:
//    // Absolute robot position in pixels
//    qreal pixRX; //x
//    qreal pixRY; //y
//	qreal pixCRX; //x
//	qreal pixCRY; //y
//    // Absolute robot orientation in degree
//    qreal gmRT; // theta
//    // Absolute Head orientation in degree
//    qreal gmRHT;
//};

class qwCanvas : public QWidget
{
    Q_OBJECT

public:
    qwCanvas(QWidget *parent = 0);
    ~qwCanvas();

    void NaoSpaceTo2DMap(qreal &, qreal &, qreal &);

    void setUrbiMessenger(USpy *pUS);
	void setCommunicationMessenger(NetCommunicatioin *pCom);
	// Draw a symbol to represent the robot.

	void createRobotSymbol(qreal size = 18);
	void createRobotHeadSymbol(qreal size = 13);

	// Movement	
	void moveRobot(qreal x, qreal y);
	void turnRobot(qreal a);
	void setRobotAbsoluteOrientation(qreal a);
	void setRobotAbsolutePosition(qreal x, qreal y);
	void relocateRobot(int numLandmark, const double* xyz, double headYaw);

	void faceLandmark();

	void updateStartEnd();
	// from UI
	void initStartDest();


private:
    Ui::qwCanvas ui;

    USpy *pUSpy;

	NetCommunicatioin *pCommunication;


    // Save the robot symbol
    QPainterPath pixRobotSymbol;
    // The size of robot symbol
    qreal pixRobotSymbolSize; 
    // The size of robot head symbol
    qreal pixRobotHeadSymbolSize;
    // Save the symbol
    QPainterPath pixRobotHeadSymbol;

    // Draw robot track
    QVector <QPoint> robotTrack;

	// Qt coordinates Sys
    // Absolute robot position in pixels
    qreal robotCX_pix; //x
    qreal robotCY_pix; //y
    // Absolute robot orientation in degree
    qreal robotTheta; // theta
    // Head orientation relative to robot body in degree
    qreal robotHeadThetaR;
    // Absolute Head orientation in degree
    qreal robotHeadTheta;

    // Robot positions before correction
   // QVector<RobotBearing> rpbc; 

    qreal biasX;
    qreal biasY;
    qreal biasT;


    // Start and destination point in pixel
    QPointF startPoint_pix;
    QPointF destPoint_pix;

    // Landmark
    QPointF *landmarkPosition_pix;

	// Destinations
	//QPointF *destPoint;

public:
    GridMap *pGridMap;

	//int step;

protected:
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent( QMouseEvent * event );


Q_SIGNALS:
    void sigDestinationCorrection();
	void achievedNavigation(int);

private Q_SLOTS:

	void calculateNewWalkPath();
	void initialDirection();

public Q_SLOTS:

    // from UI
    void updateInertial(qreal x, qreal y, qreal theta);
    void execution(bool b);
	//from netCommunication
	void onExecuteNavigation(int a);
    
};

#endif // QWCANVAS_H
