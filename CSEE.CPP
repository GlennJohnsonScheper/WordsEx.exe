// This is file: CSee.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

/*
In CSee and CTop, EVERY routine has a return at end to assist fgrep of:
::
Crisis(
  return
*/

CSee::CSee( )
{
    #if DO_DEBUG_CALLS
        Routine( L"211" );
    #endif
    SatEnterCrisis( );
    m_LineXRef = 0;
    m_LineXEnd = 0;
    m_pMalFittedLineList = NULL;
    m_nMalFittedLineMallocCount = 0;
    m_nLenFittedLineFilledCount = 0;
    m_nFittedLineFocusedAtopScreen = 0;
    m_ExactScanLinesPerSecond = 0.0;
    m_CurrentSmoothScrollingScanLine = 0;
    m_ActiveSmoothScrollingScanLinesPerAct = 0;
    m_ActiveSmoothScrollingMsPerAct = 0;
    m_MouseButtonDown_PauseScrolling = 0;
    m_pIdxTextBlocks = NULL;
    SatLeaveCrisis( );
    return;
}

void CSee::Reset( )
{
    #if DO_DEBUG_CALLS
        Routine( L"212" );
    #endif
    SatEnterCrisis( );

    // And zero these correlated variables.
    // Anyone setting them should test ptr.
    // Or, of course, they will set it too.

    size_t oldsize = m_nMalFittedLineMallocCount * sizeof( FITTED_LINE );

    m_nMalFittedLineMallocCount = 0;
    m_nLenFittedLineFilledCount = 0;
    m_nFittedLineFocusedAtopScreen = 0;

    // Wait a minute... I reset often, cannot discard this:
    // m_pIdxTextBlocks = NULL;

    if( m_pMalFittedLineList != NULL )
    {
        // Like many other places, a thread safe approach
        // must NULL the pointer before trying to free it.
        // In fact, users should make a copy to test, use.
        // That won't do. Critical section should protect.

        {
            FITTED_LINE * Temp = m_pMalFittedLineList;
            m_pMalFittedLineList = NULL;        // NULL before free
            MyFree( 82, UNPREDICTABLE, Temp );
            Temp = NULL; // obligatory rigor after delete
        }
    }

    SatLeaveCrisis( );
    return;
}

CSee::~CSee( )
{
    #if DO_DEBUG_CALLS
        Routine( L"213" );
    #endif
    SatEnterCrisis( );
    Reset( );
    SatLeaveCrisis( );
    return;
}


LRESULT CSee::WmPaintHandler( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"221" );
    #endif

    // due to infinite loops here, try no more paints after program error.
    // But my infinite loop is due to not VALIDATING the screen!
    if( g_szInternalError != NULL )
    {
        #if DO_DEBUG_PAINT
            ; Spew( L"Paint 99" );
        #endif
        ValidateRect( g_hWnd, NULL );
        return 0; // Prior error; Leave before/without PasEnterCrisis.
    }

    PasEnterCrisis( );

    // Services WM_PAINT message


    // Aug 05 2007 I notice the CPU gets used 100% while doing nothing.
    // Analysis: Windows help says calling BeginPaint does ValidateRect.
    // Spewed calls, opening one big file and sitting, shows BeginPaint,
    // Paint 25 happens as often as Paint 1, and Invalidate 100 happens
    // as often too, so it was not a failure to revalidate during paint.
    // Go solve what retriggered the invalidate.

    #if DO_DEBUG_PAINT
        ; Spew( L"Paint 1" );
    #endif

    // When this lived in CSee, I got deadlocks wherein
    // WmPaintHandler could not Enter CSat Crix, and so
    // the message loop stopped, which kept some fellas
    // also living in CSee or CTop from being able to
    // do various SendMessage related system calls.

    // CYA - Funny these never BYA before moving here.
    // Wrong test. This just made first screen not paint.
    // So Painter was ready for nulls here...
    // Well, not really. But move to minimally affected areas.
    //
    // if( m_pMalFittedLineList == 0
    // || m_nLenFittedLineFilledCount == 0
    // || m_szWideTextBackingDisplay == 0
    // || m_nWideTextBackingDisplay == 0 )
    // {
    //     PasLeaveCrisis( );
    //     return 0;
    // }

    // Painter was NOT ready for 0xfeeefeee here:
    // Need a new arming bool, not change of addr.
    // lp->CharIndex

    // This is a bad place for interruptions,
    // so just ignore any system call errors!
    // I will set them into g_szInternalError.

    if( g_hdcCompat != NULL )
    {
        #if DO_DEBUG_PAINT
            ; Spew( L"Paint 2" );
        #endif
        // Perform all my painting operations on the compatible DC,
        // then BitBlt the image onto display, causing no flashing.
        {
            #if DO_DEBUG_PAINT
                ; Spew( L"Paint 3" );
            #endif

            // With BitBlt method, I'll preerase the whole area once.
            // In that case, I need not say ETO_OPAQUE on ExtTextOut.
            HBRUSH hbrushErase = CreateSolidBrush( g_rgbErase );
            if( hbrushErase == NULL )
            {
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 4" );
                #endif
                PasLeaveCrisis( );
                ProgramError( L"Createbrush" );
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 5" );
                #endif
                return 0; // failure
            }

            RECT rc;
            rc.left   = 0;
            rc.top    = 0;
            rc.right  = g_ClientWidth;
            rc.bottom = g_ClientHeight;

            if( ! FillRect( g_hdcCompat, & rc, hbrushErase ) )
            {
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 6" );
                #endif
                PasLeaveCrisis( );
                ProgramError( L"FillRect g_hdcCompatible" );
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 7" );
                #endif
                return 0; // failure
            }

            if( ! DeleteObject( hbrushErase ) )
            {
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 8" );
                #endif
                PasLeaveCrisis( );
                ProgramError( L"Delete brush" );
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 9" );
                #endif
                return 0; // failure
            }
            #if DO_DEBUG_PAINT
                ; Spew( L"Paint 10" );
            #endif
        }


        if( m_pMalFittedLineList != NULL
        && m_szWideTextBackingDisplay != NULL )
        {
            #if DO_DEBUG_PAINT
                ; Spew( L"Paint 11" );
            #endif
            // Loop to paint the visible lines of m_pMalFittedLineList.

            // When I added a speed stripe ( i.e., scrollbar ), it could move
            // m_nFittedLineFocusedAtopScreen past m_nLenFittedLineFilledCount.
            // So defend that loop-limit here.

            // A minimum loop-safe adjustment is to N-1,
            // but that left last line at top of screen.
            // So reduce by the visible lines, min zero.
            // These are unsigned, so think a bit on it.

            // empirically, g_nLinesPaintingStop lost some.
            // so test against larger g_nLinesPageAdvances.

            int limit = 0;
            if( m_nLenFittedLineFilledCount > ( int ) g_nLinesPageAdvances )
                limit = m_nLenFittedLineFilledCount - g_nLinesPageAdvances;
            if( limit < 0 )
                limit = 0;
            if( m_nFittedLineFocusedAtopScreen >= limit )
                m_nFittedLineFocusedAtopScreen = limit;
            {
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 12" );
                #endif
                // carefully bracket all FITTED_LINE blocks to aid debug.
                FITTED_LINE * lp = m_pMalFittedLineList + m_nFittedLineFocusedAtopScreen;
                FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;

                size_t i = 0;
                for( ;; )
                {
                    #if DO_DEBUG_PAINT
                        ; Spew( L"Paint 13" );
                    #endif
                    if( lp < m_pMalFittedLineList
                    ||  lp >= xp )
                    {
                        #if DO_DEBUG_PAINT
                            ; Spew( L"Paint 14" );
                        #endif
                        // Just past the end of text, erase any partial line.

                        if( ! ExtTextOut(
                            g_hdcCompat, // handle to my double-buffered DC
                            m_LineXRef, // x-coordinate of ref pt ( left of rectangle )
                            g_LineRects[i].top, // y-coordinate of ref pt ( top of rectangle )
                            0, // saves time? vs. ETO_OPAQUE | ETO_CLIPPED, // text-output options
                            NULL, // g_LineRects + i, // RECT, optional dimensions
                            L"", // character string
                            0, // number of characters
                            NULL // array of spacing values
                        ) )
                        {
                            #if DO_DEBUG_PAINT
                                ; Spew( L"Paint 15" );
                            #endif
                            PasLeaveCrisis( );
                            ProgramError( L"ExtTextOut 1a" );
                            #if DO_DEBUG_PAINT
                                ; Spew( L"Paint 16" );
                            #endif
                            return 0; // failure
                        }
                        #if DO_DEBUG_PAINT
                            ; Spew( L"Paint 17" );
                        #endif
                    }
                    else
                    {
                        #if DO_DEBUG_PAINT
                            ; Spew( L"Paint 18" );
                        #endif
                        // Normal case.

                        if( ! ExtTextOut(
                            g_hdcCompat, // handle to my double-buffered DC
                            m_LineXRef, // x-coordinate of ref pt ( left of rectangle )
                            g_LineRects[i].top, // y-coordinate of ref pt ( top of rectangle )
                            0, // saves time? vs. ETO_OPAQUE | ETO_CLIPPED, // text-output options
                            NULL, // g_LineRects + i, // RECT, optional dimensions
                            m_szWideTextBackingDisplay + lp->CharIndex, // character string
                            lp->CharCount, // number of characters
                            NULL // array of spacing values
                        ) )
                        {
                            #if DO_DEBUG_PAINT
                                ; Spew( L"Paint 19" );
                            #endif
                            PasLeaveCrisis( );
                            ProgramError( L"ExtTextOut 1b" );
                            #if DO_DEBUG_PAINT
                                ; Spew( L"Paint 20" );
                            #endif
                            return 0; // failure
                        }
                        #if DO_DEBUG_PAINT
                            ; Spew( L"Paint 21" );
                        #endif
                    }

                    if( ++i >= g_nLinesPaintingStop )
                       break;

                    // This BREAK now leaves final 1/2 line unerased at end.
                    // Instead, I will increment here, but IF at ExtTextout.
                    // if( ++lp >= xp )
                    //    break;

                    lp ++;
                    #if DO_DEBUG_PAINT
                        ; Spew( L"Paint 22" );
                    #endif
                }
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 23" );
                #endif
            }
            #if DO_DEBUG_PAINT
                ; Spew( L"Paint 24" );
            #endif
        }

        if( g_CurrentlyShowingSplashScreen
        && g_pOmega != NULL ) // CYA
            g_pOmega->DrawOmega( );

        {
            #if DO_DEBUG_PAINT
                ; Spew( L"Paint 25" );
            #endif
            // BitBlt it! ( Whatever BLIT means. )
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint( g_hWnd, & ps );
            if( hdc == NULL )
            {
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 26" );
                #endif
                PasLeaveCrisis( );
                ProgramError( L"BeginPaint 1" );
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 27" );
                #endif
                return 0; // failure
            }
            else
            {
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 28" );
                #endif

                g_BitBltCount ++;

                if ( ! BitBlt( hdc, 0, 0, g_ClientWidth, g_ClientHeight,
                    g_hdcCompat, 0, 0, SRCCOPY )
                    // --ok--... || 1 // insert just to see error provision.
                )
                {
                    #if DO_DEBUG_PAINT
                        ; Spew( L"Paint 29" );
                    #endif
                    PasLeaveCrisis( );

                    #if DO_DEBUG_PAINT
                        ; Spew( L"Paint 30" );
                    #endif

                    // I saw this program error on XP laptop when maximized.
                    // Reduce severity of error and use a fallback strategy.
                    // Oh-oh. There is no more non-bitblt method.
                    // Use this bool to gate off further warning.

                    #if DO_DEBUG_BITBLT
                    {
                        wchar_t wk[60];
                        wsprintf( wk, L"BitBlt failed. Counter=%d.", g_BitBltCount );
                        ; Spew( L"Paint 30" );
                    }
                    #endif

                    if( ! g_BitBltFailed )
                    {
                        g_BitBltFailed = 1;

                        size_t dwLastError = GetLastError( );

                        HMODULE hModule = NULL; // default to system source
                        DWORD dwBufferLength = 0;
                        DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                            FORMAT_MESSAGE_IGNORE_INSERTS |
                            FORMAT_MESSAGE_FROM_SYSTEM;

                        wchar_t * MessageBuffer = NULL; // receives the LocalAlloc.
                        wchar_t wkle[80];
                        wchar_t * ShowWhich = NULL;

                        dwBufferLength = FormatMessage(
                            dwFormatFlags,
                            hModule,
                            dwLastError,
                            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
                            ( wchar_t * ) & MessageBuffer, // type was mis-cast on purpose
                            0,
                            NULL );

                        ShowWhich = MessageBuffer;
                        // Handle the case that FormatMessage failed.
                        if( dwBufferLength == 0 )
                        {
                            wsprintf( wkle, L"Error No. %d", dwLastError );
                            ShowWhich = wkle;
                        }

                        wchar_t wk[380];
                        wsprintf( wk,
                            L"Call to system function BitBlt which copies to display failed.\r\n"
                            L"WordsEx will continue without further mention of BitBlt error.\r\n"
                            L"Window size and display settings can improve this situation.\r\n"
                            L"Error is: %.100s\r\n", ShowWhich
                         );
                         Say( wk );

                        if( MessageBuffer != NULL )
                            LocalFree( MessageBuffer );
                        MessageBuffer = NULL;
                    }

                    return 0; // failure
                }

                if( ! EndPaint( g_hWnd, & ps ) )
                {
                    #if DO_DEBUG_PAINT
                        ; Spew( L"Paint 31" );
                    #endif
                    PasLeaveCrisis( );
                    ProgramError( L"EndPaint 1" );
                    #if DO_DEBUG_PAINT
                        ; Spew( L"Paint 32" );
                    #endif
                    return 0; // failure
                }
                #if DO_DEBUG_PAINT
                    ; Spew( L"Paint 33" );
                #endif
            }
            #if DO_DEBUG_PAINT
                ; Spew( L"Paint 34" );
            #endif
        }
        #if DO_DEBUG_PAINT
            ; Spew( L"Paint 65" );
        #endif
    }

    // There is NO more else clause. It's BitBlt or Bust!


    // ... Any returns in here?
    PasLeaveCrisis( );
    #if DO_DEBUG_PAINT
        ; Spew( L"Paint 69" );
    #endif
    return 0;
}

void CSee::UpdateVScrollbarParameters( )
{
    #if DO_DEBUG_CALLS
        Routine( L"218" );
    #endif
    PasEnterCrisis( );
    // Call me whenever window size or text content/position changes.

    // As with WmPaintHandler, when this lived in CSee,
    // I got deadlocks ( really easy to find on VC++ 8 )
    // wherein the SetScrollInfo call would not return.

    // Much from Windows Help...
    // New applications should use the SetScrollInfo function.
    // typedef struct tagSCROLLINFO {
    //     UINT cbSize;
    //     UINT fMask;
    //     int  nMin;
    //     int  nMax;
    //     UINT nPage;
    //     int  nPos;
    //     int  nTrackPos;
    // }   SCROLLINFO, *LPSCROLLINFO;
    // typedef SCROLLINFO CONST *LPCSCROLLINFO;

    // cbSize
    // Specifies the size, in bytes, of this structure.

    // fMask
    // Specifies the scroll bar parameters to set or retrieve. This member can be a combination of the following values: Value Meaning
    // SIF_ALL Combination of SIF_PAGE, SIF_POS, SIF_RANGE, and SIF_TRACKPOS.
    // SIF_DISABLENOSCROLL This value is used only when setting a scroll bar's parameters. If the scroll bar's new parameters make the scroll bar unnecessary, disable the scroll bar instead of removing it.
    // SIF_PAGE The nPage member contains the page size for a proportional scroll bar.
    // SIF_POS The nPos member contains the scroll box position, which is not updated while the user drags the scroll box.
    // SIF_RANGE The nMin and nMax members contain the minimum and maximum values for the scrolling range.
    // SIF_TRACKPOS The nTrackPos member contains the current position of the scroll box while the user is dragging it.

    // nMin
    // Specifies the minimum scrolling position.

    // nMax
    // Specifies the maximum scrolling position.

    // nPage
    // Specifies the page size. A scroll bar uses this value to determine the appropriate size of the proportional scroll box.

    // nPos
    // Specifies the position of the scroll box.

    // nTrackPos
    // Specifies the immediate position of a scroll box that the user is dragging. An application can retrieve this value while processing the SB_THUMBTRACK request code. An application cannot set the immediate scroll position; the SetScrollInfo function ignores this member.

    // ---

    // If an application scrolls the content of the window,
    // it must also reset the position of the scroll box by
    // using the SetScrollPos function.

    // Set the vertical scrolling range and page size
    m_si.cbSize = sizeof( m_si );
    m_si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
    m_si.nMin   = 0;
    m_si.nMax   = m_nLenFittedLineFilledCount - 1;
    m_si.nPage  = g_nLinesPageAdvances;
    m_si.nPos   = m_nFittedLineFocusedAtopScreen;

    // Rectify high limit and thumb pos within current bounds.

    if( m_si.nMax > m_nLenFittedLineFilledCount - 1 )
        m_si.nMax = m_nLenFittedLineFilledCount - 1;
    if( m_si.nMax < 0 )
        m_si.nMax = 0;
    if( m_si.nPos > m_nLenFittedLineFilledCount - 1 )
        m_si.nPos = m_nLenFittedLineFilledCount - 1;
    if( m_si.nPos < 0 )
        m_si.nPos = 0;

    SetScrollInfo( g_hWnd, SB_VERT, & m_si, TRUE );

    PasLeaveCrisis( );
    return;
}


