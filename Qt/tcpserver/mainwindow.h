#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void readMessage();
	void sendMessage();
	void newConnect();
	void startTcpserver();


private:
	Ui::MainWindow *ui;

	QTcpServer *m_tcpServer;
	QTcpSocket *m_tcpSocket;
};

#endif // MAINWINDOW_H
