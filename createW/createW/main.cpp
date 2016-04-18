#include <stdlib.h> //atoi
#include <windows.h>
#include "iostream"
#include "resource.h"
#include "macro.h"
#include "app.h"
#include "function2.h"
#include <TlHelp32.h>
#include <map>
#include <vector>
#include <commctrl.h>
#include "cbuff.h"
#include "InputBox.h"
#pragma comment(lib, "ComCtl32.lib")

using namespace FuncTion;



HINSTANCE App::hInstMain = NULL;
HWND App::hwndMain = NULL;
HWND App::hwndYourAcc = NULL;
HWND App::hwndRegister = NULL;
HWND App::hwndListProcess;
bool App::bStop = false; // stop thread load process

static std::multimap<DWORD, nameProcess> listProcessOld;
static std::multimap<DWORD, stObjMessage> listObjMessage;
static std::vector<int> listProRunning;// Process running in list Process and it choosed
static int iSelect = 0; 
static bool bFlagBtnRecord = false;
static HWND hwndDlgRegistry = NULL;
static HWND hwndDlgMain = NULL;
static int nThread = 0;

LVCOLUMN LvCol; // Make Coluom struct for ListView
LVITEM LvItem;  // ListView Item struct



LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
//LRESULT CALLBACK WndProcYourAccount( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WndProcRegistry( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WndProcDislpay( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
App* app = NULL;


/*
bool myLoopMessageDlg(HWND hWnd);
bool myLoopMessageWindow(HWND hWnd);
BOOL isMainWindow(HWND handle);
LRESULT ProcessCustomDraw (LPARAM lParam, std::vector<int> &vtItems);
HWND findHandleMain(DWORD proceesID);
void DPrint(char *format, ...);
void mySetFont(HWND hWnd, char* szFontName, int nSize);
*/




UINT ThreadSendRecognize(LPVOID lParam){
	DPrint("Thread thu %d", ++nThread);
		std::multimap<DWORD, stObjMessage>::const_iterator ptr = listObjMessage.find((int)lParam);	
		app->readFileMapping(ptr->second.dwCodeName, ptr->first, true);	
	return 0;
}


UINT ThreadLoadProcess(LPVOID lParam){
	app->loadProcessGame();
	return 0;
}

UINT ThreadLoopMsg(LPVOID lParam){
//	UNREFERENCED_PARAMETER( lParam);
	app->loopMessageCapcha();
	return 0;
}

static int nFlagSync = 0;
CRITICAL_SECTION m_cs;
bool App::loopMessageCapcha(){
	InitializeCriticalSection(&m_cs); 
	DWORD AppThreadId;
	HANDLE hThrd[5];						

	while(!app->bStop){
		if(listObjMessage.empty())
			continue;
		std::multimap<DWORD, stObjMessage>::iterator ptr = listObjMessage.begin();
		for(; ptr != listObjMessage.end(); ptr++){
			for(int i = 0; i < listProRunning.size(); i++){
				if(ptr->first == (int)listProRunning.at(i)){
					if(false == ptr->second.bSent){
						hThrd[nFlagSync] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) ThreadSendRecognize,(LPVOID)ptr->first, 0, &AppThreadId);
						ptr->second.bSent = true;
//						WaitForMultipleObjects(++nFlagSync,hThrd, TRUE,INFINITE);
					}
				}
			}
		}
		Sleep(300);
	}
	DeleteCriticalSection(&m_cs);
	return true;
}
BOOL App::createMappingBmp(int nCodeName, stObjRecognize &stObj){
	char szName[256];
	strcpy(szName, NAME_MAP_FILE_BMP);
	char szCodeName[256];
	sprintf_s(szCodeName, sizeof(szCodeName),"%d", int(nCodeName));
	strcat(szName, szCodeName);

	HANDLE hMapFile;
	char* pBuf;
	hMapFile = CreateFileMappingA(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE_RECOGNIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		return FALSE;
	}
	pBuf = (LPTSTR) MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		BUF_SIZE_RECOGNIZE);

	if (pBuf == NULL)
	{
		CloseHandle(hMapFile);

		return FALSE;
	}
		CopyMemory((PVOID)pBuf, &stObj, sizeof(stObj));
		UnmapViewOfFile(pBuf);
//	CloseHandle(hMapFile);
	return TRUE;
}


