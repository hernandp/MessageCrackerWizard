/******************************************************************************

MIT License

This file is part of Message Cracker Wizard

Copyright (c) 2003, 2017, 2018 Hern醤 Di Pietro

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

***************************************************************************

	Windows API Message Cracker Wizard Tool
	for easier use of WINDOWSX.H message cracking macros

	(c) 2003, 2017, 2018 Hernan Di Pietro

	Release 2.5

	version history
	1.0 - first release
	1.2   + multiple message selection feature
		  + added missing message crackers for WM_HOTKEY and WM_COPYDATA
		  + fixed small interface bugs
	2.0   + added WM_CTLCOLORxxx message crackers
		  + added show/hide target code
		  + added enable/disable stay on top
		  + added window transparency options for 2000/XP
		  + added message filter
		  + added bitmaps on list box
		  + added "No heading comments" option
		  + added "No TODO comments" option
		  + fixed keyboard logic and tab stops
	2.1   - fixed clipboard-copy bug (Thanks to Agnel Kurian mailto:agnel.kurian@gmail.com)
	2.2   - C++ code modernization to near- modern standards
		  - updated for compilation with VS2015
		  - changed to Unicode for all strings
		  - fixed analysis, warnings, and related issues
		  - changed to MIT license
    2.5   - fix memory corruption error 
          - fix non-dialog procedure generating 'hwndDlg' instead of 'hwnd' 
          + added re-sizable window feature.
          + added 'Dark Mode' color scheme
          + settings are saved/retrieved  to/from Registry
          * removed 'Hide Target Code' option

*******************************************************************************/


#include "stdafx.h"
#include "MsgCrackWizard.h"
#include "resource.h"

// globals
UINT                g_fFilter;
MCWCONFIG           g_mcwConfig;
DARKMODERESOURCES   g_darkModeRes;

// search listbox (ctrl + s)
WCHAR               g_strFindWhat[80];
UINT                g_idFindMessage;
HWND                g_hFindDlg;
FINDREPLACEW        g_fr;


//
// Returns a child window RECT in parent-window  coordinate space
//
static RECT GetChildWindowRect(HWND hwndParent, UINT childId)
{
    assert(GetDlgItem(hwndParent, childId));

    RECT rc;
    GetClientRect(GetDlgItem(hwndParent, childId), &rc);
    MapWindowPoints(GetDlgItem(hwndParent, childId), hwndParent, (LPPOINT)&rc, 2);
    return rc;
}

static RECT GetChildWindowRect(HWND hwndParent, HWND hwndChild)
{
    RECT rc;
    GetClientRect(hwndChild, &rc);
    MapWindowPoints(hwndChild, hwndParent, (LPPOINT)&rc, 2);
    return rc;
}


///////////////////////////////////////////////////////////////////////////////
//  Process WM_CREATE message for window/dialog: Cls
//
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // find commdlg init
    g_fr = { 0 };
    g_fr.lStructSize = sizeof(FINDREPLACEW);
    g_fr.hwndOwner = hwnd;
    g_fr.hInstance = hInstance;
    g_fr.Flags |= FR_DOWN | FR_HIDEWHOLEWORD  | FR_HIDEMATCHCASE;
    g_fr.lpstrFindWhat = g_strFindWhat;
    g_fr.wFindWhatLen = _countof(g_strFindWhat);

    g_idFindMessage = RegisterWindowMessage(FINDMSGSTRING);

	// Allocate memory for the message list box
	SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_INITSTORAGE, (WPARAM) NUMMSG,
		(LPARAM) sizeof(msginfo));

	// load bitmaps for ownerdrawn controls
	hbmpKeyboard = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_KEYBOARD),
		IMAGE_BITMAP, 16, 16, NULL);

	hbmpMouse = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_MOUSE),
		IMAGE_BITMAP, 16, 16, NULL);

	hbmpClipboard = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_CLIPBOARD),
		IMAGE_BITMAP, 16, 16, NULL);

	hbmpMDI = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_MDI),
		IMAGE_BITMAP, 16, 16, NULL);

	hbmpNonClient = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_NONCLIENT),
		IMAGE_BITMAP, 16, 16, NULL);

	hbmpGeneral = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_GENERAL),
		IMAGE_BITMAP, 16, 16, NULL);

	hbmpWindow = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_WINDOW),
		IMAGE_BITMAP, 16, 16, NULL);

    hbmpKeyboard_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_KEYBOARD_D),
        IMAGE_BITMAP, 16, 16, NULL);

    hbmpMouse_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_MOUSE_D),
        IMAGE_BITMAP, 16, 16, NULL);

    hbmpClipboard_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_CLIPBOARD_D),
        IMAGE_BITMAP, 16, 16, NULL);

    hbmpMDI_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_MDI_D),
        IMAGE_BITMAP, 16, 16, NULL);

    hbmpNonClient_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_NONCLIENT_D),
        IMAGE_BITMAP, 16, 16, NULL);

    hbmpGeneral_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_GENERAL_D),
        IMAGE_BITMAP, 16, 16, NULL);

    hbmpWindow_d = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_WINDOW_D),
        IMAGE_BITMAP, 16, 16, NULL);


	// Disable window transparency option if not present on current OS
	HMODULE hUser32;
	HMENU   hMenu;

	hUser32 = LoadLibrary(L"user32.dll");
	if (hUser32)
	{
		if (GetProcAddress(hUser32, "SetLayeredWindowAttributes") == NULL)
		{
			// not present on USER32.DLL, disable menus
			hMenu = GetMenu(hwnd);
			EnableMenuItem(hMenu, ID_WINDOWTRANSPARENCY_10, MF_GRAYED);
			EnableMenuItem(hMenu, ID_WINDOWTRANSPARENCY_25, MF_GRAYED);
			EnableMenuItem(hMenu, ID_WINDOWTRANSPARENCY_50, MF_GRAYED);
			EnableMenuItem(hMenu, ID_WINDOWTRANSPARENCY_75, MF_GRAYED);
			DrawMenuBar(hwnd);
		}
		FreeLibrary(hUser32);
	}

	// Set message filter flags to include all
	g_fFilter = NOFILTER;

	// Setup Window Controls (fill list box,etc)
	SetupControls(hwnd);
	return 1L;
}

