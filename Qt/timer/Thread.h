#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QTimer>
#include <QEventLoop>
class Thread : public QThread
{
	Q_OBJECT
public:
	Thread();
	~Thread();
	void Boot();
	void StopTimer();

signals:
	void StopTimer2();
protected:
	void run();
private:
	QTimer *m_timer;
};

#endif // THREAD_H
