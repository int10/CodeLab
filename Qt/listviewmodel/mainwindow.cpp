#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_imeilistmodel = new ImeiListModel;

	ImeiListDelegate *imeilistdelegate = new ImeiListDelegate;
	ui->tableView->setModel(m_imeilistmodel);
	ui->tableView->setItemDelegate(imeilistdelegate);

	ui->tableView->setColumnWidth(0,200);
	ui->tableView->setColumnWidth(1,200);
	ui->tableView->setColumnWidth(2,200);
	ui->tableView->setColumnWidth(3,200);
//	ui->tableView->verticalHeader()->setDefaultSectionSize(60);
	for(int i = 0;i<10000;i++)
	{
	sImeiInfo info;
	info.guid = "000000";
	info.imeilist<<QString::number(i)<<QString::number(i+1)<<QString::number(i+2)<<QString::number(i+3);
	m_imeilistmodel->addData(info);
	}

}

MainWindow::~MainWindow()
{
	delete ui;
}
