#include "stdafx.h"
#include "naourbiqtremoteclient.h"
#include "USpy.h"
#include <QtGui/QApplication>
#include "QGuiThread.h"

QGuiThread::QGuiThread()
    : QThread()
    , pUspy(NULL)
    , hasSpy(0)
    , mainWindow(NULL)
{

}

QGuiThread::~QGuiThread()
{
    quit();
    wait();

    delete mainWindow;
}

void QGuiThread::setUSpy(USpy *us)
{
    pUspy = us;
    hasSpy = 1;
}

bool QGuiThread::hasUSpy()
{
    return hasSpy;
}

void QGuiThread::run()
{
    /* 
     * Note that you must create the 
     * QApplication (or QCoreApplication) object 
     * before you can create a QThread.
     */
    int argc = 0;
    char *argv[1];
	QApplication a(argc, argv);

    // It doesn't work
    //QObject::connect( &a, SIGNAL(destoryed()), this, SLOT(clearUObjects()) );

	mainWindow = new NaoUrbiQtRemoteClient(pUspy);

    // Show main GUI, start interaction between users
	mainWindow->show();

    qDebug() << "GUI thread terminated!";

    /* 
     * Note that QCoreApplication::exec() must always be called 
     * from the main thread (the thread that executes main()), not from a QThread. 
     * In GUI applications, the main thread is also called the GUI thread 
     * because it's the ONLY thread that is allowed to perform GUI-related operations.
     *
     * We recommend that you connect clean-up code to the aboutToQuit() signal, 
     *  instead of putting it in your application's main() function. 
     *  This is because, on some platforms the QApplication::exec() call may not return. 
     *  For example, on the Windows platform, when the user logs off, 
     *  the system terminates the process after Qt closes all top-level windows. 
     *  Hence, there is no guarantee that the application will have time to exit its event loop 
     *  and execute code at the end of the main() function, after the QApplication::exec() call.
	 */
    a.exec();

    // call function in UrbiScript to clear up the UObjects 
    pUspy->send("closeServer();");

    // return after a.exec() returning
    return;
}