
#include <windows.h>
#include <Psapi.h>
#include <map>
#include "search.h"
#include "Decrypt.h"
#include "cbuff.h"
#include <string>
#include "app.h"
#pragma comment( lib, "psapi.lib" )

using namespace namespcace_MySearch ;

std::map<DWORD, stDataPacket> Decrypt::m_stPacketData;
DWORD Decrypt::m_dwDatatAddr[GAMEADR_MAXINDEX];
DWORD Decrypt::m_dwOffsetData[GAMEOFFSET_MAXINDEX];


extern App *app;
extern bool g_ModuleExit;



//-------------------------------------------------------------------
char* _tl_string_c_str( void* lpstring ) 
{
	int nSize = *(int*)( (unsigned char*)lpstring + 0x18 ) ;
	if ( nSize < 0x10 )
		return (char*)( (unsigned char*)lpstring + 0x4 ) ;
	return (char*)(*(DWORD*)( (unsigned char*)lpstring + 0x4) ) ;
}

int _tl_string_length( void* lpstring )
{
	return *(int*)((unsigned char*)lpstring + 0x14 ) ;
}

//-----------------------------------------------------------------

BOOL Decrypt::initPacketMsgProcess( void )
{
	MODULEINFO mod;
	DWORD dwBeginAddr = 0, dwEndAddr = 0;

	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId() );
	if( !GetModuleInformation( hProcess, GetModuleHandle( NULL ), &mod, sizeof( mod) ) )
	{
		DPrint( "Error GetModuleInformation: %x", GetLastError() );
		return false;
	}
	dwBeginAddr = (DWORD)mod.lpBaseOfDll;
	dwEndAddr = (DWORD)mod.lpBaseOfDll + mod.SizeOfImage;
	WORD wCodeMsg = 0;
	DWORD dwCodeMsgAddr = 0, dwCodeExeAddr = 0;
	DWORD dwMaxMsg = 0;
	std::multimap<WORD, DWORD>	listCodeMsg; // list packet need securies

	DPrint("dwBeginAddr: %x, dwEndAddr: %x", dwBeginAddr, dwEndAddr);

	for( DWORD dwSearch = dwBeginAddr; dwSearch + 0x10 < dwEndAddr; dwSearch += 4 )
	{
		if( ::IsBadReadPtr( (void*)dwSearch, sizeof(DWORD)*5 ) )
			continue;

		dwCodeMsgAddr = *(DWORD*)( dwSearch + 0x10 );
		dwCodeExeAddr = *(DWORD*)( dwSearch + 0x0c );

		if( dwCodeMsgAddr < dwBeginAddr || dwCodeMsgAddr >= (dwEndAddr - 4 ))
			continue;

		if( ::IsBadCodePtr( (FARPROC)( dwCodeMsgAddr ) ) || ::IsBadCodePtr( (FARPROC)( dwCodeExeAddr ) ) )
			continue;

		if( *(BYTE*)( dwCodeMsgAddr + 0 ) == 0x66 && *(BYTE*)( dwCodeMsgAddr + 1 ) == 0xb8 && *(BYTE*)( dwCodeMsgAddr + 4 ) == 0xc3 )
		{
			wCodeMsg = *(WORD*)( dwCodeMsgAddr + 2 );
			listCodeMsg.insert( std::make_pair( wCodeMsg, dwSearch ) );
			dwMaxMsg++;
			//			DPrint("%x ", dwSearch);
		}
	}

	DPrint( "dwMaxMsg %d", dwMaxMsg );

	char szNameStr[64] = {0} ;
	char szNoteStr[64] = {0} ;
	DWORD dwIndex = 0;
	DWORD dwPtrAddr = 0;
	std::multimap<WORD, DWORD>::const_iterator ptr = listCodeMsg.begin();

	for( int i = 0; ptr != listCodeMsg.end(); ptr++, i++ )
	{
		wCodeMsg = ptr->first;
		dwPtrAddr = ptr->second;
		dwCodeExeAddr = *(DWORD*)(dwPtrAddr + 0xc);

		if( matchPacketCode( GetModuleHandle( NULL ), dwCodeExeAddr, dwIndex, szNameStr, sizeof( szNameStr ), szNoteStr, sizeof( szNoteStr ) ) )
		{
			if( dwIndex >= GAME_PACKET_MAXINDEX )
				continue;
		}
		if( dwIndex == GAMEPACKET_RECV_TRAINCAPTCHA )
		{
			DPrint( "GAMEPACKET_RECV_TRAINCAPTCHA %x", dwCodeExeAddr );

			m_stPacketData[dwPtrAddr].dwMask = 0;
			m_stPacketData[dwPtrAddr].dwNewAddr = 0;
			m_stPacketData[dwPtrAddr].dwRealAddr = dwCodeExeAddr;
			m_stPacketData[dwPtrAddr].dwIndex = dwIndex;
			m_stPacketData[dwPtrAddr].wUmsg = wCodeMsg;
		}

	}
	return TRUE;
}

BOOL Decrypt::matchPacketCode( HMODULE hModule, DWORD dwFuncAdr, DWORD &uIndex, char *lpOutNameStr, int nNameStrLen, char *lpOutNoteStr, int nNoteStrLen )
{
	UNREFERENCED_PARAMETER( lpOutNameStr );
	UNREFERENCED_PARAMETER( nNameStrLen );
	UNREFERENCED_PARAMETER( lpOutNoteStr );
	UNREFERENCED_PARAMETER( nNoteStrLen );

	uIndex = INFINITE;
//	BOOL bIsReturn = FALSE;
	DWORD dwCall1 = 0;//, dwTmp = 0, dwAddr = 0;
	dwCall1 = _SRCforModule( (HMODULE)hModule, 1, dwFuncAdr, dwFuncAdr+0x20, -7 , "83c408", 0, "", NULL, NULL );	
	if( ::IsBadCodePtr( (FARPROC)dwCall1) )
	{
		return FALSE;
	}
	if( 0 != _SRCforModule( (HMODULE)hModule, 0, dwCall1, dwCall1 + 0x50, 0 , "8b75088b8e740200008b56105152", 0, "", NULL, NULL ) )
	{
		if( uIndex != INFINITE )
			DPrint("Goi tin bi trung: %d", uIndex);

		uIndex = GAMEPACKET_RECV_TRAINCAPTCHA;
		return TRUE;
	}
	return FALSE;
}

ULONG BinToHex(const char *src, ULONG len, char * out, bool mode, char *ktext, int hlen)
{
	char *hout = out;
	char *ctext = "0123456789ABCDEF";
	if (mode) ctext = "0123456789abcdef";
	for (ULONG i = 0; i< len; i++)
	{
		*out = ctext[(BYTE)(src[i]) >> 4];		out++;
		*out = ctext[src[i] & 0x0F];			out++;
		if ( hlen > 0 && ((int)(i % hlen) == (hlen - 1) )) {*(WORD *)(out) = 0x0A0D; out += 2;}
		else if (ktext && *ktext){*out = *ktext; out++;}
	}
	*out = '\0';
	return out - hout;
}


