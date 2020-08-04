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
//	installer, such as those produced by InstallShield.
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

#pragma once

#include "tinyxmlA.h"
#include "tinyxml.h"
#include "Scintilla.h"
#include "ScintillaRef.h"
#include "ToolBar.h"
#include "UserDefineLangReference.h"
#include "colors.h"
#include "shortcut.h"
#include "ContextMenu.h"
#include "dpiManager.h"
#include <assert.h>
#include <tchar.h>

class NativeLangSpeaker;

const bool POS_VERTICAL = true;
const bool POS_HORIZOTAL = false;

const int UDD_SHOW   = 1; // 0000 0001
const int UDD_DOCKED = 2; // 0000 0010

// 0 : 0000 0000 hide & undocked
// 1 : 0000 0001 show & undocked
// 2 : 0000 0010 hide & docked
// 3 : 0000 0011 show & docked

const int TAB_DRAWTOPBAR = 1;      //0000 0000 0001
const int TAB_DRAWINACTIVETAB = 2; //0000 0000 0010
const int TAB_DRAGNDROP = 4;       //0000 0000 0100
const int TAB_REDUCE = 8;          //0000 0000 1000
const int TAB_CLOSEBUTTON = 16;    //0000 0001 0000
const int TAB_DBCLK2CLOSE = 32;    //0000 0010 0000
const int TAB_VERTICAL = 64;       //0000 0100 0000
const int TAB_MULTILINE = 128;     //0000 1000 0000
const int TAB_HIDE = 256;          //0001 0000 0000
const int TAB_QUITONEMPTY = 512;   //0010 0000 0000


enum class EolType: std::uint8_t
{
	windows,
	macos,
	unix,

	// special values
	unknown, // can not be the first value for legacy code
	osdefault = windows,
};

/*!
** \brief Convert an int into a FormatType
** \param value An arbitrary int
** \param defvalue The default value to use if an invalid value is provided
*/
EolType convertIntToFormatType(int value, EolType defvalue = EolType::osdefault);




enum UniMode {uni8Bit=0, uniUTF8=1, uni16BE=2, uni16LE=3, uniCookie=4, uni7Bit=5, uni16BE_NoBOM=6, uni16LE_NoBOM=7, uniEnd};
enum ChangeDetect {cdDisabled=0, cdEnabled=1, cdAutoUpdate=2, cdGo2end=3, cdAutoUpdateGo2end=4};
enum BackupFeature {bak_none = 0, bak_simple = 1, bak_verbose = 2};
enum OpenSaveDirSetting {dir_followCurrent = 0, dir_last = 1, dir_userDef = 2};
enum MultiInstSetting {monoInst = 0, multiInstOnSession = 1, multiInst = 2};
//enum CloudChoice {noCloud = 0, dropbox = 1, oneDrive = 2, googleDrive = 3};

const int LANG_INDEX_INSTR = 0;
const int LANG_INDEX_INSTR2 = 1;
const int LANG_INDEX_TYPE = 2;
const int LANG_INDEX_TYPE2 = 3;
const int LANG_INDEX_TYPE3 = 4;
const int LANG_INDEX_TYPE4 = 5;
const int LANG_INDEX_TYPE5 = 6;

const int COPYDATA_PARAMS = 0;
const int COPYDATA_FILENAMESA = 1;
const int COPYDATA_FILENAMESW = 2;

#define PURE_LC_NONE	0
#define PURE_LC_BOL	 1
#define PURE_LC_WSP	 2

#define DECSEP_DOT	  0
#define DECSEP_COMMA	1
#define DECSEP_BOTH	 2


#define DROPBOX_AVAILABLE 1
#define ONEDRIVE_AVAILABLE 2
#define GOOGLEDRIVE_AVAILABLE 4