//
//  Process WM_SIZE message for window/dialog: Cls
//
void Cls_OnSize(HWND hwnd, UINT state, int dialogWidth, int dialogHeight)
{
    // This resizes and/or positions controls to accommodate new 
    // window size.
    //
    // All units are based on pixels. DO NOT refer to editor or .RC file units,
    // as they are specified in DLUs (Dialog-Units). 
    //
    // In any case, use MapDialogRect API to do necessary conversion.
    //
           
    const int DIALOG_OUTER_SPACING = 10;
    const int GROUPBOX_INTERIOR_SPACING = 10;
    const int CONTROL_SPACING = 8;
    const int DEFAULT_BUTTON_WIDTH = 86;
    const int DEFAULT_BUTTON_HEIGHT = 24;

    RECT rr;
    GetClientRect(GetDlgItem(hwnd, IDC_MESSAGES), &rr);

    // Setup layout    
    UINT layoutColWidth[3] = { 0, 165, 86 };
    layoutColWidth[0] = dialogWidth - ( GetSystemMetrics(SM_CXHSCROLL) + layoutColWidth[1] + layoutColWidth[2] + (CONTROL_SPACING * 2));
    
    UINT messageListHeight = 132;

    RECT rcLabel;
    GetWindowRect(GetDlgItem(hwnd, IDC_MESSAGES_LABEL), &rcLabel);
    const UINT labelCtlHeight = rcLabel.bottom - rcLabel.top;

    // 
    // Second column controls
    //
    RECT rcCombo;
    GetWindowRect(GetDlgItem(hwnd, IDC_WINDOWID), &rcCombo);
    UINT comboHeight = rcCombo.bottom - rcCombo.top;

    RECT rcCheck;
    GetWindowRect(GetDlgItem(hwnd, IDC_MAKEWNDPROC), &rcCheck);
    UINT checkHeight = rcCheck.bottom - rcCheck.top;

    RECT rcRadio;
    GetWindowRect(GetDlgItem(hwnd, IDC_RADWINDOW), &rcRadio);
    UINT radioHeight = rcRadio.bottom - rcRadio.top;

    UINT vPos = DIALOG_OUTER_SPACING;

    UINT colLeft = layoutColWidth[0] + DIALOG_OUTER_SPACING + CONTROL_SPACING;
    MoveWindow(GetDlgItem(hwnd, IDC_WINDOWID_LABEL), colLeft,  vPos, layoutColWidth[1], labelCtlHeight, TRUE);
    vPos += labelCtlHeight;

    MoveWindow(GetDlgItem(hwnd, IDC_WINDOWID), colLeft, vPos, layoutColWidth[1], comboHeight, TRUE);
    vPos += comboHeight + 1;

    MoveWindow(GetDlgItem(hwnd, IDC_MAKEWNDPROC), colLeft, vPos, layoutColWidth[1], checkHeight, TRUE);
    vPos += checkHeight + 1;

    UINT typeGroupBoxTop = vPos;
    MoveWindow(GetDlgItem(hwnd, IDC_TYPEGROUPBOX), colLeft, vPos, layoutColWidth[1], 58, TRUE);
    vPos += labelCtlHeight + 2;

    MoveWindow(GetDlgItem(hwnd, IDC_RADWINDOW), colLeft + CONTROL_SPACING, vPos, layoutColWidth[1] - CONTROL_SPACING * 2, radioHeight, TRUE);
    vPos += radioHeight + 1;

    MoveWindow(GetDlgItem(hwnd, IDC_DIALOG), colLeft + CONTROL_SPACING, vPos, layoutColWidth[1] - CONTROL_SPACING * 2, radioHeight, TRUE);
    vPos += radioHeight + 1;

    vPos += CONTROL_SPACING * 2;
    MoveWindow(GetDlgItem(hwnd, IDC_OPTIONSGROUPBOX), colLeft, vPos, layoutColWidth[1], 59, TRUE);
    vPos += labelCtlHeight + 2;

    MoveWindow(GetDlgItem(hwnd, IDC_NOHEADINGCOMMENTS), colLeft + CONTROL_SPACING, vPos, layoutColWidth[1] - CONTROL_SPACING * 2, checkHeight, TRUE);
    vPos += checkHeight + 2;

    MoveWindow(GetDlgItem(hwnd, IDC_NOTODOCOMMENTS), colLeft + CONTROL_SPACING, vPos, layoutColWidth[1] - CONTROL_SPACING * 2, checkHeight, TRUE);

    InvalidateRect(GetDlgItem(hwnd, IDC_RADWINDOW), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_DIALOG), NULL, TRUE);
  
    InvalidateRect(GetDlgItem(hwnd, IDC_NOTODOCOMMENTS), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_NOHEADINGCOMMENTS), NULL, TRUE);

    //
    // Third column controls
    //

    colLeft += layoutColWidth[1] + CONTROL_SPACING;
    vPos = typeGroupBoxTop + 5;

    MoveWindow(GetDlgItem(hwnd, IDC_COPYMACRO), colLeft, vPos, layoutColWidth[2], DEFAULT_BUTTON_HEIGHT, TRUE);
    vPos += DEFAULT_BUTTON_HEIGHT + 4;

    MoveWindow(GetDlgItem(hwnd, IDC_COPYFUNC),  colLeft, vPos, layoutColWidth[2], DEFAULT_BUTTON_HEIGHT, TRUE);
    vPos += DEFAULT_BUTTON_HEIGHT + 13;

    MoveWindow(GetDlgItem(hwnd, IDC_CLOSE), colLeft, vPos, layoutColWidth[2], DEFAULT_BUTTON_HEIGHT, TRUE);

    InvalidateRect(GetDlgItem(hwnd, IDC_COPYMACRO), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_COPYFUNC), NULL, TRUE);
    InvalidateRect(GetDlgItem(hwnd, IDC_CLOSE), NULL, TRUE);

    // First column controls

    MoveWindow(GetDlgItem(hwnd, IDC_MESSAGES_LABEL), DIALOG_OUTER_SPACING, DIALOG_OUTER_SPACING, layoutColWidth[0], labelCtlHeight, TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_MESSAGES), DIALOG_OUTER_SPACING, DIALOG_OUTER_SPACING + labelCtlHeight, layoutColWidth[0], messageListHeight, TRUE);

    MoveWindow(GetDlgItem(hwnd, IDC_FILTERBTN), DIALOG_OUTER_SPACING + layoutColWidth[0] - DEFAULT_BUTTON_WIDTH,
        DIALOG_OUTER_SPACING + labelCtlHeight + messageListHeight + CONTROL_SPACING,
        DEFAULT_BUTTON_WIDTH, DEFAULT_BUTTON_HEIGHT, TRUE);

    InvalidateRect(GetDlgItem(hwnd, IDC_FILTERBTN), NULL, TRUE);

    //
    // Target Code group-box and controls
    //

    RECT rcFilterBtn = GetChildWindowRect(hwnd, IDC_FILTERBTN);
    vPos = rcFilterBtn.bottom + 2;
    const UINT groupHeight = dialogHeight - (rcFilterBtn.bottom + 2) - DIALOG_OUTER_SPACING;

    MoveWindow(GetDlgItem(hwnd, IDC_TARGETCODEGROUP), DIALOG_OUTER_SPACING, vPos, dialogWidth - DIALOG_OUTER_SPACING * 2, groupHeight, TRUE);
        
    vPos += labelCtlHeight + CONTROL_SPACING;

    const UINT groupElemLeft = DIALOG_OUTER_SPACING + GROUPBOX_INTERIOR_SPACING;
    const UINT groupElemWidth = dialogWidth - GROUPBOX_INTERIOR_SPACING * 2 - DIALOG_OUTER_SPACING * 2;

    MoveWindow(GetDlgItem(hwnd, IDC_HANDLEMSG_LABEL), groupElemLeft, vPos, groupElemWidth, labelCtlHeight, TRUE);
    vPos += labelCtlHeight;

    const UINT msgTextHeight = (groupHeight / 2) - (DIALOG_OUTER_SPACING) - CONTROL_SPACING * 2;
    MoveWindow(GetDlgItem(hwnd, IDC_HANDLEMSG), groupElemLeft, vPos, groupElemWidth, msgTextHeight, TRUE);
    vPos += msgTextHeight;

    MoveWindow(GetDlgItem(hwnd, IDC_MSGFUNCTION_LABEL), groupElemLeft, vPos, groupElemWidth, labelCtlHeight, TRUE);
    vPos += labelCtlHeight;

    MoveWindow(GetDlgItem(hwnd, IDC_MSGFUNCTION), groupElemLeft, vPos, groupElemWidth, msgTextHeight, TRUE);

    // Update configuration data

    RECT wndRect;
    GetWindowRect(hwnd, &wndRect);
    g_mcwConfig.rcWindow = wndRect;
}

