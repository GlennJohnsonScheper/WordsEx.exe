// This is file: CWww.h
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
class CWww
{
public:
    CWww( );
    ~CWww( );
    int LoadWinInetDll( );
    void UserUrlError( wchar_t * szSystemCommand );
    int FixUpUserUrl( wchar_t * * NewMallocPtr, wchar_t * MallocPtr );
    int FirstTimeSetupInternet( );
    void AdjustInternetOptions( );

    void RunOneFetchThread( size_t Index );
    void ObtainOneURLContent( COnePaper * pOnePaper, CAsb * pAsbOutput, size_t ClaimedUrlIndex, CBud * pBudLog, int BinaryOkay );
    void AddRejectionToLog( CBud * pBudLog, wchar_t * szUrl, wchar_t * szReason );
    void AddInternetErrorToLog( CBud * pBudLog, wchar_t * szUrl, wchar_t * szOperation );
    void AddCachePhaseOne( );
    void AddCachePhaseTwo( );
    void LoadInternetCacheItem( LPINTERNET_CACHE_ENTRY_INFO  InfoBufferPtr );
    void ReadInternetCacheItem( CAsb * pAsbOutput, wchar_t * FullKey );
    void ParseHttpHeader( COnePaper * pOnePaper, size_t ClaimedUrlIndex, wchar_t * lpOutBuffer, size_t nLength );
    int CombineAndHangUrl( wchar_t * szBaseUrl, wchar_t * szLinkUrl, int ForForm );
    int TestPaperHttpHeaderValuesAcceptable( COnePaper * pOnePaper, CWsb * pWsbCritique, int BinaryOkay );
    // SED_PILE1
    void RecursiveGetter( int Recursions, size_t UrlIndex, CBud * pBudLog, wchar_t * szBinaryPath, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int FromEngineNo, int PageIsAQrp, wchar_t * szQueryResultUrl, int bTopUrl, COneSurl * pOneSurl );
    size_t CommonGetter( size_t UrlIndex, CBud * pBudLog, wchar_t * szBinaryPath, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int FromEngineNo, int PageIsAQrp, wchar_t * szQueryResultUrl, int bTopUrl, COneSurl * pOneSurl );
    size_t GetterHelper( size_t UrlIndexToClaim, CBud * pBudLog, wchar_t * szBinaryPath, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int FromEngineNo, int PageIsAQrp, int bTopUrl, COneSurl * pOneSurl );
    // SED_PILE2
    void RunOneQueryThread( size_t QueryIndex );
    void PerformOneQuery( int QRPNo, COneQuery * pQuery, wchar_t * szUrl, COneSurl * pOneSurl, CSol * pSolMore, CSol * pSolHits );
    int RejectableQueryUrl( wchar_t * szPageUrl, wchar_t * szHitUrl );
    int RejectableHitUrl( CSol * pSol, wchar_t * szHitUrl, int ToAnyDot );
    void MarkAsRejectedUrl( size_t UrlIndexToClaim );

    int m_FirstTimeCalled;
    int m_ConnectedOkay;
    HINTERNET m_hInternet; // opened handle pointer, or NULL
private:
    int m_TotalCacheFiles;
    int m_GoodCacheFiles;
    int m_GoodCacheBytes;
};


// I notice that my T-Mobile Pocket PC Phone has a Wininet.dll.
extern HINSTANCE g_hWinInet;     // Loaded WININET.DLL


// DLL calling syntax is easiest to express with a typedef:

//  First routine:
typedef HANDLE ( __stdcall * FIND_FIRST_URL_ETC ) (
    LPCTSTR lpszUrlSearchPattern,
    LPINTERNET_CACHE_ENTRY_INFO lpFirstCacheEntryInfo,
    LPDWORD lpdwFirstCacheEntryInfoBufferSize
 );
extern FIND_FIRST_URL_ETC pFindFirstUrlCacheEntry; // Pointer to DLL routine

//  Next routine:
typedef BOOL ( __stdcall * FIND_NEXT_URL_ETC ) (
    HANDLE hEnumHandle,
    LPINTERNET_CACHE_ENTRY_INFO lpNextCacheEntryInfo,
    LPDWORD lpdwNextCacheEntryInfoBufferSize
 );
extern FIND_NEXT_URL_ETC pFindNextUrlCacheEntry; // Pointer to DLL routine


// However, while these names linked okay on VC++6 Desktop,
// eVC++3 / PPC 2020 sdk said unresolved external symbols:
// Perhaps I have to load their entry points in Wininet DLL?
// No, they were not there at all. But that works on desktop.
// - InternetAttemptConnect
// - InternetGoOnline
// - InternetGetConnectedState


// typedef DWORD ( __stdcall * INTERNET_ATTEMPT_CONNECT ) (
//     DWORD dwReserved
// );
// extern INTERNET_ATTEMPT_CONNECT pInternetAttemptConnect; // Pointer to DLL routine


// typedef BOOL  ( __stdcall * INTERNET_GET_CONNECTED_STATE ) (
//     LPDWORD lpdwFlags,
//     DWORD dwReserved
// );
// extern INTERNET_GET_CONNECTED_STATE pInternetGetConnectedState; // Pointer to DLL routine


// typedef BOOL ( __stdcall * INTERNET_GO_ONLINE ) (
//     LPTSTR lpszURL,
//     HWND hwndParent,
//     DWORD dwReserved
// );
// extern INTERNET_GO_ONLINE pInternetGoOnline; // Pointer to DLL routine