char* App::createCaptchaBmp(int nPID, bool bCheck, char* lpBuffer, int nLen, int nWidth, int nHeight, int* nlpOutLen, BOOL IsSaveFile,  ...)
{

	va_list	arg_ptr;
	va_start(arg_ptr, IsSaveFile);

	int nBmpWidth = nWidth;
	int nBmpHeight = nHeight;

	BYTE *pData = NULL;
	BITMAPINFO bmpInfo = {0};
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bmpInfo.bmiHeader.biWidth = nBmpWidth;  
	bmpInfo.bmiHeader.biHeight = nBmpHeight;  
	bmpInfo.bmiHeader.biPlanes = 1;  
	bmpInfo.bmiHeader.biBitCount = 1; 


	BITMAPINFOHEADER bmInfoHeader = {0};  
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bmInfoHeader.biWidth = nBmpWidth;  
	bmInfoHeader.biHeight = nBmpHeight;  
	bmInfoHeader.biPlanes = 1;  
	bmInfoHeader.biBitCount = 1;  


	RGBQUAD aColors[2] = {0};
	aColors[0].rgbReserved = 0;
	aColors[0].rgbRed = 0;
	aColors[0].rgbGreen = 0;
	aColors[0].rgbBlue = 0;

	aColors[1].rgbReserved = 0;
	aColors[1].rgbRed = 255;
	aColors[1].rgbGreen = 255;
	aColors[1].rgbBlue = 255;


	BITMAPFILEHEADER bmFileHeader = {0};  

	bmFileHeader.bfType = 0x4d42;  //bmp    
	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*2;  
	bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((nBmpWidth * nBmpHeight)/8); 


	
	char* p1 = va_arg(arg_ptr, char*);
	char* p2 = va_arg(arg_ptr, char*);
	char* p3 = va_arg(arg_ptr, char*);
	char* p4 = va_arg(arg_ptr, char*);


	HBITMAP hBmp = NULL;  
	HDC hdcMem = CreateCompatibleDC(NULL);	
//		GdiFlush();
	hBmp = CreateDIBSection(hdcMem, &bmpInfo, DIB_RGB_COLORS, (void **)&pData, NULL, 0);
	if (hBmp == NULL)
	{
		DPrint("CreateDIBSection error [%d]", GetLastError());
		DeleteObject(hdcMem);
		va_end(arg_ptr);
		return NULL;
	}

	HGDIOBJ hOldObj = SelectObject(hdcMem, hBmp);
	for (int i=0; i < nHeight; i++)
	{
		for (int j=0; j < nWidth/8; j++)
		{
			for (int k=8; k > 0; k--)
			{
				if (-1 == SetPixel(hdcMem, j*8+8-k, i, ((lpBuffer[j+i*(nWidth/8)] >> (k-1))&1) ? RGB(255,255,255) : RGB(0,0,0)))
				{
//					DPrint("SetPixel error [%d]", GetLastError());
				}
			}
		}
	}

	if(bCheck){
		std::multimap<DWORD, stObjMessage>::const_iterator ptr2 = listObjMessage.find(nPID);
		stObjRecognize stObj;
		strcpy(stObj.szAnser1, p1);
		strcpy(stObj.szAnser2, p2);
		strcpy(stObj.szAnser3, p3);
		strcpy(stObj.szAnser4, p4);
		GetObject(hBmp, sizeof(BITMAP), &stObj.bmp);
		app->createMappingBmp(ptr2->second.dwCodeName, stObj);
	}
	nBmpHeight = nHeight*3;
	bmpInfo.bmiHeader.biHeight = nBmpHeight;  		
	hdcMem = CreateCompatibleDC(NULL);
	hBmp = CreateDIBSection(hdcMem, &bmpInfo, DIB_RGB_COLORS, (void **)&pData, NULL, 0);
	if (hBmp == NULL)
	{
		DPrint("CreateDIBSection error2 [%d]", GetLastError());
		DeleteObject(hdcMem);
		va_end(arg_ptr);
		return NULL;
	}
	hOldObj = SelectObject(hdcMem, hBmp);	

	for (int i = nHeight; i < nHeight*3; i++)
	{
		for (int j=0; j < nWidth/8; j++)
		{
			for (int k=8; k > 0; k--)
			{
				if (-1 == SetPixel(hdcMem, j*8+8-k, i, RGB(255,255,255)))

				{
//					DPrint("Error Setpixel 2");
				}
			}
		}

	}	



	bmInfoHeader.biWidth = nBmpWidth;  
	bmInfoHeader.biHeight = nBmpHeight;  

	HFONT hFont;
	hFont = CreateFont(12,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("宋体"));
	SelectObject(hdcMem, hFont);

	SetTextColor(hdcMem, 0);
	TextOut(hdcMem, 10, nBmpHeight-68, p1, strlen(p1));
	TextOut(hdcMem, 10, nBmpHeight-52, p2, strlen(p2));
	TextOut(hdcMem, 10, nBmpHeight-36, p3, strlen(p3));
	TextOut(hdcMem, 10, nBmpHeight-20, p4, strlen(p4));

	bmFileHeader.bfSize = bmFileHeader.bfOffBits + ((nBmpWidth * nBmpHeight)/8); 
//// kiemtra
	if (IsSaveFile)
	{
		HANDLE hFile = CreateFile("Bitmap1.bmp",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

		if ( INVALID_HANDLE_VALUE != hFile )
		{
			DWORD dwWrite = 0;  
			WriteFile(hFile,&bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
			WriteFile(hFile,&bmInfoHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
			WriteFile(hFile,&aColors[0], sizeof(RGBQUAD)*2, &dwWrite, NULL);
			WriteFile(hFile, pData, ((bmInfoHeader.biWidth * bmInfoHeader.biHeight)/8), &dwWrite, NULL);
			CloseHandle(hFile);
//			DPrint("答题图片:%s" , (char*)Dati.strCaptchaFileName.GetBuffer()) ;
		}
		else
		{
			DPrint("Error Create file bitmap to save 生成用于答题的图片时发生错误,无法答题! 错误代码:0x%08x" , ::GetLastError()) ;
		}
	}

	
	CBuff buf;
	buf.writem((char*)&bmFileHeader, sizeof(BITMAPFILEHEADER));
	buf.writem((char*)&bmInfoHeader, sizeof(BITMAPINFOHEADER));
	buf.writem((char*)&aColors[0], sizeof(RGBQUAD)*2);
	buf.writem((char*)pData, ((bmInfoHeader.biWidth * bmInfoHeader.biHeight)/8));
	char* pBuff = (char *)malloc(buf.size());
	memcpy(pBuff, buf.buff(), buf.size());
	*nlpOutLen = buf.size();

//	GdiFlush();
	SelectObject(hdcMem, hOldObj);  
	DeleteObject(hBmp); 
	DeleteObject(hdcMem);

	va_end(arg_ptr);
	return pBuff;
}
bool App::returnObjMapping(int nCodeName, stObjRecognize *stObjRet){
	char szName[256];
	strcpy(szName, NAME_MAP_FILE_BMP);//kiemtra
	char szCodeName[256];
	sprintf_s(szCodeName, sizeof(szCodeName),"%d", nCodeName);
	strcat(szName, szCodeName);

	HANDLE hMapFile;
	LPCTSTR pBuf;
	BITMAP* pBmp;
	stObjRecognize* stObj;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		DPrint("Could not open file mapping object (%d).\n", GetLastError());
		return false;
	}

	stObj = (stObjRecognize*) MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE_RECOGNIZE);

	if (stObj == NULL)
	{
		DPrint("Could not map view of file (%d).\n",GetLastError());
		CloseHandle(hMapFile);
		return false;
	}
	*stObjRet = *stObj;
	UnmapViewOfFile(stObj);
	CloseHandle(hMapFile);
	return true;
}

BOOL App::readFileMappingBMP(stCodeResult* objCodeRet){

	char szClassName[256];
	sprintf_s(szClassName, sizeof(szClassName), "Dis1Play%d", objCodeRet->nCodeName);	
	myRegisterDlg(GetModuleHandle(NULL),  szClassName, (WNDPROC)WndProcDislpay);	
	HWND hwndDlgDisplay = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_DISPLAYBMP), NULL/*(HWND)app->hwndMain*/, (DLGPROC)WndProcDislpay, (LPARAM)(LPVOID) objCodeRet);
	if(NULL == hwndDlgDisplay){
		MessageBox(NULL, TEXT("ERROR Create hwndDlgDisplay"), TEXT("Error!"), 0);
		return false;
	}
	ShowWindow(hwndDlgDisplay, SW_NORMAL);
	UpdateWindow(hwndDlgDisplay);
	myLoopMessageDlg(hwndDlgDisplay);
	myUnRegister(GetModuleHandle(NULL), szClassName);
	return TRUE;
}
// ODAY
LRESULT CALLBACK WndProcDislpay( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ){
	static HANDLE hbm;
	static HDC hdcTemp;
	static int nResultAnser =0;
	static stCodeResult* stObjRet;
	static HWND hwndSTbmp;
	static	stObjRecognize stObjReco;
	static bool bLParam = false;
	HBRUSH hBackground = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	switch(msg){
/*
		case WM_PAINT:
			{
//				if(bLParam){
					RECT r;
					GetClientRect(GetDlgItem(hWnd, IDC_STATIC_BITMAP), &r);
					hbm = CreateBitmapIndirect(&stObjReco.bmp);
					HDC hdc = GetDC(GetDlgItem(hWnd, IDC_STATIC_BITMAP));
					hdcTemp = CreateCompatibleDC(hdc);
					SelectObject(hdcTemp, hbm);
					StretchBlt(hdc, r.left, r.top, stObjReco.bmp.bmWidth*1.5 , stObjReco.bmp.bmHeight*1.5, hdcTemp, 0, 0, stObjReco.bmp.bmWidth, stObjReco.bmp.bmHeight, SRCCOPY);					
					::ReleaseDC(GetDlgItem(hWnd, IDC_STATIC_BITMAP), hdc);
//					bLParam = false;
//				}
			}break;
*/		
		case WM_INITDIALOG:
			{
				RECT r, r2;
				GetWindowRect(app->hwndMain, &r);
				GetWindowRect(hWnd, &r2);
				if(!bFlagBtnRecord)
					SetWindowPos(hWnd, HWND_TOP, r.left , r.top + (r.bottom - r.top)/2, r2.right - r2.left, (r2.bottom - r2.top), SWP_SHOWWINDOW);
				else
					SetWindowPos(hWnd, HWND_TOP, r.left, r.top + (r.bottom - r.top)/2, (r2.right - r2.left)/2, (r2.bottom - r2.top), SWP_SHOWWINDOW);

				stObjRet = (stCodeResult*)lParam;
				app->returnObjMapping(stObjRet->nCodeName, &stObjReco);
				hbm = CreateBitmapIndirect(&stObjReco.bmp);
				SendMessage(GetDlgItem(hWnd, IDC_STATIC_BITMAP), (WPARAM)STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);
//				bLParam = true;
				SetWindowText(GetDlgItem(hWnd, IDC_RADIO_ANSER1), stObjReco.szAnser1);
				SetWindowText(GetDlgItem(hWnd, IDC_RADIO_ANSER2), stObjReco.szAnser2);
				SetWindowText(GetDlgItem(hWnd, IDC_RADIO_ANSER3), stObjReco.szAnser3);
				SetWindowText(GetDlgItem(hWnd, IDC_RADIO_ANSER4), stObjReco.szAnser4);
//				InvalidateRect(GetDlgItem(hWnd, IDC_STATIC_BITMAP), &r, false);				
				UpdateWindow(hWnd);

			}break;
	case WM_CTLCOLORSTATIC:
		{

			switch(stObjRet->nResult)
			{
			case 1:
				{
					if((HWND)lParam == GetDlgItem(hWnd, IDC_RADIO_ANSER1)){
						HDC hdc = (HDC)wParam;
						SetTextColor(hdc, RGB(255,0,0));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
						return (LRESULT)hBackground;
					}
				}break;
			case 2:
				{
					if((HWND)lParam == GetDlgItem(hWnd, IDC_RADIO_ANSER2)){
						HDC hdc = (HDC)wParam;
						SetTextColor(hdc, RGB(255,0,0));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
						return (LRESULT)hBackground;
					}
				}break;

			case 3:
				{
					if((HWND)lParam == GetDlgItem(hWnd, IDC_RADIO_ANSER3)){
						HDC hdc = (HDC)wParam;
						SetTextColor(hdc, RGB(255,0,0));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
						return (LRESULT)hBackground;
					}
				}break;

			case 4:
				{
					if((HWND)lParam == GetDlgItem(hWnd, IDC_RADIO_ANSER4)){
						HDC hdc = (HDC)wParam;
						SetTextColor(hdc, RGB(255,0,0));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
						return (LRESULT)hBackground;
					}
				}break;

			}

		}break;
	
	case WM_COMMAND:
		{
			if(HIWORD(wParam) == BN_CLICKED){
				switch(LOWORD(wParam))
				{
				case IDC_RADIO_ANSER1:
					{
						nResultAnser = 1;
					}break;
				case IDC_RADIO_ANSER2:
					{
						nResultAnser = 2;
					}break;
				case IDC_RADIO_ANSER3:
					{
						nResultAnser = 3;
					}break;
				case IDC_RADIO_ANSER4:
					{
						nResultAnser = 4;
					}break;				
				
				}
			}
			if(wParam == IDC_BUTTON_DISPLAY_OK){
				SendMessage(findHandleMain(stObjRet->nPID), MYWM_MESSAGE, (WPARAM)stObjRet->nPID, (LPARAM)nResultAnser);

			}else if(wParam == IDC_BUTTON_DISPAY_CANCEL){
				nResultAnser = stObjRet->nResult;
//				SendMessage(findHandleMain(stObjRet->nPID), MYWM_MESSAGE, (WPARAM)stObjRet->nPID, (LPARAM)nResultAnser);
			}
			SendMessage(findHandleMain(stObjRet->nPID), MYWM_MESSAGE, (WPARAM)stObjRet->nPID, (LPARAM)777);
			SendMessage(hWnd, WM_CLOSE, 0, 0);

		}break;
	case WM_CLOSE:
		{
			DeleteDC(hdcTemp);
			DeleteObject(hbm);
			DeleteObject(hBackground);
			hbm = NULL;
			DestroyWindow(hWnd);
		}break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}break;
	}
	return  DefWindowProc(hWnd, msg, wParam, lParam);
	
}



