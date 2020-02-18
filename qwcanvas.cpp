#include "StdAfx.h"
#include "qwcanvas.h"
#include <QMouseEvent>
#include <QtGui>
#include <math.h>
#include "synchronization.h"

qwCanvas::qwCanvas(QWidget *parent)
    : QWidget(parent)
    , robotCX_pix(0)
    , robotCY_pix(0)
    , robotTheta(0)
    , robotHeadTheta(0)
    , robotHeadThetaR(0)
    , biasX(0)
    , biasY(0)
    , biasT(0)
{     
    ui.setupUi(this);

    //Initiate robot start and end position
    ui.sbStartX->setValue(285);
    ui.sbStartY->setValue(450);
    ui.sbStartTheta->setValue(0);
    ui.sbEndX->setValue(285);
    ui.sbEndY->setValue(400);

	// Initiate the landmark position
	landmarkPosition_pix = new QPointF[14];
	landmarkPosition_pix[9] = QPointF(240,530);

	landmarkPosition_pix[0] = QPointF(240,430);
	landmarkPosition_pix[3] = QPointF(240,450);
	landmarkPosition_pix[8] = QPointF(240,440);

	landmarkPosition_pix[1] = QPointF(330,350);
	landmarkPosition_pix[2] = QPointF(330,370);


	landmarkPosition_pix[10] = QPointF(240,280);
	landmarkPosition_pix[11] = QPointF(240,290);


	landmarkPosition_pix[5] = QPointF(240,200);
	landmarkPosition_pix[6] = QPointF(240,210);

	landmarkPosition_pix[4] = QPointF(330,110);
	landmarkPosition_pix[7] = QPointF(330,120);


	//step=1;

	//destPoint = new QPointF[6];
	//destPoint[0] = QPointF( 285,455 );
	//destPoint[1] = QPointF( 285,375 );
	//destPoint[2] = QPointF( 285,295 );
	//destPoint[3] = QPointF( 285,215 );
	//destPoint[4] = QPointF( 285,125 );
	//destPoint[5] = QPointF( 285,85 );

	// create symbol of robot
	createRobotSymbol();
	createRobotHeadSymbol();

    setBackgroundRole(QPalette::Base);

    pGridMap = new GridMap();

	initStartDest();
    // Initial position on the map UI
    setRobotAbsolutePosition( startPoint_pix.rx(), startPoint_pix.ry() );
    setRobotAbsoluteOrientation( ui.sbStartTheta->value() );
}

qwCanvas::~qwCanvas()
{
    delete pGridMap;
	delete [] landmarkPosition_pix;
	//delete destPoint;
}

void qwCanvas::setUrbiMessenger(USpy *pUS)
{
    pUSpy = pUS;

    connect( pUSpy, SIGNAL(activeNewWalkPath()), 
             this,  SLOT(calculateNewWalkPath()) );

    return;
}

void qwCanvas::setCommunicationMessenger(NetCommunicatioin *pCom)
{
	pCommunication = pCom;

	connect( pCommunication, SIGNAL(executeNavigation(int)), 
		this,  SLOT(onExecuteNavigation(int)) );

	connect( this, SIGNAL(achievedNavigation(int)), 
		pCommunication,  SLOT(onAchievedNavigation(int)) );

	return;
}

