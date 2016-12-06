#include "remotecontrol.h"
#include "Windows.h"
#include <QDebug>

QList<ConItem> gConItem;
RemoteControl::RemoteControl(QObject *parent) :
	QObject(parent)
{
}

bool RemoteControl::FindWindow(QString title)
{


	for(int i = 0;i<5000;i++)
	{
		m_hWnd = ::FindWindowA(NULL,("SmartPhone Multi-port download tool (After-Sale Service Center)"));
		if(m_hWnd != NULL)
			return true;
	}


	qDebug()<<"Can't find window";
	return false;
}

void RemoteControl::RemoveControl(int id)
{
	HWND cid = ::GetDlgItem(m_hWnd,id);
	if(cid == NULL)
		return;

	::ShowWindow(cid,SW_HIDE);

}

void RemoteControl::SetControlText(int id,QString text)
{
	HWND cid = ::GetDlgItem(m_hWnd,id);
	if(cid == NULL)
		return;
	::SendMessage( cid ,WM_SETTEXT,0,(LPARAM)text.toStdWString().data());
}


void RemoteControl::SetControlCheck(int id,bool check)
{
	HWND cid = ::GetDlgItem(m_hWnd,id);
	if(cid == NULL)
		return;
	::CheckDlgButton(m_hWnd,id,check);
}

int RemoteControl::ClickCtrl(int id)
{
	HWND ctrl = ::GetDlgItem(m_hWnd, id);
	if(!ctrl)
		return -1;

	PostMessage(ctrl, BM_CLICK, 0, 0);
	return 0;
}

BOOL __stdcall EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR ClassName[100]={0};
	::GetClassName(hWnd,ClassName,sizeof(ClassName)/sizeof(TCHAR));

	TCHAR WindowText[100]={0};
	::SendMessage(hWnd,WM_GETTEXT,(WPARAM)(sizeof(WindowText)/sizeof(TCHAR)),(LPARAM)WindowText);

	ConItem item;
	item.classname = TCHARToQString(ClassName);
	item.title = TCHARToQString(WindowText);
	item.hwnd = hWnd;
	gConItem.append(item);
	return TRUE ;
}


int RemoteControl::ClickCtrl(HWND wnd)
{
	return 0;
}

QList<ConItem> RemoteControl::EnumControl()
{
	gConItem.clear();
	::EnumChildWindows(m_hWnd,EnumWindowsProc,NULL);
	return gConItem;
}

int RemoteControl::SetControlCheck(HWND hwnd,bool check)
{
	if(check)
		::SendMessage(hwnd, BM_SETCHECK, BST_CHECKED, 0);
	else
		::SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
	return 0;
}