BOOL App::readFileMapping(int nCodeName, int nPID, bool bPassCapcha){
	HANDLE hMapFile;
	stBmpCapcha* stDataCapcha;
	char szName[256];
	strcpy(szName, NAME_MAP_FILE);
	char szCodeName[256];
	sprintf_s(szCodeName, sizeof(szCodeName),"%d", int(nCodeName));
	strcat(szName, szCodeName);
	DPrint("szPathFile %s", szName);

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (NULL == hMapFile)
	{
		DPrint("Could not open file mapping object (%d).\n", GetLastError());
		return FALSE;
	}
	if(bPassCapcha){
		stDataCapcha = (stBmpCapcha*) MapViewOfFile(hMapFile, // handle to map object
			FILE_MAP_ALL_ACCESS,  // read/write permission
			0,
			0,
			BUF_SIZE_BMP +256);

		if (NULL == stDataCapcha)
		{
			DPrint("Could not map view of file (%d).\n", GetLastError());
			CloseHandle(hMapFile);
			return FALSE;
		}

		stBmpCapcha stDataBmp;

		memcpy(stDataBmp.szDataBmp, stDataCapcha->szDataBmp, 0x240);
		strcpy(stDataBmp.szAnser, stDataCapcha->szAnser);
		char szAnser1[9];
		char szAnser2[9];
		char szAnser3[9];
		char szAnser4[9];
		strncpy(szAnser1, stDataCapcha->szAnser, 8);
		szAnser1[8] = '\0';
		strncpy(szAnser2, stDataCapcha->szAnser +8, 8);
		szAnser2[8] = '\0';
		strncpy(szAnser3, stDataCapcha->szAnser +8*2, 8);
		szAnser3[8] = '\0';
		strncpy(szAnser4, stDataCapcha->szAnser +8*3, 8);
		szAnser4[8] = '\0';

		char szAnserA[12] = "1. ";
		char szAnserB[12] = "2. ";
		char szAnserC[12] = "3. ";
		char szAnserD[12] = "4. ";
		strcat(szAnserA, szAnser1);
		strcat(szAnserB, szAnser2);
		strcat(szAnserC, szAnser3);
		strcat(szAnserD, szAnser4);
		DPrint("String anser: %s", stDataBmp.szAnser);
		DPrint("szAnserA: %s, szAnserB: %s, szAnserC: %s, szAnserD: %s ", szAnserA, szAnserB, szAnserC, szAnserD);

		GetWindowTextA(GetDlgItem(app->hwndMain, IDC_EDIT_USERNAME), app->szUserName, sizeof(app->szUserName));	
		GetWindowTextA(GetDlgItem(app->hwndMain, IDC_EDIT_PASSWORD), app->szPassword, sizeof(app->szPassword));		

		int nLenBmp;
		char* pszBmpBuf = (char*)app->createCaptchaBmp(nPID, (IsDlgButtonChecked(app->hwndMain, IDC_CHECK_CHECKAGAIN)), stDataCapcha->szDataBmp, 0x240, 0x10*8, 0x24, &nLenBmp, 1, szAnserA, szAnserB, szAnserC, szAnserD);
		if(!pszBmpBuf){
			DPrint("Error crate bitmap image: %d", GetLastError());
			return FALSE;
		}
//		DPrint("Bitmap Data from CBuf: %s", pszBmpBuf);
		int nRet;
		char szResult[256];
		nRet = app->recognizeByBytesA(app->szUserName, app->szPassword, app->nSoftwareID, app->szSoftwareKey, (unsigned char*)pszBmpBuf, nLenBmp, app->nPicType, app->szPicID, szResult);
		std::multimap<DWORD, stObjMessage>::iterator ptr = listObjMessage.find(nPID);
//		EnterCriticalSection(&m_cs);
//		ptr->second.bSent = true;
//		DPrint("second.bSent %d", ptr->second.bSent);
//		LeaveCriticalSection(&m_cs);
// Neu kiem tra lai ket qua		
//		strcpy(szResult, "1");//kiemtra
		strcpy(app->szResult, szResult);
		DPrint("Result from nxDati %s", szResult);
			if(nRet > 0 && (0 != strcmp(app->szPicID, ""))){
				if(0 != strcmp(app->szResult, "error")){
					if(IsDlgButtonChecked(app->hwndMain, IDC_CHECK_CHECKAGAIN)){
						stCodeResult stMsgCode;
						stMsgCode.nCodeName = (int)ptr->second.dwCodeName;
						if(( 0 != strlen(szResult)) && (isdigit(szResult[0]))){
							stMsgCode.nResult = atoi(szResult);
							stMsgCode.nPID = nPID;
							app->readFileMappingBMP(&stMsgCode);//kiemtra

						}else
							DPrint("Error result is not a number %d", isdigit(szResult[0]));						
					}else{
						if(( 0 != strlen(szResult)) && (isdigit(szResult[0]))){
							SendMessage(findHandleMain(nPID), MYWM_MESSAGE, (WPARAM)nPID, (LPARAM)(atoi(szResult)));
							SendMessage(findHandleMain(nPID), MYWM_MESSAGE, (WPARAM)nPID, (LPARAM)777);
						}else
							DPrint("Error result is not a number %d", isdigit(szResult[0]));						


					}				

				}else{
					SendMessage(findHandleMain(nPID), MYWM_MESSAGE, (WPARAM)nPID, (LPARAM)(555));// Error because formats Image and information data send to nxDati
					SendMessage(findHandleMain(nPID), MYWM_MESSAGE, (WPARAM)nPID, (LPARAM)777);
				}
			}else{
				nRet = app->reportError(app->szUserName, app->szPicID);
				if(nRet > 0){
					SendMessage(findHandleMain(nPID), MYWM_MESSAGE, (WPARAM)nPID, (LPARAM)nRet);
					SendMessage(findHandleMain(nPID), MYWM_MESSAGE, (WPARAM)nPID, (LPARAM)777);
				}else
					DPrint("Can not report Error from nxDati");

			}
		UnmapViewOfFile(stDataCapcha);
	}else{
		char* pszNamePlayer = (char*) MapViewOfFile(hMapFile, // handle to map object
			FILE_MAP_ALL_ACCESS,  // read/write permission
			0,
			0,
			256);

		if (NULL == pszNamePlayer)
		{
			DPrint("Could not map view of file (%d).\n", GetLastError());
			CloseHandle(hMapFile);
			return FALSE;
		}
		std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcessOld.begin();
		for(int i = 0; ptr != listProcessOld.end(); ptr++, i++){
			if(ptr->first == nPID){
					DPrint("nx_nameplayer %s", pszNamePlayer);
					strcpy((char*)ptr->second.namePlayer, pszNamePlayer);
					strcpy((char*)ptr->second.status, "Login...");
//					SendMessage(app->hwndMain, MYWM_ADD_PROCESS, 0, 0);			
			}break;
		}
		UnmapViewOfFile(pszNamePlayer);
	}

	CloseHandle(hMapFile);

	return TRUE;
}

