#ifndef NETSERVER_H
#define NETSERVER_H

#include "stdafx.h"
#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
//#include <QMap>
//#include <QSet>

class NetCommunicatioin;

class netServer : public QTcpServer
{
	Q_OBJECT

public:
	netServer(QObject *parent=0);
	~netServer();

Q_SIGNALS:
	void debugMessageStr(QString str);
	void receiveMessageStr(QString str);
	void sendMessageStr(QString str);

private Q_SLOTS:

	// from QTcpServer
	void readyRead();
	void disconnected();

	// from UI
	void newSendMessageStr(QString);

protected:

	void incomingConnection(int socketfd);

private:

	//QSet<QTcpSocket*> clients;
	//QMap<QTcpSocket*,QString> users;
	QTcpSocket *socket;

	
};

#endif // NETSERVER_H
