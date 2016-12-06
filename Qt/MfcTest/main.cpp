#include <QtGui/QApplication>
#include "mainwindow.h"
#include<QTextCodec>
#include <QFile>
#include <QDebug>


typedef struct
{
	int power;		//信号强度
	QString ssid;
	int security;	//是否加密

}sCommWifiInfo;

void ScanWifi()
{

	QFile aa("aa.txt");
	aa.open(QIODevice::ReadOnly);


	QString result = QString::fromAscii(aa.readAll());
	qDebug()<<result;

	QStringList wifilist = result.split("\n");
	foreach(QString wifinode,wifilist)
	{

		sCommWifiInfo wifiinfo;
		wifinode = wifinode.simplified();
		if(wifinode.contains("Security"))
			wifiinfo.security = 1;
		else
			wifiinfo.security = 0;

		QStringList nodeInfo = wifinode.split(" ");
		if(nodeInfo.size()<2)
			continue;
		QString power = nodeInfo.at(0);
		wifiinfo.power = power.remove("%").toInt();
		wifiinfo.ssid = nodeInfo.at(1).simplified();



//		QString ssid = wifinode.mid(wifinode.indexOf(" "),wifinode.);
//		wifiinfo.ssid = ssid;
		//list.append(wifiinfo);
		qDebug()<<wifiinfo.ssid<<" "<<wifiinfo.power<<" "<<wifiinfo.security;
	}


}



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

	ScanWifi();

	MainWindow w;
	w.show();
	
	return a.exec();
}
