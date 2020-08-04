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


#include "documentMap.h"
#include "ScintillaEditView.h"


void DocumentMap::reloadMap()
{
	if (_pMapView && _ppEditView)
	{
		Document currentDoc = (*_ppEditView)->execute(SCI_GETDOCPOINTER);
		_pMapView->execute(SCI_SETDOCPOINTER, 0, static_cast<LPARAM>(currentDoc));

		//
		// sync with the current document
		//

		Buffer *editBuf = (*_ppEditView)->getCurrentBuffer();
		_pMapView->setCurrentBuffer(editBuf);

		// folding
		std::vector<size_t> lineStateVector;
		(*_ppEditView)->getCurrentFoldStates(lineStateVector);
		_pMapView->syncFoldStateWith(lineStateVector);

		// Wrapping
		if ((*_ppEditView)->isWrap() && needToRecomputeWith())
		{
			wrapMap();
		}

		scrollMap();
	}
}

void DocumentMap::showInMapTemporarily(Buffer *buf2show, ScintillaEditView *fromEditView)
{
	if (_pMapView && fromEditView)
	{
		_pMapView->execute(SCI_SETDOCPOINTER, 0, static_cast<LPARAM>(buf2show->getDocument()));
		_pMapView->setCurrentBuffer(buf2show);

		// folding
		const std::vector<size_t> & lineStateVector = buf2show->getHeaderLineState(fromEditView);
		_pMapView->syncFoldStateWith(lineStateVector);

		// Wrapping
		if (fromEditView->isWrap() && needToRecomputeWith(fromEditView))
		{
			wrapMap(fromEditView);
		}

		MapPosition mp = buf2show->getMapPosition();
		if (mp.isValid())
			scrollMapWith(mp);
	}
}

void DocumentMap::setSyntaxHiliting()
{
	Buffer *buf = _pMapView->getCurrentBuffer();
	_pMapView->defineDocType(buf->getLangType());
	_pMapView->showMargin(ScintillaEditView::_SC_MARGE_FOLDER, false);
}

bool DocumentMap::needToRecomputeWith(const ScintillaEditView *editView)
{
	const ScintillaEditView *pEditView = editView ? editView : *_ppEditView;

	auto currentZoom = pEditView->execute(SCI_GETZOOM);
	if (_displayZoom != currentZoom)
		return true;

	int currentTextZoneWidth = getEditorTextZoneWidth(editView);
	if (_displayWidth != currentTextZoneWidth)
		return true;

	return false;
}

void DocumentMap::initWrapMap()
{
	if (_pMapView && _ppEditView)
	{
		RECT rect;
		getClientRect(rect);
		::MoveWindow(_pMapView->getHSelf(), 0, 0, rect.right - rect.left, rect.bottom-rect.top, TRUE);
		_pMapView->wrap(false);
		_pMapView->redraw(true);

		bool isRTL = (*_ppEditView)->isTextDirectionRTL();
		if (_pMapView->isTextDirectionRTL() != isRTL)
			_pMapView->changeTextDirection(isRTL);
	}
}

void DocumentMap::changeTextDirection(bool isRTL)
{
	_pMapView->changeTextDirection(isRTL);
}

/*
double ddd = (double)Xlength1/(double)Xlength2;
char dchar[256];
sprintf(dchar, "%f", ddd);
::MessageBoxA(NULL, dchar, "", MB_OK);
		
		// -10    => 1
		// -9     => 1
		// -8     => 1
		// -7     => 1
		// -6     => 1.5
		// -5     => 2
		// -4     => 2.5
		// -3     => 2.5
		// -2     => 3.5
		// -1     => 3.5
		// 0: -10 => 4
		// 1      => 4.5
		// 2      => 5
		// 3      => 5
		// 4      => 5.5
		// 5      => 6
		// 6      => 6.5
		// 7      => 7
		// 8      => 7
		// 9      => 7.5
		// 10     => 8
		// 11     => 8.5
		// 12     => 8.5
		// 13     => 9.5
		// 14     => 9.5
		// 15     => 10
		// 16     => 10.5
		// 17     => 11
		// 18     => 11
		// 19     => 11.5
		// 20     => 12
*/
double zoomRatio[] = {1, 1, 1, 1, 1.5, 2, 2.5, 2.5, 3.5, 3.5,\
4, 4.5, 5, 5, 5.5, 6, 6.5, 7, 7, 7.5, 8, 8.5, 8.5, 9.5, 9.5, 10, 10.5, 11, 11, 11.5, 12};