void onRecvEx( char* lpBuffer , int uIndex , int uMsg , DWORD& dwOutExeFlag )
{
	UNREFERENCED_PARAMETER( dwOutExeFlag );
	UNREFERENCED_PARAMETER( uMsg );
	CBuff buf ;
	char byTemp[0x300] = {0};
	char byCaptchaData[0x240];
	memset( byTemp , 0 , sizeof( byTemp ) ) ;
	memset( byCaptchaData , 0 , sizeof( byCaptchaData ) ) ;

	switch( uIndex )
	{
	case GAMEPACKET_RECV_TRAINCAPTCHA:
		{
			DPrint( "onRecvEx-> GAMEPACKET_RECV_TRAINCAPTCHA" );

			buf.add_back( lpBuffer, 0x278 );
			//			buf.seek_set(0);
			buf.seek_set( 0x10 ) ;
//			DWORD dwFlag = buf.readd();

			memset( byTemp , 0 , sizeof( byTemp ) ) ;
			buf.seek_set( 0x14 );
			buf.readm( byTemp , 32 ) ;//8*4 =32

			buf.seek_set( 0x34 ) ;
			buf.readm( byCaptchaData, 0x240 );

//			buf.seek_set( 0x274 ) ;
			//			DWORD dwKey = buf.readd();

			char szAnserCapcha[128];
			::WideCharToMultiByte( 936, 0, (LPCWSTR)byTemp, 16, szAnserCapcha, sizeof(szAnserCapcha), NULL, FALSE ) ; 

			szAnserCapcha[32] = '\0';
			DPrint( "Anser Reviev: %s", szAnserCapcha );




			memset( app->nxDati.stDataCaptcha.szDataBmp , 0 , 0x240 ) ;
			memcpy( app->nxDati.stDataCaptcha.szDataBmp , byCaptchaData , 0x240 );
			strcpy_s( app->nxDati.stDataCaptcha.szAnser , sizeof( app->nxDati.stDataCaptcha.szAnser ) , szAnserCapcha ) ;

			int nTickCount = GetTickCount() ;
			if( app->nxCreateMappingFile( nTickCount , true ) )
			{
				app->nxSendMessage( GetCurrentProcessId() , nTickCount ) ;
			}
			else
				DPrint( "Error nxCreateMappingFile: %x", GetLastError());
		}
		break;
	}
}

int onInlineRecv( DWORD pPacketObj , DWORD pPacket , DWORD pNetman )
{
	std::map<DWORD, stDataPacket>::const_iterator ptr = Decrypt::m_stPacketData.find( pPacketObj );

	if( ptr == Decrypt::m_stPacketData.end() )
	{
		return 2; // Khong tim duoc goi tin
	}
	typedef int( *_call )(void);

	DWORD dwUmsgR = ( (_call)(*(DWORD*)( pPacketObj + 0x10 ) ) )();
	dwUmsgR = dwUmsgR & 0xffff;

	if( dwUmsgR != ptr->second.wUmsg )
	{
		return 2;
	}

	DWORD dwRCall = ptr->second.dwRealAddr;
	if( 0 == dwRCall || INFINITE == dwRCall )
	{
		return 2;
	}

	typedef int( *_ExeCall )(DWORD);

	int nResult = 2;
	DWORD dwMask = ptr->second.dwMask;
	if( !( dwMask & GAMEPACKET_EXE_LAST ) )
	{
		__asm
		{
			mov eax , pNetman
				push eax
				mov ecx , pPacket
				call dwRCall
				mov nResult , eax
		} ;

	}

	DWORD dwIngone = 0;
	onRecvEx( (char*)pPacket , ptr->second.dwIndex , dwUmsgR , dwIngone ) ;
	if( dwMask & GAMEPACKET_EXE_LAST )
	{
		if( !( dwIngone & GAMEPACKET_EXE_INGONE ) )
		{
			__asm
			{
				mov eax , pNetman
					push eax
					mov ecx , pPacket
					call dwRCall
					mov nResult , eax
			} ;		
		}else
			DPrint( "Igone Msg %04x" , dwUmsgR ) ;
	}
	return nResult;
}

void _declspec(naked) onInlineRecvHead( DWORD pNetman )
{
	DWORD pPacket  ;
	DWORD pPacketObj ;
	__asm
	{
		mov pPacket, ecx 
			mov pPacketObj , edx
	} ;

	DPrint( "/------- %08x -------\\" , pPacketObj ) ;
	onInlineRecv( pPacketObj , pPacket , pNetman ) ;
	DPrint( "\\-------------------/" ) ;

	__asm
	{
		ret
	}

}

bool Decrypt::hookGameRecv()
{
	std::map<DWORD, stDataPacket>::iterator it = Decrypt::m_stPacketData.begin();
	for( ; it != Decrypt::m_stPacketData.end(); it++ )
	{
		if( it->second.dwIndex == GAMEPACKET_RECV_TRAINCAPTCHA )
		{
			if( it->second.dwRealAddr != INFINITE && it->second.dwRealAddr != 0 )	
			{
				it->second.dwNewAddr = (DWORD)onInlineRecvHead;
				if( it->second.dwIndex == GAMEPACKET_RECV_OPERATERESULT || it->second.dwIndex == GAMEPACKET_RECV_CHARSTOPACTION )
				{
					it->second.dwMask = GAMEPACKET_EXE_LAST | GAMEPACKET_EXE_INGONE;
				}
				DWORD dwAttri = 0 ;
				// set thuoc tinh read, write,...  cho vung nho
				if ( !::VirtualProtectEx( ::GetCurrentProcess(),(LPVOID)( it->first ),sizeof(DWORD) * 8 ,PAGE_READWRITE,&dwAttri ) )
					return FALSE ;

				*(DWORD*)( it->first + 0xc ) = it->second.dwNewAddr ; // exeEcute fuction  
				::VirtualProtectEx( ::GetCurrentProcess(), (LPVOID)( it->first ), sizeof(DWORD) * 8 , dwAttri, &dwAttri ) ;		//API 是否成功并不重要
				DPrint( "Hook packet: %x -> %x", it->second.dwRealAddr, it->second.dwNewAddr );

			}
		}
	}
	return TRUE;
}

