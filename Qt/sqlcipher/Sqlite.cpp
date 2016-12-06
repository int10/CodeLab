#include "Sqlite.h"

Sqlite::Sqlite(QObject *parent) :
	QObject(parent)
{
	m_db = NULL;
}

Sqlite::~Sqlite()
{
	Close();
}

bool Sqlite::Init(QString database, QString key)
{
	if(m_db){
		Close();
	}
	if(sqlite3_open(database.toUtf8().data(), &m_db) == 0){
		printf("%s\n", key.toAscii().data());
		if(!key.isEmpty()) sqlite3_key(m_db, key.toAscii().data(), key.size());
		return true;
	}
	else
	{
		m_db = NULL;
		return false;
	}
}

void Sqlite::Close()
{
	if(m_db){
		sqlite3_close(m_db);
		m_db = NULL;
	}
}

int Sqlite::GetData(QByteArray sql, QList<QMap<QString, QByteArray> > &result)
{
	result.clear();

	if(NULL == m_db) return 0;

	char **dbresult;int nrow, ncolumn;
	char * zErrMsg = 0;
	if(SQLITE_OK != sqlite3_get_table(m_db, sql.data(), &dbresult, &nrow, &ncolumn, &zErrMsg))
	{
		sqlite3_free(zErrMsg);
	}
	else
	{
		result.clear();
		int i, j, index = ncolumn;
		for( i = 0; i < nrow; i++ )
		{
			QMap<QString, QByteArray> rowdata;
			for( j = 0; j < ncolumn; j++ )
			{
				rowdata[dbresult[j]] = QByteArray(dbresult[index]);
				++index; // dbResult 的字段值是连续的，从第0索引到第 nColumn - 1索引都是字段名称，从第 nColumn 索引开始，后面都是字段值，它把一个二维的表（传统的行列表示法）用一个扁平的形式来表示 }

			}
			result.append(rowdata);
		}
	#if 0		//打印结果
		for(int i = 0; i < result.count(); i++)
		{
			QMap<QString, QByteArray> rowdata = result.at(i);
			QStringList keys = rowdata.keys();
			foreach(QString key, keys)
			{
				qDebug()<<key<<":"<<rowdata[key];
			}
			qDebug()<<"--------------------------------";

		}
	#endif
	}
	//到这里，不论数据库查询是否成功，都释放 char** 查询结果
	sqlite3_free_table( dbresult );
	return result.count();
}

int Sqlite::Exec(QByteArray sql)
{
	char * zErrMsg = 0;
	if(SQLITE_OK != sqlite3_exec(m_db, sql, NULL, 0, &zErrMsg))
	{
		sqlite3_free(zErrMsg);
	}
	return 0;
}


