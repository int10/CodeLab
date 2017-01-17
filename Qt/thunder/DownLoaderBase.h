#ifndef DOWNLOADERBASE_H
#define DOWNLOADERBASE_H

#include <QObject>
#include <QMutex>
#include <QFile>
#include <QDateTime>

#define DOWNLOAD_LOG_FILE	"downloadlog.txt"
#define DOWNLOAD_TIMEOUT	1000*60

class DownLoaderBase : public QObject
{
    Q_OBJECT
public:
	virtual void Boot(){}
	virtual void Stop(bool rmsg = true){rmsg = rmsg;}
public:
	//get file size
	virtual qint64 GetFileSize(){return 0;}
	//get file name include path
	virtual QString GetFileName(){return "";}
    //get down load is succes or fail
	virtual bool Succ(){return false;}
    //get down load error
	virtual int GetLastError(){return 0;}
};

#endif // DOWNLOADERBASE_H
