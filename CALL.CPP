// This is file: CAll.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// lets put all the scalar globals first.

HDC g_hdcMain = NULL; // main device context, if CS_OWNDC
size_t g_BitsPerPixel = 0;
size_t g_ColorPlanes = 0;
size_t g_PelsPerInch = 0;
size_t g_ClientHeight = 0;
size_t g_ClientWidth = 0;
size_t g_LineXRef = 0;
size_t g_LineXEnd = 0;
size_t g_MaxExtentLogicalUnits = 0;
size_t g_KwicTextWidthLimit = 0;

HDC g_hdcCompat = NULL; // compatible device context
HBITMAP g_hbmCompat = NULL; // and compatible bitmap
HBITMAP g_CompatibleDcOriginalSystemBitmap = NULL;
HFONT g_CompatibleDcOriginalSystemFont = NULL;
HFONT g_CurrentDcSelectedFont = NULL;
HFONT g_LatestCreatedFont = NULL;

#ifdef _WIN32_WCE
    double g_dFontPointSize = 16.0;
#else // not _WIN32_WCE
    double g_dFontPointSize = 24.0;
#endif // _WIN32_WCE
LOGFONT LogicalFont;
int g_MustMeasureLatestCreatedFont = 0;
unsigned short g_SomeGlyphWidths[SOME_GLYPHS];
unsigned short g_WidestGlyphWidth = 0;
char ToHex[] = "0123456789ABCDEF"; // HTML 4.0 prefers uppercase

int rgbGlyph[N_RGB_CYCLE] = {
    RGB(   0,   0,   0 ), // black
    RGB(   0,   0,   0 ), // black
    RGB( 255, 255, 255 ), // white
    RGB(   0, 255,   0 ), // pure green
};

int rgbErase[N_RGB_CYCLE] = {
    RGB(   0, 255,   0 ), // pure green
    RGB( 255, 255, 255 ), // white
    RGB(   0,   0,   0 ), // black
    RGB(   0,   0,   0 ), // black
};

// Surf4me had a really elegant and precise Canonicalization work.
// Bring me his tables to know what are unreserved chars, etc...

    // These macros help me implement the RFC2396 parse,
    // inheiritance from base, canonicalization of URLs.


int cM [128] = {
  EXCLUDE1 | EXCLUDE2 | EXCLUDE3 | EXCLUDE4 /* end of string */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
, 0 /* control */
,/* 0x20 = ' ' */ SPACE | BOUNDING
,/* 0x21 = '!' */ MARK
,/* 0x22 = '"' */ DELIMS | BOUNDING | BADFILE
,/* 0x23 = '#' */ DELIMS | EXCLUDE1 | EXCLUDE2 | EXCLUDE3 | EXCLUDE4
,/* 0x24 = '$' */ RESERVED
,/* 0x25 = '%' */ ESCAPE | DELIMS
,/* 0x26 = '&' */ RESERVED
,/* 0x27 = ''' */ MARK
,/* 0x28 = '(' */ MARK
,/* 0x29 = ')' */ MARK
,/* 0x2a = '*' */ MARK | BADFILE
,/* 0x2b = '+' */ RESERVED
,/* 0x2c = ',' */ RESERVED
,/* 0x2d = '-' */ MARK
,/* 0x2e = '.' */ MARK
,/* 0x2f = '/' */ RESERVED | EXCLUDE1 | EXCLUDE2 | BADFILE
,/* 0x30 = '0' */ DIGIT | HEX
,/* 0x31 = '1' */ DIGIT | HEX
,/* 0x32 = '2' */ DIGIT | HEX
,/* 0x33 = '3' */ DIGIT | HEX
,/* 0x34 = '4' */ DIGIT | HEX
,/* 0x35 = '5' */ DIGIT | HEX
,/* 0x36 = '6' */ DIGIT | HEX
,/* 0x37 = '7' */ DIGIT | HEX
,/* 0x38 = '8' */ DIGIT | HEX
,/* 0x39 = '9' */ DIGIT | HEX
,/* 0x3a = ':' */ RESERVED | EXCLUDE1 | BADFILE
,/* 0x3b = ';' */ RESERVED
,/* 0x3c = '<' */ DELIMS | BOUNDING | BADFILE
,/* 0x3d = '=' */ RESERVED
,/* 0x3e = '>' */ DELIMS | BOUNDING | BADFILE
,/* 0x3f = '?' */ RESERVED | EXCLUDE1 | EXCLUDE2 | EXCLUDE3 | BADFILE
,/* 0x40 = '@' */ RESERVED
,/* 0x41 = 'A' */ UPALPHA | HEX
,/* 0x42 = 'B' */ UPALPHA | HEX
,/* 0x43 = 'C' */ UPALPHA | HEX
,/* 0x44 = 'D' */ UPALPHA | HEX
,/* 0x45 = 'E' */ UPALPHA | HEX
,/* 0x46 = 'F' */ UPALPHA | HEX
,/* 0x47 = 'G' */ UPALPHA
,/* 0x48 = 'H' */ UPALPHA
,/* 0x49 = 'I' */ UPALPHA
,/* 0x4a = 'J' */ UPALPHA
,/* 0x4b = 'K' */ UPALPHA
,/* 0x4c = 'L' */ UPALPHA
,/* 0x4d = 'M' */ UPALPHA
,/* 0x4e = 'N' */ UPALPHA
,/* 0x4f = 'O' */ UPALPHA
,/* 0x50 = 'P' */ UPALPHA
,/* 0x51 = 'Q' */ UPALPHA
,/* 0x52 = 'R' */ UPALPHA
,/* 0x53 = 'S' */ UPALPHA
,/* 0x54 = 'T' */ UPALPHA
,/* 0x55 = 'U' */ UPALPHA
,/* 0x56 = 'V' */ UPALPHA
,/* 0x57 = 'W' */ UPALPHA
,/* 0x58 = 'X' */ UPALPHA
,/* 0x59 = 'Y' */ UPALPHA
,/* 0x5a = 'Z' */ UPALPHA
,/* 0x5b = '[' */ UNWISE
,/* 0x5c = '\' */ UNWISE | BADFILE
,/* 0x5d = ']' */ UNWISE
,/* 0x5e = '^' */ UNWISE
,/* 0x5f = '_' */ MARK
,/* 0x60 = '`' */ UNWISE
,/* 0x61 = 'a' */ LOWALPHA | HEX
,/* 0x62 = 'b' */ LOWALPHA | HEX
,/* 0x63 = 'c' */ LOWALPHA | HEX
,/* 0x64 = 'd' */ LOWALPHA | HEX
,/* 0x65 = 'e' */ LOWALPHA | HEX
,/* 0x66 = 'f' */ LOWALPHA | HEX
,/* 0x67 = 'g' */ LOWALPHA
,/* 0x68 = 'h' */ LOWALPHA
,/* 0x69 = 'i' */ LOWALPHA
,/* 0x6a = 'j' */ LOWALPHA
,/* 0x6b = 'k' */ LOWALPHA
,/* 0x6c = 'l' */ LOWALPHA
,/* 0x6d = 'm' */ LOWALPHA
,/* 0x6e = 'n' */ LOWALPHA
,/* 0x6f = 'o' */ LOWALPHA
,/* 0x70 = 'p' */ LOWALPHA
,/* 0x71 = 'q' */ LOWALPHA
,/* 0x72 = 'r' */ LOWALPHA
,/* 0x73 = 's' */ LOWALPHA
,/* 0x74 = 't' */ LOWALPHA
,/* 0x75 = 'u' */ LOWALPHA
,/* 0x76 = 'v' */ LOWALPHA
,/* 0x77 = 'w' */ LOWALPHA
,/* 0x78 = 'x' */ LOWALPHA
,/* 0x79 = 'y' */ LOWALPHA
,/* 0x7a = 'z' */ LOWALPHA
,/* 0x7b = '{' */ UNWISE
,/* 0x7c = '|' */ UNWISE | BADFILE
,/* 0x7d = '}' */ UNWISE
,/* 0x7e = '~' */ MARK
, 0 /* control */
};


int g_FocusOffsetToRestoreWhenResize = -1; // always -1 unless is iconic
int g_rgbIndex = 0; // Start with black on green
int g_rgbGlyph = RGB( 0, 0, 0 ); // black
int g_rgbErase = RGB( 0, 255, 0 ); // green
int g_CurrentlyShowingSplashScreen = 0;
int OmegaElapsedms = 0;

