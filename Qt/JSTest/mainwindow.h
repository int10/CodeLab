#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebView>
#include <QWebFrame>

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

	void onCall(QString strName,QString strPwd);
	void setValueFromWeb(const QString &strName,const QString &strPwd);
	void addObject();
private slots:
	void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;
	QWebFrame *wf;
};

#endif // MAINWINDOW_H
