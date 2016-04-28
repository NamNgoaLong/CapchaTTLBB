
//#include "CDati.h"

#include "app.h"
#include "function2.h"
#include "macro.h"
#include <process.h>

#include <DbgHelp.h>

#pragma comment( lib , "Dbghelp.lib" )

using namespace functions;

extern App *app;
extern bool g_ModuleExit;

LRESULT CALLBACK gameSafeProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam ) ;

App::App()
{
	InitializeCriticalSection( &m_cs ) ;

	hMappingFile = NULL;
	hwndGame = NULL ;

	m_nInitAddrOk = 222 ;
	strcpy_s( info.szNamePlayer, sizeof(info.szNamePlayer), "" );

}
App::~App()
{
	if( hMappingFile )
		CloseHandle( hMappingFile ) ;	
	DeleteCriticalSection( &m_cs ) ;
}

unsigned int _stdcall App::threadExitDll( LPVOID lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	__AutoFreeLibrary_EnterExitProcess ;


	//HMODULE _hModule = NULL ; 
	//::GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT , __GAMEDLL_NAME__ , &_hModule );
	//if( NULL == _hModule )
	//	return 1 ;
//	FreeLibraryAndExitThread( _hModule , 0 );

	DPrint( "ExitWgThread 0x%08x , err:0x%08x" , _hModule , ::GetLastError()) ;

	__AutoFreeLibrary_LeaveProcess ;

//	return 0 ;
}

void App::exitDll()
{
	__AutoFreeLibrary_SetExit ;

	SetEvent( app->nxDati.hEventDati );
	if( app->decrypt.unHookGameRecv() )
		DPrint( "Unhook inline packet sucess!" );
	else
		DPrint( "Error Unhook inline packet!" );

	if( app->hwndGame )
	{
		if( app->gameSrcProc )
		{
			SetWindowLong( app->hwndGame , GWL_WNDPROC , (LONG)app->gameSrcProc );
			app->gameSrcProc = NULL ;
		}
	}
	CloseHandle( (HANDLE)_beginthreadex( NULL, 0 , &App::threadExitDll , this , 0 , 0 ) );
}

 unsigned int _stdcall App::initThreadMain( LPVOID lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	__AutoFreeLibrary_EnterProcess ;

#ifdef _DEBUG
	DPrint( "initThreadMain1" ) ;
#endif

	app->decrypt.startInitAddrProcess();

	app->initThreadInfo();

#ifdef _DEBUG
	DPrint( "initThreadMain2" ) ;
#endif

	while( 0 == app->getGameWindows() )
		Sleep( 200 );


#ifdef _DEBUG
	DPrint( "getGameWindows" ) ;
#endif

	__AutoFreeLibrary_LeaveProcess ;
//	return 0;
}

void App::lock()
{
	EnterCriticalSection( &m_cs ) ; 
}

void App::unLock()
{
	LeaveCriticalSection( &m_cs ) ;
}
 
void App::initThreadInfo()
{
	unsigned int AppThreadId;
	CloseHandle( (HANDLE)_beginthreadex( NULL, 0, &App::updateInfo, 0, 0, &AppThreadId ) ) ;

}

HWND App::getGameWindows()
{
	HWND hWnd = NULL ;
	DWORD dwPID = 0 ;
	bool bStop = false ;
	while( !bStop )
	{
		hWnd = FindWindowEx( NULL , hWnd , "TianLongBaBu WndClass" , NULL ) ;
		if ( !hWnd ) 
			break ;

		GetWindowThreadProcessId( hWnd , &dwPID ) ;
		if( GetCurrentProcessId() == dwPID )
		{
			hwndGame = hWnd;
//			Dati.m_hGameHwnd = hWnd;
			gameSrcProc = (WNDPROC)GetWindowLong( hwndGame , GWL_WNDPROC ) ;

			//SwitchToThisWindow(hwndGame, TRUE);
			//GetClientRect(hwndGame, &GameRC);

			SetWindowLongPtrA( hwndGame , GWL_WNDPROC , (LONG)gameSafeProc ) ;

#ifdef _DEBUG
			DPrint( "SetWindowLongPtrA: %x" , hwndGame );
#endif
			bStop = true ;
			return hWnd ;
		}
	}
	return 0 ;
}

