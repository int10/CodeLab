#include "Thread.h"
#include <QDebug>

Thread::Thread()
{

}

Thread::~Thread()
{
	if(m_timer)
	{
		//QTimer::singleShot(0, m_timer, SLOT(stop()));
		//m_timer->stop();
		m_timer->deleteLater();
	}

	wait();
	qDebug()<<"333333333";

}

void Thread::Boot()
{

	start();
}
void Thread::StopTimer()
{
	emit StopTimer2();
}

void Thread::run()
{
	m_timer = new QTimer;
	m_timer->setSingleShot(true);
	QEventLoop loop;
	connect(m_timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(m_timer, SIGNAL(destroyed()), &loop, SLOT(quit()));
	connect(this, SIGNAL(StopTimer2()), m_timer, SLOT(stop()));
	m_timer->setInterval(10000);
	m_timer->start();
	loop.exec();
	qDebug()<<"44444444444";
}
