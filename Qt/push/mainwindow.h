#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
protected:
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
private slots:
	void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;
	QFileInfo m_srcFile;
};

#endif // MAINWINDOW_H