bool Decrypt::unHookGameRecv()
{
	DPrint("UnkhookGameRecv.... ") ;
	std::map<DWORD, stDataPacket>::iterator it = Decrypt::m_stPacketData.begin();
	for ( ; it != Decrypt::m_stPacketData.end() ; it++ )
	{
		if( it->second.dwIndex == GAMEPACKET_RECV_TRAINCAPTCHA )
		{
			if ( it->second.dwRealAddr != INFINITE && it->second.dwRealAddr != 0 && it->second.dwNewAddr == (DWORD)onInlineRecvHead )
			{

#ifdef _DEBUG
				DPrint("UnHook GameRecv[0x%04x]:0x%08x => 0x%08x [%s]" , it->second.wUmsg , *(DWORD*)( it->first + 0xc ) , it->second.dwRealAddr ) ;
#endif
				DWORD dwAttri = 0 ;
				if ( !::VirtualProtectEx(::GetCurrentProcess(),(LPVOID)( it->first ),sizeof(DWORD) * 8,PAGE_READWRITE,&dwAttri) )
					return false ;

				*(DWORD*)( it->first + 0xc ) = it->second.dwRealAddr ;

				::VirtualProtectEx(::GetCurrentProcess(),(LPVOID)( it->first ),sizeof(DWORD) * 8,dwAttri,&dwAttri) ;		//API 是否成功并不重要
			}
		}
	}

	return true ;
}

//
//BOOL Decrypt::unhookGameRecv()
//{
//	DPrint("UnkhookGameRecv.... ") ;
//	std::map<DWORD, stDataPacket>::iterator it = Decrypt::m_stPacketData.begin();
//	DWORD dwAttri = 0 ;
//	for( ; it != Decrypt::m_stPacketData.end(); it++ )
//	{
//		if ( it->second.dwRealAddr != INFINITE && it->second.dwRealAddr != 0 && it->second.dwNewAddr == (DWORD)onInlineRecvHead )
//		{
//			if ( !::VirtualProtectEx( ::GetCurrentProcess(),(LPVOID)( it->first ), sizeof(DWORD) * 8, PAGE_READWRITE, &dwAttri ) )
//				return FALSE ;
//
//			*(DWORD*)( it->first + 0xc ) = it->second.dwRealAddr ;
//
//			::VirtualProtectEx( ::GetCurrentProcess(), (LPVOID)( it->first ), sizeof(DWORD) * 8, dwAttri, &dwAttri ) ;		//API 是否成功并不重要
//		}
//	}
//
//	return TRUE ;
//}



BOOL Decrypt::initClientAddrProcess(void)
{
	HMODULE m_dwGameMainBaseAddress = GetModuleHandle( NULL );
	HMODULE m_dwGameUi_ceguiBaseAddress = GetModuleHandle( "UI_CEGUI.dll" );
	HMODULE hLuaPlusDllBaseAddress = GetModuleHandle( "LuaPlus.dll" );

	if ( NULL == m_dwGameMainBaseAddress || NULL == m_dwGameUi_ceguiBaseAddress || NULL == hLuaPlusDllBaseAddress )
	{
		return FALSE;
	}
	std::map<std::string,int>tBaseAdrInfo ;
	//tBaseAdrInfo["CInputSystem"]	= GAMEADR_BASE_CINPUTSYSTEM ;
	tBaseAdrInfo["tTimeSystem"]		= GAMEADR_BASE_TTIMESYSTEM ;
	tBaseAdrInfo["CNetManager"]		= GAMEADR_BASE_CNETMANAGER ;
	tBaseAdrInfo["CNewSoundSystemFMod"] = GAMEADR_BASE_CNEWSOUNDSYSTEMFMOD ;
	tBaseAdrInfo["CRenderSystem"]	= GAMEADR_BASE_CRENDERSYSTEM ;
	tBaseAdrInfo["CObjectManager"]	= GAMEADR_BASE_COBJECTMANAGER ;
	tBaseAdrInfo["CWorldManager"]	= GAMEADR_BASE_CWORLDMANAGER ;
	tBaseAdrInfo["CDataBaseSystem"]	= GAMEADR_BASE_CDATABASESYSTEM ;
	tBaseAdrInfo["CUISystem"]		= GAMEADR_BASE_CUISYSTEM ;
	tBaseAdrInfo["CGameInterface"]	= GAMEADR_BASE_CGAMEINTERFACE ;
	tBaseAdrInfo["CUIDataPool"]		= GAMEADR_BASE_CUIDATAPOOL ;
	tBaseAdrInfo["CDataPool"]		= GAMEADR_BASE_CDATAPOOL ;
	tBaseAdrInfo["CCursorMng"]		= GAMEADR_BASE_CCURSORMNG ;
	tBaseAdrInfo["CScriptSystem"]	= GAMEADR_BASE_CSCRIPTSYSTEM ;
	tBaseAdrInfo["CEventSystem"]	= GAMEADR_BASE_CEVENTSYSTEM ;
	tBaseAdrInfo["CActionSystem"]	= GAMEADR_BASE_CACTIONSYSTEM ;
	tBaseAdrInfo["CFakeObjSystem"]	= GAMEADR_BASE_CFAKEOBJSYSTEM ;
	tBaseAdrInfo["CHelperSystem"]	= GAMEADR_BASE_CHELPERSYSTEM ;
	tBaseAdrInfo["CResourceProvider"] = GAMEADR_BASE_CRESOURCEPROVIDER ;
	tBaseAdrInfo["CTransferItemSystem"] = GAMEADR_BASE_CTRANSFERITEMSYSTEM ;
	tBaseAdrInfo["CItemSystem"]		= GAMEADR_BASE_CITEMSYSTEM ;
	tBaseAdrInfo["CMiniClientSystem"] = GAMEADR_BASE_CMINICLIENTSYSTEM ;
	tBaseAdrInfo["CTimerManager"]	= GAMEADR_BASE_CTIMERMANAGER ;

	DWORD dwAddr = 0 ;
	std::map<std::string,int>::const_iterator it_base_find = tBaseAdrInfo.begin() ;
	if ( SRCforModule( m_dwGameMainBaseAddress, 0, 0, 0, 2, "eb0233c06820040000", 0, "", &dwAddr , NULL ) != 0 )
	{

		for ( DWORD dwSearchFuncAdr = dwAddr ; dwSearchFuncAdr  < dwAddr + 0x500 ; dwSearchFuncAdr++ )
		{
			if( ::IsBadReadPtr( (void*)( dwSearchFuncAdr ) , 50 ) )
				break ; 

			BYTE bKey = *(BYTE*)dwSearchFuncAdr ;
			DWORD pKeyStr = *(DWORD*)( dwSearchFuncAdr + 1 ) ;
			if (  bKey != 0x68 || IsBadStringPtrA( (LPCSTR)pKeyStr , 0x50 ) )
				continue ;

			DWORD dwNodeAdr = INFINITE ;
			//简单的汇编解析
			BYTE* pCode = (BYTE*)dwSearchFuncAdr ;
			BYTE* pStart = pCode ;
			while ( true )
			{
				if ( *pCode == 0x68 || *pCode == 0xb9 )
				{
					pCode += 0x05 ;
				}
				else if ( *(WORD*)pCode == 0xd7ff )
				{
					pCode += 0x02 ;
					dwNodeAdr = 0 ;		//实际需要的数据在 0xd7ff 指令之后
				}
				else if ( *pCode == 0xa3 )
				{
					if ( dwNodeAdr == 0 )
					{
						dwNodeAdr = *(DWORD*)( pCode + 1 ) ;
						break ;
					}
					else
					{
						pCode += 0x5 ;
					}
				}
				else
				{
					break ;
				}

				if ( ( pCode - pStart) > 50 )
					break ; //预定的段内没有找到指令 // Khong chi dan trong mot phan doan duoc xac dinh truoc
			}

			if ( dwNodeAdr == INFINITE || dwNodeAdr == 0 || IsBadReadPtr( (void*)dwNodeAdr , 4 ) )
				continue;

			it_base_find = tBaseAdrInfo.find( (char*)pKeyStr ) ;

			if ( it_base_find == tBaseAdrInfo.end() )
				continue; 

			m_dwDatatAddr[it_base_find->second] = dwNodeAdr ;
		}
	}
	for( int i = 0; i < GAMEADR_MAXINDEX; i++ )
	{
		matchClientCode( i, m_dwDatatAddr[i], m_dwGameMainBaseAddress , m_dwGameUi_ceguiBaseAddress , hLuaPlusDllBaseAddress );
	}

	int nUnvalidValue = 0;
	for( int i = 0; i < GAMEADR_MAXINDEX; i++ )
	{
		if( m_dwDatatAddr[i] ==	INFINITE )
		{
			nUnvalidValue++;
		}
		else
		{
			DPrint( "DataAddr: %x ", m_dwDatatAddr[i] );
		}
	}
	DPrint( "nUnvalidValue %d: ", nUnvalidValue );
	return TRUE;
}