void App::s_dump_callstack( CONTEXT *context )
{
	STACKFRAME sf;
	memset( &sf, 0, sizeof( STACKFRAME ) );
	DWORD CallAdr = 0 ;
	DWORD uLine = 0 ;
	char szFileName[256] = {0} ;
	char szFuncName[128] = {0} ;

	sf.AddrPC.Offset = context->Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = context->Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = context->Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;

	DWORD machineType = IMAGE_FILE_MACHINE_I386;

	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

	for(  int i = 1 ; ; i++ )
	{
		if( !StackWalk(machineType, hProcess, hThread, &sf, context, 0, SymFunctionTableAccess, SymGetModuleBase, 0 ) )
		{
			break;
		}

		if( sf.AddrFrame.Offset == 0 )
		{
			break;
		}
		BYTE symbolBuffer[ sizeof( SYMBOL_INFO ) + 1024 ];
		PSYMBOL_INFO pSymbol = ( PSYMBOL_INFO ) symbolBuffer;

		pSymbol->SizeOfStruct = sizeof( symbolBuffer );
		pSymbol->MaxNameLen = 1024;

//		DWORD64 symDisplacement = 0;
		CallAdr = sf.AddrPC.Offset ;
		sprintf_s( szFuncName , sizeof(szFuncName) , "%s" , "<Unknow>" ) ;
		if( SymFromAddr( hProcess, CallAdr, 0, pSymbol ) )
		{
			if ( pSymbol->Name && pSymbol->NameLen > 0 && pSymbol->NameLen < sizeof( szFuncName ) )
				sprintf_s( szFuncName , sizeof(szFuncName) , "%s" , pSymbol->Name ) ;
		}

		IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
		DWORD dwLineDisplacement;

		uLine = INFINITE ;
		sprintf_s( szFileName , sizeof(szFileName) , "%s" , "<Unknow file>") ;

		if( SymGetLineFromAddr( hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) )
		{
			if ( lineInfo.FileName )
			{
				unsigned int clen = strlen(lineInfo.FileName) ;
				if ( clen > 0 && clen < sizeof( szFileName ) )
					sprintf_s( szFileName , sizeof(szFileName) , "%s" , lineInfo.FileName ) ;
			}
			uLine = lineInfo.LineNumber ;
		}

		DPrint("%02d -> 0x%08x %s %s[%d]" , i , CallAdr , szFuncName , szFileName , uLine ) ;
	}
}



int App::s_seh_handler(int code , PEXCEPTION_POINTERS p)
{  
	DPrint("error code:%08X\r\n" , code);
	CONTEXT *c = p->ContextRecord;
	DPrint("ESI:%08X EDI:%08X\r\n", c->Esi, c->Edi);
	DPrint("EBP:%08X ESP:%08X EIP:%08X\r\n", c->Ebp, c->Esp, c->Eip);
	DPrint("EAX:%08X EBX:%08X ECX:%08X EDX:%08X\r\n",c->Eax , c->Ebx , c->Ecx , c->Edx);


	SymInitialize( GetCurrentProcess(), NULL, TRUE ) ;
	s_dump_callstack( c ) ;
	SymCleanup( GetCurrentProcess() ) ;

	return 1;
} 


LRESULT CALLBACK gameSafeProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
{
//	DPrint( "gameSafeProc" );
	LRESULT Result = 0  ;
	HWND sWnd = hWnd ;
	UINT sMsg = uMsg ;
	WPARAM sWParam = wParam ;
	LPARAM sLParam = lParam ;

	__try
	{
		Result = app->gameAlterProc( hWnd , uMsg , wParam , lParam );
	}
	__except(app->s_seh_handler(_exception_code() , (PEXCEPTION_POINTERS)_exception_info()))
	{
		DPrint("Except Safe_GameWinProc HWND:%08x , Msg:%04x , wParam:%08x , lParam:%08x" , sWnd , sMsg , sWParam , sLParam ) ;
		Result = 0 ;
	}
	return Result ;

}


LRESULT CALLBACK App::gameAlterProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
{
	switch (uMsg)
	{

	case MYWM_FROM_MYCAPTCHA_DLL:
		{

#ifdef _DEBUG
			DPrint( "Auto MYWM_FROM_MYCAPTCHA_DLL wParam: %x , lParam: %x " , wParam , lParam );
#endif

			if( wParam == GetCurrentProcessId() )
			{
				if( lParam_Exit_Dll == (int)lParam )
				{
//					DPrint( "Message get account!" );

					//if( app->nxCreateMappingFile( (int)wParam , false ) )
					//{
					//	if( !app->nxSendMessage( wParam , -1  ))
					//		DPrint( "Error send Login to AutoCapcha" );
					//}
					//else
					//{
					//	DPrint( "Error Create mapping file Login" );
					//}
					DPrint( "Message ExitDll !" );
					app->exitDll();

				}
				else if( (int)lParam >0 && (int)lParam < 5 )
				{
					DPrint( "Dap an tra loi, result: %d" , (int)lParam );

					app->lock();
					app->nxDati.m_result = (int)lParam ;
					app->unLock();

					SetEvent( app->nxDati.hEventDati );
				}
				else if( lParam_Close_Mapfile == (int)lParam )
				{
					if( NULL != app->hMappingFile )
					{
						CloseHandle( app->hMappingFile );// close mapping file

						DPrint("Clossed Handle Mapping hMappingFile");
					}
					
				}
				else
				{
					DPrint("Dap an bi loi %d", (int)lParam );
				}
			}
		}
		break;
	}

	return CallWindowProc((WNDPROC)app->gameSrcProc, hWnd, uMsg, wParam, lParam);// Goi cua so game nguyen ban
}

