/*****************************************************************

MIT License

This file is part of Message Cracker Wizard

Copyright (c) 2003-2017 Hernán Di Pietro

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
#ifndef MSGCRACKWIZARD_H
#define MSGCRACKWIZARD_H

#include "stdafx.h"

const  SIZE_T	MAX_WINDOWID_STR_LEN = 32;

// ownerdrawn listbox handles
extern HBITMAP  hbmpKeyboard, hbmpMouse, hbmpClipboard, hbmpGeneral, hbmpMDI, hbmpNonClient;
extern HBITMAP  hbmpWindow;

// define for window transparency
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED	0x80000
#endif

#ifndef LWA_ALPHA
#define LWA_ALPHA		2
#endif

// typedef for pointer to SetLayeredWindowAttributes
using PSLWA = DWORD(WINAPI *) (HWND, DWORD, BYTE, DWORD);

//
// function prototypes
//
BOOL CALLBACK FilterDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void CopyMacroToCB(HWND hwnd);
void CopyFuncToCB(HWND hwnd);
void SetupControls(HWND hwnd);
void FillListBox(HWND hwnd, UINT filter);
void UpdateUI(HWND hwnd);
void EnableControls(HWND hwnd, BOOL fEnabled);

// ownerdrawn-related functions
HBITMAP SelectItemBitmap(const DRAWITEMSTRUCT* lpDrawItem);
void    DrawItemBitmap(const DRAWITEMSTRUCT* lpDrawItem, HBITMAP hPic);

// message types
#define		KEYBOARD		1
#define		MOUSE			2
#define		CLIPBOARD		4
#define     GENERAL			8
#define		NONCLIENT		16
#define		MDI				32
#define     NOFILTER        KEYBOARD | \
	                        MOUSE | \
							GENERAL | \
							NONCLIENT| \
							MDI   | \
							CLIPBOARD;
extern UINT  g_fFilter;

//
// structure to hold window messages properties
//
const struct MESSAGEINFO
{
	UINT		messageType;
	wchar_t*	szMessageName;
	wchar_t*	szFunctionName;
	wchar_t*	szParameters;
	wchar_t*	szReturnType;
	//char* desc;
} extern msginfo[] = {

	KEYBOARD,	L"WM_VKEYTOITEM",        L"OnVkeyToItem",     L"(HWND hwnd, UINT vk, HWND hwndListbox, int iCaret)", L"int",
	KEYBOARD,	L"WM_SYSKEYDOWN",        L"OnSysKeyDown",     L"(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)", L"void",
	KEYBOARD,	L"WM_SYSKEYUP",          L"OnSysKeyUp",       L"(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)", L"void",
	KEYBOARD,	L"WM_SYSCHAR",           L"OnSysChar",        L"(HWND hwnd, TCHAR ch, int cRepeat)", L"void",
	KEYBOARD,	L"WM_SYSDEADCHAR",       L"OnSysDeadChar",    L"(HWND hwnd, TCHAR ch, int cRepeat)", L"void",
	KEYBOARD,	L"WM_KEYUP",             L"OnKeyUp",          L"(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)", L"void",
	KEYBOARD,	L"WM_KEYDOWN",           L"OnKeyDown",        L"(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)", L"void",
	KEYBOARD,	L"WM_CHAR",              L"OnChar",           L"(HWND hwnd, TCHAR ch, int cRepeat)", L"void",
	KEYBOARD,	L"WM_DEADCHAR",          L"OnDeadChar",       L"(HWND hwnd, TCHAR ch, int cRepeat)", L"void",
	KEYBOARD,	L"WM_HOTKEY",			L"OnHotKey",			L"(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk)", L"void",
	KEYBOARD,	L"WM_CHARTOITEM",        L"OnCharToItem",     L"(HWND hwnd, UINT ch, HWND hwndListbox, int iCaret)", L"int",
	GENERAL,	L"WM_COMPACTING",		L"OnCompacting",     L"(HWND hwnd, UINT compactRatio)", L"void",
	GENERAL,	L"WM_WININICHANGE",      L"OnWinIniChange",   L"(HWND hwnd, LPCTSTR lpszSectionName)", L"void",
	GENERAL,    L"WM_SYSCOLORCHANGE",    L"OnSysColorChange", L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_QUERYNEWPALETTE",   L"OnQueryNewPalette",L"(HWND hwnd)", L"BOOL",
	GENERAL,    L"WM_PALETTEISCHANGING", L"OnPaletteIsChanging", L"(HWND hwnd, HWND hwndPaletteChange)", L"void",
	GENERAL,    L"WM_PALETTECHANGED",    L"OnPaletteChanged", L"(HWND hwnd, HWND hwndPaletteChange)", L"void",
	GENERAL,	L"WM_FONTCHANGE",        L"OnFontChange",     L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_SPOOLERSTATUS",     L"OnSpoolerStatus",  L"(HWND hwnd, UINT status, int cJobInQueue)", L"void",
	GENERAL,    L"WM_DEVMODECHANGE",     L"OnDevModeChange",  L"(HWND hwnd, LPCTSTR lpszDeviceName)", L"void",
	GENERAL,    L"WM_TIMECHANGE",        L"OnTimeChange",     L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_POWER",             L"OnPower",          L"(HWND hwnd, int code)", L"void",
	GENERAL,    L"WM_QUERYENDSESSION",   L"OnQueryEndSession",L"(HWND hwnd)", L"BOOL",
	GENERAL,    L"WM_ENDSESSION",        L"OnEndSession",     L"(HWND hwnd, BOOL fEnding)", L"void",
	GENERAL,    L"WM_QUIT",              L"OnQuit",           L"(HWND hwnd, int exitCode)", L"void",
	GENERAL,    L"WM_SYSTEMERROR",       L"OnSystemError",    L"(HWND hwnd, int errCode)", L"void",
	GENERAL,    L"WM_CREATE",            L"OnCreate",         L"(HWND hwnd, LPCREATESTRUCT lpCreateStruct)", L"BOOL",
	NONCLIENT,  L"WM_NCCREATE",          L"OnNCCreate",       L"(HWND hwnd, LPCREATESTRUCT lpCreateStruct)", L"BOOL",
	GENERAL,    L"WM_DESTROY",           L"OnDestroy",        L"(HWND hwnd)", L"void",
	NONCLIENT,  L"WM_NCDESTROY",         L"OnNCDestroy",      L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_SHOWWINDOW",        L"OnShowWindow",     L"(HWND hwnd, BOOL fShow, UINT status)", L"void",
	GENERAL,    L"WM_SETREDRAW",         L"OnSetRedraw",      L"(HWND hwnd, BOOL fRedraw)", L"void",
	GENERAL,    L"WM_ENABLE",            L"OnEnable",         L"(HWND hwnd, BOOL fEnable)", L"void",
	GENERAL,    L"WM_SETTEXT",           L"OnSetText",        L"(HWND hwnd, LPCTSTR lpszText)", L"void",
	GENERAL,    L"WM_GETTEXT",           L"OnGetText",        L"(HWND hwnd, int cchTextMax, LPTSTR lpszText)", L"INT",
	GENERAL,    L"WM_GETTEXTLENGTH",     L"OnGetTextLength",  L"(HWND hwnd)", L"INT",
	GENERAL,    L"WM_WINDOWPOSCHANGING", L"OnWindowPosChanging", L"(HWND hwnd, LPWINDOWPOS lpwpos)", L"BOOL",
	GENERAL,    L"WM_WINDOWPOSCHANGED",  L"OnWindowPosChanged", L"(HWND hwnd, const LPWINDOWPOS lpwpos)", L"void",
	GENERAL,    L"WM_MOVE",              L"OnMove",           L"(HWND hwnd, int x, int y)", L"void",
	GENERAL,    L"WM_SIZE",              L"OnSize",           L"(HWND hwnd, UINT state, int cx, int cy)", L"void",
	GENERAL,    L"WM_CLOSE",             L"OnClose",          L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_QUERYOPEN",         L"OnQueryOpen",      L"(HWND hwnd)", L"BOOL",
	GENERAL,    L"WM_GETMINMAXINFO",     L"OnGetMinMaxInfo",  L"(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)", L"void",
	GENERAL,    L"WM_PAINT",             L"OnPaint",          L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_ERASEBKGND",        L"OnEraseBkgnd",     L"(HWND hwnd, HDC hdc)", L"BOOL",
	GENERAL,    L"WM_ICONERASEBKGND",    L"OnIconEraseBkgnd", L"(HWND hwnd, HDC hdc)", L"BOOL",
	NONCLIENT,  L"WM_NCPAINT",           L"OnNCPaint",        L"(HWND hwnd, HRGN hrgn)", L"void",
	NONCLIENT,  L"WM_NCCALCSIZE",        L"OnNCCalcSize",     L"(HWND hwnd, BOOL fCalcValidRects, NCCALCSIZE_PARAMS * lpcsp)", L"UINT",
	NONCLIENT,  L"WM_NCHITTEST",         L"OnNCHitTest",      L"(HWND hwnd, int x, int y)", L"UINT",
	GENERAL,    L"WM_QUERYDRAGICON",     L"OnQueryDragIcon",  L"(HWND hwnd)", L"HICON",
	MOUSE,      L"WM_DROPFILES",         L"OnDropFiles",      L"(HWND hwnd, HDROP hdrop)", L"void",
	GENERAL,    L"WM_ACTIVATE",          L"OnActivate",       L"(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)",L"void",
	GENERAL,    L"WM_ACTIVATEAPP",       L"OnActivateApp",    L"(HWND hwnd, BOOL fActivate, DWORD dwThreadId)", L"void",
	NONCLIENT,  L"WM_NCACTIVATE",        L"OnNCActivate",     L"(HWND hwnd, BOOL fActive, HWND hwndActDeact, BOOL fMinimized)", L"BOOL",
	GENERAL,    L"WM_SETFOCUS",          L"OnSetFocus",       L"(HWND hwnd, HWND hwndOldFocus)", L"void",
	GENERAL,    L"WM_KILLFOCUS",         L"OnKillFocus",      L"(HWND hwnd, HWND hwndNewFocus)", L"void",
	MOUSE,      L"WM_MOUSEMOVE",         L"OnMouseMove",      L"(HWND hwnd, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_LBUTTONDOWN",       L"OnLButtonDown",    L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_LBUTTONDBLCLK",     L"OnLButtonDblClk",  L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_LBUTTONUP",         L"OnLButtonUp",      L"(HWND hwnd, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_RBUTTONDOWN",       L"OnRButtonDown",    L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_RBUTTONDBLCLK",     L"OnRButtonDblClk",  L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_RBUTTONUP",         L"OnRButtonUp",      L"(HWND hwnd, int x, int y, UINT flags)", L"void",
	MOUSE,      L"WM_MBUTTONDOWN",       L"OnMButtonDown",    L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_MBUTTONDBLCLK",     L"OnMButtonDblclk",  L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)", L"void",
	MOUSE,      L"WM_MBUTTONUP",         L"OnMButtonUp",      L"(HWND hwnd, int x, int y, UINT flags)", L"void",
	MOUSE,      L"WM_MOUSEWHEEL",        L"OnMouseWheel",     L"(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys)", L"void",
	NONCLIENT,  L"WM_NCMOUSEMOVE",       L"OnNCMouseMove",    L"(HWND hwnd, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCLBUTTONDOWN",     L"OnNCLButtonDown",  L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCLBUTTONDBLCLK",   L"OnNCLButtonDblClk",L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCLBUTTONUP",       L"OnNCLButtonUp",    L"(HWND hwnd, int x, int y, UINT codeHitTest) ", L"void",
	NONCLIENT,  L"WM_NCRBUTTONDOWN",     L"OnNCRButtonDown",  L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCRBUTTONDBLCLK",   L"OnNCRButtonDblClk",L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCRBUTTONUP",       L"OnNCRButtonUp",    L"(HWND hwnd, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCMBUTTONDOWN",     L"OnNCMButtonDown",  L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCMBUTTONDBLCLK",   L"OnNCMButtonDblClk",L"(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)", L"void",
	NONCLIENT,  L"WM_NCMBUTTONUP",       L"OnNCMButtonUp",    L"(HWND hwnd, int x, int y, UINT codeHitTest)", L"void",
	MOUSE,      L"WM_MOUSEACTIVATE",     L"OnMouseActivate",  L"(HWND hwnd, HWND hwndTopLevel, UINT codeHitTest, UINT msg)", L"int",
	GENERAL,    L"WM_CANCELMODE",        L"OnCancelMode",     L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_TIMER",             L"OnTimer",          L"(HWND hwnd, UINT id)", L"void",
	GENERAL,    L"WM_INITMENU",          L"OnInitMenu",       L"(HWND hwnd, HMENU hMenu)", L"void",
	GENERAL,    L"WM_INITMENUPOPUP",     L"OnInitMenuPopup",  L"(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)", L"void",
	GENERAL,    L"WM_MENUSELECT",        L"OnMenuSelect",     L"(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)", L"void",
	GENERAL,    L"WM_MENUCHAR",          L"OnMenuChar",       L"(HWND hwnd, UINT ch, UINT flags, HMENU hmenu)", L"DWORD",
	GENERAL,    L"WM_COMMAND",           L"OnCommand",        L"(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)", L"void",
	GENERAL,    L"WM_HSCROLL",           L"OnHScroll",        L"(HWND hwnd, HWND hwndCtl, UINT code, int pos)", L"void",
	GENERAL,    L"WM_VSCROLL",           L"OnVScroll",        L"(HWND hwnd, HWND hwndCtl, UINT code, int pos)", L"void",
	CLIPBOARD,  L"WM_CUT",               L"OnCut",            L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_COPY",              L"OnCopy",           L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_PASTE",             L"OnPaste",          L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_CLEAR",             L"OnClear",          L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_UNDO",              L"OnUndo",           L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_RENDERFORMAT",      L"OnRenderFormat",   L"(HWND hwnd, UINT fmt)", L"HANDLE",
	CLIPBOARD,  L"WM_RENDERALLFORMATS",  L"OnRenderAllFormats",L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_DESTROYCLIPBOARD",  L"OnDestroyClipboard", L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_DRAWCLIPBOARD",     L"OnDrawClipboard"   , L"(HWND hwnd)", L"void",
	CLIPBOARD,  L"WM_PAINTCLIPBOARD",    L"OnPaintClipboard",   L"(HWND hwnd, HWND hwndCBViewer, const LPPAINTSTRUCT lpPaintStruct)", L"void",
	CLIPBOARD,  L"WM_SIZECLIPBOARD",     L"OnSizeClipboard",    L"(HWND hwnd, HWND hwndCBViewer, const LPRECT lprc)", L"void",
	CLIPBOARD,  L"WM_VSCROLLCLIPBOARD",  L"OnVScrollClipboard", L"(HWND hwnd, HWND hwndCBViewer, UINT code, int pos)", L"void",
	CLIPBOARD,  L"WM_HSCROLLCLIPBOARD",  L"OnHScrollClipboard", L"(HWND hwnd, HWND hwndCBViewer, UINT code, int pos)", L"void",
	CLIPBOARD,  L"WM_ASKCBFORMATNAME",   L"OnAskCBFormatName",    L"(HWND hwnd, int cchMax, LPTSTR rgchName)", L"void",
	CLIPBOARD,  L"WM_CHANGECBCHAIN",     L"OnChangeCBChain",      L"(HWND hwnd, HWND hwndRemove, HWND hwndNext)", L"void",
	MOUSE,      L"WM_SETCURSOR",         L"OnSetCursor",          L"(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)", L"BOOL",
	GENERAL,    L"WM_SYSCOMMAND",        L"OnSysCommand",         L"(HWND hwnd, UINT cmd, int x, int y)", L"void",
	MDI,        L"WM_MDICREATE",         L"OnMDICreate",          L"(HWND hwnd, const LPMDICREATESTRUCT lpmcs)", L"HWND",
	MDI,        L"WM_MDIDESTROY",        L"OnMDIDestroy",         L"(HWND hwnd, HWND hwndDestroy)", L"void",
	MDI,        L"WM_MDIACTIVATE",       L"OnMDIActivate",        L"(HWND hwnd, BOOL fActive, HWND hwndActivate, HWND hwndDeactivate)", L"void",
	MDI,        L"WM_MDIRESTORE",        L"OnMDIRestore",         L"(HWND hwnd, HWND hwndRestore)",L"void",
	MDI,        L"WM_MDINEXT",           L"OnMDINext",            L"(HWND hwnd, HWND hwndCur, BOOL fPrev)", L"HWND",
	MDI,        L"WM_MDIMAXIMIZE",       L"OnMDIMaximize",        L"(HWND hwnd, HWND hwndMaximize)", L"void",
	MDI,        L"WM_MDITILE",           L"OnMDITile",            L"(HWND hwnd, UINT cmd)", L"BOOL",
	MDI,        L"WM_MDICASCADE",        L"OnMDICascade",         L"(HWND hwnd, UINT cmd)", L"BOOL",
	MDI,        L"WM_MDIICONARRANGE",    L"OnMDIIconArrange",     L"(HWND hwnd)", L"void",
	MDI,        L"WM_MDIGETACTIVE",      L"OnMDIGetActive",       L"(HWND hwnd)", L"HWND",
	MDI,        L"WM_MDISETMENU",        L"OnMDISetMenu",         L"(HWND hwnd, BOOL fRefresh, HMENU hmenuFrame, HMENU hmenuWindow)", L"HMENU",
	GENERAL,    L"WM_CHILDACTIVATE",     L"OnChildActivate",      L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_INITDIALOG",        L"OnInitDialog",         L"(HWND hwnd, HWND hwndFocus, LPARAM lParam)", L"BOOL",
	GENERAL,    L"WM_NEXTDLGCTL",        L"OnNextDlgCtl",         L"(HWND hwnd, HWND hwndSetFocus, BOOL fNext)", L"HWND",
	MOUSE,      L"WM_PARENTNOTIFY",      L"OnParentNotify",       L"(HWND hwnd, UINT msg, HWND hwndChild, int idChild)", L"void",
	GENERAL,    L"WM_ENTERIDLE",         L"OnEnterIdle",          L"(HWND hwnd, UINT source, HWND hwndSource)", L"void",
	GENERAL,    L"WM_GETDLGCODE",        L"OnGetDlgCode",         L"(HWND hwnd, LPMSG lpmsg)", L"UINT",
	GENERAL,    L"WM_CTLCOLORBTN",       L"OnCtlColorBtn",        L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_CTLCOLORDLG",       L"OnCtlColorDlg",        L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_CTLCOLOREDIT",      L"OnCtlColorEdit",       L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_CTLCOLORLISTBOX",   L"OnCtlColorListbox",    L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_CTLCOLORMSGBOX",    L"OnCtlColorMsgbox",     L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_CTLCOLORSCROLLBAR", L"OnCtlColorScrollbar",  L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_CTLCOLORSTATIC",    L"OnCtlColorStatic",     L"(HWND hwnd, HDC hdc, HWND hwndChild, int type)", L"HBRUSH",
	GENERAL,    L"WM_SETFONT",           L"OnSetFont",            L"(HWND hwndCtl, HFONT hfont, BOOL fRedraw)", L"void",
	GENERAL,    L"WM_GETFONT",           L"OnGetFont",            L"(HWND hwnd)", L"HFONT",
	GENERAL,    L"WM_DRAWITEM",          L"OnDrawItem",           L"(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)", L"void",
	GENERAL,    L"WM_MEASUREITEM",       L"OnMeasureItem",        L"(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)", L"void",
	GENERAL,    L"WM_DELETEITEM",        L"OnDeleteItem",         L"(HWND hwnd, const DELETEITEMSTRUCT * lpDeleteItem)", L"void",
	GENERAL,    L"WM_COMPAREITEM",       L"OnCompareItem",        L"(HWND hwnd, const COMPAREITEMSTRUCT * lpCompareItem)", L"int",
	GENERAL,    L"WM_QUEUESYNC",         L"OnQueueSync",          L"(HWND hwnd)", L"void",
	GENERAL,    L"WM_COMMNOTIFY",        L"OnCommNotify",         L"(HWND hwnd, int cid, UINT flags)", L"void",
	GENERAL,    L"WM_DISPLAYCHANGE",     L"OnDisplayChange",      L"(HWND hwnd, UINT bitsPerPixel, UINT cxScreen, UINT cyScreen)", L"void",
	GENERAL,    L"WM_DEVICECHANGE",      L"OnDeviceChange",       L"(HWND hwnd, UINT uEvent, DWORD dwEventData)", L"BOOL",
	GENERAL,    L"WM_CONTEXTMENU",       L"OnContextMenu",        L"(HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos)", L"void",
	GENERAL,    L"WM_COPYDATA",			 L"OnCopyData",			  L"(HWND hwnd, HWND hwndFrom, PCOPYDATASTRUCT pcds)", L"BOOL"
};
//
// count elements of msginfo structure
//
const int  NUMMSG = sizeof(msginfo) / sizeof(msginfo[0]);

// array for selected messages in the list box

extern int g_nItems;

#endif // !MSGCRACKWIZARD_H