BOOL Decrypt::matchClientCode( DWORD uIndex , DWORD& dwRData, HMODULE m_dwGameMainBaseAddress , HMODULE m_dwGameUi_ceguiBaseAddress , HMODULE hLuaPlusDllBaseAddress)
{


	DWORD dwData = 0 ;

	switch ( uIndex )
	{
	case GAMEADR_LUAPLUSCALL_DOSTRING :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_dostring" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_GETGLOBALS:
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "?GetGlobals@LuaState@LuaPlus@@QAE?AVLuaObject@2@XZ" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_CREATETABLE:
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "?CreateTable@LuaObject@LuaPlus@@QAE?AV12@PBDHH@Z" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_REMOVEFROMUSEDLIST :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "?RemoveFromUsedList@LuaObject@LuaPlus@@QAEXXZ" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_SETNUMBER :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "?SetNumber@LuaObject@LuaPlus@@QAEAAV12@HN@Z" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_DOWBUFFER :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_dowbuffer" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_RAWGETI :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_rawgeti" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_GETTOP :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_gettop");
		}
		break ;
	case GAMEADR_LUAPLUSCALL_TYPE :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_type" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_TONUMBER :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_tonumber" ) ;
		}
		break ;
	case GAMEADR_LUAPLUSCALL_REMOVE :
		{
			dwData = (DWORD)GetProcAddress( hLuaPlusDllBaseAddress, "lua_remove" ) ;
		}
		break ;

	case GAMEADR_LUA_FUNCTION_ECX :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0x0b, "8D4E20C645FC01", 0, "", NULL, NULL ) ;
		}
		break ;
	case GAMEADR_LUA_FUNCTION_ENV_CALL :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 0x0a, "8D4E20C645FC01", 0, "", NULL, NULL ) ;
		}
		break ;
	case GAMEADR_LUA_FUNCTION_CALL :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 2, "FF523C8B005350", 0, "", NULL, NULL ) ;
		}
		break ;

	case GAMEADR_PACKET_DECODE_13F_CALL :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x44, "7604c64314058a4b14", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_IS_IN_GAME_FLAG : // khong dung
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0xB, "3d47230000740e3d3723000074073d31230000", 0, "", NULL, NULL ) ;
		}
		break ;

	case GAMEADR_PACKET_DECODE_311_CALL :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 1, "8B4C240483C1106A7451", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_SEND_LOCK_ECX :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0x0c, "8B56088B016A0152", 0, "", NULL, NULL );
		}
		break ;
	case GAMEADR_FUNC_LOCKNPCCOUNT :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x17, "8B56088B016A0052", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_TRADE_DISTANCE :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0x13, "d8d983c408dfe0ddd8f6c405", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_SEND_PACKET_TRADE_MONEY_ECX :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0x22, "8D55A0894DB4", 0, "", NULL, NULL );
		}
		break ;
	case GAMEADR_SEND_PACKET_ECX :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 2, "8D55A0894DB4", 0, "", NULL, NULL );
		}
		break ;
	case GAMEADR_SEND_PACKET_CALL :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 0x0f, "8D55A0894DB4", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_SEND_PACKET_TALK_TO_NPC_TO_YANBAOPIAO_ECX ://Khong dung
		{
			//12-25 版本中被虚拟机保护
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -13, "83CFFF895DD0897DE4", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_LOCK_OBJECT_STATUS_ECX :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 1, "8B45EC85C07407C7462400000000", 0, "", NULL, NULL );
		}
		break ;
	case GAMEADR_LOCK_OBJECT_4_6 :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0x20, "83F806740983F804", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_DECODE_CAPTCHA_DATA_CALL : // Trong module UI_CEGUI
		{
			dwData = SRCforModule( (HMODULE)m_dwGameUi_ceguiBaseAddress, 1, 0, 0, 1, "68400200008D8D48FCFFFF51526A00", 0, "", NULL, NULL );
		}
		break ;

	case GAMEADR_LUA_STATE_ECX :
		{
			dwData = SRCforModule( (HMODULE)m_dwGameUi_ceguiBaseAddress, 0, 0, 0, 2, "4681FE400200007ED8", 0, "", NULL, NULL );
		}
		break ;
	case GAMEADR_DECODE_CAPTCHA_PURE_LUA_CODE_LEN :
		{
			dwData =SRCforModule( (HMODULE)m_dwGameUi_ceguiBaseAddress, 0, 0, 0, 0x10, "4681FE400200007ED8", 0, "", NULL, NULL ) ;
		}
		break ;
	case GAMEADR_DECODE_CAPTCHA_PURE_LUA_CODE_PTR :
		{
			dwData = SRCforModule((HMODULE)m_dwGameUi_ceguiBaseAddress, 0, 0, 0, 0x15, "4681FE400200007ED8", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_DECODE_CAPTCHA_FIX_NUMBER_CALL :
		{
			dwData = SRCforModule((HMODULE)m_dwGameUi_ceguiBaseAddress, 1, 0, 0, -0x0f, "88040B8B16", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_IS_ATTACK_CALL :
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x12, "895d08DB4508dd1c2452", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_SEND_PACKET_PET_FIGHT_INIT_CALL :
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x24, "8D55B452C745FC060000008945C88975CC895DD0", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_SEND_PACKET_PET_FEED_INIT_CALL :
	case GAMEADR_FUNC_INITITEMPACKET:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x15, "8A4DD08B16884D9C8BCEC745FC01000000", 0, "", NULL, NULL);
		}
		break ;
		//	case GAMEADR_SEND_PACKET_PET_FEED_ECX : 
	case GAMEADR_PACKET_CANCEL_BUFF_ECX:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0x2f, "8A4DD08B16884D9C8BCEC745FC01000000", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_QUEST_FRAME_OPTION_CLICKED_ECX :
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 14, "8B166A03528BD8FF15", 0x1b, "505357", NULL, NULL);
		}
		break ;
	case GAMEADR_QUEST_FRAME_OPTION_CLICKED_CALL :
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 22, "8B166A03528BD8FF15", 0x1b, "505357", NULL, NULL);
		}
		break ;

	case GAMEADR_SEND_PACKET_CANCEL_BUFF_INIT_CALL:
		{
			SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x56, "83F90C746D83F90D74688B5508", 0, "", &dwData, NULL);
		}
		break ;

	case GAMEADR_SEND_PACKET_CHAT_INIT_CALL:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -0x1d, "C645FC0FFF5004", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_EQUIP_ATTR_DESC:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0x1f, "518B4E1483C13C", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_FUNC_PETUSESKILL :
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 1, "8B0833D2668B1168000080BF68000080BF538BCF52", 0, "", NULL, NULL);
		}
		break ;

	case GAMEADR_BASE_CLIENTINITFALG:
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0xc, "0100000084c07509", 0, "", NULL , NULL) ;			//20150611 龙门客栈版本
			//dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0x33, "83c40833f63bc674098930", 0, "", NULL , NULL) ;
		}
		break ;

	case GAMEADR_FUNC_GETMISSIONDATA:
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 0x38, "73198bd86bdb298d4c0b05", 0, "", NULL , NULL) ;
		}
		break ;

	case GAMEADR_FUNC_STOPACTION:
		{
			dwData = 0 ;
			SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0, "81c1b80b00003bcf734a", 0, "", &dwData , NULL) ;
			if ( dwData != 0 )
				dwData -= 0x1c ;
		}
		break ;

	case GAMEADR_BASE_WORLDMAP:
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 4, "8948540fbfd28950500fbfff", 0, "", NULL , NULL) ;
		}
		break ;

	case GAMEADR_FUNC_GETMISSIONNPCINFO:
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 0x2d, "3c6e0f852101000085f67c05", 0, "", NULL , NULL) ;
		}
		break ;

	case GAMEADR_FUNC_PGETXPARAM:
		{

			dwData = 0 ;
			SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0, "393274224081c268020000", 0, "", &dwData , NULL) ;
			if ( dwData )
				dwData -= 0x1e ;

		}
		break ;

	case GAMEADR_FUNC_SENDPETATTACK:
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, 0x2A, "83f801740583f80275348b8eD0000000", 0, "", NULL , NULL) ; //3.56.0230 龙门
		}
		break ;

	case GAMEADR_MULTI_CLIENT_HANDLE:
		{
			dwData = SRCforModule( (HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0x02, "3db7000000757f", 0, "", NULL , NULL) ;
		}
		break ;
	default:
		;
	}

	if ( dwData == 0 )
		return FALSE ;

	dwRData = dwData ;
	return TRUE ;
}