/*
* Warning: When the paint device is a widget, 
* QPainter can only be used inside a paintEvent() 
* function or in a function called by paintEvent(); 
* that is unless the Qt::WA_PaintOutsidePaintEvent 
* widget attribute is set. On Mac OS X and Windows, 
* you can only paint in a paintEvent() function 
* regardless of this attribute's setting.
*/
void qwCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QBrush graybrush(Qt::gray);
    QBrush redbrush(Qt::red);
    QBrush greenbrush(Qt::green);
    QBrush bluebrush(Qt::blue);
    QBrush yellowbrush(Qt::yellow);
    QBrush magentabrush(Qt::magenta);

    //Draw grid map
    for (int i=0; i<=GRIDMAP_W; i++)
    {
        for (int j = 0; j<=GRIDMAP_H; j++)
        {
            // Draw obstacle
            if (pGridMap->grid[i][j] == VALUE_OBSTACLE)
            {
                painter.setBrush( graybrush );
                painter.drawRect( i*10, j*10, 10, 10 );
            }
            // Draw path
            if (pGridMap->grid[i][j] == VALUE_PATH)
            {
                painter.setBrush( redbrush );
                painter.drawRect( i*10, j*10, 10, 10 );
            }

            if (pGridMap->grid[i][j] == VALUE_LANDMARK_L )
            {
                painter.setBrush( magentabrush );
                painter.drawRect( i*10, j*10, 10, 10 );
            }
            if (pGridMap->grid[i][j] == VALUE_LANDMARK_R )
            {
                painter.setBrush( magentabrush );
                painter.drawRect( i*10, j*10, 10, 10 );
            }
        }
    }

    // Draw grid lines
    painter.setPen( Qt::lightGray );
    for (int i=0; i<=GRIDMAP_W+1; i++)
        painter.drawLine( i*GRIDCELL_SIZE, 0, i*GRIDCELL_SIZE, (GRIDMAP_H)*GRIDCELL_SIZE );

    for (int j = 0; j<=GRIDMAP_H; j++)
        painter.drawLine( 0, j*GRIDCELL_SIZE, (GRIDMAP_W+1)*GRIDCELL_SIZE, j*GRIDCELL_SIZE );

    // Draw Landmark Position
    painter.setBrush(magentabrush);
    for (int i=0; i<12; i++)
    {
    	painter.drawEllipse( landmarkPosition_pix[i], 3, 3 );
    }

	//// Draw destinations
	//painter.setBrush(greenbrush);
	//for (int i=0; i<6; i++)
	//{
	//	painter.drawEllipse( destPoint[i], 3, 3 );
	//}

    // Draw current position
    painter.setBrush(redbrush);
    painter.drawEllipse(startPoint_pix, 5, 5);
    // Draw destination
    painter.setBrush(greenbrush);
    painter.drawEllipse(destPoint_pix, 5, 5);

    // Draw Robot
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::blue);
    painter.save();
    painter.translate(robotCX_pix,robotCY_pix);
    painter.rotate(robotTheta);
    painter.drawPath(pixRobotSymbol);
    painter.restore();

    // Draw Robot Head
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::red);
    painter.save();
    painter.translate(robotCX_pix,robotCY_pix);
    painter.rotate(robotHeadTheta);
    painter.drawPath(pixRobotHeadSymbol);
    painter.restore();

    // Draw track
    painter.setPen(Qt::red);

    for (int i= 0; i<robotTrack.size(); i++)
    {
        painter.drawPoint(robotTrack[i]);
    }

  //  // Draw Robot positions before correction
  //  painter.setPen(Qt::red);
  //  for (int i= 0; i<rpbc.size(); i++)
  //  {
		//painter.drawEllipse( QPointF( rpbc[i].pixRX, rpbc[i].pixRY ), 5, 5 );
		//painter.drawEllipse( QPointF( rpbc[i].pixCRX, rpbc[i].pixCRY ), 5, 5 );

  //  }

    return;
}

// Draw a symbol to represent the robot.
// The origin of the robot is (0,0)
void qwCanvas::createRobotSymbol(qreal size)
{
	//new robotSymbol, the direction is towards negative Y axis
	pixRobotSymbolSize = size;
	// Move to the start point of the circle
	pixRobotSymbol.moveTo(0, -size/2);
	// Draw full circle
	pixRobotSymbol.arcTo(QRectF(-size/2,-size/2,size,size),0,360);
	// Draw heading arrow
	pixRobotSymbol.moveTo(-size/2, 0);
	pixRobotSymbol.lineTo(0, -1.5*size);
	pixRobotSymbol.moveTo(size/2, 0);
	pixRobotSymbol.lineTo(0, -1.5*size);
    return;
}