//
// process WM_ONCOMMAND
//
void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

	HMENU   hMenu;
	BYTE    bAlpha;         // window transparency alpha
	HMODULE hUser32;
	PSLWA   pSetLayeredWndAttr;

	switch (id)
	{
		// menus

		case ID_HELP_ABOUT:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlg_DlgProc);
			break;

		case ID_FILE_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case ID_EDIT_COPYMACRO:
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_COPYMACRO, 0), 0);
			break;

		case ID_EDIT_COPYFUNCTION:
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_COPYFUNC, 0), 0);
			break;

		case IDC_FILTERBTN:				// filter button on dialogbox
		case ID_EDIT_MESSAGEFILTERS:

			int iRes;
			iRes = (int) DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_FILTER),
				hwnd, FilterDlg_DlgProc);

			// update listbox with selected filters
			if (iRes == IDOK)
				FillListBox(hwnd, g_fFilter);

			break;

        case ID_EDIT_SEARCHLISTBOX:
            if (g_hFindDlg == NULL)
            {
                g_fr.Flags &= ~FR_DIALOGTERM;
                g_hFindDlg = FindTextW(&g_fr);
            }

            break;

        case ID_EDIT_CLEARALLSELECTS:
            ListBox_SetSel(GetDlgItem(hwnd, IDC_MESSAGES), FALSE, -1);
            UpdateUI(hwnd);
            break;

        case ID_VIEW_DARKCOLORSCHEME:
            
            if (!g_mcwConfig.bDarkMode)
                MessageBox(hwnd, L"WARNING, This is a work-in-progress feature. You are invited to fix it in source ;)", L"Dark Mode", MB_ICONWARNING);
            
            hMenu = GetMenu(hwnd);

            // invert menu checkmark
            CheckMenuItem(hMenu, ID_VIEW_DARKCOLORSCHEME, CheckMenuItem(hMenu, ID_VIEW_DARKCOLORSCHEME,
                MF_CHECKED) == MF_CHECKED ? MF_UNCHECKED : MF_CHECKED);

            if (GetMenuState(hMenu, ID_VIEW_DARKCOLORSCHEME, MF_BYCOMMAND) & MF_CHECKED)
            {
                g_mcwConfig.bDarkMode = true;
                EnableOwnerdrawnControls(hwnd);               
            }
            else
            {
                DisableOwnerdrawnControls(hwnd);
                g_mcwConfig.bDarkMode = false;
            }
            InvalidateRect(hwnd, NULL, TRUE);
            break;

		case ID_VIEW_STAYONTOP:
			// Enable-disable topmost window
			hMenu = GetMenu(hwnd);

			// invert menu checkmark
			CheckMenuItem(hMenu, ID_VIEW_STAYONTOP, CheckMenuItem(hMenu, ID_VIEW_STAYONTOP,
				MF_CHECKED) == MF_CHECKED ? MF_UNCHECKED : MF_CHECKED);

            if (GetMenuState(hMenu, ID_VIEW_STAYONTOP, MF_BYCOMMAND) & MF_CHECKED)
            {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                g_mcwConfig.bStayOnTop = true;
            }
            else
            {
                SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                g_mcwConfig.bStayOnTop = false;
            }				
			break;

			// Make Window Transparent (2000/XP Only)

		case ID_WINDOWTRANSPARENCY_SOLID:
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            UpdateWindow(hwnd);
            g_mcwConfig.windowAlpha = ALPHA_SOLID;

            hMenu = GetMenu(hwnd);
            CheckMenuRadioItem(hMenu, ID_WINDOWTRANSPARENCY_SOLID, ID_WINDOWTRANSPARENCY_75, ID_WINDOWTRANSPARENCY_SOLID, MF_BYCOMMAND);
			break;

		case ID_WINDOWTRANSPARENCY_10:
		case ID_WINDOWTRANSPARENCY_25:
		case ID_WINDOWTRANSPARENCY_50:
		case ID_WINDOWTRANSPARENCY_75:

			if (id == ID_WINDOWTRANSPARENCY_10)    bAlpha = ALPHA_TRANS_10;
			if (id == ID_WINDOWTRANSPARENCY_25)    bAlpha = ALPHA_TRANS_25;
			if (id == ID_WINDOWTRANSPARENCY_50)    bAlpha = ALPHA_TRANS_50;
			if (id == ID_WINDOWTRANSPARENCY_75)    bAlpha = ALPHA_TRANS_75;

            hMenu = GetMenu(hwnd);
            CheckMenuRadioItem(hMenu, ID_WINDOWTRANSPARENCY_SOLID, ID_WINDOWTRANSPARENCY_75, id, MF_BYCOMMAND);

			// modify window style        
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

			// call function from USER32
			hUser32 = LoadLibrary(L"user32.dll");
			if (hUser32)
			{
				pSetLayeredWndAttr = (PSLWA) GetProcAddress(hUser32, "SetLayeredWindowAttributes");
				pSetLayeredWndAttr(hwnd, 0, bAlpha, LWA_ALPHA);
				FreeLibrary(hUser32);
			}

            g_mcwConfig.windowAlpha = bAlpha;
			break;

			//
			// WM_COMMAND for Buttons
			//

		case IDC_COPYMACRO:
			CopyMacroToCB(hwnd);
			break;

		case IDC_COPYFUNC:
			CopyFuncToCB(hwnd);
			break;

		case IDC_CLOSE:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case IDC_WINDOWID:
			switch (codeNotify)
			{
				// update the macro/function edit boxes when the user changes
				// the WindowID combo box
				case CBN_SELCHANGE:
				case CBN_EDITCHANGE:
					UpdateUI(hwnd);
					break;
			}
			break;

		case IDC_MESSAGES:
			switch (codeNotify)
			{
				case LBN_SELCHANGE:
					// update the edit boxes and enable/disable the check when the list box is clicked
					if (SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETSELCOUNT, 0, 0) == 0)
						EnableControls(hwnd, FALSE);
					else
						EnableControls(hwnd, TRUE);

					UpdateUI(hwnd);
					break;
			}
			break;

		case IDC_MAKEWNDPROC:
			// enable the radio buttons when 'make wnd proc' is checked
			EnableWindow(GetDlgItem(hwnd, IDC_DIALOG),
				IsDlgButtonChecked(hwnd, IDC_MAKEWNDPROC) == BST_CHECKED ? TRUE : FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_RADWINDOW),
				IsDlgButtonChecked(hwnd, IDC_MAKEWNDPROC) == BST_CHECKED ? TRUE : FALSE);
			break;

		case IDC_DIALOG:
		case IDC_RADWINDOW:
			UpdateUI(hwnd);
			break;

	}
}