size_t g_LineHeight = 0;
size_t g_nLinesPageAdvances = 0;
size_t g_nLinesPaintingStop = 0;
RECT g_BaseRects [MAX_VISIBLE_LINES + 1];
RECT g_LineRects [MAX_VISIBLE_LINES + 1];

int g_SystemClockQuantaIn100ns = 300000; // say, 30 ms
int g_CHtmFoundAForm = 0;

volatile int InDispatch = 1; // leave 1 for setup, till reach message loop
volatile int g_BitBltFailed = 0;
volatile int g_BitBltCount = 0;

volatile size_t MemOrdinal = 0;
wchar_t * g_szInternalError = NULL;
volatile DWORD g_dwLastError = 0;
volatile int g_bStopAllThreads = 0;
volatile int g_bStopInitThread = 0;
volatile int g_bInitThreadRunning = 0;
volatile int g_ReadyToAllowPaint = 0;
volatile int NeedPaint = 0;
volatile int g_JumpActivated = 0;

volatile int g_FetchAllLinks = 0;
volatile int g_NonTextLinks = 0;
volatile int g_QueryDiligence = DEFAULT_DILIGENCE;
volatile int g_bCreateSubfolders = 1;
volatile int g_bAddRecursively = 1;
volatile int g_Kwic1_Sentc2_Parag3 = 1;
volatile int g_PhrasesPercent = 100; // 2015-04-25 looking for the 1 percent to change; find this was already changed to 100 percent
volatile int g_Stem = 1;
volatile int g_nPasteCounter = 0;
volatile int g_nPapersFetchedButNotViewed = 0;
wchar_t * g_ClickTextToInitDialogs = NULL;

volatile int g_nCountOfCacheThreads = 0;
volatile int g_nCountOfFolderThreads = 0;
volatile int g_nCountOfQueryThreads = 0;
volatile int g_bEverStartedAnyQuery = 0;
volatile int g_nCountOfFetchThreads = 0;
volatile int g_nCountOfKwicThreads = 0;
volatile int g_nCountOfPhrasesThreads = 0;

volatile int TotalMallocBytes = 0;
volatile int gbMallocLimitExceeded = 0;
volatile int gbMallocLimitToldUser = 0;
volatile int TotalPageCount   = 0;
volatile int TotalLanguageCount = 0;
volatile int TotalPhraseCount = 0;
volatile int TotalWordCount   = 0;
volatile int TotalFormCount   = 0;

// these being pointers, they instantiate nothing.

COnePaper * g_MainShallTopAddThisPaper = NULL;
volatile int g_bOmitSignage = 1;
CBud * g_pFruitOfLastDeletedView = NULL;
size_t g_OffsetOfLastDeletedView = 0;
FILE * SpewFile = NULL;
CSol * pSolCacheToDo = NULL;
volatile int SatCrixCounter = 0;

// these also are just a wad o' ram.

CRITICAL_SECTION CSpewCriticalSection;
CRITICAL_SECTION CAsbCriticalSection;
CRITICAL_SECTION CWsbCriticalSection;
CRITICAL_SECTION CIntCriticalSection;
CRITICAL_SECTION CIdxCriticalSection;
CRITICAL_SECTION CPasCriticalSection; // New. For Paint and Scroll.
CRITICAL_SECTION CSatCriticalSection; // I defined Top and See as one crix
CRITICAL_SECTION CSolCriticalSection;
CRITICAL_SECTION CLinCriticalSection;


// Classes, which Instantiate.

// I have gathered you all together here today,
// to determine the order of your constructors.

// This 1 of 2 once-only must be FirstToInstantiate, before all others.

class CFirstToInstantiate
{
public:
    CFirstToInstantiate( )
    {
        #ifndef _WIN32_WCE
            // I could not remove them after fopen call.
            remove( SpewFileName ); // okay if not present
            remove( DumpFileName ); // okay if not present
        #endif //not _WIN32_WCE

        // Spew routines are call by many during debugging.
        // Spew routines call no others. Spew comes first.
        InitializeCriticalSection( & CSpewCriticalSection );

        // These classes serve many others, make no demands.
        InitializeCriticalSection( & CAsbCriticalSection );
        InitializeCriticalSection( & CWsbCriticalSection );
        InitializeCriticalSection( & CIntCriticalSection );
        InitializeCriticalSection( & CIdxCriticalSection );

        // The CTop and CSee are whipped by many threads.
        InitializeCriticalSection( & CPasCriticalSection ); // New. For Paint and Scroll.
        InitializeCriticalSection( & CSatCriticalSection ); // I defined Top and See as one crix

        // Nothing above here sorted, but threads do much.
        InitializeCriticalSection( & CSolCriticalSection );
        InitializeCriticalSection( & CLinCriticalSection );
    }

    ~CFirstToInstantiate( )
    {
        // delete in the reverse order of initialize.

        DeleteCriticalSection( & CLinCriticalSection );
        DeleteCriticalSection( & CSolCriticalSection );

        DeleteCriticalSection( & CPasCriticalSection ); // New. For Paint and Scroll.
        DeleteCriticalSection( & CSatCriticalSection ); // I defined Top and See as one crix

        DeleteCriticalSection( & CIntCriticalSection );
        DeleteCriticalSection( & CIdxCriticalSection );
        DeleteCriticalSection( & CWsbCriticalSection );
        DeleteCriticalSection( & CAsbCriticalSection );

        DeleteCriticalSection( & CSpewCriticalSection );
    }
};
CFirstToInstantiate FirstToInstantiate;


// Some of these other classes must be before others.

COneCache OnlyCache; // one per application
COnePhrases OnlyPhrases; // one per application
CFio Fio; // one per application
CIni Initiator; // one per application
CPag Pag; // one per application
CPor Por; // one per application
CSee See; // one per application
CWww Www; // one per application
CTop Top; // one per application
CUrl Url; // one per application

COme * g_pOmega = NULL;

CSol CSolUserQuerys( CSOL_OBJECT );
CSol CSolUserFetchs( CSOL_OBJECT );
CSol CSolUserKwics( CSOL_OBJECT );
CSol CSolUserFolders( CSOL_OBJECT );
CSol CSolAllUrls( CSOL_OBJECT );
CSol CSolAnchorText( CSOL_SCALAR );
CLin CLinAnchorHeads;
CLin CLinAnchorTails;
CSol CSolFormUrls( CSOL_OBJECT );
CSol CSolRedirects( CSOL_SCALAR );
CSol CSolLanguageAliases( CSOL_SCALAR );
CSol CSolCharsets( CSOL_SCALAR );
// CSol CSolProperNames( CSOL_SCALAR );
CSol CSolCommonWords( CSOL_SCALAR );
CSol CSolAllWords( CSOL_SCALAR );
CSol CSolConsonantRunValues( CSOL_SCALAR );
CSol CSolVowelRunValues( CSOL_SCALAR );
CSol CSolNewVows( CSOL_SCALAR );
CSol CSolNewCons( CSOL_SCALAR );
CSol CSolHtmlTagNames( CSOL_SCALAR );
CSol CSolHtmlAttrNames( CSOL_SCALAR );
CSol CSolHtmlBcEntities( CSOL_SCALAR );
CSol CSolHtmlLcEntities( CSOL_SCALAR );
CSol CSolTopLevelDomains( CSOL_SCALAR );
CSol CSolCountryDomains( CSOL_SCALAR );
CSol CSolBinExtensions( CSOL_SCALAR );
CSol CSolSearchUrls( CSOL_OBJECT );
CSol CSolRejectUrls( CSOL_OBJECT );
CSol CSol3CharLcDomains( CSOL_SCALAR );
CSol CSol4CharLcDomains( CSOL_SCALAR );

CWsb WsbClipboardAccumulator;
CWsb WsbTempPathToSaveNonText;


// This 2 of 2 once-only must be SecondToInstantiate,
// and had to wait until constructors above executed.

