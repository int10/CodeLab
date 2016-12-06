#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include <QSqlError>
//#include <QSettings>
//#include <QSqlRecord>
#include <QDebug>
#include "sqlite3.h"
#include "Sqlite.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	Uni2Pinyin();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::Uni2Pinyin()
{
	QString test("J蛔人我为以要");
	Sqlite sql;
	QList<QMap<QString, QByteArray>> pinyin;
	sql.Init("E:/SysFolder/Desktop/uni2pinyin.db");

	QVector<uint> uni = test.toUcs4();

	foreach(uint u, uni)
	{
		if(u < 0x3007) {
			qDebug()<<QString::fromAscii((char *)&u);
			continue;
		}
		sql.GetData("select * from unicode2pinyin where code=" + QByteArray::number(u), pinyin);
		if(pinyin.count())
		{
			QMap<QString, QByteArray> map = pinyin.at(0);
			qDebug()<<map.value("p1")<<map.value("p2")<<map.value("p3")<<map.value("p4");
		}
		else
		{
			qDebug()<<"";
		}
	}

	sql.Close();

}

void MainWindow::CreateDb()
{
	QFile txtfile("E:/SysFolder/Desktop/Uni2Pinyin");
	txtfile.open(QIODevice::ReadOnly);
	QByteArray data = txtfile.readAll();
	QList<QByteArray> datalist = data.split(0x0a);
	txtfile.close();

	Sqlite sql;
	sql.Init("E:/SysFolder/Desktop/aa.db");
	sql.Exec("create table unicode2pinyin(code INTEGER PRIMARY KEY, p1 TEXT, p2 TEXT, p3 TEXT, "
			 "p4 TEXT, p5 TEXT, p6 TEXT)");

	sql.Exec("BEGIN");
	for(int i = 0; i < datalist.count(); i++)
	{
		QList<QByteArray> recodelist = datalist.at(i).split(0x09);
		QString insert = "insert into unicode2pinyin values(";
		int uniindex = recodelist.at(0).toInt(0, 16);
		insert += "\'" + QString::number(uniindex) + "\',";
		for(int j = 0; j < 6; j++)
		{
			if(recodelist.count() - 1 > j){
				insert += "\'" + recodelist.at(j + 1) + "\'";
			}else{
				insert += "\'\'";
			}
			if(j != 5){
				insert += ",";
			}
		}
		insert += ")";
		qDebug()<<datalist.at(i);
		sql.Exec(insert.toUtf8());
	}
	sql.Exec("COMMIT");
	sql.Close();
}
