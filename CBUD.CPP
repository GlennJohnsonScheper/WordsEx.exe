// This is file: CBud.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals




CBud::CBud( int CSolIndex, int FruitType )
{
    #if DO_DEBUG_CALLS
        Routine( L"107" );
    #endif
    m_CSolIndex = CSolIndex;
    m_FruitType = FruitType;
    m_WorkInProgress = 0;
    m_StopThisThread = 0;
    pWsbResultText = new CWsb( );
    pIdxResultIndex = new CIdx( );
    pIdxTextBlocks = new CIdx( );
    pIdxSentences = new CIdx( );
    pSolWordList = new CSol( CSOL_SCALAR );
}

CBud::~CBud( )
{
    #if DO_DEBUG_CALLS
        Routine( L"108" );
    #endif

    // It may seem a waste of time to NULL things going away,
    // but suppose they hold valid data after freeing memory,
    // a reference after delete, delaying discovery of error.
    // I think I had that case when I imported two files with
    // same URL, causing one to delete other's fruit, but was
    // still referenced in view. Viewing got memory error, and
    // memory at the fruit object location now held WSB object.
    // SO: PLAN: After every delete, also free, then NULL ptr.

    // Moreover, to prevent wierd memory reference in that instant:
    // before any delete using a pointer that might get referenced,
    // copy the pointer, null original pointer, then do the delete.
    {
        CWsb * Temp = pWsbResultText; // copy before delete
        pWsbResultText = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    {
        CIdx * Temp = pIdxResultIndex; // copy before delete
        pIdxResultIndex = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    {
        CIdx * Temp = pIdxTextBlocks; // copy before delete
        pIdxTextBlocks = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    {
        CIdx * Temp = pIdxSentences; // copy before delete
        pIdxSentences = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    {
        CSol * Temp = pSolWordList; // copy before delete
        pSolWordList = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
}



COneFetch::COneFetch( int CSolIndex ) : CBud( CSolIndex, ENUM_ADDPAGE )
{
    #if DO_DEBUG_CALLS
        Routine( L"109" );
    #endif

    m_FetchAllLinks = 0; // to be set after created, in FetchDlgProc.
    m_NonTextLinks = 0; // to be set after created, in InitiateFetch.
    m_NonTextPath = NULL; // to be set after created, in InitiateFetch.

    #if DO_DEBUG_CALLS
        Routine( L"110" );
    #endif
}

COneFetch::~COneFetch( )
{

    if( m_NonTextPath != NULL )
    {
        wchar_t * temp = m_NonTextPath;
        m_NonTextPath = NULL;
        MyFree( 104, UNPREDICTABLE, temp );
    }

    #if DO_DEBUG_CALLS
        Routine( L"111" );
    #endif
}



COneQuery::COneQuery( int CSolIndex ) : CBud( CSolIndex, ENUM_ADDSEARCH )
{
    #if DO_DEBUG_CALLS
        Routine( L"112" );
    #endif
    m_QueryDiligence = DEFAULT_DILIGENCE; // to be set after created, in QueryDlgProc.
    pWsbQueryValue = new CWsb( );
    m_NonTextLinks = 0; // to be set after created, in InitiateFetch.
    m_NonTextPath = NULL; // to be set after created, in InitiateFetch.
    ppSolMore = NULL;
    ppSolHits = NULL;
    ppSurls = NULL;
    nEngineVectors = 0;
}

COneQuery::~COneQuery( )
{
    #if DO_DEBUG_CALLS
        Routine( L"113" );
    #endif
    {
        CWsb * Temp = pWsbQueryValue; // copy before delete
        pWsbQueryValue = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
    if( m_NonTextPath != NULL )
    {
        wchar_t * temp = m_NonTextPath;
        m_NonTextPath = NULL;
        MyFree( 104, UNPREDICTABLE, temp );
    }
    if( nEngineVectors != 0 )
    {
        // Then both vectors were init, and must be freed,
        // after deleting the CSol objects in both vectors.
        size_t i = 0;
        for( ;; )
        {
            if( i == nEngineVectors )
                break;
            {
                CSol * Temp = ppSolMore[i]; // copy before delete
                ppSolMore[i] = NULL;        // NULL before delete
                delete Temp;
                Temp = NULL; // obligatory rigor after delete
            }
            {
                CSol * Temp = ppSolHits[i]; // copy before delete
                ppSolHits[i] = NULL;        // NULL before delete
                delete Temp;
                Temp = NULL; // obligatory rigor after delete
            }
            i ++;
        }
        // Now free both vector mallocs. It looks like four now.
        {
            CSol * * temp = ppSolMore;
            ppSolMore = NULL;
            MyFree( 178, nEngineVectors * sizeof( CSol * ), temp );
        }
        {
            CSol * * temp = ppSolHits;
            ppSolHits = NULL;
            MyFree( 183, nEngineVectors * sizeof( CSol * ), temp );
        }
        {
            COneSurl * * temp = ppSurls;
            ppSurls = NULL;
            MyFree( 188, nEngineVectors * sizeof( COneSurl * ), temp );
        }
        nEngineVectors = 0;
    }
}



COneKwic::COneKwic( int CSolIndex ) : CBud( CSolIndex, ENUM_ADDFIND )
{
    #if DO_DEBUG_CALLS
        Routine( L"114" );
    #endif
    m_Kwic1_Sentc2_Parag3 = 1; // to be set after created, in KwicDlgProc.
}

COneKwic::~COneKwic( )
{
    #if DO_DEBUG_CALLS
        Routine( L"115" );
    #endif
}



COnePaper::COnePaper( int CSolIndex ) : CBud( CSolIndex, ENUM_ONEPAPER )
{
    #if DO_DEBUG_CALLS
        Routine( L"116" );
    #endif
    HttpHeaderContentType = CONTENT_OTHER;
    HttpHeaderContentLanguage = LANGUAGE_UNKNOWN;
    HttpHeaderContentEncoding = ENCODING_NONE;
    HttpHeaderContentLength = 0;
    HttpHeaderStatus = FETCH_STATUS_NOT_ATTEMPTED;
    HttpHeaderCharset = CHARSET_UNKNOWN;
    HttpHeaderDateyyyymmdd = 0;
    LanguageGroup = LANGUAGE_GROUP_UNKNOWN;
    FetchedButNotViewed = 0;
    NewLocationIndex = 0;
    PageRankingScore = 0;
    PageIsAQrpThisIsItsOrdinal = ZERO_ORDINAL_FOR_NON_QRP;
    pWsbTitleText = NULL; // do JIT: new CWsb( );
    pIntFromEngine = NULL; // do JIT: new CInt( );
    pIdxClearTextUrls = NULL; // do JIT: new CIdx( );
    pWsbAnnotation = NULL; // do JIT: new CWsb( );
}

COnePaper::~COnePaper( )
{
    #if DO_DEBUG_CALLS
        Routine( L"117" );
    #endif

    if( pWsbTitleText != NULL )
    {
        CWsb * Temp = pWsbTitleText; // copy before delete
        pWsbTitleText = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    if( pWsbAnnotation != NULL )
    {
        CWsb * Temp = pWsbAnnotation; // copy before delete
        pWsbAnnotation = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    if( pIntFromEngine != NULL )
    {
        CInt * Temp = pIntFromEngine; // copy before delete
        pIntFromEngine = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
    // CWsbAnnotation is automatically deleted.
}



COneCache::COneCache( ) : CBud( 0, ENUM_ADDCACHE )
{
    #if DO_DEBUG_CALLS
        Routine( L"118" );
    #endif
}
COneCache::~COneCache( )
{
    #if DO_DEBUG_CALLS
        Routine( L"119" );
    #endif
}



COnePhrases::COnePhrases( ) : CBud( 0, ENUM_ADDPHRASES )
{
    #if DO_DEBUG_CALLS
        Routine( L"120" );
    #endif
}
COnePhrases::~COnePhrases( )
{
    #if DO_DEBUG_CALLS
        Routine( L"121" );
    #endif
}



COneFolder::COneFolder( int Which ) : CBud( Which, ENUM_ADDDIRECTORY )
{
    #if DO_DEBUG_CALLS
        Routine( L"124" );
    #endif
    pSolFolderToDo = NULL;
    m_TotalFolderFiles = 0;
    m_TotalFolderBytes = 0;
    m_bAddRecursively = 0;
    // pWsbResultText->Add( L"Add Directory0:\r\n" );
}
COneFolder::~COneFolder( )
{
    #if DO_DEBUG_CALLS
        Routine( L"125" );

    if( pSolFolderToDo != NULL )
    {
        CSol * Temp = pSolFolderToDo; // copy before delete
        pSolFolderToDo = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
    #endif
}

COneForm::COneForm( int CSolIndex ) : CBud( CSolIndex, ENUM_ONEFORM )
{
    #if DO_DEBUG_CALLS
        Routine( L"473" );
    #endif
    pSolFormAtoms = new CSol( CSOL_SCALAR );
    pSolFormSpecies = new CSol( CSOL_SCALAR );
    FormRejectedBitfield = 0;
}


COneForm::~COneForm( )
{
    #if DO_DEBUG_CALLS
        Routine( L"474" );
    #endif
    {
        CSol * Temp = pSolFormAtoms; // copy before delete
        pSolFormAtoms = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
    {
        CSol * Temp = pSolFormSpecies; // copy before delete
        pSolFormSpecies = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
}

    // SED_PILE1
COneSurl::COneSurl( int CSolIndex ) : CBud( CSolIndex, ENUM_ONESURL )
{
    #if DO_DEBUG_CALLS
        Routine( L"477" );
    #endif
    GetOrdinal = 0;
    // Changing from a CSol to a realloc'ed vector:
    pRuleVector = NULL;
    nRuleVector = 0;
    szDomainName = NULL;
    ContainsAnyRuleNoneGoodUntil = 0;
    KeepQueryResultPageAsGoodText = 0;
    AnnotateFactsForStudy = 0;
}
    // SED_PILE2


COneSurl::~COneSurl( )
{
    #if DO_DEBUG_CALLS
        Routine( L"478" );
    #endif
    if( pRuleVector != NULL )
    {
        // Before I can free the malloc'ed pRuleVector,
        // I must free all the wchar_t mallocs it holds.
        size_t i = 0;
        for( ;; )
        {
            if( i == nRuleVector )
                break;
            wchar_t * wcp = pRuleVector[i].wcp;
            pRuleVector[i].wcp = NULL;
            // Never expect NULL
            if( wcp != NULL )
                MyFree( 330, UNPREDICTABLE, wcp );
            i++;
        }

        WCPUI * temp = pRuleVector;
        pRuleVector = NULL;
        MyFree( 332, UNPREDICTABLE, temp );
    }
    nRuleVector = 0;
    if( szDomainName != NULL )
    {
        wchar_t * temp = szDomainName;
        szDomainName = NULL;
        MyFree( 339, UNPREDICTABLE, temp );
    }
}
