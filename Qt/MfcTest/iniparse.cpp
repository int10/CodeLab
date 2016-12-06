//////////////////creat by int10@131211
///////////ini文件解析///////////////
//////ps:不支持注释，ini文件里不能有不必要的字符。。
//////只支持标准的ini文件。。就是必须包含有section,key,value的项才能解析出来。。

#include "iniparse.h"
#include <QDebug>


IniParse::IniParse(QObject *parent) :
	QObject(parent)
{
}

//////////////////////////解析ini文件，在进行get_value或set_value之前，必须先调用这个函数。。。。
bool IniParse::Parse(QString filename)
{
	m_file.setFileName(filename);
	if(m_file.open(QFile::ReadOnly) ==false)
		return false;

	QByteArray file_content = m_file.readAll();
	QList<QByteArray> sectionlist = file_content.split('[');

	//split the content by  "[",then can get each section's cotent
	foreach(QByteArray section,sectionlist)
	{
		IniSection ini_section;
		QList<QByteArray> split_section_content = section.split(']');
		if(split_section_content.count()<=1)
			continue;
		ini_section.SectionName = split_section_content.at(0);
		ini_section.SectionName = ini_section.SectionName.simplified();

		QByteArray section_content = split_section_content.at(1);
		QList<QByteArray> split_item = section_content.split('\n');
		////get each key & value in this section;
		foreach(QByteArray item,split_item)
		{
			IniItem ini_item;
			QList<QByteArray> item_content = item.split('=');
			if(item_content.count()<=1)
				continue;
			ini_item.key = item_content.at(0);
			ini_item.value= item_content.at(1);
			ini_item.key = ini_item.key.simplified();
			ini_item.value = ini_item.value.simplified();
			ini_section.itemlist.append(ini_item);
		}
		m_inistruct.SectionList.append(ini_section);
	}
	m_file.close();

	return true;
}
////////////////保存为文件，所有的set操作都不是直接的文件操作，只有在调用了这个函数后，所作的改动才会保留。。
bool IniParse::Save()
{

	if(!(m_file.open(QFile::WriteOnly)))
		return false;

	foreach(IniSection section,m_inistruct.SectionList)
	{
		m_file.write("["+section.SectionName+"]\n");

		foreach(IniItem item,section.itemlist)
		{
			m_file.write(item.key+"="+item.value+"\n");
		}
	}
	m_file.close();


	foreach(IniSection section,m_inistruct.SectionList)
	{
		qDebug()<<"["<<section.SectionName<<"]";
		foreach(IniItem item,section.itemlist)
		{
			qDebug()<<item.key<<"="<<item.value;
		}
	}

	return true;
}

QByteArray IniParse::GetValue(QByteArray section,QByteArray key)
{
	IniSection ini_section;
	IniItem ini_item;
	int i;
	for(i = 0;i<m_inistruct.SectionList.count();i++)
	{
		ini_section = m_inistruct.SectionList.at(i);
		if(ini_section.SectionName == section)
			break;
	}
	if(i>=m_inistruct.SectionList.count())
		return "";


	for(i =0 ;i<ini_section.itemlist.count();i++)
	{
		ini_item = ini_section.itemlist.at(i);
		if(ini_item.key == key)
			break;
	}
	if(i>=ini_section.itemlist.count())
		return "";

	return ini_item.value;

}


//////////修改设定值，如果不存在，就创建一个，如果value为空，删除对应项。
void IniParse::SetValue(QByteArray section,QByteArray key,QByteArray value)
{
	IniSection ini_section;
	IniItem ini_item;
	int section_index,item_index;
	for(section_index = 0;section_index<m_inistruct.SectionList.count();section_index++)
	{
		ini_section = m_inistruct.SectionList.at(section_index);
		if(ini_section.SectionName == section)
			break;
	}
	////////没找到这个section，全部新建。。
	if(section_index>=m_inistruct.SectionList.count())
	{
		if(value.isEmpty())
			return;
		IniSection newSection;
		IniItem newItem;
		newSection.SectionName = section;
		newItem.key = key;
		newItem.value = value;
		newSection.itemlist.append(newItem);
		m_inistruct.SectionList.append(newSection);
		return;
	}
	/////找到section ,找key。。。
	for(item_index =0 ;item_index<ini_section.itemlist.count();item_index++)
	{
		ini_item = ini_section.itemlist.at(item_index);
		if(ini_item.key == key)
			break;
	}
	if(item_index>=ini_section.itemlist.count())		//找不到key，新建一个item;
	{
		if(value.isEmpty())
			return;
		IniItem newItem;
		newItem.key = key;
		newItem.value = value;
		m_inistruct.SectionList[section_index].itemlist.append(newItem);
		return;
	}
	////找到key，直接修改item。。
	if(value.isEmpty())
		m_inistruct.SectionList[section_index].itemlist.removeAt(item_index);
	else
		m_inistruct.SectionList[section_index].itemlist[item_index].value = value;
	return;


}
