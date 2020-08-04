// this file is part of notepad++
// Copyright (C)2003 Harry <harrybharry@users.sourceforge.net>
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

#include "SmartHighlighter.h"
#include "ScintillaEditView.h"
#include "FindReplaceDlg.h"

#define MAXLINEHIGHLIGHT 400	//prevent highlighter from doing too much work when a lot is visible

SmartHighlighter::SmartHighlighter(FindReplaceDlg * pFRDlg)
: _pFRDlg(pFRDlg)
{
	//Nothing to do
}

void SmartHighlighter::highlightViewWithWord(ScintillaEditView * pHighlightView, const generic_string & word2Hilite)
{
	// save target locations for other search functions
	auto originalStartPos = pHighlightView->execute(SCI_GETTARGETSTART);
	auto originalEndPos = pHighlightView->execute(SCI_GETTARGETEND);

	// Get the range of text visible and highlight everything in it
	auto firstLine = static_cast<int>(pHighlightView->execute(SCI_GETFIRSTVISIBLELINE));
	auto nbLineOnScreen = pHighlightView->execute(SCI_LINESONSCREEN);
	auto nbLines = min(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1;
	auto lastLine = firstLine + nbLines;
	int startPos = 0;
	int endPos = 0;
	auto currentLine = firstLine;
	int prevDocLineChecked = -1;	//invalid start

	// Determine mode for SmartHighlighting
	bool isWordOnly = true;
	bool isCaseSensentive = true;

	const NppGUI & nppGUI = NppParameters::getInstance()->getNppGUI();

	if (nppGUI._smartHiliteUseFindSettings)
	{
		// fetch find dialog's setting
		NppParameters *nppParams = NppParameters::getInstance();
		FindHistory &findHistory = nppParams->getFindHistory();
		isWordOnly = findHistory._isMatchWord;
		isCaseSensentive = findHistory._isMatchCase;
	}
	else
	{
		isWordOnly = nppGUI._smartHiliteWordOnly;
		isCaseSensentive = nppGUI._smartHiliteCaseSensitive;
	}

	FindOption fo;
	fo._isMatchCase = isCaseSensentive;
	fo._isWholeWord = isWordOnly;

	FindReplaceInfo frInfo;
	frInfo._txt2find = word2Hilite.c_str();

	for (; currentLine < lastLine; ++currentLine)
	{
		int docLine = static_cast<int>(pHighlightView->execute(SCI_DOCLINEFROMVISIBLE, currentLine));
		if (docLine == prevDocLineChecked)
			continue;	//still on same line (wordwrap)
		prevDocLineChecked = docLine;
		startPos = static_cast<int>(pHighlightView->execute(SCI_POSITIONFROMLINE, docLine));
		endPos = static_cast<int>(pHighlightView->execute(SCI_POSITIONFROMLINE, docLine + 1));
		
		frInfo._startRange = startPos;
		frInfo._endRange = endPos;
		if (endPos == -1)
		{	//past EOF
			frInfo._endRange = pHighlightView->getCurrentDocLen() - 1;
			_pFRDlg->processRange(ProcessMarkAll_2, frInfo, NULL, &fo, -1, pHighlightView);
			break;
		}
		else
		{
			_pFRDlg->processRange(ProcessMarkAll_2, frInfo, NULL, &fo, -1, pHighlightView);
		}
	}

	// restore the original targets to avoid conflicts with the search/replace functions
	pHighlightView->execute(SCI_SETTARGETRANGE, originalStartPos, originalEndPos);
}

void SmartHighlighter::highlightView(ScintillaEditView * pHighlightView, ScintillaEditView * unfocusView)
{
	// Clear marks
	pHighlightView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);

	const NppGUI & nppGUI = NppParameters::getInstance()->getNppGUI();

	// If nothing selected, dont mark anything
	if (pHighlightView->execute(SCI_GETSELECTIONEMPTY) == 1)
	{
		if (nppGUI._smartHiliteOnAnotherView && unfocusView && unfocusView->isVisible()
			&& unfocusView->getCurrentBufferID() != pHighlightView->getCurrentBufferID())
		{
			unfocusView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
		}
		return;
	}

	auto curPos = pHighlightView->execute(SCI_GETCURRENTPOS);
	auto range = pHighlightView->getSelection();

	// Determine mode for SmartHighlighting
	bool isWordOnly = true;

	if (nppGUI._smartHiliteUseFindSettings)
	{
		// fetch find dialog's setting
		NppParameters *nppParams = NppParameters::getInstance();
		FindHistory &findHistory = nppParams->getFindHistory();
		isWordOnly = findHistory._isMatchWord;
	}
	else
	{
		isWordOnly = nppGUI._smartHiliteWordOnly;
	}

	// additional checks for wordOnly mode
	// Make sure the "word" positions match the current selection
	if (isWordOnly)
	{
		auto wordStart = pHighlightView->execute(SCI_WORDSTARTPOSITION, curPos, true);
		auto wordEnd = pHighlightView->execute(SCI_WORDENDPOSITION, wordStart, true);

		if (wordStart == wordEnd || wordStart != range.cpMin || wordEnd != range.cpMax)
			return;
	}

	int textlen = range.cpMax - range.cpMin + 1;
	char * text2Find = new char[textlen];
	pHighlightView->getSelectedText(text2Find, textlen, false); //do not expand selection (false)

	WcharMbcsConvertor *wmc = WcharMbcsConvertor::getInstance();
	UINT cp = static_cast<UINT>(pHighlightView->execute(SCI_GETCODEPAGE));
	const TCHAR * text2FindW = wmc->char2wchar(text2Find, cp);

	highlightViewWithWord(pHighlightView, text2FindW);

	if (nppGUI._smartHiliteOnAnotherView && unfocusView && unfocusView->isVisible()
		&& unfocusView->getCurrentBufferID() != pHighlightView->getCurrentBufferID())
	{
		// Clear marks
		unfocusView->clearIndicator(SCE_UNIVERSAL_FOUND_STYLE_SMART);
		highlightViewWithWord(unfocusView, text2FindW);
	}

	delete[] text2Find;
}
