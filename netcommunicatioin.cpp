#include "stdafx.h"
#include "netcommunicatioin.h"
//#include "qwcanvas.h"


NetCommunicatioin::NetCommunicatioin(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
	, pServer(NULL)
	//, pNavigation(NULL)
{
	ui.setupUi(this);

	//initiate the components
	ui.pbStopServer->setEnabled(false);
	ui.lbServerState->setStyleSheet("QLabel {background-color:red; color:blue;}"); 

    // local ui
	QObject::connect( ui.pbStartServer,    SIGNAL(clicked()), this, SLOT(OnClickStartServer()));
	QObject::connect( ui.pbStopServer,     SIGNAL(clicked()), this, SLOT(OnClickStopServer()));
	QObject::connect( ui.pbSendMessage,     SIGNAL(clicked()), this, SLOT(OnClickSendMessage()));

//	QObject::connect(this, SIGNAL(executeNavigation), ui.Canvas, SLOT(execution()));

}

NetCommunicatioin::~NetCommunicatioin()
{
	delete pServer;
}

void NetCommunicatioin::setUrbiMessenger(USpy *pUS)
{
	pUSpy = pUS;
	return;
}


void NetCommunicatioin::OnClickStartServer()
{
	//clear up message
	ui.lwDebugMessage->clear();
	ui.lwReceiver->clear();
	ui.lwSender->clear();


	pServer = new netServer();


	if (!pServer->listen(QHostAddress::LocalHost, 5566))
	{
		ui.lwReceiver->addItem(pServer->errorString());

		pServer->close();		

		ui.pbStartServer->setText("Server Failed");
		ui.pbStartServer->setEnabled(true);

	}
	else
	{
		ui.lwDebugMessage->addItem("Server Started!");


		ui.pbStartServer->setText("Server Started");
		ui.pbStartServer->setEnabled(false);
		ui.pbStopServer->setEnabled(true);
		ui.lbServerState->setStyleSheet("QLabel {background-color:green; color:blue;}"); 

		//bind connection
		QObject::connect(pServer, SIGNAL(debugMessageStr(QString)), this, SLOT(OndebugMessageStr(QString)));
		QObject::connect(pServer, SIGNAL(sendMessageStr(QString)), this, SLOT(OnsendMessageStr(QString)));
		QObject::connect(pServer, SIGNAL(receiveMessageStr(QString)), this, SLOT(OnreceiveMessageStr(QString)));

		QObject::connect(this, SIGNAL(sendMessageStr(QString)), pServer, SLOT(newSendMessageStr(QString)));

	}

}

void NetCommunicatioin::OnClickStopServer()
{
	pServer->close();

	ui.pbStartServer->setText("Start Server");
	ui.pbStartServer->setEnabled(true);
	ui.pbStopServer->setEnabled(false);
	ui.lbServerState->setStyleSheet("QLabel {background-color:red; color:blue;}"); 
	ui.lwDebugMessage->addItem("Server stopped!");

}

void NetCommunicatioin::OnClickSendMessage()
{

	QString message = ui.leSendMessage->text().trimmed();

	if(!message.isEmpty())
	{
		Q_EMIT sendMessageStr(message);
	}

	//ui.leSendMessage->clear();
	ui.leSendMessage->setFocus();

	//display in sender QListWidget
	ui.lwSender->addItem(message);
	ui.lwSender->scrollToBottom();

}

void NetCommunicatioin::OndebugMessageStr(QString str)
{
	ui.lwDebugMessage->addItem(str);
	ui.lwDebugMessage->scrollToBottom();
}

void NetCommunicatioin::OnreceiveMessageStr(QString str)
{
	ui.lwReceiver->addItem(str);
	ui.lwReceiver->scrollToBottom();

    parseXMLMessage(str);

}

void NetCommunicatioin::parseXMLMessage(QString str)
{
	//xml parsing 
	QXmlStreamReader xml(str);
	xml.readNextStartElement();

	//qDebug() << xml.name().toString();
	QXmlStreamAttributes responseAttr = xml.attributes();
	QString strFunctionName = responseAttr.value("function").toString();

	QString cmd;
	qDebug() << strFunctionName;

    if (strFunctionName == "Say")
    {
		cmd = QString("tts.say(\"%1\"),").arg(responseAttr.value("words").toString());

		// send commands
		pUSpy->send(cmd.toStdString().data());
    }

	else if (strFunctionName == "Goto")
	{   
		int a = 1;

		Q_EMIT executeNavigation(a);
	}

	else if (strFunctionName == "EnterRoom")
	{		
		// turn 90 degree
		cmd = QString("motion.walkTo(0,0,1.3);");
		pUSpy->send(cmd.toStdString().data());

		int a = 2;
		Q_EMIT executeNavigation(a);
	}

	else if (strFunctionName == "Gesture")
	{
		cmd = QString("BehaviorManager.runBehavior(\"raiseArm\");");
		pUSpy->send(cmd.toStdString().data());
	}

}

void NetCommunicatioin::OnsendMessageStr(QString str)
{
	ui.lwSender->addItem(str);
	ui.lwSender->scrollToBottom();

}

void NetCommunicatioin::onAchievedNavigation(int a)
{

	QString message;

	if (a==1)
	{
		message = QString("at(roomA)");
	}
	else if (a==2)
	{
		message = QString("in(roomA)");
	}
	
	if(!message.isEmpty())
	{
		Q_EMIT sendMessageStr(message);
	}

	//ui.leSendMessage->clear();
	ui.leSendMessage->setFocus();

	//display in sender QListWidget
	ui.lwSender->addItem(message);
	ui.lwSender->scrollToBottom();
}