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


#include "BabyGridWrapper.h"

const TCHAR *babyGridClassName = TEXT("BABYGRID");

bool BabyGridWrapper::_isRegistered = false;

void BabyGridWrapper::init(HINSTANCE hInst, HWND parent, int16_t id)
{
	Window::init(hInst, parent);

	if (!_isRegistered)
		RegisterGridClass(_hInst);
 
	_hSelf = ::CreateWindowEx(WS_EX_CLIENTEDGE,
	                babyGridClassName,\
					TEXT(""),\
					WS_CHILD | WS_VISIBLE | WS_TABSTOP,\
					CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,\
					_hParent,\
					reinterpret_cast<HMENU>(id), \
					_hInst,\
					NULL);
}
