#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DownLoader.h"

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
	void SlotInfo(qint64 speed, qint64 recvsize, qint64 filesize);
	void SlotOver();
private slots:
	void on_btnAdd_clicked();

private:
	Ui::MainWindow *ui;
	DownLoader *m_dl;
};

#endif // MAINWINDOW_H
