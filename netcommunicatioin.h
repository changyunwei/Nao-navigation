#ifndef NETCOMMUNICATIOIN_H
#define NETCOMMUNICATIOIN_H

#include <QtGui/QWidget>
#include <QXmlStreamReader>
#include "stdafx.h"
#include <QDebug>
#include "ui_netcommunicatioin.h"
#include "netserver.h"
#include "USpy.h"

//#include "qwcanvas.h"

//class qwCanvas;

class NetCommunicatioin : public QWidget
{
	Q_OBJECT

public:
	NetCommunicatioin(QWidget *parent = 0, Qt::WFlags flags = 0);
	~NetCommunicatioin();
    
	
	void setUrbiMessenger(USpy *pUS);

	// parsing XML message
	void parseXMLMessage(QString);


Q_SIGNALS:

	void sendMessageStr(QString);
	void executeNavigation(int);


private Q_SLOTS:

	// from TCP signal
	void OndebugMessageStr(QString);
	void OnreceiveMessageStr(QString);
	void OnsendMessageStr(QString);
    


	//from UI
	void OnClickStartServer();
	void OnClickStopServer();
	void OnClickSendMessage();

	//from canvas
	void onAchievedNavigation(int);

private:
	Ui::NetCommunicatioinClass ui;
    
	//qwCanvas *pNavigation;

	// for execute urbiscripts
	USpy *pUSpy;

	//QTcpServer * netServer;
	netServer *pServer;
};

#endif // NETCOMMUNICATIOIN_H
