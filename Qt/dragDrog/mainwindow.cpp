#include "mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	textEdit = new QTextEdit;
	setCentralWidget(textEdit);

	textEdit->setAcceptDrops(true);
	setAcceptDrops(false);

	setWindowTitle(tr("Text Editor"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) {
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	QWidget *foin = event->source();
	//foin->setVisible(false);
	((QTextEdit *)foin)->setText("aaaaa");


	if (urls.isEmpty()) {
		return;
	}

	QString fileName = urls.first().toLocalFile();
	if (fileName.isEmpty()) {
		return;
	}
	qDebug()<<"filename="<<fileName;

	if (readFile(fileName)) {
		setWindowTitle(tr("%1 - %2").arg(fileName, tr("Drag File")));
	}
}

bool MainWindow::readFile(const QString &fileName)
{
	bool r = true;
//	QFile file(fileName);
//	QTextStream in(&file);
//	QString content;
//	if(file.open(QIODevice::ReadOnly)) {
//		in >> content;
//		r = true;
//	}
//	textEdit->setText(content);
	return r;
}
