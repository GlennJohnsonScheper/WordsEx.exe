// This is file: CWsb.h
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

class CWsb
{
public:
    CWsb( );
    ~CWsb( );
    void Reset( );
    void Add( wchar_t * szNew );
    void AddWsb( CWsb * pWsb );
    void Addn( wchar_t * szNew, size_t nNewLen );
    void AddnByte( BYTE * szNew, size_t nNewLen );
    size_t StrLen;
    wchar_t * GetBuffer( size_t * nMallocBuf ); // a malloc, user frees
    void Insert( wchar_t * szNew, int nFinalNL );
private:
    int MakeFit( size_t nNewLen );
    wchar_t * szBuf;
    size_t nMal;
    size_t nLen;
};



