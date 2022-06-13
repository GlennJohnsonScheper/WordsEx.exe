// This is file: CIni.h
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
    // Win32 recommends using _beginthread when calling C runtime libs.
    // WinCE does not use _beginthread, so make a CreateThread version.


typedef struct _StartThreadPending
{
    WPARAM Type_Query1_Fetch2_Kwic3;
    LPARAM CThreadParam;
    struct _StartThreadPending * NextPending;
} START_THREAD_PENDING;


class CIni
{
public:
    CIni( );
    ~CIni( );
    void OnThreadEnding( WPARAM MyWParam, LPARAM MyLParam );
    void SaveThreadParamsPending( WPARAM MyWParam, LPARAM MyLParam );
    void InitiateSalts( );
    void InitiateQuery( size_t Index );
    void InitiateFetch( size_t Index );
    void InitiateKwic( size_t Index );
    void InitiateCache( );
    void InitiateFolder( size_t Index );
    void InitiatePhrases( );

private:

#ifdef _WIN32_WCE
    DWORD static __cdecl ProcessSaltsThread( void * ThreadParam );
    DWORD static __cdecl ProcessQueryThread( void * ThreadParam );
    DWORD static __cdecl ProcessFetchThread( void * ThreadParam );
    DWORD static __cdecl ProcessKwicThread( void * ThreadParam );
    DWORD static __cdecl ProcessCacheThread( void * ThreadParam );
    DWORD static __cdecl ProcessFolderThread( void * ThreadParam );
    DWORD static __cdecl ProcessPhrasesThread( void * ThreadParam );
#else
    void static __cdecl ProcessSaltsThread( void * ThreadParam );
    void static __cdecl ProcessQueryThread( void * ThreadParam );
    void static __cdecl ProcessFetchThread( void * ThreadParam );
    void static __cdecl ProcessKwicThread( void * ThreadParam );
    void static __cdecl ProcessCacheThread( void * ThreadParam );
    void static __cdecl ProcessFolderThread( void * ThreadParam );
    void static __cdecl ProcessPhrasesThread( void * ThreadParam );
#endif
};

// Globals
extern CIni Initiator; // one per application
extern START_THREAD_PENDING * pListStartThreadPending;

