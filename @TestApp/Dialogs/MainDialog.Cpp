///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright © NetworkDLS 2002, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _MainDialog_Cpp
#define _MainDialog_Cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <ShlObj.H>
#include <Stdio.H>
#include <ShlOBJ.H>
#include <Stdlib.H>

#include "../../CTabControl.h"

#include "../Source/Entry.H"
#include "../Dialogs/MainDialog.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace NSWFL::Windows;
using namespace NSWFL::String;
using namespace NSWFL::Conversion;
using namespace NSWFL::ListView;
using namespace NSWFL::File;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK MainDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CTabControl *TabControl;

	static HWND hTabBasic = NULL;
	static HWND hTabIPBindings = NULL;
	static HWND hTabRouting = NULL;
	static HWND hTabHTTPHeaders = NULL;
	static HWND hTabAdvanced = NULL;

	static HWND hName = NULL;
	static HWND hDescr = NULL;

    //--------------------------------------------------------------------------

    if(uMsg == WM_INITDIALOG)
    {
		SendMessage(hWnd, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)"Route Settings");

		static LPSTR tabNames[]= {
			"Basic",
			"IP Bindings",
			"Routing",
			"HTTP Headers",
			"Advanced",
			NULL
		};

		static LPSTR dlgNames[] = {
			MAKEINTRESOURCE(IDD_ROUTE_BASIC),
			MAKEINTRESOURCE(IDD_ROUTE_IPBINDINGS),
			MAKEINTRESOURCE(IDD_ROUTE_ROUTINGS),
			MAKEINTRESOURCE(IDD_ROUTE_HEADERS),
			MAKEINTRESOURCE(IDD_ROUTE_ADVANCED),
			NULL
		};

		TabControl = new CTabControl(GetDlgItem(hWnd, IDC_TABCONTROL), tabNames, dlgNames, &MainDialog, TRUE);

		hTabBasic			= TabControl->Handle("Basic");
		hName				= GetDlgItem(hTabBasic, IDC_NAME);
		hDescr				= GetDlgItem(hTabBasic, IDC_DESCR);

		/*
		hTabIPBindings		= TabControl->Handle("IP Bindings");
		ghIPAddressList		= GetDlgItem(hTabIPBindings, IDC_IPADDRESSLIST);

		hTabRouting			= TabControl->Handle("Routing");
		ghHostsAddressList	= GetDlgItem(hTabRouting, IDC_HOST_LIST);
		hLocalPort			= GetDlgItem(hTabRouting, IDC_LOCALPORT);
		hMaxConnections		= GetDlgItem(hTabRouting, IDC_MAXCONNECTIONS);
		hLoadBalanceMethod	= GetDlgItem(hTabRouting, IDC_HOST_BALANCE);

		hTabHTTPHeaders		= TabControl->Handle("HTTP Headers");
		ghHTTPHeaderList	= GetDlgItem(hTabHTTPHeaders, IDC_HEADER_LIST);

		hTabAdvanced		= TabControl->Handle("Advanced");
		hBufferSize			= GetDlgItem(hTabAdvanced, IDC_BUFFERSIZE);
		hIdleTimeout		= GetDlgItem(hTabAdvanced, IDC_IDLETIMEOUT);
		hOrphanTimeout		= GetDlgItem(hTabAdvanced, IDC_ORPHANTIMEOUT);
		hPersistenceTime	= GetDlgItem(hTabAdvanced, IDC_PERSISTENCETIME);
		*/

		CenterOverOwner(hWnd);

        return TRUE; // Return TRUE to set the keyboard focus, Otherwise return FALSE.
    }
	//--------------------------------------------------------------------------
    else if(uMsg == WM_COMMAND)
    {
		if(wParam == IDC_OK || wParam == IDOK)
		{
			delete TabControl;
			return EndDialog(hWnd, 1);
		}
		else if(wParam == IDC_CANCEL || wParam == IDCANCEL)
		{
			delete TabControl;
			return EndDialog(hWnd, 0);
		}

		return FALSE;
    }
	//--------------------------------------------------------------------------
	else if(uMsg == WM_PAINT)
    {
		HDC ThisHDC;
        PAINTSTRUCT ThisPS;

        ThisHDC = BeginPaint(hWnd, &ThisPS);

        //- Any painting should be done here.

        EndPaint(hWnd, &ThisPS);
        return TRUE;
    }
    //--------------------------------------------------------------------------
    else if(uMsg == WM_CLOSE) //- Received close message.
    {
		delete TabControl;
		return EndDialog(hWnd, 0);
    }
    //--------------------------------------------------------------------------

	if(TabControl)
	{
		TabControl->ProcessParentMessages(hWnd, uMsg, wParam, lParam);
	}

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
