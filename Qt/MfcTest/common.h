#ifndef COMMON_H
#define COMMON_H

#include <QObject>

class Common : public QObject
{
	Q_OBJECT
public:
	explicit Common(QObject *parent = 0);
	static QString CmdRun(QString cmd, int timeout, QString standOutputfile, bool waitfinish);
signals:
	
public slots:
	
};

#endif // COMMON_H