LRESULT CSee::OnWmVscroll( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"222" );
    #endif
    PasEnterCrisis( );
    // This is as cloned from a Windows Scrollbar Help.

    // Get all the vertical scroll bar information
    m_si.cbSize = sizeof ( m_si );
    m_si.fMask  = SIF_ALL | SIF_DISABLENOSCROLL;
    GetScrollInfo ( hWnd, SB_VERT, & m_si );

    // Save the position for comparison later on
    int yPos = m_si.nPos;

    switch ( LOWORD ( wParam ) )
    {
    // user clicked the HOME keyboard key
    case SB_TOP:
         m_si.nPos = m_si.nMin;
         break;

    // user clicked the END keyboard key
    case SB_BOTTOM:
         m_si.nPos = m_si.nMax;
         break;

    // user clicked the top arrow
    case SB_LINEUP:
         m_si.nPos -= 1;
         break;

    // user clicked the bottom arrow
    case SB_LINEDOWN:
         m_si.nPos += 1;
         break;

    // user clicked the shaft above the scroll box
    case SB_PAGEUP:
         m_si.nPos -= m_si.nPage;
         break;

    // user clicked the shaft below the scroll box
    case SB_PAGEDOWN:
         m_si.nPos += m_si.nPage;
         break;

    // user dragged the scroll box
    case SB_THUMBTRACK:
         m_si.nPos = m_si.nTrackPos;
         break;

    default:
         break;
    }

    // Set the position and then retrieve it.  Due to adjustments
    //   by Windows it may not be the same as the value set.
    m_si.fMask = SIF_POS | SIF_DISABLENOSCROLL;

    // Rectify high limit and thumb pos within current bounds.

    if( m_si.nMax > m_nLenFittedLineFilledCount - 1 )
        m_si.nMax = m_nLenFittedLineFilledCount - 1;
    if( m_si.nMax < 0 )
        m_si.nMax = 0;
    if( m_si.nPos > m_nLenFittedLineFilledCount - 1 )
        m_si.nPos = m_nLenFittedLineFilledCount - 1;
    if( m_si.nPos < 0 )
        m_si.nPos = 0;

    SetScrollInfo( hWnd, SB_VERT, & m_si, TRUE );

    GetScrollInfo ( hWnd, SB_VERT, & m_si );

    // If the position has changed, scroll window and update it
    if( m_si.nPos != yPos )
    {
        m_nFittedLineFocusedAtopScreen = m_si.nPos;

        int limit = 0;
        if( m_nLenFittedLineFilledCount > ( int ) g_nLinesPageAdvances )
            limit = m_nLenFittedLineFilledCount - g_nLinesPageAdvances;
        if( limit < 0 )
            limit = 0;
        if( m_nFittedLineFocusedAtopScreen > limit )
            m_nFittedLineFocusedAtopScreen = limit;
        IMightHaveChangedTheDisplay( ); // Scrollbar changed
    }
    PasLeaveCrisis( );
    return 0;
}

void CSee::SetFirstOrRevisedTextColors( )
{
    #if DO_DEBUG_CALLS
        Routine( L"357" );
    #endif
    if( g_hdcCompat == NULL )
    {
        ProgramError( L"SelectColor: g_hdcCompat == NULL" );
        return; // failure
    }

    // Set background and text colors on the compatible DC.
    if( SetTextColor( g_hdcCompat, g_rgbGlyph ) == CLR_INVALID )
    {
        ProgramError( L"SetTextColor" );
        return; // failure
    }
    if( SetBkColor( g_hdcCompat, g_rgbErase ) == CLR_INVALID )
    {
        ProgramError( L"SetBkColor" );
        return; // failure
    }
}


