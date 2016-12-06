#ifndef IMEILISTDELEGATE_H
#define IMEILISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QTableView>
#include "ImeiListModel.h"
#include <QEvent>
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

class ImeiListDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit ImeiListDelegate(QObject * parent = 0);
	~ImeiListDelegate();

	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
	QTableView * m_tableview;

};

#endif // IMEILISTDELEGATE_H
