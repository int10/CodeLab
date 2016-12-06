#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpServer>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	m_server = new QTcpServer(this);
	connect(m_server, SIGNAL(newConnection()), this, SLOT(SlotNewConnection()));
	m_server->listen(QHostAddress::Any, 59980);


//	QByteArray data = "GET /main.apk HTTP/1.1\n";
//	data += "Host: 127.0.0.1:59980\n";
//	data += "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:23.0) Gecko/20100101 Firefox/23.0\n";
//	data += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n";
//	data += "Accept-Language: en-US,en;q=0.5\n";
//	data += "Accept-Encoding: gzip, deflate\n";
//	data += "Connection: keep-alive\n\n\n\n";


//	QEventLoop loop;
//	m_cli = new QTcpSocket(this);
//	connect(m_cli, SIGNAL(connected()), &loop, SLOT(quit()));
//	m_cli->connectToHost("127.0.0.1", 59980);
//	loop.exec();

//	m_cli->write(data);

//	QTimer tt;
//	tt.setSingleShot(true);
//	tt.start(1000);

//	connect(&tt, SIGNAL(timeout()), &loop, SLOT(quit()));
//	loop.exec();
//	data = m_cli->readAll();
//	qDebug()<<data.size();
//	qDebug()<<data;


}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::SlotNewConnection()
{
	if(m_server->hasPendingConnections())
	{
		m_socket = m_server->nextPendingConnection();
		connect(m_socket, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
		connect(m_socket, SIGNAL(disconnected()), this, SLOT(SlotDisconnected()));
		connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(SlotError(QAbstractSocket::SocketError)));
		connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(SlotStateChange(QAbstractSocket::SocketState)));
	}
}

void MainWindow::SlotDisconnected()
{
qDebug()<<"disconnected";
}
void MainWindow::SlotError(QAbstractSocket::SocketError error)
{
	qDebug()<<"error:"<<error;
}
void MainWindow::SlotStateChange(QAbstractSocket::SocketState state)
{
	qDebug()<<"state:"<<state;
}

void MainWindow::SlotReadyRead()
{
	QHttp http;
	http.setSocket(m_socket);
	//qDebug()<<m_socket->readAll();
	qDebug()<<http.readAll();
	QHttpResponseHeader head;
	head = http.lastResponse();
	qDebug()<<head.keys();


	QFile file("d:/ee.zip");

	if(!file.open(QIODevice::ReadOnly))
	{
		QString result = send_error(404, "aa", "bb", "cc");
		m_socket->write(result.toAscii());
		m_socket->flush();
		m_socket->abort();
		m_socket->close();
		m_socket->deleteLater();
		return;
	}

	QString result;
	result.sprintf("mime type = %s\n", get_mime_type(file.fileName().toAscii().data()));
	qDebug()<<result;
	//m_socket->write(result.toAscii());
//	qDebug()<<get_mime_type(file.fileName().toAscii().data());
	result = send_headers(200, "OK", (char*) 0, get_mime_type(file.fileName().toAscii().data()), file.size(), 514541);
	qDebug()<<result;
	//m_socket->write(result.toAscii());

	while(1)
	{
		QByteArray data = file.read(1024*1024);
		if(data.size() > 0){
			m_socket->write(data);
			m_socket->waitForBytesWritten(5000);
		}else{
			break;
		}

	}
//	QTimer tt;
//	QEventLoop loop;
//	tt.setSingleShot(true);
//	connect(&tt, SIGNAL(timeout()), &loop, SLOT(quit()));
//	tt.start(1000);
//	loop.exec();
//	qDebug()<<"555555555555";
	m_socket->flush();
	m_socket->abort();
	m_socket->close();
	m_socket->deleteLater();
	m_socket = NULL;
	qDebug()<<"end";
}


QString MainWindow::send_error( int status, char* title, char* extra_header, char* text )
{
	QString result, tmp;
	result = send_headers( status, title, extra_header, "text/html", -1, -1 );
	tmp.sprintf( "\
				   <!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n\
				   <html>\n\
				   <head>\n\
				   <meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\">\n\
			<title>%d %s</title>\n\
			</head>\n\
			<body bgcolor=\"#cc9999\">\n\
			<h4>%d %s</h4>\n", status, title, status, title );

			result += tmp;
			tmp.sprintf( "%s\n", text );
	result += tmp;
	tmp.sprintf( "\
				<hr>\n\
				<address><a href=\"%s\">%s</a></address>\n\
			</body>\n\
			</html>\n", SERVER_URL, SERVER_NAME );
			result += tmp;
			return result;
}


