//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "StaticDialog.h"
#include "RunDlg.h"
#include "FileDialog.h"
#include "Notepad_plus_msgs.h"
#include "shortcut.h"
#include "Parameters.h"
#include "Notepad_plus.h"


void Command::extractArgs(TCHAR *cmd2Exec, TCHAR *args, const TCHAR *cmdEntier)
{
	size_t i = 0;
	bool quoted = false;
	for (size_t len = lstrlen(cmdEntier); i < len ; ++i)
	{
		if ((cmdEntier[i] == ' ') && (!quoted))
			break;
		if (cmdEntier[i]=='"')
			quoted = !quoted;

		cmd2Exec[i] = cmdEntier[i];
	}
	cmd2Exec[i] = '\0';
	
	if (i < size_t(lstrlen(cmdEntier)))
	{
		for (size_t len = size_t(lstrlen(cmdEntier)); (i < len) && (cmdEntier[i] == ' ') ; ++i);
		if (i < size_t(lstrlen(cmdEntier)))
		{
			for (size_t k = 0, len2 = size_t(lstrlen(cmdEntier)); i <= len2; ++i, ++k)
			{
				args[k] = cmdEntier[i];
			}
		}

		int l = lstrlen(args);
		if (args[l-1] == ' ')
		{
			for (l -= 2 ; (l > 0) && (args[l] == ' ') ; l--);
			args[l+1] = '\0';
		}

	}
	else
		args[0] = '\0';
}


int whichVar(TCHAR *str)
{
	if (!lstrcmp(fullCurrentPath, str))
		return FULL_CURRENT_PATH;
	else if (!lstrcmp(currentDirectory, str))
		return CURRENT_DIRECTORY;
	else if (!lstrcmp(onlyFileName, str))
		return FILE_NAME;
	else if (!lstrcmp(fileNamePart, str))
		return NAME_PART;
	else if (!lstrcmp(fileExtPart, str))
		return EXT_PART;
	else if (!lstrcmp(currentWord, str))
		return CURRENT_WORD;
	else if (!lstrcmp(nppDir, str))
		return NPP_DIRECTORY;
	else if (!lstrcmp(nppFullFilePath, str))
		return NPP_FULL_FILE_PATH;
	else if (!lstrcmp(currentLine, str))
		return CURRENT_LINE;
	else if (!lstrcmp(currentColumn, str))
		return CURRENT_COLUMN;

	return VAR_NOT_RECOGNIZED;
}

// Since I'm sure the length will be 256, I won't check the lstrlen : watch out!
void expandNppEnvironmentStrs(const TCHAR *strSrc, TCHAR *stringDest, size_t strDestLen, HWND hWnd)
{
	size_t j = 0;
	for (int i = 0, len = lstrlen(strSrc); i < len; ++i)
	{
		int iBegin = -1;
		int iEnd = -1;
		if ((strSrc[i] == '$') && (strSrc[i+1] == '('))
		{
			iBegin = i += 2;
			for (size_t len2 = size_t(lstrlen(strSrc)); size_t(i) < len2 ; ++i)
			{
				if (strSrc[i] == ')')
				{
					iEnd = i - 1;
					break;
				}
			}
		}
		if (iBegin != -1)
		{
			if (iEnd != -1)
			{
				TCHAR str[MAX_PATH];
				int m = 0;
				for (int k = iBegin  ; k <= iEnd ; ++k)
					str[m++] = strSrc[k];
				str[m] = '\0';

				int internalVar = whichVar(str);
				if (internalVar == VAR_NOT_RECOGNIZED)
				{
					i = iBegin - 2;
					if (j < (strDestLen-1))
						stringDest[j++] = strSrc[i];
					else
						break;
				}
				else
				{
					TCHAR expandedStr[CURRENTWORD_MAXLENGTH];
					if (internalVar == CURRENT_LINE || internalVar == CURRENT_COLUMN)
					{
						auto lineNumber = ::SendMessage(hWnd, RUNCOMMAND_USER + internalVar, 0, 0);
						wsprintf(expandedStr, TEXT("%d"), lineNumber);
					}
					else
						::SendMessage(hWnd, RUNCOMMAND_USER + internalVar, CURRENTWORD_MAXLENGTH, reinterpret_cast<LPARAM>(expandedStr));

					for (size_t p = 0, len3 = size_t(lstrlen(expandedStr)); p < len3; ++p)
					{
						if (j < (strDestLen-1))
							stringDest[j++] = expandedStr[p];
						else
							break;
					}
				}
			}
			else
			{
				i = iBegin - 2;
				if (j < (strDestLen-1))
					stringDest[j++] = strSrc[i];
				else
					break;
			}
		}
		else
			if (j < (strDestLen-1))
				stringDest[j++] = strSrc[i];
			else
				break;
	}
	stringDest[j] = '\0';
}

