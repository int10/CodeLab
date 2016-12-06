#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->connect(ui->pushButton_start,SIGNAL(clicked()),this,SLOT(startTcpserver()));
	this->connect(ui->pushButton_send,SIGNAL(clicked()),this,SLOT(sendMessage()));
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::startTcpserver()
{
	m_tcpServer = new QTcpServer(this);
	m_tcpServer->listen(QHostAddress::Any,19999); //监听任何连上19999端口的ip
	connect(m_tcpServer,SIGNAL(newConnection()),this,SLOT(newConnect())); //新连接信号触发，调用newConnect()槽函数，这个跟信号函数一样，其实你可以随便取。
}

void MainWindow::newConnect()
{
	m_tcpSocket = m_tcpServer->nextPendingConnection(); //得到每个连进来的socket
	connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(readMessage())); //有可读的信息，触发读函数槽
}


void MainWindow::readMessage()
{
//    ui->textEdit_rec->te
	QByteArray qba= m_tcpSocket->readAll(); //读取
	qDebug()<<qba;
	QString ss=QVariant(qba).toString();
	ui->textEdit_rec->setText(ss);
}

void MainWindow::sendMessage() //发送信息
{
	QString strMesg= ui->lineEdit_sendmessage->text();
	qDebug()<<strMesg;
	m_tcpSocket->write(strMesg.toStdString().c_str(),strlen(strMesg.toStdString().c_str())); //发送
}

