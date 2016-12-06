#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebElement>
#include <QWebFrame>
#include <QDebug>
#include <QNetworkProxy>
#include <QTimer>
#include <QFile>

namespace Ui {
class MainWindow;
}

typedef enum
{
	E_WEB_FRAME_UNKNOWN,
	E_WEB_FRAME_LICENSE,
	E_WEB_FRAME_HUAWEI_LOGIN,
	E_WEB_FRAME_FACEBOOK_LOGIN,
	E_WEB_FRAME_UNLOCK_DETAIN,
	E_WEB_FRAME_REDIRECTING
}eWebFrameType;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void SlotWebViewFinished(bool);
	void on_btnSubmit_clicked();
	void on_btnSave_clicked();
	void on_btnGetKey_clicked();
	void on_btnNext_clicked();
	void on_btnInput_clicked();
	void on_btnFacebook_clicked();
	void on_btnLogin_clicked();

	void on_btnSmartphone_clicked();
//	void slot_NetFinished(QNetworkReply*);
	void on_btnRefresh_clicked();

private:
	void ShowVerifyGif();
private:
	Ui::MainWindow *ui;

//	QNetworkAccessManager *m_netmanager;
//	QNetworkReply *m_reply;
};

#endif // MAINWINDOW_H
