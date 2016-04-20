#pragma once
#include <windows.h>
#include "macro.h"
#include <stdio.h>


	typedef int ( _stdcall *Func_NX_RecognizeByBytesA )( char* nx_user, char* nx_pwd, int nx_soft_id, char* nx_soft_key, byte* byte_date, int byte_len, int nx_type, char *nx_code, char *nx_result );
	typedef int ( _stdcall *Func_NX_RecognizeByFileA )( char* nx_user, char* nx_pwd, int nx_soft_id, char* nx_soft_key, char * fname, int nx_type, char *nx_code, char *nx_result );
	typedef int ( _stdcall *Func_NX_RecognizeByScreenA )( char* nx_user, char* nx_pwd, int nx_soft_id, char* nx_soft_key, int x, int y, int w, int h, int t, char *nx_code, char *nx_result, char * save_path );

	typedef int ( _stdcall *Func_NX_ReportError )( char* nx_user, char * nx_code );
	typedef int ( _stdcall *Func_NX_QueryBlance )( char* nx_user, char * nx_pwd );
	typedef int ( _stdcall *Func_NX_RechargeA )( int nx_soft_id, char* nx_user, char* nx_pwd, char* nx_card_id, char* nx_result, int nx_reserve );
	typedef int ( _stdcall *Func_NX_RegUserA )( int nx_soft_id, char* nx_user, char* nx_pwd, char* nx_email, char* nx_qq, char* nx_rname, char* nx_phone, char* nx_result, int nx_reserve );



class NXDATI
{
public:
	HINSTANCE hInsDll;
	char szUserName[50];
	char szPassword[30];
	int  nSoftwareID;
	char szSoftwareKey[256];
	int  nPicType;
	char szResult[256];
	char szPicID[256];
	int nReserve;
	char szCardId[50];
	char szPicFilePath[256];
	char szEmail[256];
	char szNumQQ[256];
	char szNickName[256];
	char szPhone[256];
public:
	NXDATI();
	int recognizeByBytesA( char* szUserName,char* szPassword, int nSoftwareID, char* szSoftwareKey, byte* byte_date, int img_len, int nPicType, char* szPicID, char* szResult );
	int recognizeByFileA( char* szUserName, char* szPassword, int nSoftwareID, char* szSoftwareKey, char* szPicFilePath, int nPicType, char* szPicID, char* szResult );

//	int recognizeByScreenA(HINSTANCE hInsDll);

	int reportError( char*szUserName, char* szPicID );
	int queryBlance( char* szUserName, char* szPassword );
	int rechargeA( int nSoftwareID, char* szUserName, char* szPassword, char* szCardId, char* szResult, int nReserve );
	int regUserA( int nSoftwareID, char* szUserName, char* szPassword, char* szEmail, char* szQQ, char* szNName, char* szPhone, char* szResult, int nReserve );
	bool nxDatiInit();
	bool nxDatiUnInit();
//	bool read_image(char *bmpName, unsigned char *&byte_date,int &img_len);
};

//------------------------------------------------------------------------------


NXDATI::NXDATI()
{
	hInsDll = NULL;
	nReserve = 1;
	nPicType = 11;
	nSoftwareID = 10257;
	strcpy_s( szSoftwareKey, strlen( szSoftwareKey ) +1 , "2a08fec22e39453db927c3c554b8e2c0" );
	strcpy_s( szUserName, strlen( szUserName ) + 1, "" );
	strcpy_s( szPassword, strlen( szPassword ) + 1, "" );

	strcpy_s( szResult, strlen( szResult ) +1, "" );
	strcpy_s( szPicID, strlen( szPicID ) + 1, "" );
	strcpy_s( szCardId, strlen( szCardId ) + 1, "" );
	strcpy_s( szPicFilePath, strlen( szPicFilePath ) + 1, "" );
	strcpy_s( szEmail, strlen( szEmail ) + 1, "" );
	strcpy_s( szNumQQ, strlen( szNumQQ ) + 1, "" );
	strcpy_s( szNickName, strlen( szNickName ) + 1, "" );
	strcpy_s( szPhone,strlen( szPhone ) + 1, "" );

}
/// load module file NXCaptcha.dll 
bool NXDATI::nxDatiInit()
{
	HINSTANCE hDll;
	hDll = LoadLibraryA( NAME_DLL_NXDATI );
	if ( !hDll ) 
	{
		MessageBox( NULL, TEXT( "Error load file NXCaptcha.dll" ), TEXT( "Error!" ), 0 );
		return false;
	}
	else
	{
		hInsDll = hDll;
		return true;
	}
	
}

