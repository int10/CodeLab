#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AdbMgr.h"
#include <QDebug>
#include <QFileInfo>
#include <QDropEvent>
#include <QUrl>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setAcceptDrops(true);


//	QString result;
//	result = AdbMgr::Run("ls /mnt/sdcard/");
//	qDebug()<<result;
//	QString srcfile = "\"D:\\qtTest\\build-push-桌面-Debug\\附件3.doc\"";
//	QFileInfo fileinfo(srcfile);
//	qDebug()<<fileinfo.fileName();
//	QString desfile = "\"/mnt/sdcard/"+fileinfo.fileName()+"\"";
//	QString cmd = "push "+srcfile + " " + desfile.toUtf8();
//	qDebug()<<AdbMgr::Run(cmd);
//	qDebug()<<AdbMgr::Run("devices");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasFormat("text/uri-list")) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
		QList <QUrl> urls = event->mimeData()->urls();
		if(urls.isEmpty())
				return;

		foreach(QUrl url, urls) {
				QString file_name = url.toLocalFile();
				qDebug() << file_name;
				m_srcFile.setFile(file_name);
		}
}

void MainWindow::on_pushButton_clicked()
{
	if(!m_srcFile.exists())
		return;
	QString srcfile = "\""+m_srcFile.filePath()+"\"";

	QString realfilename = "\"/mnt/sdcard/"+m_srcFile.fileName()+"\"";
	QString desfile = "\"/mnt/sdcard/tmp.rd\"";
//	QByteArray ff = desfile.toUtf8();
//	qDebug()<<QString::fromUtf8(ff);
//	QString cmd = "push "+srcfile + " " + desfile.toUtf8();
	QString cmd = "push "+srcfile + " " + desfile.toUtf8();
	qDebug()<<AdbMgr::Run(cmd);
	AdbMgr::ShellRun("mv /mnt/sdcard/tmp.rd "+realfilename.toUtf8());
}
