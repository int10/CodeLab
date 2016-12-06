#include <QCoreApplication>
#include <QDebug>



#include <windows.h>
#include <setupapi.h>

#ifdef UNICODE
	#define QStringToTCHAR(x)     (wchar_t*) x.utf16()
	#define PQStringToTCHAR(x)    (wchar_t*) x->utf16()
	#define TCHARToQString(x)     QString::fromUtf16((ushort*)(x))
	#define TCHARToQStringN(x,y)  QString::fromUtf16((ushort*)(x),(y))
#else
	#define QStringToTCHAR(x)     x.local8Bit().constData()
	#define PQStringToTCHAR(x)    x->local8Bit().constData()
	#define TCHARToQString(x)     QString::fromLocal8Bit((x))
	#define TCHARToQStringN(x,y)  QString::fromLocal8Bit((x),(y))
#endif /*UNICODE*/


//static GUID GUID_DEVINTERFACE_USB_DEVICE =
//{ 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };

static GUID GUID_DEVINTERFACE_USB_DEVICE =
{ 0x86E0D1E0L, 0x8089, 0x11d0, { 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73} };

//static GUID GUID_DEVINTERFACE_USB_DEVICE =
//	GUID_CLASS_COMPORT;

/*!
 * Structure containing port information.
 */
struct QextPortInfo {
	QString portName;		///< Port name.
	QString physName;		///< Physical name.
	QString friendName;		///< Friendly name.
	QString enumName;		///< Enumerator name.
};

QString getRegKeyValue(HKEY key, LPCTSTR property)
{
	DWORD size = 0;
	RegQueryValueEx(key, property, NULL, NULL, NULL, & size);
	BYTE * buff = new BYTE[size];
	if (RegQueryValueEx(key, property, NULL, NULL, buff, & size) == ERROR_SUCCESS) {
		return TCHARToQStringN(buff, size);
		delete [] buff;
	} else {
		qWarning("QextSerialEnumerator::getRegKeyValue: can not obtain value from registry");
		delete [] buff;
		return QString();
	}
}

QString getDeviceProperty(HDEVINFO devInfo, PSP_DEVINFO_DATA devData, DWORD property)
{
	DWORD buffSize = 0;
	SetupDiGetDeviceRegistryProperty(devInfo, devData, property, NULL, NULL, 0, & buffSize);
	BYTE * buff = new BYTE[buffSize];
	if (!SetupDiGetDeviceRegistryProperty(devInfo, devData, property, NULL, buff, buffSize, NULL))
		qCritical("Can not obtain property: %ld from registry", property);
	QString result = TCHARToQString(buff);
	delete [] buff;
	return result;
}



