#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebView>
#include <QWebFrame>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->webView->setUrl(QUrl("form.html"));
	//ui->webView->page()->mainFrame()->addToJavaScriptWindowObject(QString("mywebkit"),this);
	wf = ui->webView->page()->mainFrame();
	addObject();


	connect(wf, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addObject()));
}


MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::addObject()
{
	wf->addToJavaScriptWindowObject(QString("mywebkit"),this);
}

void MainWindow::on_pushButton_clicked()
{
	QString strVal = QString("callfromqt(\"%1\",\"%2\");").arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text() );
	 // ui->webView->page()->mainFrame()->evaluateJavaScript(strVal);
	wf->evaluateJavaScript(strVal);
}

void MainWindow::onCall(QString strName,QString strPwd)
{

	ui->lineEdit->setText(strName);
	ui->lineEdit_2->setText(strPwd);
}

void MainWindow::setValueFromWeb(const QString &strName,const QString &strPwd)
{

}
