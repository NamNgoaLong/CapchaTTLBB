
#include <windows.h>
#include "Lua.h"
#include <stdio.h>
//#include "function2.h"

/*
BOOL MyLua::luaDoStringV( const char* szCmd , ... )
{
	UNREFERENCED_PARAMETER( szCmd );
	//char szText[256] = {0};
	//va_list arglist;
//	functions::DPrint("%s",szCmd);
	//va_start( arglist, szCmd );
	//vsprintf( szText, szCmd, arglist );
	//va_end( arglist );


	//if ( TestThreadId(E_GAMEMAIN_THREAD) )
	//{
	//	return LuaDoStringSafe_CallBack(szText , NULL) ; 
	//}
	return true;

}
/**/
/*
BOOL MyLua::luaDoStringSafe_CallBack( const char* szCmd , void* lParam ){
	lua_result_list* pResult = (lua_result_list*)lParam;
	if(!szCmd)
		return FALSE;
	lua_result_list* pResult = (lua_result_list*)lParam;
	if(!szCmd)
		return FALSE;
	CScriptSystem tScript;
	if(!tScript.Init())
		return FALSE;
	DWORD dwState = tScript.GetLuaState();
	if(!dwState)
		return FALSE;


}
*/