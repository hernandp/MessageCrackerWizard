/*****************************************************************

MIT License

This file is part of Message Cracker Wizard

Copyright (c) 2003, 2017, 2018 Hernán Di Pietro

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***************************************************************************/

#include "stdafx.h"
#include "msgcrackwizard.h"
#include "resource.h"

HBITMAP		hbmpKeyboard, hbmpMouse, hbmpClipboard;
HBITMAP		hbmpGeneral, hbmpNonClient, hbmpMDI, hbmpWindow;

//
// Set / Unset a window style to a control by Id
//
static void SetWindowStyle(HWND hwndParent, UINT childId, DWORD dwStyle)
{
    HWND hWnd = GetDlgItem(hwndParent, childId);
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | dwStyle);
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

static void RemoveWindowStyle(HWND hwndParent, UINT childId, DWORD dwStyle)
{
    HWND hWnd = GetDlgItem(hwndParent, childId);
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~dwStyle);
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

//
// fill the list box with the messages (and apply filters)
//
void FillListBox(HWND hwnd, UINT filter)
{
	int itemdx;

	// first clear all items if any
	SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_RESETCONTENT, 0, 0);

	// add messages to listbox
	for (const auto & i : msginfo)
	{
		// check for filter
		if (i.messageType & g_fFilter)
		{
			itemdx = (int) SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_ADDSTRING, 0, (LPARAM) i.szMessageName);
			// add a bitmap index depending on type
			SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_SETITEMDATA, (WPARAM) itemdx, (LPARAM) i.messageType);
		}
	}
}

