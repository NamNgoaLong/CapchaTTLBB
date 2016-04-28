#include "nxdati.h"




NXDATI::NXDATI()
{
	hInsDll = NULL;
	nReserve = 1;
	nPicType = 11;
	nSoftwareID = 10257;
	strcpy_s( szSoftwareKey, sizeof( szSoftwareKey ), "2a08fec22e39453db927c3c554b8e2c0" );
	strcpy_s( szUserName, sizeof( szUserName ), "" );
	strcpy_s( szPassword, sizeof( szPassword ), "" );

	strcpy_s( szResult, sizeof( szResult ), "" );
	strcpy_s( szPicID, sizeof( szPicID ), "" );
	strcpy_s( szCardId, sizeof( szCardId ), "" );
	strcpy_s( szPicFilePath, sizeof( szPicFilePath ), "" );
	strcpy_s( szEmail, sizeof( szEmail ), "" );
	strcpy_s( szNumQQ, sizeof( szNumQQ ), "" );
	strcpy_s( szNickName, sizeof( szNickName ), "" );
	strcpy_s( szPhone, sizeof( szPhone ), "" );

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
	hInsDll = hDll;
	return true;	
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
