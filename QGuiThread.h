#pragma once
#ifndef QGUITHREAD_H
#define QGUITHREAD_H

#include <QThread>

class NaoUrbiQtRemoteClient;
class USpy;
class UVideo;


class QGuiThread : public QThread
{
    Q_OBJECT

public:
    QGuiThread();
    ~QGuiThread();

    void run();

    void setUSpy(USpy *us);
    bool hasUSpy();

private:
    // main GUI
    NaoUrbiQtRemoteClient *mainWindow;

    // UObject for receiving data
    USpy *pUspy;
    bool hasSpy;
};

#endif // QGUITHREAD_H