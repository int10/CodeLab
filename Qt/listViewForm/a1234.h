#ifndef A1234_H
#define A1234_H


#include <QMainWindow>
#include <QList>
#include "devitem.h"
#include "ui_a1234.h"

class a1234 : public QMainWindow
{
	Q_OBJECT

public:
	a1234(QWidget *parent = 0);
	~a1234();

public:
	void createNewItem(uint index);
	void destoryItem(uint index);
	void itemConnect(uint index,devItem* pItem);


public slots:
	void on_pushButton_clicked();

private slots:
	void on_pushButton_2_clicked();

private:
	Ui::a1234Class ui;
	QList<devItem*> devListItem;
	QList<QListWidgetItem*> ListItem;
};

#endif // A1234_H
