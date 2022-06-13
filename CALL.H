// This is file: CAll.h
// Copyright ( C ) 2006, Glenn Scheper

// epoch seconds
#define BUILD_TIME 1214249846 // V1.0 = 1200673538
// as yyyymmdd
#define STAR_DATE 20080623 // V1.0 = 20080118


// Additions 2013-02-12 when I tried to recompile in VS 2008, but came back to VS 2010.
#pragma warning(disable: 4996) // against many security warnings: _CRT_SECURE_NO_WARNINGS

// Despite adding these, I think I may still need to add the .lib to project settings->linker->additional inputs
#pragma comment( lib, "Comctl32" )
#pragma comment( lib, "wininet" )


// If I happen to be compiling at home, change X:\\ paths
// below into C:\\ paths, if you want to see a spew file.
// No, everybody's got a C drive, Just remember it is here:
// #define SpewFileName "C:\\WordsEx\\spew.txt"


    // 1 for real: (rare things)

// Use 1 for real. Set 0 here during rebuild word lists.
#define OKAY_TO_SALT_PROPER 01
// Use 1 for real. Set 0 here during rebuild word lists.
#define OKAY_TO_SALT_LISTS  01
// Use 1 for real. Set 0 here during rebuild word lists.
#define OKAY_TO_SALT_COMMON 01
// Use 1 for real. Set 0 here during rebuild word lists.
#define OKAY_TO_ASSIGN_LANG 01
// Use 1 for real. Set 0 here during rebuild word lists.
#define OKAY_TO_GUESS_LANG  01

// I have yet to decide if FTP hangs, or has other issues.
// With the latest Windows updates, I haven't seen a hang.
// But I need to pound on some ftp urls to be sure... wip!
#define OKAY_TO_GET_FPT_URL 01


    // 0 for real: (many such)

// Use 0 for real. Set 1 here only to recompute vowels+consonant lists.
#define DEBUG_REVISE_VOWELS 0

// troubleshooting aids. Use all 0 in real build.
#define DO_DEBUG_COMMONWORD 0
#define DO_DEBUG_SALTING    0
#define DO_DEBUG_VALUATION  0
#define DO_DEBUG_LANG_GUESS 0
#define DO_DEBUG_LANG_MATH  0
#define DO_DEBUG_VOCADD     0
#define DO_DEBUG_LANGUAGE   0
#define DO_DEBUG_LANGUAGE2  0
#define DO_DEBUG_LANG_MULT  0
#define DO_DEBUG_LANG_BKWD  0

#define DO_DEBUG_SENTENCES  0
#define DO_DEBUG_OMEGA1     0
#define DO_DEBUG_COMMENT    0
#define DO_DEBUG_HTMLTAGS   0
#define DO_DEBUG_HTMLATTRS  0
#define DO_DEBUG_TEXTTOKEN  0
#define DO_DEBUG_DISCARD    0
#define DO_DEBUG_BLOCK_NESS 0
#define DO_DEBUG_LEFTOVERS  0
#define DO_DEBUG_REFUSAL    0
#define DO_DEBUG_IF_ENDIF   0
#define DO_STUDY_ALL_QRP    0
#define DO_DEBUG_LOAD_ENG   0
#define DO_DEBUG_ORDINALS   0
#define DO_DEBUG_BITBLT     0
#define DO_DEBUG_SCRAPING   0
#define DO_DEBUG_XREF       0
#define DO_DEBUG_CACHE      0
#define DO_DEBUG_CLEARURLS  0
#define DO_DEBUG_PAPER      0
#define DO_DEBUG_PATH       0
#define DO_DEBUG_FONT       0
#define DO_DEBUG_WNDPROC    0
#define DO_DEBUG_PAINT      0

#define DO_VOCABULARY_DUMP  0
#define DO_DEBUG_DUMP       0
#define DO_DEBUG_VOCDUMP    0
#define DO_DEBUG_INPUT      0
#define DO_DEBUG_FORM0      0
#define DO_DEBUG_FORM1      0
#define DO_DEBUG_FORM2      0
#define DO_DEBUG_REFRESH    0
#define DO_DEBUG_TITLE      0
#define DO_DEBUG_LINKIDX    0
#define DO_FORMS_DUMP       0
#define DO_DEBUG_CANON      0
#define DO_DEBUG_TIMER      0
#define DO_DEBUG_BITMAP     0
#define DO_DEBUG_FITER      0
#define DO_DEBUG_EVALUATE   0
#define DO_DEBUG_CRITX      0
#define DO_DEBUG_SCREEN     0
#define DO_DEBUG_TOP        0
#define DO_DEBUG_ANCHOR     0
#define DO_DEBUG_URLFILENM  0
#define DO_DEBUG_HTMLDEEP   0
#define DO_DEBUG_BASE       0
#define DO_DEBUG_AREA       0
#define DO_DEBUG_INTERLEAVE 0
#define DO_DEBUG_HTTPDATE   0
#define DO_DEBUG_ENTITY     0
#define DO_DEBUG_XMLTAG     0
#define DO_DEBUG_CONTENT    0
#define DO_DEBUG_PAGE       0
#define DO_DEBUG_MAIN       0
#define DO_DEBUG_PHRASE     0
#define DO_DEBUG_MOUSE      0
#define DO_DEBUG_FIND       0
#define DO_SEE_NOVEL_TAGS   0
#define DO_SEE_NOVEL_ATTRS  0