void CSee::SaveBackingText_MakeLineList( wchar_t * pMallocBuf, int nMallocBuf, CIdx * pIdxTextBlocks )
{
    #if DO_DEBUG_FITER
        Routine( L"451" );
    #endif
    SatEnterCrisis( );

    // Who knocks here?
    // ID_HELP_FONTPLUS
    // ID_HELP_FONTMINUS
    // WmSizeHandler
    // CTop ShowCurrentTopItemViewOnScreen
    // CTop ShowSplashScreen
    // CTop RewrapCurrentTopItemViewOnScreen


    // If I received ( NULL, -1 ), Re-do same text's line list ( resized ).

    // Do not paint a NULL line list, in case of a failure.
    // Empty out the trash, already under critical section:

    Reset( ); // frees, nulls: m_pMalFittedLineList and three other counts

    // This will wrap the wide-text buffer with wcslen:
    // -- wchar_t m_szWideTextBackingDisplay;
    // -- size_t m_nWideTextBackingDisplay;

    // to create a display screen line list,
    // in which WM_PAINT handler reads text:
    // -- FITTED_LINE * m_pMalFittedLineList;
    // -- size_t m_nMalFittedLineMallocCount;
    // -- size_t m_nLenFittedLineFilledCount;

    // Input should contain \r\n\r\n at end of paragraphs.
    // Input may also contain \r\n to force an end of line.
    // And I must make additional soft wraps to fit screen.

    // When I add Right-to-Left display of Hebrew and Arabic,
    // I will need to do a soft-wrap every time the char set
    // changes in or out of the range of RTL characters too.

    // When I create a line list describing the entire text,
    // I will point to each start of text in the TextBuffer.

    // The line list I create is for all the text, exceeding
    // an often small subset of it that is visible on screen.

    // For each line, the line list has an offset and count;
    // Adding the char count in line should arrive atop the
    // next line, or at a newline, or possibly a final null.

    // With the new feature to hide or show signage, this loop
    // may be guided by the CBud's pIdxTextBlocks, to only show
    // the portions of text listed therein; Else, show all text.

    // In case I received ( NULL, -1 ), Re-do line list ( resized ).
    if( pMallocBuf == NULL
    &&  nMallocBuf == -1 // special signal from the WM SIZE handler
    &&  pIdxTextBlocks == NULL )
    {
        #if DO_DEBUG_FITER
            Routine( L"452" );
        #endif
        // Keep existing malloc for m_szWideTextBackingDisplay.
        // Keep existing size for m_nWideTextBackingDisplay.
        // Keep existing pointer for m_pIdxTextBlocks.
    }
    else
    {
        #if DO_DEBUG_FITER
            Routine( L"453" );
        #endif
        // Accept a new malloc.

        if( m_szWideTextBackingDisplay != NULL )
        {
            #if DO_DEBUG_FITER
                Routine( L"454" );
            #endif
            wchar_t * ptemp = m_szWideTextBackingDisplay;
            m_szWideTextBackingDisplay = NULL;
            m_nWideTextBackingDisplay = 0;
            MyFree( 449, 0, ptemp ); // inappropriate to pass size, not nWide*
        }

        // The caller allocated this for me.
        // I consume malloc. I receive this;
        // I will free it on some next call.

        m_szWideTextBackingDisplay = pMallocBuf;
        m_nWideTextBackingDisplay = nMallocBuf; // misnomer. Actually, wcslen.
        m_pIdxTextBlocks = pIdxTextBlocks;
    }

    // Because of many error exits herein; Not I, but my
    // callers shall call IMightHaveChangedTheDisplay( );

    if( m_szWideTextBackingDisplay == NULL
    ||  m_nWideTextBackingDisplay == 0 )
    {
        #if DO_DEBUG_FITER
            Routine( L"455" );
        #endif
        m_szWideTextBackingDisplay = NULL; // keep both values in sync
        m_nWideTextBackingDisplay = 0; // because free double-checks me

        // Not an error, an eventual probable condition.
        // The reset( ) above is unspoiled.
        SatLeaveCrisis( );
        return; // trivial success
    }

    #if DO_DEBUG_FITER
        Routine( L"456" );
    #endif

    // Whenever I port this, I reencounter this problem, so remember:
    // Window Style CS_OWNDC means what I set in the DC will persist.
    // If lines are not wrapping exactly at screen boundary on Win32,
    // it is because you forgot to add CS_OWNDC in MyRegisterClass:
    // i.e., wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    // Well, it could also be for not selecting the font on some DC.

    // No more, no more CS_OWNDC!
    // It's BitBlt all the way now.

    if ( g_hdcCompat == NULL )
    {
        ProgramError( L"g_hdcCompat == NULL 2" );
        SatLeaveCrisis( );
        return; // failure
    }

    // I will be creating this output array of:
    // typedef struct FittedLine {
    //     wsize_t CharIndex;
    //     wsize_t CharCount;
    // } FITTED_LINE;
    //
    // Reset( ) above established:
    // m_pMalFittedLineList = NULL;
    // m_nMalFittedLineMallocCount = 0;
    // m_nLenFittedLineFilledCount = 0;
    // m_nFittedLineFocusedAtopScreen = 0;

    // Destin to build:
    // -- m_pMalFittedLineList = NULL, or NewMallocPtr;
    // -- m_nMalFittedLineMallocCount = NewMallocCount;
    //
    // Source to scan:
    // -- m_szWideTextBackingDisplay, from = pMallocBuf;
    // -- m_nWideTextBackingDisplay, from = nMallocBuf;

    // m_NewFittedLineVector = 1; // Arm so CPas will also transfer these

    #if DO_DEBUG_FITER
        Routine( L"457" );
    #endif

    #if DO_DEBUG_FITER
        ; SpewValue( L"MaxExtentLU", g_MaxExtentLogicalUnits );
    #endif

    // How do I decide whether to show or omit signage?
    // First, there is a toggling mode, g_bOmitSignage.
    // Second, the current view's CBud must be a paper
    // having a non-null object CIdx * pIdxTextBlocks;
    // Actually, all CBud have member, and as non-null.

    // So I'll test whether pIdxTextBlocks->nSlots > 0.
    // Whoever puts one CBud to view can copy that ptr
    // to a global; It will be NULL for splash screen.

    // No. There are pages with no worthy blocks in them.
    // Test if the fruit type is a COnePaper. Then, if it
    // turns out nSlots == 0, for-loop has nothing to do.

    // I don't see the pFruit of currentview, so CTop can
    // pass me a null pointer for all that are not Papers.

    if( g_bOmitSignage
    && m_pIdxTextBlocks != NULL )
    {
        // This new version of outer loop will only show
        // such portions of text as are in listed blocks.

        // The idea I had prepared for, to omit annotations
        // in this terse view, so pOnePaper->pWsbAnnotation
        // was created; but annotation is still in the CWsb
        // of all text; That idea needs no more work, for
        // the Idx does not point into any annotation block.

        // New very outer loop iterates m_pIdxTextBlocks
        int IdxIndex = 0;
        for( ;; )
        {
            size_t Score = 0;
            size_t AtopBlock = 0;
            size_t PastBlock = 0;
            if( ! m_pIdxTextBlocks->ReturnIdxSlotRange( IdxIndex, & Score, & AtopBlock, & PastBlock ) )
                break;

            if( IdxIndex > 0 )
            {
                // Between each of original outer loops, below,
                // I need to add a blank line to the line list.
                // I cloned the following, except set count = 0.
                // And for current text offset, I use AtopBlock.

                // Add this line to line list.

                if( m_nLenFittedLineFilledCount == m_nMalFittedLineMallocCount )
                {
                    // There is no more room for 1 new line. Make room.
                    size_t nNeed = m_nLenFittedLineFilledCount + 100 + ( m_nLenFittedLineFilledCount / 8 ); // grow liberally 125 %
                    void * pvm = MyRealloc( 9993, m_nMalFittedLineMallocCount * sizeof( INDEX ), m_pMalFittedLineList, nNeed * sizeof( INDEX ) );
                    if( pvm == NULL )
                    {
                        // do not change original m_pMalFittedLineList, m_nMalFittedLineMallocCount
                        // Worse, reset everything to known empty condition.
                        // Failure cleanup.
                        Reset( ); // Because I may have started filling line list.
                        SatLeaveCrisis( );
                        return; // failure - already messaged
                    }
                    m_pMalFittedLineList = ( FITTED_LINE * ) pvm;
                    m_nMalFittedLineMallocCount = nNeed;
                }

                m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharIndex = AtopBlock;
                m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharCount = 0;
                m_nLenFittedLineFilledCount ++;
            }


            // Crazy stupid idea: Infinite loop if no increment!
            IdxIndex ++;

            // It's been a long time since I've thought on these things.
            // Add sanity checks.

            if( AtopBlock >= PastBlock )
            {
                // Failure cleanup.
                Reset( ); // Because I may have started filling line list.
                SatLeaveCrisis( );
                ProgramError( L"AtopBlock >= PastBlock" );
                return; // failure
            }
            if( PastBlock > m_nWideTextBackingDisplay )
            {
                // Failure cleanup.
                Reset( ); // Because I may have started filling line list.
                SatLeaveCrisis( );
                ProgramError( L"PastBlock > m_nWideTextBackingDisplay" );
                return; // failure
            }

            // outer loop does line after line -- HERE, WITHIN ONE BLOCK.

            // I prefer pointers, but indirection is safer:
            size_t scan = AtopBlock;
            size_t stop = PastBlock;
            for( ;; )
            {
                // Inner loop 1 looks ahead for a newline to force a hard break.
                // Inner loop sums unkerned USASCII widths for alternate ending.
                // If I estimate low, who cares? If over, GetTextExtent catches.
                // Inner loop only cares about characters from scan to stop - 1.
                // Any characters below scan were done by the prior loop if any.
                // There should be a NULL exactly at [stop] in my CWsb thingies.
                // N.B. however, if nLen == 0, then possibly also szBuf == NULL;

                size_t StartingIndexOfCurrentLine = scan;
                size_t ProposedIndexPastEndOfLine = 0;
                size_t IndexOfMostRecentSpace = 0;
                size_t SumOfWidths = 0;

                #if DO_DEBUG_FITER
                    ; SpewValue( L"scan", scan );
                #endif

                for( ;; )
                {
                    wchar_t wc = m_szWideTextBackingDisplay[ scan ];

                    // severe verbosity
                    // #if DO_DEBUG_FITER
                    //     ; SpewValue( L"wc", wc );
                    // #endif

                    // CategorizeAndConvertInputBytesToWideWithBinaryRejection now protects my assumption:
                    // No NULL before end, No controls except CR LF.

                    if( wc < ' ' ) // Anything lower than space is newline, NULL.
                    {
                        // My japanese sample page gets stuck
                        // in this loop here, doing null char!
                        // I have fixed advance to ask wc < ' '.
                        ProposedIndexPastEndOfLine = scan;
                        break;
                    }

                    if( wc == ' ' )
                        IndexOfMostRecentSpace = scan;

                    if( wc < SOME_GLYPHS )
                        SumOfWidths += g_SomeGlyphWidths[ wc ];
                    else
                        SumOfWidths += g_WidestGlyphWidth;

                    if( SumOfWidths > g_MaxExtentLogicalUnits )
                    {
                        if( IndexOfMostRecentSpace > StartingIndexOfCurrentLine )
                            ProposedIndexPastEndOfLine = IndexOfMostRecentSpace;
                        else
                            ProposedIndexPastEndOfLine = scan;
                        break;
                    }

                    scan ++;
                } // End of Inner loop 1.

                #if DO_DEBUG_FITER
                    Routine( L"458" );
                #endif

                if( StartingIndexOfCurrentLine == ProposedIndexPastEndOfLine )
                {
                    #if DO_DEBUG_FITER
                        Routine( L"460" );
                    #endif
                    // Empty line. No adjustments needed.
                }
                else
                {
                    #if DO_DEBUG_FITER
                        Routine( L"461" );
                    #endif
                    // Start of Inner ( non- )"loop" 2a.
                    // Attempt adjustments of end of line.
                    // Actually, this 2a will not be a loop.
                    // One measurement, zero or one cuts, and it's done.

                    wchar_t ReplaceThisCharAfterEOS =
                        m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine ];
                    m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine ] = NULL;

                    int HowManyProposed = ( int ) ( ProposedIndexPastEndOfLine - StartingIndexOfCurrentLine );
                    int HowManyFit = 0;
                    SIZE StringDim; // required by windows

                    if( ! GetTextExtentExPoint(
                        g_hdcCompat,
                        m_szWideTextBackingDisplay + StartingIndexOfCurrentLine,
                        HowManyProposed,
                        g_MaxExtentLogicalUnits,
                        & HowManyFit,
                        NULL,
                        & StringDim
                    ) )
                    {
                        // Failure cleanup.
                        Reset( ); // Because I may have started filling line list.
                        SatLeaveCrisis( );
                        ProgramError( L"GetTextExtentExPoint 2" );
                        return; // failure
                    }

                    m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine ] =
                        ReplaceThisCharAfterEOS;

                    if( HowManyProposed <= HowManyFit )
                    {
                        #if DO_DEBUG_FITER
                            Routine( L"462" );
                        #endif
                        // Line fits. No adjustments needed.
                    }
                    else
                    {
                        #if DO_DEBUG_FITER
                            Routine( L"463" );
                        #endif

                        // A loop to look backwards for any last space.
                        scan = StartingIndexOfCurrentLine + HowManyFit;
                        for( ;; )
                        {
                            // Start of Inner loop 2b.
                            // Suppose 5 fit, [0] to [4], and [5]==' '; Done.

                            if( m_szWideTextBackingDisplay[ scan ] == ' ' )
                                break;

                            if( scan == StartingIndexOfCurrentLine )
                                break;

                            scan --;

                        } // End of Inner loop 2b.

                        if( scan > StartingIndexOfCurrentLine )
                        {
                            #if DO_DEBUG_FITER
                                Routine( L"464" );
                            #endif
                            // this is a good new wrapping point.
                            ProposedIndexPastEndOfLine = scan;
                        }
                        else
                        {
                            #if DO_DEBUG_FITER
                                Routine( L"465" );
                            #endif
                            // This is one big word ( often URL )
                            // that we must wrap in its middle.
                            ProposedIndexPastEndOfLine = StartingIndexOfCurrentLine + HowManyFit;

                            // Make sure at least 1 char was fitted. Just to end loop.
                            if( HowManyFit == 0 )
                                ProposedIndexPastEndOfLine ++;
                        }
                    }

                } // End of Inner ( non- )"loop" 2a.

                #if DO_DEBUG_FITER
                    Routine( L"459" );
                #endif

                // I have determined ProposedIndexPastEndOfLine.

                // Whatever happens, even empty line,
                // I determined one fitted line runs
                // from StartingIndexOfCurrentLine
                // to ProposedIndexPastEndOfLine - 1.

                #if DO_DEBUG_FITER
                    ; SpewValue( L"bo", StartingIndexOfCurrentLine );
                    ; SpewValue( L"eo", ProposedIndexPastEndOfLine - 1 );
                #endif

                // Add this line to line list.

                if( m_nLenFittedLineFilledCount == m_nMalFittedLineMallocCount )
                {
                    // There is no more room for 1 new line. Make room.
                    size_t nNeed = m_nLenFittedLineFilledCount + 100 + ( m_nLenFittedLineFilledCount / 8 ); // grow liberally 125 %
                    void * pvm = MyRealloc( 9993, m_nMalFittedLineMallocCount * sizeof( INDEX ), m_pMalFittedLineList, nNeed * sizeof( INDEX ) );
                    if( pvm == NULL )
                    {
                        // do not change original m_pMalFittedLineList, m_nMalFittedLineMallocCount
                        // Worse, reset everything to known empty condition.
                        // Failure cleanup.
                        Reset( ); // Because I may have started filling line list.
                        SatLeaveCrisis( );
                        return; // failure - already messaged
                    }
                    m_pMalFittedLineList = ( FITTED_LINE * ) pvm;
                    m_nMalFittedLineMallocCount = nNeed;
                }

                m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharIndex = StartingIndexOfCurrentLine;
                m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharCount = ( size_t ) ( ProposedIndexPastEndOfLine - StartingIndexOfCurrentLine );
                m_nLenFittedLineFilledCount ++;

                // No need to affect m_nFittedLineFocusedAtopScreen since size grew.

                #if DO_DEBUG_FITER
                    Routine( L"466" );
                #endif

                // Get ready to scan the next line.
                // Sort out various possible newlines to skip over.
                // What happened?
                //
                // The character at [ProposedIndexPastEndOfLine] was not shown.
                //
                // If that character is part of a newline ( CR, or LF, or CR LF )
                // then continue past that whole ONE newline, to whatever next.
                //
                // If that character is NULL, it is end of string. Stop loop.
                //
                // If that character is space, I may either keep it or omit it.
                //
                // If that character is printable, it will start the next line.
                //
                // If I keep space, it distinguishes from case of mid-word wrap,
                // and it has been my habit to start wrapped lines with a space.
                //
                // If I omit space, it prevents moving one column on empty line.
                // That is, I would need to check next is not a newline, etc.

                {
                    // oh-oh. I see a fetch of +1 before checking if is in buffer.

                    wchar_t wc1 = 0; // leave null if hit stop
                    if( ProposedIndexPastEndOfLine < stop )
                        wc1 = m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine + 0 ];

                    wchar_t wc2 = 0; // leave null if hit stop
                    if( ProposedIndexPastEndOfLine + 1 < stop )
                        wc2 = m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine + 1 ];

                    if( wc1 == L'\r' && wc2 == L'\n' )
                    {
                        // Exact CR LF combination
                        // Setup next loop to scart linescan here:
                        scan = ProposedIndexPastEndOfLine + 2;
                        #if DO_DEBUG_FITER
                            Routine( L"467" );
                        #endif
                    }
                    else if( wc1 == L'\r' || wc1 == L'\n' )
                    {
                        // Just one CR or one LF. ( Not planned )
                        // Setup next loop to scart linescan here:
                        scan = ProposedIndexPastEndOfLine + 1;
                        #if DO_DEBUG_FITER
                            Routine( L"468" );
                        #endif
                    }
                    else if( wc1 < ' ' )
                    {
                        // Comment atop loop said:
                        // Anything lower than space is newline, NULL.

                        // CategorizeAndConvertInputBytesToWideWithBinaryRejection now protects my assumption:
                        // No NULL before end, No controls except CR LF.

                        // My japanese sample page gets stuck
                        // in this loop here, doing null char!
                        // I have fixed advance to ask wc < ' '.

                        scan = ProposedIndexPastEndOfLine + 1;
                        #if DO_DEBUG_FITER
                            Routine( L"469" );
                        #endif
                    }
                    else
                    {
                        scan = ProposedIndexPastEndOfLine; // start at very next char
                        #if DO_DEBUG_FITER
                            Routine( L"470" );
                        #endif
                    }
                }

                // Test for end!
                if( scan >= stop )
                {
                    #if DO_DEBUG_FITER
                        Routine( L"471" );
                    #endif
                    break;
                }

            } // End of Outer loop.

        } // End of Very Outer loop.


        // Because CTop had done this by adding newlines to text,
        // yet they are not indexed in any block, I must do this:

        // Now, in order to allow the focus ( top screen line ) to be set
        // even on the last line of text, I need to add N or N-1 blank
        // lines to buffer, where N is the current g_nLinesPageAdvances

        unsigned int iAfter = 0;
        for( ;; )
        {
            if( ++iAfter > g_nLinesPageAdvances )
                break;

            // Cloned from my clone above,
            // but set offset to m_nWideTextBackingDisplay.
            // I hope that is safe. A click will tell...

            {
                // Between each of original outer loops, below,
                // I need to add a blank line to the line list.
                // I cloned the following, except set count = 0.
                // And for current text offset, I use AtopBlock.

                // Add this line to line list.

                if( m_nLenFittedLineFilledCount == m_nMalFittedLineMallocCount )
                {
                    // There is no more room for 1 new line. Make room.
                    size_t nNeed = m_nLenFittedLineFilledCount + 100 + ( m_nLenFittedLineFilledCount / 8 ); // grow liberally 125 %
                    void * pvm = MyRealloc( 9993, m_nMalFittedLineMallocCount * sizeof( INDEX ), m_pMalFittedLineList, nNeed * sizeof( INDEX ) );
                    if( pvm == NULL )
                    {
                        // do not change original m_pMalFittedLineList, m_nMalFittedLineMallocCount
                        // Worse, reset everything to known empty condition.
                        // Failure cleanup.
                        Reset( ); // Because I may have started filling line list.
                        SatLeaveCrisis( );
                        return; // failure - already messaged
                    }
                    m_pMalFittedLineList = ( FITTED_LINE * ) pvm;
                    m_nMalFittedLineMallocCount = nNeed;
                }

                m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharIndex = m_nWideTextBackingDisplay;
                m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharCount = 0;
                m_nLenFittedLineFilledCount ++;
            }

            // End of a post-outer-loop little loop.
        }

    }
    else
    {
        // This original version of outer loop will show
        // all the text of the current CBud being viewed.

        // outer loop does line after line until all the input buffer is listed.

        // I prefer pointers, but indirection is safer:
        size_t scan = 0;
        size_t stop = m_nWideTextBackingDisplay;
        for( ;; )
        {
            // Inner loop 1 looks ahead for a newline to force a hard break.
            // Inner loop sums unkerned USASCII widths for alternate ending.
            // If I estimate low, who cares? If over, GetTextExtent catches.
            // Inner loop only cares about characters from scan to stop - 1.
            // Any characters below scan were done by the prior loop if any.
            // There should be a NULL exactly at [stop] in my CWsb thingies.
            // N.B. however, if nLen == 0, then possibly also szBuf == NULL;

            size_t StartingIndexOfCurrentLine = scan;
            size_t ProposedIndexPastEndOfLine = 0;
            size_t IndexOfMostRecentSpace = 0;
            size_t SumOfWidths = 0;

            #if DO_DEBUG_FITER
                ; SpewValue( L"scan", scan );
            #endif

            for( ;; )
            {
                wchar_t wc = m_szWideTextBackingDisplay[ scan ];

                // severe verbosity
                // #if DO_DEBUG_FITER
                //     ; SpewValue( L"wc", wc );
                // #endif

                // CategorizeAndConvertInputBytesToWideWithBinaryRejection now protects my assumption:
                // No NULL before end, No controls except CR LF.

                if( wc < ' ' ) // Anything lower than space is newline, NULL.
                {
                    // My japanese sample page gets stuck
                    // in this loop here, doing null char!
                    // I have fixed advance to ask wc < ' '.
                    ProposedIndexPastEndOfLine = scan;
                    break;
                }

                if( wc == ' ' )
                    IndexOfMostRecentSpace = scan;

                if( wc < SOME_GLYPHS )
                    SumOfWidths += g_SomeGlyphWidths[ wc ];
                else
                    SumOfWidths += g_WidestGlyphWidth;

                if( SumOfWidths > g_MaxExtentLogicalUnits )
                {
                    if( IndexOfMostRecentSpace > StartingIndexOfCurrentLine )
                        ProposedIndexPastEndOfLine = IndexOfMostRecentSpace;
                    else
                        ProposedIndexPastEndOfLine = scan;
                    break;
                }

                scan ++;
            } // End of Inner loop 1.

            #if DO_DEBUG_FITER
                Routine( L"458" );
            #endif

            if( StartingIndexOfCurrentLine == ProposedIndexPastEndOfLine )
            {
                #if DO_DEBUG_FITER
                    Routine( L"460" );
                #endif
                // Empty line. No adjustments needed.
            }
            else
            {
                #if DO_DEBUG_FITER
                    Routine( L"461" );
                #endif
                // Start of Inner ( non- )"loop" 2a.
                // Attempt adjustments of end of line.
                // Actually, this 2a will not be a loop.
                // One measurement, zero or one cuts, and it's done.

                wchar_t ReplaceThisCharAfterEOS =
                    m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine ];
                m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine ] = NULL;

                int HowManyProposed = ( int ) ( ProposedIndexPastEndOfLine - StartingIndexOfCurrentLine );
                int HowManyFit = 0;
                SIZE StringDim; // required by windows

                if( ! GetTextExtentExPoint(
                    g_hdcCompat,
                    m_szWideTextBackingDisplay + StartingIndexOfCurrentLine,
                    HowManyProposed,
                    g_MaxExtentLogicalUnits,
                    & HowManyFit,
                    NULL,
                    & StringDim
                ) )
                {
                    // Failure cleanup.
                    Reset( ); // Because I may have started filling line list.
                    SatLeaveCrisis( );
                    ProgramError( L"GetTextExtentExPoint 2" );
                    return; // failure
                }

                m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine ] =
                    ReplaceThisCharAfterEOS;

                if( HowManyProposed <= HowManyFit )
                {
                    #if DO_DEBUG_FITER
                        Routine( L"462" );
                    #endif
                    // Line fits. No adjustments needed.
                }
                else
                {
                    #if DO_DEBUG_FITER
                        Routine( L"463" );
                    #endif

                    // A loop to look backwards for any last space.
                    scan = StartingIndexOfCurrentLine + HowManyFit;
                    for( ;; )
                    {
                        // Start of Inner loop 2b.
                        // Suppose 5 fit, [0] to [4], and [5]==' '; Done.

                        if( m_szWideTextBackingDisplay[ scan ] == ' ' )
                            break;

                        if( scan == StartingIndexOfCurrentLine )
                            break;

                        scan --;

                    } // End of Inner loop 2b.

                    if( scan > StartingIndexOfCurrentLine )
                    {
                        #if DO_DEBUG_FITER
                            Routine( L"464" );
                        #endif
                        // this is a good new wrapping point.
                        ProposedIndexPastEndOfLine = scan;
                    }
                    else
                    {
                        #if DO_DEBUG_FITER
                            Routine( L"465" );
                        #endif
                        // This is one big word ( often URL )
                        // that we must wrap in its middle.
                        ProposedIndexPastEndOfLine = StartingIndexOfCurrentLine + HowManyFit;

                        // Make sure at least 1 char was fitted. Just to end loop.
                        if( HowManyFit == 0 )
                            ProposedIndexPastEndOfLine ++;
                    }
                }

            } // End of Inner ( non- )"loop" 2a.

            #if DO_DEBUG_FITER
                Routine( L"459" );
            #endif

            // I have determined ProposedIndexPastEndOfLine.

            // Whatever happens, even empty line,
            // I determined one fitted line runs
            // from StartingIndexOfCurrentLine
            // to ProposedIndexPastEndOfLine - 1.

            #if DO_DEBUG_FITER
                ; SpewValue( L"bo", StartingIndexOfCurrentLine );
                ; SpewValue( L"eo", ProposedIndexPastEndOfLine - 1 );
            #endif

            // Add this line to line list.

            if( m_nLenFittedLineFilledCount == m_nMalFittedLineMallocCount )
            {
                // There is no more room for 1 new line. Make room.
                size_t nNeed = m_nLenFittedLineFilledCount + 100 + ( m_nLenFittedLineFilledCount / 8 ); // grow liberally 125 %
                void * pvm = MyRealloc( 9993, m_nMalFittedLineMallocCount * sizeof( INDEX ), m_pMalFittedLineList, nNeed * sizeof( INDEX ) );
                if( pvm == NULL )
                {
                    // do not change original m_pMalFittedLineList, m_nMalFittedLineMallocCount
                    // Worse, reset everything to known empty condition.
                    // Failure cleanup.
                    Reset( ); // Because I may have started filling line list.
                    SatLeaveCrisis( );
                    return; // failure - already messaged
                }
                m_pMalFittedLineList = ( FITTED_LINE * ) pvm;
                m_nMalFittedLineMallocCount = nNeed;
            }

            m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharIndex = StartingIndexOfCurrentLine;
            m_pMalFittedLineList[ m_nLenFittedLineFilledCount ] . CharCount = ( size_t ) ( ProposedIndexPastEndOfLine - StartingIndexOfCurrentLine );
            m_nLenFittedLineFilledCount ++;

            // No need to affect m_nFittedLineFocusedAtopScreen since size grew.

            #if DO_DEBUG_FITER
                Routine( L"466" );
            #endif

            // Get ready to scan the next line.
            // Sort out various possible newlines to skip over.
            // What happened?
            //
            // The character at [ProposedIndexPastEndOfLine] was not shown.
            //
            // If that character is part of a newline ( CR, or LF, or CR LF )
            // then continue past that whole ONE newline, to whatever next.
            //
            // If that character is NULL, it is end of string. Stop loop.
            //
            // If that character is space, I may either keep it or omit it.
            //
            // If that character is printable, it will start the next line.
            //
            // If I keep space, it distinguishes from case of mid-word wrap,
            // and it has been my habit to start wrapped lines with a space.
            //
            // If I omit space, it prevents moving one column on empty line.
            // That is, I would need to check next is not a newline, etc.

            {
                // oh-oh. I see a fetch of +1 before checking if is in buffer.

                wchar_t wc1 = 0; // leave null if hit stop
                if( ProposedIndexPastEndOfLine < stop )
                    wc1 = m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine + 0 ];

                wchar_t wc2 = 0; // leave null if hit stop
                if( ProposedIndexPastEndOfLine + 1 < stop )
                    wc2 = m_szWideTextBackingDisplay[ ProposedIndexPastEndOfLine + 1 ];

                if( wc1 == L'\r' && wc2 == L'\n' )
                {
                    // Exact CR LF combination
                    // Setup next loop to scart linescan here:
                    scan = ProposedIndexPastEndOfLine + 2;
                    #if DO_DEBUG_FITER
                        Routine( L"467" );
                    #endif
                }
                else if( wc1 == L'\r' || wc1 == L'\n' )
                {
                    // Just one CR or one LF. ( Not planned )
                    // Setup next loop to scart linescan here:
                    scan = ProposedIndexPastEndOfLine + 1;
                    #if DO_DEBUG_FITER
                        Routine( L"468" );
                    #endif
                }
                else if( wc1 < ' ' )
                {
                    // Comment atop loop said:
                    // Anything lower than space is newline, NULL.

                    // CategorizeAndConvertInputBytesToWideWithBinaryRejection now protects my assumption:
                    // No NULL before end, No controls except CR LF.

                    // My japanese sample page gets stuck
                    // in this loop here, doing null char!
                    // I have fixed advance to ask wc < ' '.

                    scan = ProposedIndexPastEndOfLine + 1;
                    #if DO_DEBUG_FITER
                        Routine( L"469" );
                    #endif
                }
                else
                {
                    scan = ProposedIndexPastEndOfLine; // start at very next char
                    #if DO_DEBUG_FITER
                        Routine( L"470" );
                    #endif
                }
            }

            // Test for end!
            if( scan >= stop )
            {
                #if DO_DEBUG_FITER
                    Routine( L"471" );
                #endif
                break;
            }

        } // End of Outer loop.
    }


    #if DO_DEBUG_FITER
        Routine( L"472" );
    #endif

    SatLeaveCrisis( );
    return;
}

