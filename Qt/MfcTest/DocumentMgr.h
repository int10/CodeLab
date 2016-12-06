#ifndef DOCUMENTMGR_H
#define DOCUMENTMGR_H

#include <QObject>
#include <QByteArray>

class DocumentMgr : public QObject
{
    Q_OBJECT
public:

public:
	static QString GetDirPathOfRoot();
public:
    explicit DocumentMgr(QObject *parent = 0);
};

#endif // DOCUMENTMGR_H