class CSecondToInstantiate
{
public:
    CSecondToInstantiate( )
    {
        InitHtmlEntities( );
        InitHtmlTagNames( );
        InitHtmlAttrNames( );
        SetupCharsetNames( );
        SetupLanguageNames( );
        SetupDomainNames( );
        SetupBinaryNames( );
        Url.SetUpDefaultSearchEngineControls( );

        // Rather than do long Salts work in main thread,
        // initiate a non-logging worker thread to do it:

        Initiator.InitiateSalts( ); // thread does CVoc.cpp SaltCommonAndPerLanguageWordLists( );
    }

    ~CSecondToInstantiate( )
    {
    }
};

CSecondToInstantiate SecondToInstantiate;


    // Now go on to define some class-less global routines.


    // Routine, LogIn and LogOut will help me check that
    // I do not have any criss-cross cases of holding one
    // crix and wanting another. Even though It does not
    // hold separate vectors for each thread, and due to
    // an error I didn't get left number, nevertheless,
    // RBSL shows a case at routine 226 reverse of rest:


wchar_t * szLastRoutine = L"999";

#if DO_DEBUG_CALLS
void Routine( wchar_t * szNNN )
{
    // Always called thus,
    // atop most routines:
    // #if DO_DEBUG_CALLS
    //     Routine( L"999" );
    // #endif

    // Comment out THIS central too verbose Spew,
    // So I can flush every spew to study critx.
    // Well, sometimes it is worth waiting on...

    ; Spew( szNNN );

    szLastRoutine = szNNN; // to annotate CRITX
    if( SpewFile != NULL )
        fflush( SpewFile );
}
#endif

// 7 is for #0, unused, then #1 - #6 below.
#define N_CRITX         7
int InCounts[N_CRITX];
wchar_t * InRoutine[N_CRITX];

#if DO_DEBUG_CRITX
void LogIn( int n )
{
    wchar_t wk[80];
    int fill = 0;
    // In dire straits, spew on every call in or out.
    {
        wk[fill++] = 'i';
        wk[fill++] = 'n';
        wk[fill++] = ' ';
        wk[fill++] = szLastRoutine[0];
        wk[fill++] = szLastRoutine[1];
        wk[fill++] = szLastRoutine[2];
        wk[fill++] = ':';
        wk[fill++] = '0' + n; // atoi for N_CRITX < 10.
        wk[fill] = NULL;
        ; Spew( wk );
        if( SpewFile != NULL )
            fflush( SpewFile );
    }

    // Now back to your regularly scheduled flailing.
    fill = 0;
    int i = 0;
    for( ;; )
    {
        if( ++i == N_CRITX ) // preincrement stops on 7 to process #1-#6
            break;
        if( i == n )
            continue; // do not test my own kind. Often re-entrant.
        if( InCounts[i] != 0 )
        {
            wk[fill++] = InRoutine[i][0]; // no '0' +, already ascii
            wk[fill++] = InRoutine[i][1]; // no '0' +, already ascii
            wk[fill++] = InRoutine[i][2]; // no '0' +, already ascii
            wk[fill++] = ':';
            wk[fill++] = '0' + i; // atoi for N_CRITX < 10.
            wk[fill++] = ' ';
        }
    }

    // Actually, this analysis is defective, because I would need
    // to create subvectors of InCounts for every thread and main!
    // However, there were so few to analyze, I can do with it.

    if( fill != 0 ) // entering a new CRIX while possessing a crix.
    {
        wk[fill++] = '-';
        wk[fill++] = '>';
        wk[fill++] = ' ';
        wk[fill++] = szLastRoutine[0];
        wk[fill++] = szLastRoutine[1];
        wk[fill++] = szLastRoutine[2];
        wk[fill++] = ':';
        wk[fill++] = '0' + n; // atoi for N_CRITX < 10.
        wk[fill] = NULL;
        ; Spew( wk );
        if( SpewFile != NULL )
            fflush( SpewFile );
    }
    InCounts[n]++;
    InRoutine[n] = szLastRoutine;
}
#endif // DO_DEBUG_CRITX

#if DO_DEBUG_CRITX
void LogOut( int n )
{
    wchar_t wk[80];
    int fill = 0;
    // In dire straits, spew on every call in or out.
    {
        wk[fill++] = 'o';
        wk[fill++] = 'u';
        wk[fill++] = 't';
        // w/o a stack, is misleading to show a routine number.
        wk[fill++] = ':';
        wk[fill++] = '0' + n; // atoi for N_CRITX < 10.
        wk[fill] = NULL;
        ; Spew( wk );
        if( SpewFile != NULL )
            fflush( SpewFile );
    }
    InCounts[n]--;
}
#endif // DO_DEBUG_CRITX

void AsbEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 1 );
    #endif
    EnterCriticalSection( & CAsbCriticalSection );
}
void WsbEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 2 );
    #endif
    EnterCriticalSection( & CWsbCriticalSection );
}
void IdxEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 3 );
    #endif
    EnterCriticalSection( & CIdxCriticalSection );
}
void IntEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 3 );
    #endif
    EnterCriticalSection( & CIntCriticalSection );
}
void PasEnterCrisis( )
{
    // Bold new theory: If every Pas or Sat is from message loop,
    // then I have no more need of PAS or SAT critical sections!
    // Just comment them out here for now.

    // Do a coarse test that only come here from DispatchMessage.
    // Actually, I had to adjust to cover all the dialog procs...

    if( ! InDispatch )
        Say( L"BREAK ME: Not InDispatch" );

    #if DO_DEBUG_CRITX
        LogIn( 4 );
    #endif
    // EnterCriticalSection( & CPasCriticalSection );
}
void SatEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 5 );
    #endif

    if( ! InDispatch )
        Say( L"BREAK ME: Not InDispatch" );

    // Silly me. Do not count out here, unprotected.
    // EnterCriticalSection( & CSatCriticalSection );
    SatCrixCounter ++;
}
void SolEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 6 );
    #endif
    EnterCriticalSection( & CSolCriticalSection );
}
void LinEnterCrisis( )
{
    #if DO_DEBUG_CRITX
        LogIn( 6 );
    #endif
    EnterCriticalSection( & CLinCriticalSection );
}

void AsbLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 1 );
    #endif
    LeaveCriticalSection( & CAsbCriticalSection );
}
void WsbLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 2 );
    #endif
    LeaveCriticalSection( & CWsbCriticalSection );
}
void IdxLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 3 );
    #endif
    LeaveCriticalSection( & CIdxCriticalSection );
}
void IntLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 3 );
    #endif
    LeaveCriticalSection( & CIntCriticalSection );
}
void PasLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 4 );
    #endif

    if( ! InDispatch )
        Say( L"BREAK ME: Not InDispatch" );

    // LeaveCriticalSection( & CPasCriticalSection );
}
void SatLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 5 );
    #endif

    if( ! InDispatch )
        Say( L"BREAK ME: Not InDispatch" );

    // Aug 05 2007 - grep shows that NeedPaint is never set to 1.
    // Therefore, comment out needful ... needpaint ... invalidaterect

    // Decrement while protected, to never foul this counter.
    // int Needful = 0;
    // if( --SatCrixCounter == 0 )
    //     Needful = 1;
    //
    --SatCrixCounter;

    // LeaveCriticalSection( & CSatCriticalSection );

    // But perform CPas calls only after leaving the SAT CRIX.
    // Still, that means worker threads will be invoking this.
    //
    // Specifically, workers may trigger NeedPaint.
    // I think only messaging may cause NeedScroll.
    //
    // Yet, munged together here, worker may call scroll work
    // which SetScrollInfo invokes message loop, which cannot
    // enter the CPAS CRIX to handle OnWmVSroll. AAARRRGGHHH!
    //
    // Thank God. ( I prayed. ) Main now monitors threads using
    // a timer, so that no thread invokes the changed display.
    // Nevertheless, this separation seems good so that paint
    // need not be tied up waiting on other CSee / CTop tasks.


    // if( Needful )
    // {
    //     // See comments in CSee::WmPaintHandler,
    //     // how cannot enter SAT, hangs msg loop.
    //     // Hence, only trigger him if reached 0.
    //
    //     if( NeedPaint )
    //     {
    //         NeedPaint = 0;
    //
    //         // And, before invalidate triggers paint,
    //         // get him private copies of everything.
    //
    //         // This transfer takes place under the Sat Crix:
    //         // In fact, moved these copies to before the call
    //         // to UpdateV... in SeeMightHaveChangedTheDisplay.
    //         // So also, copy to PasMightHaveChangedTheDisplay.
    //         // Pas.SetPaintInput( ); // Under his own Pas Crix
    //
    //         // There is one transfer back to CSee of one variable,
    //         // nFittedLineFocusedAtopScreen, in CPas::OnWmVscroll.
    //
    //         // Not so FAST!
    //         if( g_ReadyToAllowPaint )
    //         {
    //             if( ! InvalidateRect( g_hWnd, NULL, 0 ) )
    //             {
    //                 ProgramError( L"InvalidateRect 1" );
    //                 return; // failure
    //             }
    //         }
    //     }
    // }

}
void SolLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 6 );
    #endif
    LeaveCriticalSection( & CSolCriticalSection );
}
void LinLeaveCrisis( )
{
    #if DO_DEBUG_CRITX
        LogOut( 6 );
    #endif
    LeaveCriticalSection( & CLinCriticalSection );
}

