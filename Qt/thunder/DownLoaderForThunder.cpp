#include "DownLoaderForThunder.h"
#include <QDebug>

DownLoaderForThunder::DownLoaderForThunder(QString url, QString savepath, QString savename)
{
	m_url = url;
	m_isboot = false;
	m_issuc = false;
	m_error = 0;
	m_filesize = 0;
	m_pWapper = NULL;
	m_filepath = savepath + savename;
	m_filename = savename;
	m_savepath = savepath;
}

DownLoaderForThunder::~DownLoaderForThunder()
{
}

void DownLoaderForThunder::DownloadInfo()
{
	DownTaskInfo info;
	ThunderDownload::Instance()->TaskQuery(m_hTask, info);

	m_filesize = info.nTotalSize;
	if (info.stat == TSC_ERROR)
	{
		m_error = info.fail_code;
		m_issuc = false;
		Stop(false);
	}
	else if(info.stat == TSC_COMPLETE)
	{
		m_issuc = true;
		Stop();
	}
	else if(info.stat == TSC_DOWNLOAD)
	{
		emit Info(info.nSpeed, info.nTotalDownload, info.nTotalSize);
	}
}

void DownLoaderForThunder::Boot()
{
	m_tasklock.lock();
	if(!m_isboot)
	{
		// 创建下载任务
		m_hTask = ThunderDownload::Instance()->TaskCreate(m_url, m_savepath, m_filename);

		qDebug() << "Create task. m_hTask = " << m_hTask;

		m_isboot = true;
		m_timeout = new QTimer(this);
		connect(m_timeout, SIGNAL(timeout()), this,SLOT(DownloadInfo()));
		m_timeout->start(1000);
	}
	m_tasklock.unlock();
}

void DownLoaderForThunder::Stop(bool rmsg)
{
	m_tasklock.lock();
	if(m_isboot)
	{
		if(m_timeout)
		{
			disconnect(m_timeout, SIGNAL(timeout()), this,SLOT(DownloadInfo()));
			m_timeout->stop(); m_timeout->deleteLater();
		}
		ThunderDownload::Instance()->TaskDelete(m_hTask);
		m_isboot = false;
		if(rmsg) emit Over();		
	}
	m_tasklock.unlock();
}

//get file size
qint64 DownLoaderForThunder::GetFileSize()
{
	return m_filesize;
}

//get file name include path
QString DownLoaderForThunder::GetFileName()
{
	return m_filepath;
}

//get down load is succes or fail
bool DownLoaderForThunder::Succ()
{
	return m_issuc;
}

//get down load error
int DownLoaderForThunder::GetLastError()
{
	return m_error;
}

//void DownLoaderForThunder::DelTempFile()
//{
//	// 删除任务临时文件
//	DownTaskParam deleteParam;
//	StrCpyW(deleteParam.szFilename, m_filename.toStdWString().c_str());
//	StrCpyW(deleteParam.szSavePath, m_savepath.toStdWString().c_str());
//	m_pWapper->DelTempFile(deleteParam);
//}