void DocumentMap::wrapMap(const ScintillaEditView *editView)
{
	const ScintillaEditView *pEditView = editView ? editView : *_ppEditView;
	RECT rect;
	getClientRect(rect);
	if (pEditView->isWrap())
	{
		// get current scintilla width W1
		int editZoneWidth = getEditorTextZoneWidth(editView);

		// update the wrap needed data
		_displayWidth = editZoneWidth;
		_displayZoom = static_cast<int32_t>(pEditView->execute(SCI_GETZOOM));
		double zr = zoomRatio[_displayZoom + 10];

		// compute doc map width: dzw/ezw = 1/zoomRatio
		double docMapWidth = editZoneWidth / zr;

		::MoveWindow(_pMapView->getHSelf(), 0, 0, int(docMapWidth), rect.bottom-rect.top, TRUE);
		_pMapView->wrap(true);

		// sync wrapping indent mode
		_pMapView->execute(SCI_SETWRAPINDENTMODE, pEditView->execute(SCI_GETWRAPINDENTMODE));

	}
}

int DocumentMap::getEditorTextZoneWidth(const ScintillaEditView *editView)
{
	const ScintillaEditView *pEditView = editView ? editView : *_ppEditView;

	RECT editorRect;
	pEditView->getClientRect(editorRect);

	int marginWidths = 0;
	for (int m = 0; m < 4; ++m)
	{
		marginWidths += static_cast<int32_t>(pEditView->execute(SCI_GETMARGINWIDTHN, m));
	}
	return editorRect.right - editorRect.left - marginWidths;
}

void DocumentMap::scrollMap()
{
	if (_pMapView && _ppEditView)
	{
		// Visible document line for the code view (but not displayed line)
		auto firstVisibleDisplayLine = (*_ppEditView)->execute(SCI_GETFIRSTVISIBLELINE);
		const auto firstVisibleDocLine = (*_ppEditView)->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLine);
		const auto nbLine = (*_ppEditView)->execute(SCI_LINESONSCREEN, firstVisibleDisplayLine);
		const auto lastVisibleDocLine = (*_ppEditView)->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLine + nbLine);

		// Visible document line for the map view
		auto firstVisibleDisplayLineMap = _pMapView->execute(SCI_GETFIRSTVISIBLELINE);
		auto firstVisibleDocLineMap = _pMapView->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLineMap);
		auto nbLineMap = _pMapView->execute(SCI_LINESONSCREEN, firstVisibleDocLineMap);
		auto lastVisibleDocLineMap = _pMapView->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLineMap + nbLineMap);

		// If part of editor view is out of map, then scroll map
		LRESULT mapLineToScroll = 0;
		if (lastVisibleDocLineMap < lastVisibleDocLine)
			mapLineToScroll = lastVisibleDocLine;
		else
			mapLineToScroll = firstVisibleDocLine;
		//
		// Scroll to make whole view zone visible
		//
		_pMapView->execute(SCI_GOTOLINE, mapLineToScroll);

		// Get the editor's higher/lower Y, then compute the map's higher/lower Y
		LRESULT higherY = 0;
		LRESULT lowerY = 0;
		LRESULT higherPos = -1 ; // -1 => not (*_ppEditView)->isWrap()
		if (not (*_ppEditView)->isWrap())
		{
			higherPos = _pMapView->execute(SCI_POSITIONFROMLINE, firstVisibleDocLine);
			auto lowerPos = _pMapView->execute(SCI_POSITIONFROMLINE, lastVisibleDocLine);
			higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, higherPos);
			lowerY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, lowerPos);
			if (lowerY == 0)
			{
				auto lineHeight = _pMapView->execute(SCI_TEXTHEIGHT, firstVisibleDocLine);
				lowerY = nbLine * lineHeight + firstVisibleDocLine;
			}
		}
		else
		{
			// Get the position of the 1st showing char from the original edit view
			higherPos = (*_ppEditView)->execute(SCI_POSITIONFROMPOINT, 0, 0);

			// Get the map higher Y point from the position in map
			higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, static_cast<int32_t>(higherPos));

			// Get line height
			auto lineHeight = _pMapView->execute(SCI_TEXTHEIGHT, firstVisibleDocLine);

			// Get the map lower Y point
			lowerY = nbLine * lineHeight + higherY;
		}

		//
		// Mark view zone in map
		//
		_vzDlg.drawZone(static_cast<int32_t>(higherY), static_cast<int32_t>(lowerY));
	}
}

