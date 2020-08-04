// This file is part of Notepad++ project
// Copyright (C)2003 Don HO <don.h@free.fr>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// Note that the GPL places important restrictions on "derived works", yet
// it does not provide a detailed definition of that term.  To avoid      
// misunderstandings, we consider an application to constitute a          
// "derivative work" for the purpose of this license if it does any of the
// following:                                                             
// 1. Integrates source code from Notepad++.
// 2. Integrates/includes/aggregates Notepad++ into a proprietary executable
//    installer, such as those produced by InstallShield.
// 3. Links to a library or executes a program that does any of the above.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#pragma once

#include "ControlsTab.h"
#include "preference_rc.h"
#include "URLCtrl.h"
#include "Parameters.h"
#include "regExtDlg.h"
#include "WordStyleDlg.h"

class SettingsDlg : public StaticDialog
{
public :
	SettingsDlg() {};

private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class BarsDlg : public StaticDialog
{
public :
	BarsDlg() {};
private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class MarginsDlg : public StaticDialog
{
public :
	MarginsDlg() {};
	virtual void destroy() {
		_verticalEdgeLineNbColVal.destroy();
	};
	
private :
	URLCtrl _verticalEdgeLineNbColVal;
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	void initScintParam();
};

struct LangID_Name
{
	LangType _id;
	generic_string _name;
	LangID_Name(LangType id, generic_string name) : _id(id), _name(name){};
};

class DefaultNewDocDlg : public StaticDialog
{
public :
	DefaultNewDocDlg() {};

private :
	std::vector<LangID_Name> _langList;
	void makeOpenAnsiAsUtf8(bool doIt){
		if (!doIt)
			::SendDlgItemMessage(_hSelf, IDC_CHECK_OPENANSIASUTF8, BM_SETCHECK, BST_UNCHECKED, 0);
		::EnableWindow(::GetDlgItem(_hSelf, IDC_CHECK_OPENANSIASUTF8), doIt);
	};
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class DefaultDirectoryDlg : public StaticDialog
{
public :
	DefaultDirectoryDlg() {};

private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class RecentFilesHistoryDlg : public StaticDialog
{
public :
	RecentFilesHistoryDlg() {};
	virtual void destroy() {
		_nbHistoryVal.destroy();
		_customLenVal.destroy();
	};
private :
	URLCtrl _nbHistoryVal;
	URLCtrl _customLenVal;
	std::vector<LangID_Name> _langList;
	void setCustomLen(int val);
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class LangMenuDlg : public StaticDialog
{
public :
	LangMenuDlg() {};
	virtual void destroy() {
		_tabSizeVal.destroy();
	};

private :
    LexerStylerArray _lsArray;
	URLCtrl _tabSizeVal;
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	std::vector<LangMenuItem> _langList;
};

class Highlighting : public StaticDialog
{
public :
	Highlighting() {};

private :

	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};


struct strCouple {
	generic_string _varDesc;
	generic_string _var;
	strCouple(TCHAR *varDesc, TCHAR *var): _varDesc(varDesc), _var(var){};
};

class PrintSettingsDlg : public StaticDialog
{
public :
	PrintSettingsDlg():_focusedEditCtrl(0), _selStart(0), _selEnd(0){};
private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	std::vector<strCouple> varList;
	int _focusedEditCtrl;
	DWORD _selStart;
	DWORD _selEnd;
};

class BackupDlg : public StaticDialog
{
public :
	BackupDlg() {};
private :
	void updateBackupGUI();
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};


class AutoCompletionDlg : public StaticDialog
{
public :
	AutoCompletionDlg() {};
private :
	URLCtrl _nbCharVal;
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class MultiInstDlg : public StaticDialog
{
public :
	MultiInstDlg() {};

private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class DelimiterSettingsDlg : public StaticDialog
{
public :
	DelimiterSettingsDlg() {};
	~DelimiterSettingsDlg() {
		if (_tip)
			::DestroyWindow(_tip);
	};

private :
	POINT _singleLineModePoint, _multiLineModePoint;
	RECT _closerRect, _closerLabelRect;
	HWND _tip = nullptr;

	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	void detectSpace(const char *text2Check, int & nbSp, int & nbTab) const;
	generic_string getWarningText(size_t nbSp, size_t nbTab) const;
	void setWarningIfNeed() const;
};

class SettingsOnCloudDlg : public StaticDialog
{
public :
	SettingsOnCloudDlg() {};

private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class SearchEngineChoiceDlg : public StaticDialog
{
public :
	SearchEngineChoiceDlg() {};

private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class PreferenceDlg : public StaticDialog
{
friend class NativeLangSpeaker;

public :
	PreferenceDlg(){};

    void init(HINSTANCE hInst, HWND parent)	{
        Window::init(hInst, parent);
	};

    void doDialog(bool isRTL = false) {
    	if (!isCreated())
		{
			create(IDD_PREFERENCE_BOX, isRTL);
			goToCenter();
		}
	    display();
    };
	bool renameDialogTitle(const TCHAR *internalName, const TCHAR *newName);
	
	int getListSelectedIndex() const {
		return static_cast<int32_t>(::SendDlgItemMessage(_hSelf, IDC_LIST_DLGTITLE, LB_GETCURSEL, 0, 0));
	};

	void showDialogByName(const TCHAR *name) const;
	void setListSelection(size_t currentSel) const;

	virtual void destroy();

private :
	INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);
	void makeCategoryList();
	int32_t getIndexFromName(const TCHAR *name) const;
	void showDialogByIndex(size_t index) const;
	WindowVector _wVector;
	BarsDlg _barsDlg;
	MarginsDlg _marginsDlg;
	SettingsDlg _settingsDlg;
	RegExtDlg _fileAssocDlg;
	LangMenuDlg _langMenuDlg;
	Highlighting _highlighting;
	PrintSettingsDlg _printSettingsDlg;
	DefaultNewDocDlg _defaultNewDocDlg;
	DefaultDirectoryDlg	_defaultDirectoryDlg;
	RecentFilesHistoryDlg _recentFilesHistoryDlg;
	BackupDlg _backupDlg;
	AutoCompletionDlg _autoCompletionDlg;
	MultiInstDlg _multiInstDlg;
	DelimiterSettingsDlg _delimiterSettingsDlg;
	SettingsOnCloudDlg _settingsOnCloudDlg;
	SearchEngineChoiceDlg _searchEngineDlg;
};

