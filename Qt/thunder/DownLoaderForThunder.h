#ifndef DOWNLOADERFORTHUNDER_H
#define DOWNLOADERFORTHUNDER_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QMutex>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include "DownLoaderBase.h"

#include <Windows.h>
#include <Shlwapi.h>
#include <string>
#include <iomanip>
#include "xldl.h"
#include "DownWrapper.h"
#include "ThunderDownLoad.h"

class DownLoaderForThunder : public DownLoaderBase
{
    Q_OBJECT
public:
	DownLoaderForThunder(QString url, QString savepath, QString savename);
	~DownLoaderForThunder();
	void Boot();
	void Stop(bool rmsg = true);
//	void DelTempFile();
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
	void DownloadInfo();
private:
    QNetworkAccessManager * m_NetworkAccessManager;
    QNetworkReply * m_NetworkReply;
	QMutex m_tasklock;
	bool m_isboot;
	QTime m_downtime;
	int m_downtick;
	qint64 m_curspeed;
	QTimer * m_timeout;
	int m_error;
	bool m_issuc;
private:
	QString m_url;
	QString m_filepath;
	QString m_filename;
	QString m_savepath;
	DownWrapper* m_pWapper;
	HANDLE m_hTask;
	qint64 m_filesize;
};

#endif // DOWNLOADERFORTHUNDER_H