// This block true may help me solve "NOT" work, and URLs that stop thread:
#define DO_DEBUG_FETCH      01
#define DEBUG_SHOW_HEADER   01
#define DO_DEBUG_FACTTEST   01
#define DO_DEBUG_NEWQUERY   01
#define DO_DEBUG_QUERY      01
#define DO_DEBUG_BINARIES   01
#define DO_DEBUG_ADDFIND    01
#define DO_DEBUG_FOLLOW     01
#define DO_DEBUG_REJECTS    01
#define DO_DEBUG_FRAME      01
#define DO_DEBUG_TOANYDOT   01
#define DO_DEBUG_HTTPHDRS   01
// This is applicable when fetch + save non-text, saves raw html.
#define DO_DEBUG_SAVEHTML   01

// This one turns on the powerful Routine() tracer:
#define DO_DEBUG_CALLS      0

#define WHAT_CACHE_FILENAME 0
#define LET_ESCAPE_EXIT     0
#define DO_MALLOC_SPEW      0
#define DO_CLOSE_SPEWS      0


// forward declarations of all class names.

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

// File paths and names on PPC or Desktop


#ifdef _WIN32_WCE
    // This compiles under MicroSoft Embedded Visual C++ 3.0.
    #define DefaultFolderFilename       L"\\My Documents\\WordsEx"
    #define DefaultEnginesFilename       L"\\My Documents\\WordsEx\\Engines.txt"
    #define DumpFileName "\\My Documents\\dump.txt"
    #define SpewFileName "\\My Documents\\spew.txt"
    #define IExploreFileName L"\\Windows\\iexplore.exe"
#else
    // This compiles under MicroSoft Visual Studio 6.0 C++.
    // BIG NEWS: You cannot create a file in Vista C:\ any more!
    #define DefaultFolderFilename       L"C:\\WordsEx"
    #define DefaultEnginesFilename       L"C:\\WordsEx\\Engines.txt"
    //#define DumpFileName "X:\\xec\\vc\\ws\\dump.txt"
    //#define WordFileName "X:\\xec\\vc\\ws\\word.txt"
    #define DumpFileName "C:\\WordsEx\\dump.txt"
    #define WordFileName "C:\\WordsEx\\word.txt"
    #define SpewFileName "C:\\WordsEx\\spew.txt"
    #define IExploreFileName L"C:\\Program Files\\Internet Explorer\\iexplore.exe"
#endif


// zx is a marker where I could add a self-check parameter
// UNPREDICTABLE is a marker where I will omit MyFree self-check parameter
#define zx 0
#define UNPREDICTABLE 0

#ifdef _MBCS
Stop! It's a brave new world: Change project settings to _UNICODE,UNICODE.
#endif

#ifndef _UNICODE
Stop! It's a brave new world: Change project settings to _UNICODE,UNICODE.
#endif

#ifndef UNICODE
Stop! It's a brave new world: Change project settings to _UNICODE,UNICODE.
#endif

typedef unsigned char BYTE;
typedef unsigned long DWORD;

// One rule to untangle Crix is to not SendMessage between threads.
// So main thread must poll, rather than worker thread update view.
// WM_USER + 0 and +1 may be in use for some Find-or-Replace thing.
#define MONITOR_THREADS_TIMER           ( WM_USER + 11 )
#define SMOOTH_SCROLL_TIMER             ( WM_USER + 12 )
// But PostMessage is still okay between threads.
#define WM_THREADENDING                 ( WM_USER + 13 )
#define WM_THREADCHANGE                 ( WM_USER + 14 )
#define WM_SAVEPROGRESS                 ( WM_USER + 15 )
#define GROW_OMEGA_TIMER                ( WM_USER + 16 )

// Splash Screen parameters:
//
// Suppose I have a ? Hertz redraw rate, stop growth in N minutes:
//
// At 20 ms, with floating math on 666 Mhz cpu, paint slows down.
// BTWTask Manager shows 100% cpu utilization with floating point.
//
// On fast vista laptop, must do rotation and translation slower.
// ( Because fast vista was actually doing one redraw per 40 ms,
// whereas my Win2K probably only serviced a few per second. )
// With fixed point max, etc., this gave about 50% cpu usage.
//
#ifdef _WIN32_WCE
    #define GROW_OMEGA_MS                   500
    #define FINAL_OMEGA_MS                  ( 1 * 60 * 1000 )
