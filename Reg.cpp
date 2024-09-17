/*****************************************************************

MIT License

This file is part of Message Cracker Wizard

Copyright (c) 2003, 2017, 2018, 2024 Hernán Di Pietro

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
#include "MsgCrackWizard.h"

void LoadConfig(MCWCONFIG& mcw)
{
    HKEY hk;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, 
        L"Software\\MessageCrackerWizard", NULL, NULL, NULL, KEY_READ, NULL, &hk, NULL) == ERROR_SUCCESS)
    {
        mcw.rcWindow = { -1,-1,-1,-1 };

        DWORD cbData = 0;
        DWORD dwDarkMode = 0, dwStayTop = -1, dwWindowAlpha = 255;
        RegQueryValueEx(hk, L"wRect.Top", NULL, NULL, (BYTE*)&mcw.rcWindow.top, &cbData);
        RegQueryValueEx(hk, L"wRect.Left", NULL, NULL, (BYTE*)&mcw.rcWindow.left, &cbData);
        RegQueryValueEx(hk, L"wRect.Right", NULL, NULL, (BYTE*)&mcw.rcWindow.right, &cbData);
        RegQueryValueEx(hk, L"wRect.Bottom", NULL, NULL, (BYTE*)&mcw.rcWindow.bottom, &cbData);
        RegQueryValueEx(hk, L"DarkMode", NULL, NULL, (BYTE*)&dwDarkMode, &cbData);
        RegQueryValueEx(hk, L"StayOnTop", NULL, NULL, (BYTE*)&dwStayTop, &cbData);
        RegQueryValueEx(hk, L"WindowAlpha", NULL, NULL, (BYTE*)&dwWindowAlpha, &cbData);

        mcw.bDarkMode = dwDarkMode == 0 ? false : true;
        mcw.bStayOnTop = dwStayTop == 0 ? false : true;
        mcw.windowAlpha = dwWindowAlpha;

        RegCloseKey(hk);
    }
    else
    {
        MessageBox(NULL, L"Cannot load or create MessageCrackerWizard registry entries", 
            L"Message Cracker Wizard", MB_OK | MB_SETFOREGROUND | MB_ICONWARNING);
    }
}

void SaveConfig(const MCWCONFIG& mcw)
{
    HKEY hk;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
        L"Software\\MessageCrackerWizard", NULL, NULL, NULL, KEY_WRITE, NULL, &hk, NULL) == ERROR_SUCCESS)
    {
        DWORD dwDarkMode = mcw.bDarkMode ? 1 :0;
        DWORD dwStayOnTop = mcw.bStayOnTop ? 1 : 0;

        RegSetValueEx(hk, L"wRect.Top", NULL, REG_DWORD, (BYTE*)&mcw.rcWindow.top, sizeof(DWORD));
        RegSetValueEx(hk, L"wRect.Left", NULL, REG_DWORD, (BYTE*)&mcw.rcWindow.left, sizeof(DWORD));
        RegSetValueEx(hk, L"wRect.Right", NULL, REG_DWORD, (BYTE*)&mcw.rcWindow.right, sizeof(DWORD));
        RegSetValueEx(hk, L"wRect.Bottom", NULL, REG_DWORD, (BYTE*)&mcw.rcWindow.bottom, sizeof(DWORD));
        RegSetValueEx(hk, L"DarkMode", NULL, REG_DWORD, (BYTE*)&dwDarkMode, sizeof(DWORD));
        RegSetValueEx(hk, L"StayOnTop", NULL, REG_DWORD, (BYTE*)&dwStayOnTop, sizeof(DWORD));
        RegSetValueEx(hk, L"WindowAlpha", NULL, REG_DWORD, (BYTE*)&mcw.windowAlpha, sizeof(DWORD));

        RegCloseKey(hk);
    }
    else
    {
        MessageBox(NULL, L"Cannot load or create MessageCrackerWizard registry entries",
            L"Message Cracker Wizard", MB_OK | MB_SETFOREGROUND | MB_ICONWARNING);
    }
}