HINSTANCE Command::run(HWND hWnd)
{
	const int argsIntermediateLen = MAX_PATH*2;
	const int args2ExecLen = CURRENTWORD_MAXLENGTH+MAX_PATH*2;

	TCHAR cmdPure[MAX_PATH];
	TCHAR cmdIntermediate[MAX_PATH];
	TCHAR cmd2Exec[MAX_PATH];
	TCHAR args[MAX_PATH];
	TCHAR argsIntermediate[argsIntermediateLen];
	TCHAR args2Exec[args2ExecLen];

	extractArgs(cmdPure, args, _cmdLine.c_str());
	int nbTchar = ::ExpandEnvironmentStrings(cmdPure, cmdIntermediate, MAX_PATH);
	if (!nbTchar)
		lstrcpy(cmdIntermediate, cmdPure);
	else if (nbTchar >= MAX_PATH)
		cmdIntermediate[MAX_PATH-1] = '\0';

	nbTchar = ::ExpandEnvironmentStrings(args, argsIntermediate, argsIntermediateLen);
	if (!nbTchar)
		lstrcpy(argsIntermediate, args);
	else if (nbTchar >= argsIntermediateLen)
		argsIntermediate[argsIntermediateLen-1] = '\0';

	expandNppEnvironmentStrs(cmdIntermediate, cmd2Exec, MAX_PATH, hWnd);
	expandNppEnvironmentStrs(argsIntermediate, args2Exec, args2ExecLen, hWnd);

	HINSTANCE res = ::ShellExecute(hWnd, TEXT("open"), cmd2Exec, args2Exec, TEXT("."), SW_SHOW);

	// As per MSDN (https://msdn.microsoft.com/en-us/library/windows/desktop/bb762153(v=vs.85).aspx)
	// If the function succeeds, it returns a value greater than 32.
	// If the function fails, it returns an error value that indicates the cause of the failure.
	int retResult = reinterpret_cast<int>(res);
	if (retResult <= 32)
	{
		generic_string errorMsg;
		errorMsg += GetLastErrorAsString(retResult);
		errorMsg += TEXT("An attempt was made to execute the below command.");
		errorMsg += TEXT("\n----------------------------------------------------------");
		errorMsg += TEXT("\nCommand: ");
		errorMsg += cmd2Exec;
		errorMsg += TEXT("\nArguments: ");
		errorMsg += args2Exec;
		errorMsg += TEXT("\nError Code: ");
		errorMsg += intToString(retResult);
		errorMsg += TEXT("\n----------------------------------------------------------");

		::MessageBox(hWnd, errorMsg.c_str(), TEXT("ShellExecute - ERROR"), MB_ICONINFORMATION | MB_APPLMODAL);
	}

	return res;
}

