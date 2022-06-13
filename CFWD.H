// This is file: CFwd.h
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
// The original intention of CFwd was to hold web pages,
// but they just got held, and parsed, and CFwd is empty.
// So I will use CFwd.cpp for the Kwic ( Find ) function.

class CFwd
{
public:
    CFwd( size_t KwicIndex );
    ~CFwd( );
    void RunFindThread( );
    void RunPhrasesThread( );
private:
    int BreakDownTheFindParameterString( );
    int PostTokenScanAnalysis( );
    int TokenStemmingAnalysis( );
    void FullTextSearch( COnePaper * pOnePaper, CSol * pSolRank );
    void TreatAsKwic( size_t PageRank, size_t UrlIndex, wchar_t * pMallocBuf, int AtopText, int offset, CSol * pSolRank );
    int TreatAsSentence( size_t PageRank, size_t UrlIndex, wchar_t * pMallocBuf, int AtopText, int offset, CSol * pSolRank, CIdx * pIdxSentences );
    int TreatAsParagraph( size_t PageRank, size_t UrlIndex, wchar_t * pMallocBuf, int AtopText, int offset, CSol * pSolRank, CIdx * pIdxTextBlocks );
    void RankItem( CSol * pSolRank, wchar_t * Text, size_t StrLen, size_t UrlQ, size_t UrlIndex, size_t UrlOffset );
    CBud * pBudLog;
    size_t m_Kwic1_Sentc2_Parag3;
    int m_Stem;
    wchar_t * UserSearchString;
    CSol * m_pSolSearchTerms;
    size_t m_Pass1WordCount;
};



