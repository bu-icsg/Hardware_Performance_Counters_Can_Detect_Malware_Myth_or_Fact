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


#ifndef FILEBROWSER_RC_H
#define FILEBROWSER_RC_H

#define	IDD_FILEBROWSER		3500

#define	IDD_FILEBROWSER_MENU		(IDD_FILEBROWSER + 10)

  #define IDM_FILEBROWSER_REMOVEROOTFOLDER (IDD_FILEBROWSER_MENU + 1)
  #define IDM_FILEBROWSER_REMOVEALLROOTS   (IDD_FILEBROWSER_MENU + 2)
  #define IDM_FILEBROWSER_ADDROOT          (IDD_FILEBROWSER_MENU + 3)
  #define IDM_FILEBROWSER_SHELLEXECUTE     (IDD_FILEBROWSER_MENU + 4)
  #define IDM_FILEBROWSER_OPENINNPP        (IDD_FILEBROWSER_MENU + 5)
  #define IDM_FILEBROWSER_COPYEPATH        (IDD_FILEBROWSER_MENU + 6)
  #define IDM_FILEBROWSER_FINDINFILES      (IDD_FILEBROWSER_MENU + 7)

  #define IDM_FILEBROWSER_EXPLORERHERE     (IDD_FILEBROWSER_MENU + 8)
  #define IDM_FILEBROWSER_CMDHERE          (IDD_FILEBROWSER_MENU + 9)

#define	IDD_FILEBROWSER_CTRL		(IDD_FILEBROWSER + 30)
  #define	ID_FILEBROWSERTREEVIEW    (IDD_FILEBROWSER_CTRL + 1)
  

#endif // FILEBROWSER_RC_H