INT_PTR CALLBACK RunDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case NPPM_INTERNAL_FINDKEYCONFLICTS:
		{
			return ::SendMessage(_hParent, message, wParam, lParam);
		}

		case WM_COMMAND : 
		{
			switch (wParam)
			{
				case IDCANCEL :
					display(false);
					return TRUE;
				
				case IDOK :
				{
					TCHAR cmd[MAX_PATH];
					::GetDlgItemText(_hSelf, IDC_COMBO_RUN_PATH, cmd, MAX_PATH);
					_cmdLine = cmd;

					HINSTANCE hInst = run(_hParent);
					if (int(hInst) > 32)
					{
						addTextToCombo(_cmdLine.c_str());
						display(false);
					}
					else
					{
						removeTextFromCombo(_cmdLine.c_str());
					}
					return TRUE;
				}

				case IDC_BUTTON_SAVE :
				{
					std::vector<UserCommand> & theUserCmds = (NppParameters::getInstance())->getUserCommandList();

					int nbCmd = static_cast<int32_t>(theUserCmds.size());

					int cmdID = ID_USER_CMD + nbCmd;
					TCHAR cmd[MAX_PATH];
					::GetDlgItemText(_hSelf, IDC_COMBO_RUN_PATH, cmd, MAX_PATH);
					UserCommand uc(Shortcut(), cmd, cmdID);
					uc.init(_hInst, _hSelf);

					if (uc.doDialog() != -1)
					{
						HMENU mainMenu = reinterpret_cast<HMENU>(::SendMessage(_hParent, NPPM_INTERNAL_GETMENU, 0, 0));
						HMENU hRunMenu = ::GetSubMenu(mainMenu, MENUINDEX_RUN);
						int const posBase = 2;
						
						if (nbCmd == 0)
							::InsertMenu(hRunMenu, posBase - 1, MF_BYPOSITION, static_cast<unsigned int>(-1), 0);
						
						theUserCmds.push_back(uc);
						::InsertMenu(hRunMenu, posBase + nbCmd, MF_BYPOSITION, cmdID, uc.toMenuItemString().c_str());

						NppParameters* nppParams = NppParameters::getInstance();
                        if (nbCmd == 0)
                        {
                            // Insert the separator and modify/delete command
							::InsertMenu(hRunMenu, posBase + nbCmd + 1, MF_BYPOSITION, static_cast<unsigned int>(-1), 0);
							NativeLangSpeaker *pNativeLangSpeaker = nppParams->getNativeLangSpeaker();
							generic_string nativeLangShortcutMapperMacro = pNativeLangSpeaker->getNativeLangMenuString(IDM_SETTING_SHORTCUT_MAPPER_MACRO);
							if (nativeLangShortcutMapperMacro == TEXT(""))
								nativeLangShortcutMapperMacro = TEXT("Modify Shortcut/Delete Command...");

							::InsertMenu(hRunMenu, posBase + nbCmd + 2, MF_BYCOMMAND, IDM_SETTING_SHORTCUT_MAPPER_RUN, nativeLangShortcutMapperMacro.c_str());
                        }
						nppParams->getAccelerator()->updateShortcuts();
						nppParams->setShortcutDirty();
					}
					return TRUE;
				}

				case IDC_BUTTON_FILE_BROWSER :
				{
					FileDialog fd(_hSelf, _hInst);
					fd.setExtFilter(TEXT("Executable file : "), TEXT(".exe"), TEXT(".com"), TEXT(".cmd"), TEXT(".bat"), NULL);
					fd.setExtFilter(TEXT("All files : "), TEXT(".*"), NULL);

					if (const TCHAR *fn = fd.doOpenSingleFileDlg())
					{
						if(wcschr(fn, ' ') != NULL)
						{
							generic_string fn_quotes(fn);
							fn_quotes = TEXT("\"") + fn_quotes + TEXT("\"");
							addTextToCombo(fn_quotes.c_str());
						}
						else
						{
							addTextToCombo(fn);
						}
					}
					return TRUE;
				}

				default :
					break;
			}
		}
	}
	return FALSE;	
}

void RunDlg::addTextToCombo(const TCHAR *txt2Add) const
{
	HWND handle = ::GetDlgItem(_hSelf, IDC_COMBO_RUN_PATH);
	auto i = ::SendMessage(handle, CB_FINDSTRINGEXACT, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(txt2Add));
	if (i == CB_ERR)
		i = ::SendMessage(handle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(txt2Add));
	::SendMessage(handle, CB_SETCURSEL, i, 0);
}
void RunDlg::removeTextFromCombo(const TCHAR *txt2Remove) const
{
	HWND handle = ::GetDlgItem(_hSelf, IDC_COMBO_RUN_PATH);
	auto i = ::SendMessage(handle, CB_FINDSTRINGEXACT, static_cast<WPARAM>(-1), reinterpret_cast<LPARAM>(txt2Remove));
	if (i == CB_ERR)
		return;
	::SendMessage(handle, CB_DELETESTRING, i, 0);
}

void RunDlg::doDialog(bool isRTL)
{
	if (!isCreated())
		create(IDD_RUN_DLG, isRTL);

    // Adjust the position in the center
	goToCenter();
	::SetFocus(::GetDlgItem(_hSelf, IDC_COMBO_RUN_PATH));
};
