#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "Windows.h"
#include "iniparse.h"
#include "common.h"
#include "DocumentMgr.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

//	MfcTest();

//	Common::CmdRun("Odin3_v1.85.exe", 5000, "",false);


	m_control.FindWindowW("");
//	m_control.SetControlCheck(0xA1266,true);
//	m_control.SetControlText(264408,"aa");
//	m_control.ClickCtrl(0x408d8);
	QList<ConItem> itemlist;
	itemlist = m_control.EnumControl();
	unsigned int btnStartIndex = 16;
	unsigned int btnStopIndex = 16;
	unsigned int btnComCheckIndex = 16;
	foreach(ConItem item,itemlist)
	{
//		qDebug()<<item.classname<<"  "<<item.title;
		if(item.classname == "TCheckBox" && item.title.contains("N/A + "))
		{
			if(btnComCheckIndex)
				m_com[--btnComCheckIndex] = item;
		}
		if(item.classname == "TButton" && item.title == "Start")
		{
			if(btnStartIndex)
				m_btnstart[--btnStartIndex] = item;
		}
		if(item.classname == "TButton" && item.title == "Stop")
		{
			if(btnStopIndex)
				m_btnstop[--btnStopIndex] = item;
		}
	}


	HDC hdc = ::GetWindowDC(m_control.m_hWnd);

	RECT rect;
	qDebug() << hdc;

	COLORREF color = ::GetPixel(hdc,0,0);

	qDebug()<< ::GetLastError();


	qDebug()<<color;
	qDebug()<<QString().sprintf("%x",color);

//	for(int y = 0;y <21 ; y++)
//	{
//		for(int x = 0;x<139 ; x++)
//		{
//			COLORREF color = ::GetPixel(hdc,x,y);
////			qDebug()<<QString("").sprintf("%x",color);
//			qDebug()<<color;
//		}

//	}

//	qDebug()<<QString("").sprintf("%x",color);



	//m_control.SetControlText(0x41B,"里面对于是");
	//m_control.RemoveControl(0x41B);
//	m_control.SetControlCheck(0x3fb,false);


//	IniParse parse;
//	parse.Parse("boot.ini");


//	parse.Parse("Config.ini");

//	parse.SetValue("operating","C:\\dr.mbr","\"力天装机\"");



//	qDebug()<<parse.GetValue("path","phone");
//	QString path = DocumentMgr::GetDirPathOfRoot()+"\\"+parse.GetValue("path","phone");
//	path.replace("/","\\");
//	path.replace("\\\\","\\");
//	m_control.SetControlText(0x412,path);




//	parse.Save();




}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_pushButton_clicked()
{
	static int i = 0;
	if(i == 16)
		i = 0;
	m_control.SetControlCheck(m_com[i].hwnd,true);
	i++;
}