void setupAPIScan(QList<QextPortInfo> & infoList)
{
	HDEVINFO devInfo = INVALID_HANDLE_VALUE;
	GUID * guidDev = (GUID *) & GUID_DEVINTERFACE_USB_DEVICE;

	devInfo = SetupDiGetClassDevs(guidDev, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if(devInfo == INVALID_HANDLE_VALUE) {
		qCritical("SetupDiGetClassDevs failed. Error code: %ld", GetLastError());
		return;
	}

	//enumerate the devices
	bool ok = true;
	SP_DEVICE_INTERFACE_DATA ifcData;
	ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	SP_DEVICE_INTERFACE_DETAIL_DATA * detData = NULL;
	DWORD detDataSize = 0;
	DWORD oldDetDataSize = 0;

	for (DWORD i = 0; ok; i++) {
		ok = SetupDiEnumDeviceInterfaces(devInfo, NULL, guidDev, i, &ifcData);
		if (ok) {
			SP_DEVINFO_DATA devData = {sizeof(SP_DEVINFO_DATA)};
			//check for required detData size
			SetupDiGetDeviceInterfaceDetail(devInfo, & ifcData, NULL, 0, & detDataSize, & devData);
			//if larger than old detData size then reallocate the buffer
			if (detDataSize > oldDetDataSize) {
				delete [] detData;
				detData = (SP_DEVICE_INTERFACE_DETAIL_DATA *) new char[detDataSize];
				detData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				oldDetDataSize = detDataSize;
			}
			//check the details
			if (SetupDiGetDeviceInterfaceDetail(devInfo, & ifcData, detData, detDataSize,
												NULL, & devData)) {
				// Got a device. Get the details.
				QextPortInfo info;
				info.friendName = getDeviceProperty(devInfo, & devData, SPDRP_FRIENDLYNAME);
				info.physName = getDeviceProperty(devInfo, & devData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);
				info.enumName = getDeviceProperty(devInfo, & devData, SPDRP_ENUMERATOR_NAME);
				//anyway, to get the port name we must still open registry directly :( ???
				//Eh...
				HKEY devKey = SetupDiOpenDevRegKey(devInfo, & devData, DICS_FLAG_GLOBAL, 0,
													DIREG_DEV, KEY_READ);
				info.portName = getRegKeyValue(devKey, TEXT("PortName"));
				RegCloseKey(devKey);
				infoList.append(info);
			} else {
				qCritical("SetupDiGetDeviceInterfaceDetail failed. Error code: %ld", GetLastError());
				delete [] detData;
				return;
			}
		} else {
			if (GetLastError() != ERROR_NO_MORE_ITEMS) {
				delete [] detData;
				qCritical("SetupDiEnumDeviceInterfaces failed. Error code: %ld", GetLastError());
				return;
			}
		}
	}
	delete [] detData;
}

void setupAPIId()
{
	HDEVINFO hdevInfo;
	DWORD nSize = 0;
	TCHAR DevInstanceId[MAX_PATH];
	QString str;
	hdevInfo = SetupDiGetClassDevs((LPGUID)& GUID_DEVINTERFACE_USB_DEVICE,
	NULL,
	NULL,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);


	if (hdevInfo != INVALID_HANDLE_VALUE)
	{
		SP_DEVINFO_DATA devInfoData;
		ZeroMemory(&devInfoData,sizeof(devInfoData));
		devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (DWORD ndx = 0; SetupDiEnumDeviceInfo(hdevInfo,ndx,&devInfoData); ndx++)
		{
			if(SetupDiGetDeviceInstanceId(hdevInfo,&devInfoData, DevInstanceId,sizeof(DevInstanceId),&nSize))
			{
				str = QString::fromUtf16(DevInstanceId);
				qDebug()<<"str is"<<str;
			}
		}
	}
}


QString GetDeviceInstanceId(HDEVINFO devinfo, PSP_DEVINFO_DATA devdata)
{
	DWORD buffsize = 0;

	::SetupDiGetDeviceInstanceId(devinfo,devdata, NULL, NULL,&buffsize);
	if (buffsize > 0)
	{
		PWSTR buff = new WCHAR[buffsize];
		::SetupDiGetDeviceInstanceId(devinfo, devdata, buff, buffsize, NULL);
		QString result = QString::fromUtf16((ushort*)buff);
		delete [] buff;
		return result;
	}
	else
	{
		return "";
	}

}

void GetMtkPorts()
{
	HDEVINFO devinfo = INVALID_HANDLE_VALUE;
	GUID * guiddev = (GUID *) & GUID_DEVINTERFACE_USB_DEVICE;

	devinfo = ::SetupDiGetClassDevs(guiddev, NULL, NULL,
								  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if(devinfo == INVALID_HANDLE_VALUE) {
		return;
	}

	bool ok = true;
	SP_DEVINFO_DATA deviceinfodata;
	deviceinfodata.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; ok; i++)
	{
		ok = ::SetupDiEnumDeviceInfo(devinfo, i ,&deviceinfodata);
		if (ok)
		{
			qDebug()<<GetDeviceInstanceId(devinfo, &deviceinfodata);
		}
		else
		{
			if (::GetLastError() != ERROR_NO_MORE_ITEMS)
			{
				return;
			}
		}
	}

	::SetupDiDestroyDeviceInfoList(devinfo);
	return;
}


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

//	setupAPIId();
	GetMtkPorts();

//	QList<QextPortInfo> list;
//	setupAPIScan(list);
//	foreach(QextPortInfo info, list)
//	{
//		qDebug()<<info.friendName;
//		qDebug()<<info.physName;
//	}
//	qDebug()<<"----------------------------";

	return 0;


	return a.exec();
}
