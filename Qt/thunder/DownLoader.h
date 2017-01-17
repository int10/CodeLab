#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QMutex>
#include "DownLoaderBase.h"

class DownLoader : public QObject
{
    Q_OBJECT
public:
	DownLoader(QString url, QString savepath, QString savename, int threads = 0, bool thunder = false);
	DownLoader(QString url, QString filepath, int threads = 0, bool thunder = false);
    ~DownLoader();
    void Boot();
    void Stop(bool rmsg = true);
public:
	//get file size
	qint64 GetFileSize();
	//get file name include path
	QString GetFileName();
    //get down load is succes or fail
    bool Succ();
    //get down load error
    int GetLastError();
signals:
    //down load be stoped or it's finished
    void Over();
    //down load info,speed is byte/s
    void Info(qint64 speed, qint64 recvsize, qint64 filesize);
private slots:
	void DownLoadTaskOver();
	void DownLoadTaskInfo(qint64,qint64,qint64);
private:
	void MakeData(QString url, QString savepath, QString savename, int threads, bool thunder);
private:
	DownLoaderBase * m_downloader;
	QString m_url;
	QString m_savepath;
	QString m_savename;
	int m_retry;
};

#endif // DOWNLOADER_H