void DocumentMap::scrollMapWith(const MapPosition & mapPos)
{
	if (_pMapView)
	{
		// Visible document line for the map view
		auto firstVisibleDisplayLineMap = _pMapView->execute(SCI_GETFIRSTVISIBLELINE);
		auto firstVisibleDocLineMap = _pMapView->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLineMap);
		auto nbLineMap = _pMapView->execute(SCI_LINESONSCREEN, firstVisibleDocLineMap);
		auto lastVisibleDocLineMap = _pMapView->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLineMap + nbLineMap);

		// If part of editor view is out of map, then scroll map
		LRESULT mapLineToScroll = 0;
		if (lastVisibleDocLineMap < mapPos._lastVisibleDocLine)
			mapLineToScroll = mapPos._lastVisibleDocLine;
		else
			mapLineToScroll = mapPos._firstVisibleDocLine;
		//
		// Scroll to make whole view zone visible
		//
		_pMapView->execute(SCI_GOTOLINE, mapLineToScroll);

		// Get the editor's higher/lower Y, then compute the map's higher/lower Y
		LRESULT higherY = 0;
		LRESULT lowerY = 0;
		if (not mapPos._isWrap)
		{
			auto higherPos = _pMapView->execute(SCI_POSITIONFROMLINE, mapPos._firstVisibleDocLine);
			auto lowerPos = _pMapView->execute(SCI_POSITIONFROMLINE, mapPos._lastVisibleDocLine);
			higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, higherPos);
			lowerY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, lowerPos);
			if (lowerY == 0)
			{
				auto lineHeight = _pMapView->execute(SCI_TEXTHEIGHT, mapPos._firstVisibleDocLine);
				lowerY = mapPos._nbLine * lineHeight + mapPos._firstVisibleDocLine;
			}
		}
		else
		{
			higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, static_cast<int32_t>(mapPos._higherPos));
			auto lineHeight = _pMapView->execute(SCI_TEXTHEIGHT, mapPos._firstVisibleDocLine);
			lowerY = mapPos._nbLine * lineHeight + higherY;
		}

		//
		// Mark view zone in map
		//
		_vzDlg.drawZone(static_cast<int32_t>(higherY), static_cast<int32_t>(lowerY));
	}
}

void DocumentMap::doMove()
{
	RECT rc;
	POINT pt = {0,0};
	::ClientToScreen(_hSelf, &pt);
	getClientRect(rc);
	::MoveWindow(_vzDlg.getHSelf(), pt.x, pt.y, (rc.right - rc.left), (rc.bottom - rc.top), TRUE);
}

void DocumentMap::fold(int line, bool foldOrNot)
{
	_pMapView->fold(line, foldOrNot);
}

void DocumentMap::foldAll(bool mode)
{
	_pMapView->foldAll(mode);
}

void DocumentMap::scrollMap(bool direction, moveMode whichMode)
{
	// Visible line for the code view
	auto firstVisibleDisplayLine = (*_ppEditView)->execute(SCI_GETFIRSTVISIBLELINE);
	auto nbLine = (*_ppEditView)->execute(SCI_LINESONSCREEN, firstVisibleDisplayLine);
	auto nbLine2go = (whichMode == perLine ? 1 : nbLine);
	(*_ppEditView)->execute(SCI_LINESCROLL, 0, (direction == moveDown) ? nbLine2go : -nbLine2go);

	scrollMap();
}