//
//  Process WM_DRAWITEM message for window/dialog: Cls
//
void Cls_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	wchar_t    szTextBuf[MAX_PATH] = { 0 };
	int     x = lpDrawItem->rcItem.left;
	int     y = lpDrawItem->rcItem.top;

    if (lpDrawItem->CtlID == IDC_MESSAGES)
    {
        switch (lpDrawItem->itemAction)
        {
        case ODA_DRAWENTIRE:
        case ODA_SELECT:

            // Draw message bitmap depending on type
            DrawItemBitmap(lpDrawItem, SelectItemBitmap(lpDrawItem));

            if (lpDrawItem->itemState & ODS_SELECTED)
            {
                // Draw selection state (uses new RECT to avoid erasing bitmap)     
                RECT rsel;
                CopyRect(&rsel, &lpDrawItem->rcItem);
                rsel.left += 16;

                SetBkMode(lpDrawItem->hDC, TRANSPARENT);
                SetTextColor(lpDrawItem->hDC, RGB(0xff, 0xff, 0xff));
                FillRect(lpDrawItem->hDC, &rsel, (HBRUSH)(COLOR_HIGHLIGHT + 1));

                // Draw Text
                SendMessage(lpDrawItem->hwndItem, LB_GETTEXT,
                    lpDrawItem->itemID, (LPARAM)szTextBuf);
                SetBkMode(lpDrawItem->hDC, TRANSPARENT);
                TextOut(lpDrawItem->hDC, lpDrawItem->rcItem.left + 18,
                    lpDrawItem->rcItem.top, szTextBuf, (int)wcslen(szTextBuf));
            }
            else
            {
                // Normal State
                SetTextColor(lpDrawItem->hDC, 0);
                DrawItemBitmap(lpDrawItem, SelectItemBitmap(lpDrawItem));
            }
            break;
        }
    }

    if (lpDrawItem->CtlType == ODT_BUTTON)
    {

        SetBkColor(lpDrawItem->hDC, DarkModeColor::EditBackground);
        SetBkMode(lpDrawItem->hDC, TRANSPARENT);
        FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, g_darkModeRes.hbrButtonFace);
        SetTextColor(lpDrawItem->hDC, DarkModeColor::StaticText);

        if (lpDrawItem->itemState & ODS_DISABLED)
        {
            FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, g_darkModeRes.hbrButtonFace);
            SetTextColor(lpDrawItem->hDC, DarkModeColor::TextDisabled);
        }

        if (lpDrawItem->itemState & ODS_HOTLIGHT)
        {
            SetTextColor(lpDrawItem->hDC, DarkModeColor::EditText);
            FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, GetSysColorBrush(COLOR_WINDOW));
        }

        SetBkColor(lpDrawItem->hDC, DarkModeColor::EditBackground);
        SetBkMode(lpDrawItem->hDC, TRANSPARENT);

        FrameRect(lpDrawItem->hDC, &lpDrawItem->rcItem, g_darkModeRes.hbrFrame);

        GetDlgItemText(hwnd, lpDrawItem->CtlID, szTextBuf, MAX_PATH);
        DrawText(lpDrawItem->hDC, szTextBuf, -1, (RECT*)&lpDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    }
}