bool App::loadProcessGame(){
	
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	static std::multimap<DWORD,nameProcess> listProcess;
	nameProcess pName;
	strcpy(pName.namePlayer, "Unknown");
	strcpy(pName.status, "Plase login Bot Game");
	int countChage =0;
	int countChageOld = 0;
	while(!app->bStop){
		hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		if( hProcessSnap == INVALID_HANDLE_VALUE){
			CloseHandle(hProcessSnap);
			app->bStop = true;
			return false;
		}
		BOOL cont = Process32First( hProcessSnap, &pe32 );
		while(cont)	{
			if(pe32.th32ProcessID != 0){			
				if(strcmp(pe32.szExeFile,"Game.exe") == 0){

					listProcess.insert(std::make_pair(pe32.th32ProcessID,pName));					
				}
			}
			cont = Process32Next(hProcessSnap, &pe32);		
		}
//Chua co game.exe nao hoat dong
		if(listProcessOld.empty()){
// Co xuat hien game.exe moi hoat dong (game moi duoc bat len)
			if(!listProcess.empty()){
				listProcessOld = listProcess;
// yeu cau den tung process game tra ve thong tin nguoi choi game
				std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcessOld.begin();
				for(; ptr != listProcessOld.end(); ptr++){					
					SendMessage(findHandleMain(ptr->first), MYWM_MESSAGE, (WPARAM)ptr->first, (LPARAM)-1);
				}
				SendMessage(app->hwndMain,MYWM_ADD_PROCESS,0,0);
			}
		}else{
// Neu khong co game.exe nao dang hoat dong (tat ca game da duoc exit)
			if(listProcess.empty()){
				listProcessOld.clear();
				SendMessage(app->hwndMain,MYWM_ADD_PROCESS,0,(LPARAM)PROCESSNAME_DELETE_ALL);
			}else{
// Co 1 vai game.exe da bi exit hoat co them 1 vai game.exe duoc bat them (sizeList != sizeListOld)
				if(listProcess.size() != listProcessOld.size()){
					if(listProcess.size() > listProcessOld.size()){
						for(std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcess.begin(); ptr != listProcess.end(); ptr++){
							if(0 == listProcessOld.count(ptr->first)){
								SendMessage(findHandleMain(ptr->first), MYWM_MESSAGE, (WPARAM)ptr->first, (LPARAM)-1);
							}
						}
					}
					for(std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcess.begin(); ptr != listProcess.end(); ptr++){
						for(std::multimap<DWORD, nameProcess>::const_iterator ptrOld = listProcessOld.begin(); ptrOld != listProcessOld.end(); ptrOld++){
							if(ptr->first == ptrOld->first){
								strcpy((char*)ptr->second.namePlayer, (char*)ptrOld->second.namePlayer);
								strcpy((char*)ptr->second.status, (char*)ptrOld->second.status);
							}
						}					
					}
						listProcessOld = listProcess;					
					SendMessage(app->hwndMain,MYWM_ADD_PROCESS,0,0);
				}
			}
		}
		listProcess.clear();
		CloseHandle( hProcessSnap );          // clean the snapshot object		
		Sleep(300);
	}
	return true;

}


