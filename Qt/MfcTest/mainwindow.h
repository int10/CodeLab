#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "remotecontrol.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void MfcTest();
	RemoteControl m_control;
private slots:
	void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;
	ConItem m_com[16];
	ConItem m_btnstart[16];
	ConItem m_btnstop[16];

};

#endif // MAINWINDOW_H
