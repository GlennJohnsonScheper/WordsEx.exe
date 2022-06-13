// This is file: CFio.h
// Copyright ( C ) 2006, Glenn Scheper

class CAll; // Globals
class CAsb; // ASCII string buffer
class CBud; // Base of sorted fruit
class CFio; // Local file in / out
class CHtm; // Parses html into text
class CIni; // Initiate operations
class CInt; // Integer list
class CIdx; // Text Indexing offsets
class CFwd; // Find Words in texts
class CTop; // Holds top-level views
class CRev; // Holds Book of Revelation
class CUse; // Holds Help Usage text
class CPag; // CodePage conversions
class CSee; // Displays, navigation
class CTxt; // Parses text into sense
class CSol; // Holds one sorted list
class CLin; // Link cross-ref info
class CWsb; // Wide string buffer
class CWww; // Web and cache access
class CPor; // Porter stemming
class CUrl; // Search Engine Urls
class COme; // Omega painting


class COneFetch; // Controls one Add One Web Page or Add Links thread
class COneQuery; // Controls one Add Internet Search thread
class COneKwic; // Controls one Add Word thread
class COneCache; // Controls one Add All Files in Cache thread
class COnePhrases; // Controls one Add Ranked Phrases thread
class COneFolder; // Controls one Add Directory thread

class COnePaper; // For each HTML or TEXT source page
class COneForm; // For each HTML FORM encountered
class COneSurl; // For each GET search engine listed

// Since FILE_ATTRIBUTE_NORMAL yielded no files, work opposite:
#ifdef _WIN32_WCE
#define FILE_ATTRIBUTE_ABNORMAL ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_INROM | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_ROMMODULE | FILE_ATTRIBUTE_ROMSTATICREF | FILE_ATTRIBUTE_ROMMODULE | FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY )
#else // not _WIN32_WCE
#define FILE_ATTRIBUTE_ABNORMAL ( FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_HIDDEN |  FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY )
#endif // _WIN32_WCE


class CFio
{
public:
    CFio( );
    ~CFio( );
    int FolderPathIsValid( wchar_t * szFolderPath );
    int MakeFoldersHelper( wchar_t * szFolderPath );
    int MakeFoldersPaths( wchar_t * szFolderPath );
    wchar_t * PromptForSaveAsAnyFileName( wchar_t * szSuggestion, wchar_t * szExtension );
    void SaveFolder( wchar_t * szFolderPath );
    void SaveFile( );
    void SaveUrlToFile( wchar_t * szFileName, size_t UrlIndex );
    void CommonFileSave( wchar_t * szFileName, BYTE * pMallocBuf, size_t nMallocBuf );
    void CommonPaperFileInput( wchar_t * szFileName, CBud * pBudLog );
    void AddFile( );
    void AddFolderP1RecursivePart( COneFolder * pFolder, wchar_t * szCurFolder );
    int AddFolderPhaseOne( size_t FolderIndex );
    void AddFolderPhaseTwo( size_t FolderIndex );
private:
};

extern CFio Fio;