// 
// configure controls
//
void SetupControls(HWND hwnd)
{
	SendDlgItemMessage(hwnd, IDC_WINDOWID, EM_LIMITTEXT, MAX_WINDOWID_STR_LEN - 1, 0);

	FillListBox(hwnd, g_fFilter);

	// put fonts
	SendDlgItemMessage(hwnd, IDC_HANDLEMSG, WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), 0);
	SendDlgItemMessage(hwnd, IDC_MSGFUNCTION, WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), 0);

	// disable necessary controls when there is no selection on list
	EnableControls(hwnd, FALSE);
	SendDlgItemMessage(hwnd, IDC_RADWINDOW, BM_SETCHECK, BST_CHECKED, 0);

    HMENU hMenu = GetMenu(hwnd);
    CheckMenuItem(hMenu, ID_VIEW_DARKCOLORSCHEME, g_mcwConfig.bDarkMode ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(hMenu, ID_VIEW_STAYONTOP, g_mcwConfig.bStayOnTop ? MF_CHECKED : MF_UNCHECKED);

   /* if (g_mcwConfig.bDarkMode)
    {
        MENUINFO mi = { 0 };
        mi.cbSize = sizeof(mi);
        mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
        mi.hbrBack = g_darkModeRes.hbrBackground;

        SetMenuInfo(hMenu, &mi);
    }
    else
    {

    }*/

    SetWindowPos(hwnd, g_mcwConfig.bStayOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    switch (g_mcwConfig.windowAlpha)
    {
    case ALPHA_SOLID:
        CheckMenuItem(hMenu, ID_WINDOWTRANSPARENCY_SOLID, MF_CHECKED);
        break;
    case ALPHA_TRANS_10:
        CheckMenuItem(hMenu, ID_WINDOWTRANSPARENCY_10, MF_CHECKED);
        break;
    case ALPHA_TRANS_25:
        CheckMenuItem(hMenu, ID_WINDOWTRANSPARENCY_25, MF_CHECKED);
        break;
    case ALPHA_TRANS_50:
        CheckMenuItem(hMenu, ID_WINDOWTRANSPARENCY_50, MF_CHECKED);
        break;
    case ALPHA_TRANS_75:
        CheckMenuItem(hMenu, ID_WINDOWTRANSPARENCY_75, MF_CHECKED);
        break;
    }

    if (g_mcwConfig.bDarkMode)
        EnableOwnerdrawnControls(hwnd);
    else
        DisableOwnerdrawnControls(hwnd);
    
    auto hUser32 = LoadLibrary(L"user32.dll");
    if (hUser32)
    {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

        PSLWA pSetLayeredWndAttr = (PSLWA)GetProcAddress(hUser32, "SetLayeredWindowAttributes");
        pSetLayeredWndAttr(hwnd, 0, g_mcwConfig.windowAlpha, LWA_ALPHA);
        FreeLibrary(hUser32);
    }

}

//
// Updates the program interface
//
void UpdateUI(HWND hwnd)
{
	std::wstring genMacro, genFunction, functionName, parameters, returnType;

	// locate the selected messages

	int selItems[NUMMSG] = { 0 };
	int nItems = (int) SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETSELITEMS, NUMMSG, (LPARAM) &selItems);

	// update number of selected messages static box

	wchar_t szStatic[255];
	StringCchPrintf(szStatic, 255, L"%d message(s) selected.", nItems);
	SetDlgItemText(hwnd, IDC_NUMSELECT, szStatic);

	if (nItems > 0)
	{
		// update interface with all selected messages

		for (int index = 0; index < nItems; index++)
		{
			int textLen = SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETTEXTLEN, selItems[index], NULL);
			auto szMessage = std::make_unique<wchar_t[]>(textLen + 1);
			SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETTEXT, selItems[index], (LPARAM) szMessage.get());

			// construct HANDLE_MSG macro

			if ((IsDlgButtonChecked(hwnd, IDC_MAKEWNDPROC) == BST_CHECKED) &&
				(IsDlgButtonChecked(hwnd, IDC_DIALOG) == BST_CHECKED))
			{
				genMacro += L"HANDLE_MSG (hwndDlg, ";
			}
			else
			{
				genMacro += L"HANDLE_MSG (hwnd, ";
			}

			genMacro += szMessage.get();

			// search for message parameters in the structure
			for (const auto & j : msginfo)
			{
				if (!wcscmp(j.szMessageName, szMessage.get()))
				{
					functionName = j.szFunctionName;
					parameters = j.szParameters;
					returnType = j.szReturnType;
				}
			}

			std::wstring suffix;
			int windowIdTextLen = GetWindowTextLength(GetDlgItem(hwnd, IDC_WINDOWID));

			if (windowIdTextLen)
			{
				auto windowIdText = std::make_unique<wchar_t[]>(windowIdTextLen + 1);
				GetDlgItemText(hwnd, IDC_WINDOWID, windowIdText.get(), windowIdTextLen + 1);
				suffix = windowIdText.get();
			}

			// construct both complete macro and functions

			genMacro += L", " + suffix + L"_" + functionName + L");\r\n";
			genFunction += returnType + L" " + suffix + L"_" + functionName + parameters + L"\r\n";
		}
	}

	// put text to edit boxes
	SetDlgItemText(hwnd, IDC_HANDLEMSG, genMacro.c_str());
	SetDlgItemText(hwnd, IDC_MSGFUNCTION, genFunction.c_str());
}

// disable some controls on the dialog
void EnableControls(HWND hwnd, BOOL fEnabled)
{
	EnableWindow(GetDlgItem(hwnd, IDC_WINDOWID), fEnabled);
	EnableWindow(GetDlgItem(hwnd, IDC_MAKEWNDPROC), fEnabled);
	EnableWindow(GetDlgItem(hwnd, IDC_DIALOG), fEnabled);
	EnableWindow(GetDlgItem(hwnd, IDC_RADWINDOW), fEnabled);
	EnableWindow(GetDlgItem(hwnd, IDC_COPYMACRO), fEnabled);
	EnableWindow(GetDlgItem(hwnd, IDC_COPYFUNC), fEnabled);
}


void EnableOwnerdrawnControls(HWND hwnd)
{
    SetWindowStyle(hwnd, IDC_COPYFUNC, BS_OWNERDRAW);
    SetWindowStyle(hwnd, IDC_COPYMACRO, BS_OWNERDRAW);
    SetWindowStyle(hwnd, IDC_CLOSE, BS_OWNERDRAW);
    SetWindowStyle(hwnd, IDC_FILTERBTN, BS_OWNERDRAW);
}

