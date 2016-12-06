#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);


	model = new QStringListModel(this);
	 QStringList data;
	 data << "Letter A" << "Letter B" << "Letter C";
	 model->setStringList(data);

	ui->listView->setModel(model);

	 connect(ui->insertBtn, SIGNAL(clicked()), this, SLOT(insertData()));
	 connect(ui->delBtn, SIGNAL(clicked()), this, SLOT(deleteData()));
	 connect(ui->showBtn, SIGNAL(clicked()), this, SLOT(showData()));






}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::insertData()
{
		bool isOK;
		QString text = QInputDialog::getText(NULL, "Insert", "Please input new data:",
																				 QLineEdit::Normal, "You are inserting new data.", &isOK);
		if(isOK) {
				int row = ui->listView->currentIndex().row();
				model->insertRows(row, 1);
				QModelIndex index = model->index(row);
				model->setData(index, text);
				ui->listView->setCurrentIndex(index);
				ui->listView->edit(index);
		}
}

void MainWindow::deleteData()
{
		if(model->rowCount() > 1) {
				model->removeRows(ui->listView->currentIndex().row(), 1);
		}
}

void MainWindow::showData()
{
		QStringList data = model->stringList();
		QString str;
		foreach(QString s, data) {
				str += s + "\n";
		}

		QMessageBox::information(this, "Data", str);
}
