#ifndef ADBMGR_H
#define ADBMGR_H

#include <QObject>

class AdbMgr : public QObject
{
    Q_OBJECT
public:
    static QString Run(QString cmd, QString sn = NULL, int timeout = 0);//timeout is m seconds
    static QString ShellRun(QString cmd, QString sn = NULL, bool issu = false, int timeout = 0);//timeout is m seconds
    static QString CmdRun(QString cmd, int timeout, QString standOutputfile = "");
};

#endif // ADBMGR_H
