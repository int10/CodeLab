#include "DocumentMgr.h"
#include <QDir>
#include <QApplication>

DocumentMgr::DocumentMgr(QObject *parent) :
    QObject(parent)
{
}


QString DocumentMgr::GetDirPathOfRoot()
{
	QDir fatherdir(QApplication::applicationDirPath());
	return fatherdir.path();
}



