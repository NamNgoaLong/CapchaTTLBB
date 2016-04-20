#pragma once
#include <commctrl.h>
#include <windows.h>
#include <stdio.h>
#include <vector>

namespace functions
{

	/// registry dialog
	/**
	* @pre
	*    
	* @post
	*    
	* @param [0, 2] paramName
	*	hInstance:
	*	szClassName:
	*	wndDlgProc: win proc exute windows
	* @return
	*	
	* @throw
	*    
	*    
	*/
	static void myRegisterDlg(HINSTANCE hInstance, char* szClassName, WNDPROC wndDlgProc)
	{
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		GetClassInfoEx(hInstance, szClassName, &wc) ;
		wc.lpfnWndProc  = wndDlgProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = szClassName;
		RegisterClassEx(&wc) ;
	}

	/// unregistry dialog
	/**
	* @pre
	*    
	* @post
	*    
	* @param [0, 2] paramName
	*	hInstance:
	*	szClassName: class name registed by myRegisterDlg()
	* @return
	*	
	* @throw
	*    
	*    
	*/	
	static void myUnRegister(HINSTANCE hInstance, char* szName)
	{
		UnregisterClass( szName, hInstance ) ;
	}


	/// unregistry winodws
	/**
	* @pre
	*    
	* @post
	*    
	* @param [0, 2] paramName
	*	hInstance:
	*	szClassName:
	*	wndProc: winproc excute message windows
	* @return
	*	
	* @throw
	*    
	*    
	*/	
	static void myRegisterWindow(HINSTANCE hInstance, char* szClassName, WNDPROC wndProc)
	{
		WNDCLASS wc = {0};
		wc.lpszClassName = szClassName;
		wc.hInstance = hInstance;
		wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
		wc.lpfnWndProc = wndProc;
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		RegisterClass(&wc);

	}

	/// unregistry windows
	/**
	* @pre
	*    
	* @post
	*    
	* @param [0, 2] paramName
	*	hInstance:
	*	szClassName: class name registed by myRegisterWindow()
	* @return
	*	
	* @throw
	*    
	*    
	*/
	static void myUnRegisterWindow(HINSTANCE hInstance, char* szName)
	{
		UnregisterClass( szName, hInstance ) ;
	} 



	/// loop get message Dialog
	/**
	* @pre
	*    
	* @post
	*    
	* @param [0, 1] paramName
	*	hWnd: HANDLE to get message
	* @return
	*	
	* @throw
	*    
	*    
	*/
	void myLoopMessageDlg(HWND hWnd)
	{
		MSG msg;
		BOOL bRet = FALSE ;
		while ( bRet = GetMessage(&msg, NULL, 0, 0) )
		{ 
			if ( -1 == bRet )
				continue ;

			if (!IsDialogMessage(hWnd, &msg)) 
			{ 
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			} 
		}
	}


	/// loop get message windows
	/**
	* @pre
	*    
	* @post
	*    
	* @param [0, 1] paramName
	*	hWnd: HANDLE to get message
	* @return
	*	
	* @throw
	*    
	*    
	*/
	void myLoopMessageWindow(HWND hWnd)
	{
		MSG msg;
		BOOL bRet = FALSE ;
		while ( bRet = GetMessage(&msg, NULL, 0, 0) )
		{ 
			if ( -1 == bRet )
				continue ;
			if (!IsDialogMessage(hWnd, &msg)) 
			{ 
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			} 
		}		
	}
	/// set font to windows
	void mySetFont(HWND hWnd, char* szFontName, int nSize)
	{
		LOGFONT lfont;
		memset(&lfont, 0, sizeof(lfont));
		lstrcpy(lfont.lfFaceName, szFontName);
		lfont.lfHeight = nSize;
		lfont.lfWeight = FW_NORMAL;//FW_BOLD;
		lfont.lfItalic = FALSE;
		lfont.lfCharSet = DEFAULT_CHARSET;
		lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lfont.lfQuality = DEFAULT_QUALITY;
		lfont.lfPitchAndFamily = DEFAULT_PITCH;
		SendMessage(hWnd, WM_SETFONT,(WPARAM)CreateFontIndirect(&lfont), 0);	
	}

	//------------------------------- FIND HWND MAIN WINDOWS FROM PROCESS ID--------------------------

	/// check handle is GW_OWNER windows in process
	BOOL isMainWindow(HWND handle)
	{
		return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
	}