#else // not _WIN32_WCE
    #define GROW_OMEGA_MS                   200
    #define FINAL_OMEGA_MS                  ( 8 * 60 * 1000 )
#endif // _WIN32_WCE


// These defines clarify some of the unused ( when 0 valued ) arguments
// passed to RecursiveGetter( ), CommonGetter( ), and GetterHelper( ),
// Also to CPag's ProcessPaper( ), and eventually to CHtm constructor.

#define ZEROETH         0       // for RecursiveGetter's parameter Recursions
#define YES_TOP         1       // for all's parameter bTopUrl
#define NOT_TOP         0       // for all's parameter bTopUrl
#define NO_BUDLOG       NULL    // for all's parameter pBudLog - not needed
#define NO_BINPATH      NULL    // for all's parameter szBinaryPath
#define NO_FRAMES       NULL    // for all's parameter pSolFrames - CFio uses
#define NO_MORES        NULL    // for all's parameter pSolMores - CFio uses
#define NO_HITS         NULL    // for all's parameter pSolHits - CFio uses
#define NO_QRURL        NULL    // for 1-2's parameter szQueryResultUrl
#define NO_SURL         NULL    // for all's parameter pOneSurl
#define YES_PAGE_IS_A_QRP 1     // for all's parameter PageIsAQrp
#define ZERO_ORDINAL_FOR_NON_QRP 0 // for all's parameter FromEngineNo, also for PageIsAQrp, etc.
#define BIT_IN_ORDINAL_TO_KEEP  0x1000 // to OR into QRPOrdinal number 1-999
#define SHOW_AS_NON_QRP 0       // for all's parameter ShowAsAQrp

#define RULETYPE_INVALID        0
#define RULETYPE_GOOD           1
#define RULETYPE_BAD            2
#define RULETYPE_MORE           3
#define RULETYPE_NONE_UNTIL     4
#define RULETYPE_NONE_AFTER     5
#define RULETYPE_GET            6
#define RULETYPE_NOT            7
#define RULETYPE_KEEP           8
#define RULETYPE_STUDY          9

#define RULESUBTYPE_UNUSED      0
#define RULESUBTYPE_NEXT        1
#define RULESUBTYPE_PRIOR       2
#define RULESUBTYPE_ANCHOR      3
#define RULESUBTYPE_ANCHORTEXT  4

#define WHERENESS_BALD          1
#define WHERENESS_TREBLE        2


enum {
    NO_NUMBER = 0,
    NUMBER_STRICT,
    NUMBER_POINTS,
    NUMBER_COMMAS,
    NUMBER_HYPHENS,
    NUMBER_SLASHS,
    NUMBER_COLONS,
    NUMBER_MIXED,
};


// Statuses due to a fetch operation. Empirically, cache always shows 200.

enum {
    FETCH_STATUS_NOT_ATTEMPTED = 0,
    FETCH_STATUS_200_SUCCESS = 200,
    FETCH_STATUS_200_BUT_CONTENT_UNDESIRABLE = 250,
    FETCH_STATUS_300_REDIRECT = 300,
    FETCH_STATUS_400_FAILURE = 400,
};


// Special "pointer"-to-pOnePaper values are used
// to reflect various statuses during a fetch etc.

#define PVOID_UNTRIED       NULL
#define PVOID_CLAIMING      ( ( void* )( -1L ) )
#define PVOID_NOTFOUNDETC   ( ( void* )( -2L ) )
#define PVOID_REDIRECTION   ( ( void* )( -3L ) )
#define PVOID_UNDESIRABLE   ( ( void* )( -4L ) )
#define PVOID_QUERYREJECT   ( ( void* )( -5L ) )
#define PVOID_VALID_BELOW   ( ( void* )( -6L ) )


// Facts from parse of http header or other analyses:

enum {
    CONTENT_UNKNOWN = 0,
    CONTENT_OTHER,
    CONTENT_TEXT,
    CONTENT_HTMLTEXT,
    CONTENT_PLAINTEXT,
    CONTENT_RTFTEXT,
    CONTENT_OTHERTEXT
};

enum {
    ENCODING_NONE = 0,
    ENCODING_USED
};

#define CHARSET_UNKNOWN 0

#define LANGUAGE_UNKNOWN 0
#define LANGUAGE_MULTIPLE 1
#define LANGUAGE_GROUP_UNKNOWN 0
#define PAST_LANGUAGE_GROUP_IDS 124 // see CVoc.cpp LanguageGroupList[]
#define CSOL_INDEX_FOR_LANGUAGE_UNKNOWN 2 // The first item after HEAD, TAIL.