void DisableOwnerdrawnControls(HWND hwnd)
{
    RemoveWindowStyle(hwnd, IDC_COPYFUNC, BS_OWNERDRAW);
    RemoveWindowStyle(hwnd, IDC_COPYMACRO, BS_OWNERDRAW);
    RemoveWindowStyle(hwnd, IDC_CLOSE, BS_OWNERDRAW);
    RemoveWindowStyle(hwnd, IDC_FILTERBTN, BS_OWNERDRAW);
}

//
// select bitmap for ownerdrawn control based on msginfo[].fType
//
HBITMAP SelectItemBitmap(const DRAWITEMSTRUCT* lpDrawItem)
{
	HBITMAP hPic = NULL;

	// select bitmap for listbox
	if (lpDrawItem->CtlID == IDC_MESSAGES)
	{
		switch (lpDrawItem->itemData)
		{
			case KEYBOARD:
				hPic = hbmpKeyboard;
				break;

			case MOUSE:
				hPic = hbmpMouse;
				break;

			case CLIPBOARD:
				hPic = hbmpClipboard;
				break;

			case GENERAL:
				hPic = hbmpGeneral;
				break;

			case MDI:
				hPic = hbmpMDI;
				break;

			case NONCLIENT:
				hPic = hbmpNonClient;
				break;
		}
	}

	// select bitmap for WindowID combobox
	if (lpDrawItem->CtlID == IDC_WINDOWID)
		hPic = hbmpWindow;

	return hPic;
}

// draw the ownerdrawn bitmap
//
void DrawItemBitmap(const DRAWITEMSTRUCT* lpDrawItem, HBITMAP hPic)
{
	HDC		hdcMem;
	HGDIOBJ hbmpOld;
	wchar_t szTextBuf[MAX_PATH] = { 0 };

	// Erase rectangle 
	FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem,
        g_mcwConfig.bDarkMode ? g_darkModeRes.hbrBackground :
        (HBRUSH) (COLOR_WINDOW + 1));

	hdcMem = CreateCompatibleDC(lpDrawItem->hDC);
	hbmpOld = SelectObject(hdcMem, hPic);

	BitBlt(lpDrawItem->hDC,
		lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
		lpDrawItem->rcItem.right - lpDrawItem->rcItem.left,
		lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top,
		hdcMem, 0, 0, SRCCOPY);

	// Draw Text
	SendMessage(lpDrawItem->hwndItem, LB_GETTEXT,
		lpDrawItem->itemID, (LPARAM) szTextBuf);

    if (g_mcwConfig.bDarkMode) 
    {
        SetTextColor(lpDrawItem->hDC, DarkModeColor::StaticText);
    }

	TextOut(lpDrawItem->hDC, lpDrawItem->rcItem.left + 18,
		lpDrawItem->rcItem.top, szTextBuf, (int) wcslen(szTextBuf));

	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);
}

//
// Create resources needed for the dark-mode color scheme
//
void CreateDarkModeResources()
{
    g_darkModeRes.hbrBackground = CreateSolidBrush(DarkModeColor::Background);
    g_darkModeRes.hbrEditBackground = CreateSolidBrush(DarkModeColor::EditBackground);
    g_darkModeRes.hbrFrame = CreateSolidBrush(DarkModeColor::Frame);
    g_darkModeRes.hbrButtonFace = CreateSolidBrush(DarkModeColor::ButtonFace);
    g_darkModeRes.hbrButtonFaceHilite = CreateSolidBrush(DarkModeColor::ButtonFaceHilite);
}

void DestroyDarkModeResources()
{
    DeleteObject(g_darkModeRes.hbrButtonFaceHilite);
    DeleteObject(g_darkModeRes.hbrButtonFace);
    DeleteObject(g_darkModeRes.hbrFrame);
    DeleteObject(g_darkModeRes.hbrBackground);
    DeleteObject(g_darkModeRes.hbrEditBackground);

}