QString MainWindow::send_headers( int status, char* title, char* extra_header, char* mime_type, off_t length, time_t mod )
{
	QString result, tmp;
	time_t now;
	char timebuf[100];

	tmp.sprintf( "%s %d %s\015\012", PROTOCOL, status, title );
	m_socket->write(tmp.toAscii());
	result += tmp;
	tmp.sprintf( "Server: %s\015\012", SERVER_NAME );
	m_socket->write(tmp.toAscii());
	result += tmp;
	now = time( (time_t*) 0 );
	(void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
	tmp.sprintf( "Date: %s\015\012", timebuf );
	m_socket->write(tmp.toAscii());
	result += tmp;
	if ( extra_header != (char*) 0 ){
		tmp.sprintf( "%s\015\012", extra_header );
		m_socket->write(tmp.toAscii());
		result += tmp;
	}
	if ( mime_type != (char*) 0 ){
		tmp.sprintf( "Content-Type: %s\015\012", mime_type );
		m_socket->write(tmp.toAscii());
		result += tmp;
	}
	if ( length >= 0 ){
		tmp.sprintf( "Content-Length: %lld\015\012", (long long) length );
		m_socket->write(tmp.toAscii());
		result += tmp;
	}
	if ( mod != (time_t) -1 )
	{
		(void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &mod ) );
		tmp.sprintf( "Last-Modified: %s\015\012", timebuf );
		m_socket->write(tmp.toAscii());
		result += tmp;
	}
	tmp.sprintf( "Connection: close\015\012" );
	m_socket->write(tmp.toAscii());
	result += tmp;
	tmp.sprintf( "\015\012" );
	m_socket->write(tmp.toAscii());
	result += tmp;
	return result;
}



char * MainWindow::get_mime_type( char * name )
{
	char* dot;

	dot = strrchr( name, '.' );
	if ( dot == (char*) 0 )
		return "text/plain; charset=UTF-8";
	if ( strcmp( dot, ".html" ) == 0 || strcmp( dot, ".htm" ) == 0 )
		return "text/html; charset=UTF-8";
	if ( strcmp( dot, ".xhtml" ) == 0 || strcmp( dot, ".xht" ) == 0 )
		return "application/xhtml+xml; charset=UTF-8";
	if ( strcmp( dot, ".jpg" ) == 0 || strcmp( dot, ".jpeg" ) == 0 )
		return "image/jpeg";
	if ( strcmp( dot, ".gif" ) == 0 )
		return "image/gif";
	if ( strcmp( dot, ".png" ) == 0 )
		return "image/png";
	if ( strcmp( dot, ".css" ) == 0 )
		return "text/css";
	if ( strcmp( dot, ".xml" ) == 0 || strcmp( dot, ".xsl" ) == 0 )
		return "text/xml; charset=UTF-8";
	if ( strcmp( dot, ".au" ) == 0 )
		return "audio/basic";
	if ( strcmp( dot, ".wav" ) == 0 )
		return "audio/wav";
	if ( strcmp( dot, ".avi" ) == 0 )
		return "video/x-msvideo";
	if ( strcmp( dot, ".mov" ) == 0 || strcmp( dot, ".qt" ) == 0 )
		return "video/quicktime";
	if ( strcmp( dot, ".mpeg" ) == 0 || strcmp( dot, ".mpe" ) == 0 )
		return "video/mpeg";
	if ( strcmp( dot, ".vrml" ) == 0 || strcmp( dot, ".wrl" ) == 0 )
		return "model/vrml";
	if ( strcmp( dot, ".midi" ) == 0 || strcmp( dot, ".mid" ) == 0 )
		return "audio/midi";
	if ( strcmp( dot, ".mp3" ) == 0 )
		return "audio/mpeg";
	if ( strcmp( dot, ".ogg" ) == 0 )
		return "application/ogg";
	if ( strcmp( dot, ".pac" ) == 0 )
		return "application/x-ns-proxy-autoconfig";
	return "text/plain; charset=UTF-8";
}
