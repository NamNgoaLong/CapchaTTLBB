
#include <windows.h>
//#include "Decrypt.h"


typedef struct _LUA_RESULTLIST
{
	int nRet ;
	va_list arg_list ;
} lua_result_list , *plua_result_list;


class MyLua
{
public:
	//MyLua();
	//~MyLua();
	//	BOOL UpdateItemInfo( WGoodsItem* lpWgData , memdata::bagitem_o *lpMemData ) ;
	//	BOOL GetScriptInfo( int nIndex , char* pScriptName , int nScriptName , char* pScriptDest , int nScriptDest ) ;
	//	int IsInKVKServer() ;
	//	DWORD GetLuaStateOnThread() ;
	//	LRESULT GetSafeLuaThreadState() ;
	//	BOOL LuaDoString( const char* szCmd , int nRet = 0 , ... ) ;
	//	BOOL luaDoStringMul( const char* szPreCmd , const char* szCmd , int nRet = 0 , ... ) ;
		bool luaDoStringV( const char* szCmd , ... ) ;
	//	void PostLuaDoStringV( const char* szCmd , ... ) ;
	//	BOOL LuaDoStringV_Env( const char* szEnv , const char* szCmd , ... ) ;
	//	BOOL luaDoStringSafe_CallBack( const char* szCmd , void* pResult ) ;
	//	void TransferCryptText( char* szText , int nLen ) ;
	bool luaDoStringSafe_CallBack( const char* szCmd , void* lParam ) ;

};
