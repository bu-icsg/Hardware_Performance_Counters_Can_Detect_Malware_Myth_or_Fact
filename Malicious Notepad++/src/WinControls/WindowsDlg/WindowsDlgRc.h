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


#ifndef WINDOWS_DLG_RC_H
#define WINDOWS_DLG_RC_H

#ifdef __GNUC__
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

	#ifndef LVS_OWNERDATA
		#define LVS_OWNERDATA 4096
	#endif

#endif

#define IDD_WINDOWS 7000
	#define IDC_WINDOWS_LIST (IDD_WINDOWS + 1)
	#define IDC_WINDOWS_SAVE (IDD_WINDOWS + 2)
	#define IDC_WINDOWS_CLOSE (IDD_WINDOWS + 3)
	#define IDC_WINDOWS_SORT (IDD_WINDOWS + 4)

#define IDR_WINDOWS_MENU 11000
	#define  IDM_WINDOW_WINDOWS   (IDR_WINDOWS_MENU + 1)
	#define  IDM_WINDOW_MRU_FIRST (IDR_WINDOWS_MENU + 20)
	#define  IDM_WINDOW_MRU_LIMIT (IDR_WINDOWS_MENU + 29)

#endif //WINDOWS_DLG_RC_H