LRESULT CSee::OnWmKeydown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"223" );
    #endif
    SatEnterCrisis( );
    // This is as copied from Windows help: getting keyboard input
    // to influence the "standard" scrollbar ( i.e., not a control ).

    // I will let the right & left arrow keys start smooth scrolling.
    int wScrollNotify = -1;
    switch ( wParam )
    {

        case VK_RETURN:
            // GREAT new idea! Let Carriage Return act like
            // a mouse click ( to follow link ) under cursor.
            OnVkReturn( );
            break;

#ifndef _WIN32_WCE
        case VK_F3:
            // Repeat any search ( if dialog ever left szFindWhat set. )
            if( g_fr_Accept_F3 )
            {
                OnFindNext( );
            }
            else
            {
                SetUpFindDialog( );
            }
            break;
#endif // not _WIN32_WCE

        case VK_SPACE:
            // duplicate this code at VK_DOWN:
            // Update this before leaving view,
            // As next restore will do WM_SIZE.

            // Crashed here when no text in ring.
            // Did not test for currentview null.
            if( Top.CurrentView != NULL )
                Top.CurrentView->FocusLineOffsetInText = GetOffsetofFocusLine( );
            ShowWindow( g_hWnd, SW_HIDE );
            ShowWindow( g_hWnd, SW_MINIMIZE );
            break;

        case VK_UP: // arrow up
            // I think I'll let ARROW UP stop smooth scrolling,
            // otherwise one must reach way over to a spacebar.
            // During braking, omit the jarring one line shift.
            if( m_ActiveSmoothScrollingScanLinesPerAct != 0 )
                QuitSmoothScrolling( 0 );
            else
                wScrollNotify = SB_LINEUP;
            break;

        case VK_PRIOR: // page up
            wScrollNotify = SB_PAGEUP;
            break;

        case VK_NEXT: // page down
            wScrollNotify = SB_PAGEDOWN;
            break;

        case VK_DOWN: // arrow down
            #ifdef _WIN32_WCE

                // On Pocket PC version, Arrow down has the customary function."
                wScrollNotify = SB_LINEDOWN;

            #else // not _WIN32_WCE

                // Instead of a scrolling act, do the hide function.
                // This duplicates the code from VK_SPACE:
                // Update this before leaving view,
                // As next restore will do WM_SIZE.
                // Crashed here when no text in ring.
                // Did not test for currentview null.
                if( Top.CurrentView != NULL )
                    Top.CurrentView->FocusLineOffsetInText = GetOffsetofFocusLine( );
                ShowWindow( g_hWnd, SW_HIDE );
                ShowWindow( g_hWnd, SW_MINIMIZE );

            #endif // _WIN32_WCE

            break;

        case VK_HOME:
            QuitSmoothScrolling( 1 );
            wScrollNotify = SB_TOP;
            break;

        case VK_END:
            QuitSmoothScrolling( 1 );
            wScrollNotify = SB_BOTTOM;
            break;

        case VK_LEFT: // arrow left
            GoSlowerSmoothScrolling( );
            break;

        case VK_RIGHT: // arrow right
            GoFasterSmoothScrolling( );
            break;

        case VK_DELETE:
            OnVkDelete( );
            break;

        case VK_ESCAPE:
            // Immediate cleardown and exit of program
            // ShowWindow( g_hWnd, SW_HIDE );
            // ExitingWithCancel( 1 ); // 1 = for escape key. No save question.
            // I dunno. I hit this key too often, as I learned in e-mail.

            // I decided to let ESCAPE be a synonym of DELETE.
            OnVkDelete( );
            break;
    }

    if ( wScrollNotify != -1 )
        PostMessage( g_hWnd, WM_VSCROLL, MAKELONG( wScrollNotify, 0 ), 0L );

    SatLeaveCrisis( );
    return 0;
}


void CSee::OnVkDelete( )
{
    #if DO_DEBUG_CALLS
        Routine( L"224" );
    #endif
    // It's a nightmare between threads and message loops.
    // Any thread running does not depend on Top's views.
    // I do not need to wait on thread to coast to a stop.

    // Change this to 1. Stop thread; else 2. Delete view.
    // Also save a global pointer for user to re-activate.
    if( Top.CurrentView != NULL )
    {
        if( Top.CurrentView->pFruit->m_WorkInProgress )
        {
            Top.CurrentView->pFruit->m_StopThisThread = 1;
        }
        else
        {
            g_pFruitOfLastDeletedView = Top.CurrentView->pFruit;
            g_OffsetOfLastDeletedView = GetOffsetofFocusLine( );
            QuitSmoothScrolling( 1 );
            Top.DeleteCurrentView( );
        }
    }
}

void CSee::OnVkReturn( )
{
    #if DO_DEBUG_CALLS
        Routine( L"225" );
    #endif
    SatEnterCrisis( );

    // Doing an alignment right now might shift the line vs cursor.
    // So change QuitSmoothScrolling parameter from 1 to 0:
    // In fact, I list as a bug that enter key in a paper stops SS,
    // and it shouldn't, as enter has no purpose. Just don't call it.
    // QuitSmoothScrolling( 0 ); // 0 = don't align scanline

    // This will serve a great new idea, that pressing ENTER
    // should act like a click under mouse to follow a link.
    // It does just some of the things used in real mouse'r.
    // I don't even need the X position to follow to source.

    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        SatLeaveCrisis( );
        return;
    }

    CBud * pFruit = NULL;

    // CurrentView is a volatile pointer to top Wonderland!
    // Lest thread or user do all manner of things to ring:
    if( Top.CurrentView == NULL )
    {
        // I hope these are a dime a dozen.
        SatLeaveCrisis( );
        return;
    }
    pFruit = Top.CurrentView->pFruit;
    if( pFruit == NULL )
    {
        // I'll take two at that price.
        SatLeaveCrisis( );
        return;
    }

    // What is my exposure hereafter to invalidity?
    // CTop only ever deletes INTERNAL type pfruit.

    int nLine;
    int nFitted;

    // Do it once to get the idea ( down ).

    int YPos = m_LastMouseMoveYPos;

    // The [0-th] line Y offset starts at 0 - m_CurrentSmoothScrollingScanLine
    // This nLine ( in screen ) is from 0 to g_nLinesPaintingStop.
    nLine = ( YPos + m_CurrentSmoothScrollingScanLine ) / g_LineHeight;

    nFitted = m_nFittedLineFocusedAtopScreen + nLine;
    // this limit test allowed a garbage pointer. Decrement by one. Retest 0.
    if( nFitted > m_nLenFittedLineFilledCount - 1 )
        nFitted = m_nLenFittedLineFilledCount - 1;
    if( nFitted < 0 )
        nFitted = 0;

    {
        // carefully bracket all FITTED_LINE blocks to aid debug.
        FITTED_LINE * lp = m_pMalFittedLineList + nFitted;
        FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;

        wchar_t * pChars1 = m_szWideTextBackingDisplay;
        size_t nChars1 = 0;
        size_t Addon1 = 0;
        size_t Offset1 = 0;

        if( lp >= m_pMalFittedLineList
        &&  lp < xp )
        {
            pChars1 = m_szWideTextBackingDisplay + lp->CharIndex;
            nChars1 = lp->CharCount;
            Addon1 = HowManyGlyphsFit( pChars1, nChars1, m_LastMouseMoveXPos - m_LineXRef );
            Offset1 = lp->CharIndex + Addon1;
        }

        int FruitType = pFruit->m_FruitType;
        switch( FruitType )
        {
        case ENUM_ADDCACHE:
            {
                COneCache * pCache = ( COneCache * ) pFruit; // Viz., OnlyCache
                CIdx * pIdxResultIndex = pCache->pIdxResultIndex;
                Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
            }
            break;

        case ENUM_ADDPHRASES:
            {
                COnePhrases * pFacts = ( COnePhrases * ) pFruit; // Viz., OnlyFacts
                CIdx * pIdxResultIndex = pFacts->pIdxResultIndex;
                Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
            }
            break;

        case ENUM_ADDDIRECTORY:
            {
                COneFolder * pFolder = ( COneFolder * ) pFruit;
                CIdx * pIdxResultIndex = pFolder->pIdxResultIndex;
                Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
            }
            break;

        case ENUM_ADDSEARCH:
            {
                COneQuery * pQuery = ( COneQuery * ) pFruit;
                CIdx * pIdxResultIndex = pQuery->pIdxResultIndex;
                Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
            }
            break;

        case ENUM_ADDPAGE:
            {
                COneFetch * pFetch = ( COneFetch * ) pFruit;
                CIdx * pIdxResultIndex = pFetch->pIdxResultIndex;
                Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
            }
            break;

        case ENUM_ADDFIND:
            {
                COneKwic * pKwic = ( COneKwic * ) pFruit;
                CIdx * pIdxResultIndex = pKwic->pIdxResultIndex;
                Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
            }
            break;
        }
    }
    SatLeaveCrisis( );
    return;
}

void CSee::CopyClipboardMakeNewPaper( )
{
    #if DO_DEBUG_CALLS
        Routine( L"317" );
    #endif
    SatEnterCrisis( );
    // The clipboard is amazingly simple.
    // Almost a mirror/clone of copy act.

    if ( ! OpenClipboard( g_hWnd ) )
    {
        SatLeaveCrisis( );
        ProgramError( L"OpenClipboard" );
        return;
    }

    // The clipboard controls the handle that the GetClipboardData
    // function returns, not the application. The application should
    // copy the data immediately. The application must not free the
    // handle nor leave it locked. The application must not use the
    // handle after the EmptyClipboard or CloseClipboard function
    // is called, or after the SetClipboardData function is called
    // with the same clipboard format.

    HGLOBAL hglbCopy = GetClipboardData( CF_UNICODETEXT );
    if( hglbCopy == NULL )
    {
        CloseClipboard( );
        SatLeaveCrisis( );
        return;
    }

    // Lock the handle and copy the text from the buffer.

    wchar_t * lptstrCopy = ( wchar_t * ) GlobalLock( hglbCopy );
    if( lptstrCopy == NULL )
    {
        GlobalFree( hglbCopy );
        hglbCopy = NULL;
        CloseClipboard( );
        SatLeaveCrisis( );
        ProgramError( L"GlobalLock" );
        return;
    }

    size_t nWBuf = wcslen( lptstrCopy );
    if( nWBuf == NULL )
    {
        GlobalFree( hglbCopy );
        hglbCopy = NULL;
        CloseClipboard( );
        SatLeaveCrisis( );
        return;
    }

    wchar_t * pWBuf = ( wchar_t * ) MyMalloc( 1847, ( nWBuf + 1 ) * sizeof( wchar_t ) );
    if( pWBuf == NULL )
    {
        GlobalFree( hglbCopy );
        hglbCopy = NULL;
        CloseClipboard( );
        SatLeaveCrisis( );
        return;
    }

    memcpy( pWBuf, lptstrCopy, nWBuf * sizeof( wchar_t ) );
    pWBuf [ nWBuf ] = NULL;

    GlobalUnlock( hglbCopy );
    // Do not test return value: 0 for no error; else a count, now 0.

    if ( ! CloseClipboard( ) )
    {
        SatLeaveCrisis( );
        ProgramError( L"CloseClipboard" );
        return;
    }

    // Now I can proceed like CTop's AddInternal routine.

    // instead of an ordinal, use CRC over text to make more unique URLs.

    unsigned int HashValue = CrcHash( pWBuf ) & 0x000fffff; // big enuf?

    wchar_t szFileUrl[80];
    g_nPasteCounter ++;
    wsprintf( szFileUrl, L"file://PastedText/%d", HashValue );

    // Go ahead and claim a fake File: type URL for internals.
    size_t UrlIndexToClaim = CSolAllUrls.AddKey( szFileUrl );
    #if DO_DEBUG_ADDFIND
        if( UrlIndexToClaim == 1 )
            { Spew( L"AddFind 1 at ctop 745" ); }
    #endif

    // If this is the second time added, the claim will fail,
    // but everything is already good in the pOnePaper fruit.

    COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndexToClaim );
    if( pOnePaper == NULL )
    {
        if( CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_CLAIMING ) )
        {
            #if DO_DEBUG_INPUT
                ; Spew( L"ClaimUserpVoid Succeeded." );
            #endif
            int AsText = 1; // treat INTERNAL as TXT input - honor newlines

            pOnePaper = new COnePaper( 0 ); // this one is for pasted text.
            // Every making of a new COnePaper must create its pWsbAnnotation.
            pOnePaper->pWsbAnnotation = new CWsb;

            // This part parses, analyzes, the source paper.
            // 1 of 5 similar sequences. This one is for add pasted text.

            // I just need second half of ProcessPaper.

            pOnePaper->pWsbResultText->Addn( pWBuf, nWBuf );
            Pag.SecondPartOfProcessForText( UrlIndexToClaim, SHOW_AS_NON_QRP, pOnePaper );

            // Now insert all annotations and second newline, atop paper content.
            {
                size_t nMalNewTop = 0;
                wchar_t * pMalNewTop = pOnePaper->pWsbAnnotation->GetBuffer( & nMalNewTop );
                pOnePaper->pWsbResultText->Insert( pMalNewTop, 1 ); // +1 newline
                int SizeChange = nMalNewTop + 2; // wchars: 1 for CR, 1 for LF
                pOnePaper->pIdxResultIndex->IncreaseOffsets( SizeChange );
                pOnePaper->pIdxTextBlocks->IncreaseOffsets( SizeChange );
                pOnePaper->pIdxSentences->IncreaseOffsets( SizeChange );
                MyFree( 908, UNPREDICTABLE, pMalNewTop );
                pMalNewTop = NULL;
            }

            // Now I must hang the paper.

            // I am about to have CSol swap in my paper pointer for the marker.
            // Before doing that, paper need to contain the valid index number.

            pOnePaper->m_CSolIndex = UrlIndexToClaim;

            // This call must work, and finishes my internal tasks.
            if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, pOnePaper ) )
            {
                SatLeaveCrisis( );
                ProgramError( L"CommonPaperFileInput: ! CSolAllUrls.ClaimUserpVoid" );
                return; // failure
            }
        }
    }

    // This is okay, in main thread, servicing ^V keystroke:
    Top.Add( pOnePaper ); // Put fruit on display

    SatLeaveCrisis( );
    return;
}

void CSee::CopyCurrentViewToClipboard( )
{
    #if DO_DEBUG_CALLS
        Routine( L"316" );
    #endif
    SatEnterCrisis( );
    // This serves a Control+C keystroke.
    // It was cloned from the mouse drag.

    // Hey, first, is m_nWideTextBackingDisplay non-null?

    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        SatLeaveCrisis( );
        return;
    }

    // In case I might do a copy-to-system-clipboard,
    // obtain a copy of all text backing the display.

    size_t nWhole;
    wchar_t * pWhole;

    nWhole = m_nWideTextBackingDisplay;
    pWhole = new wchar_t[ nWhole + 1 ]; // 1 to insert *
    memcpy( pWhole, m_szWideTextBackingDisplay, nWhole * sizeof( wchar_t ) );
    pWhole [ nWhole ] = NULL;

    CopyOffsetsToClipboard( pWhole, nWhole, 0, nWhole, 0 ); // never append
    // But keep malloc ownership out here.
    delete pWhole;
    pWhole = NULL;
    SatLeaveCrisis( );
    return;
}