void ProgramError( wchar_t * szCaption )
{
    // Even though this may be a thread, it can do a msgbox.
    // Then main message will come later from error handler.
    // Preserve only the first error.
    if( g_szInternalError == NULL )
    {
        ; Spew( L"Program Error:" );
        ; SpewValue( szCaption, GetLastError( ) );
        MessageBox( g_hWnd, szCaption, L"WordsEx Error", MB_OK );
        // leave details for main message loop to treat...
        g_szInternalError = szCaption;
        g_dwLastError = GetLastError( );
    }
}

void UserError( wchar_t * szDetail )
{
    MessageBox( g_hWnd, szDetail, L"User Error", MB_OK );
}

void Say( wchar_t * szMsg )
{
    MessageBox( g_hWnd, szMsg, L"Say...", MB_OK );
}

void Spew( wchar_t * szMsg )
{
    EnterCriticalSection( & CSpewCriticalSection );
    // fopen didn't want a wchar_t string, even on PPC!

    // Can't find your file? Current Directory changes after File Open!

    if( SpewFile == NULL )
        SpewFile = fopen( SpewFileName, "ab" );
    if( SpewFile == NULL )
        return;
    wchar_t * scan = szMsg;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }
    fputc( '\r', SpewFile );
    fputc( '\n', SpewFile );

    #if DO_CLOSE_SPEWS
        fclose( SpewFile );
        SpewFile = NULL;
    #endif

    LeaveCriticalSection( & CSpewCriticalSection );
}

void Spewn( wchar_t * szMsg, int n )
{
    EnterCriticalSection( & CSpewCriticalSection );
    // fopen didn't want a wchar_t string, even on PPC!

    // Can't find your file? Current Directory changes after File Open!

    if( SpewFile == NULL )
        SpewFile = fopen( SpewFileName, "ab" );
    if( SpewFile == NULL )
        return;

    wchar_t * scan = szMsg;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
        if( --n <= 0 )
            break;
    }
    fputc( '\r', SpewFile );
    fputc( '\n', SpewFile );

    #if DO_CLOSE_SPEWS
        fclose( SpewFile );
        SpewFile = NULL;
    #endif

    LeaveCriticalSection( & CSpewCriticalSection );
}

void SpewValue( wchar_t * szMsg, int n )
{
    EnterCriticalSection( & CSpewCriticalSection );
    // fopen didn't want a wchar_t string, even on PPC!

    // Can't find your file? Current Directory changes after File Open!

    if( SpewFile == NULL )
        SpewFile = fopen( SpewFileName, "ab" );
    if( SpewFile == NULL )
        return;

    wchar_t * scan = szMsg;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }
    wchar_t wk[20];
    wsprintf( wk, L": %d", n );

    scan = wk;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }

    fputc( '\r', SpewFile );
    fputc( '\n', SpewFile );

    #if DO_CLOSE_SPEWS
        fclose( SpewFile );
        SpewFile = NULL;
    #endif

    LeaveCriticalSection( & CSpewCriticalSection );
}

void TagValueSpew( wchar_t * szTag, int n , wchar_t * szMsg )
{
    // Same thing, only backwards: number on left, fixed width

    EnterCriticalSection( & CSpewCriticalSection );
    // fopen didn't want a wchar_t string, even on PPC!

    // Can't find your file? Current Directory changes after File Open!

    if( SpewFile == NULL )
        SpewFile = fopen( SpewFileName, "ab" );
    if( SpewFile == NULL )
        return;

    wchar_t * scan;

    scan = szTag;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }


    wchar_t wk[20];
    wsprintf( wk, L" %9d ", n );

    scan = wk;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }


    scan = szMsg;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }

    fputc( '\r', SpewFile );
    fputc( '\n', SpewFile );

    #if DO_CLOSE_SPEWS
        fclose( SpewFile );
        SpewFile = NULL;
    #endif

    LeaveCriticalSection( & CSpewCriticalSection );
}

