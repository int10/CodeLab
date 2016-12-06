#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
signals:
	void SignalMd5Result(bool result);
private slots:
	void on_pushButton_clicked();
	void SlotMd5Result(bool result);

private:
	Ui::MainWindow *ui;
	int m_test;
	void Md5(bool test);
};

#endif // MAINWINDOW_H