// Now I am inserting the LGID ( range 0, 1 to 123, 127 ) << 17 into User.Value.
// Meanwhile I already have word valuation ( range 0, 255 ) << 24 in User.Value.
// That means I must keep count, and keep it clamped, in 17 lsb of User.Value.

#define MAX_WORD_VALUE          0x000000ff
#define MASK_WORD_VALUE_SHL24   0xff000000

#define LGID_SHL17_MASK         0x00fe0000
#define LGID_SHL17_MULTIPLE     0x00fe0000
#define LGID_SHL17_UNKNOWN      0x00000000

#define MASK_17BIT_WORD_COUNT   0x0001ffff
#define MAX_17BIT_WORD_COUNT    0x0001ffff
#define RELATIVE_WORD_FACTOR    128 // heuristic onepage::commoncount

// These enums tell the type of fruit in one Views structure;
// In another unrelated usage, several name types of threads.

// No longer needed: #define ENUM_INTERNAL 1
#define ENUM_ONEFORM 1
#define ENUM_ONEPAPER 2
#define ENUM_ADDCACHE 3
#define ENUM_ADDDIRECTORY 4
#define ENUM_ADDPAGE 5
#define ENUM_ADDSEARCH 6
#define ENUM_ADDFIND 7
#define ENUM_ADDPHRASES 8
#define ENUM_ONESURL 1


#define ENUM_INPUT_TYPE_UNSTATED    0
#define ENUM_INPUT_TYPE_OTHER       1
#define ENUM_INPUT_TYPE_HIDDEN      2
#define ENUM_INPUT_TYPE_TEXT        3
#define ENUM_INPUT_TYPE_SUBMIT      4
#define ENUM_INPUT_TYPE_CHECKBOX    5
#define ENUM_INPUT_TYPE_IMAGE       6
#define ENUM_INPUT_TYPE_RADIO       7
#define ENUM_INPUT_TYPE_BUTTON      8
#define ENUM_INPUT_TYPE_SEARCH      9
#define ENUM_INPUT_TYPE_PASSWORD    10
#define ENUM_INPUT_TYPE_FILE        11

#define FORM_REJECT_NO_ACTION_URL                   ( 1 << 0 )
#define FORM_REJECT_NO_METHOD                       ( 1 << 1 )
#define FORM_REJECT_NO_TEXT_INPUT                   ( 1 << 2 )
#define FORM_REJECT_MULTIPLE_TEXT_INPUT             ( 1 << 3 )
#define FORM_REJECT_NO_SUBMIT_IMAGE_BUTTON          ( 1 << 4 )
#define FORM_REJECT_MULTIPLE_SUBMIT_IMAGE_BUTTON    ( 1 << 5 )
#define FORM_REJECT_HAS_TEXTAREA                    ( 1 << 6 )
#define FORM_REJECT_PASSWORD_INPUT                  ( 1 << 7 )
#define FORM_REJECT_FILE_INPUT                      ( 1 << 8 )
#define FORM_REJECT_UNKNOWN_TYPE_INPUT              ( 1 << 9 )
#define FORM_REJECT_UNNAMED_TEXT_INPUT              ( 1 << 10 )
#define FORM_REJECT_JAVASCRIPT_ACTION_URL           ( 1 << 11 )
#define FORM_REJECT_BAD_ACTION_URL                  ( 1 << 12 )

#define UNDF 0xb7 // undefined on screen - garbage - replace with mid-dot
#define GAM 0xb7 // garbage as mark
#define GAS 0x20 // garbage as spaces
#define SPC 0x20 // whitespace
#define DQT 0x22 // Double quote
#define AMP 0x26 // ampersand
#define AM_ 0x01 // ampersand _ replacement marker
#define SQT 0x27 // Single quote
#define SLA 0x2f // slash
#define LAB 0x3c // left angle bracket
#define LA_ 0x02 // left angle bracket _ replacement marker
#define RAB 0x3e // right angle bracket
#define RA_ 0x03 // right angle bracket _ replacement marker
#define BSL 0x5c // backslash
#define SHY 0xAD // soft-hyphen as '-' ( or should I discard them? )
#define NBS 0x20 // Non-breaking space as ' '.

    // In order to rid some funny apostrophe chars in --don't--, etc.,
    // First I hdump'ed them:
    // utf: e2 80 99 = apostraphe
    // 11100010 10000000 10011001
    // 1110xxxx 10xxxxxx 10xxxxxx
    // ....0010...000000...011001
    // 00100000 00011001 = U_2019
    //
    // Per unicode:
    // Apostrophe includes U+0027 APOSTROPHE and U+2019 RIGHT SINGLE QUOTATION MARK...
    // Hyphens include U+002D HYPHEN-MINUS, U+2010 HYPHEN, possibly also U+058A...
    //
    // So I am plan to override these during all the charset conversions:
    //
    // these as 0x002d - hyphen
    //
    // U+2010 HYPHEN,
    // U+2011 NON-BREAKING HYPHEN,
    // U+2012 FIGURE DASH,
    // U+2013 EN DASH,
    // U+2014 EM DASH,
    // U+2015 HORIZONTAL BAR ( quotation dash ), and
    // U+2212 MINUS SIGN
    //
    // these as 0x0027 // Single quote
    //
    // U+2018 LEFT SINGLE QUOTATION MARK
    // U+2019 RIGHT SINGLE QUOTATION MARK
    // U+201A SINGLE LOW-9 QUOTATION MARK
    // U+201B SINGLE HIGH-REVERSED-9 QUOTATION MARK
    //
    // these as 0x0022 // Double quote
    //
    // U+201C LEFT DOUBLE QUOTATION MARK
    // U+201D RIGHT DOUBLE QUOTATION MARK
    // U+201E DOUBLE LOW-9 QUOTATION MARK
    // U+201F DOUBLE HIGH-REVERSED-9 QUOTATION MARK

    // Use these defines in various tables; Elsewhere, test values.

