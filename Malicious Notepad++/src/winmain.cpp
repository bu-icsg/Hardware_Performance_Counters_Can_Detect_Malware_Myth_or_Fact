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

#include "Notepad_plus_Window.h"
#include "Processus.h"
#include "Win32Exception.h"	//Win32 exception
#include "MiniDumper.h"			//Write dump files

typedef std::vector<generic_string> ParamVector;


namespace
{


void allowWmCopydataMessages(Notepad_plus_Window& notepad_plus_plus, const NppParameters* pNppParameters, winVer ver)
{
	#ifndef MSGFLT_ADD
	const DWORD MSGFLT_ADD = 1;
	#endif
	#ifndef MSGFLT_ALLOW
	const DWORD MSGFLT_ALLOW = 1;
	#endif
	// Tell UAC that lower integrity processes are allowed to send WM_COPYDATA messages to this process (or window)
	// This allows opening new files to already opened elevated Notepad++ process via explorer context menu.
	if (ver >= WV_VISTA || ver == WV_UNKNOWN)
	{
		HMODULE hDll = GetModuleHandle(TEXT("user32.dll"));
		if (hDll)
		{
			// According to MSDN ChangeWindowMessageFilter may not be supported in future versions of Windows,
			// that is why we use ChangeWindowMessageFilterEx if it is available (windows version >= Win7).
			if (pNppParameters->getWinVersion() == WV_VISTA)
			{
				typedef BOOL (WINAPI *MESSAGEFILTERFUNC)(UINT message,DWORD dwFlag);

				MESSAGEFILTERFUNC func = (MESSAGEFILTERFUNC)::GetProcAddress( hDll, "ChangeWindowMessageFilter" );

				if (func)
					func(WM_COPYDATA, MSGFLT_ADD);
			}
			else
			{
				typedef BOOL (WINAPI *MESSAGEFILTERFUNCEX)(HWND hWnd,UINT message,DWORD action,VOID* pChangeFilterStruct);

				MESSAGEFILTERFUNCEX func = (MESSAGEFILTERFUNCEX)::GetProcAddress( hDll, "ChangeWindowMessageFilterEx" );

				if (func)
					func(notepad_plus_plus.getHSelf(), WM_COPYDATA, MSGFLT_ALLOW, NULL );
			}
		}
	}
}


bool checkSingleFile(const TCHAR *commandLine)
{
	if (!commandLine || commandLine[0] == TEXT('\0'))
		return false;

	TCHAR fullpath[MAX_PATH] = {0};
	const DWORD fullpathResult = ::GetFullPathName(commandLine, MAX_PATH, fullpath, NULL);

	if (fullpathResult == 0)
		return false;

	if (fullpathResult > MAX_PATH)
		return false;

	if (::PathFileExists(fullpath))
		return true;

	return false;
}

//commandLine should contain path to n++ executable running
void parseCommandLine(const TCHAR* commandLine, ParamVector& paramVector)
{
	if (!commandLine)
		return;

	TCHAR* cmdLine = new TCHAR[lstrlen(commandLine) + 1];
	lstrcpy(cmdLine, commandLine);

	TCHAR* cmdLinePtr = cmdLine;

	//remove the first element, since thats the path the the executable (GetCommandLine does that)
	TCHAR stopChar = TEXT(' ');
	if (cmdLinePtr[0] == TEXT('\"'))
	{
		stopChar = TEXT('\"');
		++cmdLinePtr;
	}
	//while this is not really DBCS compliant, space and quote are in the lower 127 ASCII range
	while(cmdLinePtr[0] && cmdLinePtr[0] != stopChar)
    {
		++cmdLinePtr;
    }

    // For unknown reason, the following command :
    // c:\NppDir>notepad++
    // (without quote) will give string "notepad++\0notepad++\0"
    // To avoid the unexpected behaviour we check the end of string before increasing the pointer
    if (cmdLinePtr[0] != '\0')
	    ++cmdLinePtr;	//advance past stopChar

	//kill remaining spaces
	while(cmdLinePtr[0] == TEXT(' '))
		++cmdLinePtr;

	bool isFile = checkSingleFile(cmdLinePtr);	//if the commandline specifies only a file, open it as such
	if (isFile)
	{
		paramVector.push_back(cmdLinePtr);
		delete[] cmdLine;
		return;
	}
	bool isInFile = false;
	bool isInWhiteSpace = true;
	size_t commandLength = lstrlen(cmdLinePtr);
	std::vector<TCHAR *> args;
	for (size_t i = 0; i < commandLength; ++i)
	{
		switch(cmdLinePtr[i])
		{
			case '\"': //quoted filename, ignore any following whitespace
			{
				if (!isInFile)	//" will always be treated as start or end of param, in case the user forgot to add an space
				{
					args.push_back(cmdLinePtr+i+1);	//add next param(since zero terminated original, no overflow of +1)
				}
				isInFile = !isInFile;
				isInWhiteSpace = false;
				//because we dont want to leave in any quotes in the filename, remove them now (with zero terminator)
				cmdLinePtr[i] = 0;
			}
			break;

			case '\t': //also treat tab as whitespace
			case ' ': 
			{
				isInWhiteSpace = true;
				if (!isInFile)
					cmdLinePtr[i] = 0;		//zap spaces into zero terminators, unless its part of a filename	
			}
			break;

			default: //default TCHAR, if beginning of word, add it
			{
				if (!isInFile && isInWhiteSpace)
				{
					args.push_back(cmdLinePtr+i);	//add next param
					isInWhiteSpace = false;
				}
			}
		}
	}
	paramVector.assign(args.begin(), args.end());
	delete [] cmdLine;
}

bool isInList(const TCHAR *token2Find, ParamVector & params)
{
	size_t nbItems = params.size();

	for (size_t i = 0; i < nbItems; ++i)
	{
		if (!lstrcmp(token2Find, params.at(i).c_str()))
		{
			params.erase(params.begin() + i);
			return true;
		}
	}
	return false;
};

bool getParamVal(TCHAR c, ParamVector & params, generic_string & value)
{
	value = TEXT("");
	size_t nbItems = params.size();

	for (size_t i = 0; i < nbItems; ++i)
	{
		const TCHAR * token = params.at(i).c_str();
		if (token[0] == '-' && lstrlen(token) >= 2 && token[1] == c) {	//dash, and enough chars
			value = (token+2);
			params.erase(params.begin() + i);
			return true;
		}
	}
	return false;
}

bool getParamValFromString(const TCHAR *str, ParamVector & params, generic_string & value)
{
	value = TEXT("");
	size_t nbItems = params.size();

	for (size_t i = 0; i < nbItems; ++i)
	{
		const TCHAR * token = params.at(i).c_str();
		generic_string tokenStr = token;
		size_t pos = tokenStr.find(str);
		if (pos != generic_string::npos && pos == 0)
		{
			value = (token + lstrlen(str));
			params.erase(params.begin() + i);
			return true;
		}
	}
	return false;
}

LangType getLangTypeFromParam(ParamVector & params)
{
	generic_string langStr;
	if (!getParamVal('l', params, langStr))
		return L_EXTERNAL;
	return NppParameters::getLangIDFromStr(langStr.c_str());
}

generic_string getLocalizationPathFromParam(ParamVector & params)
{
	generic_string locStr;
	if (!getParamVal('L', params, locStr))
		return TEXT("");
	return NppParameters::getLocPathFromStr(locStr.c_str());
}

int getNumberFromParam(char paramName, ParamVector & params, bool & isParamePresent) {
	generic_string numStr;
	if (!getParamVal(paramName, params, numStr))
	{
		isParamePresent = false;
		return -1;
	}
	isParamePresent = true;
	return generic_atoi(numStr.c_str());
};

generic_string getEasterEggNameFromParam(ParamVector & params, unsigned char & type)
{
	generic_string EasterEggName;
	if (!getParamValFromString(TEXT("-qn"), params, EasterEggName))  // get internal easter egg
	{
		if (!getParamValFromString(TEXT("-qt"), params, EasterEggName)) // get user quote from cmdline argument
		{
			if (!getParamValFromString(TEXT("-qf"), params, EasterEggName)) // get user quote from a content of file
				return TEXT("");
			else
			{
				EasterEggName = relativeFilePathToFullFilePath(EasterEggName.c_str());
				type = 2; // quote content in file
			}
		}
		else
			type = 1; // commandline quote
	}
	else
		type = 0; // easter egg

	generic_string percentTwentyStr = TEXT("%20");
	generic_string spaceStr = TEXT(" ");
	size_t start_pos = 0;
	while ((start_pos = EasterEggName.find(percentTwentyStr, start_pos)) != std::string::npos)
	{
		EasterEggName.replace(start_pos, percentTwentyStr.length(), spaceStr);
		start_pos += spaceStr.length(); // Handles case where 'to' is a substring of 'from'
	}

	return EasterEggName;
}

const TCHAR FLAG_MULTI_INSTANCE[] = TEXT("-multiInst");
const TCHAR FLAG_NO_PLUGIN[] = TEXT("-noPlugin");
const TCHAR FLAG_READONLY[] = TEXT("-ro");
const TCHAR FLAG_NOSESSION[] = TEXT("-nosession");
const TCHAR FLAG_NOTABBAR[] = TEXT("-notabbar");
const TCHAR FLAG_SYSTRAY[] = TEXT("-systemtray");
const TCHAR FLAG_LOADINGTIME[] = TEXT("-loadingTime");
const TCHAR FLAG_HELP[] = TEXT("--help");
const TCHAR FLAG_ALWAYS_ON_TOP[] = TEXT("-alwaysOnTop");
const TCHAR FLAG_OPENSESSIONFILE[] = TEXT("-openSession");
const TCHAR FLAG_RECURSIVE[] = TEXT("-r");
const TCHAR FLAG_FUNCLSTEXPORT[] = TEXT("-export=functionList");
const TCHAR FLAG_PRINTANDQUIT[] = TEXT("-quickPrint");


void doException(Notepad_plus_Window & notepad_plus_plus)
{
	Win32Exception::removeHandler();	//disable exception handler after excpetion, we dont want corrupt data structurs to crash the exception handler
	::MessageBox(Notepad_plus_Window::gNppHWND, TEXT("Notepad++ will attempt to save any unsaved data. However, dataloss is very likely."), TEXT("Recovery initiating"), MB_OK | MB_ICONINFORMATION);

	TCHAR tmpDir[1024];
	GetTempPath(1024, tmpDir);
	generic_string emergencySavedDir = tmpDir;
	emergencySavedDir += TEXT("\\N++RECOV");

	bool res = notepad_plus_plus.emergency(emergencySavedDir);
	if (res)
	{
		generic_string displayText = TEXT("Notepad++ was able to successfully recover some unsaved documents, or nothing to be saved could be found.\r\nYou can find the results at :\r\n");
		displayText += emergencySavedDir;
		::MessageBox(Notepad_plus_Window::gNppHWND, displayText.c_str(), TEXT("Recovery success"), MB_OK | MB_ICONINFORMATION);
	}
	else
		::MessageBox(Notepad_plus_Window::gNppHWND, TEXT("Unfortunatly, Notepad++ was not able to save your work. We are sorry for any lost data."), TEXT("Recovery failure"), MB_OK | MB_ICONERROR);
}


} // namespace




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	LPTSTR cmdLine = ::GetCommandLine();
	ParamVector params;
	parseCommandLine(cmdLine, params);

	MiniDumper mdump;	//for debugging purposes.

	bool TheFirstOne = true;
	::SetLastError(NO_ERROR);
	::CreateMutex(NULL, false, TEXT("nppInstance"));
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
		TheFirstOne = false;

	bool isParamePresent;
	bool showHelp = isInList(FLAG_HELP, params);
	bool isMultiInst = isInList(FLAG_MULTI_INSTANCE, params);
	bool doFunctionListExport = isInList(FLAG_FUNCLSTEXPORT, params);
	bool doPrintAndQuit = isInList(FLAG_PRINTANDQUIT, params);

	CmdLineParams cmdLineParams;
	cmdLineParams._isNoTab = isInList(FLAG_NOTABBAR, params);
	cmdLineParams._isNoPlugin = isInList(FLAG_NO_PLUGIN, params);
	cmdLineParams._isReadOnly = isInList(FLAG_READONLY, params);
	cmdLineParams._isNoSession = isInList(FLAG_NOSESSION, params);
	cmdLineParams._isPreLaunch = isInList(FLAG_SYSTRAY, params);
	cmdLineParams._alwaysOnTop = isInList(FLAG_ALWAYS_ON_TOP, params);
	cmdLineParams._showLoadingTime = isInList(FLAG_LOADINGTIME, params);
	cmdLineParams._isSessionFile = isInList(FLAG_OPENSESSIONFILE, params);
	cmdLineParams._isRecursive = isInList(FLAG_RECURSIVE, params);
	cmdLineParams._langType = getLangTypeFromParam(params);
	cmdLineParams._localizationPath = getLocalizationPathFromParam(params);
	cmdLineParams._easterEggName = getEasterEggNameFromParam(params, cmdLineParams._quoteType);

	// getNumberFromParam should be run at the end, to not consuming the other params
	cmdLineParams._line2go = getNumberFromParam('n', params, isParamePresent);
    cmdLineParams._column2go = getNumberFromParam('c', params, isParamePresent);
    cmdLineParams._pos2go = getNumberFromParam('p', params, isParamePresent);
	cmdLineParams._point.x = getNumberFromParam('x', params, cmdLineParams._isPointXValid);
	cmdLineParams._point.y = getNumberFromParam('y', params, cmdLineParams._isPointYValid);


	if (showHelp)
		::MessageBox(NULL, COMMAND_ARG_HELP, TEXT("Notepad++ Command Argument Help"), MB_OK);

	NppParameters *pNppParameters = NppParameters::getInstance();
	NppGUI & nppGui = const_cast<NppGUI &>(pNppParameters->getNppGUI());
	bool doUpdate = nppGui._autoUpdateOpt._doAutoUpdate;

	if (doFunctionListExport || doPrintAndQuit) // export functionlist feature will serialize fuctionlist on the disk, then exit Notepad++. So it's important to not launch into existing instance, and keep it silent.
	{
		isMultiInst = true;
		doUpdate = false;
		cmdLineParams._isNoSession = true;
	}

	if (cmdLineParams._localizationPath != TEXT(""))
	{
		pNppParameters->setStartWithLocFileName(cmdLineParams._localizationPath);
	}
	pNppParameters->load();

	pNppParameters->setFunctionListExportBoolean(doFunctionListExport);
	pNppParameters->setPrintAndExitBoolean(doPrintAndQuit);

	// override the settings if notepad style is present
	if (pNppParameters->asNotepadStyle())
	{
		isMultiInst = true;
		cmdLineParams._isNoTab = true;
		cmdLineParams._isNoSession = true;
	}

	// override the settings if multiInst is choosen by user in the preference dialog
	const NppGUI & nppGUI = pNppParameters->getNppGUI();
	if (nppGUI._multiInstSetting == multiInst)
	{
		isMultiInst = true;
		// Only the first launch remembers the session
		if (!TheFirstOne)
			cmdLineParams._isNoSession = true;
	}

	generic_string quotFileName = TEXT("");
    // tell the running instance the FULL path to the new files to load
	size_t nbFilesToOpen = params.size();

	for (size_t i = 0; i < nbFilesToOpen; ++i)
	{
		const TCHAR * currentFile = params.at(i).c_str();
		if (currentFile[0])
		{
			//check if relative or full path. Relative paths dont have a colon for driveletter

			quotFileName += TEXT("\"");
			quotFileName += relativeFilePathToFullFilePath(currentFile);
			quotFileName += TEXT("\" ");
		}
	}

	//Only after loading all the file paths set the working directory
	::SetCurrentDirectory(NppParameters::getInstance()->getNppPath().c_str());	//force working directory to path of module, preventing lock

	if ((!isMultiInst) && (!TheFirstOne))
	{
		HWND hNotepad_plus = ::FindWindow(Notepad_plus_Window::getClassName(), NULL);
		for (int i = 0 ;!hNotepad_plus && i < 5 ; ++i)
		{
			Sleep(100);
			hNotepad_plus = ::FindWindow(Notepad_plus_Window::getClassName(), NULL);
		}

        if (hNotepad_plus)
        {
			// First of all, destroy static object NppParameters
			pNppParameters->destroyInstance();
			MainFileManager->destroyInstance();

			int sw = 0;

			if (::IsZoomed(hNotepad_plus))
				sw = SW_MAXIMIZE;
			else if (::IsIconic(hNotepad_plus))
				sw = SW_RESTORE;

			if (sw != 0)
				::ShowWindow(hNotepad_plus, sw);

			::SetForegroundWindow(hNotepad_plus);

			if (params.size() > 0)	//if there are files to open, use the WM_COPYDATA system
			{
				COPYDATASTRUCT paramData;
				paramData.dwData = COPYDATA_PARAMS;
				paramData.lpData = &cmdLineParams;
				paramData.cbData = sizeof(cmdLineParams);

				COPYDATASTRUCT fileNamesData;
				fileNamesData.dwData = COPYDATA_FILENAMES;
				fileNamesData.lpData = (void *)quotFileName.c_str();
				fileNamesData.cbData = long(quotFileName.length() + 1)*(sizeof(TCHAR));

				::SendMessage(hNotepad_plus, WM_COPYDATA, reinterpret_cast<WPARAM>(hInstance), reinterpret_cast<LPARAM>(&paramData));
				::SendMessage(hNotepad_plus, WM_COPYDATA, reinterpret_cast<WPARAM>(hInstance), reinterpret_cast<LPARAM>(&fileNamesData));
			}
			return 0;
        }
	}

	Notepad_plus_Window notepad_plus_plus;

	generic_string updaterDir = pNppParameters->getNppPath();
	updaterDir += TEXT("\\updater\\");

	generic_string updaterFullPath = updaterDir + TEXT("gup.exe");

	generic_string updaterParams = TEXT("-v");
	updaterParams += VERSION_VALUE;

	bool isUpExist = nppGui._doesExistUpdater = (::PathFileExists(updaterFullPath.c_str()) == TRUE);

    if (doUpdate) // check more detail
    {
        Date today(0);

        if (today < nppGui._autoUpdateOpt._nextUpdateDate)
            doUpdate = false;
    }

	// wingup doesn't work with the obsolet security layer (API) under xp since downloadings are secured with SSL on notepad_plus_plus.org
	winVer ver = pNppParameters->getWinVersion();
	bool isGtXP = ver > WV_XP;
	if (TheFirstOne && isUpExist && doUpdate && isGtXP)
	{
		if (pNppParameters->isx64())
		{
			updaterParams += TEXT(" -px64");
		}
		Process updater(updaterFullPath.c_str(), updaterParams.c_str(), updaterDir.c_str());
		updater.run();

        // Update next update date
        if (nppGui._autoUpdateOpt._intervalDays < 0) // Make sure interval days value is positive
            nppGui._autoUpdateOpt._intervalDays = 0 - nppGui._autoUpdateOpt._intervalDays;
        nppGui._autoUpdateOpt._nextUpdateDate = Date(nppGui._autoUpdateOpt._intervalDays);
	}

	MSG msg;
	msg.wParam = 0;
	Win32Exception::installHandler();
	try
	{
		notepad_plus_plus.init(hInstance, NULL, quotFileName.c_str(), &cmdLineParams);
		allowWmCopydataMessages(notepad_plus_plus, pNppParameters, ver);
		bool going = true;
		while (going)
		{
			going = ::GetMessageW(&msg, NULL, 0, 0) != 0;
			if (going)
			{
				// if the message doesn't belong to the notepad_plus_plus's dialog
				if (!notepad_plus_plus.isDlgsMsg(&msg))
				{
					if (::TranslateAccelerator(notepad_plus_plus.getHSelf(), notepad_plus_plus.getAccTable(), &msg) == 0)
					{
						::TranslateMessage(&msg);
						::DispatchMessageW(&msg);
					}
				}
			}
		}
	}
	catch (int i)
	{
		TCHAR str[50] = TEXT("God Damned Exception : ");
		TCHAR code[10];
		wsprintf(code, TEXT("%d"), i);
		::MessageBox(Notepad_plus_Window::gNppHWND, lstrcat(str, code), TEXT("Int Exception"), MB_OK);
		doException(notepad_plus_plus);
	}
	catch (std::runtime_error & ex)
	{
		::MessageBoxA(Notepad_plus_Window::gNppHWND, ex.what(), "Runtime Exception", MB_OK);
		doException(notepad_plus_plus);
	}
	catch (const Win32Exception & ex)
	{
		TCHAR message[1024];	//TODO: sane number
		wsprintf(message, TEXT("An exception occured. Notepad++ cannot recover and must be shut down.\r\nThe exception details are as follows:\r\n")
			TEXT("Code:\t0x%08X\r\nType:\t%S\r\nException address: 0x%08X"), ex.code(), ex.what(), reinterpret_cast<long>(ex.where()));
		::MessageBox(Notepad_plus_Window::gNppHWND, message, TEXT("Win32Exception"), MB_OK | MB_ICONERROR);
		mdump.writeDump(ex.info());
		doException(notepad_plus_plus);
	}
	catch (std::exception & ex)
	{
		::MessageBoxA(Notepad_plus_Window::gNppHWND, ex.what(), "General Exception", MB_OK);
		doException(notepad_plus_plus);
	}
	catch (...) // this shouldnt ever have to happen
	{
		::MessageBoxA(Notepad_plus_Window::gNppHWND, "An exception that we did not yet found its name is just caught", "Unknown Exception", MB_OK);
		doException(notepad_plus_plus);
	}

	return static_cast<int>(msg.wParam);
}
