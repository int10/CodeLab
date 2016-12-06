#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <QObject>
#include <Windows.h>


#ifdef UNICODE
#define QStringToTCHAR(x)     (wchar_t*) x.utf16()
#define PQStringToTCHAR(x)    (wchar_t*) x->utf16()
#define TCHARToQString(x)     QString::fromUtf16((x))
#define TCHARToQStringN(x,y)  QString::fromUtf16((x),(y))
#else
#define QStringToTCHAR(x)     x.local8Bit().constData()
#define PQStringToTCHAR(x)    x->local8Bit().constData()
#define TCHARToQString(x)     QString::fromLocal8Bit((x))
#define TCHARToQStringN(x,y)  QString::fromLocal8Bit((x),(y))
#endif


typedef struct
{
	QString title;
	QString classname;
	HWND hwnd;
}ConItem;

class RemoteControl : public QObject
{
	Q_OBJECT
public:
	explicit RemoteControl(QObject *parent = 0);
	bool FindWindow(QString title);
	void RemoveControl(int id);
	void SetControlText(int id, QString text);
	void SetControlCheck(int id,bool check);		//assuming the control is check box
	QList<ConItem> EnumControl();
	int ClickCtrl(int id);
	int ClickCtrl(HWND wnd);
	int SetControlCheck(HWND hwnd,bool check);
	QList<ConItem> m_ConItem;
public:
	HWND m_hWnd;

signals:
	
public slots:
	
};

#endif // REMOTECONTROL_H
