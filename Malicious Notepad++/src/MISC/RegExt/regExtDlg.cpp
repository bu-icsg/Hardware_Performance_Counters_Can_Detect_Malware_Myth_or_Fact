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
#include "Common.h"
#include "regExtDlg.h"
#include "resource.h"



const TCHAR* nppName   = TEXT("Notepad++_file");
const TCHAR* nppBackup = TEXT("Notepad++_backup");
const TCHAR* nppDoc    = TEXT("Notepad++ Document");

const int nbSupportedLang = 10;
const int nbExtMax = 27;
const int extNameMax = 18;


const TCHAR defExtArray[nbSupportedLang][nbExtMax][extNameMax] =
{
	{TEXT("Notepad"),
		TEXT(".txt"), TEXT(".log")
	},
	{TEXT("ms ini/inf"),
		TEXT(".ini"), TEXT(".inf")
	},
	{TEXT("c, c++, objc"),
		TEXT(".h"), TEXT(".hh"), TEXT(".hpp"), TEXT(".hxx"), TEXT(".c"), TEXT(".cpp"), TEXT(".cxx"), TEXT(".cc"),
		TEXT(".m"), TEXT(".mm"),
		TEXT(".vcxproj"), TEXT(".vcproj"), TEXT(".props"), TEXT(".vsprops"), TEXT(".manifest")
	},
	{TEXT("java, c#, pascal"),
		TEXT(".java"), TEXT(".cs"), TEXT(".pas"), TEXT(".pp"), TEXT(".inc")
	},
	{TEXT("web script"),
		TEXT(".html"), TEXT(".htm"), TEXT(".shtml"), TEXT(".shtm"), TEXT(".hta"),
		TEXT(".asp"), TEXT(".aspx"),
		TEXT(".css"), TEXT(".js"), TEXT(".json"), TEXT(".jsm"), TEXT(".jsp"),
		TEXT(".php"), TEXT(".php3"), TEXT(".php4"), TEXT(".php5"), TEXT(".phps"), TEXT(".phpt"), TEXT(".phtml"),
		TEXT(".xml"), TEXT(".xhtml"), TEXT(".xht"), TEXT(".xul"), TEXT(".kml"), TEXT(".xaml"), TEXT(".xsml")
	},
	{TEXT("public script"),
		TEXT(".sh"), TEXT(".bsh"), TEXT(".bash"), TEXT(".bat"), TEXT(".cmd"), TEXT(".nsi"),
		TEXT(".nsh"), TEXT(".lua"), TEXT(".pl"), TEXT(".pm"), TEXT(".py")
	},
	{TEXT("property script"),
		TEXT(".rc"), TEXT(".as"), TEXT(".mx"), TEXT(".vb"), TEXT(".vbs")
	},
	{TEXT("fortran, TeX, SQL"),
		TEXT(".f"), TEXT(".for"), TEXT(".f90"), TEXT(".f95"), TEXT(".f2k"), TEXT(".tex"), TEXT(".sql")
	},
	{TEXT("misc"),
		TEXT(".nfo"), TEXT(".mak")
	},
	{TEXT("customize")}
};





void RegExtDlg::doDialog(bool isRTL)
{
	if (isRTL)
	{
		DLGTEMPLATE *pMyDlgTemplate = nullptr;
		HGLOBAL hMyDlgTemplate = makeRTLResource(IDD_REGEXT_BOX, &pMyDlgTemplate);
		::DialogBoxIndirectParam(_hInst, pMyDlgTemplate, _hParent, dlgProc, reinterpret_cast<LPARAM>(this));
		::GlobalFree(hMyDlgTemplate);
	}
	else
		::DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_REGEXT_BOX), _hParent, dlgProc, reinterpret_cast<LPARAM>(this));
}