//
//  Process WM_MEASUREITEM message for window/dialog: Cls
//
void Cls_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
    if (lpMeasureItem->CtlID == IDC_MESSAGES)
    {
        // Set control size
        lpMeasureItem->itemHeight = 16;
    }	
}

void Cls_OnClose(HWND hwnd)
{
	// destroy allocated resources
	DeleteObject((HBITMAP) hbmpKeyboard);
	DestroyWindow(hwnd);
}

void Cls_OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

void Cls_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rcGrp = GetChildWindowRect(hwnd, IDC_TYPEGROUPBOX);
    FillRect(hdc, &rcGrp, g_mcwConfig.bDarkMode ? g_darkModeRes.hbrBackground : GetSysColorBrush(COLOR_BTNFACE));

    rcGrp = GetChildWindowRect(hwnd, IDC_OPTIONSGROUPBOX);
    FillRect(hdc, &rcGrp, g_mcwConfig.bDarkMode ? g_darkModeRes.hbrBackground : GetSysColorBrush(COLOR_BTNFACE));

    EndPaint(hwnd, &ps);
}

HBRUSH Cls_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    if (g_mcwConfig.bDarkMode)
    {   
        /*wchar_t textBuffer[255];
        RECT rc;
        GetClientRect(hwndChild, &rc);

        GetWindowText(hwndChild, textBuffer, _countof(textBuffer));
        */
        SetTextColor(hdc, DarkModeColor::StaticText);
        SetBkColor(hdc, DarkModeColor::Background);
       /* DrawText(hdc, textBuffer, -1, &rc, DT_SINGLELINE);*/

        return g_darkModeRes.hbrBackground;
    }
    return (HBRUSH) FALSE;
}

