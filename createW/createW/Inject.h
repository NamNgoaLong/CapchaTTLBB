
#pragma once
#include <windows.h>

class Inject
{
public:
	char szPath[MAX_PATH] ;
public:
	Inject() ;
	void getModulePath( char*szPathModule, char* szOutPath ) ;
	BOOL remoteInjectDll( DWORD dwProcessID ) ;
	BOOL injectDll( HANDLE hProcess, const char* szDllName ) ;
	BOOL enableDebugPrivilege( BOOL bEnable ) ;
} ;