bool App::enCryptPassword(char *password,  char *outPass){
	if(password && (strlen(app->szUserName) != 0)){
		strcpy(outPass, password);
		int len  = strlen(password);
		for(int i = 0; i < len; i++){	
			outPass[i]^= ENCRYTP;		
		}
	}else
		return false;
	return true;

}
bool App::deCryptPassword(char *password, char *outPass){
	if(password && strlen(app->szUserName) != 0){
		strcpy(outPass, password);
		int len  = strlen(password);
		for(int i = 0; i < len; i++){	
			outPass[i]^= ENCRYTP;
		}
	}
	return true;
}

bool App::setConfigUser(){
	char buff[50];
	char buff2[30];
	GetWindowTextA(GetDlgItem(app->hwndMain, IDC_EDIT_USERNAME), app->szUserName, sizeof(app->szUserName));	
	GetWindowTextA(GetDlgItem(app->hwndMain, IDC_EDIT_PASSWORD), app->szPassword, sizeof(app->szPassword));		
	if(IsDlgButtonChecked(app->hwndMain, IDC_CHECK_SAVEPASS)){
		GetWindowText(GetDlgItem(app->hwndMain, IDC_EDIT_USERNAME), buff, sizeof(buff));
		GetWindowText(GetDlgItem(app->hwndMain, IDC_EDIT_PASSWORD), buff2, sizeof(buff2));
		if(strcmp(buff, "") == 0 || strcmp(buff2, "") == 0){
			MessageBox(NULL, TEXT("User Name or Password is not empty, please check again!"), NULL, 0);
			return false;
		}
		char passwordEntp[30];
		app->enCryptPassword(app->szPassword, passwordEntp);
		app->ciniFile->CIniFileWrString("User", "UserName", app->szUserName);
		app->ciniFile->CIniFileWrString("User", "Password", passwordEntp);
	}else
		app->ciniFile->CIniFileWrString("User", "Password", "ffffffff");

	app->ciniFile->CIniFileWrBoolean("Checkbox", "AutoStart", ((true == IsDlgButtonChecked(app->hwndMain, IDC_CHECK_AUTOSTART)?true:false)));
	app->ciniFile->CIniFileWrBoolean("Checkbox", "CheckAgainResultAnser", ((true == IsDlgButtonChecked(app->hwndMain, IDC_CHECK_CHECKAGAIN)?true:false)));
	app->ciniFile->CIniFileWrBoolean("Checkbox", "SavePassword", ((true == IsDlgButtonChecked(app->hwndMain, IDC_CHECK_SAVEPASS)?true:false)));
	return true;	
}
bool App::getConfigUser(){
	char *userName = app->ciniFile->CIniFileReString("User", "UserName", "");
	strcpy(app->szUserName, userName);	
	bool autoStart = app->ciniFile->CIniFileReBoolean("Checkbox", "AutoStart", false);
	bool autoAgain = app->ciniFile->CIniFileReBoolean("Checkbox", "CheckAgainResultAnser", false);
	bool savePass =  app->ciniFile->CIniFileReBoolean("Checkbox", "SavePassword", false);
	if(savePass){
		char *userPass = app->ciniFile->CIniFileReString("User", "Password", "");
		char passwordDecrypt[30];
		deCryptPassword(userPass, passwordDecrypt);
		strcpy(app->szPassword, passwordDecrypt);		
		SetWindowText(GetDlgItem(app->hwndMain, IDC_EDIT_PASSWORD), passwordDecrypt);
	}
	strcpy(app->szUserName, userName);
	SetWindowText(GetDlgItem(app->hwndMain, IDC_EDIT_USERNAME), app->szUserName);
	CheckDlgButton(app->hwndMain, IDC_CHECK_AUTOSTART, autoStart);
	CheckDlgButton(app->hwndMain, IDC_CHECK_CHECKAGAIN, autoAgain);
	CheckDlgButton(app->hwndMain, IDC_CHECK_SAVEPASS, savePass);
	return true;

}