BOOL Decrypt::matchOffsetCode( DWORD uIndex , DWORD& dwRData , HMODULE m_dwGameMainBaseAddress , HMODULE m_dwGameUi_ceguiBaseAddress , HMODULE hLuaPlusDllBaseAddress){

	UNREFERENCED_PARAMETER( hLuaPlusDllBaseAddress );
	UNREFERENCED_PARAMETER( m_dwGameUi_ceguiBaseAddress );
	DWORD dwData = 0 ;

	switch ( uIndex )
	{

	case GAMEOFFSET_OBJINFO_PSKILLTABLE :
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 8, "3bcf750666c74310ffff", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_OBJINFO_PXINFATABLE:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -113, "745a8b4d0c8b560833c085c90f9fc0", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_PBUFFTABLE:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -49, "33F683C4043BC6741B83C9FF", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_PBAGITEMLIST:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 2, "83ff647d7f", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_PRIDEBAGLIST:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 9, "7c7083f8067d6b5357", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_PPETLIST:
		{
			DWORD dwCall = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 1, 0, 0, -21, "8B3A518D4E20", 0, "", NULL, NULL) ;
			//			DPrint("dwCall_GAMEOFFSET_DATAPOOL_PPETLIST %x", dwCall);
			if ( dwCall != 0 )
			{
				dwData = _SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, dwCall, dwCall + 0x40, 2 , "A0010000", 0, "", NULL, NULL ) ;
				//				DPrint("dwData_GAMEOFFSET_DATAPOOL_PPETLIST %x", dwData);
			}
		}
		break ;

	case GAMEOFFSET_INTERFACE_PFUNCCAMPTYPE:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -25, "6a02eb0d85db750353eb06", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_PEQUIPMENTLIST:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -16, "c1e3028b0c1803c385c97421", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_GUILDWAR:
		{		
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -0x93, "3c0874303c09742c", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_WORLDMAN_STATE:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, 0x1b, "ff5220eb0233c0", 0, "", NULL, NULL);  //2014-04-25 偏移有变化 0x15 -> 0x1a
		}
		break ;

	case GAMEOFFSET_OBJ_ATTREX:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -19, "85d274088b016a01", 0, "", NULL, NULL);
		}
		break ;

	case GAMEOFFSET_DATAPOOL_TARGETINDEX:
		{
			dwData = SRCforModule((HMODULE)m_dwGameMainBaseAddress, 0, 0, 0, -18, "8b1150ff5244eb07", 0, "", NULL, NULL);
		}
		break ;
	default:
		;
	}

	if ( dwData == 0 )
		return FALSE ;

	dwRData = dwData ;
	return TRUE ;

}