void CSee::CopyOffsetsToClipboard( wchar_t * pWhole, size_t nWhole, size_t Offset1, size_t Offset2, int append )
{
    #if DO_DEBUG_CALLS
        Routine( L"315" );
    #endif
    SatEnterCrisis( );
    // Instead of copying from offset1 to offset2,
    // Make a copy in which to do 80 col line wrap,
    // and append the URL after the block of text.
    // Sixty is pretty tight, it re-wraps so many
    // lines that were already wrapped to eighty.

    // What is worst case expansion?
    // Hard newlines will not expand.
    // I might add one CR LF per 80 chars.
    // Hence, not more than 1/40.
    // I might also add one max URL, but not wrapped,
    // and about five "  -- " chars, and 3 pairs of CR LF.

    int length = Offset2 - Offset1;
    length += length / 20;
    length += MAX_TOKEN_TEXT;
    length += 20;

    // Now, add 1 max HTML title and say 5 indent and CR LF.
    length += MAX_TITLE_OR_ANCHOR + 10;

    size_t nMalWrapped = length * sizeof( wchar_t );
    wchar_t * pMalWrapped = ( wchar_t * ) MyMalloc( 1950, nMalWrapped );
    wchar_t * into = pMalWrapped;
    wchar_t * from = pWhole + Offset1;
    wchar_t * stop = pWhole + Offset2;
    size_t column = 0;

    // Do a bi-phase outer loop to scan over spaces and tokens,
    // not to trim off any more, but just to do word wrapping.

    // The from pointer advances token by token in outer loop.
    for( ;; )
    {
        wchar_t * scan = from; // is now atop a token

        // Phase 1 inner loop:
        for( ;; )
        {
            // process printables
            if( scan == stop
            || iswspace( *scan ) )
            {
                // from is atop word
                // scan is past word
                // decide whether to wrap before this word.
                size_t wordlength = scan - from;

                // Design loop so this test is always true:
                // Obviated... if( wordlength > 0 )

                if( column + wordlength > 80 )
                {
                    *into++ = '\r';
                    *into++ = '\n';
                    column = 0;
                }

                memcpy( into, from, wordlength * sizeof( wchar_t ) );
                into += wordlength;
                column += wordlength;

                break;
            }
            scan ++;
        }

        // Phase 2 inner loop:
        for( ;; )
        {
            // process spaces and newlines

            if( scan == stop
            || ! iswspace( *scan ) )
                break;

            // This loop will copy ALL whitespace.
            // which may be due to my indentation,
            // also spaces in pre-formatted text,
            // also hard newlines bounding blocks
            // or lines in pre-formatted text.
            // I left NO TAB in any held text.

            *into++ = *scan; // iswspace MEANS: CR, LF, or space.

            if( *scan == ' ' )
            {
                column ++; // space takes a column
            }
            else
            {
                column = 0; // CR or LF resets column
            }
            scan ++;
        }

        if( scan == stop )
            break;

        from = scan; // set to next printable for next loop
    }

    // Add any final newline before appending the URL.
    if( column > 0 )
    {
        *into++ = '\r';
        *into++ = '\n';
    }

    *into++ = ' ';
    *into++ = ' ';
    *into++ = '-';
    *into++ = '-';
    *into++ = ' ';

    // Now, copy the URL ( or whatever is on first line of text )
    // Allow that a URL may someday contain spaces.

    from = pWhole;
    wchar_t * stop2 = pMalWrapped + length - 6; // fat room for CR LF NULL
    wchar_t * stop1 = stop2 - ( MAX_TITLE_OR_ANCHOR + 10 ); // room for title

    for( ;; )
    {
        *into++ = *from++;
        if( into == stop1 )
            break;
        // This line has been considered, remains valid:
        if( *from < ' ' ) // stop on first CR LF or NULL
            break;
    }
    *into++ = '\r'; // post-URL newline
    *into++ = '\n';


    // In addition to annotating with the URL ( from top line ),
    // append any TITLE text right after that, also indented.

    // First, see if there is a title associated with it.
    // But Jeez, I barely know you. Is there even a view?

    // What if a thread changes view on me? Like the auto-click?
    // Thread can't change it now, due to the critical sections.

    if( Top.CurrentView != NULL )
    {
        CBud * pFruit = Top.CurrentView->pFruit;
        if( pFruit != NULL )
        {
            if( pFruit->m_FruitType == ENUM_ONEPAPER )
            {
                COnePaper * pOnePaper = ( COnePaper * ) pFruit;
                CWsb * pWsbTitleText = pOnePaper->pWsbTitleText;
                if( pWsbTitleText != NULL )
                {
                    size_t nMalTitle = 0;
                    wchar_t * pMalTitle = pWsbTitleText->GetBuffer( & nMalTitle );
                    if( pMalTitle != NULL )
                    {
                        // Indent, just as far as the hyphens, looks good:
                        *into++ = ' ';
                        *into++ = ' ';

                        from = pMalTitle;
                        for( ;; )
                        {
                            *into++ = *from++;
                            if( into == stop2 )
                                break;
                            if( *from == NULL )
                                break;
                        }
                        MyFree( 1684, 0, pMalTitle );

                        *into++ = '\r'; // post-title newline, if any.
                        *into++ = '\n';
                    }
                }
            }
        }
    }

    *into++ = '\r'; // final, separating newline
    *into++ = '\n';
    *into = NULL;

    // Now the text to copy is from pMalWrapped to into.

    // Do something to append, not replace clipboard.
    // If the control key is NOT down, reset a global
    // clipboard text accumulator Wsb. Then Addn the
    // new item. Then get buffer back, use it below.

    if( ! append )
        WsbClipboardAccumulator.Reset( ); // Oh, that was easy.

    WsbClipboardAccumulator.Addn( pMalWrapped, into - pMalWrapped );
    // done with original malloc and size.
    MyFree( 2189, 0, pMalWrapped );
    size_t nNewWrapped = 0;

    // fill name and size with a new malloc.
    pMalWrapped = WsbClipboardAccumulator.GetBuffer( & nNewWrapped );
    into = pMalWrapped + nNewWrapped; // adjust to do next malloc

    if ( ! OpenClipboard( g_hWnd ) )
    {
        SatLeaveCrisis( );
        ProgramError( L"OpenClipboard" );
        return;
    }
    if ( ! EmptyClipboard( ) )
    {
        CloseClipboard( );
        SatLeaveCrisis( );
        ProgramError( L"EmptyClipboard" );
        return;
    }

    // Allocate a global memory object for the text.

    HGLOBAL hglbCopy = GlobalAlloc( GMEM_MOVEABLE,
        ( into - pMalWrapped + 1 ) * sizeof( wchar_t ) );

    if ( hglbCopy == NULL )
    {
        CloseClipboard( );
        SatLeaveCrisis( );
        ProgramError( L"GlobalAlloc" );
        return;
    }

    // Lock the handle and copy the text to the buffer.

    wchar_t * lptstrCopy = ( wchar_t * ) GlobalLock( hglbCopy );
    if( lptstrCopy == NULL )
    {
        GlobalFree( hglbCopy );
        hglbCopy = NULL;
        CloseClipboard( );
        SatLeaveCrisis( );
        ProgramError( L"GlobalLock" );
        return;
    }

    memcpy( lptstrCopy, pMalWrapped,
        ( into - pMalWrapped ) * sizeof( wchar_t ) );

    lptstrCopy[ into - pMalWrapped ] = NULL;

    GlobalUnlock( hglbCopy );
    // Do not test return value: 0 for no error; else a count, now 0.

    // Place the handle on the clipboard.

    if( ! SetClipboardData( CF_UNICODETEXT, hglbCopy ) )
    {
        MyFree( 2255, 0, pMalWrapped );
        pMalWrapped = NULL;
        GlobalFree( hglbCopy );
        hglbCopy = NULL;
        CloseClipboard( );
        SatLeaveCrisis( );
        ProgramError( L"SetClipboardData" );
        return;
    }

    // After SetClipboardData is called, the system owns
    // the object identified by the hMem parameter.
    // But Notepad cannot paste until I close clipboard.

    if ( ! CloseClipboard( ) )
    {
        MyFree( 2273, 0, pMalWrapped );
        pMalWrapped = NULL;
        SatLeaveCrisis( );
        ProgramError( L"CloseClipboard" );
        return;
    }

    MyFree( 2281, 0, pMalWrapped );
    pMalWrapped = NULL;
    SatLeaveCrisis( );
    return;
}


