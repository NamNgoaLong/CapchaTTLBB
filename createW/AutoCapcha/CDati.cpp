
#include <windows.h>
#include "CDati.h"
#include <process.h>
#include "macro.h"
//unsigned __stdcall CDati::DaTiProc(LPVOID lParam);

extern bool g_ModuleExit;

unsigned __stdcall CDati::DaTiProc( LPVOID lParam )
{
	CDati* pDati = (CDati*)lParam;
	HANDLE hEvent[1];
	hEvent[0] = pDati->hEventDati;
	while( __AutoFreeLibrary_IsNotExit )
	{
		DWORD dwEvent = WaitForMultipleObjects( _countof(hEvent), hEvent, FALSE, INFINITE );
		switch( dwEvent )
		{
			case WAIT_OBJECT_0 +0:
				{
					if( EDT_GETANSWER == pDati->m_dwCode )
					{
						pDati->anserCapchaDati();
					}
				}
				break;
		}
	
	}

	return 0;
}

CDati::CDati()
{
	hEventDati = NULL;
	unsigned int AppThreadId;
	CloseHandle( (HANDLE)_beginthreadex( NULL, 0, &CDati::DaTiProc, this, 0, &AppThreadId ) );
}


BOOL CDati::anserCapchaDati(){
	
	return TRUE;
}