void qwCanvas::createRobotHeadSymbol(qreal size)
{
    //old robotHeadSymbel, the direction is towards positive X axis
	/*
	pixRobotHeadSymbolSize = size;
    // Move to the start point of the circle
    pixRobotHeadSymbol.moveTo(size/2,0);
    // Draw full circle
    pixRobotHeadSymbol.arcTo(QRectF(-size/2,-size/2,size,size),0,360);
    // Draw heading arrow
    pixRobotHeadSymbol.moveTo(0,0);
    pixRobotHeadSymbol.lineTo(1.5*size,0);
	*/

    //new robotHeadSymbel, the direction is towards positive X axis
	pixRobotHeadSymbolSize = size;
	// Move to the start point of the circle
	pixRobotHeadSymbol.moveTo( 0, -size/2 );
	// Draw full circle
	pixRobotHeadSymbol.arcTo( QRectF(-size/2,-size/2,size,size),0,360 );
	// Draw heading arrow
	pixRobotHeadSymbol.moveTo( 0,0 );
	pixRobotHeadSymbol.lineTo( 0, -1.5*size );

    return;
}

void qwCanvas::turnRobot(qreal dTheta)
{
    robotTheta += dTheta;
	ui.dsbRobotTheta->setValue(robotTheta);
    return;
}

void qwCanvas::moveRobot(qreal dx, qreal dy)
{
    robotCX_pix += dx;
    robotCY_pix += dy;
    ui.dsbRobotX->setValue(robotCX_pix);
    ui.dsbRobotY->setValue(robotCY_pix);
    robotTrack.append( QPoint(robotCX_pix, robotCY_pix) );
    return;
}

// Set absolute position of the robot in degree
void qwCanvas::setRobotAbsolutePosition(qreal x, qreal y)
{
    //pixRobotCX = biasX + x;
   // pixRobotCY = biasY + y;

	robotCX_pix = biasX + x;
	robotCY_pix = biasY + y;

    ui.dsbRobotX->setValue(robotCX_pix);
    ui.dsbRobotY->setValue(robotCY_pix);
    robotTrack.append( QPoint(robotCX_pix, robotCY_pix) );
    return;
}

// Set absolute orientation of the robot in degree
void qwCanvas::setRobotAbsoluteOrientation(qreal theta)
{
   // degree
   // gmRobotTheta = biasT + theta;
	robotTheta = biasT + theta;

	ui.dsbRobotTheta->setValue(robotTheta);
    return;
}

void qwCanvas::NaoSpaceTo2DMap(qreal &x, qreal &y, qreal &theta)
{
	// Translate X,Y theta to pixels
	// m -> pixel             25mm/pixel
	qreal newX     = -y * 1000.0 / 25.0;
	qreal newY     = -x * 1000.0 / 25.0;
	// rad -> degree
	qreal newTheta = -theta * 180.0 / NAO_PI;

	// output
	// pixel
	x = newX;
	y = newY;
	// degree
	theta = newTheta;
}

void qwCanvas::updateInertial(qreal x, qreal y, qreal theta)
{
	// m  m  rad
    NaoSpaceTo2DMap( x, y, theta );

    //moveRobot(x,y);
    //turnRobot(theta);
    setRobotAbsolutePosition(x,y);
    setRobotAbsoluteOrientation(theta);

    updateStartEnd();

    QWidget::update();
}

void qwCanvas::mouseDoubleClickEvent( QMouseEvent * event )
{
    qDebug() << "QME:" << event->x() << event->y();

    //destPixel.rx() = qRound( event->x() / GRIDMAP_INTERVAL ) * GRIDMAP_INTERVAL;
    //destPixel.ry() = qRound( event->y() / GRIDMAP_INTERVAL ) * GRIDMAP_INTERVAL;
    destPoint_pix.rx() = event->x();
    destPoint_pix.ry() = event->y();
    qDebug() << "Destination:" << destPoint_pix.rx() << destPoint_pix.ry();

    updateStartEnd();
    pGridMap->findNewPath();
    QWidget::update();

}



// Select next sub destination
void qwCanvas::updateStartEnd()
{

    pGridMap->curPos.gcx = robotCX_pix / GRIDCELL_SIZE;
    pGridMap->curPos.gcy = robotCY_pix / GRIDCELL_SIZE;

 	pGridMap->nextPos.gcx = destPoint_pix.rx() / GRIDCELL_SIZE;
	pGridMap->nextPos.gcy = destPoint_pix.ry() / GRIDCELL_SIZE;

}