LRESULT CSee::WmMouseHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"226" );
    #endif
    SatEnterCrisis( );
    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        SatLeaveCrisis( );
        return 0;
    }

    if( message == WM_MOUSEMOVE )
    {
        // Move has only a trivial need
        // to know the current position
        // when a user presses VK_RETURN.
        m_LastMouseMoveXPos = LOWORD( lParam );
        m_LastMouseMoveYPos = HIWORD( lParam );

        // After I characterize move values, I will comment out:
        // What this showed is that mouse move coordinates range
        // from 0,0 to clientwidth-1 and clientheight-1 inclusive.
        // #if DO_DEBUG_MOUSE
        // {
        //     wchar_t wk[60];
        //     wsprintf( wk, L"Mouse Move. X=%4d. Y=%4d.", m_LastMouseMoveXPos, m_LastMouseMoveYPos );
        //     ; Spew( wk );
        // }
        // #endif
        //
        // Mouse X Range ( ClientWidth ) = 0 to 742.
        // Mouse Y Range ( ClientHeight ) = 0 to 484.
        //
        // N.B.! These adjacent lines share one common Y LU count value:
        //
        // Line[ 0] Y is from    0 to   25.
        // Line[ 1] Y is from   25 to   50.
        // Line[ 2] Y is from   50 to   75.
        // Line[ 3] Y is from   75 to  100.
        // Line[ 4] Y is from  100 to  125.
        // Line[ 5] Y is from  125 to  150.
        // Line[ 6] Y is from  150 to  175.
        // Line[ 7] Y is from  175 to  200.
        // Line[ 8] Y is from  200 to  225.
        // Line[ 9] Y is from  225 to  250.
        // Line[10] Y is from  250 to  275.
        // Line[11] Y is from  275 to  300.
        // Line[12] Y is from  300 to  325.
        // Line[13] Y is from  325 to  350.
        // Line[14] Y is from  350 to  375.
        // Line[15] Y is from  375 to  400.
        // Line[16] Y is from  400 to  425.
        // Line[17] Y is from  425 to  450.
        // Line[18] Y is from  450 to  475.
        // Line[19] Y is from  475 to  500.
        // Line[20] Y is from  500 to  525.
        //
        // Mouse Move. X=   0. Y=   0.
        // Mouse Move. X= 741. Y= 474.
        // Mouse Move. X= 483. Y= 359.

        SatLeaveCrisis( );
        return 0;
    }

    CBud * pFruit = NULL;

    if( Top.CurrentView == NULL )
    {
        // I hope these are a dime a dozen.
        SatLeaveCrisis( );
        return 0;
    }
    pFruit = Top.CurrentView->pFruit;
    if( pFruit == NULL )
    {
        // I'll take two at that price.
        SatLeaveCrisis( );
        return 0;
    }

    // What is my exposure hereafter to invalidity?
    // CTop only ever deletes INTERNAL type pfruit.

    // X and Y are in client window
    int XPos = LOWORD( lParam );
    int YPos = HIWORD( lParam );

    // I intend to have CONTROL + DRAG append to clipboard.
    int ControlKeyDown = ( wParam & MK_CONTROL );
    int append = ControlKeyDown;

    int FinishThePair = 0;

    switch( message )
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        #if DO_DEBUG_MOUSE
        {
            wchar_t wk[60];
            wsprintf( wk, L"Mouse Down. X=%4d. Y=%4d.", XPos, YPos );
            ; Spew( wk );
        }
        #endif
        // This bool is only set & cleared by mouse down & up,
        // to be sure it always controls mouse capture truely.
        // However, it has a side effect of pausing scrolling.
        if( ! m_MouseButtonDown_PauseScrolling )
        {
            SetCapture( hWnd );
            m_MouseButtonDown_PauseScrolling = 1;
            m_MouseButtonDown_XPos = XPos;
            m_MouseButtonDown_YPos = YPos;
        }
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        #if DO_DEBUG_MOUSE
        {
            wchar_t wk[60];
            wsprintf( wk, L"Mouse -UP-. X=%4d. Y=%4d.", XPos, YPos );
            ; Spew( wk );
            wsprintf( wk, L"While Scanline Offset %4d.", m_CurrentSmoothScrollingScanLine );
            ; Spew( wk );
        }
        #endif
        // This bool is only set & cleared by mouse down & up,
        // to be sure it always controls mouse capture truely.
        // However, it has a side effect of pausing scrolling.
        if( m_MouseButtonDown_PauseScrolling )
        {
            m_MouseButtonUp_XPos = XPos;
            m_MouseButtonUp_YPos = YPos;
            m_MouseButtonDown_PauseScrolling = 0;
            ReleaseCapture( );
            FinishThePair = 1; // I.E., on every mouse-up.
        }
        break;
    }

    if( FinishThePair )
    {
        // I.E., on every mouse-up,
        // having saved one down and one up coordinate.

        int nLine;
        int nFitted;

        size_t nChars1;
        wchar_t * pChars1 = 0;

        size_t nChars2;
        wchar_t * pChars2 = 0;

        size_t nWhole;
        wchar_t * pWhole;

        // Do it once to get the idea ( down ).

        YPos = m_MouseButtonDown_YPos;

        // The [0-th] line Y offset starts at 0 - m_CurrentSmoothScrollingScanLine
        // This nLine ( in screen ) is from 0 to g_nLinesPaintingStop.
        nLine = ( YPos + m_CurrentSmoothScrollingScanLine ) / g_LineHeight;

        #if DO_DEBUG_MOUSE
        {
            wchar_t wk[60];
            wsprintf( wk, L"Line # Down = %2d.", nLine );
            ; Spew( wk );
        }
        #endif

        nFitted = m_nFittedLineFocusedAtopScreen + nLine;
        // this limit test allowed a garbage pointer. Decrement by one. Retest 0.
        if( nFitted > m_nLenFittedLineFilledCount - 1 )
            nFitted = m_nLenFittedLineFilledCount - 1;
        if( nFitted < 0 )
            nFitted = 0;

        {
            // carefully bracket all FITTED_LINE blocks to aid debug.
            FITTED_LINE * lp = m_pMalFittedLineList + nFitted;
            FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;

            if( lp >= m_pMalFittedLineList
            &&  lp < xp )
            {
                #if DO_DEBUG_MOUSE
                {
                    wchar_t wk[60];
                    wsprintf( wk, L"Text Offset Down = %6d.", lp->CharIndex );
                    ; Spew( wk );
                    wsprintf( wk, L"Text Length Down = %6d.", lp->CharCount );
                    ; Spew( wk );
                }
                #endif
                pChars1 = m_szWideTextBackingDisplay + lp->CharIndex;
                nChars1 = lp->CharCount;
            }
        }

        // Do it again while in the spirit ( up ).

        YPos = m_MouseButtonUp_YPos;

        // The [0-th] line Y offset starts at 0 - m_CurrentSmoothScrollingScanLine
        // This nLine ( in screen ) is from 0 to g_nLinesPaintingStop.
        nLine = ( YPos + m_CurrentSmoothScrollingScanLine ) / g_LineHeight;

        #if DO_DEBUG_MOUSE
        {
            wchar_t wk[60];
            wsprintf( wk, L"Line # Up = %2d.", nLine );
            ; Spew( wk );
        }
        #endif

        nFitted = m_nFittedLineFocusedAtopScreen + nLine;
        // this limit test allowed a garbage pointer. Decrement by one. Retest 0.
        if( nFitted > m_nLenFittedLineFilledCount - 1 )
            nFitted = m_nLenFittedLineFilledCount - 1;
        if( nFitted < 0 )
            nFitted = 0;

        {
            // carefully bracket all FITTED_LINE blocks to aid debug.
            FITTED_LINE * lp = m_pMalFittedLineList + nFitted;
            FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;

            if( lp >= m_pMalFittedLineList
            &&  lp < xp )
            {
                #if DO_DEBUG_MOUSE
                {
                    wchar_t wk[60];
                    wsprintf( wk, L"Text Offset -Up- = %6d.", lp->CharIndex );
                    ; Spew( wk );
                    wsprintf( wk, L"Text Length -Up- = %6d.", lp->CharCount );
                    ; Spew( wk );
                }
                #endif
                pChars2 = m_szWideTextBackingDisplay + lp->CharIndex;
                nChars2 = lp->CharCount;
            }
        }

        // In case I might do a copy-to-system-clipboard,
        // obtain a copy of all text backing the display.

        nWhole = m_nWideTextBackingDisplay;
        pWhole = new wchar_t[ nWhole + 1 ]; // 1 to insert *
        memcpy( pWhole, m_szWideTextBackingDisplay, nWhole * sizeof( wchar_t ) );
        pWhole [ nWhole ] = NULL;


        // Fix order now, while I have few variables.
        if( pChars2 < pChars1 )
        {
            #if DO_DEBUG_MOUSE
            {
                wchar_t wk[60];
                wsprintf( wk, L"Decided to swap so ( down < up )." );
                ; Spew( wk );
            }
            #endif
            wchar_t * ptemp = pChars1;
            pChars1 = pChars2;
            pChars2 = ptemp;

            size_t ntemp = nChars1;
            nChars1 = nChars2;
            nChars2 = ntemp;

            // I also have to swap the XPos used below.

            XPos = m_MouseButtonDown_XPos;
            m_MouseButtonDown_XPos = m_MouseButtonUp_XPos;
            m_MouseButtonUp_XPos = XPos;
        }

        // With debugging on, I clicked exactly on the 'o' of you,
        // and the 'a' before thread, and got this result:
        // The very letter clicked upon "does not fit":
        // One of these two callers should adjust for that.
        //
        // Fit?[ pages show the progress of a thread while it works. You may scroll, or click in,]
        // Fit:[ pages show the progress of ]
        // Fit?[ on the items in a result page, you navigate to that source text. When in]
        // Fit:[ on the items in a result page, y]

        // Here, I clicked well left, and very far right of the line.
        // Fit?[ Words, Extended]
        // Fit:[]
        // Fit?[ Words, Extended]
        // Fit:[ Words, Extended]
        // That result seems A-okay.

        // In a blank line, I passed a zero count, and got a zero fit:
        // Fit?[]
        // Fit:[]

        // In partially off-screen lines at top and bottom, it worked okay.

        size_t Addon1 = HowManyGlyphsFit( pChars1, nChars1, m_MouseButtonDown_XPos - m_LineXRef );
        size_t Addon2 = HowManyGlyphsFit( pChars2, nChars2, m_MouseButtonUp_XPos - m_LineXRef );

        if( pChars2 == pChars1
        && Addon2 < Addon1 )
        {
            size_t ntemp = Addon1;
            Addon1 = Addon2;
            Addon2 = ntemp;
        }

        // You see here decisions relative to m_szWideTextBackingDisplay,
        // but after this point, use pWhole, not m_szWideTextBackingDisplay.
        // Also, use Offsets, not pChars, except when test if multi-line.

        size_t Offset1 = ( pChars1 - m_szWideTextBackingDisplay ) + Addon1;
        size_t Offset2 = ( pChars2 - m_szWideTextBackingDisplay ) + Addon2;

        // Check whether a drag ended outside the window.

        int CopyToClipboard = 0;
        int CopyingWholeText = 0;

        if( m_MouseButtonUp_XPos < 0
        ||  m_MouseButtonUp_YPos < 0
        ||  m_MouseButtonUp_XPos > ( int ) g_ClientWidth
        ||  m_MouseButtonUp_YPos > ( int ) g_ClientHeight )
        {
            CopyToClipboard = 1;
            CopyingWholeText = 1;
            Offset1 = 0;
            Offset2 = nWhole;
        }
        else if( Offset1 == Offset2 )
        {
            // No brainer: It was a click, not a drag.
            // Which !CopyToClipboard I process below.
        }
        else if( Offset2 - Offset1 > 2
        || pChars2 != pChars1 )
        {
            // If the drag was more than two chars,
            // Or, if the drag went multiple lines.
            CopyToClipboard = 1; // a sizeable drag
        }
        else
        {
            // Scan between offsets to see if exceed one word.
            // But if down and up are within same word, click.

            // Consider the possible fit +1 adjustment analysis:
            // Suppose text = "...xxx abcde xxx..."
            // A click before 'a' may leave offset at 'x before, or at space.
            // A click on 'a' may leave offset at space before, or at 'a'.
            // A click on 'e' may leave offset at 'd' before 'e', or at 'e'.
            // A click after 'e' may leave offset at 'e', or at space.
            // So I might choose to adjust Offset1, but never Offset2.
            // Well, except in the case of both being on different lines.
            // If Addon1 == nChars1, then all fit, so apply no test; Else:
            // If Addon1 was 0 ( none fit ), and Offset1[1] is not at space;
            // OR, if Offset1[0] is at space, and Offset1[1] is not at space;
            // then raise offset1 by + 1 to point to the first letter of word.

            // Actually, do not act upon Offset1 yet, because I may do the
            // same analysis later, but change only my local variable scan:
            size_t scan = Offset1;
            if( Addon1 < nChars1 )
            {
                if( ( Addon1 == 0 || iswspace( pWhole[ Offset1 ] ) )
                && ! iswspace( pWhole[ Offset1 + 1 ] ) )
                {
                    #if DO_DEBUG_MOUSE
                        ; Spew( L"Raising Offset1 by 1 before click/drag test." );
                    #endif
                    scan++;
                }
            }

            // when I reach here after
            // failing if( Offset1 == Offset2 ) and
            // failing ( Offset2 - Offset1 > 2 ) tests, either:
            // Offset2 = Offset1 + 0 ( due to +1 adjustment above ); or
            // Offset2 = Offset1 + 1; or:
            // Offset2 = Offset1 + 2;
            // Offset1 MAY be atop a line.
            // Offset2 MAY be past a line ( pointing to newline or null ).

            for( ;; )
            {
                // This test will no longer face a newline nor a null:
                if( scan >= Offset2 )
                {
                    #if DO_DEBUG_MOUSE
                        ; Spew( L"Click/drag test saw no space: So, click." );
                    #endif
                    break;
                }
                if( iswspace( pWhole[ scan ] ) )
                {
                    #if DO_DEBUG_MOUSE
                        ; Spew( L"Click/drag test saw a space: So, drag." );
                    #endif
                    CopyToClipboard = 1; // spanned two words: drag.
                    break;
                }
                scan ++;
            }

            // The analysis above has been well tested with debugs on.
            // Now apply the same analysis to delimiting tokens below.
        }

        if( ! CopyingWholeText )
        {
            // That means, Merely not a drag ending outside the window.

            // But, this could be a click within one token.
            // Or this could be a drag over multiple tokens.

            // First, re-apply the Offset1 +1 analysis from above.
            // Do not apply any similar bump +1 analysis to Offset2.
            if( Addon1 < nChars1 )
            {
                if( ( Addon1 == 0 || iswspace( pWhole[ Offset1 ] ) )
                && ! iswspace( pWhole[ Offset1 + 1 ] ) )
                {
                    #if DO_DEBUG_MOUSE
                        ; Spew( L"Raising Offset1 by 1 before deciding extents." );
                    #endif
                    Offset1++;
                    // However, do not permit that to reverse order!
                    if( Offset2 < Offset1 )
                        Offset2 = Offset1;
                }
            }

            // Adjust Offset1 and Offset2 to whole word boundaries.

            // Consider special case, for multiple lines,
            // when top position is to the right of line.
            // That should highlight the subsequent line.
            if( pChars2 != pChars1
            && Addon1 == nChars1 )
            {
                #if DO_DEBUG_MOUSE
                    ; Spew( L"Case is multiple lines AND Offset1 at end of line." );
                #endif
                // Offset1 is on any hard ( CR LF ) newline,
                // or pointing to first char of next line.
                // Move to next line, except at end of buffer.
                // This is really a test to pass a hard newline,
                // but it is okay to advance over a first space.
                if( Offset1 < nWhole
                && iswspace( pWhole[ Offset1 ] ) )
                {
                    #if DO_DEBUG_MOUSE
                        ; Spew( L"So moved Offset1 into next line." );
                    #endif
                    Offset1 ++;
                    // However, do not permit that to reverse order!
                    if( Offset2 < Offset1 )
                        Offset2 = Offset1;
                }
            }

            // Now, scan outward over non-space characters.
            // That means, if in a token, get all of it.
            for( ;; )
            {
                if( Offset1 == 0 )
                    break;
                if( iswspace( pWhole[ Offset1 ] ) )
                    break;
                #if DO_DEBUG_MOUSE
                    ; Spew( L"In token, do Offset1 --." );
                #endif
                Offset1 --; // stop on space before, or zero
            }
            for( ;; )
            {
                if( Offset2 == nWhole )
                    break;
                if( iswspace( pWhole[ Offset2 ] ) )
                    break;
                #if DO_DEBUG_MOUSE
                    ; Spew( L"In token, do Offset2 ++." );
                #endif
                Offset2 ++; // stop on space after, or past end
            }

            // Now, scan inward over space characters.
            // That means, if in spaces, get back into token.
            // Do not search past the other offset value.
            for( ;; )
            {
                if( Offset1 == Offset2 )
                    break;
                if( ! iswspace( pWhole[ Offset1 ] ) )
                    break;
                #if DO_DEBUG_MOUSE
                    ; Spew( L"Before token, do Offset1 ++." );
                #endif
                Offset1 ++;
            }
            for( ;; )
            {
                if( Offset2 == Offset1 )
                    break;
                if( Offset2 == 0 )
                    break;
                // This time, I test one character lower,
                // so Offset2 will serve as a Past value.
                if( ! iswspace( pWhole[ Offset2 - 1 ] ) )
                    break;
                #if DO_DEBUG_MOUSE
                    ; Spew( L"After token, do Offset2 --." );
                #endif
                Offset2 --;
            }

            // Now, what if click or drag on whitespace got nothing?
            // In that case, the offsets should now be equal.
            // If unequal, we have already a well-delmited something.
            if( Offset2 == Offset1 )
            {
                #if DO_DEBUG_MOUSE
                    ; Spew( L"Click or Drag caught whitespace only." );
                #endif

                // Now we have a click or drag that didn't catch
                // anything but whitespace, and I have to decide
                // what near-by token to chose ( a single token. )

                // In the case that Offset1 click was at the top of a line:
                // Then this first clause is reverse order of second clause:
                // Search upward for a token; Failing that, search downward.

                // This original test assumed only 0 or 1 spaces atop line:
                // if( Addon1 <= 1 )
                // I should generalize it to any field of spaces atop line.
                // If I got here, Offset1 did not undergo the +1 bump.
                // But Offset1 may have moved all the way up to Offset2.
                // Recover the original Offset1 just long enough to test
                // whether any printables are left of it on same line.

                int SearchUpward = 1; // prediction
                {
                    size_t TopOfLine1 = ( pChars1 - m_szWideTextBackingDisplay ) ;
                    size_t TryOffset1 = ( pChars1 - m_szWideTextBackingDisplay ) + Addon1;
                    for( ;; )
                    {
                        if( TryOffset1 == TopOfLine1 )
                        {
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"TryOffset1 was atop line. Go right." );
                            #endif
                            break;
                        }
                        if( ! iswspace( pWhole[ TryOffset1 ] ) )
                        {
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"TryOffset1 was not atop line. Go left." );
                            #endif
                            SearchUpward = 0; // TryOffset1 was not atop line
                            break;
                        }
                        TryOffset1--;
                    }
                }

                // I'm converting all my old < ' ' tests to iswspace.
                // In this analysis, we are considering whole tokens.
                // Some later analysis might remove some punctuation.

                if( SearchUpward )
                {
                    // This is the outer up, then down case.
                    // First, search for the next whole word.
                    // scan upward for any non-space character.

                    // Choose either offset, as they are equal.
                    // Best choice already has end limit tests.

                    for( ;; )
                    {
                        if( Offset2 == nWhole )
                            break;
                        if( ! iswspace( pWhole[ Offset2 ] ) )
                            break;
                        #if DO_DEBUG_MOUSE
                            ; Spew( L"Offset2 ++." );
                        #endif
                        Offset2 ++; // stop past end, or on printable
                    }

                    if( Offset2 < nWhole
                    && ! iswspace( pWhole[ Offset2 ] ) )
                    {
                        #if DO_DEBUG_MOUSE
                            ; Spew( L"Found printable." );
                        #endif
                        Offset1 = Offset2; // first printable
                        // continue upward for a space character.
                        for( ;; )
                        {
                            if( Offset2 == nWhole )
                                break;
                            if( iswspace( pWhole[ Offset2 ] ) )
                                break;
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Offset2 ++." );
                            #endif
                            Offset2 ++; // stop past word
                        }
                    }
                    else
                    {
                        // The upward search did not find anything.
                        // Alas, search for the previous whole word.
                        // scan downward for any non-space character.
                        #if DO_DEBUG_MOUSE
                            ; Spew( L"Found nothing upward. Go left." );
                        #endif
                        for( ;; )
                        {
                            if( Offset1 == 0 )
                                break;
                            if( ! iswspace( pWhole[ Offset1 ] ) )
                                break;
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Offset1 --." );
                            #endif
                            Offset1 --; // stop on printable, or zero
                        }

                        if( ! iswspace( pWhole[ Offset1 ] ) )
                        {
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Found printable." );
                            #endif
                            Offset2 = Offset1 + 1; // just past printable
                            // continue downward for a space character.
                            for( ;; )
                            {
                                if( Offset1 == 0 )
                                    break;
                                // Now we are testing -1 below,
                                // to leave Offset1 on a printable
                                if( iswspace( pWhole[ Offset1 - 1 ] ) )
                                    break;
                                #if DO_DEBUG_MOUSE
                                    ; Spew( L"Offset1 --." );
                                #endif
                                Offset1 --; // go, but not into a space
                            }
                        }
                        else
                        {
                            // No words on display? Might never happen now.
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Found no printables in view." );
                            #endif
                            delete pWhole;
                            pWhole = NULL;
                            SatLeaveCrisis( );
                            return 0;
                        }
                    }
                }
                else
                {
                    // This is the outer case opposite of above,
                    // which will search downward before upward.


                    // First, search for the previous whole word.
                    // scan downward for any non-space character.
                    for( ;; )
                    {
                        if( Offset1 == 0 )
                            break;
                        if( ! iswspace( pWhole[ Offset1 ] ) )
                            break;
                        #if DO_DEBUG_MOUSE
                            ; Spew( L"Offset1 --." );
                        #endif
                        Offset1 --; // stop on printable, or zero
                    }

                    if( ! iswspace( pWhole[ Offset1 ] ) )
                    {
                        #if DO_DEBUG_MOUSE
                            ; Spew( L"Found printable." );
                        #endif
                        Offset2 = Offset1 + 1; // just past printable
                        // continue downward for a space character.
                        for( ;; )
                        {
                            if( Offset1 == 0 )
                                break;
                            if( iswspace( pWhole[ Offset1 - 1 ] ) )
                                break;
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Offset1 --." );
                            #endif
                            Offset1 --; // stop on first printable
                        }
                    }
                    else
                    {
                        // Alas, search for the next whole word.
                        // scan upward for any non-space character.
                        for( ;; )
                        {
                            if( Offset2 == nWhole )
                                break;
                            if( ! iswspace( pWhole[ Offset2 ] ) )
                                break;
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Offset2 ++." );
                            #endif
                            Offset2 ++; // stop past end, or on printable
                        }

                        if( Offset2 < nWhole
                        && ! iswspace( pWhole[ Offset2 ] ) )
                        {
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Found printable." );
                            #endif
                            Offset1 = Offset2; // first printable
                            // continue upward for a space character.
                            for( ;; )
                            {
                                if( Offset2 == nWhole )
                                    break;
                                if( iswspace( pWhole[ Offset2 ] ) )
                                    break;
                                #if DO_DEBUG_MOUSE
                                    ; Spew( L"Offset2 ++." );
                                #endif
                                Offset2 ++; // stop past word
                            }
                        }
                        else
                        {
                            // No words on display? Might never happen now.
                            #if DO_DEBUG_MOUSE
                                ; Spew( L"Found no printables in view." );
                            #endif
                            delete pWhole;
                            pWhole = NULL;
                            SatLeaveCrisis( );
                            return 0;
                        }
                    }
                }
            }
        }

        // By now, I either delimited a non-empty field, or left.
        // But just in case I was wrong, bow out quietly.
        if( Offset2 <= Offset1 )
        {
            delete pWhole;
            pWhole = NULL;
            SatLeaveCrisis( );
            return 0;
        }

        if( CopyToClipboard )
        {
            // A drag, not a click: Copy to Clipboard.

            // I now have a synonym in Control+C to do.
            // So moved this massive work to a helper:

            CopyOffsetsToClipboard( pWhole, nWhole, Offset1, Offset2, append );

            // But keep malloc ownership out here.
            delete pWhole;
            pWhole = NULL;
            SatLeaveCrisis( );
            return 0;
        }
        else
        {
            // A click, not a drag: Initiate Kwic or Query.
            // However, in a ResultText, show a source URL.

            // Dont QuitSmoothScrolling yet, wait until after
            // any dialog box, which loss of focus pauses it.
            // Putting this into Add should cover most cases:
            // See.QuitSmoothScrolling( 1 );

            int FruitType = pFruit->m_FruitType;
            switch( FruitType )
            {
            case ENUM_ADDCACHE:
                {
                    COneCache * pCache = ( COneCache * ) pFruit; // Viz., OnlyCache
                    CIdx * pIdxResultIndex = pCache->pIdxResultIndex;
                    Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
                }
                break;

            case ENUM_ADDPHRASES:
                {
                    COnePhrases * pFacts = ( COnePhrases * ) pFruit; // Viz., OnlyFacts
                    CIdx * pIdxResultIndex = pFacts->pIdxResultIndex;
                    Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
                }
                break;

            case ENUM_ADDDIRECTORY:
                {
                    COneFolder * pFolder = ( COneFolder * ) pFruit;
                    CIdx * pIdxResultIndex = pFolder->pIdxResultIndex;
                    Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
                }
                break;

            case ENUM_ADDSEARCH:
                {
                    COneQuery * pQuery = ( COneQuery * ) pFruit;
                    CIdx * pIdxResultIndex = pQuery->pIdxResultIndex;
                    Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
                }
                break;

            case ENUM_ADDPAGE:
                {
                    COneFetch * pFetch = ( COneFetch * ) pFruit;
                    CIdx * pIdxResultIndex = pFetch->pIdxResultIndex;
                    Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
                }
                break;

            case ENUM_ADDFIND:
                {
                    COneKwic * pKwic = ( COneKwic * ) pFruit;
                    CIdx * pIdxResultIndex = pKwic->pIdxResultIndex;
                    Top.FollowClickToAdd( pIdxResultIndex, Offset1 );
                }
                break;

            case ENUM_ONEPAPER:
                {
                    // Here we service a mouse click in a source paper,
                    // including 3 internal texts, but not result logs.

                    // Code above has already found boundaries of token.
                    // We may do ADD: FIND, WEB PAGE or WEB SEARCH here.


                    /// wip - resume iswide analysis here...

                    // Make a null-terminated copy of the selected text.
                    int nChars = Offset2 - Offset1;
                    int nMalCopySelText = ( nChars + 1 ) * sizeof( wchar_t );
                    wchar_t * pMalCopySelText = ( wchar_t * ) MyMalloc( 2205, nMalCopySelText );
                    memcpy( pMalCopySelText, pWhole + Offset1, nMalCopySelText );
                    pMalCopySelText[ nChars ] = NULL;

                    // later... - also remove puncts, etc...

                    // For now, just show the selected text.
                    // Say( pMalCopySelText );

                    // Let's point a global ptr to it to init dialog...

                    g_ClickTextToInitDialogs = pMalCopySelText;

                    // It's easier in wordsex.cpp than in csee.cpp.
                    if( Pag.ClearTextTokenResemblesUrl( pMalCopySelText, nChars ) )
                    {
                        ClickShowsDialogs( ENUM_ADDPAGE );
                    }
                    else
                    {
                        // strip leading punctuation.
                        wchar_t * scan = pMalCopySelText;
                        for( ;; )
                        {
                            if( scan[0] == NULL )
                                break;
                            if( iswalnum( scan[0] ) )
                                break;
                            scan ++;
                        }
                        g_ClickTextToInitDialogs = scan;
                        // get past end
                        for( ;; )
                        {
                            if( scan[0] == NULL )
                                break;
                            scan ++;
                        }
                        // strip trailing punctuation.
                        for( ;; )
                        {
                            if( scan == g_ClickTextToInitDialogs )
                                break;
                            if( iswalnum ( scan[-1] ) )
                                break;
                            scan --;
                        }
                        scan[0] = NULL;
                        // remove any final apostraphe, s.
                        if( scan >= g_ClickTextToInitDialogs + 2
                        && ( scan[-1] | ' ' ) == 's'
                        && ( scan[-2] | ' ' ) == '\'' )
                        {
                            scan -= 2;
                            scan[0] = NULL;
                        }
                        // lowercase
                        scan = g_ClickTextToInitDialogs;
                        for( ;; )
                        {
                            if( scan[0] == NULL )
                                break;
                            if( iswupper( scan[0] ) )
                                scan[0] |= ' ';
                            scan ++;
                        }


                        // Give this a whirl...
                        // Por.Stem( g_ClickTextToInitDialogs, scan - g_ClickTextToInitDialogs );

                        // What if it is empty? Go anyway...
                        if( message == WM_LBUTTONUP )
                            ClickShowsDialogs( ENUM_ADDFIND );
                        else
                            ClickShowsDialogs( ENUM_ADDSEARCH );
                    }
                    g_ClickTextToInitDialogs = NULL;
                    MyFree( 2420, nMalCopySelText, pMalCopySelText );
                    pMalCopySelText = NULL;
                }
                break;

            default:
                SatLeaveCrisis( );
                ProgramError( L"MouseHandler FruitType" );
                return 0;
                break;
            }
        }

        delete pWhole;
        pWhole = NULL;

    }

    SatLeaveCrisis( );
    return 0;
}

