#include "stdafx.h"
#include "netserver.h"
#include "netcommunicatioin.h"
#include <QTcpSocket>
#include <QRegExp>

netServer::netServer(QObject *parent)
	: QTcpServer(parent)
	, socket(NULL)
{
}

netServer::~netServer()
{
	delete socket;
}

void netServer::incomingConnection(int socketfd)
{
	//QTcpSocket *client = new QTcpSocket(this);
	socket = new QTcpSocket(this);

	socket->setSocketDescriptor(socketfd);


	//clients.insert(client);

    // emit signals
	QString str = QString("Connection From: %1").arg(socket->peerAddress().toString());
	Q_EMIT debugMessageStr(str);

	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void netServer::readyRead()
{
	//QTcpSocket *client = (QTcpSocket*)sender();
	while(socket->canReadLine())
	{
		QString line = QString::fromUtf8(socket->readLine()).trimmed();
		//qDebug() << "Read line:" << line;
		// emit signals
		QString str = QString("%1").arg(line);
		Q_EMIT receiveMessageStr(str);


		/*
		QRegExp meRegex("^/me:(.*)$");

		if(meRegex.indexIn(line) != -1)
		{
			QString user = meRegex.cap(1);
			users[client] = user;
			foreach(QTcpSocket *client, clients)
				client->write(QString("Server:" + user + " has joined.\n").toUtf8());
			sendUserList();
		}
		else if(users.contains(client))
		{
			QString message = line;
			QString user = users[client];
			qDebug() << "User:" << user;
			qDebug() << "Message:" << message;

			foreach(QTcpSocket *otherClient, clients)
				otherClient->write(QString(user + ":" + message + "\n").toUtf8());
		}
		else
		{
			qWarning() << "Got bad message from client:" << client->peerAddress().toString() << line;
		}

		*/
	}
}


void netServer::newSendMessageStr(QString sendStr)
{
	 socket->write(QString(sendStr + "\n").toUtf8());
}

void netServer::disconnected()
{
	//QTcpSocket *client = (QTcpSocket*)sender();

	// emit signals
	QString str = QString("Client disconnected:").arg(socket->peerAddress().toString());
	Q_EMIT debugMessageStr(str);



	/*
	clients.remove(client);
	QString user = users[client];
	users.remove(client);
	sendUserList();
	foreach(QTcpSocket *client, clients)
		client->write(QString("Server:" + user + " has left.\n").toUtf8());

		*/
}


/*

void netServer::sendUserList()
{
	QStringList userList;
	foreach(QString user, users.values())
		userList << user;

	foreach(QTcpSocket *client, clients)
		client->write(QString("/users:" + userList.join(",") + "\n").toUtf8());
}

*/