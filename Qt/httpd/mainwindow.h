#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <time.h>
#include <QFile>
#include <QTimer>
#include <QHttp>
#include <QHttpResponseHeader>

namespace Ui {
class MainWindow;
}

#define SERVER_NAME "micro_httpd"
#define SERVER_URL "http://www.acme.com/software/micro_httpd/"

#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QString send_headers( int status, char* title, char* extra_header, char* mime_type, off_t length, time_t mod );
	QString send_error( int status, char* title, char* extra_header, char* text );
	char *get_mime_type(char *name );
public slots:
	void SlotNewConnection();
	void SlotReadyRead();
	void SlotDisconnected();
	void SlotError(QAbstractSocket::SocketError error);
	void SlotStateChange(QAbstractSocket::SocketState state);
private:
	Ui::MainWindow *ui;
	QTcpServer *m_server;
	QTcpSocket *m_socket;
	QTcpSocket *m_cli;
};

#endif // MAINWINDOW_H