int CSee::HowManyGlyphsFit( wchar_t * pChars, int nChars, int nWidth )
{
    #if DO_DEBUG_CALLS
        Routine( L"227" );
    #endif
    SatEnterCrisis( );

    // With debugging on, I clicked exactly on the 'o' of you,
    // and the 'a' before thread, and got this result:
    // The very letter clicked upon "does not fit":
    // One of my two callers should adjust for that.
    //
    // Fit?[ pages show the progress of a thread while it works. You may scroll, or click in,]
    // Fit:[ pages show the progress of ]
    // Fit?[ on the items in a result page, you navigate to that source text. When in]
    // Fit:[ on the items in a result page, y]

    // What happens at end of line, start of line, blank line, top/end row?

    #if DO_DEBUG_MOUSE
    {
        wchar_t wk[500];
        wchar_t * into = wk;
        wchar_t * from = pChars;
        wchar_t * stop = pChars + nChars;
        *into++ = 'F';
        *into++ = 'i';
        *into++ = 't';
        *into++ = '?';
        *into++ = '[';
        for( ;; )
        {
            if( from == stop )
                break;
            if( into == wk + 490 )
                break;
            *into++ = *from++;
        }
        *into++ = ']';
        *into = NULL;
        ; Spew( wk );
    }
    #endif

    int HowManyFit = 0;
    if( nWidth > 0 )
    {
        SIZE StringDim;
        if( ! GetTextExtentExPoint(
            g_hdcCompat,
            pChars,
            nChars,
            nWidth,
            & HowManyFit,
            NULL,
            & StringDim
        ) )
        {
            SatLeaveCrisis( );
            ProgramError( L"GetTextExtentExPoint 3" );
            return 0; // failure
        }
    }

    if( HowManyFit < 0 )
    {
        #if DO_DEBUG_MOUSE
            ; SpewValue( L"( HowManyFit < 0 ): ", HowManyFit );
        #endif
        HowManyFit = 0;
    }
    if( HowManyFit > nChars )
    {
        #if DO_DEBUG_MOUSE
            ; SpewValue( L"( HowManyFit > nChars ): ", HowManyFit );
        #endif
        HowManyFit = nChars;
    }

    #if DO_DEBUG_MOUSE
    {
        wchar_t wk[500];
        wchar_t * into = wk;
        wchar_t * from = pChars;
        wchar_t * stop = pChars + HowManyFit;
        *into++ = 'F';
        *into++ = 'i';
        *into++ = 't';
        *into++ = ':';
        *into++ = '[';
        for( ;; )
        {
            if( from == stop )
                break;
            if( into == wk + 490 )
                break;
            *into++ = *from++;
        }
        *into++ = ']';
        *into = NULL;
        ; Spew( wk );
    }
    #endif

    SatLeaveCrisis( );
    return HowManyFit;
}

void CSee::SetSmoothScrollScanLineOffset( int ScanLineOffset )
{
    #if DO_DEBUG_CALLS
        Routine( L"228" );
    #endif
    SatEnterCrisis( );
    // ScanLineOffset should only vary within range 0 to g_LineHeight-1.
    // As text scrolls up, ScanLineOffset grows from 0 to g_LineHeight-1.
    // As text scrolls down, ScanLineOffset shrinks from g_LineHeight-1 to 0.
    // As ScanLineOffset increases, character rises, absolute Y reduces.

    // N.B.! These adjacent lines share one common Y LU count value:
    //
    // Line[ 0] Y is from    0 to   25.
    // Line[ 1] Y is from   25 to   50.
    // Line[ 2] Y is from   50 to   75.

    // m_NewDisplayLineVector = 1; // Arm so CPas will also transfer these

    m_CurrentSmoothScrollingScanLine = ScanLineOffset;
    size_t i = 0;
    for( ;; )
    {
        g_LineRects[i].top    = g_BaseRects[i].top    - m_CurrentSmoothScrollingScanLine;
        g_LineRects[i].bottom = g_BaseRects[i].bottom - m_CurrentSmoothScrollingScanLine;
        if( ++i >= g_nLinesPaintingStop )
            break;
    }
    IMightHaveChangedTheDisplay( ); // Due to smooth scrolling
    SatLeaveCrisis( );
    return;
}

void CSee::GoFasterSmoothScrolling( )
{
    #if DO_DEBUG_CALLS
        Routine( L"229" );
    #endif
    SatEnterCrisis( );
    if( m_ExactScanLinesPerSecond == 0.0 )
        m_ExactScanLinesPerSecond = g_LineHeight / 2.0; // Start at 1/2 textline / sec

    // Because the optimization to quantitized times
    // may not make a change on every step of factor,
    // reapply factor until the speed change happens.

    size_t v1a = m_ActiveSmoothScrollingScanLinesPerAct;
    size_t v2a = m_ActiveSmoothScrollingMsPerAct;
    int tries = 0;
    for( ;; )
    {
        if( ++ tries > 10 )
            break; // CYA
        m_ExactScanLinesPerSecond *= 1.2;

        if( ! OptimizeResultingScrollRate( ) )
            break; // Optimize decided no SS now.

        size_t v1b = m_ActiveSmoothScrollingScanLinesPerAct;
        size_t v2b = m_ActiveSmoothScrollingMsPerAct;
        if( v1a != v1b
        || v2a != v2b )
        {
            SetTimer( g_hWnd, SMOOTH_SCROLL_TIMER, m_ActiveSmoothScrollingMsPerAct, NULL );
            break;
        }
    }

    SatLeaveCrisis( );
    return;
}

void CSee::GoSlowerSmoothScrolling( )
{
    #if DO_DEBUG_CALLS
        Routine( L"230" );
    #endif
    SatEnterCrisis( );
    if( m_ExactScanLinesPerSecond == 0.0 )
        m_ExactScanLinesPerSecond = g_LineHeight / 2.4; // Apply first 1.2 now.

    // Because the optimization to quantitized times
    // may not make a change on every step of factor,
    // reapply factor until the speed change happens.

    size_t v1a = m_ActiveSmoothScrollingScanLinesPerAct;
    size_t v2a = m_ActiveSmoothScrollingMsPerAct;
    int tries = 0;
    for( ;; )
    {
        if( ++ tries > 10 )
            break; // CYA
        m_ExactScanLinesPerSecond /= 1.2;

        if( ! OptimizeResultingScrollRate( ) )
            break; // Optimize decided no SS now.

        size_t v1b = m_ActiveSmoothScrollingScanLinesPerAct;
        size_t v2b = m_ActiveSmoothScrollingMsPerAct;
        if( v1a != v1b
        || v2a != v2b )
        {
            SetTimer( g_hWnd, SMOOTH_SCROLL_TIMER, m_ActiveSmoothScrollingMsPerAct, NULL );
            break;
        }
    }

    SatLeaveCrisis( );
    return;
}

int CSee::OptimizeResultingScrollRate( )
{
    #if DO_DEBUG_CALLS
        Routine( L"231" );
    #endif
    SatEnterCrisis( );

    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        SatLeaveCrisis( );
        return 0; // tell caller nothing done
    }

    {
        // Do not start a smooth scroll when positioned at end.
        // Repeat the limit test from SetSmoothScrollScanLineOffset

        int limit = 0;
        if( m_nLenFittedLineFilledCount > ( int ) g_nLinesPageAdvances )
            limit = m_nLenFittedLineFilledCount - g_nLinesPageAdvances;
        if( limit < 0 )
            limit = 0;
        if( m_nFittedLineFocusedAtopScreen >= limit )
        {
            SatLeaveCrisis( );
            return 0; // tell caller nothing done
        }
    }


    // Now how shall I optimize into a pair of integers?
    // It would be best to move just 1 line every action,
    // but need to keep actions larger than N TimeQuanta.
    // In fact, try to pick exact multiples for timer ms.

    // E.g., g_SystemClockQuantaIn100ns = 100144 for 10.0144 ms.

    int MinimumMs = g_SystemClockQuantaIn100ns / 10000;
    int nScanlines = 1;
    int ms;

    // The first and original test was to be > 1 timer quanta:

    for( ;; )
    {
        ms = ( int ) ( nScanlines * 1000.0 / m_ExactScanLinesPerSecond );
        if( ms > MinimumMs )
            break;
        nScanlines ++;
    }

    // A second test would be to optimize ms to multiple of quanta.
    {
        int hundrednanosec = ms * 10000;
        int quotient = hundrednanosec / g_SystemClockQuantaIn100ns;
        int exacthns = quotient * g_SystemClockQuantaIn100ns;
        ms = exacthns / 10000;
    }

    m_ActiveSmoothScrollingScanLinesPerAct = nScanlines;
    m_ActiveSmoothScrollingMsPerAct = ms;
    // Let callers do this:
    // SetTimer( g_hWnd, SMOOTH_SCROLL_TIMER, m_ActiveSmoothScrollingMsPerAct, NULL );
    SatLeaveCrisis( );
    return 1; // tell caller computations were done
}

void CSee::QuitSmoothScrolling( int Align )
{
    #if DO_DEBUG_CALLS
        Routine( L"232" );
    #endif
    SatEnterCrisis( );
    // Stop any smooth scrolling, until user starts it.
    if( m_ActiveSmoothScrollingScanLinesPerAct != 0 )
    {
        if( ! KillTimer( g_hWnd, SMOOTH_SCROLL_TIMER ) )
        {} // No... Program Error( L"KillTimer 1" );
    }
    m_ExactScanLinesPerSecond = 0.0;
    m_ActiveSmoothScrollingScanLinesPerAct = 0;
    m_ActiveSmoothScrollingMsPerAct = 0;
    // Before setting the scanline to zero, round up or down to closer line:

    if( Align )
    {
        if( m_CurrentSmoothScrollingScanLine >= g_LineHeight / 2 )
        {

            m_nFittedLineFocusedAtopScreen ++;

            // This is 4th clone of: defend that loop-limit here.

            // A minimum loop-safe adjustment is to N-1,
            // but that left last line at top of screen.
            // So reduce by the visible lines, min zero.
            // These are unsigned, so think a bit on it.

            // empirically, g_nLinesPaintingStop lost some.
            // so test against larger g_nLinesPageAdvances.

            int limit = 0;
            if( m_nLenFittedLineFilledCount > ( int ) g_nLinesPageAdvances )
                limit = m_nLenFittedLineFilledCount - g_nLinesPageAdvances;
            if( limit < 0 )
                limit = 0;
            if( m_nFittedLineFocusedAtopScreen >= limit )
                m_nFittedLineFocusedAtopScreen = limit;
        }
        SetSmoothScrollScanLineOffset( 0 );
    }

    SatLeaveCrisis( );
    return;
}

void CSee::PauseAnySmoothScrolling( )
{
    #if DO_DEBUG_CALLS
        Routine( L"233" );
    #endif
    SatEnterCrisis( );
    // Stop any smooth scrolling, for a brief moment.
    if( m_ActiveSmoothScrollingScanLinesPerAct != 0 )
    {
        // Natchurly, w/o proper bools around it, I'd get this error...

        if( ! KillTimer( g_hWnd, SMOOTH_SCROLL_TIMER ) )
        {} // No... Program Error( L"KillTimer 2" );
    }
    SatLeaveCrisis( );
    return;
}

void CSee::ResumeAnySmoothScrolling( )
{
    #if DO_DEBUG_CALLS
        Routine( L"234" );
    #endif
    SatEnterCrisis( );
    if( m_ActiveSmoothScrollingScanLinesPerAct != 0 )
    {
        if( ! SetTimer( g_hWnd, SMOOTH_SCROLL_TIMER, m_ActiveSmoothScrollingMsPerAct, NULL ) )
        {
            SatLeaveCrisis( );
            ProgramError( L"SetTimer 1" );
            return; // failure
        }
    }
    SatLeaveCrisis( );
    return;
}

void CSee::WmSmoothScrollTimerHandler( )
{
    #if DO_DEBUG_CALLS
        Routine( L"235" );
    #endif
    SatEnterCrisis( );

    // DOWN bool is only set & cleared by mouse down & up,
    // to be sure it always controls mouse capture truely.
    // However, it has a side effect of pausing scrolling.

    if( m_ActiveSmoothScrollingScanLinesPerAct == 0
    || m_MouseButtonDown_PauseScrolling )
    {
        SatLeaveCrisis( );
        return;
    }

    // This is called at the right ms to move by signed ScrollingScanLinesPerAct.
    // Set ScrollingScanLinesPerAct positive to scroll up, negative to scroll down.
    // A single addition might possibly be more than +/- g_LineHeight

    m_CurrentSmoothScrollingScanLine += m_ActiveSmoothScrollingScanLinesPerAct;

    // I deleted a similar negative while-loop.
    // Allow for smooth scrolling upward only.
    while( m_CurrentSmoothScrollingScanLine >= g_LineHeight )
    {
        // Bring into the valid range ( 0 to g_LineHeight - 1 ).
        m_CurrentSmoothScrollingScanLine -= g_LineHeight;
        // And for each line's worth of scanlines, adjust line.

        m_nFittedLineFocusedAtopScreen ++;

        // This is 3rd clone of: defend that loop-limit here.

        // A minimum loop-safe adjustment is to N-1,
        // but that left last line at top of screen.
        // So reduce by the visible lines, min zero.
        // These are unsigned, so think a bit on it.

        // empirically, g_nLinesPaintingStop lost some.
        // so test against larger g_nLinesPageAdvances.

        int limit = 0;
        if( m_nLenFittedLineFilledCount > ( int ) g_nLinesPageAdvances )
            limit = m_nLenFittedLineFilledCount - g_nLinesPageAdvances;
        if( limit < 0 )
            limit = 0;
        // Because the SS can be up by N-1 scanlines, I am changing
        // this clone of limit test from ">" to ">=", to stop sharp.
        if( m_nFittedLineFocusedAtopScreen >= limit )
        {
            m_nFittedLineFocusedAtopScreen = limit;
            QuitSmoothScrolling( 1 );
            m_CurrentSmoothScrollingScanLine = 0; // Come to a full stop.
            break;
        }
    }

    SetSmoothScrollScanLineOffset( m_CurrentSmoothScrollingScanLine );

    IMightHaveChangedTheDisplay( ); // Due to smooth scrolling
    SatLeaveCrisis( );
    return;
}

size_t CSee::GetOffsetofFocusLine( )
{
    #if DO_DEBUG_CALLS
        Routine( L"236" );
    #endif
    SatEnterCrisis( );
    // Top will request this when about to leave a view.

    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        SatLeaveCrisis( );
        return 0;
    }

    int nFitted = m_nFittedLineFocusedAtopScreen;
    // this limit test allowed a garbage pointer. Decrement by one. Retest 0.
    if( nFitted > m_nLenFittedLineFilledCount - 1 )
        nFitted = m_nLenFittedLineFilledCount - 1;
    if( nFitted < 0 )
        nFitted = 0;

    size_t n = 0;
    {
        // carefully bracket all FITTED_LINE blocks to aid debug.
        FITTED_LINE * lp = m_pMalFittedLineList + nFitted;
        FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;

        if( lp >= m_pMalFittedLineList
        &&  lp < xp )
        {
            n = lp->CharIndex;
        }
    }

    if( n > m_nWideTextBackingDisplay - 1 ) // not likely
        n = m_nWideTextBackingDisplay - 1;

    SatLeaveCrisis( );
    return n;
}

void CSee::SetFocusLineToThisOffset( size_t Offset )
{
    #if DO_DEBUG_CALLS
        Routine( L"237" );
    #endif
    SatEnterCrisis( );

    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        m_nFittedLineFocusedAtopScreen = 0;
    }
    else
    {
        // don't return for NULL, callers expect my final act...

        // After some things like FONTPLUS, who call me, the next
        // paint shows aligned screen, but clicks and copies act
        // as if m_CurrentSmoothScrollingScanLine is non-zero.
        // Add this call to fix that, and may as well stop SS:
        // No, I am also used during SS. Cannot do it here:
        // See.QuitSmoothScrolling( 1 ); // 1 = align

        size_t n = m_nLenFittedLineFilledCount;
        size_t i = 0;
        for( ;; )
        {
            i ++; // pre-increment, to make final - 1 safe.
            if( i == n )
                break;
            size_t LineOffset = m_pMalFittedLineList[ i ] . CharIndex;
            if( LineOffset > Offset )
            {
                break;
            }
        }
        m_nFittedLineFocusedAtopScreen = i - 1;

        // defend EVERYTHING!
        if( m_nFittedLineFocusedAtopScreen > ( int ) m_nWideTextBackingDisplay - 1 )
            m_nFittedLineFocusedAtopScreen = m_nWideTextBackingDisplay - 1;
        if( m_nFittedLineFocusedAtopScreen < 0 )
            m_nFittedLineFocusedAtopScreen = 0;

    }

    IMightHaveChangedTheDisplay( ); // Due to outsiders setting offset
    SatLeaveCrisis( );
    return;
}

