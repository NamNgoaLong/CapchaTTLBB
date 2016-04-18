#pragma once

#define CLASS_NAME "AutoCapcha"
#define CLASS_NAME2 "AutoCapcha2"
#define CLASS_NAME3 "AutoCapcha3"

#define MYWM_ADD_PROCESS WM_USER + 0x2211
#define PROCESSNAME_DELETE_ALL	WM_USER + 0x2212
#define MYWM_MESSAGE WM_USER + 0x2333

#define NAME_DLL_NXDATI "NXCaptcha.dll"
#define NAME_FILE_CONFIG	".\\config.ini"
#define NAME_PATH_DLL "MycapchaDll\\"
#define NAME_PATH_VER_MAIN "MyCapchaVerM.dll"

// Send from game.exe
#define NAME_MAP_FILE "Global\\MyFileMappingObject" 
// Only for excute in Autocapcha
#define NAME_MAP_FILE_BMP "Global\\MyMappingBMP"

#define ENCRYTP	((strlen(app->szUserName)*300)%255)

#define NAME_PROCESS_GAME "Game.exe"

typedef struct _myPathCapcha
{
	char path[MAX_PATH];
}myPathCapcha;

typedef struct _myInfoPlayer{
	char infoPlayer[100];
}myInfoPlayer;

typedef struct _myResult{
	char result[256];
	int codeID;
}myResult;

typedef struct _nameProcess{
//	char exeName[MAX_PATH] ;
	char namePlayer[100] ;
	char status[100] ;
}nameProcess, *pNameProcess;

#define BUF_SIZE_BMP 0x240
typedef struct _stBmpCapcha{
	char szDataBmp[BUF_SIZE_BMP];
	char szAnser[128];
}stBmpCapcha;



#define BUF_SIZE_RECOGNIZE 128*108*4 +256

typedef struct _stObjRecognize{
	BITMAP bmp;
//	DWORD dwCodeName;
	char szAnser1[12];
	char szAnser2[12];
	char szAnser3[12];
	char szAnser4[12];
}stObjRecognize;

typedef struct _stObjMessage{
//	DWORD dwPID;
	DWORD dwCodeName;
	bool bSent;
}stObjMessage;

typedef struct _stCodeResult{
	int nCodeName;
	int nResult;
	int nPID;
}stCodeResult;

enum Result{result_nxdati_error = -1, result_nxdati_ok, nx_nameplayer, nx_pass_capcha};
