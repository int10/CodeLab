#include "ImeiListModel.h"

ImeiListModel::ImeiListModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}
ImeiListModel::~ImeiListModel()
{

}

int ImeiListModel::rowCount(const QModelIndex &parent) const
{
	return m_recordList.count();
}

int ImeiListModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}

QVariant ImeiListModel::data(const QModelIndex &index, int role) const
{
	if(index.isValid())
	{
		if(role == Qt::TextAlignmentRole)
		{
			return int(Qt::AlignRight | Qt::AlignVCenter);
		}
		else if(role == Qt::DisplayRole)
		{
			QVariant var = QVariant::fromValue(m_recordList.at(index.row()));
			return var;
		}
	}
	return QVariant();
}

QVariant ImeiListModel::headerData(int section, Qt::Orientation d, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(d == Qt::Vertical)
		return QVariant();

	QVariant var;

	switch(section)
	{
	case 0:var=QString::fromUtf8("imei1"); break;
	case 1:var=QString::fromUtf8("imei2"); break;
	case 2:var=QString::fromUtf8("imei3"); break;
	case 3:var=QString::fromUtf8("imei4"); break;
	}
	return var;

}

void ImeiListModel::addData(sImeiInfo imeiinfo)
{

//	beginResetModel();
	m_recordList.append(imeiinfo);
//	endResetModel();
	reset();
	return;
}


bool ImeiListModel::delData(const QModelIndex &index)
{
//	beginResetModel();
	m_recordList.removeAt(index.row());
//	endResetModel();
	reset();
	return true;
}

QList<sImeiInfo> ImeiListModel::GetAllData()
{
	return m_recordList;
}