void SpewTwo( wchar_t * szMsg, wchar_t * sz2 )
{
    EnterCriticalSection( & CSpewCriticalSection );
    // fopen didn't want a wchar_t string, even on PPC!

    // Can't find your file? Current Directory changes after File Open!

    if( SpewFile == NULL )
        SpewFile = fopen( SpewFileName, "ab" );
    if( SpewFile == NULL )
        return;
    wchar_t * scan = szMsg;
    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }

    fputc( ':', SpewFile );
    fputc( ' ', SpewFile );

    scan = sz2;
    if( scan == NULL )
        scan = L"-NULL-";

    for( ;; )
    {
        wchar_t wc = *scan++;
        if( wc == NULL )
            break;
        if( wc > 0x00ff ) // clamp overrange chars
            wc = 511;     // produce \777
        if( wc < ' '
        || wc > '~' ) // encode the sort of special 0x7f char too
        {
            fputc( '\\', SpewFile ); // octal escape introducer
            int n = ( ( wc >> 6 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 1
            n = ( ( wc >> 3 ) & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 2
            n = ( wc & 7 ) + '0';
            fputc( n, SpewFile ); // octal digit 3
        }
        else
        {
            fputc( wc, SpewFile );
        }
    }

    fputc( '\r', SpewFile );
    fputc( '\n', SpewFile );

    #if DO_CLOSE_SPEWS
        fclose( SpewFile );
        SpewFile = NULL;
    #endif

    LeaveCriticalSection( & CSpewCriticalSection );
}

FILE * f_dump = NULL;

void AnyFinalDebugDumps( )
{
    // Only on the desktop?
#ifndef _WIN32_WCE


    // First one of several post-execution dump types


#if DO_DEBUG_DUMP

    // Can't find your file? Current Directory changes after File Open!

    f_dump = fopen( DumpFileName, "wb" );
    if( f_dump == NULL )
        return;

    CoIt * pMalVector = CSolAllWords.GetSortedVector( CSOL_BACKWARD );
    if( pMalVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;

        size_t WordValue = pCoIt->User.Value;
        char wk[20];
        sprintf( wk, "%10d ", WordValue & MASK_17BIT_WORD_COUNT );
        fputs( wk, f_dump );

        wchar_t * FullKey = CoItFullKey( pCoIt );
        wchar_t * scan = FullKey;
        for( ;; )
        {
            wchar_t wc = *scan++;
            if( wc == NULL )
                break;

            // Not needed in this case, outputting 5 octals:
            // if( wc > 0x00ff ) // clamp overrange chars
            //     wc = 511;     // produce \777

            if( wc < ' '
            || wc > '~' ) // encode the sort of special 0x7f char too
            {
                fputc( '\\', f_dump ); // octal escape introducer

                // Give me 5 instead of 3 octal digits = 15 bits:

                int n = ( ( wc >> 12 ) & 7 ) + '0';
                fputc( n, f_dump ); // octal digit 1
                n = ( ( wc >> 9 ) & 7 ) + '0';
                fputc( n, f_dump ); // octal digit 2
                n = ( ( wc >> 6 ) & 7 ) + '0';
                fputc( n, f_dump ); // octal digit 1
                n = ( ( wc >> 3 ) & 7 ) + '0';
                fputc( n, f_dump ); // octal digit 2
                n = ( wc & 7 ) + '0';
                fputc( n, f_dump ); // octal digit 3
            }
            else
            {
                fputc( wc, f_dump );
            }
        }
        fputc( '\r', f_dump );
        fputc( '\n', f_dump );
        MyFree( 338, UNPREDICTABLE, FullKey );
        FullKey = NULL;
    }
    MyFree( 341, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    fclose( f_dump );
    f_dump = NULL;

#endif // DO_DEBUG_DUMP


#endif // not _WIN32_WCE
}

int WordLetterValuation( wchar_t * szWord )
{
    // Took this out of CTxt to use in other places.
    // CTxt limited return value to 999, good range.

    // Being called from static ResortByFreqValueConsumer,
    // this routine must also be static; Also thread-safe.

    // szWord is lowercase, and may later include hyphens, apostraphes.
    // Use the KJV vowel and consonant runs to rate the wordness of it.

    int sum = 0;

    // revised from the loop of BuildConsonantVowelRuns...
    // But you better damn well name them back to these:
    // CSolNewCons -> CSolConsonantRunValues
    // CSolNewVows -> CSolVowelRunValues


    // Now I can perfect that rule about y is/not vowel.

    // my own unicode ( but not CJK ) int isvowel( wchar_t wc ) does:
    // Return 1 if wc is a vowel, 0 for a consonant ( assumes isalpha ).
    // Return 2 for 'Y', 'y' which hard case user must further sort out.

    // A consonant in a word is a letter other than A, E, I, O or U,
    // and other than Y preceded by a consonant.
    // -- http://snowball.tartarus.org/algorithms/porter/stemmer.html

    // This should have the same loop as WordLetterValuation,
    // only perfect it for 'y' with my own isvowel( ) routine.

    wchar_t * from = szWord;
    int priorConsonant = 0;
    wchar_t run[ MAX_CONSONANT_OR_VOWEL_RUN + 1 ]; // Consonant or Vowel runs
    int nFillingVows = 0;
    int nFillingCons = 0;

    for( ;; )
    {
        wchar_t wc = *from++;

        switch( isvowel( wc ) )
        {
        case 2:
            // Is a Y. -- Y is a vowel after a prior consonant.
            if( priorConsonant )
            {
                goto TreatYAsVowel;
            }
            else
            {
                goto TreatYAsConsonant;
            }
            break;
        case 1:
            // Is a vowel.
            {
                TreatYAsVowel : ;
                // So here, process ( another ) vowel.
                priorConsonant = 0;

                // Finish any valid consonants in progress.
                if( nFillingCons > 0
                && nFillingCons < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingCons ] = NULL;
                    unsigned int index = CSolConsonantRunValues.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1281" ); }
                    #endif
                    if( index > 1 )
                    {
                        sum += CSolConsonantRunValues.GetUserValue( index );
                    }
                }
                nFillingCons = 0;

                // Save this vowel into run.

                if( nFillingVows < MAX_CONSONANT_OR_VOWEL_RUN )
                    run[ nFillingVows++ ] = wc;
            }
            break;
        case 0:
            // Not a vowel. Test if even an alpha.
            if( iswalpha( wc ) )
            {
                TreatYAsConsonant : ;
                // So here, process ( another ) consonant.
                priorConsonant = 1;

                // Finish any valid Vowels in progress.
                if( nFillingVows > 0
                && nFillingVows < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingVows ] = NULL;
                    unsigned int index = CSolVowelRunValues.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        sum += CSolVowelRunValues.GetUserValue( index );
                    }
                }
                nFillingVows = 0;

                // Save this consonant into run.

                if( nFillingCons < MAX_CONSONANT_OR_VOWEL_RUN )
                    run[ nFillingCons++ ] = wc;
            }
            else
            {
                // May be numbers, puntuation.
                // N.B. May be at final NULL.
                priorConsonant = 0;

                // No character to save, but two finishers.

                // Finish any valid consonants in progress.
                if( nFillingCons > 0
                && nFillingCons < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingCons ] = NULL;
                    unsigned int index = CSolConsonantRunValues.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        sum += CSolConsonantRunValues.GetUserValue( index );
                    }
                }
                nFillingCons = 0;

                // Finish any valid Vowels in progress.
                if( nFillingVows > 0
                && nFillingVows < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingVows ] = NULL;
                    unsigned int index = CSolVowelRunValues.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        sum += CSolVowelRunValues.GetUserValue( index );
                    }
                }
                nFillingVows = 0;
            }
            break;
        }

        if( wc == NULL ) // Don't miss this opportunity!
            break;
    }


    int StrLen = from - szWord; // Note, off by one due to ++ on null

    return sum * 10 / ( StrLen + 10 ); // heuristic slightly favor short words
}


wchar_t * CoItFullKey( CoIt * pCoIt ) // returns a malloc, user frees
{
    // No Crisis: caller alone possesses the CoIt vector, keys never change.
    // I merely concatenate the two parts of key, and return non-null malloc.
    size_t nExtLen = 0;

    if( pCoIt->szKeyExt != NULL )
        nExtLen = wcslen( pCoIt->szKeyExt );

    size_t nMallocLen = NKEYSTART + nExtLen + 1;

    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 22, nMallocLen * sizeof( wchar_t ) );
    memcpy( MallocPtr, pCoIt->KeyStart, sizeof( pCoIt->KeyStart ) );

    MallocPtr[NKEYSTART] = NULL;

    if( pCoIt->szKeyExt != NULL )
        wcscpy( MallocPtr + NKEYSTART, pCoIt->szKeyExt );

    return MallocPtr; // never NULL
}


// Even after debug diagnostics are gone, I will
// keep the NULL check to exit, sparing all users
// from having to test NULL malloc/realloc return.
// No, that was drastic. Once I get a NULL pointer
// back from realloc, maybe AllUrls gets clobbered,
// and nothing more can run. Change all MyRealloc
// callers to test for NULL, and if NULL, to not
// change the old pointer they hold to the new,
// and MyRealloc will also not hurt old memory.
// After that, restore all the NULL ptr tests
// and zero index tests, etc, that may arise
// after a potential My( Re/M )alloc failure.

