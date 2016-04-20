
#include <windows.h>
#include "Inject.h"
#include <stdio.h>
#include <shlwapi.h>
#include "macro.h"



Inject::Inject()
{
	strcpy_s(szPath, sizeof(szPath), "");
}

void DPrint( char *format, ... )
{
	va_list arg_ptr;
	char outstr[0x1000];
	va_start(arg_ptr, format);
	vsprintf_s(outstr, sizeof(outstr), format, arg_ptr);
	OutputDebugStringA(outstr);
	va_end(arg_ptr);
}

void Inject::getModulePath(char*szPathModule, char* szOutPath)
{
	GetModuleFileNameA(GetModuleHandle(NULL), szOutPath, MAX_PATH);
	strcpy_s(strrchr(szOutPath, '\\') + 1,sizeof(szOutPath), NAME_PATH_DLL);
	strcpy_s(strrchr(szOutPath, '\\') + 1,sizeof(szOutPath), szPathModule);
	DPrint("path: %s", szOutPath);
}

BOOL Inject::enableDebugPrivilege(BOOL bEnable)
{
	BOOL bOK = FALSE;
	HANDLE hToken;
	// Try cap vao Token process, TOKEN_ADJUST_PRIVILEGES: yeu cau mo hoac dong Token
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		//TOKEN_PRIVILEGES: Chua cac thong tin de set quyen truy cap vao Token
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		// Tim den Privilege SE_DEBUG_NAME de thay de thay doi
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		// Gan quyen da thiet cho Token process
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		bOK = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return bOK;
}
BOOL Inject::injectDll(HANDLE hProcess, const char* szDllName)
{
	LPVOID lpDllNameAddr = VirtualAllocEx( hProcess, NULL, strlen(szDllName)+1, MEM_COMMIT, PAGE_READWRITE );
	if(NULL == lpDllNameAddr)
	{
		DPrint("Error VirtualAllocEx: %x", GetLastError());
		return FALSE;
	}
	DWORD dwRes = 0;
	if(!WriteProcessMemory( hProcess, lpDllNameAddr, szDllName, strlen(szDllName), &dwRes ))
	{
		DPrint("Error WriteProcessMemory: %x", GetLastError());
		VirtualFreeEx( hProcess, lpDllNameAddr, strlen(szDllName)+1, MEM_DECOMMIT );
		return FALSE;
	}
	HMODULE hModKernel32 = GetModuleHandle("kernel32.dll");
	LPTHREAD_START_ROUTINE lpLibAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hModKernel32, "LoadLibraryA");
	if(NULL == lpLibAddr)
	{
		DPrint("Error GetProcAddress: %x", GetLastError());
		VirtualFreeEx( hProcess, lpDllNameAddr, strlen(szDllName)+1, MEM_DECOMMIT );
		return FALSE;
	}
	// Thay ham LoadLibraryA bang ham cua minh
	HANDLE hRemote = CreateRemoteThread( hProcess, NULL, 0, lpLibAddr, lpDllNameAddr, 0, NULL );
	if(NULL == hRemote)
	{
		if(5 == GetLastError()){
			DPrint("Error CreateRemoteThread: %x", GetLastError());
			VirtualFreeEx( hProcess, lpDllNameAddr, strlen(szDllName)+1, MEM_DECOMMIT );
			MessageBox(NULL, TEXT("You must run TLBB.exe as Administrator"), TEXT("Notification!"), 0);
			return FALSE;
		}
	}
	WaitForSingleObject( hRemote, INFINITE );

	CloseHandle( hRemote );
	VirtualFreeEx( hProcess, lpDllNameAddr, strlen(szDllName)+1, MEM_DECOMMIT );
	return TRUE;
}

BOOL Inject::remoteInjectDll(DWORD dwProcessID)
{
	char szPath[MAX_PATH];
	getModulePath(NAME_PATH_VER_MAIN, szPath);
	if(!PathFileExists(szPath))
	{
		MessageBox(NULL, TEXT("Error failed dll file in folder"), TEXT("Error!"), 0);
		return FALSE;
	}
	enableDebugPrivilege(TRUE);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if(!hProcess)
	{
		if(5 == GetLastError())
		{
			MessageBox(NULL, TEXT("You must run TLBB.exe as Administrator"), TEXT("Notification!"), 0);
			return false;
		}
	}
	if(!injectDll(hProcess, szPath))
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	CloseHandle(hProcess);
	return TRUE;
}
