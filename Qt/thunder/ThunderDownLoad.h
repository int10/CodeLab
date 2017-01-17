#ifndef THUNDERDOWNLOAD_H
#define THUNDERDOWNLOAD_H

#include <QObject>

#include <Windows.h>
#include <Shlwapi.h>
#include <QList>
#include <QMutex>
#include "xldl.h"
#include "DownWrapper.h"
#include <QDir>
#include <QApplication>

class ThunderDownload : public QObject
{
	Q_OBJECT
public:
	static ThunderDownload * Instance();
//	static void UnInstance();
signals:

public slots:
private:
	ThunderDownload(QObject * parent = 0);
	~ThunderDownload();
public:
	HANDLE TaskCreate(QString url, QString savepath, QString savename);
	bool TaskDelete(HANDLE handle);
	bool TaskQuery(HANDLE handle, DownTaskInfo &info);

private:
	static ThunderDownload m_instance;
private:
	DownWrapper * m_pWapper;
	QList<HANDLE> m_handlelist;		//当前正在下载的handle句柄
	QMutex m_tasklock;
};

#endif // THUNDERDOWNLOAD_H