#define U_2010 0x002d
#define U_2011 0x002d
#define U_2012 0x002d
#define U_2013 0x002d
#define U_2014 0x002d
#define U_2015 0x002d
#define U_2212 0x002d

#define U_2018 0x0027
#define U_2019 0x0027
#define U_201A 0x0027
#define U_201B 0x0027

#define U_201C 0x0022
#define U_201D 0x0022
#define U_201E 0x0022
#define U_201F 0x0022


#define TOTAL_MALLOC_LIMIT              500000000

#define MAX_TAG_OR_ATTR_CHARS           20
#define MAX_SUSPENSE                    200
#define MAX_MARKUP_TEXT                 4000

#define MAX_TOKEN_TEXT                  4000
#define MAX_WRAP_COLUMN                 80

#define LOW_WRAP_COLUMN                 70
#define MAX_TITLE_OR_ANCHOR             200

#define MAX_PLAINTEXT_BLOCK             4000

#ifdef _WIN32_WCE
    // on Pocket PC
    #define BORDER_PELS         2
    #define MAX_VISIBLE_LINES   40
#else
    // on Desktop
    #define BORDER_PELS         4
    #define MAX_VISIBLE_LINES   100
#endif

#define MAX_CONSONANT_OR_VOWEL_RUN      9
#define MAX_LEGITIMATE_WORD_LENGTH      59


// SoIt is a structure defined in CAll.h, needed in other .h files.
// In fact, move all these defines from CSol.h to CAll.h:

#define TAIL 0
#define HEAD 1

#define NKEYSTART 7
#define MAX_RANK 9999999

#define CSOL_SCALAR 0
#define CSOL_MALLOC 1
#define CSOL_OBJECT 2

#define CSOL_FORWARD 0
#define CSOL_BACKWARD 1


// UCP, UI - structure ( generic to hold unsigned char ptr and unsigned int )

typedef struct _UCPUI
{
    unsigned char * ucp;
    unsigned int ui;
} UCPUI;

typedef struct _WCPUI
{
    wchar_t * wcp;
    unsigned int ui;
} WCPUI;


// SoIt - Sorted Item

typedef struct _SoIt
{
    wchar_t * szKeyExt; // fattest alignment item
    wchar_t   KeyStart [NKEYSTART]; // Any 2N-1 shorts
    wchar_t   Red;
    size_t    Lower;
    size_t    Higher;
    union {
        int     Value;
        void *  pVoid;
    } User;
} SoIt;


// Copies ( partial ) of Soit returned from GetSortedVector:
typedef struct _CoIt
{
    wchar_t * szKeyExt; // fattest alignment item
    wchar_t   KeyStart [NKEYSTART]; // Any 2N-1 shorts
    wchar_t   IsSentinel; // non-zero past last item
    union {
        int     Value;
        void *  pVoid;
    } User;
} CoIt;


// Link cross-referencing list structure
// In CLinAnchorHeads, UrlIndex is to the target or tail URL.
// In CLinAnchorTails, UrlIndex is to the referer, head, URL.

typedef struct _UiAti
{
    size_t UrlIndex;
    size_t AnchorTextIndex;
} UiAti;

typedef struct _LinkXRef
{
    size_t nMallocSlots;
    size_t nFilledSlots;
    UiAti Slot[1];
} LinkXRef;


// I moved this structure definition from cvoc.cpp into Call.h:

typedef struct tagname_tag {
    wchar_t enumTag;
    wchar_t * wcp;
} TAGNAME;

extern TAGNAME TagNames[];


