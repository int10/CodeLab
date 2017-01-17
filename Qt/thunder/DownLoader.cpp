#include "DownLoader.h"
#include "DownLoaderForThunder.h"
#include <QHttp>

DownLoader::DownLoader(QString url, QString filepath, int threads, bool thunder)
{	
	QString savepath;
	QString savename;

	savename = filepath.mid(filepath.lastIndexOf("/")+1);
	savepath = filepath.mid(0,filepath.lastIndexOf("/")+1);

	MakeData(url, savepath, savename, threads, thunder);
}

DownLoader::DownLoader(QString url, QString savepath, QString savename, int threads, bool thunder)
{	
	MakeData(url, savepath, savename, threads, thunder);
}

void DownLoader::MakeData(QString url, QString savepath, QString savename, int , bool thunder)
{
	m_url = url;
	m_savepath = savepath;
	m_savename = savename;
	m_retry = 5;

	m_downloader = new DownLoaderForThunder(m_url, m_savepath, m_savename);

	connect(m_downloader, SIGNAL(Over()), this, SLOT(DownLoadTaskOver()),Qt::QueuedConnection);
	connect(m_downloader, SIGNAL(Info(qint64, qint64, qint64)), this, SLOT(DownLoadTaskInfo(qint64, qint64, qint64)));
}

void DownLoader::DownLoadTaskOver()
{
	//如果多线程模块下载失败 那就使用单线程模块做次尝试(单线程模块纯GET方式,几乎能应对所有情况)
	if(!m_downloader->Succ() && m_retry > 0)
	{
		m_retry--;
		delete m_downloader;
		m_downloader = new DownLoaderForThunder(m_url, m_savepath, m_savename);
		connect(m_downloader, SIGNAL(Over()), this, SLOT(DownLoadTaskOver()),Qt::QueuedConnection);
		connect(m_downloader, SIGNAL(Info(qint64,qint64,qint64)), this, SLOT(DownLoadTaskInfo(qint64,qint64,qint64)));
		m_downloader->Boot();
	}
	else
	{
		emit Over();
	}
}

void DownLoader::DownLoadTaskInfo(qint64 speed, qint64 recvsize, qint64 filesize)
{
	emit Info(speed, recvsize, filesize);
}

DownLoader::~DownLoader()
{
	if(m_downloader) delete m_downloader;
}

void DownLoader::Boot()
{
	m_downloader->Boot();
}

void DownLoader::Stop(bool rmsg)
{
	m_downloader->Stop(rmsg);
}

int DownLoader::GetLastError()
{
	return m_downloader->GetLastError();
}

bool DownLoader::Succ()
{
	return m_downloader->Succ();
}

QString DownLoader::GetFileName()
{
	return m_downloader->GetFileName();
}

qint64 DownLoader::GetFileSize()
{
	return m_downloader->GetFileSize();
}