BOOL Decrypt::initClientOffsetProcess(void)
{
	m_dwOffsetData[GAMEOFFSET_NET_SOCKET] = 0x38 ; //2148
	m_dwOffsetData[GAMEOFFSET_NET_CRITICALSECTION] = 0x458;//0x480; //0x474 ;

	m_dwOffsetData[GAMEOFFSET_OBJMAN_POBJECTTABLE] = 0x68 ; //Offset_GAMEOFFSET_OBJMAN_POBJECTTABLE // 8b 16 8b ce ff 92 ec 02 00 00
	m_dwOffsetData[GAMEOFFSET_OBJMAN_PPLAYERSELF] = 0x74 ;//0x70 --> 74 //Offset_GAMEOFFSET_OBJMAN_PPLAYERSELF // 8b c8 2b cf 81 f9 e8 03 00 00 72 73
	m_dwOffsetData[GAMEOFFSET_OBJFUNCPTR_CLASS] = 0x38 ; // Offset_GAMEOFFSET_OBJFUNCPTR_CLASS // 33 c0 83 fb 02 0f 94 c0
	m_dwOffsetData[GAMEOFFSET_OBJFUNCPTR_MODELID] = 0x284 ; // khong dung toi

	m_dwOffsetData[GAMEOFFSET_OBJ_INDEX] = 0x30 ;// Offset_GAMEOFFSET_OBJ_INDEX // 57 8b ce ff 52 7c bf 01 00 00 00 3b c7 8a 43
	m_dwOffsetData[GAMEOFFSET_OBJ_ID] =  0x34 ; // Offset_GAMEOFFSET_OBJ_INDEX
	m_dwOffsetData[GAMEOFFSET_OBJ_X] = 0x48 ;
	m_dwOffsetData[GAMEOFFSET_OBJ_Z] = 0x4c ;
	m_dwOffsetData[GAMEOFFSET_OBJ_Y] = 0x50 ;
	m_dwOffsetData[GAMEOFFSET_OBJ_PRESOURCE] = 0x118 ;
	m_dwOffsetData[GAMEOFFSET_OBJ_POBJINFO] = 0x1ec ; //Offset_GAMEOFFSET_OBJ_POBJINFO // 33 C0 80 FA 02 0F 94 C0 85 C0
	m_dwOffsetData[GAMEOFFSET_OBJ_LOGIC] = 0x1f4 ;		//lua:ComposeItem_Begin //Offset_GAMEOFFSET_OBJ_LOGIC // 57 33 ff 3b c3 89 be
	m_dwOffsetData[GAMEOFFSET_OBJ_PACTION] = 0x200 ;// Khi moi dang nhap vao game - trong thanh - cuoi thu // ff 50 40 3d 30 75 00 00 7d 70// Offset_GAMEOFFSET_OBJMAN_PPLAYERSELF
	m_dwOffsetData[GAMEOFFSET_OBJFUNC_USESKILL] = 0x1c ;//in MT_FUNCMSG_FUNC_SELFUSESKILL->FUNC_SELFUSESKILL
	m_dwOffsetData[GAMEOFFSET_OBJFUNC_USESKILL_SPEC2] = 0x18 ;

	m_dwOffsetData[GAMEOFFSET_OBJINFO_PARENT] = 0x00 ;	//主人
	m_dwOffsetData[GAMEOFFSET_OBJINFO_HPPERCENT] = 0x08 ;//8b 41 74 39 46 08 75 2f 8b 56 04// Offset_GAMEOFFSET_OBJINFO_HPPERCENT
	m_dwOffsetData[GAMEOFFSET_OBJINFO_MPPERCENT] = 0x0c ;//8b cb 8b 51 08 03 c7 3b d0// Offset_GAMEOFFSET_OBJINFO_MPPERCENT
	m_dwOffsetData[GAMEOFFSET_OBJINFO_SID] = 0x14 ;//8b 81 ec 01 00 00 85 c0 74 18 8b 48 04 8b 51 14 8b 45 // Offset_GAMEOFFSET_OBJINFO_SID_1
	m_dwOffsetData[GAMEOFFSET_OBJINFO_MODEL] = 0x1c ;//8b 81 40 04 00 00 8b 00 25 01 00 00 80 79 05 // Offset_GAMEOFFSET_OBJINFO_MODEL
	m_dwOffsetData[GAMEOFFSET_OBJINFO_FID] = 0x24 ;		//占有者,归属 // toan so ffffff ffffff
	m_dwOffsetData[GAMEOFFSET_OBJINFO_NAME] = 0x38 ;//
	m_dwOffsetData[GAMEOFFSET_OBJINFO_ATTRI] = 0x54 ;//89 46 04 8b 47 04 8b 48 14 83 c0 14 8b 40 04 ->[ESI+10]
	m_dwOffsetData[GAMEOFFSET_OBJINFO_SPEED] = 0x5c ;//
	m_dwOffsetData[GAMEOFFSET_OBJINFO_LEVEL] = 0x6c ;
	m_dwOffsetData[GAMEOFFSET_OBJINFO_NU] = 0x70 ;//Offset_GAMEOFFSET_OBJINFO_NU
	m_dwOffsetData[GAMEOFFSET_OBJINFO_TITLE] = 0x78 ;//
	m_dwOffsetData[GAMEOFFSET_OBJINFO_COLOR] = 0xa8 ;
	m_dwOffsetData[GAMEOFFSET_OBJINFO_STEALLEVEL] = 0x60 ;	//隐身等级 0 // Offset_GAMEOFFSET_OBJINFO_STEALLEVEL_60 //s_eqTable
	m_dwOffsetData[GAMEOFFSET_OBJINFO_BIANSHEN] = 0x9c;		//变身 0xffffffff 
	m_dwOffsetData[GAMEOFFSET_OBJINFO_RIDE] = 0xa4 ;	// 83 ba a4 00 00 00 ff 74 12 //Offset_GAMEOFFSET_OBJINFO_RIDE	//坐骑 0xffffffff
	m_dwOffsetData[GAMEOFFSET_OBJINFO_AITYPE] = 0x94 ;
	m_dwOffsetData[GAMEOFFSET_OBJINFO_RELATIVE] = 0x98 ;//关系 2-友好 0-敌对 1-中立?
	m_dwOffsetData[GAMEOFFSET_OBJINFO_GUILD] = 0xec ;//e4	//行会id
	m_dwOffsetData[GAMEOFFSET_OBJINFO_MENPAI] = 0xf0 ;	//针对玩家的
	m_dwOffsetData[GAMEOFFSET_OBJINFO_PKLIST] = 0x100 ;//f8	//pk列表

	m_dwOffsetData[GAMEOFFSET_PETINFO_ISPRESENT] = 0x4 ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_64ID] = 0x8 ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_ID] = 0x10 ;//Decrypt::MatchPacketCode_GAMEPACKET_RECV_PETINFOUPDATE
	m_dwOffsetData[GAMEOFFSET_PETINFO_SID] = 0x14 ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_NAME] = 0x20;
	m_dwOffsetData[GAMEOFFSET_PETINFO_LOVER64] = 0x5c ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_LEV] = 0x3c ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_EXP] = 0x44 ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_SEX] = 0x0c;//0x50;
	m_dwOffsetData[GAMEOFFSET_PETINFO_NHAPPY] = 0x58 ;// Hoan Ky
	m_dwOffsetData[GAMEOFFSET_PETINFO_HP] = 0x48 ;//HP o tren//Sinh Luc
	m_dwOffsetData[GAMEOFFSET_PETINFO_MAXHP] = 0x4c ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_LIFE] = 0x50 ;
	m_dwOffsetData[GAMEOFFSET_PETINFO_PSKILLLIST] = 0x120 ;//Offset_GAMEOFFSET_PETINFO_PSKILLLIST // 85 d2 7c 25 83 fa 0d 7d 20 69 c0 a0 01 00 00 03 81
	m_dwOffsetData[GAMEOFFSET_PETINFO_SIZE] = 0x1A0 ;//SetFightPetAsMainTarget

	m_dwOffsetData[GAMEOFFSET_SELFINFO_HP] = 0x273c;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MP] = 0x2740;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_TEP] = 0x2a7c;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MAXTEP] = 0x2a80; // Chi can cai nay thay doi thi check lai tat ca
	m_dwOffsetData[GAMEOFFSET_SELFINFO_EXP] = 0x2744;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_STRIKEPOINT] = 0x274c;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MONEY] = 0x2748;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MAXHP] = 0x27bc;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MAXMP] = 0x27c0;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_VIGOR] = 0x2750;//活力//44
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MAXVIGOR] = 0x2754;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_ENERGY] = 0x2758; //精力//4c
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MAXENERGY] = 0x275c;

	//这一段一般无变化
	m_dwOffsetData[GAMEOFFSET_SELFINFO_PKVALUE] = 0x2654;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_PET_EXTRANUM] = 0x266c;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_MONEY_JZ] = 0x2670;
	//		m_dwOffsetData[GAMEOFFSET_SELFINFO_TEAMFOLLOW] = 0x918 ;//10 //InTeamFollowMode

	m_dwOffsetData[GAMEOFFSET_SELFINFO_POWER] = 0x2760;
	m_dwOffsetData[GAMEOFFSET_SELFINFO_KILLMONSTERCOUNT] = 0x29f0;

	//这一段一般无变化
	m_dwOffsetData[GAMEOFFSET_SELFINFO_UNIONID] = 0x26a8;

	m_dwOffsetData[GAMEOFFSET_SELFINFO_TODAYEXP] = 0x2a58;

	//这一段没有直接的 lua 
	m_dwOffsetData[GAMEOFFSET_SELFINFO_FIGHTPET] = 0x2778;// in SetFightPetAsMainTarget
	m_dwOffsetData[GAMEOFFSET_SELFINFO_TEAMFOLLOW] = 0x918 ;//InTeamFollowMode	

	//----------- ->Player ->functions [0x0077d3d0]GetXinfaInfo
	m_dwOffsetData[GAMEOFFSET_XINFAITEM_ID] = 0x00 ;
	m_dwOffsetData[GAMEOFFSET_XINFAITEM_PNAME] = 0x08 ;
	//-------------------------------------------------------

	//----------- ->Player ->functions [0x0077d370]GetSkillInfo
	m_dwOffsetData[GAMEOFFSET_SKILLITEM_ID] = 0x00 ;
	m_dwOffsetData[GAMEOFFSET_SKILLITEM_PNAME] = 0x0C ;
	//--------------------------------------------------------

	m_dwOffsetData[GAMEOFFSET_SCRIPT_PFUNC_GETLUASTATE] = 0x3c ;//Decrypt::MatchClientCode ->Offset_GAMEOFFSET_SCRIPT_PFUNC_GETLUASTATE //8b 11 ff 52 3c 8b 00 53 50
	m_dwOffsetData[GAMEOFFSET_SCRIPT_PREVLUAENVSTRING] = 0x4c ;//??? // K dung toi
	m_dwOffsetData[GAMEOFFSET_INTERFACE_LOCKOBJ] = 0x3c ;//Khi bi vo tu - Noi chuyen voi cai nguc // MT_FUNCMSG::FUNC_SPEEKNPC ->GAMEOFFSET_INTERFACE_LOCKOBJ //8b 40 30 83 c1 64 3b c1
	m_dwOffsetData[GAMEOFFSET_INTERFACE_MOVETO] = 0x4c ;  //0x4c 无寻路的本地图移动/ khong tim duoc 0x50 是有寻路的本地图移动  0x84 是带寻路的跨地图移动 // MT_FUNCMSG::FUNC_SPEEKNPC ->Offset_GAMEOFFSET_INTERFACE_MOVETO 
	m_dwOffsetData[GAMEOFFSET_INTERFACE_COMPOSE] = 0x6c ;//Offset_GAMEOFFSET_INTERFACE_COMPOSE // 8b 11 50 57 53 ff
	m_dwOffsetData[GAMEOFFSET_INTERFACE_SPEEKNPC] = 0x7c ;//MT_FUNCMSG::FUNC_SPEEKNPC ->Offset_GAMEOFFSET_INTERFACE_SPEEKNPC
	m_dwOffsetData[GAMEOFFSET_INTERFACE_PICKUPOPEN] = 0x80 ;//Mo thung do //MT_FUNCMSG::FUNC_SPEEKNPC -> GAMEADR_FUNC_LOCKNPCCOUNT - Offset_GAMEOFFSET_INTERFACE_PICKUPOPEN
	m_dwOffsetData[GAMEOFFSET_DATAPOOL_ASKITEMINFO] = 0xe8 ;//???
	m_dwOffsetData[GAMEOFFSET_INTERFACE_OPENNPC] = 0x148;//0x144 ;// Lam nhiem vu (Level 10-30) di gap NPC de nhan Nhiem vu	//已经发生变化// ->Offset_GAMEOFFSET_INTERFACE_OPENNPC
	m_dwOffsetData[GAMEOFFSET_EVENT_PFUNCPUSHDEBUGMSG] = 0x68 ;	//Hien thi thong tin len phan tren cua man hinh game //push debug message // FUNC_PUSHDEBUGMESG ->Offset_GAMEOFFSET_EVENT_PFUNCPUSHDEBUGMSG //8b 16 8b f8 8b 1f// functions [0x0054b4d0]PushDebugMessage

	m_dwOffsetData[GAMEOFFSET_UIDATAPOOL_TASKLIST] = 0x30;//0x30 ;
	m_dwOffsetData[GAMEOFFSET_UIDATAPOOL_TALKMSG] = 0x34;//0x34 ;

	m_dwOffsetData[GAMEOFFSET_UIDATAPOOL_TALKDEBUGMSG] = 0x44 ;
	m_dwOffsetData[GAMEOFFSET_UIDATAPOOL_TEAMLIST] = 0xe4 ;

	m_dwOffsetData[GAMEOFFSET_WORLDMAP_MAPID] = 0x50 ;
	m_dwOffsetData[GAMEOFFSET_WORLDMAP_SCENEID] = 0x48 ;

	m_dwOffsetData[GAMEOFFSET_MISSION_PARAMINDEX] = 0x54 ;
	m_dwOffsetData[GAMEOFFSET_MISSION_STRLIST] = 0xb0 ;
	m_dwOffsetData[GAMEOFFSET_MISSION_DESCRIPT] = 0xe8 ;
	m_dwOffsetData[GAMEOFFSET_MISSION_NAME] = 0xcc ;
	m_dwOffsetData[GAMEOFFSET_LOGIN_ACCOUNTNAME] = 0xe0 ;

	m_dwOffsetData[GAMEOFFSET_DBC_GETINSTANCEPTR] = 0x44 ;
	m_dwOffsetData[GAMEOFFSET_DBC_SPINFANTEXPMIS] = 0x16a;
	m_dwOffsetData[GAMEOFFSET_DBC_SPMISSIONITEM] = 0xa9 ;


	HMODULE m_dwGameMainBaseAddress = GetModuleHandle(NULL);
	HMODULE m_dwGameUi_ceguiBaseAddress = GetModuleHandle("UI_CEGUI.dll");
	HMODULE hLuaPlusDllBaseAddress = GetModuleHandle("LuaPlus.dll");

	if (NULL == m_dwGameMainBaseAddress || NULL == m_dwGameUi_ceguiBaseAddress || NULL == hLuaPlusDllBaseAddress)
	{
		return FALSE;
	}
	BOOL bResult = TRUE ;
	for ( int i = 0 ; i < GAMEOFFSET_MAXINDEX ; i++ )
	{
		if ( m_dwOffsetData[i] == INFINITE )
			bResult = matchOffsetCode( i , m_dwOffsetData[i] , m_dwGameMainBaseAddress , m_dwGameUi_ceguiBaseAddress , hLuaPlusDllBaseAddress ) ? bResult : FALSE ;

	}

	int nUnvalidValue = 0 ; 
	for ( int i = 0 ; i < GAMEOFFSET_MAXINDEX ; i++ )
	{
		if ( m_dwOffsetData[i] == INFINITE )
		{
			DPrint("    Failed Offset:%03d" , i ) ;
			nUnvalidValue++ ;
		}else{
			DPrint("Dataoffset: %x", m_dwOffsetData[i]);
		}
	}

	if ( nUnvalidValue > 0 )
		bResult = FALSE ;

	return bResult ;
}

