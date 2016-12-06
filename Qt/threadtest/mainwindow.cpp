#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <QtConcurrentRun>



typedef void (MainWindow::*cbTest)(bool test);

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(this, SIGNAL(SignalMd5Result(bool)), this, SLOT(SlotMd5Result(bool)));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::Md5(bool test)
{
//	qDebug()<<test;
	emit SignalMd5Result(test);
}

void MainWindow::SlotMd5Result(bool result)
{
	qDebug()<<result;
}

static void cbMd5test(MainWindow *win, bool test, cbTest func)
{
	(win->*func)(test);
}

void MainWindow::on_pushButton_clicked()
{
	cbTest tt = &MainWindow::Md5;
//	(this->*tt)(true);


	QtConcurrent::run(cbMd5test, this, true, tt);
}