INT_PTR CALLBACK RegExtDlg::run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_INITDIALOG :
		{
			getRegisteredExts();
			getDefSupportedExts();
			//goToCenter();
			::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEXT_BUTTON), false);
			::EnableWindow(::GetDlgItem(_hSelf, IDC_REMOVEEXT_BUTTON), false);
			::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, EM_SETLIMITTEXT, extNameMax-1, 0);
			return TRUE;
		}

		case WM_DRAWITEM :
		{
			HICON hIcon = ::LoadIcon(_hInst, MAKEINTRESOURCE(IDI_DELETE_ICON));
			DRAWITEMSTRUCT *pdis = (DRAWITEMSTRUCT *)lParam;
			::DrawIcon(pdis->hDC, 0, 0, hIcon);
			return TRUE;
		}

		case WM_COMMAND :
		{
			// Handle File association list extension
			if (LOWORD(wParam) == IDC_REGEXT_LANGEXT_LIST || LOWORD(wParam) == IDC_REGEXT_REGISTEREDEXTS_LIST)
			{
				// On double click an item in the list then toggle the item between both lists
				// by simulating "<-" or "->" button clicked
				if (HIWORD(wParam) == LBN_DBLCLK)
				{
					// Check whether click happened on a item not in empty area
					if (-1 != ::SendDlgItemMessage(_hSelf, LOWORD(wParam), LB_GETCURSEL, 0, 0))
					{
						HWND(lParam) == ::GetDlgItem(_hSelf, IDC_REGEXT_LANGEXT_LIST) ?
							::SendMessage(_hSelf, WM_COMMAND, IDC_ADDFROMLANGEXT_BUTTON, 0) :
							::SendMessage(_hSelf, WM_COMMAND, IDC_REMOVEEXT_BUTTON, 0);
					}
					return TRUE;
				}
			}

			switch (wParam)
			{
				case IDC_ADDFROMLANGEXT_BUTTON :
				{
					writeNppPath();

					TCHAR ext2Add[extNameMax] = TEXT("");
					if (!_isCustomize)
					{
						auto index2Add = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_GETCURSEL, 0, 0);
						::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_GETTEXT, index2Add, reinterpret_cast<LPARAM>(ext2Add));
						addExt(ext2Add);
						::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_DELETESTRING, index2Add, 0);
					}
					else
					{
						::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_GETTEXT, extNameMax, reinterpret_cast<LPARAM>(ext2Add));
						auto i = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_FINDSTRINGEXACT, 0, reinterpret_cast<LPARAM>(ext2Add));
						if (i != LB_ERR)
							return TRUE;
						addExt(ext2Add);
						::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(TEXT("")));
					}
					::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(ext2Add));
					::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEXT_BUTTON), false);
					return TRUE;
				}

				case IDC_REMOVEEXT_BUTTON :
				{
					TCHAR ext2Sup[extNameMax] = TEXT("");
					auto index2Sup = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETCURSEL, 0, 0);
					::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETTEXT, index2Sup, reinterpret_cast<LPARAM>(ext2Sup));
					if (deleteExts(ext2Sup))
						::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_DELETESTRING, index2Sup, 0);
					auto langIndex = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANG_LIST, LB_GETCURSEL, 0, 0);

					::EnableWindow(::GetDlgItem(_hSelf, IDC_REMOVEEXT_BUTTON), false);

					if (langIndex != LB_ERR)
					{
						for (int i = 1 ; i < nbExtMax ; ++i)
						{
							if (!generic_stricmp(ext2Sup, defExtArray[langIndex][i]))
							{
								::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(ext2Sup));
								return TRUE;
							}
						}
					}
					return TRUE;
				}

				case IDCANCEL:
				{
					::EndDialog(_hSelf, 0);
					return TRUE;
				}
			}

			if (HIWORD(wParam) == EN_CHANGE)
			{
				TCHAR text[extNameMax] = TEXT("");
				::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_GETTEXT, extNameMax, reinterpret_cast<LPARAM>(text));
				if ((lstrlen(text) == 1) && (text[0] != '.'))
				{
					text[1] = text[0];
					text[0] = '.';
					text[2] = '\0';
					::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
					::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, EM_SETSEL, 2, 2);
				}
				::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEXT_BUTTON), (lstrlen(text) > 1));
				return TRUE;
			}

			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				auto i = ::SendDlgItemMessage(_hSelf, LOWORD(wParam), LB_GETCURSEL, 0, 0);
				if (LOWORD(wParam) == IDC_REGEXT_LANG_LIST)
				{
					if (i != LB_ERR)
					{
						TCHAR itemName[32];
						::SendDlgItemMessage(_hSelf, LOWORD(wParam), LB_GETTEXT, i, reinterpret_cast<LPARAM>(itemName));

						if (!generic_stricmp(defExtArray[nbSupportedLang-1][0], itemName))
						{
							::ShowWindow(::GetDlgItem(_hSelf, IDC_REGEXT_LANGEXT_LIST), SW_HIDE);
							::ShowWindow(::GetDlgItem(_hSelf, IDC_CUSTOMEXT_EDIT), SW_SHOW);
							_isCustomize = true;
						}
						else
						{
							if (_isCustomize)
							{
								::ShowWindow(::GetDlgItem(_hSelf, IDC_REGEXT_LANGEXT_LIST), SW_SHOW);
								::ShowWindow(::GetDlgItem(_hSelf, IDC_CUSTOMEXT_EDIT), SW_HIDE);

								_isCustomize = false;
							}
							LRESULT count = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_GETCOUNT, 0, 0);
							for (count -= 1 ; count >= 0 ; count--)
								::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_DELETESTRING, count, 0);

							for (int j = 1 ; j < nbExtMax ; ++j)
							{
								if (lstrcmp(TEXT(""), defExtArray[i][j]))
								{
									auto index = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_FINDSTRINGEXACT, 0, reinterpret_cast<LPARAM>(defExtArray[i][j]));
									if (index == -1)
										::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(defExtArray[i][j]));
								}
							}
						}

						::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEXT_BUTTON), false);
					}
				}
				else if (LOWORD(wParam) == IDC_REGEXT_LANGEXT_LIST)
				{
					if (i != LB_ERR)
						::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEXT_BUTTON), true);
				}
				else if (LOWORD(wParam) == IDC_REGEXT_REGISTEREDEXTS_LIST)
				{
					if (i != LB_ERR)
						::EnableWindow(::GetDlgItem(_hSelf, IDC_REMOVEEXT_BUTTON), true);
				}
			}

			// break; // no break here
		}

		default :
			return FALSE;
	}
	//return FALSE;
}


