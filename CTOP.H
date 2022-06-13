// This is file: CTop.h
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
// Views - doubly linked list of fruit

typedef struct _Views
{
    struct _Views * Next;
    struct _Views * Back;
    CBud * pFruit;
    size_t FocusLineOffsetInText;
} Views;


class CTop
{
public:
    CTop( );
    ~CTop( );
    void Next( );
    void Back( );
    void FollowClickToAdd( CIdx * pIdxResultIndex, size_t Offset1 );
    void RemoveReferences( CBud * pFruit );
    void Add( CBud * pFruit );
    void AddLinks( );
    void AddUsage( );
    void AddEzekiel( );
    void AddTeruah( );
    void AddRevelation( );
    void AddRevNotes( );
    void UpdateViewIfOnScreen( void * pFruit );
    void ShowCurrentTopItemViewOnScreen( );
    void DeleteCurrentView( );
    Views * CurrentView;
    void SetBookmark( size_t n );
    void VisitBookmark( size_t n );
    void InvokeInternetExplorer( );
    void RewrapCurrentTopItemViewOnScreen( );
    void ShowSplashScreen( );
private:
    void DebugTop( wchar_t* Legend );
    void AddInternal( wchar_t * pWBuf, size_t nWBuf, wchar_t * szFileUrl );
};