void qwCanvas::initStartDest()
{
    // initiate the start and end robot position
    startPoint_pix.rx() = ui.sbStartX->value();
    startPoint_pix.ry() = ui.sbStartY->value();
    destPoint_pix.rx()  = ui.sbEndX->value();
    destPoint_pix.ry()  = ui.sbEndY->value();
    robotTheta       = ui.sbStartTheta->value();
	ui.dsbRobotTheta->setValue(robotTheta);

	//initiate starting robot position
	biasX = startPoint_pix.rx();
	biasY = startPoint_pix.ry();


}

void qwCanvas::execution(bool b)
{
    if ( b )
    {
        pUSpy->send("tts.say(\"ready!\"),");
        pUSpy->send("walkCheck.unfreeze;");
    }
    else
    {
        pUSpy->send("tts.say(\"Stop\"),");
        pUSpy->send("walkCheck.freeze;");
    }
}

void qwCanvas::onExecuteNavigation(int a)
{
	if (a == 1)
	{
		//pUSpy->send("tts.say(\"ready!\"),");
		pUSpy->send("walkCheck.unfreeze;");
	}
	else if (a ==2 )
	{
		destPoint_pix.rx() = 250;
		destPoint_pix.ry() = 400;

		//pUSpy->send("tts.say(\"ready!\"),");
		pUSpy->send("walkCheck.unfreeze;");
	}
	
}

// Real path for robot walking
void qwCanvas::calculateNewWalkPath()
{

	// update path
	pGridMap->findNewPath();

    if ( !pGridMap->path.isEmpty() )
    {
        // for calculate and save the data from Grid Map
        float dx = 0.0;
        float dy = 0.0;

		QPointF nextCell;
		// Top left of the square to center
		nextCell.rx() = (pGridMap->path[0].gcx * GRIDCELL_SIZE) + (GRIDCELL_SIZE/2);
		nextCell.ry() = (pGridMap->path[0].gcy * GRIDCELL_SIZE) + (GRIDCELL_SIZE/2);

		// Vector
		dx = -0.025*(nextCell.rx() - robotCX_pix);
		dy = -0.025*(nextCell.ry() - robotCY_pix);



		// because the function of rotate is clockwise, it should be -robotTheta
		float stepY =  dx*cos(robotTheta*PI/180) + dy*sin(robotTheta*PI/180);
		float stepX =  dy*cos(robotTheta*PI/180) - dx*sin(robotTheta*PI/180);

		//qDebug() << "step:" << dy << dx << stepY <<stepX <<gmRobotTheta;
		
        
		/**
		// Rotate around X axis
		float dx_zx = -dx_z;
		float dy_zx = -dy_z;

		// Pixel -> m
		float stepX = dx_zx * 0.025;
		float stepY = dy_zx * 0.025;
		*/

		// Send command
        //QString cmd = QString("motion.walkTo(%1,%2,%3);").arg(stepX).arg(stepY).arg(gmRobotTheta*PI/180);
		QString cmd = QString("motion.walkTo(%1,%2,0);").arg(stepX).arg(stepY);
        pUSpy->send(cmd.toStdString().data());
    }
    else
    {
		//report arrival
		static int a = 0;

       // pUSpy->send("tts.say(\"next\"),");
        pUSpy->send("walkCheck.freeze;");

		a++;

		Q_EMIT achievedNavigation(a);


		//qDebug() << "I have arrived here";

		// Next phase
		//step++;
    }
}