BOOL Decrypt::getAccountInfo( char* pBuffer , int len )// da login game ok
{
	CCommonObject tObj ;
	CAccountInfo tAccount ;
	if ( !tAccount.Init() )// Khoi tao m_dwObject = [GAMEADR_BASE_WORLDMAP] -0x10
	{
		strcpy_s( pBuffer, 100, "" ) ;
		return FALSE ;
	}
	

	//更新当前玩家数据 // cap nhat nguoi choi hien tai
	if ( !tObj.InitBySelf() )// Nguoi choi da login game
	{
		strcpy_s( pBuffer, 100, "" ) ;
		return FALSE ;
	}

	if ( !pBuffer )
	{
		strcpy_s( pBuffer, 100, "" ) ;
		return FALSE ;	
	}
	tAccount.GetAccountName( pBuffer , len ) ;// Get username login game laoxiaogl2@changyou.com
	if ( strlen(pBuffer) <= 0 )
	{
		strcpy_s( pBuffer, 100, "" ) ;
		return FALSE ;
	}

	return TRUE ;
}

bool Decrypt::initData()
{
	for( int i = 0 ; i < GAMEADR_MAXINDEX ; i++ )
	{
		m_dwDatatAddr[i] = INFINITE ;
	}
	for( int i = 0 ; i < GAMEOFFSET_MAXINDEX ; i++ )
	{
		m_dwOffsetData[i] = INFINITE ;
	}

	//for( int i = 0 ; i < GAME_PACKET_MAXINDEX ; i++ )
	//{
	//	m_stPacketData[i]. = INFINITE ;
	//}

	if( !initClientAddrProcess() )
	{
		app->lock() ;
		app->m_nInitAddrOk = ERROR_INIT_ADDR ;
		app->unLock() ;

#ifdef _DEBUG

		DPrint( "Error Result initClientAddrProcess : %d" , app->m_bLogin );
#endif
		return false ;
	}
	if( !initClientOffsetProcess() )
	{
		app->lock() ;
		app->m_nInitAddrOk = ERROR_INIT_OFFSET ;
		app->unLock() ;

#ifdef _DEBUG

		DPrint( "Error Result initClientOffsetProcess : %d" , app->m_bLogin );
#endif

		return false ;
	}

	if( !initPacketMsgProcess() )
	{		
		app->lock() ;
		app->m_nInitAddrOk = ERROR_INIT_PACKET ;
		app->unLock() ;

#ifdef _DEBUG

		DPrint( "Error Result initPacketMsgProcess : %d" , app->m_bLogin );
#endif


		return false ;
	}	

	return true ;

}

