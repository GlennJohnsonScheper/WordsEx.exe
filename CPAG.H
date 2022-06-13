// This is file: CPag.h
// Copyright ( C ) 2006, Glenn Scheper

class CAll; // Globals
class CAsb; // ASCII string buffer
class CBud; // Base of sorted fruit
class CFio; // Local file in / out
class CHtm; // Parses html into text
class CIni; // Initiate operations
class CInt; // Integer list
class CIdx; // CodePage conversions
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

class CPag
{
public:
    CPag( );
    ~CPag( );
    wchar_t * CategorizeAndConvertInputBytesToWideWithBinaryRejection( size_t * pnWBuf, BYTE * pBBuf, size_t nBBuf );
    wchar_t * CategorizeAndConvertInputBytesToWide( size_t * pnWBuf, BYTE * pBBuf, size_t nBBuf );
    void AdjustTextBufferPerCharset( COnePaper * pOnePaper );
    void ScanForClearTextUrls( COnePaper * pOnePaper );
    BYTE * Interleave( CBud * pFruit,  size_t * nMallocBuf, int AsText );
    // SED_PILE1
    size_t ProcessPaper( size_t UrlIndex, COnePaper * pOnePaper, wchar_t * pInputBuffer, size_t nInputSize, int AsText, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, COneSurl * pOneSurl );
    // SED_PILE2
    void SecondPartOfProcessForText( size_t UrlIndex, int ShowAsAQrp, COnePaper * pOnePaper );
    int ClearTextTokenResemblesUrl( wchar_t * pToken, size_t nChars );
    size_t SkipWordsExHeader( size_t * pnUrl, size_t * pnDate, size_t * pnLang, size_t * pnCSet, wchar_t * pWBuf, size_t nWBuf, COnePaper * pOnePaper );
    wchar_t * GetWordsExHeader( size_t UrlIndex );
    int DislikeDueToDataContent( wchar_t * pWBuf, size_t nWBuf );
    CSol * CSolLGroupWords[ PAST_LANGUAGE_GROUP_IDS ];
private:
};