	/// find main windows by process id
	// @return: HWND of windows
	HWND findHandleMain(DWORD proceesID)
	{
		//			DWORD processID = 0;
		HWND hWndF = 0;
		do{
			hWndF = FindWindowEx(NULL, hWndF, NULL, NULL );
			//				hWndF = FindWindowEx(0, 0, "TianLongBaBu WndClass", 0);
			DWORD dwPID = 0;
			GetWindowThreadProcessId(hWndF, &dwPID);
			if(dwPID == proceesID){
				if(isMainWindow(hWndF))
					//						DPrint("Process ID(%d) : %d , HWND: %x ", ++i, dwPID, hWndF);
					return hWndF;
			}
		}while(hWndF != NULL);
		return 0;
	}
	/// draw items in liview
	LRESULT ProcessCustomDraw (LPARAM lParam, std::vector<int> &vtItems)
	{	
		LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

		//	lplvcd->clrText
		switch(lplvcd->nmcd.dwDrawStage) 
		{
		case CDDS_PREPAINT : //Before the paint cycle begins
			//request notifications for individual listview items
			return CDRF_NOTIFYITEMDRAW;

		case CDDS_ITEMPREPAINT: //Before an item is drawn
			{
				return CDRF_NOTIFYSUBITEMDRAW;
			}
			break;

			//        case CDDS_SUBITEM | CDDS_ITEMPREPAINT: //Before a subitem is drawn
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT: //Before a subitem is drawn
			{
				if(!vtItems.empty())
				{
					for(int i = 0; i < (int)vtItems.size(); i++)
					{
						if(lplvcd->nmcd.dwItemSpec == int(vtItems.at(i)))
						{
							lplvcd->clrText   = RGB(255,255,255);
							lplvcd->clrTextBk = RGB(240,55,23);
							return CDRF_NEWFONT;					
						}
					}
				}
				//					return CDRF_DODEFAULT;	COLOR_WINDOW+1				

				/*
				if(lplvcd->nmcd.dwItemSpec == sel){
				lplvcd->clrText   = RGB(255,255,255);
				lplvcd->clrTextBk = RGB(240,55,23);
				return CDRF_NEWFONT;
				}
				break;
				*/
				/*
				if(sel == 1){
				switch(lplvcd->iSubItem)
				{
				case 0:
				{
				lplvcd->clrText   = RGB(255,255,255);
				lplvcd->clrTextBk = RGB(240,55,23);
				return CDRF_NEWFONT;
				}
				break;
				}

				}else{

				switch(lplvcd->iSubItem)
				{
				case 2:
				{
				lplvcd->clrText   = RGB(255,255,255);
				lplvcd->clrTextBk = RGB(144,55,144);
				return CDRF_NEWFONT;
				}
				break;
				}

				}


				switch(lplvcd->iSubItem)
				{
				case sel:
				{
				lplvcd->clrText   = RGB(255,255,255);
				lplvcd->clrTextBk = RGB(240,55,23);
				return CDRF_NEWFONT;
				}
				break;

				case 1:
				{
				lplvcd->clrText   = RGB(255,255,0);
				lplvcd->clrTextBk = RGB(0,0,0);
				return CDRF_NEWFONT;
				}
				break;  

				case 2:
				{
				lplvcd->clrText   = RGB(20,26,158);
				lplvcd->clrTextBk = RGB(200,200,10);
				return CDRF_NEWFONT;
				}
				break;

				case 3:
				{
				lplvcd->clrText   = RGB(12,15,46);
				lplvcd->clrTextBk = RGB(200,200,200);
				return CDRF_NEWFONT;
				}
				break;

				case 4:
				{
				lplvcd->clrText   = RGB(120,0,128);
				lplvcd->clrTextBk = RGB(20,200,200);
				return CDRF_NEWFONT;
				}
				break;

				case 5:
				{
				lplvcd->clrText   = RGB(255,255,255);
				lplvcd->clrTextBk = RGB(0,0,150);
				return CDRF_NEWFONT;
				}
				break;

				}

				}

				case CDDS_ITEM:
				{
				lplvcd->clrText   = RGB(120,0,128);
				lplvcd->clrTextBk = RGB(20,200,200);
				return CDRF_NEWFONT;				  

				}break;
				*/	
			}
		}
		return CDRF_DODEFAULT;
	}

	void DPrint(char *format, ...)
	{
		va_list arg_ptr;
		char outstr[0x1000];
		va_start(arg_ptr, format);
		vsprintf(outstr, format, arg_ptr);
		OutputDebugStringA(outstr);
		va_end(arg_ptr);
	}
}