void * MyRealloc( int id, size_t hisslimoldsize, void *hisslimoldptr, size_t slimnewsize )
{
    size_t fatoldsize = hisslimoldsize + 12;
    size_t fatnewsize = slimnewsize + 12;
    char * slimoldptr = ( char * ) hisslimoldptr;
    char * fatoldptr = slimoldptr - 8;
    wchar_t wk[80];

    #if DO_MALLOC_SPEW
        wsprintf( wk, L"%08x %8d %8d old-real. ID#%4d", hisslimoldptr, ++MemOrdinal, hisslimoldsize, id );
        ; Spew( wk );
    #endif // DO_MALLOC_SPEW

    if( slimnewsize == 0 )
    {
        wsprintf( wk, L"%08x %8d %8d PASSED ZERO SIZE. ID#%4d", -1, ++MemOrdinal, slimnewsize, id );
        #if DO_MALLOC_SPEW
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
        ProgramError( wk );
        return NULL; // caller must keep using his old memory.
    }

    // I heard rumors realloc is not safe to pass in a NULL.
    // But, doing it myself, I will surely accept this NULL.
    if( hisslimoldptr != NULL )
    {
        // Not every free-er knows the old size, may pass 0.
        // No problem. I will put it below the check integer.
        if( slimoldptr[-1] != 'z'
        ||  slimoldptr[-2] != '7'
        ||  slimoldptr[-3] != 'x'
        ||  slimoldptr[-4] != 'g' )
        {
            wsprintf( wk, L"%08x %8d damaged low. ID#%4d", hisslimoldptr, ++MemOrdinal, id );
            #if DO_MALLOC_SPEW
                ; Spew( wk );
            #endif // DO_MALLOC_SPEW
            ProgramError( wk );
            return NULL; // caller must keep using his old memory.
        }
        else
        {
            size_t myslimoldsize = ( ( int* )slimoldptr )[-2];
            if( slimoldptr[myslimoldsize+0] != 'z'
            ||  slimoldptr[myslimoldsize+1] != '7'
            ||  slimoldptr[myslimoldsize+2] != 'x'
            ||  slimoldptr[myslimoldsize+3] != 'g' )
            {
                wsprintf( wk, L"%08x %8d damaged high. ID#%4d", hisslimoldptr, ++MemOrdinal, id );
                #if DO_MALLOC_SPEW
                    ; Spew( wk );
                #endif // DO_MALLOC_SPEW
                ProgramError( wk );
                return NULL; // caller must keep using his old memory.
            }
            if( hisslimoldsize != 0
            && hisslimoldsize != myslimoldsize )
            {
                wsprintf( wk, L"%08x %8d forgot size %d vs. %d. ID#%4d", hisslimoldptr, ++MemOrdinal, hisslimoldsize, myslimoldsize, id );
                #if DO_MALLOC_SPEW
                    ; Spew( wk );
                #endif // DO_MALLOC_SPEW
                ProgramError( wk );
                return NULL; // caller must keep using his old memory.
            }
        }
    }

    if( SpewFile != NULL )
        fflush( SpewFile );

    // I still get failures here that should not be,
    // after all that error checking. I will replace
    // this call to realloc with a malloc and a free.
    // THAT seems to have stopped a crash in realloc!
    // No, it was my failure to interlock the threads.
    // But, maybe not. I had a crash in realloc in ^C,
    // then changed to 3 steps, and ^C had no crash.
    // Maybe realloc in debug mode made that feeefeee
    // fill, which forces one of my subsequence uses
    // via old memory to fail!? Let's do a memfill.
    // Is the memfill with 0 crashing me? Probably.
    // Memfill with -1 ran okay. Or, due to chance.

    /****** do it ******/
    // 1.2.3.
    // void * fatnewptr = realloc( fatoldptr, fatnewsize );

    // 1.
    void * fatnewptr = malloc( fatnewsize );

    if( fatnewptr == NULL )
    {
        // implement the fallback strategy.
        wsprintf( wk, L"%08x %8d %8d REALLOC RETURNED ZERO PTR. ID#%4d", -1, ++MemOrdinal, slimnewsize, id );
        #if DO_MALLOC_SPEW
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
        slimoldptr[-1] = 'z'; // undo pre-free change from z
        ProgramError( wk );
        return NULL; // caller must keep using his old memory.
    }

    // I heard rumors realloc is not safe to pass in a NULL.
    // But, doing it myself, I will surely accept this NULL.
    if( hisslimoldptr != NULL )
    {
        slimoldptr[-1] = 'y'; // change from z in case of 2 frees...

        // 2.
        memcpy( fatnewptr, fatoldptr, fatoldsize );

        // 2.5 -- Is this crashing me? Yes, definitely. Go find my errors.
        // 2.5 -- Maybe -1 wont be so bad, and VC Debug can trap it:
        // Or not...
        // memset( fatoldptr, 0, fatoldsize );
        // memset( fatoldptr, -1, fatoldsize );

        // 3.
        free( fatoldptr );

        TotalMallocBytes -= fatoldsize;
    }

    TotalMallocBytes += fatnewsize;

    if( TotalMallocBytes > TOTAL_MALLOC_LIMIT )
        gbMallocLimitExceeded = 1;

    char * slimnewptr = ( char * ) fatnewptr + 8;

    slimnewptr[-1] = 'z';
    slimnewptr[-2] = '7';
    slimnewptr[-3] = 'x';
    slimnewptr[-4] = 'g';
    slimnewptr[slimnewsize+0] = 'z';
    slimnewptr[slimnewsize+1] = '7';
    slimnewptr[slimnewsize+2] = 'x';
    slimnewptr[slimnewsize+3] = 'g';
    ( ( int* )slimnewptr )[-2] = slimnewsize;

    if( slimnewptr != slimoldptr )
    {
        #if DO_MALLOC_SPEW
            wsprintf( wk, L"%08x %8d->%08x. ID#%4d", slimoldptr, ++MemOrdinal, slimnewptr, id );
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
    }
    #if DO_MALLOC_SPEW
        wsprintf( wk, L"%08x %8d %8d new-real. ID#%4d", slimnewptr, ++MemOrdinal, slimnewsize, id );
        ; Spew( wk );
    #endif // DO_MALLOC_SPEW
    return slimnewptr;
}

void * MyMalloc( int id, size_t slimnewsize )
{
    size_t fatnewsize = slimnewsize + 12;
    wchar_t wk[80];

    if( slimnewsize == 0 )
    {
        wsprintf( wk, L"%08x %8d %8d PASSED ZERO SIZE. ID#%4d", -1, ++MemOrdinal, slimnewsize, id );
        #if DO_MALLOC_SPEW
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
        ProgramError( wk );
        return NULL; // all callers must restore their null tests.
    }

    if( SpewFile != NULL )
        fflush( SpewFile );

    /****** do it ******/
    void * fatnewptr = malloc( fatnewsize );

    if( fatnewptr == NULL )
    {
        // implement the fallback strategy.
        wsprintf( wk, L"%08x %8d %8d MALLOC RETURNED ZERO PTR. ID#%4d", -1, ++MemOrdinal, slimnewsize, id );
        #if DO_MALLOC_SPEW
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
        ProgramError( wk );
        return NULL; // all callers must restore their null tests.
    }

    TotalMallocBytes += fatnewsize;

    if( TotalMallocBytes > TOTAL_MALLOC_LIMIT )
        gbMallocLimitExceeded = 1;

    char * slimnewptr = ( char * ) fatnewptr + 8;
    slimnewptr[-1] = 'z';
    slimnewptr[-2] = '7';
    slimnewptr[-3] = 'x';
    slimnewptr[-4] = 'g';
    slimnewptr[slimnewsize+0] = 'z';
    slimnewptr[slimnewsize+1] = '7';
    slimnewptr[slimnewsize+2] = 'x';
    slimnewptr[slimnewsize+3] = 'g';
    ( ( int* )slimnewptr )[-2] = slimnewsize;

    #if DO_MALLOC_SPEW
        wsprintf( wk, L"%08x %8d %8d new-allc. ID#%4d", slimnewptr, ++MemOrdinal, slimnewsize, id );
        ; Spew( wk );
    #endif // DO_MALLOC_SPEW
    return slimnewptr;
}

void MyFree( int id, size_t hisslimoldsize, void * hisslimoldptr )
{
    char * slimoldptr = ( char * ) hisslimoldptr;
    char * fatoldptr = slimoldptr - 8;
    size_t fatoldsize = 0;
    wchar_t wk[80];
    #if DO_MALLOC_SPEW
        wsprintf( wk, L"%08x %8d %8d old-free. ID#%4d", hisslimoldptr, ++MemOrdinal, hisslimoldsize, id );
        ; Spew( wk );
    #endif // DO_MALLOC_SPEW

    if( hisslimoldptr == NULL )
    {
        wsprintf( wk, L"%08x %8d %8d PASSED ZERO PTR TO FREE. ID#%4d", hisslimoldptr, ++MemOrdinal, hisslimoldsize, id );
        #if DO_MALLOC_SPEW
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
        ProgramError( wk );
    }

    // Not every free-er knows the old size, may pass 0.
    // No problem. I will put it below the check integer.
    if( slimoldptr[-1] != 'z'
    ||  slimoldptr[-2] != '7'
    ||  slimoldptr[-3] != 'x'
    ||  slimoldptr[-4] != 'g' )
    {
        wsprintf( wk, L"%08x %8d damaged low. ID#%4d", hisslimoldptr, ++MemOrdinal, id );
        #if DO_MALLOC_SPEW
            ; Spew( wk );
        #endif // DO_MALLOC_SPEW
        ProgramError( wk );
    }
    else
    {
        size_t myslimoldsize = ( ( int* )slimoldptr )[-2];
        fatoldsize = myslimoldsize + 12;
        if( slimoldptr[myslimoldsize+0] != 'z'
        ||  slimoldptr[myslimoldsize+1] != '7'
        ||  slimoldptr[myslimoldsize+2] != 'x'
        ||  slimoldptr[myslimoldsize+3] != 'g' )
        {
            wsprintf( wk, L"%08x %8d damaged high. ID#%4d", hisslimoldptr, ++MemOrdinal, id );
            #if DO_MALLOC_SPEW
                ; Spew( wk );
            #endif // DO_MALLOC_SPEW
            ProgramError( wk );
        }
        if( hisslimoldsize != 0
        && hisslimoldsize != myslimoldsize )
        {
            wsprintf( wk, L"%08x %8d forgot size %d vs. %d. ID#%4d", hisslimoldptr, ++MemOrdinal, hisslimoldsize, myslimoldsize, id );
            #if DO_MALLOC_SPEW
                ; Spew( wk );
            #endif // DO_MALLOC_SPEW
            ProgramError( wk );
        }
    }

    if( SpewFile != NULL )
        fflush( SpewFile );

    /****** do it ******/
    slimoldptr[-1] = 'y'; // change from z in case of 2 frees...
    free( fatoldptr );

    TotalMallocBytes -= fatoldsize;

}