unsigned int _stdcall Decrypt::initAddrData( LPVOID lParam )
{
//	UNREFERENCED_PARAMETER( lParam );
	DPrint("Init Address data 1");	
	__AutoFreeLibrary_EnterProcess ;

	void* lpThis = ( void* )lParam;
	if(!((Decrypt*)lpThis)->initData() )
	{
		DPrint("Init Address data failed");		
		return 111 ;
	}
	DPrint("Init Address data 2");		
	if( !((Decrypt*)lpThis)->hookGameRecv() )
	{
		DPrint("Init Address packet data failed");		
		return 111 ;	
	}
	DPrint("Init Address data 3");		
	app->m_nInitAddrOk = ERROR_INIT_OK ;

	__AutoFreeLibrary_LeaveProcess ;
	return 0;
}
//
//unsigned int _stdcall Decrypt::initPacketData( LPVOID lParam )
//{
//	((Decrypt*)lpThis)->hookGameRecv() ; // kiemtra
//}
//
void Decrypt::startInitAddrProcess( void ) 
{
	CloseHandle( (HANDLE)_beginthreadex( NULL , 0 , initAddrData , this , 0 , NULL ) ) ;
//	CloseHandle( (HANDLE)_beginthreadex( NULL , 0 , initPacketData , this , 0 , NULL ) ) ;
}