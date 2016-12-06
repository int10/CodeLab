#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

	MainWindow w;
	w.show();
	
	return a.exec();
}
