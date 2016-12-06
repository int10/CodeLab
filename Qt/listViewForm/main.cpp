#include <QtGui/QApplication>
#include "mainwindow.h"
#include "a1234.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
//	MainWindow w;
//	w.show();
	a1234 w;
	w.show();
	
	return a.exec();
}