bool createApp(HINSTANCE hInstance){
	myRegisterDlg(hInstance, CLASS_NAME, WndProc);
	hwndDlgMain = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)WndProc, 0);
	if(NULL == hwndDlgMain){
		MessageBox(NULL, TEXT("ERROR Create DlgMain"), TEXT("Error!"), 0);
		return false;
	}
	ShowWindow(hwndDlgMain, SW_NORMAL);
	UpdateWindow(hwndDlgMain);

	DWORD AppThreadId;
	HANDLE hThrd[2];
	hThrd[0] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) ThreadLoadProcess,(LPVOID)NULL,0,&AppThreadId);
	hThrd[1] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) ThreadLoopMsg,(LPVOID)NULL,0,&AppThreadId);	
	myLoopMessageDlg(hwndDlgMain);
	myUnRegister(hInstance, CLASS_NAME);
	return true;
}
bool readByteImages(char *bmpName, unsigned char* &byteData, int &img_len){
	FILE *  fpPhoto = fopen (bmpName, "rb") ; 
	if (!fpPhoto )     
		return false;
	byte  szBuf[102400] ;
	img_len= fread (szBuf, 1, sizeof (szBuf), fpPhoto);
	fclose (fpPhoto);
	byteData =  (unsigned char*)&szBuf;
	return true;	
}
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow )
{		

	app = new App();	
	app->nxDatiInit();
	createApp(hInstance);	
	app->nxDatiUnInit();
	return( FALSE );
}

