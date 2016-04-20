
//#include "CDati.h"
#include "app.h"
#include "function2.h"
#include "macro.h"
#include <process.h>

using namespace functions;

extern App *app;
extern bool g_ModuleExit;

App::App()
{
	hMappingFile = NULL;
	strcpy_s( info.szNamePlayer, sizeof(info.szNamePlayer), "" );



	decrypt.initClientAddrProcess();
	decrypt.initPacketMsgProcess();
	decrypt.initClientOffsetProcess();
	decrypt.hookGameRecv();

	char szNamePlayer[128];
	decrypt.getAccountInfo(szNamePlayer, sizeof(szNamePlayer));

	DPrint("Name Player: %s", szNamePlayer);

	unsigned int AppThreadId;
	CloseHandle((HANDLE)_beginthreadex( NULL, 0, &App::updateInfo, 0, 0, &AppThreadId ));

}

unsigned int _stdcall App::updateInfo( LPVOID lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	char szNamePlayer[128];
	strcpy_s( szNamePlayer, sizeof(szNamePlayer), "" );
	while( __AutoFreeLibrary_IsNotExit )
	{
		if( !app->decrypt.getAccountInfo( szNamePlayer, sizeof( szNamePlayer ) ) )
			continue;

		if( 0 != strcmp( szNamePlayer, app->info.szNamePlayer ) )
		{
			strcpy_s( app->info.szNamePlayer, sizeof( app->info.szNamePlayer ), szNamePlayer );
			if( app->nxCreateMappingFile( GetCurrentProcessId(), false ) )
			{
				app->nxSendMessage( GetCurrentProcessId(), -1 );
			}
			else
			{
				DPrint("Error Create mapping file Login");
			}
		}
	}
	return 1;
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

		PostMessage( hWndCapchaNxDati, MYWM_MESSAGE, (WPARAM)wParam, (LPARAM)lParam );//
		return true;
	}
}

BOOL App::nxCreateMappingFile( int nCodeName, bool bPassCapcha )
{

	char szName[256];
	strcpy_s( szName, sizeof(szName), NAME_MAP_FILE );
	char szCodeName[256];
	sprintf_s( szCodeName, sizeof( szCodeName ),"%d", int( nCodeName ) );
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
			BUF_SIZE_BMP +256);
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
		strcpy_s( stSendData.szAnser, sizeof(stSendData.szAnser), nxDati.stDataCaptcha.szAnser );
		memcpy( stSendData.szDataBmp, nxDati.stDataCaptcha.szDataBmp, 0x240 );
		CopyMemory( (PVOID)pBuf, &stSendData, sizeof( stSendData ) );
	}
	else
	{

		char szNamePlayer[128];
		if( app->decrypt.getAccountInfo( szNamePlayer, sizeof( szNamePlayer) ) )
			CopyMemory( (PVOID)pBuf, szNamePlayer, sizeof( szNamePlayer ) );

	}
	UnmapViewOfFile( pBuf );
// Chua dong file Handle
	return TRUE;
}


