#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	tt = NULL;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	if(tt) return;
	tt = new Thread();
	tt->Boot();
}

void MainWindow::on_pushButton_2_clicked()
{
	if(!tt) return;
	delete tt;
	tt = NULL;
}

void MainWindow::on_pushButton_3_clicked()
{
	if(!tt) return;
	tt->StopTimer();
}