//int NXDATI::regUserA(int nSoftwareID = 0,char* szUserName = "xiaomi",char* szPassword = "xiaomi",char* szEmail = "888888@qq.com",char* szQQ = "888888",char* szNName = "张三",char* szPhone = "13888888888",char* szResult = "",int nReserve = 1){

int NXDATI::regUserA( int nSoftwareID, char* szUserName, char* szPassword, char* szEmail, char* szQQ, char* szNName, char* szPhone, char* szResult, int nReserve )
{
	Func_NX_RegUserA   NX_RegUserA   = ( Func_NX_RegUserA )GetProcAddress( hInsDll, "NX_RegUserA" );
	return NX_RegUserA( nSoftwareID, szUserName, szPassword, szEmail, szQQ, szNName, szPhone, szResult, nReserve );
}
int NXDATI::rechargeA( int nSoftwareID, char* szUserName, char* szPassword, char* szCardId, char* szResult, int nReserve )
{
	Func_NX_RechargeA   NX_RechargeA   = (Func_NX_RechargeA)GetProcAddress( hInsDll, "NX_RechargeA" );
	return NX_RechargeA( nSoftwareID, szUserName, szPassword, szCardId, szResult, nReserve );
}
/*
bool NXDATI::read_image(char *bmpName, unsigned char *&byte_date,int &img_len){
	FILE *  fpPhoto = fopen (bmpName, "rb") ; 
	if (!fpPhoto )     
		return false;
	char  szBuf[102400] ;
	img_len= fread (szBuf, 1, sizeof (szBuf), fpPhoto);
	fclose (fpPhoto);
	byte_date=( unsigned char *)szBuf;
	return true;
}
*/
int NXDATI::recognizeByBytesA( char *szUserName, char *szPassword, int nSoftwareID, char *szSoftwareKey, unsigned char* byte_date, int img_len, int nPicType, char *szPicID, char *szResult )
{
	Func_NX_RecognizeByBytesA   NX_RecognizeByBytesA  = (Func_NX_RecognizeByBytesA)GetProcAddress( hInsDll, "NX_RecognizeByBytesA" );
	return NX_RecognizeByBytesA( szUserName, szPassword, nSoftwareID, szSoftwareKey, byte_date, img_len, nPicType, szPicID, szResult );
}
int NXDATI::recognizeByFileA( char* szUserName, char* szPassword, int nSoftwareID, char* szSoftwareKey, char* szPicFilePath, int nPicType, char* szPicID, char* szResult)
{
	Func_NX_RecognizeByFileA   NX_RecognizeByFileA  = (Func_NX_RecognizeByFileA)GetProcAddress( hInsDll, "NX_RecognizeByFileA" );
	return NX_RecognizeByFileA( szUserName, szPassword, nSoftwareID, szSoftwareKey, szPicFilePath, nPicType, szPicID, szResult );
}
int NXDATI::reportError( char *szUserName, char *szPicID )
{
	Func_NX_ReportError   NX_ReportError  = (Func_NX_ReportError)GetProcAddress( hInsDll, "NX_ReportError" );
	return NX_ReportError( szUserName, szPicID );
}
/// free module file NXCaptcha.dll
bool NXDATI::nxDatiUnInit()
{
	FreeLibrary( hInsDll );
	hInsDll = NULL;
	return true;
}
/*
void NXDATI::setAllValue(int nSoftwareID, char *szSoftwareKey, int nPicType, char *szResult, char *szPicID, int nReserve, char *szCardId){
	nSoftwareID = nSoftwareID;				// 软件ID和软件key. 
	szSoftwareKey = szSoftwareKey;
	nPicType = nPicType;				// 图片类型.
	szResult = szResult;			// 返回的识别结果 (一定要大于或等于256个字节, 而且必须清零, 否则会出现错误)
	szPicID = szPicID;			// 返回的图片ID, 用于报告错误. (一定要大于或等于256个字节, 而且必须清零, 否则会出现错误. )
	nReserve = nReserve;				//扩展参数  添1即可
	szCardId = szCardId; //充值卡ID
//	szPicFilePath = szPicFilePath;	// 图片所在位置	
}
*/

int NXDATI::queryBlance( char* szUserName, char* szPassword )
{
	Func_NX_QueryBlance   NX_QueryBlance   = (Func_NX_QueryBlance)GetProcAddress( hInsDll, "NX_QueryBlance" );
	return NX_QueryBlance( szUserName, szPassword );
}
