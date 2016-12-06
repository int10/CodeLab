#ifndef DEVITEM_H
#define DEVITEM_H

#include <QWidget>
#include "ui_devitem.h"

class devItem : public QWidget
{
	Q_OBJECT

public:
	devItem(QWidget *parent = 0,Qt::WindowFlags f=0);
	~devItem();

	Ui::devItem ui;
	

public slots:
	void btnDevName_clicked();
private slots:
	void on_btnDevName_clicked();
};

#endif // DEVITEM_H
