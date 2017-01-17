#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <QUrl>

#define DOWNLOAD_PATH		"h:"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	m_dl = NULL;
	ui->pbDownload->setValue(0);
}

MainWindow::~MainWindow()
{
	if(m_dl)
	{
		m_dl->deleteLater();
		m_dl = NULL;
	}
	delete ui;
}

void MainWindow::SlotInfo(qint64 speed, qint64 recvsize, qint64 filesize)
{
	qDebug() << "speed:" << speed << "recvsize:" << recvsize << "filesize:" << filesize << "percent:" << (recvsize * 100) / filesize << "%";
	qint64 percent = (recvsize * 100) / filesize;
	QString s = "speed:" + QString::number(speed);
	s += " recvsize:" + QString::number(recvsize);
	s += " filesize:" + QString::number(filesize);
	s += " percent:" + QString::number(percent) + "%";
	ui->lbInfo->setText(s);
}

void MainWindow::SlotOver()
{
	qDebug()<<"over";
	m_dl->deleteLater();
	m_dl = NULL;
	ui->lbInfo->setText("finished!");
}

void MainWindow::on_btnAdd_clicked()
{
	if(m_dl) return;

	QString url = ui->lineEdit->text();
	QUrl u = QUrl::fromEncoded(url.toUtf8());

	QString path = u.path();
	path = path.mid(path.lastIndexOf("/"));
	if(path.isEmpty()) {
		ui->lbInfo->setText("address seems not ok!");
		return;
	}
	path = DOWNLOAD_PATH + path;
	ui->lbPath->setText(path);
	qDebug()<<path;
	m_dl = new DownLoader(url, path, 3, true);
	connect(m_dl, SIGNAL(Info(qint64,qint64,qint64)), this, SLOT(SlotInfo(qint64,qint64,qint64)));
	connect(m_dl, SIGNAL(Over()), this, SLOT(SlotOver()));
	m_dl->Boot();
}
