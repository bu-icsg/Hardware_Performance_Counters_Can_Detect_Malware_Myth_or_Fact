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
#pragma once
#include <mutex>

#include "Utf8_16.h"



class Notepad_plus;
class Buffer;
typedef Buffer* BufferID;	//each buffer has unique ID by which it can be retrieved
#define BUFFER_INVALID	reinterpret_cast<BufferID>(0)

typedef sptr_t Document;

enum DocFileStatus
{
	DOC_REGULAR    = 0x01, // should not be combined with anything
	DOC_UNNAMED    = 0x02, // not saved (new ##)
	DOC_DELETED    = 0x04, // doesn't exist in environment anymore, but not DOC_UNNAMED
	DOC_MODIFIED   = 0x08, // File in environment has changed
	DOC_NEEDRELOAD = 0x10  // File is modified & needed to be reload (by log monitoring)
};

enum BufferStatusInfo
{
	BufferChangeLanguage	= 0x001,  // Language was altered
	BufferChangeDirty		= 0x002,  // Buffer has changed dirty state
	BufferChangeFormat		= 0x004,  // EOL type was changed
	BufferChangeUnicode		= 0x008,  // Unicode type was changed
	BufferChangeReadonly	= 0x010,  // Readonly state was changed, can be both file and user
	BufferChangeStatus		= 0x020,  // Filesystem Status has changed
	BufferChangeTimestamp	= 0x040,  // Timestamp was changed
	BufferChangeFilename	= 0x080,  // Filename was changed
	BufferChangeRecentTag	= 0x100,  // Recent tag has changed
	BufferChangeLexing		= 0x200,  // Document needs lexing
	BufferChangeMask		= 0x3FF   // Mask: covers all changes
};

//const int userLangNameMax = 16;
const TCHAR UNTITLED_STR[] = TEXT("new ");



//File manager class maintains all buffers
class FileManager final
{
public:
	void init(Notepad_plus* pNotepadPlus, ScintillaEditView* pscratchTilla);

	//void activateBuffer(int index);
	void checkFilesystemChanges();

	size_t getNbBuffers() { return _nbBufs; };
	int getBufferIndexByID(BufferID id);
	Buffer * getBufferByIndex(size_t index);
	Buffer * getBufferByID(BufferID id) {return static_cast<Buffer*>(id);}

	void beNotifiedOfBufferChange(Buffer * theBuf, int mask);

	void closeBuffer(BufferID, ScintillaEditView * identifer);		//called by Notepad++

	void addBufferReference(BufferID id, ScintillaEditView * identifer);	//called by Scintilla etc indirectly

	BufferID loadFile(const TCHAR * filename, Document doc = NULL, int encoding = -1, const TCHAR *backupFileName = NULL, time_t fileNameTimestamp = 0);	//ID == BUFFER_INVALID on failure. If Doc == NULL, a new file is created, otherwise data is loaded in given document
	BufferID newEmptyDocument();
	//create Buffer from existing Scintilla, used from new Scintillas. If dontIncrease = true, then the new document number isnt increased afterwards.
	//usefull for temporary but neccesary docs
	//If dontRef = false, then no extra reference is added for the doc. Its the responsibility of the caller to do so
	BufferID bufferFromDocument(Document doc,  bool dontIncrease = false, bool dontRef = false);

	BufferID getBufferFromName(const TCHAR * name);
	BufferID getBufferFromDocument(Document doc);

	bool reloadBuffer(BufferID id);
	bool reloadBufferDeferred(BufferID id);
	bool saveBuffer(BufferID id, const TCHAR* filename, bool isCopy = false, generic_string * error_msg = NULL);
	bool backupCurrentBuffer();
	bool deleteCurrentBufferBackup();
	bool deleteFile(BufferID id);
	bool moveFile(BufferID id, const TCHAR * newFilename);
	bool createEmptyFile(const TCHAR * path);
	static FileManager * getInstance() {return _pSelf;};
	void destroyInstance() { delete _pSelf; };
	int getFileNameFromBuffer(BufferID id, TCHAR * fn2copy);
	int docLength(Buffer * buffer) const;
	size_t nextUntitledNewNumber() const;


private:
	~FileManager();
	int detectCodepage(char* buf, size_t len);
	bool loadFileData(Document doc, const TCHAR* filename, char* buffer, Utf8_16_Read* UnicodeConvertor, LangType & language, int & encoding, EolType & eolFormat);
	LangType detectLanguageFromTextBegining(const unsigned char *data, size_t dataLen);


private:
	static FileManager *_pSelf;