//	int cX =40, cY =60, cW =300, cH =350;

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ){
	switch(msg){
	case WM_SIZE:
		break;
	case WM_CREATE:
		break;
	case MYWM_MESSAGE:
		{
//			MessageBox(NULL, TEXT("DA NHAN"), NULL, 0);
			DPrint("MYWM_MESSAGE wParam %d, lParam %d", wParam, lParam);
			for(int i =0; i< listProRunning.size(); i++){
				if((int)(wParam) == (int)listProRunning.at(i)){	
					if(-1 != (int)lParam){
						stObjMessage stObj;
						stObj.bSent = false;
						stObj.dwCodeName = (DWORD)lParam;
						listObjMessage.insert(std::make_pair((DWORD)wParam, stObj));
						break;
						//app->readFileMapping((int)lParam, (int)wParam, true);						
						//SendMessage(findHandleMain(wParam), MYWM_MESSAGE, (WPARAM)wParam, (LPARAM)777);// close Handle mapping file
					}
				}			
			}
			if(-1 == lParam){
				std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcessOld.begin();
				for(; ptr != listProcessOld.end(); ptr++){
					if(ptr->first == wParam){

						if(app->readFileMapping((int)wParam, (int)wParam, false)){
							SendMessage(app->hwndMain,MYWM_ADD_PROCESS, 0, 0);
						}
						SendMessage(findHandleMain(wParam), MYWM_MESSAGE, (WPARAM)wParam, (LPARAM)777);// close Handle mapping file

					}
					break;
				}
			}
		}break;
	case WM_NOTIFY:
		{
			switch(LOWORD(wParam))
			{
			case IDC_LIST_Process:
				{
					LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;
					if(pnm->hdr.hwndFrom == app->hwndListProcess && pnm->hdr.code == NM_CUSTOMDRAW)
					{
						static std::vector<int> listItemsSeleted;
						for(int i = 0; i< listProRunning.size(); i++){
							if(listProcessOld.count(DWORD(listProRunning.at(i))) == 0){
								listProRunning.erase(listProRunning.begin() +i);
							}
						}
						if(!listProRunning.empty()){
							for(int i =0; i< listProRunning.size(); i++){
								int j =0;
								for(std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcessOld.begin(); ptr !=listProcessOld.end(); ptr++, j++){
									if((int)listProRunning.at(i) == (int)ptr->first){
										listItemsSeleted.push_back(j);
									}
								}
							}

						}else{
							listItemsSeleted.clear();
						}

						SetWindowLong(hWnd, DWL_MSGRESULT, (LONG)ProcessCustomDraw(lParam, listItemsSeleted));
						listItemsSeleted.clear();
						return TRUE;
					}

					if(((LPNMHDR)lParam)->code == NM_CLICK){//chuot trai
						iSelect=SendMessage(app->hwndListProcess,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
						
						if(iSelect == -1){
							break;
						}
//						index = iSelect;
					}

					if(((LPNMHDR)lParam)->code == NM_DBLCLK){
						int iSelected =0;
						char Text[255]={0};  
						char Temp[255]={0};
						iSelected = SendMessage(app->hwndListProcess,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
						if(iSelected == -1)
							break;
						memset(&LvItem,0,sizeof(LvItem));
						LvItem.mask = LVIF_TEXT;
						LvItem.iSubItem = 0;
						LvItem.pszText = Text;
						LvItem.cchTextMax = 256;
						LvItem.iItem = iSelected;
						SendMessage(app->hwndListProcess, LVM_GETITEMTEXT, iSelected, (LPARAM)&LvItem);
						sprintf(Temp,Text);// processId
				//		DPrint("message : %s", Temp);
						int isValid = -1;
						for(int i =0; i< listProRunning.size(); i++){
							if(atoi(Temp) == listProRunning.at(i))
								isValid = i;								
						}
						if(-1 != isValid)
							listProRunning.erase(listProRunning.begin() + isValid);
						else
							listProRunning.push_back(atoi(Temp));
//						SendMessage(app->hwndListProcess,CCM_SETVERSION ,5,0);
						SendMessage(app->hwndListProcess, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
					}

				}break;
//
//			case IDC_RADIO1:
//				{					
////						if(pnm->hdr.hwndFrom == (GetDlgItem(app->hwndMain, IDC_RADIO1)) && pnm->hdr.code == NM_CUSTOMTEXT){
////							SetTextColor(GetDC(GetDlgItem(app->hwndMain, IDC_RADIO1)), RGB(0,0,255));
//							MessageBox(NULL, TEXT("DA VE radio"), NULL, 0);
////						}
//					
//
//				}break;
				
			}


		}break;

	case MYWM_ADD_PROCESS:
		{
			if(listProcessOld.empty()){
				if(lParam == PROCESSNAME_DELETE_ALL){
					SendMessage(app->hwndListProcess,LVM_DELETEALLITEMS,0,0);
					ShowWindow(hWnd,SW_NORMAL); 
					UpdateWindow(hWnd); 

				}
				break;
			}
			memset(&LvItem,0,sizeof(LvItem)); // Reset Coluom
			LvItem.mask = LVIF_TEXT;
			LvItem.cchTextMax = 256;

			SendMessage(app->hwndListProcess,LVM_DELETEALLITEMS,0,0);
			std::multimap<DWORD, nameProcess>::const_iterator ptr = listProcessOld.begin();			
			char Temp[255];
			for(int i = 0; ptr != listProcessOld.end(); ptr++, i++){
				LvItem.iItem=i;
				LvItem.iSubItem=0;
				sprintf(Temp,"%d", ptr->first);
				LvItem.pszText = Temp;
				SendMessage(app->hwndListProcess, LVM_INSERTITEM,0,(LPARAM)&LvItem); 

				LvItem.iSubItem= 1;
				LvItem.pszText = (LPSTR)ptr->second.namePlayer;
				SendMessage(app->hwndListProcess, LVM_SETITEM, 0, (LPARAM)&LvItem);

				LvItem.iSubItem= 2;
				LvItem.pszText = (LPSTR)ptr->second.status;
				SendMessage(app->hwndListProcess, LVM_SETITEM, 0, (LPARAM)&LvItem);

			}
			ShowWindow(hWnd,SW_NORMAL); 
			UpdateWindow(hWnd); 
		}break;
	case WM_INITDIALOG:
		{				
			app->hwndMain = hWnd;
			app->getConfigUser();
			app->hwndListProcess = GetDlgItem(hWnd, IDC_LIST_Process);
			RECT r;
			GetWindowRect(hWnd, &r);
			SetWindowPos(hWnd, HWND_TOP, r.left, r.top,(r.right - r.left)/2, r.bottom - r.top, SWP_SHOWWINDOW);

			SendMessage(app->hwndListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT); // Set style
			memset(&LvCol,0,sizeof(LvCol)); // Reset Coluom
			LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			LvCol.cx = 0x32;
			LvCol.pszText = "PID";
			SendMessage(app->hwndListProcess,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol); // Insert/Show the coloum

			LvCol.cx = 0x85;
			LvCol.pszText="Name Player";                          // Next coloum
			SendMessage(app->hwndListProcess,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol); // ...			
			LvCol.cx = 0x75;
			LvCol.pszText="Status";                          // Next coloum
			SendMessage(app->hwndListProcess,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol); // ...	

			ShowWindow(hWnd,SW_NORMAL); 
			UpdateWindow(hWnd); 

		}break;			

	case WM_VSCROLL:

		break;
	case WM_COMMAND:
		{
			switch(wParam){
			case IDC_BUTTON_RECORD:
				{
				if(!bFlagBtnRecord){
					//				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_RECORD), false);
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTON_RECORD),"<<");
					RECT r;
					GetWindowRect(hWnd, &r);
					SetWindowPos(hWnd, HWND_TOP, r.left, r.top,2*(r.right - r.left), r.bottom - r.top, SWP_SHOWWINDOW);
					UpdateWindow(hWnd);
					bFlagBtnRecord = !bFlagBtnRecord;
				}else{
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTON_RECORD),">>");
					RECT r;
					GetWindowRect(hWnd, &r);
					SetWindowPos(hWnd, HWND_TOP, r.left, r.top,(r.right - r.left)/2, r.bottom - r.top, SWP_SHOWWINDOW);
					UpdateWindow(hWnd);	
					bFlagBtnRecord = !bFlagBtnRecord;
				}
			}break;
			case IDC_BUTTON_REGISTRY:
				{	

				EnableWindow(hWnd, false);
				myRegisterDlg(GetModuleHandle(NULL), CLASS_NAME2, (WNDPROC)WndProcRegistry);
				hwndDlgRegistry = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_REGISTRY), hWnd, (DLGPROC)WndProcRegistry, 0);
				if(NULL == hwndDlgRegistry){
					MessageBox(NULL, TEXT("ERROR Create hwndDlgRegistry"), TEXT("Error!"), 0);
					return false;
				}
				ShowWindow(hwndDlgRegistry, SW_NORMAL);
				UpdateWindow(hwndDlgRegistry);
				myLoopMessageDlg(hwndDlgRegistry);
				EnableWindow(hWnd, true);
				myUnRegister(GetModuleHandle(NULL), CLASS_NAME2);

			}break;

			case IDC_BUTTON_START:
				{
					int iSelected =0;
					char Text[255]={0};  
					char Temp[255]={0};
					iSelected = SendMessage(app->hwndListProcess,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
					if(iSelected == -1)
						break;
					memset(&LvItem,0,sizeof(LvItem));
					LvItem.mask = LVIF_TEXT;
					LvItem.iSubItem = 0;
					LvItem.pszText = Text;
					LvItem.cchTextMax = 256;
					LvItem.iItem = iSelected;
					SendMessage(app->hwndListProcess, LVM_GETITEMTEXT, iSelected, (LPARAM)&LvItem);
					sprintf(Temp,Text);// processId
					int isValid = -1;
					for(int i =0; i< listProRunning.size(); i++){
						if(atoi(Temp) == listProRunning.at(i))
							isValid = i;								
					}
					if(-1 != isValid)
						listProRunning.erase(listProRunning.begin() + isValid);
					else
						listProRunning.push_back(atoi(Temp));
					NMHDR nmh;
					nmh.code = NM_CUSTOMDRAW;
					nmh.idFrom = GetDlgCtrlID(app->hwndListProcess);
					nmh.hwndFrom = app->hwndListProcess;
//					SendMessage(app->hwndListProcess,CCM_SETVERSION ,5,0);
					ListView_Update(app->hwndMain, -1);
				}break;				
			case IDC_BUTTON_STOP:
				{
					int nSelected =0;
					char Text[255]={0};  
					char Temp[255]={0};
					nSelected = SendMessage(app->hwndListProcess,LVM_GETNEXTITEM,-1,LVNI_FOCUSED);
					if(nSelected == -1){
						MessageBox(NULL, TEXT("you are not select, plase select again!"), NULL, 0);
						break;
					}
					memset(&LvItem,0,sizeof(LvItem));
					LvItem.mask = LVIF_TEXT;
					LvItem.iSubItem = 0;
					LvItem.pszText = Text;
					LvItem.cchTextMax = 256;
					LvItem.iItem = nSelected;
					SendMessage(app->hwndListProcess,LVM_GETITEMTEXT, nSelected, (LPARAM)&LvItem);
					sprintf(Temp,Text);// processId
					//						DPrint("message : %s", Temp);
					int isValid = -1;
					for(int i =0; i< listProRunning.size(); i++){
						if(atoi(Temp) == listProRunning.at(i))
							isValid = i;								
					}
					if(-1 != isValid)
						listProRunning.erase(listProRunning.begin() + isValid);
					else
						MessageBox(NULL, TEXT("You must select items running, plase select again!"), "", 0);
					UpdateWindow(NULL); 

				}break;
			case IDC_BUTTON_STOPALL:// oday
				{
					app->objInject.remoteInjectDll(1140);
				}break;
			}

		}break;
	case WM_CLOSE:
		{
			app->setConfigUser();
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return false;//DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
	
}


