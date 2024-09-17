# Message Cracker Wizard
A tool for aiding Win32 C development using WINDOWSX.H message crackers.

![Image of MessageCrackerWizard](https://github.com/hernandp/MessageCrackerWizard/blob/master/bmp/mcw.png)

## Introduction

This is the source code, MIT licensed now, of my old ClassWizard-like tool to working with WINDOWSX.H message cracker macros. It's intended as you imagine, for raw-C and C++ programmers developing Win32 native applications.

For a detailed description of this program, check out the original CodeProject article at:

https://www.codeproject.com/Articles/4948/Message-Cracker-Wizard-for-Win-SDK-Developers

Suggestions are welcome.

## Version History

* 1.0
  * First release, Sep, 2003.
* 1.2
  * Added multiple selection feature.
  * Added missing crackers for WM_COPYDATA and WM_HOTKEY messages.
  * Fixed little interface bugs.
* 2.0
  * Added message filtering.
  * Added window transparency option (only for Windows 2000/XP/Server 2003).
  * Added show/hide Target code.
  * Added enable/disable stay on top window.
  * Added WM_CTLCOLORxxxx message support.
  * Added message-type bitmaps on list box.
  * Added include/exclude comments option.
  * Fixed keyboard logic.
* 2.1
  * Fixed clipboard copy bug (thanks to Agnel Kurian).
  * Now the program is licensed under the GPL.
* 2.2
  * C++ code modernization to near- modern standards
  * Updated for compilation with VS2015
  * Changed to Unicode for all strings
  * Fixed analysis, warnings, and related issues
  * Changed to MIT license
* 2.5
  * Fix memory corruption error 
  * Fix non-dialog procedure generating 'hwndDlg' instead of 'hwnd' 
  * Added re-sizable window feature.
  * Added 'Dark Mode' color scheme (WIP)
  * Settings are saved/retrieved  to/from Registry
  * Removed 'Hide Target Code' option
* 2.6
  * Added Search in listbox feature, clear selections and enhancements (thanks @HHsomeHand)
  * fix MFC-dependent afxres.h in resource file
  * Solved Misc build issues for latest  VC environment.
