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



#include <stdexcept>
#include "ListView.h"
#include "Parameters.h"
#include "localization.h"

using namespace std;

void ListView::init(HINSTANCE hInst, HWND parent)
{
	Window::init(hInst, parent);
    INITCOMMONCONTROLSEX icex;

    // Ensure that the common control DLL is loaded.
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Create the list-view window in report view with label editing enabled.
	int listViewStyles = LVS_REPORT | LVS_NOSORTHEADER\
						| LVS_SINGLESEL | LVS_AUTOARRANGE\
						| LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS;

	_hSelf = ::CreateWindow(WC_LISTVIEW,
                                TEXT(""),
								WS_CHILD | WS_BORDER | listViewStyles,
                                0,
                                0,
                                0,
                                0,
                                _hParent,
                                nullptr,
                                hInst,
                                nullptr);
	if (!_hSelf)
	{
		throw std::runtime_error("ListView::init : CreateWindowEx() function return null");
	}

	::SetWindowLongPtr(_hSelf, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	_defaultProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(_hSelf, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(staticProc)));

	DWORD exStyle = ListView_GetExtendedListViewStyle(_hSelf);
	exStyle |= LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT | _extraStyle;
	ListView_SetExtendedListViewStyle(_hSelf, exStyle);

	if (_columnInfos.size())
	{
		LVCOLUMN lvColumn;
		lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;

		short i = 0;
		for (auto it = _columnInfos.begin(); it != _columnInfos.end(); ++it)
		{
			lvColumn.cx = static_cast<int>(it->_width);
			lvColumn.pszText = const_cast<TCHAR *>(it->_label.c_str());
			ListView_InsertColumn(_hSelf, ++i, &lvColumn);
		}
	}
}

void ListView::destroy()
{
	::DestroyWindow(_hSelf);
	_hSelf = NULL;
}

void ListView::addLine(const vector<generic_string> & values2Add, LPARAM lParam, int pos2insert)
{
	if (not values2Add.size())
		return;

	if (pos2insert == -1)
		pos2insert = static_cast<int>(nbItem());

	auto it = values2Add.begin();

	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;

	item.pszText = const_cast<TCHAR *>(it->c_str());
	item.iItem = pos2insert;
	item.iSubItem = 0;
	item.lParam = lParam;
	ListView_InsertItem(_hSelf, &item);
	++it;

	int j = 0;
	for (; it != values2Add.end(); ++it)
	{
		ListView_SetItemText(_hSelf, pos2insert, ++j, const_cast<TCHAR *>(it->c_str()));
	}
}


LPARAM ListView::getLParamFromIndex(int itemIndex) const
{
	LVITEM item;
	item.mask = LVIF_PARAM;
	item.iItem = itemIndex;
	ListView_GetItem(_hSelf, &item);

	return item.lParam;
}

std::vector<size_t> ListView::getCheckedIndexes() const
{
	vector<size_t> checkedIndexes;
	size_t nbItem = ListView_GetItemCount(_hSelf);
	for (size_t i = 0; i < nbItem; ++i)
	{
		UINT st = ListView_GetItemState(_hSelf, i, LVIS_STATEIMAGEMASK);
		if (st == INDEXTOSTATEIMAGEMASK(2)) // checked
			checkedIndexes.push_back(i);
	}
	return checkedIndexes;
}

LRESULT ListView::runProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return ::CallWindowProc(_defaultProc, hwnd, Message, wParam, lParam);
}