// I moved this structure definition from cvoc.cpp into Call.h:

typedef struct attrname_tag {
    wchar_t enumAttr;
    wchar_t * wcp;
} ATTRNAME;

extern ATTRNAME AttrNames[];



// I am the include file that every .cpp includes. I include all .h files.

#include "CAsb.h" // ASCII string buffer
#include "CBud.h" // Base of sorted fruit
#include "CFio.h" // Local file in / out
#include "CHtm.h" // Parses html into text
#include "CIni.h" // Initiate operations
#include "CInt.h" // Integer lists
#include "CIdx.h" // Text Indexing offsets
#include "CFwd.h" // Find Words in texts
#include "CTop.h" // Holds top-level views
#include "CRev.h" // Holds Book of Revelation
#include "CUse.h" // Holds Help Usage text
#include "CPag.h" // CodePage conversions
#include "CSee.h" // Displays, navigation
#include "CTxt.h" // Parses text into sense
#include "CSol.h" // Holds one sorted list
#include "CLin.h" // Holds one sorted list
#include "CVoc.h" // Vocabulary, html names
#include "CWsb.h" // Wide string buffer
#include "CWww.h" // Web and cache access
#include "CPor.h" // Porter stemming
#include "Curl.h" // Search Engine Urls
#include "COme.h" // Omega Drawing

extern HWND g_hWnd; // main window
extern HWND g_hWndStatusDialog; // when visible, else NULL
extern HWND g_hWndSaveProgressBar; // when visible, else NULL

extern HDC g_hdcMain; // main device context, when have CS_OWNDC
extern size_t g_BitsPerPixel;
extern size_t g_ColorPlanes;
extern size_t g_PelsPerInch;
extern size_t g_ClientHeight;
extern size_t g_ClientWidth;
extern size_t g_LineXRef;
extern size_t g_LineXEnd;
extern size_t g_MaxExtentLogicalUnits;
extern size_t g_KwicTextWidthLimit;

extern HDC g_hdcCompat; // compatible device context
extern HBITMAP g_hbmCompat; // and compatible bitmap
extern HBITMAP g_CompatibleDcOriginalSystemBitmap;
extern HFONT g_CompatibleDcOriginalSystemFont;
extern HFONT g_CurrentDcSelectedFont;
extern HFONT g_LatestCreatedFont;

extern double g_dFontPointSize;
extern LOGFONT LogicalFont;
extern int g_MustMeasureLatestCreatedFont;

#ifdef _WIN32_WCE
#define SOME_GLYPHS 0x0200 // too big on PPC was pgm-fatal
#else // not _WIN32_WCE
#define SOME_GLYPHS 0x2f00 // included punct, stops at CJK
#endif // _WIN32_WCE

extern unsigned short g_SomeGlyphWidths[SOME_GLYPHS];
extern unsigned short g_WidestGlyphWidth;
extern char ToHex[]; // "0123456789ABCDEF"

#define N_RGB_CYCLE 4
extern int rgbGlyph[N_RGB_CYCLE];
extern int rgbErase[N_RGB_CYCLE];

#ifndef _WIN32_WCE
// It turns out that the FIND common dialog is only via MFC libs on PPC.
extern int g_fr_Flags_Down;
extern int g_fr_Flags_Case;
extern int g_fr_Flags_Word;
extern int g_fr_Accept_F3;
extern wchar_t g_szFindWhat[80];
#endif // not _WIN32_WCE


// Surf4me had a really elegant and precise Canonicalization work.
// Bring me his tables to know what are unreserved chars, etc...

    // These macros help me implement the RFC2396 parse,
    // inheiritance from base, canonicalization of URLs.

extern int cM [128];

#define LOWALPHA            0x0001
#define UPALPHA             0x0002
#define DIGIT               0x0004
#define RESERVED            0x0008
#define ESCAPE              0x0010
#define MARK                0x0020
#define HEX                 0x0040
#define SPACE               0x0080
#define DELIMS              0x0100
#define UNWISE              0x0200
#define BOUNDING            0x0400
#define EXCLUDE1            0x0800
#define EXCLUDE2            0x1000
#define EXCLUDE3            0x2000
#define EXCLUDE4            0x4000
#define BADFILE             0x8000

#define ALPHA               ( LOWALPHA|UPALPHA )
#define ALPHANUM            ( ALPHA|DIGIT )
#define UNRESERVED          ( ALPHANUM|MARK )
#define URIC_PART1          ( RESERVED|UNRESERVED )

