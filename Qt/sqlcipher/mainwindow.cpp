#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include "sqlite3.h"
#include "Sqlite.h"

#define DB_OLD_FILE_PATH  ("D:/localrom")
#define DB_FILE_PATH		("D:/localrom.db")
#define DB_SERVER_PATH		("d:/romdata.db")


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
//	Go();
	Get();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::Get()
{
	QFile datafile(DB_SERVER_PATH);
	Sqlite sqldata;
	sqldata.Init(datafile.fileName(), "F|0CU?q~e>\"*+1_~");
	QList<QMap<QString, QByteArray> > data;
	QByteArray sql = "select count(*) cnt from rompacket";
//	QByteArray sql = "select distinct phonefactory from rompacket";
	sqldata.GetData(sql, data);
	qDebug()<<data.at(0).value("cnt").toInt();

	for(int i = 0; i < data.count(); i++)
	{
		QMap<QString, QByteArray> d = data.at(i);
		foreach(QString k, d.keys())
		{
			qDebug() << k << QString::fromUtf8(d.value(k));
		}
	}

	sqldata.Close();
}


void MainWindow::Go()
{
	QFile datafile(DB_FILE_PATH);
	QFile olddatafile(DB_OLD_FILE_PATH);
	if(!datafile.exists() || olddatafile.exists())	//如果旧文件存在，或者新文件不存在，都把旧数据导到新数据库里
	{
		Sqlite localrom, oldlocalrom;

		localrom.Init(datafile.fileName(), "{'Kd&x}5h*.Jp{#_");
		oldlocalrom.Init(olddatafile.fileName(), "");

		localrom.Exec("create table rompacket(channel TEXT, romname TEXT, rommd5 TEXT, "
					  "romsysver TEXT, rombasever TEXT, phonefactory TEXT, phonefullname TEXT, "
					  "phonename TEXT, phonemodel TEXT, language TEXT,flashtype INTEGER, "
					  "flashmultable INTEGER, makedate TEXT, apks TEXT, apksmd5 TEXT, romlibid INTEGER, "
					  "imgpath TEXT, makeinfo TEXT, id INTEGER, romdesc TEXT, romtype INTEGER, flashdesc TEXT)");

		localrom.Exec("BEGIN");

		int begin = 0, page = 5;
		for(;;)
		{
			QList<QMap<QString, QByteArray> > localdata;
			//从旧数据库里获取数据
			oldlocalrom.GetData("select * from rompacket limit " + QByteArray::number(page) + " offset " + QByteArray::number(begin), localdata);
			//已经读完了
			qDebug()<<localdata.count();
			if(localdata.count() == 0) break;
			//把读到的数据写到新数据库里
			for(int i = 0; i < localdata.count(); i++)
			{
				QMap<QString, QByteArray> d = localdata.at(i);
				QByteArray keys, values;

				QList<QMap<QString, QByteArray> > datacheck;
				QByteArray sqlcheck;
				sqlcheck = "select * from rompacket where ";
				sqlcheck += "channel='" + d.value("channel") + "'";
				sqlcheck += "and phonefullname='" + d.value("phonefullname") + "'";
				sqlcheck += "and phonefactory='" + d.value("phonefactory") + "'";
				sqlcheck += "and imgpath='" + d.value("imgpath") + "'";
				localrom.GetData(sqlcheck, datacheck);

				if(datacheck.count()) continue;	//新数据库里已经有相应的数据，就不用插入了。

				foreach(QString k, d.keys())
				{
					if(!keys.isEmpty()) keys += ",";
					keys += k.toAscii();
					if(!values.isEmpty()) values += ",";
					values += "'" + d.value(k) + "'";
				}
				QByteArray sql;
				sql = "insert into rompacket(" + keys + ") ";
				sql += "values (" + values + ")";
				localrom.Exec(sql);
			}
			begin += page;
		}
		oldlocalrom.Close();
		localrom.Exec("COMMIT");
		localrom.Close();
		olddatafile.remove();
	}
}



