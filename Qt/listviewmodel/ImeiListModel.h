#ifndef IMEILISTMODEL_H
#define IMEILISTMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

//typedef struct
//{
//	QString guid;
//	QStringList imeilist;
//}sImeiInfo;

class sImeiInfo
{
public:
	QString guid;
	QStringList imeilist;
};

Q_DECLARE_METATYPE(sImeiInfo)

class ImeiListModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit ImeiListModel(QObject *parent = 0);
	~ImeiListModel();

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation d, int role) const;

	void addData(sImeiInfo imeiinfo);
	bool delData(const QModelIndex &index);
	QList<sImeiInfo> GetAllData();

signals:

public slots:
private:
	QList<sImeiInfo> m_recordList;

};

#endif // IMEILISTMODEL_H
