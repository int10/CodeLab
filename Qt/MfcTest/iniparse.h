#ifndef INIPARSE_H
#define INIPARSE_H

#include <QObject>



#include <QString>
#include <QList>
#include <QFile>

typedef struct
{

	QByteArray key;
	QByteArray value;
}IniItem;


typedef struct
{
	QByteArray SectionName;
	QList<IniItem> itemlist;
}IniSection;

typedef struct
{
	QList<IniSection> SectionList;
}IniStruct;



class IniParse : public QObject
{
	Q_OBJECT
public:
	explicit IniParse(QObject *parent = 0);
	bool Parse(QString filename);
	bool Save();
	QByteArray GetValue(QByteArray section,QByteArray key);
	void SetValue(QByteArray section,QByteArray key,QByteArray value);
	
signals:
	
public slots:
private:
	IniStruct m_inistruct;
	QFile m_file;
	
};

#endif // INIPARSE_H
