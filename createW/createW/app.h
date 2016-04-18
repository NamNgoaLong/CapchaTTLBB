#pragma once
#include <windows.h>
#include "nxdati.h"
#include "CIniFile.h"
#include <map>
#include <vector>
#include "macro.h"
#include "Inject.h"


class App:public NXDATI{
public:
	bool bAutoStart;
	bool bCheckGain;
	bool bSavePass;

	CIniFile* ciniFile;
	static HINSTANCE hInstMain;
	static HWND hwndMain;
	static HWND hwndYourAcc;
	static HWND hwndRegister;
	static HWND hwndListProcess;
	static bool bStop; // stop thread load process
	HBITMAP hBmp;

	Inject objInject;

public:
	App();

	bool setConfigUser(void);
	bool getConfigUser(void);

	bool loadProcessGame(void);

	bool enCryptPassword(char *password,  char *outPass);
	bool deCryptPassword(char *password, char *outPass);	

	BOOL readFileMapping(int nCodeName, int nPID, bool bPassCapcha);
	char* createCaptchaBmp(int nPID, bool bCheck, char* lpBuffer, int nLen, int nWidth, int nHeight, int* nlpOutLen, BOOL IsSaveFile,  ...);

	BOOL createMappingBmp(int nCodeName, stObjRecognize &stObj);
	BOOL readFileMappingBMP(stCodeResult* objCodeRet);
	bool returnObjMapping(int nCodeName, stObjRecognize *stObjRet);

	bool loopMessageCapcha(void);
	

};
App::App(){
	 bAutoStart = false;
	 bCheckGain = false;
	 bSavePass = false;
	 hBmp = NULL;
	 NXDATI::NXDATI();	 
//	 objInject();
	 ciniFile = new CIniFile(NAME_FILE_CONFIG);
}