void RegExtDlg::getRegisteredExts()
{
	int nbRegisteredKey = getNbSubKey(HKEY_CLASSES_ROOT);
	for (int i = 0 ; i < nbRegisteredKey ; ++i)
	{
		TCHAR extName[extNameLen];
		//FILETIME fileTime;
		int extNameActualLen = extNameLen;
		int res = ::RegEnumKeyEx(HKEY_CLASSES_ROOT, i, extName, reinterpret_cast<LPDWORD>(&extNameActualLen), nullptr, nullptr, nullptr, nullptr);
		if ((res == ERROR_SUCCESS) && (extName[0] == '.'))
		{
			//TCHAR valName[extNameLen];
			TCHAR valData[extNameLen];
			int valDataLen = extNameLen * sizeof(TCHAR);
			int valType;
			HKEY hKey2Check;
			extNameActualLen = extNameLen;
			::RegOpenKeyEx(HKEY_CLASSES_ROOT, extName, 0, KEY_ALL_ACCESS, &hKey2Check);
			::RegQueryValueEx(hKey2Check, TEXT(""), nullptr, reinterpret_cast<LPDWORD>(&valType), reinterpret_cast<LPBYTE>(valData), reinterpret_cast<LPDWORD>(&valDataLen));

			if ((valType == REG_SZ) && (!lstrcmp(valData, nppName)))
				::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(extName));
			::RegCloseKey(hKey2Check);
		}
	}
}


void RegExtDlg::getDefSupportedExts()
{
	for (int i = 0 ; i < nbSupportedLang ; ++i)
		::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANG_LIST, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(defExtArray[i][0]));
}


