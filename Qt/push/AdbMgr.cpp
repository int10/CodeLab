#include "AdbMgr.h"
#include <QProcess>
#include <QDebug>
#include <QByteArray>

QString AdbMgr::Run(QString cmd, QString sn, int timeout)
{
    QProcess * excpro = new QProcess;
    QString result = "Fail";
    QString exccmd;

#if 1
//    QString adbpath = DocumentMgr::GetDirPathOfBin() + "/";
	QString adbpath = "";
	exccmd = (sn != "")?("\""+adbpath + "adb\" -s " + sn + " "+ cmd):("\""+adbpath + "adb\" " + cmd);
	qDebug()<<exccmd;
#else
	QString arg = (sn != "")?("-s " + sn + " " + cmd):(cmd);
	arg = Common::EncryptArgument(arg);
	exccmd = "\"" + DocumentMgr::GetDirPathOfBin() + "/adb\"" + " D " + arg;
#endif


    excpro->setProcessChannelMode(QProcess::MergedChannels);
    excpro->start(exccmd);

    if(excpro->waitForStarted())
    {
        excpro->closeWriteChannel();
        timeout = (timeout > 0)?(timeout):(-1);
        if(excpro->waitForFinished(timeout))
        {
			result = QString::fromUtf8(excpro->readAll());
//			result = excpro->readAll();
        }
    }

    excpro->kill();
    excpro->deleteLater();

    return result;
}

QString AdbMgr::ShellRun(QString cmd, QString sn, bool issu, int timeout)
{
    QProcess * excpro = new QProcess;
    QString result = "Fail";
    QString exccmd;


	#if 1
//    QString adbpath = DocumentMgr::GetDirPathOfBin() + "/";
	QString adbpath = "";
	exccmd = (sn != "")?("\""+adbpath + "adb\" -s " + sn + " shell "):("\""+adbpath + "adb\" shell ");
//    if(issu)
//        exccmd += DocumentMgr::GetFilePathOfPhoneSU()+" -c \""+cmd+"\"";
//    else
        exccmd += cmd;
	#else
	QString arg = (sn != "")?("-s " + sn + " shell"):("shell");
	if(issu)
		arg += " " + DocumentMgr::GetFilePathOfPhoneSU()+" -c \""+cmd+"\"";
	else
		arg += " " + cmd;
	arg = Common::EncryptArgument(arg);
	exccmd = "\"" + DocumentMgr::GetDirPathOfBin() + "/adb\"" + " D " + arg;

	#endif

    excpro->setProcessChannelMode(QProcess::MergedChannels);
    excpro->start(exccmd);

    if(excpro->waitForStarted())
    {

        excpro->closeWriteChannel();
        timeout = (timeout > 0)?(timeout):(-1);
        if(excpro->waitForFinished(timeout))
        {
            result = excpro->readAll();
        }
    }

    excpro->kill();
    excpro->deleteLater();

    return result;
}

QString AdbMgr::CmdRun(QString cmd, int timeout, QString standOutputfile)
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
        excpro->closeWriteChannel();
        timeout = (timeout > 0)?(timeout):(-1);
        if(excpro->waitForFinished(timeout))
        {
            result = excpro->readAll();
        }
    }

    excpro->kill();
    excpro->deleteLater();

    return result;
}
