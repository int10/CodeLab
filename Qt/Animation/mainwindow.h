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
	QPoint GetMousePoint();
signals:
	void startAnimation();
public slots:

	void downloadAnimation();
private slots:
	void on_btnStart_clicked();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