void RegExtDlg::addExt(TCHAR *ext)
{
	HKEY  hKey;
	DWORD dwDisp;
	long  nRet;

	nRet = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, ext, 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, &dwDisp);

	if (nRet == ERROR_SUCCESS)
	{
		TCHAR valData[MAX_PATH];
		int valDataLen = MAX_PATH * sizeof(TCHAR);

		if (dwDisp == REG_OPENED_EXISTING_KEY)
		{
			int res = ::RegQueryValueEx(hKey, TEXT(""), nullptr, nullptr, reinterpret_cast<LPBYTE>(valData), reinterpret_cast<LPDWORD>(&valDataLen));
			if (res == ERROR_SUCCESS)
				::RegSetValueEx(hKey, nppBackup, 0, REG_SZ, reinterpret_cast<LPBYTE>(valData), valDataLen);
		}
		::RegSetValueEx(hKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE *>(nppName), (lstrlen(nppName) + 1) * sizeof(TCHAR));

		::RegCloseKey(hKey);
	}
}


bool RegExtDlg::deleteExts(const TCHAR *ext2Delete)
{
	HKEY hKey;
	::RegOpenKeyEx(HKEY_CLASSES_ROOT, ext2Delete, 0, KEY_ALL_ACCESS, &hKey);

	int nbValue = getNbSubValue(hKey);
	int nbSubkey = getNbSubKey(hKey);

	if ((nbValue <= 1) && (!nbSubkey))
	{
		TCHAR subKey[32] = TEXT("\\");
		lstrcat(subKey, ext2Delete);
		::RegDeleteKey(HKEY_CLASSES_ROOT, subKey);
	}
	else
	{
		TCHAR valData[extNameLen];
		int valDataLen = extNameLen*sizeof(TCHAR);
		int valType;
		int res = ::RegQueryValueEx(hKey, nppBackup, nullptr, (LPDWORD)&valType, (LPBYTE)valData, (LPDWORD)&valDataLen);

		if (res == ERROR_SUCCESS)
		{
			::RegSetValueEx(hKey, nullptr, 0, valType, (LPBYTE)valData, valDataLen);
			::RegDeleteValue(hKey, nppBackup);
		}
		else
			::RegDeleteValue(hKey, nullptr);
	}

	return true;
}


void RegExtDlg::writeNppPath()
{
	HKEY  hKey, hRootKey;
	DWORD dwDisp;
	long  nRet;
	generic_string regStr(nppName);
	regStr += TEXT("\\shell\\open\\command");

	nRet = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, regStr.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, &dwDisp);


	if (nRet == ERROR_SUCCESS)
	{
		//if (dwDisp == REG_CREATED_NEW_KEY)
		{
			// Write the value for new document
			::RegOpenKeyEx(HKEY_CLASSES_ROOT, nppName, 0, KEY_ALL_ACCESS, &hRootKey);
			::RegSetValueEx(hRootKey, nullptr, 0, REG_SZ, (LPBYTE)nppDoc, (lstrlen(nppDoc)+1)*sizeof(TCHAR));
			RegCloseKey(hRootKey);

			TCHAR nppPath[MAX_PATH];
			::GetModuleFileName(_hInst, nppPath, MAX_PATH);

			TCHAR nppPathParam[MAX_PATH] = TEXT("\"");
			lstrcat(lstrcat(nppPathParam, nppPath), TEXT("\" \"%1\""));

			::RegSetValueEx(hKey, nullptr, 0, REG_SZ, (LPBYTE)nppPathParam, (lstrlen(nppPathParam)+1)*sizeof(TCHAR));
		}
		RegCloseKey(hKey);
	}

	//Set default icon value
	regStr = nppName;
	regStr += TEXT("\\DefaultIcon");
	nRet = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, regStr.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, &dwDisp);

	if (nRet == ERROR_SUCCESS)
	{
		//if (dwDisp == REG_CREATED_NEW_KEY)
		{
			TCHAR nppPath[MAX_PATH];
			::GetModuleFileName(_hInst, nppPath, MAX_PATH);

			TCHAR nppPathParam[MAX_PATH] = TEXT("\"");
			lstrcat(lstrcat(nppPathParam, nppPath), TEXT("\",0"));

			::RegSetValueEx(hKey, nullptr, 0, REG_SZ, (LPBYTE)nppPathParam, (lstrlen(nppPathParam)+1)*sizeof(TCHAR));
		}
		RegCloseKey(hKey);
	}
}