void DocumentMap::redraw(bool) const
{
	_pMapView->execute(SCI_COLOURISE, 0, -1);
}

INT_PTR CALLBACK DocumentMap::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG :
        {
			HWND hwndScintilla = reinterpret_cast<HWND>(::SendMessage(_hParent, NPPM_CREATESCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(_hSelf)));
			_pMapView = reinterpret_cast<ScintillaEditView *>(::SendMessage(_hParent, NPPM_INTERNAL_GETSCINTEDTVIEW, 0, reinterpret_cast<LPARAM>(hwndScintilla)));
			_pMapView->execute(SCI_SETZOOM, static_cast<WPARAM>(-10), 0);
			_pMapView->execute(SCI_SETVSCROLLBAR, FALSE, 0);
			_pMapView->execute(SCI_SETHSCROLLBAR, FALSE, 0);

			_pMapView->showIndentGuideLine(false);
			_pMapView->display();
			
			reloadMap();

			_vzDlg.init(::GetModuleHandle(NULL), _hSelf);
			_vzDlg.doDialog();
			(NppParameters::getInstance())->SetTransparent(_vzDlg.getHSelf(), 50); // 0 <= transparancy < 256

			setSyntaxHiliting();
			
			_pMapView->showMargin(0, false);
			_pMapView->showMargin(1, false);
			_pMapView->showMargin(2, false);
			_pMapView->showMargin(3, false);
			
            return TRUE;
        }

        case WM_SIZE:
        {
			if (_pMapView)
			{
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);

				if (_vzDlg.isCreated())
				{
					POINT pt = {0,0};
					::ClientToScreen(_hSelf, &pt);
					if (!_pMapView->isWrap())
						::MoveWindow(_pMapView->getHSelf(), 0, 0, width, height, TRUE);
						
					::MoveWindow(_vzDlg.getHSelf(), pt.x, pt.y, width, height, TRUE);
				}
			}
            break;
        }

		case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
				case DMN_CLOSE:
				{
					::SendMessage(_hParent, WM_COMMAND, IDM_VIEW_DOC_MAP, 0);
					return TRUE;
				}

				case DMN_SWITCHIN:
				{
					_vzDlg.display();
					reloadMap();
					setSyntaxHiliting();
					return TRUE;
				}

				case DMN_SWITCHOFF:
				{
					_vzDlg.display(false);
					return TRUE;
				}

				case DMN_FLOATDROPPED:
				{
					RECT rc;
					getClientRect(rc);
					int width = rc.right - rc.left;
					int height = rc.bottom - rc.top;

					POINT pt = {0,0};
					::ClientToScreen(_hSelf, &pt);
					::MoveWindow(_vzDlg.getHSelf(), pt.x, pt.y, width, height, TRUE);
					scrollMap();
					return TRUE;
				}

				case NM_DBLCLK:
				{
					return TRUE;
				}

				default:
					break;
			}
		}
		return TRUE;

		case DOCUMENTMAP_SCROLL:
		{
			bool dir = (wParam != 0);
			moveMode mode = (lParam == 0)?perLine:perPage;
			scrollMap(dir, mode);
		}
		return TRUE;

		case DOCUMENTMAP_MOUSECLICKED:
		{
			int newPosY = HIWORD(lParam);
			int currentCenterPosY = _vzDlg.getCurrentCenterPosY();
			int pixelPerLine = static_cast<int32_t>(_pMapView->execute(SCI_TEXTHEIGHT, 0));
			int jumpDistance = newPosY - currentCenterPosY;
			int nbLine2jump = jumpDistance/pixelPerLine;
			(*_ppEditView)->execute(SCI_LINESCROLL, 0, nbLine2jump);

			scrollMap();
		}
		return TRUE;

		case DOCUMENTMAP_MOUSEWHEEL:
		{
			(*_ppEditView)->mouseWheel(wParam, lParam);
		}
		return TRUE;

        default :
            return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
    }
	return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}

