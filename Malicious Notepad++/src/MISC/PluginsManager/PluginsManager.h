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

#include "resource.h"
#include "Parameters.h"
#include "PluginInterface.h"
#include "IDAllocator.h"

typedef BOOL (__cdecl * PFUNCISUNICODE)();

struct PluginCommand
{
	generic_string _pluginName;
	int _funcID;
	PFUNCPLUGINCMD _pFunc;
	PluginCommand(const TCHAR *pluginName, int funcID, PFUNCPLUGINCMD pFunc): _funcID(funcID), _pFunc(pFunc), _pluginName(pluginName){};
};

struct PluginInfo
{
	PluginInfo() {}
	~PluginInfo()
	{
		if (_pluginMenu)
			::DestroyMenu(_pluginMenu);

		if (_hLib)
			::FreeLibrary(_hLib);
	}

	HINSTANCE _hLib = NULL;
	HMENU _pluginMenu = NULL;

	PFUNCSETINFO _pFuncSetInfo = NULL;
	PFUNCGETNAME _pFuncGetName = NULL;
	PBENOTIFIED	_pBeNotified = NULL;
	PFUNCGETFUNCSARRAY _pFuncGetFuncsArray = NULL;
	PMESSAGEPROC _pMessageProc = NULL;
	PFUNCISUNICODE _pFuncIsUnicode = NULL;

	FuncItem *_funcItems = NULL;
	int _nbFuncItem = 0;
	generic_string _moduleName;
	generic_string _funcName;
};

struct LoadedDllInfo
{
	generic_string _fullFilePath;
	generic_string _fileName;

	LoadedDllInfo(const generic_string & fullFilePath, const generic_string & fileName) : _fullFilePath(fullFilePath), _fileName(fileName) {};
};

class PluginsManager
{
friend class PluginsAdminDlg;
public:
	PluginsManager() : _dynamicIDAlloc(ID_PLUGINS_CMD_DYNAMIC, ID_PLUGINS_CMD_DYNAMIC_LIMIT),
					   _markerAlloc(MARKER_PLUGINS, MARKER_PLUGINS_LIMIT)	{}
	~PluginsManager()
	{
		for (size_t i = 0, len = _pluginInfos.size(); i < len; ++i)
			delete _pluginInfos[i];

		if (_hPluginsMenu)
			DestroyMenu(_hPluginsMenu);
	}

	void init(const NppData & nppData)
	{
		_nppData = nppData;
	}

    int loadPlugin(const TCHAR *pluginFilePath, std::vector<generic_string> & dll2Remove);
	bool loadPlugins(const TCHAR *dir = NULL);
	bool loadPluginsV2(const TCHAR *dir);

    bool unloadPlugin(int index, HWND nppHandle);

	void runPluginCommand(size_t i);
	void runPluginCommand(const TCHAR *pluginName, int commandID);

    void addInMenuFromPMIndex(int i);
	HMENU setMenu(HMENU hMenu, const TCHAR *menuName);
	bool getShortcutByCmdID(int cmdID, ShortcutKey *sk);

	void notify(const SCNotification *notification);
	void relayNppMessages(UINT Message, WPARAM wParam, LPARAM lParam);
	bool relayPluginMessages(UINT Message, WPARAM wParam, LPARAM lParam);

	HMENU getMenuHandle() const { return _hPluginsMenu; }

	void disable() {_isDisabled = true;}
	bool hasPlugins() {return (_pluginInfos.size()!= 0);}

	bool allocateCmdID(int numberRequired, int *start);
	bool inDynamicRange(int id) { return _dynamicIDAlloc.isInRange(id); }

	bool allocateMarker(int numberRequired, int *start);
	generic_string getLoadedPluginNames() const;

private:
	NppData _nppData;
	HMENU _hPluginsMenu = NULL;

	std::vector<PluginInfo *> _pluginInfos;
	std::vector<PluginCommand> _pluginsCommands;
	std::vector<LoadedDllInfo> _loadedDlls;
	bool _isDisabled = false;
	IDAllocator _dynamicIDAlloc;
	IDAllocator _markerAlloc;

	void pluginCrashAlert(const TCHAR *pluginName, const TCHAR *funcSignature)
	{
		generic_string msg = pluginName;
		msg += TEXT(" just crashed in\r");
		msg += funcSignature;
		::MessageBox(NULL, msg.c_str(), TEXT("Plugin Crash"), MB_OK|MB_ICONSTOP);
	}

	bool isInLoadedDlls(const TCHAR *fn) const
	{
		for (size_t i = 0; i < _loadedDlls.size(); ++i)
			if (generic_stricmp(fn, _loadedDlls[i]._fileName.c_str()) == 0)
				return true;
		return false;
	}

	void addInLoadedDlls(const TCHAR *fullPath, const TCHAR *fn) {
		_loadedDlls.push_back(LoadedDllInfo(fullPath, fn));
	}
};

#define EXT_LEXER_DECL __stdcall

// External Lexer function definitions...
typedef int (EXT_LEXER_DECL *GetLexerCountFn)();
typedef void (EXT_LEXER_DECL *GetLexerNameFn)(unsigned int Index, char *name, int buflength);
typedef void (EXT_LEXER_DECL *GetLexerStatusTextFn)(unsigned int Index, TCHAR *desc, int buflength);