#define lowalpha( p )       ( cM[( p )[0]] & LOWALPHA )
#define upalpha( p )        ( cM[( p )[0]] & UPALPHA )
#define digit( p )          ( cM[( p )[0]] & DIGIT )
#define reserved( p )       ( cM[( p )[0]] & RESERVED )
#define escaped( p )        ( ( cM[( p )[0]] & ESCAPE ) && ( cM[( p )[1]] & HEX ) && ( cM[( p )[2]] & HEX ) )
#define mark( p )           ( cM[( p )[0]] & MARK )
#define hex( p )            ( cM[( p )[0]] & HEX )
#define space( p )          ( cM[( p )[0]] & SPACE )
#define delims( p )         ( cM[( p )[0]] & DELIMS )
#define unwise( p )         ( cM[( p )[0]] & UNWISE )
#define uric( p )           ( ( cM[( p )[0]] & URIC_PART1 ) || escaped( p ) )
#define uric_part1( p )     ( cM[( p )[0]] & URIC_PART1 )
#define alpha( p )          ( cM[( p )[0]] & ALPHA )
#define alphanum( p )       ( cM[( p )[0]] & ALPHANUM )
#define unreserved( p )     ( cM[( p )[0]] & UNRESERVED )
#define bounding( p )       ( cM[( p )[0]] & BOUNDING )
#define exclude1( p )       ( cM[( p )[0]] & EXCLUDE1 )
#define exclude2( p )       ( cM[( p )[0]] & EXCLUDE2 )
#define exclude3( p )       ( cM[( p )[0]] & EXCLUDE3 )
#define exclude4( p )       ( cM[( p )[0]] & EXCLUDE4 )
#define badfile( p )        ( cM[( p )[0]] & BADFILE )

extern int g_FocusOffsetToRestoreWhenResize;
extern int g_rgbIndex;
extern int g_rgbGlyph;
extern int g_rgbErase;
extern int g_CurrentlyShowingSplashScreen;
extern int OmegaElapsedms;

extern size_t g_LineHeight;
extern size_t g_nLinesPageAdvances;
extern size_t g_nLinesPaintingStop;
extern RECT g_BaseRects [MAX_VISIBLE_LINES + 1];
extern RECT g_LineRects [MAX_VISIBLE_LINES + 1];

extern int g_SystemClockQuantaIn100ns;
extern int g_CHtmFoundAForm;


    // not all really volatile...

extern volatile int InDispatch;
extern volatile int g_BitBltFailed;
extern volatile int g_BitBltCount;

extern volatile int g_bMallocFailure;
extern wchar_t * g_szInternalError;
extern volatile DWORD g_dwLastError;
extern volatile int g_bStopAllThreads;
extern volatile int g_bStopInitThread;
extern volatile int g_bInitThreadRunning;
extern volatile int g_ReadyToAllowPaint;
extern volatile int NeedPaint;
extern volatile int g_JumpActivated;


extern volatile int g_FetchAllLinks;
extern volatile int g_NonTextLinks;
#define MIN_DILIGENCE 1
#define DEFAULT_DILIGENCE 3
#define MAX_DILIGENCE 100
extern volatile int g_QueryDiligence;
extern volatile int g_bCreateSubfolders;
extern volatile int g_bAddRecursively;
extern volatile int g_Kwic1_Sentc2_Parag3;
extern volatile int g_PhrasesPercent;
extern volatile int g_Stem;
extern volatile int g_nPasteCounter;
extern volatile int g_nPapersFetchedButNotViewed;
extern wchar_t * g_ClickTextToInitDialogs;

extern COnePaper * g_MainShallTopAddThisPaper;

extern volatile int g_bOmitSignage;

extern CBud * g_pFruitOfLastDeletedView;
extern size_t g_OffsetOfLastDeletedView;

extern FILE * SpewFile;

// I have gathered you all together here today,
// to determine the order of your constructors.
// ( Actually, what matters is the .cpp, not .h )

// Nothing like a critical section to solve a hairy problem.
// But CTop needs CSee, so move ALL class objects into here.

// I am certainly having lots of trouble with threads lately.
// For example, now a deadlock occurs when I click a link in
// the cache results while it is still running. I reason the
// deadlock requires TWO crix, so let' munge CTop and CSee!

// Web Study of deadlock issues shows:
// - Add volatile to all inter-thread globals.
// - Check that used no static local variables.
// - Never do SendMessage between threads.
// - Must link using the Multithreaded libraries.
// - Load balancing: Enter A then B; but never B then A.
// - Keep duration of holding the crix to a minimum.

extern volatile int SatCrixCounter;

extern CRITICAL_SECTION CAsbCriticalSection;
extern CRITICAL_SECTION CIntCriticalSection;
extern CRITICAL_SECTION CIdxCriticalSection;
extern CRITICAL_SECTION CPasCriticalSection; // New. For Paint and Scroll.
extern CRITICAL_SECTION CSatCriticalSection; // I defined Top and See as one crix
extern CRITICAL_SECTION CSolCriticalSection;
extern CRITICAL_SECTION CLinCriticalSection;
extern CRITICAL_SECTION CSpewCriticalSection;
extern CRITICAL_SECTION CWsbCriticalSection;
extern CRITICAL_SECTION CWwwCriticalSection;

