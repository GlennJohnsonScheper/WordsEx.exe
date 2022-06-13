// This is file: CPor.h
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
/* stemmer is a structure for a few local bits of data */
struct stemmer {
   wchar_t * b;    /* buffer for word to be stemmed */
   int k;          /* offset to the end of the string */
   int j;          /* a general offset into the string */
};

class CPor
{
public:
    CPor( );
    ~CPor( );
    void Stem( wchar_t * b, int k );
private:
    int cons( struct stemmer * z, int i );
    int m( struct stemmer * z );
    int vowelinstem( struct stemmer * z );
    int doublec( struct stemmer * z, int j );
    int cvc( struct stemmer * z, int i );
    int ends( struct stemmer * z, wchar_t * s );
    void setto( struct stemmer * z, wchar_t * s );
    void r( struct stemmer * z, wchar_t * s );
    void step1ab( struct stemmer * z );
    void step1c( struct stemmer * z );
    void step2( struct stemmer * z );
    void step3( struct stemmer * z );
    void step4( struct stemmer * z );
    void step5( struct stemmer * z );
    int stem( struct stemmer * z, wchar_t * b, int k );
    struct stemmer * m_z;
};


// Globals
extern CPor Por;