void qwCanvas::relocateRobot(int numLandmark, const double* xyz, double headYaw)
{

 //   // to degree
 //   double theta = 180 * qAtan( xyz[0]/xyz[2] ) / PI;
 //   qreal newPixelX = 0;
 //   qreal newPixelY = 0;
 //   qreal newTheta  = 0;

 //   switch (numLandmark)
 //   {
 //   // right wall
 //   case 2:
 //   case 3:
 //   case 5:
 //   case 8:
 //   //case 10:
 //       newPixelX = pixLandmarkPosition[ numLandmark-1 ].x() + (GRIDCELL_SIZE * xyz[2] / FACTOR);
 //       newPixelY = pixLandmarkPosition[ numLandmark-1 ].y() + (GRIDCELL_SIZE * xyz[0] / FACTOR);
 //       //robotHeadTheta = theta;
 //       robotHeadTheta = 90 + theta;
 //       break;
	//// left wall
	//case 11:
	//case 12:
 //   case 10:
 //   case 4:
 //   case 9:
	//case 1:
	//case 6:
	//case 7:
 //       newPixelX = pixLandmarkPosition[ numLandmark-1 ].x() - (GRIDCELL_SIZE * xyz[2] / FACTOR);
 //       newPixelY = pixLandmarkPosition[ numLandmark-1 ].y() - (GRIDCELL_SIZE * xyz[0] / FACTOR);
 //       //robotHeadTheta  = theta - 180;
 //       robotHeadTheta = -90 + theta;
 //       break;
 //   default:
 //       break;
 //   }

	//// degree
 //   robotHeadThetaR = headYaw;

	//// degree
 //   newTheta = robotHeadTheta + robotHeadThetaR;

	//if ( newTheta > 360 )
	//{
	//	newTheta -= 360;
	//}
	//if ( newTheta < (-360) )
	//{
	//	newTheta += 360;
	//}

 //   // Bias
 //   biasX += (newPixelX - pixRobotCX);
 //   biasY += (newPixelY - pixRobotCY);
 //   biasT += (newTheta - gmRobotTheta);
 //   //
 //   ui.dsbBiasX->setValue(biasX);
 //   ui.dsbBiasY->setValue(biasY);
 //   ui.dsbBiasTheta->setValue(biasT);

	//qDebug() << "bias X Y Theta:" << biasX << biasY << biasT;


 //   RobotBearing rb;
 //   rb.pixRX = pixRobotCX;
 //   rb.pixRY = pixRobotCY;
	//rb.pixCRX = newPixelX;
	//rb.pixCRY = newPixelY;
 //   rb.gmRT  = gmRobotTheta;
 //   rb.gmRHT = robotHeadTheta;
 //   rpbc.append( rb );

 //   pixRobotCX = newPixelX;
 //   pixRobotCY = newPixelY;
 //   gmRobotTheta = newTheta;

 //   ui.dsbRobotX->setValue(pixRobotCX);
 //   ui.dsbRobotY->setValue(pixRobotCY);
 //   ui.dsbRobotTheta->setValue(gmRobotTheta);
 //   ui.dsbCorrectedX->setValue(pixRobotCX);
 //   ui.dsbCorrectedY->setValue(pixRobotCY);
 //   ui.dsbCorrectedTheta->setValue(gmRobotTheta);

 //   QWidget::update();

 //   return;
}

void qwCanvas::initialDirection()
{
	if ( robotTheta > 360 )
	{
		robotTheta -= 360;
	}
	if ( robotTheta < (-360) )
	{
		robotTheta += 360;
	}

    QString cmd = QString("motion.walkTo(0,0,%1);").arg( robotTheta*PI/180 );
    pUSpy->send(cmd.toStdString().data());

	// 0 degree 1.5s
	QString s = QString( "motion.angleInterpolation(\"HeadYaw\", %1, %2, 1);" ).arg(0).arg(1.5);
	pUSpy->send( s.toStdString().data() );
}




void qwCanvas::faceLandmark()
{
	/*
	// radius
	double t;

	switch (step)
	{
    // left
	case 1:
	case 2:
	case 4:
	case 5:
		t = PI/2.0 + gmRobotTheta * PI / 180.0;
		if ( t > PI )
		{
			t -= 2*PI;
		}
		break;
    // right
	case 3:
	case 6:
	    t = gmRobotTheta*PI/180;
		break;
	default:
		break;
	}

	if ( abs(t) < 3 )
	{
		//adjust direction
		QString cmd = QString("motion.walkTo(0,0,%1);").arg( t );
	    pUSpy->send(cmd.toStdString().data());
	}
	else
	{
		qDebug() << "Turn too much!";
	}
	*/

}
