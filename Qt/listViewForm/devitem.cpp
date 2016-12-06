#include "devitem.h"
#include <QDebug>

devItem::devItem(QWidget *parent,Qt::WindowFlags f)
	: QWidget(parent,f)
{
	ui.setupUi(this);

}

devItem::~devItem()
{

}
void devItem::btnDevName_clicked()
{
//	ui.isEn->click();
}

void devItem::on_btnDevName_clicked()
{
	qDebug()<<"111111111";
}
