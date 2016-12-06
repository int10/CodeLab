#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlite3.h"
#include "sqlite3ext.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	sqlite3 *sql;
}

MainWindow::~MainWindow()
{
	delete ui;
}
