// This is file: CSol.h
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
// I first coded Red-Black-Sorted-List separately...
// http://home.earthlink.net/~glenn_scheper/rbsl.cpp
// This version uses wchar_t !
// This threadsafe version uses critical section and __try.

// SoIt is a structure defined in CAll.h, needed in other .h files.
// In fact, move all these defines from CSol.h to CAll.h:

// #define TAIL 0
// #define HEAD 1
//
// #define NKEYSTART 7
// #define MAX_RANK 9999999
//
// #define CSOL_SCALAR 0
// #define CSOL_MALLOC 1
// #define CSOL_OBJECT 2
//
// #define CSOL_FORWARD 0
// #define CSOL_BACKWARD 1

// SoIt - Sorted Item

// typedef struct _SoIt
// {
//     wchar_t * szKeyExt; // fattest alignment item last
//     wchar_t   KeyStart [NKEYSTART]; // Any 2N-1 shorts
//     wchar_t   Red;
//     size_t    Lower;
//     size_t    Higher;
//     union {
//         int     Value;
//         void *  pVoid;
//     } User;
// } SoIt;


wchar_t * CoItFullKey( CoIt * pCoIt ); // returns a malloc, user frees

class CSol
{
public:
    CSol( int UserType );
    void Reset( );
    ~CSol( );
    size_t    nList; // including tail ( #0 ) and head ( #1 )
    size_t    AddKey( wchar_t * InsertKey );
    size_t    AddUniqueRank( int Rank );
    size_t    Find( wchar_t * InsertKey );
    size_t    MayFind( wchar_t * InsertKey );
    size_t    MustFind( wchar_t * InsertKey );
    CoIt *    GetSortedVector( int ReverseOutputOrder );
    wchar_t * GetFullKey( size_t index ); // a malloc, user frees
    void *    GetUserpVoid( size_t index );
    void      SetUserpVoid( size_t index, void * UserpVoid );
    int       ClaimUserpVoid( size_t index, void * NewUserpVoid );
    size_t    GetUserValue( size_t index );
    void      SetUserValue( size_t index, int UserValue );
    void      IncrementUserValue( size_t index );
private:
    SoIt    * List;
    size_t    ListMallocCount;
    size_t    ListFilledCount;
    wchar_t * MallocTailKeyExtPtr;
    size_t    MallocTailKeyExtSize;
    int       m_UserType; // 0 = scalar, 1 = malloc/free, 2 = object/delete
    void      FreeMallocs( );
    int       StrcmpTrialValueInTailTo( size_t y );
    size_t    Rotate( size_t y );
    void      InvalidIndex( wchar_t * szCaption, size_t index );
};