const TCHAR fontSizeStrs[][3] = {TEXT(""), TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"), TEXT("9"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("14"), TEXT("16"), TEXT("18"), TEXT("20"), TEXT("22"), TEXT("24"), TEXT("26"), TEXT("28")};

const TCHAR localConfFile[] = TEXT("doLocalConf.xml");
const TCHAR allowAppDataPluginsFile[] = TEXT("allowAppDataPlugins.xml");
const TCHAR notepadStyleFile[] = TEXT("asNotepad.xml");

void cutString(const TCHAR *str2cut, std::vector<generic_string> & patternVect);


struct Position
{
	int _firstVisibleLine = 0;
	int _startPos = 0;
	int _endPos = 0;
	int _xOffset = 0;
	int _selMode = 0;
	int _scrollWidth = 1;
};


struct MapPosition
{
	int32_t _firstVisibleDisplayLine = -1;

	int32_t _firstVisibleDocLine = -1; // map
	int32_t _lastVisibleDocLine = -1;  // map
	int32_t _nbLine = -1;              // map
	int32_t _higherPos = -1;           // map
	int32_t _width = -1;
	int32_t _height = -1;
	int32_t _wrapIndentMode = -1;

	int64_t _KByteInDoc = _maxPeekLenInKB;

	bool _isWrap = false;
	bool isValid() const { return (_firstVisibleDisplayLine != -1); };
	bool canScroll() const { return (_KByteInDoc < _maxPeekLenInKB); }; // _nbCharInDoc < _maxPeekLen : Don't scroll the document for the performance issue

private:
	int64_t _maxPeekLenInKB = 512; // 512 KB
};


struct sessionFileInfo : public Position
{
	sessionFileInfo(const TCHAR *fn, const TCHAR *ln, int encoding, Position pos, const TCHAR *backupFilePath, int originalFileLastModifTimestamp, const MapPosition & mapPos) :
		_encoding(encoding), Position(pos), _originalFileLastModifTimestamp(originalFileLastModifTimestamp), _mapPos(mapPos)
	{
		if (fn) _fileName = fn;
		if (ln)	_langName = ln;
		if (backupFilePath) _backupFilePath = backupFilePath;
	}

	sessionFileInfo(generic_string fn) : _fileName(fn) {}

	generic_string _fileName;
	generic_string	_langName;
	std::vector<size_t> _marks;
	std::vector<size_t> _foldStates;
	int	_encoding = -1;

	generic_string _backupFilePath;
	time_t _originalFileLastModifTimestamp = 0;

	MapPosition _mapPos;
};


struct Session
{
	size_t nbMainFiles() const {return _mainViewFiles.size();};
	size_t nbSubFiles() const {return _subViewFiles.size();};
	size_t _activeView = 0;
	size_t _activeMainIndex = 0;
	size_t _activeSubIndex = 0;
	std::vector<sessionFileInfo> _mainViewFiles;
	std::vector<sessionFileInfo> _subViewFiles;
};


struct CmdLineParams
{
	bool _isNoPlugin = false;
	bool _isReadOnly = false;
	bool _isNoSession = false;
	bool _isNoTab = false;
	bool _isPreLaunch = false;
	bool _showLoadingTime = false;
	bool _alwaysOnTop = false;
	int _line2go   = -1;
	int _column2go = -1;
	int _pos2go = -1;

	POINT _point;
	bool _isPointXValid = false;
	bool _isPointYValid = false;

	bool _isSessionFile = false;
	bool _isRecursive = false;

	LangType _langType = L_EXTERNAL;
	generic_string _localizationPath;
	generic_string _easterEggName;
	unsigned char _quoteType = '\0';

	CmdLineParams()
	{
		_point.x = 0;
		_point.y = 0;
	}

	bool isPointValid() const
	{
		return _isPointXValid && _isPointYValid;
	}
};

struct FloatingWindowInfo
{
	int _cont;
	RECT _pos;

	FloatingWindowInfo(int cont, int x, int y, int w, int h)
		: _cont(cont)
	{
		_pos.left	= x;
		_pos.top	= y;
		_pos.right	= w;
		_pos.bottom = h;
	}
};


struct PluginDlgDockingInfo final
{
	generic_string _name;
	int _internalID = -1;

	int _currContainer = -1;
	int _prevContainer = -1;
	bool _isVisible = false;

	PluginDlgDockingInfo(const TCHAR* pluginName, int id, int curr, int prev, bool isVis)
		: _internalID(id), _currContainer(curr), _prevContainer(prev), _isVisible(isVis), _name(pluginName)
	{}

	bool operator == (const PluginDlgDockingInfo& rhs) const
	{
		return _internalID == rhs._internalID and _name == rhs._name;
	}
};


struct ContainerTabInfo final
{
	int _cont = 0;
	int _activeTab = 0;

	ContainerTabInfo(int cont, int activeTab) : _cont(cont), _activeTab(activeTab) {};
};


struct DockingManagerData final
{
	int _leftWidth = 200;
	int _rightWidth = 200;
	int _topHeight = 200;
	int _bottomHight = 200;

	std::vector<FloatingWindowInfo> _flaotingWindowInfo;
	std::vector<PluginDlgDockingInfo> _pluginDockInfo;
	std::vector<ContainerTabInfo> _containerTabInfo;

	bool getFloatingRCFrom(int floatCont, RECT& rc) const
	{
		for (size_t i = 0, fwiLen = _flaotingWindowInfo.size(); i < fwiLen; ++i)
		{
			if (_flaotingWindowInfo[i]._cont == floatCont)
			{
				rc.left   = _flaotingWindowInfo[i]._pos.left;
				rc.top	= _flaotingWindowInfo[i]._pos.top;
				rc.right  = _flaotingWindowInfo[i]._pos.right;
				rc.bottom = _flaotingWindowInfo[i]._pos.bottom;
				return true;
			}
		}
		return false;
	}
};



const int FONTSTYLE_NONE = 0;
const int FONTSTYLE_BOLD = 1;
const int FONTSTYLE_ITALIC = 2;
const int FONTSTYLE_UNDERLINE = 4;

const int STYLE_NOT_USED = -1;

const int COLORSTYLE_FOREGROUND = 0x01;
const int COLORSTYLE_BACKGROUND = 0x02;
const int COLORSTYLE_ALL = COLORSTYLE_FOREGROUND|COLORSTYLE_BACKGROUND;



struct Style
{
	int _styleID = -1;
	const TCHAR* _styleDesc = nullptr;

	COLORREF _fgColor = COLORREF(STYLE_NOT_USED);
	COLORREF _bgColor = COLORREF(STYLE_NOT_USED);
	int _colorStyle = COLORSTYLE_ALL;
	const TCHAR* _fontName = nullptr;
	int _fontStyle = FONTSTYLE_NONE;
	int _fontSize = STYLE_NOT_USED;
	int _nesting = FONTSTYLE_NONE;

	int _keywordClass = STYLE_NOT_USED;
	generic_string* _keywords = nullptr;


	Style() = default;

	Style(const Style & style)
	{
		_styleID	  = style._styleID;
		_styleDesc	= style._styleDesc;
		_fgColor	  = style._fgColor;
		_bgColor	  = style._bgColor;
		_colorStyle   = style._colorStyle;
		_fontName	 = style._fontName;
		_fontSize	 = style._fontSize;
		_fontStyle	= style._fontStyle;
		_keywordClass = style._keywordClass;
		_nesting	  = style._nesting;
		_keywords	 = (style._keywords) ? new generic_string(*(style._keywords)) : nullptr;
	}

	~Style()
	{
		delete _keywords;
	}


	Style& operator = (const Style & style)
	{
		if (this != &style)
		{
			_styleID	  = style._styleID;
			_styleDesc	= style._styleDesc;
			_fgColor	  = style._fgColor;
			_bgColor	  = style._bgColor;
			_colorStyle   = style._colorStyle;
			_fontName	 = style._fontName;
			_fontSize	 = style._fontSize;
			_fontStyle	= style._fontStyle;
			_keywordClass = style._keywordClass;
			_nesting	  = style._nesting;

			if (!(_keywords) && style._keywords)
				_keywords = new generic_string(*(style._keywords));
			else if (_keywords && style._keywords)
				_keywords->assign(*(style._keywords));
			else if (_keywords && !(style._keywords))
			{
				delete (_keywords);
				_keywords = nullptr;
			}
		}
		return *this;
	}

	void setKeywords(const TCHAR *str)
	{
		if (!_keywords)
			_keywords = new generic_string(str);
		else
			*_keywords = str;
	}
};


struct GlobalOverride final
{
	bool isEnable() const {return (enableFg || enableBg || enableFont || enableFontSize || enableBold || enableItalic || enableUnderLine);}
	bool enableFg = false;
	bool enableBg = false;
	bool enableFont = false;
	bool enableFontSize = false;
	bool enableBold = false;
	bool enableItalic = false;
	bool enableUnderLine = false;
};


struct StyleArray
{
public:
	StyleArray & operator=(const StyleArray & sa)
	{
		if (this != &sa)
		{
			this->_nbStyler = sa._nbStyler;
			for (int i = 0 ; i < _nbStyler ; ++i)
			{
				this->_styleArray[i] = sa._styleArray[i];
			}
		}
		return *this;
	}

	int getNbStyler() const {return _nbStyler;};
	void setNbStyler(int nb) {_nbStyler = nb;};

	Style& getStyler(size_t index)
	{
		assert(index < SCE_STYLE_ARRAY_SIZE);
		return _styleArray[index];
	}

	bool hasEnoughSpace() {return (_nbStyler < SCE_STYLE_ARRAY_SIZE);};
	void addStyler(int styleID, TiXmlNode *styleNode);

	void addStyler(int styleID, const TCHAR *styleName)
	{
		_styleArray[styleID]._styleID = styleID;
		_styleArray[styleID]._styleDesc = styleName;
		_styleArray[styleID]._fgColor = black;
		_styleArray[styleID]._bgColor = white;
		++_nbStyler;
	}

	int getStylerIndexByID(int id)
	{
		for (int i = 0 ; i < _nbStyler ; ++i)
		{
			if (_styleArray[i]._styleID == id)
				return i;
		}
		return -1;
	}

	int getStylerIndexByName(const TCHAR *name) const
	{
		if (!name)
			return -1;
		for (int i = 0 ; i < _nbStyler ; ++i)
		{
			if (!lstrcmp(_styleArray[i]._styleDesc, name))
				return i;
		}
		return -1;
	}

protected:
	Style _styleArray[SCE_STYLE_ARRAY_SIZE];
	int _nbStyler = 0;
};



struct LexerStyler : public StyleArray
{
public:
	LexerStyler & operator=(const LexerStyler & ls)
	{
		if (this != &ls)
		{
			*(static_cast<StyleArray *>(this)) = ls;
			this->_lexerName = ls._lexerName;
			this->_lexerDesc = ls._lexerDesc;
			this->_lexerUserExt = ls._lexerUserExt;
		}
		return *this;
	}

	void setLexerName(const TCHAR *lexerName)
	{
		_lexerName = lexerName;
	}

	void setLexerDesc(const TCHAR *lexerDesc)
	{
		_lexerDesc = lexerDesc;
	}

	void setLexerUserExt(const TCHAR *lexerUserExt) {
		_lexerUserExt = lexerUserExt;
	};

	const TCHAR * getLexerName() const {return _lexerName.c_str();};
	const TCHAR * getLexerDesc() const {return _lexerDesc.c_str();};
	const TCHAR * getLexerUserExt() const {return _lexerUserExt.c_str();};

private :
	generic_string _lexerName;
	generic_string _lexerDesc;
	generic_string _lexerUserExt;
};



const int MAX_LEXER_STYLE = 100;

struct LexerStylerArray
{
public :
	LexerStylerArray() : _nbLexerStyler(0){};

	LexerStylerArray & operator=(const LexerStylerArray & lsa)
	{
		if (this != &lsa)
		{
			this->_nbLexerStyler = lsa._nbLexerStyler;
			for (int i = 0 ; i < this->_nbLexerStyler ; ++i)
				this->_lexerStylerArray[i] = lsa._lexerStylerArray[i];
		}
		return *this;
	}

	int getNbLexer() const {return _nbLexerStyler;};

	LexerStyler & getLexerFromIndex(int index)
	{
		return _lexerStylerArray[index];
	};

	const TCHAR * getLexerNameFromIndex(int index) const {return _lexerStylerArray[index].getLexerName();}
	const TCHAR * getLexerDescFromIndex(int index) const {return _lexerStylerArray[index].getLexerDesc();}

	LexerStyler * getLexerStylerByName(const TCHAR *lexerName) {
		if (!lexerName) return NULL;
		for (int i = 0 ; i < _nbLexerStyler ; ++i)
		{
			if (!lstrcmp(_lexerStylerArray[i].getLexerName(), lexerName))
				return &(_lexerStylerArray[i]);
		}
		return NULL;
	};
	bool hasEnoughSpace() {return (_nbLexerStyler < MAX_LEXER_STYLE);};
	void addLexerStyler(const TCHAR *lexerName, const TCHAR *lexerDesc, const TCHAR *lexerUserExt, TiXmlNode *lexerNode);
	void eraseAll();
private :
	LexerStyler _lexerStylerArray[MAX_LEXER_STYLE];
	int _nbLexerStyler;
};


struct NewDocDefaultSettings final
{
	EolType _format = EolType::osdefault;
	UniMode _unicodeMode = uniCookie;
	bool _openAnsiAsUtf8 = true;
	LangType _lang = L_TEXT;
	int _codepage = -1; // -1 when not using
};


struct LangMenuItem final
{
	LangType _langType;
	int	_cmdID;
	generic_string _langName;

	LangMenuItem(LangType lt, int cmdID = 0, generic_string langName = TEXT("")):
	_langType(lt), _cmdID(cmdID), _langName(langName){};
};

struct PrintSettings final {
	bool _printLineNumber = true;
	int _printOption = SC_PRINT_COLOURONWHITE;

	generic_string _headerLeft;
	generic_string _headerMiddle;
	generic_string _headerRight;
	generic_string _headerFontName;
	int _headerFontStyle = 0;
	int _headerFontSize = 0;

	generic_string _footerLeft;
	generic_string _footerMiddle;
	generic_string _footerRight;
	generic_string _footerFontName;
	int _footerFontStyle = 0;
	int _footerFontSize = 0;

	RECT _marge;

	PrintSettings() {
		_marge.left = 0; _marge.top = 0; _marge.right = 0; _marge.bottom = 0;
	};

	bool isHeaderPresent() const {
		return ((_headerLeft != TEXT("")) || (_headerMiddle != TEXT("")) || (_headerRight != TEXT("")));
	};

	bool isFooterPresent() const {
		return ((_footerLeft != TEXT("")) || (_footerMiddle != TEXT("")) || (_footerRight != TEXT("")));
	};

	bool isUserMargePresent() const {
		return ((_marge.left != 0) || (_marge.top != 0) || (_marge.right != 0) || (_marge.bottom != 0));
	};
};


class Date final
{
public:
	Date() = default;
	Date(unsigned long year, unsigned long month, unsigned long day)
		: _year(year)
		, _month(month)
		, _day(day)
	{
		assert(year > 0 && year <= 9999); // I don't think Notepad++ will last till AD 10000 :)
		assert(month > 0 && month <= 12);
		assert(day > 0 && day <= 31);
		assert(!(month == 2 && day > 29) &&
			   !(month == 4 && day > 30) &&
			   !(month == 6 && day > 30) &&
			   !(month == 9 && day > 30) &&
			   !(month == 11 && day > 30));
	}

	explicit Date(const TCHAR *dateStr);

	// The constructor which makes the date of number of days from now
	// nbDaysFromNow could be negative if user want to make a date in the past
	// if the value of nbDaysFromNow is 0 then the date will be now
	Date(int nbDaysFromNow);

	void now();

	generic_string toString() const // Return Notepad++ date format : YYYYMMDD
	{
		TCHAR dateStr[16];
		wsprintf(dateStr, TEXT("%04u%02u%02u"), _year, _month, _day);
		return dateStr;
	}

	bool operator < (const Date & compare) const
	{
		if (this->_year != compare._year)
			return (this->_year < compare._year);
		if (this->_month != compare._month)
			return (this->_month < compare._month);
		return (this->_day < compare._day);
	}

	bool operator > (const Date & compare) const
	{
		if (this->_year != compare._year)
			return (this->_year > compare._year);
		if (this->_month != compare._month)
			return (this->_month > compare._month);
		return (this->_day > compare._day);
	}

	bool operator == (const Date & compare) const
	{
		if (this->_year != compare._year)
			return false;
		if (this->_month != compare._month)
			return false;
		return (this->_day == compare._day);
	}

	bool operator != (const Date & compare) const
	{
		if (this->_year != compare._year)
			return true;
		if (this->_month != compare._month)
			return true;
		return (this->_day != compare._day);
	}

private:
	unsigned long _year  = 2008;
	unsigned long _month = 4;
	unsigned long _day   = 26;
};


class MatchedPairConf final
{
public:
	bool hasUserDefinedPairs() const { return _matchedPairs.size() != 0; }
	bool hasDefaultPairs() const { return _doParentheses||_doBrackets||_doCurlyBrackets||_doQuotes||_doDoubleQuotes||_doHtmlXmlTag; }
	bool hasAnyPairsPair() const { return hasUserDefinedPairs() || hasDefaultPairs(); }

public:
	std::vector<std::pair<char, char>> _matchedPairs;
	std::vector<std::pair<char, char>> _matchedPairsInit; // used only on init
	bool _doHtmlXmlTag = false;
	bool _doParentheses = false;
	bool _doBrackets = false;
	bool _doCurlyBrackets = false;
	bool _doQuotes = false;
	bool _doDoubleQuotes = false;
};


struct NppGUI final
{
	NppGUI()
	{
		_appPos.left = 0;
		_appPos.top = 0;
		_appPos.right = 1100;
		_appPos.bottom = 700;

		_defaultDir[0] = 0;
		_defaultDirExp[0] = 0;
	}

	toolBarStatusType _toolBarStatus = TB_STANDARD;
	bool _toolbarShow = true;
	bool _statusBarShow = true;
	bool _menuBarShow = true;

	// 1st bit : draw top bar;
	// 2nd bit : draw inactive tabs
	// 3rd bit : enable drag & drop
	// 4th bit : reduce the height
	// 5th bit : enable vertical
	// 6th bit : enable multiline

	// 0:don't draw; 1:draw top bar 2:draw inactive tabs 3:draw both 7:draw both+drag&drop
	int _tabStatus = (TAB_DRAWTOPBAR | TAB_DRAWINACTIVETAB | TAB_DRAGNDROP | TAB_REDUCE | TAB_CLOSEBUTTON);

	bool _splitterPos = POS_VERTICAL;
	int _userDefineDlgStatus = UDD_DOCKED;

	int _tabSize = 4;
	bool _tabReplacedBySpace = false;

	ChangeDetect _fileAutoDetection = cdEnabled;
	ChangeDetect _fileAutoDetectionOriginalValue = cdEnabled;
	bool _checkHistoryFiles = false;

	RECT _appPos;

	bool _isMaximized = false;
	bool _isMinimizedToTray = false;
	bool _rememberLastSession = true; // remember next session boolean will be written in the settings
	bool _isCmdlineNosessionActivated = false; // used for if -nosession is indicated on the launch time
	bool _detectEncoding = true;
	bool _doTaskList = true;
	bool _maitainIndent = true;
	bool _enableSmartHilite = true;

	bool _smartHiliteCaseSensitive = false;
	bool _smartHiliteWordOnly = true;
	bool _smartHiliteUseFindSettings = false;
	bool _smartHiliteOnAnotherView = false;

	bool _disableSmartHiliteTmp = false;
	bool _enableTagsMatchHilite = true;
	bool _enableTagAttrsHilite = true;
	bool _enableHiliteNonHTMLZone = false;
	bool _styleMRU = true;
	char _leftmostDelimiter = '(';
	char _rightmostDelimiter = ')';
	bool _delimiterSelectionOnEntireDocument = false;
	bool _backSlashIsEscapeCharacterForSql = true;

	bool _isWordCharDefault = true;
	std::string _customWordChars;

	// 0 : do nothing
	// 1 : don't draw underline
	// 2 : draw underline
	int _styleURL = 2;

	NewDocDefaultSettings _newDocDefaultSettings;


	void setTabReplacedBySpace(bool b) {_tabReplacedBySpace = b;};
	const NewDocDefaultSettings & getNewDocDefaultSettings() const {return _newDocDefaultSettings;};
	std::vector<LangMenuItem> _excludedLangList;
	bool _isLangMenuCompact = true;

	PrintSettings _printSettings;
	BackupFeature _backup = bak_none;
	bool _useDir = false;
	generic_string _backupDir;
	DockingManagerData _dockingData;
	GlobalOverride _globalOverride;
	enum AutocStatus{autoc_none, autoc_func, autoc_word, autoc_both};
	AutocStatus _autocStatus = autoc_both;
	size_t  _autocFromLen = 1;
	bool _autocIgnoreNumbers = true;
	bool _funcParams = true;
	MatchedPairConf _matchedPairConf;

	generic_string _definedSessionExt;
	generic_string _definedWorkspaceExt;

	struct AutoUpdateOptions
	{
		bool _doAutoUpdate = true;
		int _intervalDays = 15;
		Date _nextUpdateDate;
		AutoUpdateOptions(): _nextUpdateDate(Date()) {};
	}
	_autoUpdateOpt;

	bool _doesExistUpdater = false;
	int _caretBlinkRate = 600;
	int _caretWidth = 1;
	bool _enableMultiSelection = false;

	bool _shortTitlebar = false;

	OpenSaveDirSetting _openSaveDir = dir_followCurrent;

	TCHAR _defaultDir[MAX_PATH];
	TCHAR _defaultDirExp[MAX_PATH];	//expanded environment variables
	generic_string _themeName;
	MultiInstSetting _multiInstSetting = monoInst;
	bool _fileSwitcherWithoutExtColumn = false;
	bool isSnapshotMode() const {return _isSnapshotMode && _rememberLastSession && !_isCmdlineNosessionActivated;};
	bool _isSnapshotMode = true;
	size_t _snapshotBackupTiming = 7000;
	generic_string _cloudPath; // this option will never be read/written from/to config.xml
	unsigned char _availableClouds = '\0'; // this option will never be read/written from/to config.xml
	bool _useNewStyleSaveDlg = false;

	enum SearchEngineChoice{ se_custom = 0, se_duckDuckGo = 1, se_google = 2, se_bing = 3, se_yahoo = 4 };
	SearchEngineChoice _searchEngineChoice = se_google;
	generic_string _searchEngineCustom;

	bool _isFolderDroppedOpenFiles = false;

	bool _isDocPeekOnTab = false;
	bool _isDocPeekOnMap = false;
};

struct ScintillaViewParams
{
	bool _lineNumberMarginShow = true;
	bool _bookMarkMarginShow = true;
	folderStyle  _folderStyle = FOLDER_STYLE_BOX; //"simple", "arrow", "circle", "box" and "none"
	lineWrapMethod _lineWrapMethod = LINEWRAP_ALIGNED;
	bool _foldMarginShow = true;
	bool _indentGuideLineShow = true;
	bool _currentLineHilitingShow = true;
	bool _wrapSymbolShow = false;
	bool _doWrap = false;
	int _edgeMode = EDGE_NONE;
	int _edgeNbColumn = 80;
	int _zoom = 0;
	int _zoom2 = 0;
	bool _whiteSpaceShow = false;
	bool _eolShow = false;
	int _borderWidth = 2;
	bool _scrollBeyondLastLine = false;
	bool _disableAdvancedScrolling = false;
	bool _doSmoothFont = false;
	bool _showBorderEdge = true;
};

const int NB_LIST = 20;
const int NB_MAX_LRF_FILE = 30;
const int NB_MAX_USER_LANG = 30;
const int NB_MAX_EXTERNAL_LANG = 30;
const int NB_MAX_IMPORTED_UDL = 50;

const int NB_MAX_FINDHISTORY_FIND	= 30;
const int NB_MAX_FINDHISTORY_REPLACE = 30;
const int NB_MAX_FINDHISTORY_PATH	= 30;
const int NB_MAX_FINDHISTORY_FILTER  = 20;


const int MASK_ReplaceBySpc = 0x80;
const int MASK_TabSize = 0x7F;




struct Lang final
{
	LangType _langID = L_TEXT;
	generic_string _langName;
	const TCHAR *_defaultExtList = nullptr;
	const TCHAR *_langKeyWordList[NB_LIST];
	const TCHAR *_pCommentLineSymbol = nullptr;
	const TCHAR *_pCommentStart = nullptr;
	const TCHAR *_pCommentEnd = nullptr;

	bool _isTabReplacedBySpace = false;
	int _tabSize = -1;

	Lang()
	{
		for (int i = 0 ; i < NB_LIST ; _langKeyWordList[i] = NULL, ++i);
	}

	Lang(LangType langID, const TCHAR *name) : _langID(langID), _langName(name ? name : TEXT(""))
	{
		for (int i = 0 ; i < NB_LIST ; _langKeyWordList[i] = NULL, ++i);
	}

	~Lang() = default;

	void setDefaultExtList(const TCHAR *extLst){
		_defaultExtList = extLst;
	}

	void setCommentLineSymbol(const TCHAR *commentLine){
		_pCommentLineSymbol = commentLine;
	}

	void setCommentStart(const TCHAR *commentStart){
		_pCommentStart = commentStart;
	}

	void setCommentEnd(const TCHAR *commentEnd){
		_pCommentEnd = commentEnd;
	}

	void setTabInfo(int tabInfo)
	{
		if (tabInfo != -1 && tabInfo & MASK_TabSize)
		{
			_isTabReplacedBySpace = (tabInfo & MASK_ReplaceBySpc) != 0;
			_tabSize = tabInfo & MASK_TabSize;
		}
	}

	const TCHAR * getDefaultExtList() const {
		return _defaultExtList;
	}

	void setWords(const TCHAR *words, int index) {
		_langKeyWordList[index] = words;
	}

	const TCHAR * getWords(int index) const {
		return _langKeyWordList[index];
	}

	LangType getLangID() const {return _langID;};
	const TCHAR * getLangName() const {return _langName.c_str();};

	int getTabInfo() const
	{
		if (_tabSize == -1) return -1;
		return (_isTabReplacedBySpace?0x80:0x00) | _tabSize;
	}
};



class UserLangContainer final
{
public:
	UserLangContainer() :_name(TEXT("new user define")), _ext(TEXT("")), _udlVersion(TEXT(""))
	{
		init();
	}

	UserLangContainer(const TCHAR *name, const TCHAR *ext, const TCHAR *udlVer) : _name(name), _ext(ext), _udlVersion(udlVer)
	{
		init();
	}

	UserLangContainer & operator = (const UserLangContainer & ulc)
	{
		if (this != &ulc)
		{
			this->_name = ulc._name;
			this->_ext = ulc._ext;
			this->_udlVersion = ulc._udlVersion;
			this->_isCaseIgnored = ulc._isCaseIgnored;
			this->_styleArray = ulc._styleArray;
			this->_allowFoldOfComments = ulc._allowFoldOfComments;
			this->_forcePureLC = ulc._forcePureLC;
			this->_decimalSeparator = ulc._decimalSeparator;
			this->_foldCompact = ulc._foldCompact;
			int nbStyler = this->_styleArray.getNbStyler();
			for (int i = 0 ; i < nbStyler ; ++i)
			{
				Style & st = this->_styleArray.getStyler(i);
				if (st._bgColor == COLORREF(-1))
					st._bgColor = white;
				if (st._fgColor == COLORREF(-1))
					st._fgColor = black;
			}

			for (int i = 0 ; i < SCE_USER_KWLIST_TOTAL ; ++i)
				lstrcpy(this->_keywordLists[i], ulc._keywordLists[i]);

			for (int i = 0 ; i < SCE_USER_TOTAL_KEYWORD_GROUPS ; ++i)
				_isPrefix[i] = ulc._isPrefix[i];
		}
		return *this;
	}

	const TCHAR * getName() {return _name.c_str();};
	const TCHAR * getExtention() {return _ext.c_str();};
	const TCHAR * getUdlVersion() {return _udlVersion.c_str();};

private:
	StyleArray _styleArray;
	generic_string _name;
	generic_string _ext;
	generic_string _udlVersion;

	TCHAR _keywordLists[SCE_USER_KWLIST_TOTAL][max_char];
	bool _isPrefix[SCE_USER_TOTAL_KEYWORD_GROUPS];

	bool _isCaseIgnored;
	bool _allowFoldOfComments;
	int  _forcePureLC;
	int _decimalSeparator;
	bool _foldCompact;

	// nakama zone
	friend class Notepad_plus;
	friend class ScintillaEditView;
	friend class NppParameters;

	friend class SharedParametersDialog;
	friend class FolderStyleDialog;
	friend class KeyWordsStyleDialog;
	friend class CommentStyleDialog;
	friend class SymbolsStyleDialog;
	friend class UserDefineDialog;
	friend class StylerDlg;

	void init()
	{
		_forcePureLC = PURE_LC_NONE;
		_decimalSeparator = DECSEP_DOT;
		_foldCompact = false;
		_isCaseIgnored = false;
		_allowFoldOfComments = false;

		for (int i = 0; i < SCE_USER_KWLIST_TOTAL; ++i)
			*_keywordLists[i] = '\0';

		for (int i = 0; i < SCE_USER_TOTAL_KEYWORD_GROUPS; ++i)
			_isPrefix[i] = false;
	}
};

#define MAX_EXTERNAL_LEXER_NAME_LEN 16
#define MAX_EXTERNAL_LEXER_DESC_LEN 32



class ExternalLangContainer final
{
public:
	TCHAR _name[MAX_EXTERNAL_LEXER_NAME_LEN];
	TCHAR _desc[MAX_EXTERNAL_LEXER_DESC_LEN];

	ExternalLangContainer(const TCHAR* name, const TCHAR* desc)
	{
		generic_strncpy(_name, name, MAX_EXTERNAL_LEXER_NAME_LEN);
		generic_strncpy(_desc, desc, MAX_EXTERNAL_LEXER_DESC_LEN);
	}
};


struct FindHistory final
{
	enum searchMode{normal, extended, regExpr};
	enum transparencyMode{none, onLossingFocus, persistant};

	int _nbMaxFindHistoryPath    = 10;
	int _nbMaxFindHistoryFilter  = 10;
	int _nbMaxFindHistoryFind    = 10;
	int _nbMaxFindHistoryReplace = 10;

	std::vector<generic_string> _findHistoryPaths;
	std::vector<generic_string> _findHistoryFilters;
	std::vector<generic_string> _findHistoryFinds;
	std::vector<generic_string> _findHistoryReplaces;

	bool _isMatchWord = false;
	bool _isMatchCase = false;
	bool _isWrap = true;
	bool _isDirectionDown = true;
	bool _dotMatchesNewline = false;

	bool _isFifRecuisive = true;
	bool _isFifInHiddenFolder = false;

	searchMode _searchMode = normal;
	transparencyMode _transparencyMode = onLossingFocus;
	int _transparency = 150;

	bool _isDlgAlwaysVisible = false;
	bool _isFilterFollowDoc = false;
	bool _isFolderFollowDoc = false;
};



class LocalizationSwitcher final
{
friend class NppParameters;
public:
	struct LocalizationDefinition
	{
		wchar_t *_langName;
		wchar_t *_xmlFileName;
	};

	bool addLanguageFromXml(std::wstring xmlFullPath);
	std::wstring getLangFromXmlFileName(const wchar_t *fn) const;

	std::wstring getXmlFilePathFromLangName(const wchar_t *langName) const;
	bool switchToLang(wchar_t *lang2switch) const;

	size_t size() const
	{
		return _localizationList.size();
	}

	std::pair<std::wstring, std::wstring> getElementFromIndex(size_t index) const
	{
		if (index >= _localizationList.size())
			return std::pair<std::wstring, std::wstring>(std::wstring(), std::wstring());
		return _localizationList[index];
	}

	void setFileName(const char *fn)
	{
		if (fn)
			_fileName = fn;
	}

	std::string getFileName() const
	{
		return _fileName;
	}

private:
	std::vector< std::pair< std::wstring, std::wstring > > _localizationList;
	std::wstring _nativeLangPath;
	std::string _fileName;
};


class ThemeSwitcher final
{
friend class NppParameters;

public:
	void addThemeFromXml(generic_string xmlFullPath)
	{
		_themeList.push_back(std::pair<generic_string, generic_string>(getThemeFromXmlFileName(xmlFullPath.c_str()), xmlFullPath));
	}

	void addDefaultThemeFromXml(generic_string xmlFullPath)
	{
		_themeList.push_back(std::pair<generic_string, generic_string>(TEXT("Default (stylers.xml)"), xmlFullPath));
	}

	generic_string getThemeFromXmlFileName(const TCHAR *fn) const;

	generic_string getXmlFilePathFromThemeName(const TCHAR *themeName) const
	{
		if (!themeName || themeName[0])
			return generic_string();
		generic_string themePath = _stylesXmlPath;
		return themePath;
	}

	bool themeNameExists(const TCHAR *themeName)
	{
		for (size_t i = 0; i < _themeList.size(); ++i )
		{
			if (! (getElementFromIndex(i)).first.compare(themeName))
				return true;
		}
		return false;
	}

	size_t size() const
	{
		return _themeList.size();
	}


	std::pair<generic_string, generic_string> & getElementFromIndex(size_t index)
	{
		assert(index < _themeList.size());
		return _themeList[index];
	}

private:
	std::vector<std::pair<generic_string, generic_string>> _themeList;
	generic_string _stylesXmlPath;
};


class PluginList final
{
public :
	void add(generic_string fn, bool isInBL)
	{
		_list.push_back(std::pair<generic_string, bool>(fn, isInBL));
	}

private:
	std::vector<std::pair<generic_string, bool>>_list;
};


const int NB_LANG = 100;
const bool DUP = true;
const bool FREE = false;

const int RECENTFILES_SHOWFULLPATH = -1;
const int RECENTFILES_SHOWONLYFILENAME = 0;




class NppParameters final
{
public:
	static NppParameters * getInstance() {return _pSelf;};
	static LangType getLangIDFromStr(const TCHAR *langName);
	static generic_string getLocPathFromStr(const generic_string & localizationCode);

	bool load();
	bool reloadLang();
	bool reloadStylers(TCHAR *stylePath = nullptr);
	void destroyInstance();
	generic_string getSettingsFolder();

	bool _isTaskListRBUTTONUP_Active = false;
	int L_END;

	const NppGUI & getNppGUI() const {
		return _nppGUI;
	}

	const TCHAR * getWordList(LangType langID, int typeIndex) const
	{
		Lang *pLang = getLangFromID(langID);
		if (!pLang) return nullptr;

		return pLang->getWords(typeIndex);
	}


	Lang * getLangFromID(LangType langID) const
	{
		for (int i = 0 ; i < _nbLang ; ++i)
		{
			if ( _langList[i] && _langList[i]->_langID == langID )
				return _langList[i];
		}
		return nullptr;
	}

	Lang * getLangFromIndex(size_t i) const {
		return (i < size_t(_nbLang)) ? _langList[i] : nullptr;
	}

	int getNbLang() const {return _nbLang;};

	LangType getLangFromExt(const TCHAR *ext);

	const TCHAR * getLangExtFromName(const TCHAR *langName) const
	{
		for (int i = 0 ; i < _nbLang ; ++i)
		{
			if (_langList[i]->_langName == langName)
				return _langList[i]->_defaultExtList;
		}
		return nullptr;
	}

	const TCHAR * getLangExtFromLangType(LangType langType) const
	{
		for (int i = 0 ; i < _nbLang ; ++i)
		{
			if (_langList[i]->_langID == langType)
				return _langList[i]->_defaultExtList;
		}
		return nullptr;
	}

	int getNbLRFile() const {return _nbRecentFile;};

	generic_string *getLRFile(int index) const {
		return _LRFileList[index];
	};

	void setNbMaxRecentFile(int nb) {
		_nbMaxRecentFile = nb;
	};

	int getNbMaxRecentFile() const {return _nbMaxRecentFile;};

	void setPutRecentFileInSubMenu(bool doSubmenu) {
		_putRecentFileInSubMenu = doSubmenu;
	}

	bool putRecentFileInSubMenu() const {return _putRecentFileInSubMenu;};

	void setRecentFileCustomLength(int len) {
		_recentFileCustomLength = len;
	}

	int getRecentFileCustomLength() const {return _recentFileCustomLength;};


	const ScintillaViewParams& getSVP() const {
		return _svp;
	}

	bool writeRecentFileHistorySettings(int nbMaxFile = -1) const;
	bool writeHistory(const TCHAR *fullpath);

	bool writeProjectPanelsSettings() const;
	bool writeFileBrowserSettings(const std::vector<generic_string> & rootPath, const generic_string & latestSelectedItemPath) const;

	TiXmlNode* getChildElementByAttribut(TiXmlNode *pere, const TCHAR *childName, const TCHAR *attributName, const TCHAR *attributVal) const;

	bool writeScintillaParams();
	void createXmlTreeFromGUIParams();

	void writeStyles(LexerStylerArray & lexersStylers, StyleArray & globalStylers);
	bool insertTabInfo(const TCHAR *langName, int tabInfo);

	LexerStylerArray & getLStylerArray() {return _lexerStylerArray;};
	StyleArray & getGlobalStylers() {return _widgetStyleArray;};

	StyleArray & getMiscStylerArray() {return _widgetStyleArray;};
	GlobalOverride & getGlobalOverrideStyle() {return _nppGUI._globalOverride;};

	COLORREF getCurLineHilitingColour();
	void setCurLineHilitingColour(COLORREF colour2Set);

	void setFontList(HWND hWnd);
	bool isInFontList(const generic_string fontName2Search) const;
	const std::vector<generic_string>& getFontList() const { return _fontlist; }

	int getNbUserLang() const {return _nbUserLang;}
	UserLangContainer & getULCFromIndex(size_t i) {return *_userLangArray[i];};
	UserLangContainer * getULCFromName(const TCHAR *userLangName);

	int getNbExternalLang() const {return _nbExternalLang;};
	int getExternalLangIndexFromName(const TCHAR *externalLangName) const;

	ExternalLangContainer & getELCFromIndex(int i) {return *_externalLangArray[i];};

	bool ExternalLangHasRoom() const {return _nbExternalLang < NB_MAX_EXTERNAL_LANG;};

	void getExternalLexerFromXmlTree(TiXmlDocument *doc);
	std::vector<TiXmlDocument *> * getExternalLexerDoc() { return &_pXmlExternalLexerDoc; };

	void writeUserDefinedLang();
	void writeShortcuts();
	void writeSession(const Session & session, const TCHAR *fileName = NULL);
	bool writeFindHistory();

	bool isExistingUserLangName(const TCHAR *newName) const
	{
		if ((!newName) || (!newName[0]))
			return true;

		for (int i = 0 ; i < _nbUserLang ; ++i)
		{
			if (!lstrcmp(_userLangArray[i]->_name.c_str(), newName))
				return true;
		}
		return false;
	}

	const TCHAR * getUserDefinedLangNameFromExt(TCHAR *ext, TCHAR *fullName) const;

	int addUserLangToEnd(const UserLangContainer & userLang, const TCHAR *newName);
	void removeUserLang(size_t index);

	bool isExistingExternalLangName(const TCHAR *newName) const;

	int addExternalLangToEnd(ExternalLangContainer * externalLang);

	TiXmlDocumentA * getNativeLangA() const {return _pXmlNativeLangDocA;};

	TiXmlDocument * getToolIcons() const {return _pXmlToolIconsDoc;};

	bool isTransparentAvailable() const {
		return (_transparentFuncAddr != NULL);
	}

	// 0 <= percent < 256
	// if (percent == 255) then opacq
	void SetTransparent(HWND hwnd, int percent);

	void removeTransparent(HWND hwnd);

	void setCmdlineParam(const CmdLineParams & cmdLineParams)
	{
		_cmdLineParams = cmdLineParams;
	}
	CmdLineParams & getCmdLineParams() {return _cmdLineParams;};

	void setFileSaveDlgFilterIndex(int ln) {_fileSaveDlgFilterIndex = ln;};
	int getFileSaveDlgFilterIndex() const {return _fileSaveDlgFilterIndex;};

	bool isRemappingShortcut() const {return _shortcuts.size() != 0;};

	std::vector<CommandShortcut> & getUserShortcuts() { return _shortcuts; };
	std::vector<size_t> & getUserModifiedShortcuts() { return _customizedShortcuts; };
	void addUserModifiedIndex(size_t index);

	std::vector<MacroShortcut> & getMacroList() { return _macros; };
	std::vector<UserCommand> & getUserCommandList() { return _userCommands; };
	std::vector<PluginCmdShortcut> & getPluginCommandList() { return _pluginCommands; };
	std::vector<size_t> & getPluginModifiedKeyIndices() { return _pluginCustomizedCmds; };
	void addPluginModifiedIndex(size_t index);

	std::vector<ScintillaKeyMap> & getScintillaKeyList() { return _scintillaKeyCommands; };
	std::vector<int> & getScintillaModifiedKeyIndices() { return _scintillaModifiedKeyIndices; };
	void addScintillaModifiedIndex(int index);

	std::vector<MenuItemUnit> & getContextMenuItems() { return _contextMenuItems; };
	const Session & getSession() const {return _session;};

	bool hasCustomContextMenu() const {return !_contextMenuItems.empty();};

	void setAccelerator(Accelerator *pAccel) {_pAccelerator = pAccel;};
	Accelerator * getAccelerator() {return _pAccelerator;};
	void setScintillaAccelerator(ScintillaAccelerator *pScintAccel) {_pScintAccelerator = pScintAccel;};
	ScintillaAccelerator * getScintillaAccelerator() {return _pScintAccelerator;};

	generic_string getNppPath() const {return _nppPath;};
	generic_string getContextMenuPath() const {return _contextMenuPath;};
	const TCHAR * getAppDataNppDir() const {return _appdataNppDir.c_str();};
	const TCHAR * getLocalAppDataNppDir() const { return _localAppdataNppDir.c_str(); };
	const TCHAR * getWorkingDir() const {return _currentDirectory.c_str();};
	const TCHAR * getWorkSpaceFilePath(int i) const {
		if (i < 0 || i > 2) return nullptr;
		return _workSpaceFilePathes[i].c_str();
	}
	const std::vector<generic_string> getFileBrowserRoots() const { return _fileBrowserRoot; };
	void setWorkSpaceFilePath(int i, const TCHAR *wsFile);

	void setWorkingDir(const TCHAR * newPath);

	void setStartWithLocFileName(generic_string locPath) {
		_startWithLocFileName = locPath;
	};

	void setFunctionListExportBoolean(bool doIt) {
		_doFunctionListExport = doIt;
	};
	bool doFunctionListExport() const {
		return _doFunctionListExport;
	};

	void setPrintAndExitBoolean(bool doIt) {
		_doPrintAndExit = doIt;
	};
	bool doPrintAndExit() const {
		return _doPrintAndExit;
	};

	bool loadSession(Session & session, const TCHAR *sessionFileName);
	int langTypeToCommandID(LangType lt) const;
	WNDPROC getEnableThemeDlgTexture() const {return _enableThemeDialogTextureFuncAddr;};

	struct FindDlgTabTitiles final {
		generic_string _find;
		generic_string _replace;
		generic_string _findInFiles;
		generic_string _mark;
	};

	FindDlgTabTitiles & getFindDlgTabTitiles() { return _findDlgTabTitiles;};

	bool asNotepadStyle() const {return _asNotepadStyle;};

	bool reloadPluginCmds() {
		return getPluginCmdsFromXmlTree();
	}

	bool getContextMenuFromXmlTree(HMENU mainMenuHadle, HMENU pluginsMenu);
	bool reloadContextMenuFromXmlTree(HMENU mainMenuHadle, HMENU pluginsMenu);
	winVer getWinVersion() const {return _winVersion;};
	generic_string getWinVersionStr() const;
	generic_string getWinVerBitStr() const;
	FindHistory & getFindHistory() {return _findHistory;};
	bool _isFindReplacing = false; // an on the fly variable for find/replace functions
	void safeWow64EnableWow64FsRedirection(BOOL Wow64FsEnableRedirection);

	LocalizationSwitcher & getLocalizationSwitcher() {
		return _localizationSwitcher;
	}

	ThemeSwitcher & getThemeSwitcher() {
		return _themeSwitcher;
	}

	std::vector<generic_string> & getBlackList() { return _blacklist; };
	bool isInBlackList(TCHAR *fn) const
	{
		for (auto& element: _blacklist)
		{
			if (element == fn)
				return true;
		}
		return false;
	}

	PluginList & getPluginList() {return _pluginList;};
	bool importUDLFromFile(generic_string sourceFile);
	bool exportUDLToFile(size_t langIndex2export, generic_string fileName2save);
	NativeLangSpeaker* getNativeLangSpeaker() {
		return _pNativeLangSpeaker;
	}
	void setNativeLangSpeaker(NativeLangSpeaker *nls) {
		_pNativeLangSpeaker = nls;
	}

	bool isLocal() const {
		return _isLocal;
	};

	void saveConfig_xml();

	generic_string getUserPath() const {
		return _userPath;
	}

	bool writeSettingsFilesOnCloudForThe1stTime(const generic_string & cloudSettingsPath);
	void setCloudChoice(const TCHAR *pathChoice);
	void removeCloudChoice();
	bool isCloudPathChanged() const;
	bool isx64() const { return _isx64; };

	COLORREF getCurrentDefaultBgColor() const {
		return _currentDefaultBgColor;
	}

	COLORREF getCurrentDefaultFgColor() const {
		return _currentDefaultFgColor;
	}

	void setCurrentDefaultBgColor(COLORREF c) {
		_currentDefaultBgColor = c;
	}

	void setCurrentDefaultFgColor(COLORREF c) {
		_currentDefaultFgColor = c;
	}

	bool useNewStyleSaveDlg() const {
		return _nppGUI._useNewStyleSaveDlg;
	}

	void setUseNewStyleSaveDlg(bool v) {
		_nppGUI._useNewStyleSaveDlg = v;
	}
	DPIManager _dpiManager;

	generic_string static getSpecialFolderLocation(int folderKind);


private:
	NppParameters();
	~NppParameters();

	static NppParameters *_pSelf;

	TiXmlDocument *_pXmlDoc = nullptr;
	TiXmlDocument *_pXmlUserDoc = nullptr;
	TiXmlDocument *_pXmlUserStylerDoc = nullptr;
	TiXmlDocument *_pXmlUserLangDoc = nullptr;
	TiXmlDocument *_pXmlToolIconsDoc = nullptr;
	TiXmlDocument *_pXmlShortcutDoc = nullptr;
	TiXmlDocument *_pXmlSessionDoc = nullptr;
	TiXmlDocument *_pXmlBlacklistDoc = nullptr;
	
	TiXmlDocument *_importedULD[NB_MAX_IMPORTED_UDL];

	TiXmlDocumentA *_pXmlNativeLangDocA = nullptr;
	TiXmlDocumentA *_pXmlContextMenuDocA = nullptr;
	
	int _nbImportedULD;



	std::vector<TiXmlDocument *> _pXmlExternalLexerDoc;

	NppGUI _nppGUI;
	ScintillaViewParams _svp;
	Lang *_langList[NB_LANG];
	int _nbLang = 0;

	// Recent File History
	generic_string *_LRFileList[NB_MAX_LRF_FILE];
	int _nbRecentFile = 0;
	int _nbMaxRecentFile = 10;
	bool _putRecentFileInSubMenu = false;
	int _recentFileCustomLength = RECENTFILES_SHOWFULLPATH;	//	<0: Full File Path Name
															//	=0: Only File Name
															//	>0: Custom Entry Length

	FindHistory _findHistory;

	UserLangContainer *_userLangArray[NB_MAX_USER_LANG];
	int _nbUserLang = 0;
	generic_string _userDefineLangPath;
	ExternalLangContainer *_externalLangArray[NB_MAX_EXTERNAL_LANG];
	int _nbExternalLang = 0;

	CmdLineParams _cmdLineParams;

	int _fileSaveDlgFilterIndex = -1;

	// All Styles (colours & fonts)
	LexerStylerArray _lexerStylerArray;
	StyleArray _widgetStyleArray;

	std::vector<generic_string> _fontlist;
	std::vector<generic_string> _blacklist;
	PluginList _pluginList;

	HMODULE _hUXTheme = nullptr;

	WNDPROC _transparentFuncAddr = nullptr;
	WNDPROC _enableThemeDialogTextureFuncAddr = nullptr;
	bool _isLocal;
	bool _isx64 = false; // by default 32-bit

public:
	void setShortcutDirty() { _isAnyShortcutModified = true; };
private:
	bool _isAnyShortcutModified = false;
	std::vector<CommandShortcut> _shortcuts;			//main menu shortuts. Static size
	std::vector<size_t> _customizedShortcuts;			//altered main menu shortcuts. Indices static. Needed when saving alterations
	std::vector<MacroShortcut> _macros;				//macro shortcuts, dynamic size, defined on loading macros and adding/deleting them
	std::vector<UserCommand> _userCommands;			//run shortcuts, dynamic size, defined on loading run commands and adding/deleting them
	std::vector<PluginCmdShortcut> _pluginCommands;	//plugin commands, dynamic size, defined on loading plugins
	std::vector<size_t> _pluginCustomizedCmds;			//plugincommands that have been altered. Indices determined after loading ALL plugins. Needed when saving alterations

	std::vector<ScintillaKeyMap> _scintillaKeyCommands;	//scintilla keycommands. Static size
	std::vector<int> _scintillaModifiedKeyIndices;		//modified scintilla keys. Indices static, determined by searching for commandId. Needed when saving alterations

	LocalizationSwitcher _localizationSwitcher;
	generic_string _startWithLocFileName;
	bool _doFunctionListExport = false;
	bool _doPrintAndExit = false;

	ThemeSwitcher _themeSwitcher;

	//vector<generic_string> _noMenuCmdNames;
	std::vector<MenuItemUnit> _contextMenuItems;
	Session _session;

	generic_string _shortcutsPath;
	generic_string _contextMenuPath;
	generic_string _sessionPath;
	generic_string _blacklistPath;
	generic_string _nppPath;
	generic_string _userPath;
	generic_string _stylerPath;
	generic_string _appdataNppDir; // sentinel of the absence of "doLocalConf.xml" : (_appdataNppDir == TEXT(""))?"doLocalConf.xml present":"doLocalConf.xml absent"
	generic_string _localAppdataNppDir; // for plugins
	generic_string _currentDirectory;
	generic_string _workSpaceFilePathes[3];

	std::vector<generic_string> _fileBrowserRoot;

	Accelerator *_pAccelerator;
	ScintillaAccelerator * _pScintAccelerator;

	FindDlgTabTitiles _findDlgTabTitiles;
	bool _asNotepadStyle = false;

	winVer _winVersion;
	Platform _platForm;

	NativeLangSpeaker *_pNativeLangSpeaker = nullptr;

	COLORREF _currentDefaultBgColor;
	COLORREF _currentDefaultFgColor;

	generic_string _initialCloudChoice;

	void getLangKeywordsFromXmlTree();
	bool getUserParametersFromXmlTree();
	bool getUserStylersFromXmlTree();
	bool getUserDefineLangsFromXmlTree(TiXmlDocument *tixmldoc);
	bool getUserDefineLangsFromXmlTree()
	{
		return getUserDefineLangsFromXmlTree(_pXmlUserLangDoc);
	}

	bool getShortcutsFromXmlTree();

	bool getMacrosFromXmlTree();
	bool getUserCmdsFromXmlTree();
	bool getPluginCmdsFromXmlTree();
	bool getScintKeysFromXmlTree();
	bool getSessionFromXmlTree(TiXmlDocument *pSessionDoc = NULL, Session *session = NULL);
	bool getBlackListFromXmlTree();

	void feedGUIParameters(TiXmlNode *node);
	void feedKeyWordsParameters(TiXmlNode *node);
	void feedFileListParameters(TiXmlNode *node);
	void feedScintillaParam(TiXmlNode *node);
	void feedDockingManager(TiXmlNode *node);
	void feedFindHistoryParameters(TiXmlNode *node);
	void feedProjectPanelsParameters(TiXmlNode *node);
	void feedFileBrowserParameters(TiXmlNode *node);
	bool feedStylerArray(TiXmlNode *node);
	bool feedUserLang(TiXmlNode *node);
	void feedUserStyles(TiXmlNode *node);
	void feedUserKeywordList(TiXmlNode *node);
	void feedUserSettings(TiXmlNode *node);
	void feedShortcut(TiXmlNode *node);
	void feedMacros(TiXmlNode *node);
	void feedUserCmds(TiXmlNode *node);
	void feedPluginCustomizedCmds(TiXmlNode *node);
	void feedScintKeys(TiXmlNode *node);
	bool feedBlacklist(TiXmlNode *node);

	void getActions(TiXmlNode *node, Macro & macro);
	bool getShortcuts(TiXmlNode *node, Shortcut & sc);

	void writeStyle2Element(Style & style2Write, Style & style2Sync, TiXmlElement *element);
	void insertUserLang2Tree(TiXmlNode *node, UserLangContainer *userLang);
	void insertCmd(TiXmlNode *cmdRoot, const CommandShortcut & cmd);
	void insertMacro(TiXmlNode *macrosRoot, const MacroShortcut & macro);
	void insertUserCmd(TiXmlNode *userCmdRoot, const UserCommand & userCmd);
	void insertScintKey(TiXmlNode *scintKeyRoot, const ScintillaKeyMap & scintKeyMap);
	void insertPluginCmd(TiXmlNode *pluginCmdRoot, const PluginCmdShortcut & pluginCmd);
	void stylerStrOp(bool op);
	TiXmlElement * insertGUIConfigBoolNode(TiXmlNode *r2w, const TCHAR *name, bool bVal);
	void insertDockingParamNode(TiXmlNode *GUIRoot);
	void writeExcludedLangList(TiXmlElement *element);
	void writePrintSetting(TiXmlElement *element);
	void initMenuKeys();		//initialise menu keys and scintilla keys. Other keys are initialized on their own
	void initScintillaKeys();	//these functions have to be called first before any modifications are loaded
	int getCmdIdFromMenuEntryItemName(HMENU mainMenuHadle, generic_string menuEntryName, generic_string menuItemName); // return -1 if not found
	int getPluginCmdIdFromMenuEntryItemName(HMENU pluginsMenu, generic_string pluginName, generic_string pluginCmdName); // return -1 if not found
	winVer getWindowsVersion();
};
