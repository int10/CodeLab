#include "common.h"
#include <QProcess>

Common::Common(QObject *parent) :
	QObject(parent)
{
}
QString Common::CmdRun(QString cmd, int timeout, QString standOutputfile,bool waitfinish)
{
	QProcess * excpro = new QProcess;
	QString result = "Fail";
	QString exccmd;

	exccmd = cmd;

	if(!standOutputfile.isEmpty()){
		excpro->setStandardOutputFile(standOutputfile);
	}

	excpro->setProcessChannelMode(QProcess::MergedChannels);
	excpro->start(exccmd);

	if(excpro->waitForStarted())
	{
		if(waitfinish == true)
		{
			excpro->closeWriteChannel();
			timeout = (timeout > 0)?(timeout):(-1);
			if(excpro->waitForFinished(timeout))
			{
				result = excpro->readAll();
			}
		}

	}
	if(waitfinish == true)
	{
		excpro->kill();
		excpro->deleteLater();
		result = "sucess";
	}


	return result;
}
