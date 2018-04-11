///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © NetworkDLS 2002, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CTabControl_H
#define _CTabControl_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INIT_HANDLE_TABBING (WM_USER + 1000)

typedef INT_PTR (CALLBACK* TabControlDlgProc)(HWND, UINT, WPARAM, LPARAM);

class CTabControl  
{
public: //Constructor / Deconstructor
	CTabControl(HWND hTab, char **sTabNames, char **sDlgNames, TabControlDlgProc pParentProc, bool fStretch);
	~CTabControl();

public: //Public functions:
	bool CenterTabPage(int iPage);
	bool StretchTabPage(int iPage);
	bool SetControlFocus(HWND hWnd);
	bool Notify(LPNMHDR pnm);
	void ProcessParentMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND Handle(char *sGivenName);
	HWND Handle();
	int TabPageCount(void);
	HWND VisibleTabPage(void);
	char **TabPageNames(void);
	HWND *TabPageHandles(void);

private: //Settings:
	HWND _hTabControl;
	HWND _hVisiblePage;
	HWND* _hTabPages;
	char **_sTabNames;
	int _iTabPageCount;
	bool _bStretchTabs;
	bool _bFirstPaint;
	TabControlDlgProc _ParentProc;

private: //Internal functions:
	VOID TabControl_GetClientRect(HWND hwnd, RECT * prc);
	VOID FirstTabstop_SetFocus(HWND hWnd);
	void SetupKeyHandlers(HWND hTabPage);

private: //Events:
	bool OnSelectionChanged(VOID);

	friend LRESULT CALLBACK TabPage_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK TabPage_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TabCtrl_SelectTab(hTab,iSel) { \
	TabCtrl_SetCurSel(hTab,iSel); \
	NMHDR nmh = { hTab, GetDlgCtrlID(hTab), TCN_SELCHANGE }; \
	SendMessage(nmh.hwndFrom,WM_NOTIFY,(WPARAM)nmh.idFrom,(LPARAM)&nmh); }

#define HANDLE_DLGMSG(hwnd, message, fn) \
   case (message): return (SetDlgMsgResult(hwnd, msg,HANDLE_##message((hwnd), (wParam), (lParam), (fn))))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