HBRUSH Cls_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    if (g_mcwConfig.bDarkMode)
    {
        SetTextColor(hdc, DarkModeColor::EditText);
        SetBkColor(hdc, DarkModeColor::EditBackground);
        return g_darkModeRes.hbrEditBackground;
    }

    return (HBRUSH)FALSE;
}

HBRUSH Cls_OnCtlColorBtn(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
  /*  if (g_mcwConfig.bDarkMode)
    {
        SetTextColor(hdc, DarkModeColor::EditText);
        SetBkColor(hdc, DarkModeColor::EditBackground);
        return g_darkModeRes.hbrEditBackground;
    }*/

    return (HBRUSH)FALSE;
}

HBRUSH Cls_OnCtlColorScrollbar(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    /*if (g_mcwConfig.bDarkMode)
      {
          SetTextColor(hdc, DarkModeColor::EditText);
          SetBkColor(hdc, DarkModeColor::EditBackground);
          return g_darkModeRes.hbrEditBackground;
      }*/

    return (HBRUSH)FALSE;
}

void Cls_OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
    const int MinHeight = 452;
    const int MinWidth = 507;

    lpMinMaxInfo->ptMinTrackSize.x = MinWidth;
    lpMinMaxInfo->ptMinTrackSize.y = MinHeight;
}

