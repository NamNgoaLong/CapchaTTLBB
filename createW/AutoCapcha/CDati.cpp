
#include <windows.h>
//#include "CDati.h"
#include <process.h>
#include "macro.h"
#include <stdio.h>
#include "app.h"

extern bool g_ModuleExit;
extern App *app;

unsigned __stdcall CDati::DaTiProc( LPVOID lParam )
{
	__AutoFreeLibrary_EnterProcess ;

	CDati* pDati = (CDati*)lParam;
	HANDLE hEvent[1];
	hEvent[0] = pDati->hEventDati;
	while( __AutoFreeLibrary_IsNotExit )
	{
		DWORD dwEvent = WaitForMultipleObjects( _countof(hEvent), hEvent, FALSE, INFINITE );
		if ( !(__AutoFreeLibrary_IsNotExit ) )
			break ;

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
//	return 0;
	__AutoFreeLibrary_LeaveProcess ;
}

CDati::CDati()
{
	hEventDati = NULL;
	m_result = -1;
	hEventDati = CreateEvent( NULL , FALSE , FALSE , NULL );
//	InitializeCriticalSection( &m_cs );


	unsigned int AppThreadId;
	CloseHandle( (HANDLE)_beginthreadex( NULL, 0, &CDati::DaTiProc, this, 0, &AppThreadId ) );
}

CDati::~CDati()
{
	if( hEventDati )
		CloseHandle( hEventDati );
}
//
//void lock()
//{
//	EnterCriticalSection( &m_cs );
//}
//
//void unLock()
//{
//	LeaveCriticalSection( &m_cs );
//}

namespace cdati{
void DPrint( char *format, ... )
{
	va_list arg_ptr;
	char outstr[0x1000];
	va_start( arg_ptr, format );
	vsprintf_s( outstr, sizeof( outstr ), format, arg_ptr );
	OutputDebugStringA( outstr );
	va_end( arg_ptr );
}
}
bool CDati::anserCapchaDati()
{
	char szAnswer[256] ;
	bool bRet ;
	bool bStop = false ;
	while( bStop )
	{
		Sleep(200);
		if ( -1 == m_result )
		{
			cdati::DPrint( "Result answer captcha error!" ) ;
			bStop = true ;
			return false ;
		}
		sprintf_s( szAnswer, sizeof( szAnswer ), "%s", m_result ) ;
		//	strcpy(szAnswer, app->wg.szAnserNxDati);

		cdati::DPrint("Cau tra loi, anser %s", szAnswer);

		if (strcmp(szAnswer,"1") == 0)
		{
			//		TRACE_DEBUG("DaTi.GetAnswer:A");
			//strncpy_s( szCaptchaCodeId , sizeof(szCaptchaCodeId) , szTemp , sCodeLen >= sizeof(szCaptchaCodeId) ? sizeof(szCaptchaCodeId) - 1 : sCodeLen ) ;
			//app->wg.SetWarning( EFW_CAPTCHA , false , NULL ) ;
			bRet = TRUE;
			app->lua.luaDoStringV("setmetatable(_G, {__index = %s});%s;%s", "AntiRobot_Env" ,"AntiRobot_SelAnswer(0)" , "AntiRobot_OnCommit(0)" ) ;

			bStop = true ;
//			break;
		}
		else if (strcmp(szAnswer,"2") == 0)
		{
			bRet = TRUE;
			app->lua.luaDoStringV("setmetatable(_G, {__index = %s});%s;%s", "AntiRobot_Env" ,"AntiRobot_SelAnswer(1)" , "AntiRobot_OnCommit(1)" ) ;

			bStop = true ;
//			break;
		}
		else if (strcmp(szAnswer,"3") == 0)
		{
			bRet = TRUE;
			app->lua.luaDoStringV("setmetatable(_G, {__index = %s});%s;%s", "AntiRobot_Env" ,"AntiRobot_SelAnswer(2)" , "AntiRobot_OnCommit(2)" ) ;

			bStop = true ;
//			break;
		}
		else if (strcmp(szAnswer,"4") == 0)
		{
			bRet = TRUE;
			app->lua.luaDoStringV("setmetatable(_G, {__index = %s});%s;%s", "AntiRobot_Env" ,"AntiRobot_SelAnswer(3)" , "AntiRobot_OnCommit(3)" ) ;

			bStop = true ;
//			break;
		}
		else// kiemtra
		{
			//app->wg.SetWarning( EFW_CAPTCHA , false , NULL ) ;
			//bRet = TRUE;
			//break;
		}
	}
	return true ;
}
