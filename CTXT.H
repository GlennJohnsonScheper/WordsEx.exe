// This is file: CTxt.h
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

// PipeItem is a class only used in CTxt.h, CTxt.cpp:
class CPipeItem
{
public:
    CPipeItem( );
    ~CPipeItem( );
    size_t AtopBlock;
    size_t PastBlock;
    size_t IndexBlock;
    void AddSentence( size_t Atop, size_t Past, size_t Index );
    size_t * pMallocTriplets;
    size_t nMallocTriplets; // N, multiply by ( 3 * sizeof( size_t ) )
    size_t nFilledTriplets; // N, comparable to nMallocTriplets
};


class CTxt
{
public:
    CTxt( COnePaper * pOnePaper );
    ~CTxt( );
    size_t SimpleWordScan( );
    size_t SentenceScan( );
    void MostUncommonWords( CWsb * pWsbNotes );
    void LanguageGuessingWork( );
    void AddWordsToOtherLists( );
private:
    double DivideBlockIntoSentences( wchar_t * atop, wchar_t * past, int AtopOffset );
    int No_PrescanBlockShowsPatterns( wchar_t * atop, wchar_t * past );
    void RejectedPotentialSentence( wchar_t * atopPassed, wchar_t * past, int AtopOffset );
    double ConsiderPotentialSentence( wchar_t * atop, wchar_t * past, int AtopOffset );
    void OpenBlockInPipeline( size_t Atop, size_t Past );
    void CloseBlockInPipeline( size_t Index, size_t Signify );
    void AddSentenceToPipeline( size_t Atop, size_t Past, size_t Index );
    void AddPipeItemToIdxes( CPipeItem * pPipeItem );
    void SpewSentenceSafely( size_t Atop, size_t Past, wchar_t wcMarker );
    void SpewPipeItemToIgnore( CPipeItem * pPipeItem );
    COnePaper * m_pOnePaper;
    wchar_t * LocalCopy;
    size_t nLocalCopy;
    wchar_t * szSentenceBuffer;
    size_t nMalSentenceBuffer;
    CSol * m_pSolWordList;

    #if DO_DEBUG_LANG_GUESS
        CWsb * m_pWsbGuessWork;
    #endif

    CPipeItem * pPiCurrentBlock;
    CPipeItem * pPiPipeLine[ 3 ];
    int WarrantCounter;

};
