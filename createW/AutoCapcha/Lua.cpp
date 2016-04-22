
#include <windows.h>
#include "Lua.h"
#include <stdio.h>
#include "Decrypt.h"
#include "../LuaPlus/Src/LuaPlus/LuaPlus.h"

#pragma comment(lib,"LuaPlus.lib")
//using namespace functions;

bool MyLua::luaDoStringV( const char* szCmd , ... )
{
//	UNREFERENCED_PARAMETER( szCmd );
	char szText[256] = {0};
	va_list arglist;

	va_start( arglist, szCmd );
	vsprintf_s( szText, sizeof(szText), szCmd, arglist );
	va_end( arglist );	
	return luaDoStringSafe_CallBack(szText , NULL) ; 

}


void DPrint( char *format, ... )
{
	va_list arg_ptr;
	char outstr[0x1000];
	va_start( arg_ptr, format );
	vsprintf_s( outstr, sizeof( outstr ), format, arg_ptr );
	OutputDebugStringA( outstr );
	va_end( arg_ptr );
}

bool MyLua::luaDoStringSafe_CallBack( const char* szCmd , void* lParam )
{
	lua_result_list* pResult = (lua_result_list*)lParam ;

	if ( !szCmd )
		return FALSE ;
	CScriptSystem tScript ;
	if ( !tScript.Init() )
		return FALSE ;
	if ( !tScript.IsInit() )
		return FALSE ;
	DWORD dwState = tScript.GetLuaState() ;
//	DPrint("dwState %x", dwState);
	if ( !dwState )
		return FALSE ;	
	LuaPlus::LuaState* lpState = NULL ;
	lpState = (LuaPlus::LuaState*)dwState ;

	if ( !pResult )
	{
		try
		{
			lpState->DoString("__pre_G = _G;setmetatable(_G, {__pre_index = __index})") ;
			lpState->DoString( szCmd ) ;
		}
		catch(std::exception ex)
		{
			DPrint("GameLuaDo:%s , %s" , ex.what() , szCmd ) ;
			//LuaPlus::LuaState::Destroy( lpState ) ;
			lpState->DoString("_G = __pre_G;setmetatable(_G, {__index = __pre_index})") ;
			return FALSE ;
		}
		catch(...)
		{
			DPrint("GameLuaDo: Except! %s" , szCmd ) ;
			//LuaPlus::LuaState::Destroy( lpState ) ;
			lpState->DoString("_G = __pre_G;setmetatable(_G, {__index = __pre_index})") ;
			return FALSE ;
		}
		lpState->DoString("_G = __pre_G;setmetatable(_G, {__index = __pre_index})") ;
	}
	else
	{
		char szText[512] = {0} ;

		//默认一个参数
		sprintf_s( szText , sizeof(szText) , "%s" , "_MyResult1" ) ;

		int nPos = 0 ;
		for ( int i = 1 ; i < pResult->nRet ; i++ )
		{
			//不定参数的 void *lpData , int data_type , void* default_data ;
			nPos = strlen(szText) ;
			sprintf_s( szText + nPos , sizeof( szText ) - nPos , ",%s%d" , "_MyResult" , i+1 ) ;
		}

		nPos = strlen(szText) ;
		sprintf_s( szText + nPos , sizeof(szText) - nPos , "=%s" , szCmd ) ;

		//dostring
		try
		{
			lpState->DoString("__pre_G = _G;setmetatable(_G, {__pre_index = __index})") ;
			lpState->DoString(szText) ;

			//取出结果
			for ( int i = 0 ; i < pResult->nRet ; i++ )
			{
				sprintf_s( szText , sizeof(szText) , "_MyResult%d" , i+1 ) ; 
				LuaPlus::LuaObject ObjResult = lpState->GetGlobals().GetByName( szText ) ;

				LuaPlus::LuaObject* lpResult = va_arg( pResult->arg_list , LuaPlus::LuaObject* ) ;

				*lpResult = ObjResult ;
			}
		}
		catch(std::exception ex)
		{
			DPrint("GameLuaDo:%s" , ex.what()) ;
			//LuaPlus::LuaState::Destroy( lpState ) ;
			lpState->DoString("_G = __pre_G;setmetatable(_G, {__index = __pre_index})") ;
			return false ;
		}
		catch(...)
		{
			DPrint("GameLuaDo: Except!") ;
			//LuaPlus::LuaState::Destroy( lpState ) ;
			lpState->DoString("_G = __pre_G;setmetatable(_G, {__index = __pre_index})") ;
			return false ;
		}
		lpState->DoString("_G = __pre_G;setmetatable(_G, {__index = __pre_index})") ;
	}

	//LuaPlus::LuaState::Destroy( lpState ) ;
	DPrint("\\---------------- levea lua call -------------/" ) ;
	return true ;

}