	Notepad_plus* _pNotepadPlus = nullptr;
	ScintillaEditView* _pscratchTilla = nullptr;
	Document _scratchDocDefault;
	std::vector<Buffer*> _buffers;
	BufferID _nextBufferID = 0;
	size_t _nbBufs = 0;
};

#define MainFileManager FileManager::getInstance()

class Buffer final
{
	friend class FileManager;
public:
	//Loading a document:
	//constructor with ID.
	//Set a reference (pointer to a container mostly, like DocTabView or ScintillaEditView)
	//Set the position manually if needed
	//Load the document into Scintilla/add to TabBar
	//The entire lifetime if the buffer, the Document has reference count of _atleast_ one
	//Destructor makes sure its purged
	Buffer(FileManager * pManager, BufferID id, Document doc, DocFileStatus type, const TCHAR *fileName);

	// this method 1. copies the file name
	//             2. determinates the language from the ext of file name
	//             3. gets the last modified time
	void setFileName(const TCHAR *fn, LangType defaultLang = L_TEXT);

	const TCHAR * getFullPathName() const {
		return _fullPathName.c_str();
	}

	const TCHAR * getFileName() const { return _fileName; }

	BufferID getID() const { return _id; }

	void increaseRecentTag() {
		_recentTag = ++_recentTagCtr;
		doNotify(BufferChangeRecentTag);
	}

	long getRecentTag() const { return _recentTag; }

	bool checkFileState();

	bool isDirty() const {
		return _isDirty;
	}

	bool isReadOnly() const {
		return (_isUserReadOnly || _isFileReadOnly);
	};

	bool isUntitled() const {
		return (_currentStatus == DOC_UNNAMED);
	}

	bool getFileReadOnly() const {
		return _isFileReadOnly;
	}

	void setFileReadOnly(bool ro) {
		_isFileReadOnly = ro;
		doNotify(BufferChangeReadonly);
	}

	bool getUserReadOnly() const {
		return _isUserReadOnly;
	}

	void setUserReadOnly(bool ro) {
		_isUserReadOnly = ro;
		doNotify(BufferChangeReadonly);
	}

	EolType getEolFormat() const {
		return _eolFormat;
	}

	void setEolFormat(EolType format) {
		_eolFormat = format;
		doNotify(BufferChangeFormat);
	}

	LangType getLangType() const {
		return _lang;
	}

	void setLangType(LangType lang, const TCHAR * userLangName = TEXT(""));

	UniMode getUnicodeMode() const {
		return _unicodeMode;
	}

	void setUnicodeMode(UniMode mode);

	int getEncoding() const {
		return _encoding;
	}

	void setEncoding(int encoding);

	DocFileStatus getStatus() const {
		return _currentStatus;
	}

	Document getDocument() {
		return _doc;
	}

	void setDirty(bool dirty);

	void setPosition(const Position & pos, ScintillaEditView * identifier);
	Position & getPosition(ScintillaEditView * identifier);

	void setHeaderLineState(const std::vector<size_t> & folds, ScintillaEditView * identifier);
	const std::vector<size_t> & getHeaderLineState(const ScintillaEditView * identifier) const;

	bool isUserDefineLangExt() const
	{
		return (_userLangExt[0] != '\0');
	}

	const TCHAR * getUserDefineLangName() const
	{
		return _userLangExt.c_str();
	}

	const TCHAR * getCommentLineSymbol() const
	{
		Lang *l = getCurrentLang();
		if (!l)
			return NULL;
		return l->_pCommentLineSymbol;
	}

	const TCHAR * getCommentStart() const
	{
		Lang *l = getCurrentLang();
		if (!l)
			return NULL;
		return l->_pCommentStart;
	}

	const TCHAR * getCommentEnd() const
	{
		Lang *l = getCurrentLang();
		if (!l)
			return NULL;
		return l->_pCommentEnd;
	}

	bool getNeedsLexing() const
	{
		return _needLexer;
	}

