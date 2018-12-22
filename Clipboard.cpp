/*****************************************************************

MIT License

This file is part of Message Cracker Wizard

Copyright (c) 2003-2017 Hern? Di Pietro

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

//
// Copia la macro al clipboard
//
void CopyMacroToCB(HWND hwnd)
{
	const std::wstring	windowProcHeader = L"_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)\r\n";
	const std::wstring	dialogProcHeader = L"_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)\r\n";

	auto windowIdTextLen = GetWindowTextLength(GetDlgItem(hwnd, IDC_WINDOWID));
	auto handleMsgTextLen = GetWindowTextLength(GetDlgItem(hwnd, IDC_HANDLEMSG));
	auto WindowID = std::make_unique<wchar_t[]>(windowIdTextLen + 1);
	auto macroText = std::make_unique<wchar_t[]>(handleMsgTextLen + 1);

	GetDlgItemText(hwnd, IDC_WINDOWID, WindowID.get(), windowIdTextLen + 1);
	GetDlgItemText(hwnd, IDC_HANDLEMSG, macroText.get(), handleMsgTextLen + 1);

	int selItems[NUMMSG];
	int nItems = (int) SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETSELITEMS, NUMMSG, (LPARAM) &selItems);

	std::wstring clipText;

	if (IsDlgButtonChecked(hwnd, IDC_MAKEWNDPROC) == BST_CHECKED)
	{
		// construir todo el wndproc
		if (IsDlgButtonChecked(hwnd, IDC_RADWINDOW) == BST_CHECKED)
		{
			// skip comments if checkbox is enabled
			if (IsDlgButtonChecked(hwnd, IDC_NOHEADINGCOMMENTS) == BST_UNCHECKED)
			{
				clipText += L"//\r\n// " + std::wstring(WindowID.get()) + L"  Window Procedure\r\n//\r\n";;
			}

			clipText += L"LRESULT CALLBACK " + std::wstring(WindowID.get()) + windowProcHeader + L"{\r\n\tswitch(msg)\r\n\t{\r\n";

			for (int i = 0; i < nItems; i++)
			{
				// now send the message to get the line text

				auto lineIndex = Edit_LineIndex(GetDlgItem(hwnd, IDC_HANDLEMSG), i);
				int lineLength = SendDlgItemMessage(hwnd, IDC_HANDLEMSG, EM_LINELENGTH, lineIndex, 0);
				auto lineBuf = std::make_unique<wchar_t[]>(lineLength + 1);

				memset(lineBuf.get(), 0, (lineLength + 1) * sizeof(WCHAR));
				*((WORD*) lineBuf.get()) = (lineLength + 1) * sizeof(WCHAR);

				SendDlgItemMessage(hwnd, IDC_HANDLEMSG, EM_GETLINE, i, (LPARAM) lineBuf.get());

				clipText += L"\t" + std::wstring(lineBuf.get()) + L"\r\n";
			}

			if (IsDlgButtonChecked(hwnd, IDC_NOTODOCOMMENTS) == BST_UNCHECKED)
			{
				clipText += L"\r\n\t\t//// TODO: Add window message crackers here...\r\n";
			}
			clipText += L"\r\n\tdefault: return DefWindowProc (hwnd, msg, wParam, lParam);\r\n\t}\r\n}";
		}

		// Dialog Procedure

		else if (IsDlgButtonChecked(hwnd, IDC_DIALOG) == BST_CHECKED)
		{
			// DLGPROC (modal)

			// skip comments?
			if (IsDlgButtonChecked(hwnd, IDC_NOHEADINGCOMMENTS) == BST_UNCHECKED)
			{
				clipText += L"//\r\n// " + std::wstring(WindowID.get()) + L"  Dialog Procedure\r\n//\r\n";
			}
			clipText += L"BOOL CALLBACK " + std::wstring(WindowID.get()) + dialogProcHeader + L"{\r\n\tswitch(msg)\r\n\t{\r\n";

			for (int i = 0; i < nItems; i++)
			{
				auto lineIndex = Edit_LineIndex(GetDlgItem(hwnd, IDC_HANDLEMSG), i);
				int lineLength = SendDlgItemMessage(hwnd, IDC_HANDLEMSG, EM_LINELENGTH, lineIndex, 0);
				auto lineBuf = std::make_unique<wchar_t[]>(lineLength + 1);

				memset(lineBuf.get(), 0, (lineLength + 1) * sizeof(WCHAR));
				*((WORD*) lineBuf.get()) = lineLength;

				SendDlgItemMessage(hwnd, IDC_HANDLEMSG, EM_GETLINE, i, (LPARAM) lineBuf.get());

				clipText += L"\t" + std::wstring(lineBuf.get()) + L"\r\n";
			}

			if (IsDlgButtonChecked(hwnd, IDC_NOTODOCOMMENTS) == BST_UNCHECKED)
			{
				clipText += L"\r\n\t\t//// TODO: Add dialog message crackers here...\r\n";
			}
			clipText += L"\r\n\tdefault: return FALSE;\r\n\t}\r\n}";
		}
	}
	else
	{
		clipText += macroText.get();
	}

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * clipText.length() + sizeof(WCHAR));
	if (hGlobal)
	{
		wchar_t* pGlobalStr = (wchar_t*) GlobalLock(hGlobal);
		if (pGlobalStr)
		{
			StringCchCopy(pGlobalStr, clipText.length() + sizeof(wchar_t), clipText.c_str());
			GlobalUnlock(hGlobal);

			OpenClipboard(hwnd);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hGlobal);
			CloseClipboard();
		}
	}
	else
	{
		MessageBox(NULL, L"Cannot allocate memory for clipboard text", L"Error", MB_ICONERROR);
	}
}

void CopyFuncToCB(HWND hwnd)
{
	HGLOBAL		hGlobal;
	wchar_t		WindowID[MAX_WINDOWID_STR_LEN];

	// store Window ID, function header and message name

	GetDlgItemText(hwnd, IDC_WINDOWID, WindowID, _countof(WindowID));

	std::wstring clipText;

	int selItems[NUMMSG] = { 0 };
	int nItems = (int) SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETSELITEMS, NUMMSG, (LPARAM) &selItems);

	for (int i = 0; i < nItems; i++)
	{
		int textLen = SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETTEXTLEN, selItems[i], NULL);
		auto szMessage = std::make_unique<wchar_t[]>(textLen + 1);
		SendDlgItemMessage(hwnd, IDC_MESSAGES, LB_GETTEXT, selItems[i], (LPARAM) szMessage.get());

		// now send the message to get the line text
		auto lineIndex = Edit_LineIndex(GetDlgItem(hwnd, IDC_MSGFUNCTION), i);
		int lineLength = SendDlgItemMessage(hwnd, IDC_MSGFUNCTION, EM_LINELENGTH, lineIndex, 0);
		auto lineBuf = std::make_unique<wchar_t[]>(lineLength + 1);

		memset(lineBuf.get(), 0, (lineLength + 1) * sizeof(WCHAR));
		*((WORD*) lineBuf.get()) = lineLength;

		SendDlgItemMessage(hwnd, IDC_MSGFUNCTION, EM_GETLINE, (WPARAM) i, (LPARAM) lineBuf.get());

		// skip comments?
		if (IsDlgButtonChecked(hwnd, IDC_NOHEADINGCOMMENTS) == BST_UNCHECKED)
		{
			clipText += L"//\r\n//  Process " + std::wstring(szMessage.get()) + L" message for window/dialog: " + WindowID + L"\r\n//\r\n";
		}
		clipText += std::wstring(lineBuf.get()) + L"\r\n{\r\n";

		if (IsDlgButtonChecked(hwnd, IDC_NOTODOCOMMENTS) == BST_UNCHECKED)
		{
			clipText += L"\t// TODO: Add your message processing code here...";
		}
		clipText += L"\r\n}\r\n\r\n";
	}

	// Copy to clipboard

	hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * clipText.length() + sizeof(WCHAR));
	if (hGlobal)
	{
		wchar_t* pGlobalStr = (wchar_t*) GlobalLock(hGlobal);
		if (pGlobalStr)
		{
			StringCchCopy(pGlobalStr, clipText.length() + sizeof(wchar_t), clipText.c_str());
			GlobalUnlock(hGlobal);

			if (OpenClipboard(hwnd))
			{
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hGlobal);
				CloseClipboard();
			}
			else
			{
				MessageBox(NULL, L"Cannot open clipboard", L"Error", MB_ICONERROR);
			}
		}
	}
	else
	{
		MessageBox(NULL, L"Cannot allocate memory for clipboard text", L"Error", MB_ICONERROR);
	}
}