void Cls_OnMove(HWND hwnd, int x, int y)
{
    RECT wndRect;
    GetWindowRect(hwnd, &wndRect);
    g_mcwConfig.rcWindow = wndRect;
}

BOOL Cls_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    if (g_mcwConfig.bDarkMode)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, g_darkModeRes.hbrBackground);
        return TRUE;
    }

    return FALSE;
}

//
//  Process WM_INITDIALOG message for window/dialog: FilterDlg
//
BOOL FilterDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	// Mark checkbox depending on filter settings

	CheckDlgButton(hwnd, IDC_FGENERAL, (g_fFilter & GENERAL ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hwnd, IDC_FKEYBOARD, (g_fFilter & KEYBOARD ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hwnd, IDC_FCLIPBRD, (g_fFilter & CLIPBOARD ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hwnd, IDC_FNCLIENT, (g_fFilter & NONCLIENT ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hwnd, IDC_FMDI, (g_fFilter & MDI ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(hwnd, IDC_FMOUSE, (g_fFilter & MOUSE ? BST_CHECKED : BST_UNCHECKED));

   

	return 1L;
}

//
//  Process WM_COMMAND message for window/dialog: AboutDlg
//
void AboutDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	EndDialog(hwnd, IDOK);
}

// ----------------------------------------------------------------------------
//
// AboutDlg  Dialog Procedure
//
// ----------------------------------------------------------------------------
BOOL CALLBACK AboutDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwndDlg, WM_COMMAND, AboutDlg_OnCommand);
		//// TODO: Add dialog message crackers here...

		default: return FALSE;
	}
}

// ----------------------------------------------------------------------------
//
//  Process WM_COMMAND message for window/dialog: FilterDlg
//
// ----------------------------------------------------------------------------
void FilterDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
		case IDOK:
			// apply filters

			if (IsDlgButtonChecked(hwnd, IDC_FMOUSE) == BST_CHECKED)
				g_fFilter |= MOUSE;
			else
				g_fFilter &= ~MOUSE;

			if (IsDlgButtonChecked(hwnd, IDC_FKEYBOARD) == BST_CHECKED)
				g_fFilter |= KEYBOARD;
			else
				g_fFilter &= ~KEYBOARD;

			if (IsDlgButtonChecked(hwnd, IDC_FCLIPBRD) == BST_CHECKED)
				g_fFilter |= CLIPBOARD;
			else
				g_fFilter &= ~CLIPBOARD;

			if (IsDlgButtonChecked(hwnd, IDC_FMDI) == BST_CHECKED)
				g_fFilter |= MDI;
			else
				g_fFilter &= ~MDI;

			if (IsDlgButtonChecked(hwnd, IDC_FGENERAL) == BST_CHECKED)
				g_fFilter |= GENERAL;
			else
				g_fFilter &= ~GENERAL;

			if (IsDlgButtonChecked(hwnd, IDC_FNCLIENT) == BST_CHECKED)
				g_fFilter |= NONCLIENT;
			else
				g_fFilter &= ~NONCLIENT;

			EndDialog(hwnd, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
	}
}

// ----------------------------------------------------------------------------
//
// FilterDlg  Dialog Procedure
//
// ----------------------------------------------------------------------------
BOOL CALLBACK FilterDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwndDlg, WM_INITDIALOG, FilterDlg_OnInitDialog);
		HANDLE_MSG(hwndDlg, WM_COMMAND, FilterDlg_OnCommand);

		//// TODO: Add dialog message crackers here...

		default: return FALSE;
	}
}

int user_ListBox_FindString(HWND hListBox, int beginPos, const WCHAR* searchStr) {
    int count = ListBox_GetCount(hListBox);
    WCHAR strItemText[512] = { 0 };

    int offset = -1;
    if (g_fr.Flags & FR_DOWN)
    {
        offset = 1;
    }

    for (int i = beginPos; i < count && i >= 0; i += offset) {
        ListBox_GetText(hListBox, i, strItemText);

        // 这里使用wcsstr来查找子字符串
        if (wcsstr(strItemText, searchStr) != NULL) {
            return i;  // 返回第一个匹配项的索引
        }
    }
    return LB_ERR;  // 没有找到匹配项
}