void App::init()
{
	unsigned int AppThreadId;
	CloseHandle( (HANDLE)_beginthreadex( NULL, 0, &App::initThreadMain, 0, 0, &AppThreadId ) ) ;	
}


unsigned int _stdcall App::updateInfo( LPVOID lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	__AutoFreeLibrary_EnterProcess ;
//	DPrint( "updateInfo" ) ;
	char szNamePlayer[128];
	strcpy_s( szNamePlayer, sizeof(szNamePlayer), "" );

	while( __AutoFreeLibrary_IsNotExit )
	{
		Sleep( 300 ) ;

		if( app->m_nInitAddrOk != ERROR_INIT_OK)
		{
#ifdef _DEBUG
			DPrint( " updateInfo m_nInitAddrOk: %d", app->m_nInitAddrOk );
#endif
			continue;
		}

		if( !app->decrypt.getAccountInfo( szNamePlayer , sizeof( szNamePlayer ) ) )
			continue;

		if( 0 != strcmp( szNamePlayer , app->info.szNamePlayer ) )
		{
			app->lock() ;
			strcpy_s( app->info.szNamePlayer , sizeof( app->info.szNamePlayer ) , szNamePlayer );
			app->unLock() ;

			if( app->nxCreateMappingFile( GetCurrentProcessId() , false ) )
			{
				DPrint( "info.szNamePlayer: %s" , app->info.szNamePlayer );

				app->lock();
				app->m_bLogin = true ;
				app->unLock();

				app->nxSendMessage( GetCurrentProcessId() , -1 );
			}
			else
			{
				DPrint("Error Create mapping file Login");
			}
			
		}
	}
	__AutoFreeLibrary_LeaveProcess ;
//	return 1;
}



bool App::nxSendMessage( int wParam, int lParam )
{
	HWND hWndCapchaNxDati = FindWindow( TEXT( "#32770" ), TEXT( NAME_CAPTION_AUTO_CAPCHA ) );
	if( NULL == hWndCapchaNxDati )
	{
		DPrint( "Find Window AutoCapcha failed 1" );
		return false;
	}
	else
	{
		DPrint("Find Window AutoCapcha hWndCapchaNxDati: %x ", hWndCapchaNxDati);

		PostMessage( hWndCapchaNxDati, MYWM_FROM_MYCAPTCHA_DLL, (WPARAM)wParam, (LPARAM)lParam );//
		return true;
	}
}

bool App::nxCreateMappingFile( int nCodeName, bool bPassCapcha )
{

	char szName[256];
	strcpy_s( szName, sizeof(szName), NAME_MAP_FILE );
	char szCodeName[256];
	sprintf_s( szCodeName, sizeof( szCodeName ), "%d", (int)nCodeName );
	strcat_s( szName, sizeof(szName), szCodeName );


	char* pBuf;
	hMappingFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE_BMP +256,                // maximum object size (low-order DWORD)
		szName );                 // name of mapping object

	if ( hMappingFile == NULL )
	{
		DPrint( "Could not create file mapping object (%d).\n", GetLastError() );
		return FALSE;
	}

	if( bPassCapcha )
	{
		pBuf = (LPTSTR) MapViewOfFile(hMappingFile,   // handle to map object
			FILE_MAP_ALL_ACCESS, // read/write permission
			0,
			0,
			BUF_SIZE_BMP + 256 );
	}
	else
	{
		pBuf = (LPTSTR) MapViewOfFile( hMappingFile,   // handle to map object
			FILE_MAP_ALL_ACCESS, // read/write permission
			0,
			0,
			256 );

	}

	if ( pBuf == NULL )
	{
		DPrint( "Could not map view of file (%d).\n",GetLastError() );
		CloseHandle( hMappingFile );
		return FALSE;
	}	
	if( bPassCapcha )
	{
		stBmpCapcha stSendData;
		strcpy_s( stSendData.szAnser, sizeof( stSendData.szAnser ), nxDati.stDataCaptcha.szAnser );
		memcpy( stSendData.szDataBmp, nxDati.stDataCaptcha.szDataBmp, 0x240 );
		CopyMemory( (PVOID)pBuf, &stSendData, sizeof( stSendData ) );
	}
	else
	{

//		char szNamePlayer[128];
//		if( app->decrypt.getAccountInfo( szNamePlayer, sizeof( szNamePlayer) ) )
			CopyMemory( (PVOID)pBuf, app->info.szNamePlayer, sizeof( app->info.szNamePlayer ) );

	}
	UnmapViewOfFile( pBuf );
// Chua dong file Handle
	return TRUE;
}


