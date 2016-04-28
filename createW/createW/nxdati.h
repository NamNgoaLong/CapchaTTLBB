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
