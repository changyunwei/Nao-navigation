#include "stdafx.h"
#include "naourbiqtremoteclient.h"
#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
    /* 
     * Note that you must create the 
     * QApplication (or QCoreApplication) object 
     * before you can create a QThread.
     */
	QApplication a(argc, argv);
	NaoUrbiQtRemoteClient w;
	w.show();

    /* 
     * Note that QCoreApplication::exec() must always be called 
     * from the main thread (the thread that executes main()), not from a QThread. 
     * In GUI applications, the main thread is also called the GUI thread 
     * because it's the ONLY thread that is allowed to perform GUI-related operations.
	*/
    return a.exec();
}
