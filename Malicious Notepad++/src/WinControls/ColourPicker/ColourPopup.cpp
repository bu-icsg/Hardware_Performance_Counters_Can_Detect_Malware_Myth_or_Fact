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


#include <iostream>
#include <stdexcept>
#include "ColourPopup.h"

DWORD colourItems[] = {
	RGB(  0,   0,   0),	RGB( 64,   0,   0),	RGB(128,   0,   0),	RGB(128,  64,  64),	RGB(255,   0,   0),	RGB(255, 128, 128),
	RGB(255, 255, 128),	RGB(255, 255,   0),	RGB(255, 128,  64),	RGB(255, 128,   0),	RGB(128,  64,   0),	RGB(128, 128,   0),
	RGB(128, 128,  64),	RGB(  0,  64,   0),	RGB(  0, 128,   0),	RGB(  0, 255,   0),	RGB(128, 255,   0),	RGB(128, 255, 128),
	RGB(  0, 255, 128),	RGB(  0, 255,  64),	RGB(  0, 128, 128),	RGB(  0, 128,  64),	RGB(  0,  64,  64),	RGB(128, 128, 128),
	RGB( 64, 128, 128),	RGB(  0,   0, 128),	RGB(  0,   0, 255),	RGB(  0,  64, 128),	RGB(  0, 255, 255), RGB(128, 255, 255),
	RGB(  0, 128, 255),	RGB(  0, 128, 192),	RGB(128, 128, 255),	RGB(  0,   0, 160),	RGB(  0,   0,  64),	RGB(192, 192, 192),
	RGB( 64,   0,  64),	RGB( 64,   0,  64),	RGB(128,   0, 128),	RGB(128,   0,  64),	RGB(128, 128, 192),	RGB(255, 128, 192),
	RGB(255, 128, 255),	RGB(255,   0, 255), RGB(255,   0, 128),	RGB(128,   0, 255), RGB( 64,   0, 128),	RGB(255, 255, 255),
};

void ColourPopup::create(int dialogID) 
{
	_hSelf = ::CreateDialogParam(_hInst, MAKEINTRESOURCE(dialogID), _hParent,  dlgProc, reinterpret_cast<LPARAM>(this));
	
	if (!_hSelf)
	{
		throw std::runtime_error("ColourPopup::create : CreateDialogParam() function return null");
	}
	Window::getClientRect(_rc);
	display();
}

INT_PTR CALLBACK ColourPopup::dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message) 
	{
		case WM_MEASUREITEM:
		{
			RECT rc;
			LPMEASUREITEMSTRUCT lpmis = reinterpret_cast<LPMEASUREITEMSTRUCT>(lParam);
			::GetWindowRect(::GetDlgItem(hwnd, lpmis->CtlID), &rc);
			lpmis->itemHeight = (rc.bottom-rc.top)/6; 
			lpmis->itemWidth = (rc.right-rc.left)/8;
			return TRUE;
		}

		case WM_INITDIALOG :
		{
			ColourPopup *pColourPopup = reinterpret_cast<ColourPopup *>(lParam);
			pColourPopup->_hSelf = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, static_cast<LONG_PTR>(lParam));
			pColourPopup->run_dlgProc(message, wParam, lParam);
			return TRUE;
		}

		default :
		{
			ColourPopup *pColourPopup = reinterpret_cast<ColourPopup *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (!pColourPopup)
				return FALSE;
			return pColourPopup->run_dlgProc(message, wParam, lParam);
		}
	}
}