extern COneCache OnlyCache;
extern COnePhrases OnlyPhrases;

extern CSol * pSolCacheToDo;
extern CSol CSolUserQuerys;
extern CSol CSolUserFetchs;
extern CSol CSolUserKwics;
extern CSol CSolUserFolders;

extern volatile int g_nCountOfCacheThreads;
extern volatile int g_nCountOfFolderThreads;
extern volatile int g_nCountOfQueryThreads;
extern volatile int g_bEverStartedAnyQuery;
extern volatile int g_nCountOfFetchThreads;
extern volatile int g_nCountOfKwicThreads;
extern volatile int g_nCountOfPhrasesThreads;

extern volatile int TotalMallocBytes;
extern volatile int gbMallocLimitExceeded;
extern volatile int gbMallocLimitToldUser;
extern volatile int TotalPageCount;
extern volatile int TotalLanguageCount;
extern volatile int TotalPhraseCount;
extern volatile int TotalWordCount;
extern volatile int TotalFormCount;

extern CSol CSolAllUrls;
extern CSol CSolAnchorText;
extern CLin CLinAnchorHeads;
extern CLin CLinAnchorTails;
extern CSol CSolFormUrls;
extern CSol CSolRedirects;
extern CSol CSolLanguageAliases;
extern CSol CSolCharsets;
// extern CSol CSolProperNames;
extern CSol CSolCommonWords;
extern CSol CSolAllWords;
extern CSol CSolAllFacts;
extern CSol CSolConsonantRunValues;
extern CSol CSolVowelRunValues;
extern CSol CSolNewVows;
extern CSol CSolNewCons;
extern CSol CSolHtmlTagNames;
extern CSol CSolHtmlAttrNames;
extern CSol CSolHtmlBcEntities;
extern CSol CSolHtmlLcEntities;
extern CSol CSolTopLevelDomains;
extern CSol CSolCountryDomains;
extern CSol CSolBinExtensions;
extern CSol CSolSearchUrls;
extern CSol CSolRejectUrls;
extern CSol CSol3CharLcDomains;
extern CSol CSol4CharLcDomains;

extern CWsb WsbClipboardAccumulator;
extern CWsb WsbTempPathToSaveNonText;


extern CFio Fio;
extern CIni Initiator; // one per application
extern CPag Pag;
extern CPor Por;
extern CSee See;
extern CWww Www;
extern CTop Top;
extern CUrl Url;

extern COme * g_pOmega;

// global routines where no Cxxx.h file:

// in WordsEx.cpp:
extern void ProgramError( wchar_t * szCaption );
extern void UserError( wchar_t * szDetail );
extern void AskForNonTextDirectory( );
extern void SetUpFindDialog( );

// in CAll.cpp:
extern void Routine( wchar_t * szNNN );

extern void AsbEnterCrisis( );
extern void WsbEnterCrisis( );
extern void IntEnterCrisis( );
extern void IdxEnterCrisis( );
extern void PasEnterCrisis( );
extern void SatEnterCrisis( );
extern void SolEnterCrisis( );
extern void LinEnterCrisis( );
extern void WwwEnterCrisis( );

extern void AsbLeaveCrisis( );
extern void WsbLeaveCrisis( );
extern void IntLeaveCrisis( );
extern void IdxLeaveCrisis( );
extern void PasLeaveCrisis( );
extern void SatLeaveCrisis( );
extern void SolLeaveCrisis( );
extern void LinLeaveCrisis( );
extern void WwwLeaveCrisis( );

extern void Say( wchar_t * );
extern void Spew( wchar_t * );
extern void Spewn( wchar_t * szMsg, int n );
extern void SpewTwo( wchar_t * szMsg, wchar_t * sz2 );
extern void SpewValue( wchar_t * szMsg, int n );
extern void TagValueSpew( wchar_t * szTag, int n, wchar_t * szMsg );

extern void ClickShowsDialogs( int which ); // in WordsEx.cpp
extern int WordLetterValuation( wchar_t * szWord );
extern void BuildConsonantVowelRuns( wchar_t * szWord, int CountInLanguage );
extern void AnyFinalDebugDumps( );

extern void * MyRealloc( int id, size_t oldsize, void *oldptr, size_t newsize );
extern void * MyMalloc( int id, size_t newsize );
extern void MyFree( int id, size_t oldsize, void *oldptr );

extern int isvowel( wchar_t wc );

extern unsigned int CrcHash( wchar_t * sz );

// in COme.cpp
extern void InitOmegaWork( );
extern void DoOmegaWork( );