char bVowels[0x200] = {
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x000 - 0x00f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x010 - 0x01f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x020 - 0x02f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x030 - 0x03f
    0,1,0,0, 0,1,0,0, 0,1,0,0, 0,0,0,1, // 0x040 - 0x04f
    0,0,0,0, 0,1,0,0, 0,2,0,0, 0,0,0,0, // 0x050 - 0x05f -- 'Y' is 2!
    0,1,0,0, 0,1,0,0, 0,1,0,0, 0,0,0,1, // 0x060 - 0x06f
    0,0,0,0, 0,1,0,0, 0,2,0,0, 0,0,0,0, // 0x070 - 0x07f -- 'y' is 2!
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x080 - 0x08f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x090 - 0x09f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x0a0 - 0x0af
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x0b0 - 0x0bf
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x0c0 - 0x0cf
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x0d0 - 0x0df
    0,1,1,1, 1,1,1,0, 1,1,1,0, 0,1,0,0, // 0x0e0 - 0x0ef
    0,0,0,1, 1,1,1,0, 1,0,1,0, 1,1,0,0, // 0x0f0 - 0x0ff
    0,1,0,1, 0,1,0,0, 0,0,0,0, 0,0,0,0, // 0x100 - 0x10f
    0,0,0,1, 0,0,0,1, 0,1,0,1, 0,0,0,0, // 0x110 - 0x11f
    0,0,0,0, 0,0,0,0, 0,1,0,1, 0,0,0,1, // 0x120 - 0x12f
    0,1,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,0, // 0x130 - 0x13f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,1,0,0, // 0x140 - 0x14f
    0,1,0,0, 0,1,0,0, 0,0,0,0, 0,0,0,0, // 0x150 - 0x15f
    0,0,0,0, 0,0,0,0, 0,1,0,1, 0,0,0,1, // 0x160 - 0x16f
    0,1,0,1, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x170 - 0x17f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x180 - 0x18f
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x190 - 0x19f
    0,1,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x1a0 - 0x1af
    1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x1b0 - 0x1bf
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x1c0 - 0x1cf
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x1d0 - 0x1df
    0,0,0,0, 0,0,0,0, 0,1,0,0, 0,0,0,0, // 0x1e0 - 0x1ef
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0x1f0 - 0x1ff
};