INT_PTR CALLBACK ColourPopup::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:
		{
			int nColor;
			for (nColor = 0 ; nColor < int(sizeof(colourItems)/sizeof(DWORD)) ; ++nColor)
			{
				::SendDlgItemMessage(_hSelf, IDC_COLOUR_LIST, LB_ADDSTRING, nColor, reinterpret_cast<LPARAM>(""));
				::SendDlgItemMessage(_hSelf, IDC_COLOUR_LIST, LB_SETITEMDATA, nColor, static_cast<LPARAM>(colourItems[nColor]));
			}
			return TRUE;
		}
		
		case WM_CTLCOLORLISTBOX:
			return (LRESULT) CreateSolidBrush(GetSysColor(COLOR_3DFACE));

		case WM_DRAWITEM:
		{
			HDC hdc;
			COLORREF	cr;
			HBRUSH		hbrush;
	
			DRAWITEMSTRUCT *pdis = (DRAWITEMSTRUCT *)lParam;
			hdc = pdis->hDC;
			RECT rc = pdis->rcItem;
	
			// Transparent.
			SetBkMode(hdc,TRANSPARENT);
	
			// NULL object
			if (pdis->itemID == UINT(-1)) return 0; 

			switch (pdis->itemAction)
			{
				case ODA_DRAWENTIRE:
					switch (pdis->CtlID)
					{
						case IDC_COLOUR_LIST:
							rc = pdis->rcItem;
							cr = (COLORREF) pdis->itemData;
							InflateRect(&rc, -3, -3);
							hbrush = CreateSolidBrush((COLORREF)cr);
							FillRect(hdc, &rc, hbrush);
							DeleteObject(hbrush);
							FrameRect(hdc, &rc, (HBRUSH) GetStockObject(GRAY_BRUSH));
							break;
					}
					// *** FALL THROUGH ***
				case ODA_SELECT:
					rc = pdis->rcItem;
					if (pdis->itemState & ODS_SELECTED)
					{
						rc.bottom --;
						rc.right --;
						// Draw the lighted side.
						HPEN hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
						HPEN holdPen = (HPEN)SelectObject(hdc, hpen);
						MoveToEx(hdc, rc.left, rc.bottom, NULL);
						LineTo(hdc, rc.left, rc.top);
						LineTo(hdc, rc.right, rc.top);
						SelectObject(hdc, holdPen);
						DeleteObject(hpen);
						// Draw the darkened side.
						hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
						holdPen = (HPEN)SelectObject(hdc, hpen);
						LineTo(hdc, rc.right, rc.bottom);
						LineTo(hdc, rc.left, rc.bottom);
						SelectObject(hdc, holdPen);
						DeleteObject(hpen);
					}
					else 
					{
						hbrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
						FrameRect(hdc, &rc, hbrush);
						DeleteObject(hbrush);
					}
					break;
				case ODA_FOCUS:
					rc = pdis->rcItem;
					InflateRect(&rc, -2, -2);
					DrawFocusRect(hdc, &rc);
					break;
				default:
					break;
			}
			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam))
            {
                case IDOK :
			    {
					//isColourChooserLaunched = true;
					CHOOSECOLOR cc;                 // common dialog box structure 
					static COLORREF acrCustClr[16] = {
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
						RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),RGB(0xFF,0xFF,0xFF),\
					}; // array of custom colors 

					// Initialize CHOOSECOLOR 
					::ZeroMemory(&cc, sizeof(cc));
					cc.lStructSize = sizeof(cc);
					cc.hwndOwner = _hParent;

					cc.lpCustColors = (LPDWORD) acrCustClr;
					cc.rgbResult = _colour;
					cc.Flags = CC_FULLOPEN | CC_RGBINIT;

					display(false);
					 
					if (ChooseColor(&cc)==TRUE) 
					{
						::SendMessage(_hParent, WM_PICKUP_COLOR, cc.rgbResult, 0);
					}
					else
					{
						::SendMessage(_hParent, WM_PICKUP_CANCEL, 0, 0);
					}

				    return TRUE;
			    }

                case IDC_COLOUR_LIST :
                {
			        if (HIWORD(wParam) == LBN_SELCHANGE)
		            {
                        auto i = ::SendMessage(reinterpret_cast<HWND>(lParam), LB_GETCURSEL, 0L, 0L);
						_colour = static_cast<COLORREF>(::SendMessage(reinterpret_cast<HWND>(lParam), LB_GETITEMDATA, i, 0L));

                        ::SendMessage(_hParent, WM_PICKUP_COLOR, _colour, 0);
					    return TRUE;
		            }
                }
			    
                default :
                    return FALSE;
            }
		
		case WM_ACTIVATE :
        {
			if (LOWORD(wParam) == WA_INACTIVE)
				::SendMessage(_hParent, WM_PICKUP_CANCEL, 0, 0);
			return TRUE;
		}
		
	}
	return FALSE;
}



