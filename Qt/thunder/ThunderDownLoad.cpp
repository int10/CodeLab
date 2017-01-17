#include "ThunderDownLoad.h"
#include <QDebug>

ThunderDownload ThunderDownload::m_instance;

ThunderDownload::ThunderDownload(QObject *parent) : QObject(parent)
{
	m_pWapper = NULL;
}

ThunderDownload::~ThunderDownload()
{
	if(m_pWapper)
	{
		foreach(HANDLE h, m_handlelist)	m_pWapper->TaskDelete(h);
		m_pWapper->UnInit();
		delete m_pWapper;
		m_pWapper = NULL;
	}
}

ThunderDownload * ThunderDownload::Instance()
{
	return &m_instance;
}

HANDLE ThunderDownload::TaskCreate(QString url, QString savepath, QString savename)
{
	HANDLE handle = NULL;
	m_tasklock.lock();
	if(!m_pWapper)
	{
		QDir fatherdir(QApplication::applicationDirPath());
		QString dllpath = fatherdir.path() + "/xldl.dll";
		m_pWapper = new DownWrapper((const wchar_t *)dllpath.utf16());
		// 初始化下载引擎
		if (!m_pWapper->Init())
		{
			qDebug() << "Init failed!";
		}
	}

	if(m_pWapper)
	{
		// 创建下载任务
		DownTaskParam downTaskParam;
		StrCpyW(downTaskParam.szTaskUrl, url.toStdWString().c_str());
		StrCpyW(downTaskParam.szFilename, savename.toStdWString().c_str());
		StrCpyW(downTaskParam.szSavePath, savepath.toStdWString().c_str());
		handle = m_pWapper->TaskCreate(downTaskParam);
		if(handle)
		{
			m_pWapper->TaskStart(handle);
			m_handlelist.append(handle);
		}
	}
	m_tasklock.unlock();
	return handle;
}

bool ThunderDownload::TaskDelete(HANDLE handle)
{
	m_tasklock.lock();
	if(m_pWapper)
	{
		//删除任务
		m_pWapper->TaskPause(handle);
		m_pWapper->TaskDelete(handle);
	}
	m_handlelist.removeOne(handle);
	m_tasklock.unlock();
	return true;
}

bool ThunderDownload::TaskQuery(HANDLE handle, DownTaskInfo &info)
{
	bool result = false;
	m_tasklock.lock();
	if(m_pWapper)
	{
		//查询任务状态
		if(m_handlelist.contains(handle))
		{
			result = m_pWapper->TaskQueryEx(handle, info);
		}
	}
	m_tasklock.unlock();
	return result;
}