void ViewZoneDlg::drawPreviewZone(DRAWITEMSTRUCT *pdis)
{
	RECT rc = pdis->rcItem;
	
	const COLORREF orange = RGB(0xFF, 0x80, 0x00);
	const COLORREF white = RGB(0xFF, 0xFF, 0xFF);
	HBRUSH hbrushFg = CreateSolidBrush(orange);
	HBRUSH hbrushBg = CreateSolidBrush(white);					
	FillRect(pdis->hDC, &rc, hbrushBg);

	rc.top = _higherY;
	rc.bottom = _lowerY;
	FillRect(pdis->hDC, &rc, hbrushFg);
	
	DeleteObject(hbrushFg);
	DeleteObject(hbrushBg);
}

void ViewZoneDlg::doDialog()
{
	if (!isCreated())
		create(IDD_VIEWZONE);
	display();
};

INT_PTR CALLBACK ViewZoneDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
        case WM_INITDIALOG :
		{
			_viewZoneCanvas = ::GetDlgItem(_hSelf, IDC_VIEWZONECANVAS);
			if (NULL != _viewZoneCanvas)
			{
				::SetWindowLongPtr(_viewZoneCanvas, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
				_canvasDefaultProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(_viewZoneCanvas, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(canvasStaticProc)));
				return TRUE;
			}
			break;
		}

		case WM_LBUTTONDOWN:
		{
			::SendMessage(_hParent, DOCUMENTMAP_MOUSECLICKED, wParam, lParam);
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (wParam & MK_LBUTTON)
				::SendMessage(_hParent, DOCUMENTMAP_MOUSECLICKED, wParam, lParam);
			break;
		}

		case WM_DRAWITEM :
		{
			drawPreviewZone((DRAWITEMSTRUCT *)lParam);
			return TRUE;
		}

		case WM_SIZE:
        {
			if (NULL != _viewZoneCanvas)
			{
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);
				::MoveWindow(_viewZoneCanvas, 0, 0, width , height, TRUE);
			}
            break;
        }

		case WM_MOUSEWHEEL :
		{
			//Have to perform the scroll first, because the first/last line do not get updated untill after the scroll has been parsed
			::SendMessage(_hParent, DOCUMENTMAP_MOUSEWHEEL, wParam, lParam);
			return TRUE;
		}

		case WM_DESTROY :
		{
			return TRUE;
		}
	}
	return FALSE;
}

LRESULT CALLBACK ViewZoneDlg::canvasStaticProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	ViewZoneDlg *pViewZoneDlg = reinterpret_cast<ViewZoneDlg *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (!pViewZoneDlg)
		return FALSE;
	return pViewZoneDlg->canvas_runProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ViewZoneDlg::canvas_runProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
    {
		case WM_DESTROY:
		{
			//::MessageBoxA(NULL,"Destroy","",MB_OK);
		}
		return TRUE;

		case WM_KEYDOWN:
			if (wParam == VK_UP)
			{
				::SendMessage(_hParent, DOCUMENTMAP_SCROLL, static_cast<WPARAM>(moveUp), 0);
			}
			if (wParam == VK_DOWN)
			{
				::SendMessage(_hParent, DOCUMENTMAP_SCROLL, static_cast<WPARAM>(moveDown), 0);
			}
			if (wParam == VK_PRIOR)
			{
				::SendMessage(_hParent, DOCUMENTMAP_SCROLL, static_cast<WPARAM>(moveUp), 1);
			}
			if (wParam == VK_NEXT)
			{
				::SendMessage(_hParent, DOCUMENTMAP_SCROLL, static_cast<WPARAM>(moveDown), 1);
			}
			break;

        case WM_SIZE:
        {
            break;
        }

		case WM_NOTIFY:
		{
		}
		return TRUE;

        default :
            return _canvasDefaultProc(hwnd, message, wParam, lParam);
    }
	return _canvasDefaultProc(hwnd, message, wParam, lParam);
}