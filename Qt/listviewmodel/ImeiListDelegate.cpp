#include "ImeiListDelegate.h"

ImeiListDelegate::ImeiListDelegate(QObject *parent) :QStyledItemDelegate(parent)
{
	m_tableview = (QTableView*)parent;
}
ImeiListDelegate::~ImeiListDelegate()
{

}

void ImeiListDelegate::paint(QPainter * painter, const QStyleOptionViewItem & opt, const QModelIndex & index ) const
{
	painter->save();

	sImeiInfo data = index.data().value<sImeiInfo>();

	int colindex = index.column();

	if(colindex == 0) //第一列
	{
		QFont sizeFont;
		sizeFont.setPixelSize(18);
		painter->setFont(sizeFont);
		painter->setPen(QColor("#666666"));
		QStyleOptionViewItem size = opt;
		size.displayAlignment=Qt::AlignLeft|Qt::AlignVCenter;
		QApplication::style()->drawItemText(painter,size.rect,size.displayAlignment,QApplication::palette(),true,data.imeilist.at(0));
		painter->setPen(QPen(QColor("#CCCCCC"),1,Qt::DashLine));
	}

	if(colindex==1)//第二列
	{
		QFont sizeFont;
		sizeFont.setPixelSize(18);
		painter->setFont(sizeFont);
		painter->setPen(QColor("#666666"));
		QStyleOptionViewItem size = opt;
		size.displayAlignment=Qt::AlignLeft|Qt::AlignVCenter;
		QApplication::style()->drawItemText(painter,size.rect,size.displayAlignment,QApplication::palette(),true,data.imeilist.at(1));
		painter->setPen(QPen(QColor("#CCCCCC"),1,Qt::DashLine));
	}

	if(colindex==2)//第三列
	{
		QFont sizeFont;
		sizeFont.setPixelSize(18);
		painter->setFont(sizeFont);
		painter->setPen(QColor("#666666"));
		QStyleOptionViewItem size = opt;
		size.displayAlignment=Qt::AlignLeft|Qt::AlignVCenter;
		QApplication::style()->drawItemText(painter,size.rect,size.displayAlignment,QApplication::palette(),true,data.imeilist.at(2));
		painter->setPen(QPen(QColor("#CCCCCC"),1,Qt::DashLine));

	}
	if(colindex==3)//第四列
	{
		QFont sizeFont;
		sizeFont.setPixelSize(18);
		painter->setFont(sizeFont);
		painter->setPen(QColor("#666666"));
		QStyleOptionViewItem size = opt;
		size.displayAlignment=Qt::AlignLeft|Qt::AlignVCenter;
		QApplication::style()->drawItemText(painter,size.rect,size.displayAlignment,QApplication::palette(),true,data.imeilist.at(3));
		painter->setPen(QPen(QColor("#CCCCCC"),1,Qt::DashLine));

	}



	painter->restore();
}
bool ImeiListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	sImeiInfo data =index.data().value<sImeiInfo>();

	if(event->type() == QEvent::MouseButtonDblClick)
	{
		return ((ImeiListModel *)model)->delData(index);
	}
	return true;
}