	void setNeedsLexing(bool lex)
	{
		_needLexer = lex;
		doNotify(BufferChangeLexing);
	}

	//these two return reference count after operation
	int addReference(ScintillaEditView * identifier);		//if ID not registered, creates a new Position for that ID and new foldstate
	int removeReference(ScintillaEditView * identifier);		//reduces reference. If zero, Document is purged

	void setHideLineChanged(bool isHide, int location);

	void setDeferredReload();

	bool getNeedReload()
	{
		return _needReloading;
	}

	void setNeedReload(bool reload)
	{
		_needReloading = reload;
	}

	int docLength() const
	{
		assert(_pManager != nullptr);
		return _pManager->docLength(_id);
	}

	int getFileLength() const; // return file length. -1 if file is not existing.

	enum fileTimeType { ft_created, ft_modified, ft_accessed };
	generic_string getFileTime(fileTimeType ftt) const;

	Lang * getCurrentLang() const;

	bool isModified() const { return _isModified; }
	void setModifiedStatus(bool isModified) { _isModified = isModified; }
	generic_string getBackupFileName() const { return _backupFileName; }
	void setBackupFileName(generic_string fileName) { _backupFileName = fileName; }
	time_t getLastModifiedTimestamp() const { return _timeStamp; }

	bool isLoadedDirty() const
	{
		return _isLoadedDirty;
	}

	void setLoadedDirty(bool val)
	{
		_isLoadedDirty = val;
	}

	void startMonitoring() { 
		_isMonitoringOn = true; 
		_eventHandle = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	};

	HANDLE getMonitoringEvent() const {
		return _eventHandle;
	};

	void stopMonitoring() { 
		_isMonitoringOn = false;
		::SetEvent(_eventHandle);
		::CloseHandle(_eventHandle);
	};

	bool isMonitoringOn() const { return _isMonitoringOn; };
	void updateTimeStamp();
	void reload();
	void setMapPosition(const MapPosition & mapPosition) { _mapPosition = mapPosition; };
	MapPosition getMapPosition() const { return _mapPosition; };

	void langHasBeenSetFromMenu() { _hasLangBeenSetFromMenu = true; };

private:
	int indexOfReference(const ScintillaEditView * identifier) const;

	void setStatus(DocFileStatus status) {
		_currentStatus = status;
		doNotify(BufferChangeStatus);
	}

	void doNotify(int mask);

	Buffer(const Buffer&) = delete;
	Buffer& operator = (const Buffer&) = delete;


private:
	FileManager * _pManager = nullptr;
	bool _canNotify = false;
	int _references = 0; // if no references file inaccessible, can be closed
	BufferID _id = nullptr;

	//document properties
	Document _doc;	//invariable
	LangType _lang;
	generic_string _userLangExt; // it's useful if only (_lang == L_USER)
	bool _isDirty = false;
	EolType _eolFormat = EolType::osdefault;
	UniMode _unicodeMode;
	int _encoding = -1;
	bool _isUserReadOnly = false;
	bool _needLexer = false; // new buffers do not need lexing, Scintilla takes care of that
	//these properties have to be duplicated because of multiple references
	//All the vectors must have the same size at all times
	std::vector<ScintillaEditView *> _referees; // Instances of ScintillaEditView which contain this buffer
	std::vector<Position> _positions;
	std::vector<std::vector<size_t>> _foldStates;

	//Environment properties
	DocFileStatus _currentStatus;
	time_t _timeStamp = 0; // 0 if it's a new doc

	bool _isFileReadOnly = false;
	generic_string _fullPathName;
	TCHAR * _fileName = nullptr; // points to filename part in _fullPathName
	bool _needReloading = false; // True if Buffer needs to be reloaded on activation

	long _recentTag = -1;
	static long _recentTagCtr;

	// For backup system
	generic_string _backupFileName;
	bool _isModified = false;
	bool _isLoadedDirty = false; // it's the indicator for finding buffer's initial state

	// For the monitoring
	HANDLE _eventHandle = nullptr;
	bool _isMonitoringOn = false;

	bool _hasLangBeenSetFromMenu = false;

	MapPosition _mapPosition;

	std::mutex _reloadFromDiskRequestGuard;
};