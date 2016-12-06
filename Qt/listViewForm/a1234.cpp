#include "a1234.h"
#include <QDebug>
#include <QListWidgetItem>
#include "devitem.h"
#include <QLabel>

a1234::a1234(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//透明
//	ui.listWidget->setStyleSheet("background-color:transparent");
	ui.listWidget->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

	QLabel *title1=new QLabel(QString("123"));
	QLabel *title2=new QLabel(QString("1234"));
	QLabel *title3=new QLabel(QString("12345"));
	QHBoxLayout *hlay = new QHBoxLayout;
	QWidget *widget = new QWidget(ui.listWidget);
	widget->setLayout(hlay);
	//新的行
//	QListWidgetItem *item = new QListWidgetItem();
//	ui.listWidget->addItem(item);
//	ui.listWidget->setItemWidget(item,widget);
}

a1234::~a1234()
{
	int size = devListItem.size();
	int cnt = devListItem.count();
	for(int i=0;i<size;i++)
	{
		destoryItem(0);
	}
}

void a1234::on_pushButton_clicked()
{
	int cnt = 1;
 	for (int i=0;i<cnt;i++)
 	{
		createNewItem(i);

 	}
	
}
void a1234::createNewItem(uint index)
{
	static int i = 0;
	//新的行
	QListWidgetItem *item = new QListWidgetItem();  
	ui.listWidget->addItem(item); 

	//每个元素绑定布局
	devItem *myItem = new devItem(ui.listWidget,Qt::Window);  
	ui.listWidget->setItemWidget(item,myItem); 
	devListItem.push_back(myItem);
	ListItem.push_back(item);

	//布局元素大小更新
	item->setSizeHint (QSize(myItem->rect().width(),myItem->rect().height()+i));
	i+=20;
	
	//每个元素与外部关联
	itemConnect(index,myItem);
}
void a1234::itemConnect(uint index,devItem* pItem)
{
	//界面和外部交互关联
	connect(ui.pushButton,SIGNAL(clicked()),pItem,SLOT(btnDevName_clicked()));

}
void a1234::destoryItem(uint index)
{
	QListWidgetItem *item  = ListItem.takeAt(index);
	ui.listWidget->removeItemWidget(item);

	ListItem.removeAt(index);
	delete devListItem.at(index);
	devListItem.removeAt(index);

}

void a1234::on_pushButton_2_clicked()
{
	destoryItem(0);
}
