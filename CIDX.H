// This is file: CIdx.h
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
typedef struct _Index {
    size_t nAtopText;
    size_t nPastText;
    size_t nSourceIndex;
    size_t nSourceOffset;
} INDEX;

class CIdx
{
public:
    CIdx( );
    ~CIdx( );
    void Reset( );
    int AddIdx( size_t Atop, size_t Past, size_t Index, size_t Offset );
    int GetIdx( size_t nLookup, size_t * Atop, size_t * Past, size_t * Index, size_t * Offset );
    int ScanIdxToFollow( size_t Lookup, size_t * pIndex, size_t * pOffset );
    int ScanIdxToTestRange( size_t Lookup, size_t * pScore, size_t * pOffset, size_t * pAtopText, size_t * pPastText );
    int ReturnIdxSlotRange( size_t Index, size_t * pScore, size_t * pAtopText, size_t * pPastText );
    void IncreaseOffsets( size_t SizeChange );
    size_t nSlots;
private:
    INDEX * Indices;
    size_t nMal;
    size_t nLen;
};



