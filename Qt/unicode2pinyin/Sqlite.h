#ifndef SQLITE_H
#define SQLITE_H

#include <QObject>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QStringList>
#include "sqlite3.h"

class Sqlite : public QObject
{
	Q_OBJECT
public:
	explicit Sqlite(QObject *parent = 0);
	~Sqlite();
	//在调用其它接口前必须先调一次Init。
	bool Init(QString database);
	void Close();
	//查询数据，sql：查询select语句。。如果放在result，result里qmap的key为字段名，value为数据值。返回查到的记录数。
	int GetData(QByteArray sql, QList<QMap<QString, QByteArray>> &result);
	//执行一些查询以外的sql语句，不需要返回的。
	int Exec(QByteArray sql);

signals:

public slots:
private:
	sqlite3 *m_db;
};

#endif // SQLITE_H