// ----------------------------------------------------------------------------
//
// Window (main dialog) procedure
//
// ----------------------------------------------------------------------------
BOOL CALLBACK MainDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == g_idFindMessage)
    {
        if (g_fr.Flags & FR_DIALOGTERM)
        {
            g_hFindDlg = NULL;
        }
        else if (g_fr.Flags & FR_FINDNEXT)
        {
            HWND hListBox = GetDlgItem(hwnd, IDC_MESSAGES);
            _wcsupr_s(g_fr.lpstrFindWhat, _countof(g_strFindWhat));

            int caretIndex = ListBox_GetCaretIndex(hListBox);
            int topIndex = ListBox_GetTopIndex(hListBox);
            int beginPos = 0;

            if (g_fr.Flags & FR_DOWN)
            {
                beginPos = max(caretIndex, topIndex) + 1;
            }
            else 
            {
                beginPos = topIndex - 1;
            }

            int pos = user_ListBox_FindString(hListBox, beginPos, g_fr.lpstrFindWhat);
            if (pos != LB_ERR)
            {
                ListBox_SetTopIndex(hListBox, pos);
            }
            else
            {
                MessageBox(g_hFindDlg, L"can't find text in listbox", L"not found", MB_OK);
            }
        } // END  else if (g_fr.Flags & FR_FINDNEXT)
    } // END  if (msg == g_idFindMessage)

	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, Cls_OnClose);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);
		HANDLE_MSG(hwnd, WM_COMMAND, Cls_OnCommand);
		HANDLE_MSG(hwnd, WM_INITDIALOG, Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_DRAWITEM, Cls_OnDrawItem);
		HANDLE_MSG(hwnd, WM_MEASUREITEM, Cls_OnMeasureItem);
        HANDLE_MSG(hwnd, WM_SIZE, Cls_OnSize);
        HANDLE_MSG(hwnd, WM_MOVE, Cls_OnMove);
        HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
        HANDLE_MSG(hwnd, WM_GETMINMAXINFO, Cls_OnGetMinMaxInfo);
        HANDLE_MSG(hwnd, WM_ERASEBKGND, Cls_OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_CTLCOLORSTATIC, Cls_OnCtlColorStatic);
        HANDLE_MSG(hwnd, WM_CTLCOLORBTN, Cls_OnCtlColorBtn);
        HANDLE_MSG(hwnd, WM_CTLCOLOREDIT, Cls_OnCtlColorEdit);
        //HANDLE_MSG(hwnd, WM_CTLCOLORSCROLLBAR, Cls_OnCtlColorScrollbar);

		default:
			return FALSE;
	}
}

// ----------------------------------------------------------------------------
//
// Entry-point
//
// ----------------------------------------------------------------------------

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
	_In_ LPSTR cmdLine, _In_ int nCmdShow)
{
	static wchar_t szClassName[] = L"MessageCrackerWizard";
	HWND        hwnd;
    MSG         msg;
	WNDCLASSEX  wcex;
	HACCEL      hAccel;

	// Get the class for the system dialog class
	wcex.cbSize = sizeof(wcex);
	GetClassInfoEx(NULL, L"#32770", &wcex);
	wcex.style &= ~CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;

	// Register class
	wcex.lpszClassName = szClassName;
	wcex.cbClsExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MCWICON));
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MCWICON));
	wcex.cbWndExtra = DLGWINDOWEXTRA;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, L"Error in RegisterClassEx", L"error", MB_ICONERROR);
		return 0;
	}

	// initialize common controls, accelerators, dark-mode r and load configuration
	InitCommonControls();
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));
    LoadConfig(g_mcwConfig);
    CreateDarkModeResources();

	// create the dialog    
	hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_WIZARD), 0, MainDlgProc);

    if (hwnd != NULL)
    {
        // Override the default 'center' position if all values are 
        // present.

        if (g_mcwConfig.rcWindow.bottom != -1 &&
            g_mcwConfig.rcWindow.left != -1 &&
            g_mcwConfig.rcWindow.top != -1 &&
            g_mcwConfig.rcWindow.right != -1)
        {
            MoveWindow(hwnd,
                g_mcwConfig.rcWindow.left,
                g_mcwConfig.rcWindow.top,
                g_mcwConfig.rcWindow.right - g_mcwConfig.rcWindow.left,
                g_mcwConfig.rcWindow.bottom - g_mcwConfig.rcWindow.top, FALSE);
        }

        ShowWindow(hwnd, SW_NORMAL);

        // Message Loop
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            if (!TranslateAccelerator(hwnd, hAccel, &msg))
            {
                if (!IsDialogMessage(hwnd, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
        SaveConfig(g_mcwConfig);
    }
    else
    {
        MessageBox(NULL, L"Can't create Main window", L"error", MB_ICONERROR);
    }

    DestroyDarkModeResources();    
    return (int)msg.wParam;
}

