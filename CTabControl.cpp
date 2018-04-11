///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © NetworkDLS 2002, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CTabControl_Cpp
#define _CTabControl_Cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#include "CTabControl.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Ensure focus on first tab stop when entering a tab page.

VOID CTabControl::FirstTabstop_SetFocus(HWND hWnd)
{
	HWND hNextTabStop = GetNextDlgTabItem(hWnd, NULL, FALSE);
	if(hNextTabStop)
	{
		SendMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)hNextTabStop, 1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CTabControl::SetControlFocus(HWND hWnd)
{
	HWND hParent = GetParent(hWnd);

	while(hParent)
	{
		for(int i = 0; i < this->_iTabPageCount; i++)
		{
			if(this->_hTabPages[i] == hParent)
			{
				TabCtrl_SetCurSel(this->_hTabControl, i);
				this->OnSelectionChanged();
				SetFocus(hWnd);
				return true;
			}
		}
		hParent = GetParent(hParent);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Return a Client rectangle for the tab control under

VOID CTabControl::TabControl_GetClientRect(HWND hwnd, RECT * prc)
{
	RECT rtab_0;
	LONG_PTR lStyle = GetWindowLongPtr(hwnd, GWL_STYLE);

	// Calculate the tab control's display area
	GetWindowRect(hwnd, prc);
	ScreenToClient(GetParent(hwnd), (POINT *) & prc->left);
	ScreenToClient(hwnd, (POINT *) & prc->right);
	TabCtrl_GetItemRect(hwnd, 0, &rtab_0);	//The tab itself

	if ((lStyle & TCS_BOTTOM) && (lStyle & TCS_VERTICAL))	//Tabs to Right
	{
		prc->top = prc->top + 6;	//x coord
		prc->left = prc->left + 4;	//y coord
		prc->bottom = prc->bottom - 12;	// height
		prc->right = prc->right - (12 + rtab_0.right - rtab_0.left);	// width
	}
	else if (lStyle & TCS_VERTICAL)	//Tabs to Left
	{
		prc->top = prc->top + 6;	//x coord
		prc->left = prc->left + (4 + rtab_0.right - rtab_0.left);	//y coord
		prc->bottom = prc->bottom - 12;	// height
		prc->right = prc->right - (12 + rtab_0.right - rtab_0.left);	// width
	}
	else if (lStyle & TCS_BOTTOM)	//Tabs on Bottom
	{
		prc->top = prc->top + 6;	//x coord
		prc->left = prc->left + 4;	//y coord
		prc->bottom = prc->bottom - (16 + rtab_0.bottom - rtab_0.top);	// height
		prc->right = prc->right - 12;	// width
	}
	else	//Tabs on top
	{
		prc->top = prc->top + (6 + rtab_0.bottom - rtab_0.top);	//x coord
		prc->left = prc->left + 4;	//y coord
		prc->bottom = prc->bottom - (16 + rtab_0.bottom - rtab_0.top);	// height
		prc->right = prc->right - 12;	// width
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Center the tab page in the tab control's display area.

bool CTabControl::CenterTabPage(int iPage)
{
	RECT rect, rclient;

	TabControl_GetClientRect(this->_hTabControl, &rect);	// left, top, width, height

	// Get the tab page size
	GetClientRect(this->_hTabPages[iPage], &rclient);
	rclient.right = rclient.right - rclient.left;	// width
	rclient.bottom = rclient.bottom - rclient.top;	// height
	rclient.left = rect.left;
	rclient.top = rect.top;

	// Center the tab page, or cut it off at the edge of the tab control(bad)
	if (rclient.right < rect.right)
		rclient.left += (rect.right - rclient.right) / 2;

	if (rclient.bottom < rect.bottom)
		rclient.top += (rect.bottom - rclient.bottom) / 2;

	// Move the child and put it on top
	return (SetWindowPos(this->_hTabPages[iPage], HWND_TOP, rclient.left, rclient.top, rclient.right, rclient.bottom, 0) != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CTabControl::TabPageCount(void)
{
	return this->_iTabPageCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND CTabControl::VisibleTabPage(void)
{
	return this->_hVisiblePage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char **CTabControl::TabPageNames(void)
{
	return this->_sTabNames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND *CTabControl::TabPageHandles(void)
{
	return this->_hTabPages;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Returns the handle (hWnd) of a tab page by its given name.

HWND CTabControl::Handle()
{
	return this->_hTabControl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Returns the handle (hWnd) of a tab page by its given name.

HWND CTabControl::Handle(char *sGivenName)
{
	for(int i = 0; i < this->_iTabPageCount; i++)
	{
		if(strcmp(this->_sTabNames[i], sGivenName) == 0)
		{
			return this->_hTabPages[i];
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Stretch the tab page to fit the tab control's display area.

bool CTabControl::StretchTabPage(int iPage)
{
	RECT rect;

	TabControl_GetClientRect(this->_hTabControl, &rect);	// left, top, width, height

	// Move the child and put it on top
	return (SetWindowPos(this->_hTabPages[iPage], HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, 0) != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK HandleTabbingProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC lpOriginalhandler = (WNDPROC)GetProp(hWnd, "Originalhandler");
	HWND hOwner = (HWND)GetProp(hWnd, "TabPageHandle");
	CTabControl *pTabControl = (CTabControl *)GetProp(hWnd, "CTabControl");

	if(!lpOriginalhandler)
	{
		return 0;
	}

    if(uMsg == WM_GETDLGCODE)
	{
		if(lpOriginalhandler)
		{
			return (DLGC_WANTALLKEYS | CallWindowProc(lpOriginalhandler, hWnd, uMsg, wParam, lParam));
		}
	}
	else if(uMsg == WM_KEYDOWN)
	{
		if(wParam == VK_TAB)
		{
			HWND hNextTabStop = GetNextDlgTabItem(hOwner, hWnd, FALSE);
			if(hNextTabStop)
			{
				SendMessage(hOwner, WM_NEXTDLGCTL, (WPARAM)hNextTabStop, 1);
				return 0;
			}
		}
	}
	else if(uMsg == WM_CHAR)
	{
		if(wParam == VK_RETURN)
		{
			SendMessage(GetParent(pTabControl->Handle()), WM_COMMAND, IDOK, 0);
			return 0; //Supress beep sound.
		}
		else if(wParam == VK_ESCAPE)
		{
			SendMessage(GetParent(pTabControl->Handle()), WM_COMMAND, IDCANCEL, 0);
			return 0; //Supress beep sound.
		}
		else if(wParam == VK_TAB)
		{
			return 0; //Supress beep sound.
		}
	}
	else if(uMsg == WM_DESTROY)
	{
		RemoveProp(hWnd, "Originalhandler");
		RemoveProp(hWnd, "TabPageHandle");
		RemoveProp(hWnd, "CTabControl");
	}

	if(lpOriginalhandler)
	{
		return CallWindowProc(lpOriginalhandler, hWnd, uMsg, wParam, lParam);
	}
	else{
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Window Procedure for the tab pages (dialogs).

LRESULT CALLBACK TabPage_DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_INITDIALOG)
	{
		// We handle this message so that it is not sent to the
		//	main dlg proc each time a tab page is instanciated.
		return(DefWindowProc(hWnd, WM_INITDIALOG, wParam, lParam) != 0); 
	}

	HWND hTab = (HWND)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if(!hTab)
	{
		return FALSE;
	}

	CTabControl *pTabControl = (CTabControl *)GetWindowLongPtr(hTab, GWLP_USERDATA);
    //--------------------------------------------------------------------------
	if(uMsg == WM_PAINT)
	{
		//If this is the first painting, then set the focus.
		if(pTabControl->_bFirstPaint)
		{
			pTabControl->FirstTabstop_SetFocus(hWnd);
			pTabControl->_bFirstPaint = false;
		}
	}
    //--------------------------------------------------------------------------
    else if(uMsg == WM_CLOSE) //- Received close message.
    {
		return 0; //Supress tab closing.
    }
    //--------------------------------------------------------------------------

	return pTabControl->_ParentProc(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	This function must be called by the owner window.
*/
void CTabControl::ProcessParentMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //--------------------------------------------------------------------------
	if(uMsg == WM_DRAWITEM)
	{
		//If the tabs are owner draw, then we need to fill them in.
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
		char sText[255];

		TCITEM tci;
		memset(&tci, 0, sizeof(tci));
		tci.mask = TCIF_TEXT;
		tci.pszText = sText;
		tci.cchTextMax = sizeof(sText)-1;
		TabCtrl_GetItem(lpdis->hwndItem, lpdis->itemID, &tci);
		TextOut(lpdis->hDC,lpdis->rcItem.left + 5, lpdis->rcItem.top + 3, tci.pszText, lstrlen(tci.pszText));
	}
	//--------------------------------------------------------------------------
	else if(uMsg == WM_NOTIFY)
    {
        LPNMHDR pHdr = (LPNMHDR)lParam;
		if(pHdr->hwndFrom == this->_hTabControl)
		{
			if(pHdr->code == TCN_SELCHANGE)
			{
				this->OnSelectionChanged();
			}
		}
	}
	//--------------------------------------------------------------------------
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- A tab has been pressed, Handle TCN_SELCHANGE notification.

bool CTabControl::OnSelectionChanged(VOID)
{
	int iSel = TabCtrl_GetCurSel(this->_hTabControl);

	//Hide the current child dialog box, if any.
	ShowWindow(this->_hVisiblePage, FALSE);

	//ShowWindow() does not seem to post the WM_SHOWWINDOW message
	// to the tab page.  Since I use the hiding of the window as an
	// indication to stop the message loop I post it explicitly.
	//PostMessage() is necessary in the event that the Loop was started
	// in response to a mouse click.
	FORWARD_WM_SHOWWINDOW(this->_hVisiblePage, FALSE, 0, PostMessage);

	//Show the new child dialog box.
	ShowWindow(this->_hTabPages[iSel], TRUE);

	// Save the current child
	this->_hVisiblePage = this->_hTabPages[iSel];

	// The the focus to the first (tab stop) item in the dialog.
	this->FirstTabstop_SetFocus(this->_hVisiblePage);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Destroy the tab page dialogs and free the list of pointers to the dialogs.

CTabControl::~CTabControl()
{
	for (int i = 0; i < this->_iTabPageCount; i++)
	{
		DestroyWindow(this->_hTabPages[i]);
	}

	free(this->_hTabPages);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTabControl::SetupKeyHandlers(HWND hTabPage)
{
	HWND hTabStop = NULL;
	HWND hFirstStop = NULL;

	//Add tabbing handlers to handle user "tab key press" events.
	while((hTabStop = GetNextDlgTabItem(hTabPage, hTabStop, FALSE)))
	{
		if(hTabStop == hFirstStop)
		{
			break;
		}

		WNDPROC gpOldProc = (WNDPROC) SetWindowLongPtr(hTabStop, GWLP_WNDPROC, (LONG_PTR) &HandleTabbingProc);

		SetProp(hTabStop, "Originalhandler", gpOldProc);
		SetProp(hTabStop, "TabPageHandle", hTabPage);
		SetProp(hTabStop, "CTabControl", this);

		if(!hFirstStop)
		{
			hFirstStop = hTabStop;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-- Initialize a TABCTRL struct, create tab page dialogs and start the message loop.

CTabControl::CTabControl(HWND hTab, char **sTabNames, char **sDlgNames, TabControlDlgProc pParentProc, bool fStretch)
{
	this->_hTabControl = hTab;
	this->_sTabNames = sTabNames;
	this->_bStretchTabs = fStretch;
	this->_bFirstPaint = true;

	//Link struct this pointer to hTab
	SetWindowLongPtr(hTab, GWLP_USERDATA, (LONG_PTR) this);

	//We set the style to owner draw, so that we can prevent the tab pages from being white.
	//	This is unfortunate because it also means that we have to draw the tab text.
	LONG_PTR lpCurrentStyle = GetWindowLongPtr(hTab, GWL_STYLE);
	if(!(lpCurrentStyle &TCS_OWNERDRAWFIXED))
	{
		SetWindowLongPtr(hTab, GWL_STYLE, lpCurrentStyle | TCS_OWNERDRAWFIXED);
	}

	// Point to external functions
	this->_ParentProc = pParentProc;

	// Determine number of tab pages to insert based on DlgNames
	this->_iTabPageCount = 0;
	char **ptr = sDlgNames;
	while (*ptr++)
	{
		this->_iTabPageCount++;
	}

	//create array based on number of pages
	this->_hTabPages = (HWND*)malloc(this->_iTabPageCount * sizeof(HWND*));

	TCITEM tie;
	memset(&tie, 0, sizeof(tie));
	// Add a tab for each name in tabnames (list ends with 0)
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	for (int i = 0; i < this->_iTabPageCount; i++)
	{
		tie.pszText = this->_sTabNames[i];
		TabCtrl_InsertItem(this->_hTabControl, i, &tie);

		// Add page to each tab
		this->_hTabPages[i] = CreateDialog(GetModuleHandle(NULL), sDlgNames[i], GetParent(this->_hTabControl), (DLGPROC)TabPage_DlgProc);

		this->SetupKeyHandlers(this->_hTabPages[i]);

		// Since the hTab is not the paren link it to this page 
		SetWindowLongPtr(this->_hTabPages[i], GWLP_USERDATA, (LONG_PTR)this->_hTabControl);

		// Set initial tab page position
		if (this->_bStretchTabs)
		{
			this->StretchTabPage(i);
		}
		else {
			this->CenterTabPage(i);
		}
	}

	// Save the current child
	this->_hVisiblePage = this->_hTabPages[0];

	// Show first tab
	ShowWindow(this->_hVisiblePage, SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
