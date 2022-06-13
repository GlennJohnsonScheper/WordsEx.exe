// This is file: CBud.h
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

class CBud
{
public:
    CBud( int CSolIndex, int FruitType );
    // Declaring the base class destructer as virtual
    // means that deleting the base class object will
    // invoke destructor for any derived classes too.
    //
    // Declaring the base class destructer as virtual
    // means that the compiler will use an expensive
    // construction, and allow typecasts into fruits.
    virtual ~CBud( );
    int m_FruitType;
    int m_CSolIndex;
    int m_WorkInProgress;
    int m_StopThisThread;
    CWsb * pWsbResultText;
    CIdx * pIdxResultIndex;
    CIdx * pIdxTextBlocks;
    CIdx * pIdxSentences;
    CSol * pSolWordList;
private:
};

    // Derived class of CBud to hold a User Fetch.

class COneFetch : public CBud
{
public:
    // base class CBud member m_CSolIndex is to my entry in CSolUserFetchs.
    // This object hangs from CSolUserFetchs.List[m_CSolIndex].User.pVoid.
    COneFetch( int CSolIndex );
    ~COneFetch( );
    size_t m_FetchAllLinks;
    size_t m_NonTextLinks;
    wchar_t * m_NonTextPath;
private:
};

    // Derived class of CBud to hold a User Query.

class COneQuery : public CBud
{
public:
    // base class CBud member m_CSolIndex is to my entry in CSolUserQuerys.
    // This object hangs from CSolUserQuerys.List[m_CSolIndex].User.pVoid.
    // This object can also be referenced by pointer from a CTop item.
    COneQuery( int CSolIndex );
    ~COneQuery( );
    size_t m_QueryDiligence;
    CWsb * pWsbQueryValue;
    size_t m_NonTextLinks;
    wchar_t * m_NonTextPath;
    CSol * * ppSolMore;
    CSol * * ppSolHits;
    COneSurl * * ppSurls;
    size_t nEngineVectors;
private:
};

    // Derived class of CBud to hold a User Kwic.

class COneKwic : public CBud
{
public:
    // base class CBud member m_CSolIndex is to my entry in CSolUserKwics.
    // This object hangs from CSolUserKwics.List[m_CSolIndex].User.pVoid.
    // This object can also be referenced by pointer from a CTop item.
    COneKwic( int CSolIndex );
    ~COneKwic( );
    size_t m_Stem;
    size_t m_Kwic1_Sentc2_Parag3;
private:
};


    // Derived class of CBud to hold One URL.

class COnePaper : public CBud
{
public:
    // base class CBud member m_CSolIndex is to my entry in CSolAllUrls.
    // This object hangs from CSolAllUrls.List[m_CSolIndex].User.pVoid.
    // This object can also be referenced by pointer from a CTop item.
    COnePaper( int CSolIndex );
    ~COnePaper( );
    size_t HttpHeaderContentType;
    size_t HttpHeaderContentLanguage;
    size_t HttpHeaderContentEncoding;
    size_t HttpHeaderContentLength;
    size_t HttpHeaderCharset;
    size_t HttpHeaderStatus;
    size_t HttpHeaderDateyyyymmdd;
    size_t FetchedButNotViewed;
    size_t LanguageGroup;
    int NewLocationIndex;
    int PageRankingScore;
    int PageIsAQrpThisIsItsOrdinal;

    // I asked myself, when adding new member CWsbAnnotation,
    // why is pWsbTitleText a pointer instead of an instance?
    // Because trying to make the object in this object gets:
    // error C2079: 'CWsbAnnotation' uses undefined class 'CWsb'

    CWsb * pWsbTitleText;
    CInt * pIntFromEngine;
    CIdx * pIdxClearTextUrls;
    CWsb * pWsbAnnotation;
private:
};


    // Derived class of CBud to hold Add-Cache progress log.

class COneCache : public CBud
{
public:
    // base class CBud member m_CSolIndex is: Not relevant here.
    // This object hangs from a CTop item, as its owner, to delete it.
    COneCache( );
    ~COneCache( );
private:
};


    // Derived class of CBud to hold Add-Phrases progress log.

class COnePhrases : public CBud
{
public:
    // base class CBud member m_CSolIndex is: Not relevant here.
    // This object hangs from a CTop item, as its owner, to delete it.
    COnePhrases( );
    ~COnePhrases( );
private:
};

    // Derived class of CBud to hold Add-Folder progress log.

class COneFolder : public CBud
{
public:
    // base class CBud member m_CSolIndex is: Not relevant here.
    // This object hangs from a CTop item, as its owner, to delete it.
    COneFolder( int CSolIndex );
    ~COneFolder( );
    CSol * pSolFolderToDo;
    size_t m_TotalFolderFiles;
    size_t m_TotalFolderBytes;
    int m_bAddRecursively;
private:
};

    // Derived class of CBud to hold Search Engine FORM info

class COneForm : public CBud
{
public:
    // base class CBud member m_CSolIndex is to my entry in CSolFormUrls.
    // This object hangs from CSolFormUrls.List[m_CSolIndex].User.pVoid.
    COneForm( int CSolIndex );
    ~COneForm( );
    CSol * pSolFormAtoms;
    CSol * pSolFormSpecies;
    int FormRejectedBitfield;
private:
};


    // Derived class of CBud to hold Search Engine URL and other info

    // SED_PILE1
class COneSurl : public CBud
{
public:
    // base class CBud member m_CSolIndex is to my entry in CSolSearchUrls.
    // This object hangs from CSolSearchUrls.List[m_CSolIndex].User.pVoid.
    // Many query threads might be consulting the very same object at once,
    // but no query thread changes it; It is determined from engine setups.
    COneSurl( int CSolIndex );
    ~COneSurl( );
    int GetOrdinal;
    WCPUI * pRuleVector;
    size_t  nRuleVector;
    wchar_t * szDomainName;
    int ContainsAnyRuleNoneGoodUntil;
    int KeepQueryResultPageAsGoodText;
    int AnnotateFactsForStudy;
private:
};
    // SED_PILE2

