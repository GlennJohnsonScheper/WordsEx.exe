// This is file: CSee.h
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

typedef struct FittedLine {
    size_t CharIndex;
    size_t CharCount;
} FITTED_LINE;

class CSee
{
public:
    CSee( );
    void Reset( );
    ~CSee( );
    void HelperFontSizeChange( );
    void MakeFontSizeChange( );
    int CharacterizeCreatedFont( );
    void FigureVisibleLinesOnMainScreen( );
    void SetFirstOrRevisedTextColors( );
    void SaveBackingText_MakeLineList( wchar_t * pMallocBuf, int nMallocBuf, CIdx * pIdxTextBlocks );
    LRESULT OnWmKeydown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    void OnVkDelete( );
    void OnVkReturn( );
    void CopyClipboardMakeNewPaper( );
    void CopyCurrentViewToClipboard( );
    void CopyOffsetsToClipboard( wchar_t * pWhole, size_t nWhole, size_t Offset1, size_t Offset2, int append );
    LRESULT WmMouseHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    int HowManyGlyphsFit( wchar_t * pChars, int nChars, int nWidth );
    void SetSmoothScrollScanLineOffset( int ScanLineOffset );
    void GoFasterSmoothScrolling( );
    void GoSlowerSmoothScrolling( );
    int OptimizeResultingScrollRate( );
    void QuitSmoothScrolling( int Align );
    void PauseAnySmoothScrolling( );
    void ResumeAnySmoothScrolling( );
    void WmSmoothScrollTimerHandler( );
    void WmMonitorThreadsTimerHandler( );
    size_t GetOffsetofFocusLine( );
    void SetFocusLineToThisOffset( size_t Offset );
#ifndef _WIN32_WCE
    void OnFindNext( );
#endif // not _WIN32_WCE
    void SeeMightHaveChangedTheDisplay( );

    LRESULT WmPaintHandler( HWND hWnd, WPARAM wParam, LPARAM lParam );
    void UpdateVScrollbarParameters( );
    LRESULT OnWmVscroll( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    void IMightHaveChangedTheDisplay( );
    void InitOmegaTables( );


    size_t m_LineXRef;
    size_t m_LineXEnd;

    double m_ExactScanLinesPerSecond;
    size_t m_CurrentSmoothScrollingScanLine;
    size_t m_ActiveSmoothScrollingScanLinesPerAct;
    size_t m_ActiveSmoothScrollingMsPerAct;
    int m_MouseButtonDown_PauseScrolling;

    int m_MouseButtonDown_XPos;
    int m_MouseButtonDown_YPos;
    int m_MouseButtonUp_XPos;
    int m_MouseButtonUp_YPos;
    int m_LastMouseMoveXPos;
    int m_LastMouseMoveYPos;

    FITTED_LINE * m_pMalFittedLineList;
    // because of lots of N-1 sets, and testing for < 0, make signed:
    int m_nLenFittedLineFilledCount;
    int m_nMalFittedLineMallocCount;
    int m_nFittedLineFocusedAtopScreen;

    size_t m_nFruitStrlenBackingDisplay;
    size_t m_nWideTextBackingDisplay;
    wchar_t * m_szWideTextBackingDisplay;

    SCROLLINFO m_si;

    int rancid1;
    int rancid2;
    int screendiameter;

    CIdx * m_pIdxTextBlocks;

private:
};

extern CSee See;