LRESULT CALLBACK WndProcRegistry( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ){
	switch(msg){
		case WM_INITDIALOG:
			{
				RECT r;
				GetWindowRect(GetParent(hWnd), &r);
				SetWindowPos(hWnd, HWND_TOP, r.left + (r.right - r.left), r.top, (r.right - r.left), (r.bottom - r.top), SWP_SHOWWINDOW);
				if(0 != app->nSoftwareID){
					char szMsg[256];
					sprintf_s(szMsg, sizeof(szMsg), "%d", app->nSoftwareID);
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_SOFTID), szMsg);
				}
				if(0 != strcmp(app->szUserName, "")){
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_USERNAME), app->szUserName);
				}
				if(0 != strcmp(app->szPassword, "")){
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_PASSWORD), app->szPassword);					
				}
				if(0 != strcmp(app->szEmail, "")){
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_EMAIL), app->szEmail);					
				}
				if(0 != strcmp(app->szNumQQ, "")){
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_NUMQQ), app->szNumQQ);					
				}
				if(0 != strcmp(app->szNickName, "")){
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_NICKNAME), app->szNickName);					
				}
				if(0 != strcmp(app->szPhone, "")){
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_PHONE), app->szPhone);					
				}
				UpdateWindow(hWnd);

			}break;
		case WM_COMMAND:
			{
				if(wParam == IDC_BUTTON_REGISTRY_OK){
//					MessageBox(NULL, TEXT("IDC_BUTTON_REGISTRY_OK"), NULL, 0);
					char szMsg[256];
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_SOFTID), szMsg, sizeof(szMsg));
					if((0 != strlen(szMsg)) && (!isdigit(szMsg[0]))){
						app->nSoftwareID = atoi(szMsg);
//						MessageBox(NULL, TEXT("LA SO"), NULL, 0);
					}
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_USERNAME), app->szUserName, sizeof(app->szUserName));
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_PASSWORD), app->szPassword, sizeof(app->szPassword));
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_EMAIL), app->szEmail, sizeof(app->szEmail));
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_NUMQQ), app->szNumQQ, sizeof(app->szNumQQ));
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_NICKNAME), app->szNickName, sizeof(app->szNickName));
					GetWindowText(GetDlgItem(hWnd, IDC_EDIT_PHONE), app->szPhone, sizeof(app->szPhone));					
					if(0 == strcmp(app->szUserName, "")){
						MessageBox(NULL, TEXT("User Name empty, Plase check again"), TEXT("Erro!"), 0);
						break;
					}
					if(0 == strcmp(app->szPassword, "")){
						MessageBox(NULL, TEXT("Password empty, Plase check again"), TEXT("Erro!"), 0);
						break;
					}
					if(0 == strcmp(app->szEmail, "")){
						MessageBox(NULL, TEXT("Email empty, Plase check again"), TEXT("Erro!"), 0);
						break;
					}
					if(0 == strcmp(app->szNumQQ, "")){
						MessageBox(NULL, TEXT("NumQQ empty, Plase check again"), TEXT("Erro!"), 0);
						break;
					}
					if(0 == strcmp(app->szNickName, "")){
						MessageBox(NULL, TEXT("Nick Name empty, Plase check again"), TEXT("Erro!"), 0);
						break;
					}
					if(0 == strcmp(app->szPhone, "")){
						MessageBox(NULL, TEXT("Phone empty, Plase check again"), TEXT("Erro!"), 0);
						break;
					}

					int nRet;
					nRet = app->regUserA(app->nSoftwareID, app->szUserName, app->szPassword, app->szEmail, app->szNumQQ, app->szNickName, app->szPhone, app->szResult, app->nReserve);
					if(nRet > 0)
						MessageBox(NULL, TEXT("Registry sucess"), TEXT("Registry!"), 0);
					else
						MessageBox(NULL, TEXT("Error Registry!"), TEXT("Registry!"), 0);
				}
			}break;
		case WM_CLOSE:
			{
				DestroyWindow(hWnd);
			}break;
		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}break;

		default:
			return false;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}