int isvowel( wchar_t wc )
{
    // Return 1 if wc is a vowel, 0 for a consonant ( assumes isalpha ).
    // Return 2 for 'Y', 'y' which hard case user must further sort out.

    // A consonant in a word is a letter other than A, E, I, O or U,
    // and other than Y preceded by a consonant.
    // -- http://snowball.tartarus.org/algorithms/porter/stemmer.html

    // sed, nasty.exe and sed analysis of http://www.answers.com/topic/vowel
    // produced a list of UTF-8 sequences that are VOWELS in many languages:
    // 3 0xc3 0xa1
    // 4 0xc3 0xa2
    // 2 0xc3 0xa3
    // 8 0xc3 0xa4
    // 2 0xc3 0xa5
    // 1 0xc3 0xa6
    // 1 0xc3 0xa8
    // 5 0xc3 0xa9
    // 1 0xc3 0xaa
    // 4 0xc3 0xad
    // 5 0xc3 0xb3
    // 1 0xc3 0xb4
    // 5 0xc3 0xb5
    // 7 0xc3 0xb6
    // 1 0xc3 0xb8
    // 3 0xc3 0xba
    // 5 0xc3 0xbc
    // 2 0xc3 0xbd
    // 4 0xc4 0x81
    // 1 0xc4 0x83
    // 2 0xc4 0x85
    // 3 0xc4 0x93
    // 1 0xc4 0x97
    // 2 0xc4 0x99
    // 1 0xc4 0x9b
    // 1 0xc4 0xa9
    // 2 0xc4 0xab
    // 1 0xc4 0xaf
    // 2 0xc4 0xb1
    // 1 0xc4 0xba
    // 2 0xc5 0x8d
    // 1 0xc5 0x91
    // 1 0xc5 0x95
    // 1 0xc5 0xa9
    // 4 0xc5 0xab
    // 1 0xc5 0xaf
    // 1 0xc5 0xb1
    // 1 0xc5 0xb3
    // 1 0xc6 0xa1
    // 1 0xc6 0xb0
    // 2 0xc7 0xa9
    // 1 0xc9 0x99
    // 1 0xce 0xb1
    // 1 0xce 0xb5
    // 1 0xce 0xb7
    // 1 0xce 0xb9
    // 1 0xce 0xbf
    // 1 0xcf 0x85
    // 1 0xcf 0x89
    // 2 0xd0 0x81
    // 1 0xd0 0x84
    // 1 0xd0 0x86
    // 1 0xd0 0x87
    // 4 0xd0 0x90
    // 4 0xd0 0x95
    // 3 0xd0 0x98
    // 4 0xd0 0x9e
    // 4 0xd0 0xa3
    // 1 0xd0 0xaa
    // 2 0xd0 0xab
    // 2 0xd0 0xad
    // 4 0xd0 0xae
    // 4 0xd0 0xaf
    // 1 0xd5 0xa1
    // 1 0xd5 0xa5
    // 2 0xd5 0xa7
    // 1 0xd5 0xa8
    // 2 0xd5 0xab
    // 2 0xd5 0xb8
    // 2 0xd6 0x82
    // 2 0xd6 0x85
    // 1 0xd6 0xb0
    // 1 0xd6 0xb1
    // 1 0xd6 0xb2
    // 1 0xd6 0xb3
    // 1 0xd6 0xb4
    // 1 0xd6 0xb5
    // 1 0xd6 0xb6
    // 1 0xd6 0xb7
    // 1 0xd6 0xb8
    // 1 0xd6 0xb9
    // 1 0xd6 0xbb
    // 1 0xd8 0xa7
    // 1 0xd9 0x88
    // 1 0xd9 0x8a
    // 1 0xd9 0x8e
    // 1 0xd9 0x8f
    // 1 0xd9 0x90
    // 1 0xe0 0xa4 0x85
    // 1 0xe0 0xa4 0x86
    // 1 0xe0 0xa4 0x87
    // 1 0xe0 0xa4 0x88
    // 1 0xe0 0xa4 0x89
    // 1 0xe0 0xa4 0x8a
    // 1 0xe0 0xa4 0x8f
    // 1 0xe0 0xa4 0x90
    // 1 0xe0 0xa4 0x93
    // 1 0xe0 0xa4 0x94
    // 1 0xe0 0xa4 0xbe
    // 1 0xe0 0xa4 0xbf
    // 1 0xe0 0xa5 0x80
    // 1 0xe0 0xa5 0x81
    // 1 0xe0 0xa5 0x82
    // 1 0xe0 0xa5 0x87
    // 1 0xe0 0xa5 0x88
    // 1 0xe0 0xa5 0x8b
    // 1 0xe0 0xa5 0x8c
    // 1 0xe0 0xae 0x85
    // 1 0xe0 0xae 0x86
    // 1 0xe0 0xae 0x87
    // 1 0xe0 0xae 0x88
    // 1 0xe0 0xae 0x89
    // 1 0xe0 0xae 0x8a
    // 1 0xe0 0xae 0x8e
    // 1 0xe0 0xae 0x8f
    // 1 0xe0 0xae 0x90
    // 1 0xe0 0xae 0x92
    // 1 0xe0 0xae 0x93
    // 1 0xe0 0xae 0x94
    // 1 0xe1 0xba 0xa7
    // 1 0xe1 0xba 0xbd
    // 1 0xe1 0xbb 0xa9
    // 1 0xe1 0xbb 0xb9
    // 1 0xe3 0x81 0x82
    // 1 0xe3 0x81 0x84
    // 1 0xe3 0x81 0x86
    // 1 0xe3 0x81 0x88
    // 1 0xe3 0x81 0x8a
    // 1 0xe3 0x81 0xb8
    // 1 0xe3 0x82 0x92
    // 1 0xe3 0x85 0x8f
    // 1 0xe3 0x85 0x90
    // 1 0xe3 0x85 0x91
    // 1 0xe3 0x85 0x92
    // 1 0xe3 0x85 0x93
    // 1 0xe3 0x85 0x94
    // 1 0xe3 0x85 0x95
    // 1 0xe3 0x85 0x96
    // 1 0xe3 0x85 0x97
    // 1 0xe3 0x85 0x98
    // 1 0xe3 0x85 0x99
    // 1 0xe3 0x85 0x9a
    // 1 0xe3 0x85 0x9b
    // 1 0xe3 0x85 0x9c
    // 1 0xe3 0x85 0x9d
    // 1 0xe3 0x85 0x9e
    // 1 0xe3 0x85 0x9f
    // 1 0xe3 0x85 0xa0
    // 1 0xe3 0x85 0xa1
    // 1 0xe3 0x85 0xa2
    // 1 0xe3 0x85 0xa3

    // using unhex, notepad, hex to convert to big-endian wides:

    // 000000:  FE FF 00 E1 00 E2 00 E3 00 E4 00 E5 00 E6 00 E8   ................
    // 000010:  00 E9 00 EA 00 ED 00 F3 00 F4 00 F5 00 F6 00 F8   ................
    // 000020:  00 FA 00 FC 00 FD 01 01 01 03 01 05 01 13 01 17   ................
    // 000030:  01 19 01 1B 01 29 01 2B 01 2F 01 31 01 3A 01 4D   ..... ).+./.1.:.M
    // 000040:  01 51 01 55 01 69 01 6B 01 6F 01 71 01 73 01 A1   .Q.U.i.k.o.q.s..
    // 000050:  01 B0 01 E9 02 59 03 B1 03 B5 03 B7 03 B9 03 BF   .....Y..........
    // 000060:  03 C5 03 C9 04 01 04 04 04 06 04 07 04 10 04 15   ................
    // 000070:  04 18 04 1E 04 23 04 2A 04 2B 04 2D 04 2E 04 2F   .....#.*.+.-.../
    // 000080:  05 61 05 65 05 67 05 68 05 6B 05 78 05 82 05 85   .a.e.g.h.k.x....
    // 000090:  05 B0 05 B1 05 B2 05 B3 05 B4 05 B5 05 B6 05 B7   ................
    // 0000A0:  05 B8 05 B9 05 BB 06 27 06 48 06 4A 06 4E 06 4F   .......'.H.J.N.O
    // 0000B0:  06 50 09 05 09 06 09 07 09 08 09 09 09 0A 09 0F   .P..............
    // 0000C0:  09 10 09 13 09 14 09 3E 09 3F 09 40 09 41 09 42   .......>.?.@.A.B
    // 0000D0:  09 47 09 48 09 4B 09 4C 0B 85 0B 86 0B 87 0B 88   .G.H.K.L........
    // 0000E0:  0B 89 0B 8A 0B 8E 0B 8F 0B 90 0B 92 0B 93 0B 94   ................
    // 0000F0:  1E A7 1E BD 1E E9 1E F9 30 42 30 44 30 46 30 48   ........0B0D0F0H
    // 000100:  30 4A 30 78 30 92 31 4F 31 50 31 51 31 52 31 53   0J0x0.1O1P1Q1R1S
    // 000110:  31 54 31 55 31 56 31 57 31 58 31 59 31 5A 31 5B   1T1U1V1W1X1Y1Z1[
    // 000120:  31 5C 31 5D 31 5E 31 5F 31 60 31 61 31 62 31 63   1\1]1^1_1`1a1b1c

    // The 3-UTF-8 chars are all beyond wide_t, so ignored.
    // The rest I can split between an array[512] and if's.
    // A side C++ program vowut.cpp created the array body.

    if( wc < 0x200 )
    {
        return ( int ) bVowels[ ( unsigned int ) wc ];
    }
    else
    {
        switch( ( int ) bVowels[ ( unsigned int ) wc ] )
        {
        case 0x01B0: case 0x01E9: case 0x0259: case 0x03B1: case 0x03B5: case 0x03B7: case 0x03B9: case 0x03BF:
        case 0x03C5: case 0x03C9: case 0x0401: case 0x0404: case 0x0406: case 0x0407: case 0x0410: case 0x0415:
        case 0x0418: case 0x041E: case 0x0423: case 0x042A: case 0x042B: case 0x042D: case 0x042E: case 0x042F:
        case 0x0561: case 0x0565: case 0x0567: case 0x0568: case 0x056B: case 0x0578: case 0x0582: case 0x0585:
        case 0x05B0: case 0x05B1: case 0x05B2: case 0x05B3: case 0x05B4: case 0x05B5: case 0x05B6: case 0x05B7:
        case 0x05B8: case 0x05B9: case 0x05BB: case 0x0627: case 0x0648: case 0x064A: case 0x064E: case 0x064F:
        case 0x0650: case 0x0905: case 0x0906: case 0x0907: case 0x0908: case 0x0909: case 0x090A: case 0x090F:
        case 0x0910: case 0x0913: case 0x0914: case 0x093E: case 0x093F: case 0x0940: case 0x0941: case 0x0942:
        case 0x0947: case 0x0948: case 0x094B: case 0x094C: case 0x0B85: case 0x0B86: case 0x0B87: case 0x0B88:
        case 0x0B89: case 0x0B8A: case 0x0B8E: case 0x0B8F: case 0x0B90: case 0x0B92: case 0x0B93: case 0x0B94:
        case 0x1EA7: case 0x1EBD: case 0x1EE9: case 0x1EF9: // I willfully ignore above 0x3000, CJK
            return 1;
        }
    }
    return 0;
}

unsigned int CrcHash( wchar_t * sz )
{
    #if DO_DEBUG_CALLS
        Routine( L"132" );
    #endif
    // Hash uses CRC-16 forward and backward on URL string.
    // Just let any extra non-zero wchar_t bit in for free.
    wchar_t * scan = sz;
    unsigned int crc1 = 0x0000ffff;
    for( ;; )
    {
        unsigned int i = ( unsigned int ) *scan;
        if( i == 0 )
            break;
        crc1 ^= i << 8;
        crc1 <<= 8;
        if ( crc1 & 0x010000L << 7 ) crc1 ^= 0x011021L << 7;
        if ( crc1 & 0x010000L << 6 ) crc1 ^= 0x011021L << 6;
        if ( crc1 & 0x010000L << 5 ) crc1 ^= 0x011021L << 5;
        if ( crc1 & 0x010000L << 4 ) crc1 ^= 0x011021L << 4;
        if ( crc1 & 0x010000L << 3 ) crc1 ^= 0x011021L << 3;
        if ( crc1 & 0x010000L << 2 ) crc1 ^= 0x011021L << 2;
        if ( crc1 & 0x010000L << 1 ) crc1 ^= 0x011021L << 1;
        if ( crc1 & 0x010000L << 0 ) crc1 ^= 0x011021L << 0;
        scan ++;
    }
    unsigned int crc2 = 0x0000ffff;
    for( ;; )
    {
        scan --;
        unsigned int i = ( unsigned int ) *scan;
        crc2 ^= i << 8;
        crc2 <<= 8;
        if ( crc2 & 0x010000L << 7 ) crc2 ^= 0x011021L << 7;
        if ( crc2 & 0x010000L << 6 ) crc2 ^= 0x011021L << 6;
        if ( crc2 & 0x010000L << 5 ) crc2 ^= 0x011021L << 5;
        if ( crc2 & 0x010000L << 4 ) crc2 ^= 0x011021L << 4;
        if ( crc2 & 0x010000L << 3 ) crc2 ^= 0x011021L << 3;
        if ( crc2 & 0x010000L << 2 ) crc2 ^= 0x011021L << 2;
        if ( crc2 & 0x010000L << 1 ) crc2 ^= 0x011021L << 1;
        if ( crc2 & 0x010000L << 0 ) crc2 ^= 0x011021L << 0;
        if( scan == sz )
            break;
    }
    return ( ( ~ crc2 & 0x0000ffff ) << 16 ) | ( ~ crc1 & 0x0000ffff );
}