void CSee::IMightHaveChangedTheDisplay( )
{
    #if DO_DEBUG_CALLS
        Routine( L"322" );
    #endif
    PasEnterCrisis( );

    // The CSee routine sets bools.
    // Versus CPas routine does it.

    NeedPaint = 0; // Servicing this bool now. Disarm it.

    // See comments in CSee::WmPaintHandler,
    // how cannot enter SAT, hangs msg loop.
    // Hence, only trigger him if reached 0.

    UpdateVScrollbarParameters( );

    // And, before invalidate triggers paint,
    // get him private copies of everything.

    // This transfer takes place under the Sat Crix.
    // Pas.SetPaintInput( ); // Under his own Pas Crix

    #if DO_DEBUG_PAINT
        ; Spew( L"Invalidate 100" );
    #endif
    if( ! InvalidateRect( g_hWnd, NULL, 0 ) )
    {
        ProgramError( L"InvalidateRect 2" );
        return; // failure
    }
    #if DO_DEBUG_PAINT
        ; Spew( L"Invalidate 101" );
    #endif

    PasLeaveCrisis( );
    return;
}

void CSee::WmMonitorThreadsTimerHandler( )
{
    // One rule to untangle Crix is to not SendMessage between threads.
    // So main thread must poll, rather than worker thread update view.
    // This will run, say, every 100 ms throughout program life.
    // Aug 05 2007 - As written, this used 100% of CPU doing nothing.
    // 1. fix so not: if( pWsbResultText->StrLen != m_nWideTextBackingDisplay )
    // perhaps by saving a new m_nFruitStrlenBackingDisplay to test instead.
    // Yes, the lengths differ since I added more lines for final screenfull.
    // 2. change time from 100 ms to 500 ms period.

    // Originally, worker threads after adding to any pWsbResultText
    // would call // obs: Top.UpdateViewIfOnScreen, but I had deadlocks with
    // message loop. Instead of that, this timer will examine pFruit
    // backing current view to see if it has grown, and if so, paint
    // and adjust the scrollinfo.

    if( Top.CurrentView != NULL )
    {
        CBud * pFruit = Top.CurrentView->pFruit;
        if( pFruit == NULL )
        {
            KillTimer( g_hWnd, MONITOR_THREADS_TIMER );
            ProgramError( L"MonitorThreads pFruit == NULL" );
            return;
        }
        CWsb * pWsbResultText = pFruit->pWsbResultText;
        if( pWsbResultText == NULL )
        {
            KillTimer( g_hWnd, MONITOR_THREADS_TIMER );
            ProgramError( L"MonitorThreads pWsbResultText == NULL" );
            return;
        }

        if( pWsbResultText->StrLen != m_nFruitStrlenBackingDisplay )
        {
            // tested above that Top.CurrentView != NULL:
            Top.CurrentView->FocusLineOffsetInText = GetOffsetofFocusLine( );

            Top.ShowCurrentTopItemViewOnScreen( ); // It gruesome.
        }
    }

    // Also, originally, I let the fetch worker thread, whenever
    // it got only a single page, automatically simulate a click
    // to do a Top.Add of that fetched page. But now, the worker
    // thread will leave some pFruit pointer( s ) for me to do Add.
    // There may be many thread progress logs doing Top.Add that
    // I can simply move to earlier before initiating the thread.

    if( g_MainShallTopAddThisPaper != NULL )
    {
        Top.Add( g_MainShallTopAddThisPaper ); // Put fruit on display
        g_MainShallTopAddThisPaper = NULL;
    }
}

#ifndef _WIN32_WCE
void CSee::OnFindNext( )
{
    #if DO_DEBUG_CALLS
        Routine( L"486" );
    #endif
    SatEnterCrisis( );

    // I am cloning this from GetOffsetofFocusLine.
    //
    // I was going to call it anyway, to find the offset / line
    // of the focused line, as I plan to start search atop the
    // NEXT line, and bring any found word's line to focus line.
    // Do not wrap from end to top. If nothing found, do nothing.
    //
    // Consult these three states:
    // g_fr_Flags_Down = ( g_fr.Flags & FR_DOWN );
    // g_fr_Flags_Case = ( g_fr.Flags & FR_MATCHCASE );
    // g_fr_Flags_Word = ( g_fr.Flags & FR_WHOLEWORD );
    //
    // Set a new value for:
    //     m_nFittedLineFocusedAtopScreen
    // Then do:
    //     IMightHaveChangedTheDisplay( ); // Due to FindNext

    if( m_pMalFittedLineList == NULL
    || m_nLenFittedLineFilledCount == 0
    || m_szWideTextBackingDisplay == NULL
    || m_nWideTextBackingDisplay == 0 )
    {
        SatLeaveCrisis( );
        return;
    }

    int nFitted = m_nFittedLineFocusedAtopScreen;
    // this limit test allowed a garbage pointer. Decrement by one. Retest 0.
    if( nFitted > m_nLenFittedLineFilledCount - 1 )
        nFitted = m_nLenFittedLineFilledCount - 1;
    if( nFitted < 0 )
        nFitted = 0;

    // size_t n = 0;
    // {
    //     // carefully bracket all FITTED_LINE blocks to aid debug.
    //     FITTED_LINE * lp = m_pMalFittedLineList + nFitted;
    //     FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;
    //
    //     if( lp >= m_pMalFittedLineList
    //     &&  lp < xp )
    //     {
    //         n = lp->CharIndex;
    //     }
    // }
    //
    // if( n > m_nWideTextBackingDisplay - 1 ) // not likely
    //     n = m_nWideTextBackingDisplay - 1;

    // Having inheriting all code above to start,
    // nFitted is the top line, and n its offset.
    // So now code a text search, then set focus.

    int nStartLine;

    if( g_fr_Flags_Down )
    {
        // start forwards search at the top of the next line.
        if( nFitted == m_nLenFittedLineFilledCount - 1 )
        {
            // Already at last line.
            SatLeaveCrisis( );
            return;
        }
        nStartLine = nFitted + 1;
    }
    else
    {
        // start backwards search at the top of current line.
        if( nFitted == 0 )
        {
            // Already at top line.
            SatLeaveCrisis( );
            return;
        }
        nStartLine = nFitted;
    }

    size_t nStartOffset = 0;
    {
        // carefully bracket all FITTED_LINE blocks to aid debug.
        FITTED_LINE * lp = m_pMalFittedLineList + nStartLine;
        FITTED_LINE * xp = m_pMalFittedLineList + m_nLenFittedLineFilledCount;

        if( lp >= m_pMalFittedLineList
        &&  lp < xp )
        {
            nStartOffset = lp->CharIndex;
        }
    }

    if( nStartOffset > m_nWideTextBackingDisplay - 1 ) // not likely
        nStartOffset = m_nWideTextBackingDisplay - 1;

    // I no longer appreciate all those careful lines I cloned.
    // I see m_nWideTextBackingDisplay is said to hold a wcslen.
    // Actual wchars are at [0] to [m_nWideTextBackingDisplay-1].
    // So nStartOffset is right now locating some non-null wchar.

    // Remember, the backing text may, or may not, have a newline
    // at the locations that the line list advances to a new line.
    // Cause loop such that any whitespaces match any whitespaces.
    // I have not even looked yet at contents of g_szFindWhat[80].

    size_t nWhat = wcslen( g_szFindWhat );
    if( nWhat == 0 )
    {
        SatLeaveCrisis( );
        return;
    }

    // Applying the same limits to nStopOffset as for nStartOffset
    // means nStopOffset will also be locating some non-null wchar.
    // Therefore, loops should be stop-tested after trying compare.

    size_t nStopOffset = 0;
    int AllMatched = 0;
    int MatchOffset = 0;

    if( g_fr_Flags_Down )
    {
        nStopOffset = m_nWideTextBackingDisplay - 1;

        // I guess it's time to code the loop:
        size_t nTestOffset = nStartOffset;
        for( ;; )
        {
            // 1. text compare. 2. test if done. 3. advance test offset.

            // 1. text compare.

            // This block makes 1 test, leaves with/without AllMatched.
            {
                // Again, run these limits IN, not PAST, text:
                // No, because I decided to ++ i and j at top.
                // These must be signed, for -1 in bkwds loop.
                int i = 0;
                int iPast = nWhat;
                int j = nTestOffset;
                int jPast = nStopOffset + 1;
                int bFirstChar = 1;

                for( ;; )
                {
                    // Both i and j are IN characters of buffers.
                    wchar_t wc1 = g_szFindWhat[i];
                    wchar_t wc2 = m_szWideTextBackingDisplay[j];

                    // Advance now, to ease multiple advance over spaces.
                    size_t pre_j = j;
                    size_t prior_j = j - 1;
                    i ++;
                    j ++;

                    if( iswspace( wc1 ) )
                    {
                        // can only match spaces
                        if( iswspace( wc2 ) )
                        {
                            // A match of spaces.
                            // Advance both over all spaces.
                            for( ;; )
                            {
                                if( i == iPast )
                                    break;
                                if( ! iswspace( g_szFindWhat[i] ) )
                                    break;
                                i ++;
                            }
                            for( ;; )
                            {
                                if( j == jPast )
                                    break;
                                if( ! iswspace( m_szWideTextBackingDisplay[j] ) )
                                    break;
                                j ++;
                            }
                            if( i == iPast )
                            {
                                // A successful match of whole user text.
                                AllMatched = 1;
                                MatchOffset = nTestOffset;
                                break;
                            }
                        }
                        else
                        {
                            // A failure to match.
                            break; // inner loop
                        }
                    }
                    else
                    {
                        // can only match non-spaces
                        if( iswspace( wc2 ) )
                        {
                            // A failure to match.
                            break; // inner loop
                        }
                        else
                        {
                            // Both are non-spaces.
                            // Test per user rules.
                            int match = 0;
                            // exact match will always do
                            if( wc1 == wc2 )
                            {
                                match = 1;
                            }
                            else if( ! g_fr_Flags_Case )
                            {
                                // approximate match will do
                                // The rule I use for USASCII is okay.
                                if( iswalpha( wc1 )
                                &&  iswalpha( wc2 )
                                &&  ( wc1 | 0x20 ) == ( wc2 | 0x20 ) )
                                {
                                    match = 1;
                                }
                            }

                            if( match
                            && bFirstChar )
                            {
                                bFirstChar = 0; // disarm test

                                if( g_fr_Flags_Word )
                                {
                                    // This must be a "first" character.
                                    // Either, first/last char in buffer
                                    // Or, next char to test is a space.
                                    if( pre_j == nStartOffset
                                    || ! iswalnum( m_szWideTextBackingDisplay[prior_j] ) )
                                    {
                                        // Okay, do not snatch football.
                                    }
                                    else
                                    {
                                        match = 0;
                                    }
                                }
                            }

                            if( match )
                            {
                                if( i == iPast )
                                {
                                    // A successful match of whole user text.
                                    // That will always stop inner loop.
                                    // But before we declare a match,
                                    // May have to verify next space.

                                    if( g_fr_Flags_Word )
                                    {
                                        // hard answer
                                        if( j == jPast
                                        || ! iswalnum( m_szWideTextBackingDisplay[j] ) )
                                        {
                                            AllMatched = 1;
                                            MatchOffset = nTestOffset;
                                        }
                                    }
                                    else
                                    {
                                        // easy answer
                                        AllMatched = 1;
                                        MatchOffset = nTestOffset;
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                // A failure to match.
                                break; // inner loop
                            }
                        }
                    }
                    if( j == jPast )
                    {
                        // The i string is passing end of j string.
                        // No further match is possible.
                        break;
                    }
                    if( i == iPast )
                    {
                        // Interior tests should have broken loop.
                        ProgramError( L"I goofed 2." );
                    }
                }
            }

            if( AllMatched )
            {
                break; // outer loop. Already set the MatchOffset.
            }

            // 2. test if done, for used last starting test offset.
            if( nTestOffset == nStopOffset )
            {
                // no match found. Done.
                SatLeaveCrisis( );
                return;
            }

            // 3. advance test offset.
            nTestOffset ++;
        }
    }
    else
    {
        nStopOffset = 0;

        // I guess it's time to code the loop:
        size_t nTestOffset = nStartOffset;
        for( ;; )
        {
            // 1. text compare. 2. test if done. 3. advance test offset.

            // 1. text compare.

            // This block makes 1 test, leaves with/without AllMatched.
            {
                // Again, run these limits IN, not PAST, text:
                // No, because I decided to ++ i and j at top.
                // These must be signed, for -1 in bkwds loop.
                int i = nWhat - 1;
                int iPast = -1;
                int j = nTestOffset;
                int jPast = nStopOffset - 1;
                int bFirstChar = 1;

                for( ;; )
                {
                    // Both i and j are IN characters of buffers.
                    wchar_t wc1 = g_szFindWhat[i];
                    wchar_t wc2 = m_szWideTextBackingDisplay[j];

                    // Advance now, to ease multiple advance over spaces.
                    size_t pre_j = j;
                    size_t prior_j = j + 1;
                    i --;
                    j --;

                    if( iswspace( wc1 ) )
                    {
                        // can only match spaces
                        if( iswspace( wc2 ) )
                        {
                            // A match of spaces.
                            // Advance both over all spaces.
                            for( ;; )
                            {
                                if( i == iPast )
                                    break;
                                if( ! iswspace( g_szFindWhat[i] ) )
                                    break;
                                i --;
                            }
                            for( ;; )
                            {
                                if( j == jPast )
                                    break;
                                if( ! iswspace( m_szWideTextBackingDisplay[j] ) )
                                    break;
                                j --;
                            }
                            if( i == iPast )
                            {
                                // A successful match of whole user text.
                                AllMatched = 1;
                                MatchOffset = nTestOffset;
                                break;
                            }
                        }
                        else
                        {
                            // A failure to match.
                            break; // inner loop
                        }
                    }
                    else
                    {
                        // can only match non-spaces
                        if( iswspace( wc2 ) )
                        {
                            // A failure to match.
                            break; // inner loop
                        }
                        else
                        {
                            // Both are non-spaces.
                            // Test per user rules.
                            int match = 0;
                            // exact match will always do
                            if( wc1 == wc2 )
                            {
                                match = 1;
                            }
                            else if( ! g_fr_Flags_Case )
                            {
                                // approximate match will do
                                // The rule I use for USASCII is okay.
                                if( iswalpha( wc1 )
                                &&  iswalpha( wc2 )
                                &&  ( wc1 | 0x20 ) == ( wc2 | 0x20 ) )
                                {
                                    match = 1;
                                }
                            }

                            if( match
                            && bFirstChar )
                            {
                                bFirstChar = 0; // disarm test

                                if( g_fr_Flags_Word )
                                {
                                    // This must be a "first" character.
                                    // Either, first/last char in buffer
                                    // Or, next char to test is a space.
                                    if( pre_j == nStartOffset
                                    || ! iswalnum( m_szWideTextBackingDisplay[prior_j] ) )
                                    {
                                        // Okay, do not snatch football.
                                    }
                                    else
                                    {
                                        match = 0;
                                    }
                                }
                            }

                            if( match )
                            {
                                if( i == iPast )
                                {
                                    // A successful match of whole user text.
                                    // That will always stop inner loop.
                                    // But before we declare a match,
                                    // May have to verify next space.

                                    if( g_fr_Flags_Word )
                                    {
                                        // hard answer
                                        if( j == jPast
                                        || ! iswalnum( m_szWideTextBackingDisplay[j] ) )
                                        {
                                            AllMatched = 1;
                                            MatchOffset = nTestOffset;
                                        }
                                    }
                                    else
                                    {
                                        // easy answer
                                        AllMatched = 1;
                                        MatchOffset = nTestOffset;
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                // A failure to match.
                                break; // inner loop
                            }
                        }
                    }
                    if( j == jPast )
                    {
                        // The i string is passing end of j string.
                        // No further match is possible.
                        break;
                    }
                    if( i == iPast )
                    {
                        // Interior tests should have broken loop.
                        ProgramError( L"I goofed 2." );
                    }
                }
            }

            if( AllMatched )
            {
                break; // outer loop. Already set the MatchOffset.
            }

            // 2. test if done, for used last starting test offset.
            if( nTestOffset == nStopOffset )
            {
                // no match found. Done.
                SatLeaveCrisis( );
                return;
            }

            // 3. advance test offset.
            nTestOffset --;
        }
    }

    if( AllMatched )
    {
        // I need to determine line, move view to the MatchOffset.
        // Got it.
        // I had a case of the hit being positioned out of sight.
        // It must be the scanline was near the end of that line.
        See.QuitSmoothScrolling( 1 ); // 1 = align
        SetFocusLineToThisOffset( MatchOffset );
    }

    SatLeaveCrisis( );
}
#endif // not _WIN32_WCE
