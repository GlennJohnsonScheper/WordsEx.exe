// This is file: CLin.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// CLin parallels CSol for the case of CSolAllUrls,
// to hold other URL information without burdening
// the oft-used CSol class with additional members.

// CLin's main vector will hold one pointer per slot.
// Non-null slots point to some kind of XRef thingy:
// I don't see ( at this time ) XRef has to be a class.
// How about a structure with malloc, filled counts,
// and a re-alloc'able tail vector of information,
// which information would be N x:
// 1. An index into CSolAllUrls ( of the head, or tail )
// 2. An index into CSolAnchorText ( given with head )
// Then there will be two globals, one for all the
// links OUT of a page, the other for links TO page.

// Call.h defines these Link cross-referencing list structures
// In CLinAnchorHeads, UrlIndex is to the target or tail URL.
// In CLinAnchorTails, UrlIndex is to the referer, head, URL.
//
// typedef struct _UiAti
// {
//     size_t UrlIndex;
//     size_t AnchorTextIndex;
// } UiAti;
//
// typedef struct _LinkXRef
// {
//     size_t nMallocSlots;
//     size_t nFilledSlots;
//     UiAti Slot[1];
// } LinkXRef;

CLin::CLin( )
{
    #if DO_DEBUG_CALLS
        Routine( L"504" );
    #endif
    LinEnterCrisis( );
    List = NULL; // Just until the reset, then will hold 3 slots
    Reset( );
    LinLeaveCrisis( );
    return;
}

void CLin::Reset( )
{
    #if DO_DEBUG_CALLS
        Routine( L"505" );
    #endif
    LinEnterCrisis( );
    // CLin parallels the construction and use of CSol
    if( List != NULL )
        FreeMallocs( );

    List = NULL;
    // Since a CLin parallels a CSol, [0] and [1] are never used.
    ListMallocCount = 3; // head[0], tail[1], slot[2], ...
    List = ( LinkXRef ** ) MyMalloc( 54, ListMallocCount * sizeof( LinkXRef * ) );

    List[TAIL] = NULL;
    List[HEAD] = NULL;
    #if DO_DEBUG_XREF
        ; SpewValue( L"Zeroing ", 2 );
    #endif
    List[2] = NULL; // all new mallocs must be NULL'ed

    LinLeaveCrisis( );
    return;
}

CLin::~CLin( )
{
    #if DO_DEBUG_CALLS
        Routine( L"506" );
    #endif
    LinEnterCrisis( );
    if( List != NULL )
        FreeMallocs( );
    LinLeaveCrisis( );
    return;
}

void CLin::FreeMallocs( )
{
    #if DO_DEBUG_CALLS
        Routine( L"508" );
    #endif
    // private function
    // No Crisis needed.
    // List is not NULL now.
    size_t i = 0;
    for( ;; )
    {
        if( i == ListMallocCount )
            break;
        if( List[i] != NULL )
        {
            // Getting cranky in my old age. Do this idiom a lot:
            LinkXRef * Temp = List[i]; // copy before delete
            List[i] = NULL;         // NULL before delete
            MyFree( 148, zx, Temp ); // a struct, not an object/delete...
            Temp = NULL; // obligatory rigor after delete
        }
        i ++;
    }
    MyFree( 843, zx, List );
    List = NULL;
    // No Crisis needed.
    return;
}

int CLin::EnsureListIsThisHigh( size_t index )
{
    #if DO_DEBUG_CALLS
        Routine( L"507" );
    #endif
    // private function
    // No Crisis needed.

    if( index < 2 )
        InvalidIndex( L"EnsureListIsThisHigh", index );

    #if DO_DEBUG_XREF
        ; SpewValue( L"Ensure: index", index );
        ; SpewValue( L"ListMallocCount", ListMallocCount );
    #endif

    if( index >= ListMallocCount )
    {
        // we need to expand this list before adding new member.

        size_t WasLmc = ListMallocCount;

        size_t nNeed = index + 1 + 10;
        nNeed += ( nNeed / 8 ); // grow liberally 125 %

        #if DO_DEBUG_XREF
            ; SpewValue( L"realloc List WasLmc", WasLmc );
            ; SpewValue( L"realloc List nNeed", nNeed );
        #endif

        void * pvm = MyRealloc( 9990, WasLmc * sizeof( LinkXRef * ), List, nNeed * sizeof( LinkXRef * ) );
        if( pvm == NULL )
        {
            #if DO_DEBUG_XREF
                ; Spew( L"realloc retn null" );
            #endif
            // do not change original List, ListMallocCount
            return 0; // failure - already messaged
        }
        List = ( LinkXRef * * ) pvm;
        ListMallocCount = nNeed;

        // I need to zero all new slots.
        size_t i = WasLmc;
        size_t n = nNeed;
        for( ;; )
        {
            #if DO_DEBUG_XREF
                ; SpewValue( L"Zeroing List", i );
            #endif
            List[ i ] = NULL;
            if( ++i == n )
                break;
        }
    }

    // No Crisis needed.
    return 1; // success
}

LinkXRef * CLin::GetCopyOfLinkXRef( size_t index )
{
    #if DO_DEBUG_CALLS
        Routine( L"510" );
    #endif
    LinEnterCrisis( );

    // GetCopyOfLinkXRef will allow indices larger than the List, return NULL.
    // Many entries in the List may contain NULL, return NULL.
    // If List contains a LinkXRef pointer, malloc and return a copy.

    LinkXRef * retn = NULL;

    if( index < 2 )
        InvalidIndex( L"GetCopyOfLinkXRef", index );

    if( index < ListMallocCount )
    {
        // Oh, I cannot return a pointer to a structure that might
        // be subject to realloc! Make a copy, caller must free it.

        LinkXRef * from = List[ index ];

        // Not wise to copy if NULL.
        if( from != NULL )
        {
            size_t nNeed = from->nMallocSlots;
            size_t nBytesNeed = sizeof( LinkXRef ) + ( nNeed - 1 ) * sizeof( UiAti );

            #if DO_DEBUG_XREF
                ; SpewValue( L"Copy XRef nNeed", nNeed );
                ; SpewValue( L"Copy XRef nBytesNeed", nBytesNeed );
            #endif

            LinkXRef * pNew = ( LinkXRef * ) MyMalloc( 211, nBytesNeed );
            if( pNew != NULL )
            {
                memcpy( pNew, from, nBytesNeed );
            }
            retn = pNew;
        }
    }

    LinLeaveCrisis( );
    return retn;
}

void CLin::InvalidIndex( wchar_t * szCaption, size_t index )
{
    #if DO_DEBUG_CALLS
        Routine( L"509" );
    #endif
    // private function
    // No Crisis needed.
    static wchar_t wk[100]; // consciously using this static in a thread.
    wsprintf( wk, L"%s: index %d < 2 or >= ListMallocCount %d", szCaption, index, ListMallocCount );
    SetLastError( ERROR_SUCCESS );
    ProgramError( wk );
    // No Crisis needed.
    return;
}

#if DO_DEBUG_XREF
void DamnIAmDumbDump( wchar_t * szLegend, CLin * pLin )
{
    // What I finally found, and I have fixed it, was this error:
    // an attempt to update member inside realloc via old pointer:
    // Oh, the devil was in some details...
    // WRONG! pXRef->nMallocSlots = nNeed;
    // RIGHT: pNew->nMallocSlots = nNeed;
    // I am not used to keeping the count INSIDE the malloc.


    // Finally, a big fat clue: Filled > Malloc!
    // ________ DamnIAmDumbDump: CLinAnchorHeads
    // List address: 14621032
    // ListMallocCount: 3
    // 00000: 0x00000000:
    // 00001: 0x00000000:
    // 00002: 0x02f42fd8:
    //     XRef nMallocSlots: 1
    //     XRef nFilledSlots: 2
    //     Slot    0:     3,     2
    //     Slot    1:     4,     2
    //

    ; SpewTwo( L"________ DamnIAmDumbDump", szLegend );
    ; SpewValue( L"List address", ( int ) pLin->List );
    ; SpewValue( L"ListMallocCount", pLin->ListMallocCount );
    int i = 0;
    int n = pLin->ListMallocCount;
    for( ;; )
    {
        if( i >= n )
            break;
        LinkXRef * pXRef = pLin->List[ i ];
        wchar_t wk[80];
        wsprintf( wk, L"%05d: 0x%08x:", i, pXRef );
        ; Spew( wk );

        if( pXRef != NULL )
        {
            ; SpewValue( L"    XRef nMallocSlots", pXRef->nMallocSlots );
            ; SpewValue( L"    XRef nFilledSlots", pXRef->nFilledSlots );
            int j = 0;
            int m = pXRef->nFilledSlots;
            for( ;; )
            {
                if( j >= m )
                    break;
                UiAti * pPair = pXRef->Slot + j;

                wchar_t wk2[80];
                wsprintf( wk2, L"    Slot %4d: %5d, %5d",
                    j,
                    pPair->UrlIndex,
                    pPair->AnchorTextIndex );
                ; Spew( wk2 );

                j++;
            }
        }
        i++;
    }
}
#endif

void AddAnchorHeadTailText( size_t iHead, size_t iTail, size_t iAnchorText )
{
    #if DO_DEBUG_CALLS
        Routine( L"510" );
    #endif

    if( iHead < 2
    || iTail < 2 )
        return; // I'll deal with you later...

    LinEnterCrisis( );

    // This routine can't be a member of the CLin class.
    // However, I think it should work in the CLin Crix.
    // It needs to be a global to access 2 global CLins:
    // extern CLin CLinAnchorHeads;
    // extern CLin CLinAnchorTails;

    // In CLinAnchorHeads, Slot[].UrlIndex is to the target or tail URL.
    // In CLinAnchorTails, Slot[].UrlIndex is to the referer, head, URL.

    // 1. Add a slot for iHead in CLinAnchorHeads, deal with any NULL.
    // -- In CLinAnchorHeads[iHead] LinkXRef, add iTail + iAnchorText.

    // 2. Add a slot for iTail in CLinAnchorTails, deal with any NULL.
    // -- In CLinAnchorTails[iTail] LinkXRef, add iHead + iAnchorText.

    // My first attempt at creating List or LinkRef is mangling memory.
    // Perhaps the single size_t followed by Slot[] has packing issues.
    // Change to a separate malloc and filled count, which pads struct.

    // Perhaps I foo the name1/name2 in pair of reversed code blocks.
    // Make the code clearer by using these aliases.

    #if DO_DEBUG_XREF
        ; Spew( L"" );
        ; Spew( L"============ new Add ============" );
        ; SpewValue( L"Add iHead", iHead );
        ; SpewValue( L"Add iTail", iTail );
        ; SpewValue( L"Add iAnchorText", iAnchorText );
    #endif

    CLin * pLin = & CLinAnchorHeads;
    size_t iSuch = iHead;
    size_t iOther = iTail;

    // Now do it once, using those aliases.

    if( pLin->EnsureListIsThisHigh( iSuch ) )
    {
        LinkXRef * pXRef = pLin->List[ iSuch ];

        if( pXRef == NULL )
        {
            // This is a new index. Hang a new LinkXRef structure with 1 slot.

            // The LinkXRef struct includes a single Slots[0] element.
            #if DO_DEBUG_XREF
                ; SpewValue( L"Setting new malloc in List", iSuch );
            #endif

            #if DO_DEBUG_XREF
                ; SpewValue( L"new malloc/filled count", 1 );
                ; SpewValue( L"new malloc sizeof( LinkXRef )", sizeof( LinkXRef ) );
            #endif

            pXRef = ( LinkXRef * ) MyMalloc( 217, sizeof( LinkXRef ) );
            if( pXRef == NULL )
            {
                #if DO_DEBUG_XREF
                    ; Spew( L"new malloc retn null" );
                #endif
            }
            else
            {
                #if DO_DEBUG_XREF
                    ; SpewValue( L"Filling slot no.", 0 );
                    ; SpewValue( L"with iOther", iOther );
                    ; SpewValue( L"iAnchorText", iAnchorText );
                #endif

                pXRef->Slot[0].UrlIndex = iOther;
                pXRef->Slot[0].AnchorTextIndex = iAnchorText;
                // In which case, no search/extend is necessary...
                pLin->List[ iSuch ] = pXRef; // No longer a NULL slot.

                pXRef->nMallocSlots = 1;
                pXRef->nFilledSlots = 1;
            }
        }
        else
        {
            // There is an existing non-NULL pointer at iSuch.

            // Scan LinkXRef slots to know if pair are listed.
            int found = 0;
            size_t i = 0;
            size_t n = pXRef->nFilledSlots;
            for( ;; )
            {
                if( pXRef->Slot[i].UrlIndex == iOther
                &&  pXRef->Slot[i].AnchorTextIndex == iAnchorText )
                {
                    found = 1;
                    break;
                }

                if( ++i == n )
                    break;
            }

            if( found )
            {
                #if DO_DEBUG_XREF
                    ; SpewValue( L"Pair found in LinkXref Slot", i );
                #endif
            }
            else
            {
                // This pair is not yet listed. Add pair into LinkXRef.
                #if DO_DEBUG_XREF
                    ; Spew( L"Pair not found in LinkXref Slot" );
                #endif

                if( pXRef->nMallocSlots == pXRef->nFilledSlots )
                {
                    // A little bigger please.
                    // Since re-alloc will change pointer, fix List[iSuch].

                    size_t nWas = pXRef->nMallocSlots;
                    size_t nNeed = pXRef->nMallocSlots + 1;
                    nNeed += nNeed / 8;
                    size_t nBytesWas = sizeof( LinkXRef ) + ( nWas - 1 ) * sizeof( UiAti );
                    size_t nBytesNeed = sizeof( LinkXRef ) + ( nNeed - 1 ) * sizeof( UiAti );

                    #if DO_DEBUG_XREF
                        ; SpewValue( L"realloc XRef nWas", nWas );
                        ; SpewValue( L"realloc XRef nBytesWas", nBytesWas );
                        ; SpewValue( L"realloc XRef nNeed", nNeed );
                        ; SpewValue( L"realloc XRef nBytesNeed", nBytesNeed );
                    #endif

                    LinkXRef * pNew = ( LinkXRef * ) MyRealloc( 9989, nBytesWas, pXRef, nBytesNeed );
                    if( pNew == NULL )
                    {
                        #if DO_DEBUG_XREF
                            ; Spew( L"realloc retn null" );
                        #endif
                        // do not change anything
                        LinLeaveCrisis( );
                        return; // failure - already messaged
                    }

                    // Oh, the devil was in some details...
                    // WRONG!! pXRef->nMallocSlots = nNeed;
                    pNew->nMallocSlots = nNeed;

                    // I am not used to keeping the count INSIDE the malloc.

                    // Soon I will set my local pXRef,
                    // and everybody else will get by.

                    #if DO_DEBUG_XREF
                        ; SpewValue( L"Setting realloc in List", i );
                    #endif
                    pLin->List[iSuch] = pXRef = pNew; // Update pointer in vector
                }

                #if DO_DEBUG_XREF
                    ; SpewValue( L"Filling slot no.", pXRef->nFilledSlots );
                    ; SpewValue( L"with iOther", iOther );
                    ; SpewValue( L"iAnchorText", iAnchorText );
                #endif

                pXRef->Slot[pXRef->nFilledSlots].UrlIndex = iOther;
                pXRef->Slot[pXRef->nFilledSlots].AnchorTextIndex = iAnchorText;
                pXRef->nFilledSlots ++;
            }
        }
    }



    pLin = & CLinAnchorTails;
    iSuch = iTail;
    iOther = iHead;

    // Now do it again, using those aliases.


    if( pLin->EnsureListIsThisHigh( iSuch ) )
    {
        LinkXRef * pXRef = pLin->List[ iSuch ];

        if( pXRef == NULL )
        {
            // This is a new index. Hang a new LinkXRef structure with 1 slot.

            // The LinkXRef struct includes a single Slots[0] element.
            #if DO_DEBUG_XREF
                ; SpewValue( L"Setting new malloc in List", iSuch );
            #endif

            #if DO_DEBUG_XREF
                ; SpewValue( L"new malloc/filled count", 1 );
                ; SpewValue( L"new malloc sizeof( LinkXRef )", sizeof( LinkXRef ) );
            #endif

            pXRef = ( LinkXRef * ) MyMalloc( 217, sizeof( LinkXRef ) );
            if( pXRef == NULL )
            {
                #if DO_DEBUG_XREF
                    ; Spew( L"new malloc retn null" );
                #endif
            }
            else
            {
                #if DO_DEBUG_XREF
                    ; SpewValue( L"Filling slot no.", 0 );
                    ; SpewValue( L"with iOther", iOther );
                    ; SpewValue( L"iAnchorText", iAnchorText );
                #endif

                pXRef->Slot[0].UrlIndex = iOther;
                pXRef->Slot[0].AnchorTextIndex = iAnchorText;
                // In which case, no search/extend is necessary...
                pLin->List[ iSuch ] = pXRef; // No longer a NULL slot.

                pXRef->nMallocSlots = 1;
                pXRef->nFilledSlots = 1;
            }
        }
        else
        {
            // There is an existing non-NULL pointer at iSuch.

            // Scan LinkXRef slots to know if pair are listed.
            int found = 0;
            size_t i = 0;
            size_t n = pXRef->nFilledSlots;
            for( ;; )
            {
                if( pXRef->Slot[i].UrlIndex == iOther
                &&  pXRef->Slot[i].AnchorTextIndex == iAnchorText )
                {
                    found = 1;
                    break;
                }

                if( ++i == n )
                    break;
            }

            if( found )
            {
                #if DO_DEBUG_XREF
                    ; SpewValue( L"Pair found in LinkXref Slot", i );
                #endif
            }
            else
            {
                // This pair is not yet listed. Add pair into LinkXRef.
                #if DO_DEBUG_XREF
                    ; Spew( L"Pair not found in LinkXref Slot" );
                #endif

                if( pXRef->nMallocSlots == pXRef->nFilledSlots )
                {
                    // A little bigger please.
                    // Since re-alloc will change pointer, fix List[iSuch].

                    size_t nWas = pXRef->nMallocSlots;
                    size_t nNeed = pXRef->nMallocSlots + 1;
                    nNeed += nNeed / 8;
                    size_t nBytesWas = sizeof( LinkXRef ) + ( nWas - 1 ) * sizeof( UiAti );
                    size_t nBytesNeed = sizeof( LinkXRef ) + ( nNeed - 1 ) * sizeof( UiAti );

                    #if DO_DEBUG_XREF
                        ; SpewValue( L"realloc XRef nWas", nWas );
                        ; SpewValue( L"realloc XRef nBytesWas", nBytesWas );
                        ; SpewValue( L"realloc XRef nNeed", nNeed );
                        ; SpewValue( L"realloc XRef nBytesNeed", nBytesNeed );
                    #endif

                    LinkXRef * pNew = ( LinkXRef * ) MyRealloc( 9989, nBytesWas, pXRef, nBytesNeed );
                    if( pNew == NULL )
                    {
                        #if DO_DEBUG_XREF
                            ; Spew( L"realloc retn null" );
                        #endif
                        // do not change anything
                        LinLeaveCrisis( );
                        return; // failure - already messaged
                    }

                    // Oh, the devil was in some details...
                    // WRONG!! pXRef->nMallocSlots = nNeed;
                    pNew->nMallocSlots = nNeed;

                    // I am not used to keeping the count INSIDE the malloc.

                    // Soon I will set my local pXRef,
                    // and everybody else will get by.

                    #if DO_DEBUG_XREF
                        ; SpewValue( L"Setting realloc in List", i );
                    #endif
                    pLin->List[iSuch] = pXRef = pNew; // Update pointer in vector
                }

                #if DO_DEBUG_XREF
                    ; SpewValue( L"Filling slot no.", pXRef->nFilledSlots );
                    ; SpewValue( L"with iOther", iOther );
                    ; SpewValue( L"iAnchorText", iAnchorText );
                #endif

                pXRef->Slot[pXRef->nFilledSlots].UrlIndex = iOther;
                pXRef->Slot[pXRef->nFilledSlots].AnchorTextIndex = iAnchorText;
                pXRef->nFilledSlots ++;
            }
        }
    }

    #if DO_DEBUG_XREF
        DamnIAmDumbDump( L"CLinAnchorHeads", & CLinAnchorHeads );
        DamnIAmDumbDump( L"CLinAnchorTails", & CLinAnchorTails );
    #endif

    LinLeaveCrisis( );
}

void SaveListOfAllUrls( )
{
    #if DO_DEBUG_CALLS
        Routine( L"484" );
    #endif

    // Clone some file-save dialog work:

    wchar_t szFile[ MAX_PATH + 1 ]; // Win Help says call will add null
    szFile[0] = NULL; // Initial null means do not recommend a filename
    wcscpy( szFile, L"AllUrls.txt" );

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrTitle = L"Save List of All URLs";
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_NOREADONLYRETURN|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"txt"; // let system add the single default ( . )txt

    if ( ! GetSaveFileName( & ofn ) )
    {
#ifndef _WIN32_WCE
        if( CommDlgExtendedError( ) ) // zero if user canceled
            ProgramError( L"GetSaveFileName" );
#endif // not _WIN32_WCE

        return;
    }

    // Win CE returns a bizarre string containing the desired filename
    // followed by a semicolon, and the filter string; Fix that string:
    // I see now. Win CE appended my whole string from ofn.lpstrFilter.
    // That is no problem here, as I do not have a *.htm;*.html option.


    // Rewriting this now that I have link cross referencing info.
    // planned output file format:

    // Introduce a header line for each block during output loop:

    // 7 First, all non-query fetched pages
    // 6 then, all undesirable urls ( add a CLin with mime type, size? )
    // 5 then, all query result fetched pages
    // 4 then, all not found urls
    // 3 then, all redirection urls
    // 2 then, all query reject urls
    // 1 then, all untried/claiming urls
    // First loop puts their text into 7 CSols for final assy, save.
    // Better: 1 CSol with a grouping prefix digit on each line.

    // format of fetched pages:
    // ( prefix #referers + #score to sort pages, but do not output )
    // <any grep/sort char prefix> szUrl
    // 5-line annotation from paper.
    // From 5 search engines: 101, 102, 103, etc...
    // Anchor Text...
    // From szUrl...
    // From szUrl...
    // Anchor Text... ( sort anchor texts as if sentence scoring )
    // From szUrl...
    // From szUrl...

    // format of other urls:
    // ( prefix #referers + sum anchor text score to sort, do not output )
    // <any grep/sort char prefix> szUrl
    // ( for undesirables, add a mime type / file size line... )
    // ( for redirect/refresh, add another line: To szUrl... )
    // Anchor Text...
    // From szUrl...
    // From szUrl...
    // Anchor Text... ( sort anchor texts as if sentence scoring )
    // From szUrl...
    // From szUrl...

    // the char prefix on URL line is:
    // ( BYTE * ) "*" ); // Ordinary, fetched, pages
    // ( BYTE * ) "+" ); // UNDESIRABLE
    // ( BYTE * ) "_" ); // QUERY RESULT fetched pages
    // ( BYTE * ) "?" ); // NOT_FOUND
    // ( BYTE * ) "^" ); // REDIRECTION
    // ( BYTE * ) "~" ); // QUERYREJECT
    // no prefix for: case ( long ) PVOID_UNTRIED
    // no prefix for: case ( long ) PVOID_CLAIMING

    // So, what sort of sort key format shall we establish?
    // A fixed number of sorting ( bkwds ) characters to skip over:
    // 1 group prefix digit ( 9 to 1 )
    // 2 count of referrers ( clamp at 99 )
    // 5 score if fetched ( clamp at 99999 ) / other subsort criteria
    // 9 digit URL index ( no real possibility to exceed, right? )
    // Then the complete block of text to output, including newlines:
    // any prefix character
    // szUrl, newline
    // rest of block per group type

    // Sorting works in wchar_t, so build each entry text in one Wsb.
    // Because annotations are wide, output will have to be wide too.
    // A user desiring ANSI can open file in Notepad, and re-save it.

    CSol CSolTextBlocks( CSOL_SCALAR ); // goes out of scope to delete

    // This block will run through CSolAllUrls.
    {
        CoIt * pMalUrlVector = CSolAllUrls.GetSortedVector( CSOL_FORWARD );
        if( pMalUrlVector == NULL )
            return;
        size_t take = 0;
        for( ;; )
        {
            CoIt * pCoIt = pMalUrlVector + take++;
            if( pCoIt->IsSentinel )
                break;

            wchar_t EntryPrefix[ 17 + 1 + 1 ];
            wcscpy( EntryPrefix, L"00000000000000000*" ); // 17 zeroes, prefix, NULL


            // 1. Get the full key.
            // 2. Get the CSol index.
            // 3. Consult User.pVoid.

            // I notice that the CoIt structure does not contain the
            // index into the original CSol List. Perhaps it should.
            // Meanwhile, I will have to use Find to get that index.
            // No, fgrep does not show Find after CoItFullKey often.


            // 1. Get the full key.
            wchar_t * szUrlFullKey = CoItFullKey( pCoIt );

            // 2. Get the CSol index.
            size_t UrlIndex = CSolAllUrls.Find( szUrlFullKey );

            // It's not possible for this not to be found, is it?
            // Yeah, when I clone code leaving a wrong CSol name.

            #if DO_DEBUG_ADDFIND
                if( UrlIndex <= 1 )
                    { Spew( L"AddFind <= 1 at clin 764" ); }
            #endif

            if( UrlIndex > 999999999 )
            {
                MyFree( 725, UNPREDICTABLE, szUrlFullKey );
                continue; // CYA
            }


            // 3. Consult User.pVoid.
            COnePaper * pOnePaper = ( COnePaper * ) pCoIt->User.pVoid;
            if( pOnePaper != NULL
            &&  pOnePaper < PVOID_VALID_BELOW )
            {
                // This URL has a paper hanging.

                int score = 0;

                if( pOnePaper->PageIsAQrpThisIsItsOrdinal != ZERO_ORDINAL_FOR_NON_QRP ) // bool: in export of urls ranking
                {
                    // Query result pages
                    EntryPrefix[0] = '5'; // all query result fetched pages
                    EntryPrefix[17] = '_'; // QUERY RESULT fetched pages
                    // Their score is always zero.--Oh!
                    // I could use outbound link count.
                    // Although, that could be to spam.
                    // Let's try this: Is negative safe?
                    // No, but 1000 - ordinal 1 to 999 is.

                    int BoolsEngineNo = pOnePaper->PageIsAQrpThisIsItsOrdinal; // two bools and ordinal
                    int PlainEngineNo = BoolsEngineNo & ~ BIT_IN_ORDINAL_TO_KEEP;
                    score = 1000 - PlainEngineNo;
                }
                else
                {
                    // Ordinary pages
                    EntryPrefix[0] = '7'; // all non-query fetched pages
                    EntryPrefix[17] = '*'; // Ordinary, fetched, pages
                    score = pOnePaper->PageRankingScore;
                }

                if( score > 99999 )
                    score = 99999;

                // I am going to reverse the all-urls sorting order:
                // put the SCORE in EntryPrefix[ 1 to 5 ], 99999 max
                // put REFERRERS in EntryPrefix[ 6 to 7 ], 99 max

                EntryPrefix[5] = '0' + score % 10; score /= 10;
                EntryPrefix[4] = '0' + score % 10; score /= 10;
                EntryPrefix[3] = '0' + score % 10; score /= 10;
                EntryPrefix[2] = '0' + score % 10; score /= 10;
                EntryPrefix[1] = '0' + score % 10; // MSB here

            }
            else
            {
                // This URL has no paper hanging.
                // one of the special marker values:
                switch( ( long ) pOnePaper )
                {
                case ( long ) PVOID_UNTRIED:
                case ( long ) PVOID_CLAIMING:
                    EntryPrefix[0] = '1'; // all untried/claiming urls
                    EntryPrefix[17] = NULL; // PVOID_UNTRIED, PVOID_CLAIMING
                    break;

                case ( long ) PVOID_NOTFOUNDETC:
                    EntryPrefix[0] = '4'; // all not found urls
                    EntryPrefix[17] = '?'; // NOT_FOUND
                    break;

                case ( long ) PVOID_REDIRECTION:
                    EntryPrefix[0] = '3'; // all redirection urls
                    EntryPrefix[17] = '^'; // REDIRECTION
                    break;

                case ( long ) PVOID_UNDESIRABLE:
                    EntryPrefix[0] = '6'; // all undesirable urls ( add a CLin with mime type, size? )
                    EntryPrefix[17] = '+'; // UNDESIRABLE
                    break;

                case ( long ) PVOID_QUERYREJECT:
                    EntryPrefix[0] = '2'; // all query reject urls
                    EntryPrefix[17] = '~'; // QUERYREJECT
                    break;

                }
            }

            // Everybody gets a referrer count.

            int ReferrerCount = 0;

            // This URL may have a NULL or non-NULL
            // entry in CLinAnchorTails; if non-NULL,
            // get the LinkXRef pointer, get count.
            // Use a CLin class routine under Crix.

            // If GetCopyOfLinkXRef retunr is non-NULL,
            // it is a malloc copy, I must free it.
            // I will need this later, to annotate referrers.

            LinkXRef * pXRef = CLinAnchorTails.GetCopyOfLinkXRef( UrlIndex );
            if( pXRef != NULL )
            {
                ReferrerCount = pXRef->nFilledSlots;
            }


            // I am going to reverse the all-urls sorting order:
            // put the SCORE in EntryPrefix[ 1 to 5 ], 99999 max
            // put REFERRERS in EntryPrefix[ 6 to 7 ], 99 max

            if( ReferrerCount > 99 )
                ReferrerCount = 99;
            EntryPrefix[7] = '0' + ReferrerCount / 10;
            EntryPrefix[6] = '0' + ReferrerCount % 10; // MSB here


            // Everybody gets a URL index no.

            int n = UrlIndex; // format nine digits:

            EntryPrefix[16] = '0' + n % 10; n /= 10;
            EntryPrefix[15] = '0' + n % 10; n /= 10;
            EntryPrefix[14] = '0' + n % 10; n /= 10;

            EntryPrefix[13] = '0' + n % 10; n /= 10;
            EntryPrefix[12] = '0' + n % 10; n /= 10;
            EntryPrefix[11] = '0' + n % 10; n /= 10;

            EntryPrefix[10] = '0' + n % 10; n /= 10;
            EntryPrefix[9] = '0' + n % 10; n /= 10;
            EntryPrefix[8] = '0' + n % 10;


            // Build up the sortable text entry, add entry to CSol.

            CWsb Build; // goes out of scope to delete
            Build.Add( EntryPrefix );

            if( pOnePaper != NULL
            &&  pOnePaper < PVOID_VALID_BELOW )
            {
                // This URL has a paper hanging.
                // Header starts with the URL line
                wchar_t * pMalTop = Pag.GetWordsExHeader( UrlIndex ); // malloc to free.
                Build.Add(  pMalTop ); // which includes just one final newline
                MyFree( 910, UNPREDICTABLE, pMalTop );

                // Now append a line about the FROM SEARCH ENGINES... wip

                // If this URL was found by search engines, list them next:

                if( pOnePaper->pIntFromEngine != NULL
                && pOnePaper->pIntFromEngine->nList > 0 )
                {
                    size_t nMalNos = 0;
                    int * pMalNos = pOnePaper->pIntFromEngine->GetList( & nMalNos );

                    wchar_t wk[80];
                    wsprintf( wk, L" From %3d search engines:", nMalNos );
                    Build.Add( wk );

                    size_t i = 0;
                    for( ;; )
                    {
                        if( i == nMalNos )
                            break;
                        int Value = pMalNos[ i ];

                        wsprintf( wk, L" %03d", Value );
                        Build.Add( wk );
                        i ++;
                    }
                    Build.Add( L"\r\n" );

                    MyFree( 941, UNPREDICTABLE, pMalNos );
                    pMalNos = NULL;
                }

            }
            else
            {
                // This URL has no paper hanging.
                // Start with the URL line
                Build.Add(  szUrlFullKey );
                Build.Add(  L"\r\n" );
            }

            // Now add referrer information.

            // I don't need this explicit line once I append referrers:
            // {
            //     wchar_t wk[60];
            //     wsprintf( wk, L"Referrers: %5d\r\n", ReferrerCount );
            //     Build.Add(  wk );
            // }

            if( pXRef != NULL )
            {
                // Run through all the slots.
                // Make a CSol to sort the unique anchor text
                // by their valuation scores ( as if sentences ).
                // For each anchor text, show with 'from' URL( s ).

                // For now, just iterate list showing all items...
                // That's not so bad, but break it into 2 phases.
                // In phase1, list all internal links ( w/o self url )
                // In phase2, list all external URLs and anchor texts

                // Phase 1
                int i = 0;
                int n = pXRef->nFilledSlots;
                int FirstTime = 1;
                for( ;; )
                {
                    if( i == n )
                        break;

                    size_t AnchorTextIndex = pXRef->Slot[i].AnchorTextIndex;
                    size_t HeadUrlIndex = pXRef->Slot[i].UrlIndex;

                    if( HeadUrlIndex == UrlIndex )
                    {

                        wchar_t * pMalAnchorText = CSolAnchorText.GetFullKey( AnchorTextIndex ); // a malloc, user frees

                        // Like I used other prefixes on http to prevent recognition
                        // Let the referrer head have a "<" prefix ( as in FROM ).
                        // Or, do I want to block recognition? Not really.
                        // And put the anchor text in double quotes.
                        //
                        // |yadayada... fetched summary or bare URL
                        // | < http://url1
                        // |   "saying this"
                        // | < http://url2
                        // |   "saying that"
                        if( FirstTime )
                        {
                            FirstTime = 0;
                            Build.Add( L" < from within this same page:\r\n" );
                        }
                        Build.Add( L"   \"" );
                        Build.Add( pMalAnchorText );
                        Build.Add( L"\"\r\n" );

                        MyFree( 953, UNPREDICTABLE, pMalAnchorText );
                    }

                    i++;
                }

                // Phase 2
                i = 0;
                n = pXRef->nFilledSlots;
                for( ;; )
                {
                    if( i == n )
                        break;

                    size_t AnchorTextIndex = pXRef->Slot[i].AnchorTextIndex;
                    size_t HeadUrlIndex = pXRef->Slot[i].UrlIndex;

                    if( HeadUrlIndex != UrlIndex )
                    {
                        wchar_t * pMalAnchorText = CSolAnchorText.GetFullKey( AnchorTextIndex ); // a malloc, user frees
                        wchar_t * pMalHeadUrl = CSolAllUrls.GetFullKey( HeadUrlIndex ); // a malloc, user frees

                        // Like I used other prefixes on http to prevent recognition
                        // Let the referrer head have a "<" prefix ( as in FROM ).
                        // Or, do I want to block recognition? Not really.
                        // And put the anchor text in double quotes.
                        //
                        // |yadayada... fetched summary or bare URL
                        // | < http://url1
                        // |   "saying this"
                        // | < http://url2
                        // |   "saying that"
                        Build.Add( L" < " );
                        Build.Add( pMalHeadUrl );
                        Build.Add( L"\r\n   \"" );
                        Build.Add( pMalAnchorText );
                        Build.Add( L"\"\r\n" );

                        MyFree( 953, UNPREDICTABLE, pMalAnchorText );
                        MyFree( 954, UNPREDICTABLE, pMalHeadUrl );
                    }

                    i++;
                }
            }


            // End each entry with one blank separator line:
            Build.Add(  L"\r\n" );

            size_t nMallocBuf = 0;
            wchar_t * pMalBuf = Build.GetBuffer( & nMallocBuf ); // a malloc, user frees
            if( pMalBuf != NULL )
                CSolTextBlocks.AddKey( pMalBuf );
            MyFree( 898, UNPREDICTABLE, pMalBuf );

            if( pXRef != NULL )
            {
                MyFree( 875, UNPREDICTABLE, pXRef );
            }

            MyFree( 877, UNPREDICTABLE, szUrlFullKey );
        }
        MyFree( 1613, UNPREDICTABLE, pMalUrlVector );
        pMalUrlVector = NULL;
    }


    // Having processed all URLs, run the CSol to generate output.

    wchar_t PriorLineType = NULL;

    // I need to create one wide text block for an easy file save:
    CWsb WsbAccum; // goes out of scope to delete.

    // Because wchar_t are held as Intel lsb,msb shorts,
    // Prefix a UNICODE LSB-FIRST identifier prefix.
    // Unicode as saved by notepad: Unicode ( LSB-first ):
    // 000000:  FF FE 68 00 74 00 74 00 70 00 3A 00 2F 00 2F 00   ..h.t.t.p.:././.

    // static unsigned char UnicodePrefix[3] = { 0xff, 0xfe, 0 };
    static wchar_t UnicodePrefix[2] = { 0xfeff, 0 };
    WsbAccum.Add( UnicodePrefix );

    // This block will run through CSolTextBlocks.
    {
        CoIt * pMalAssyVector = CSolTextBlocks.GetSortedVector( CSOL_BACKWARD );
        if( pMalAssyVector == NULL )
            return;
        size_t take = 0;
        for( ;; )
        {
            CoIt * pCoIt = pMalAssyVector + take++;
            if( pCoIt->IsSentinel )
                break;

            wchar_t * pMalEntryFullKey = CoItFullKey( pCoIt );

            wchar_t LineType = pMalEntryFullKey[0];
            if( PriorLineType != LineType )
            {
                PriorLineType = LineType;
                switch( LineType )
                {
                    case '7':
                        WsbAccum.Add( L"\r\n ======= Group 1: Good URLs in memory\r\n\r\n" );
                        break;
                    case '6':
                        WsbAccum.Add( L"\r\n ======= Group 2: Non-text/html URLs\r\n\r\n" );
                        break;
                    case '5':
                        WsbAccum.Add( L"\r\n ======= Group 3: Query results URLs\r\n\r\n" );
                        break;
                    case '4':
                        WsbAccum.Add( L"\r\n ======= Group 4: Not found etc. URLs\r\n\r\n" );
                        break;
                    case '3':
                        WsbAccum.Add( L"\r\n ======= Group 5: Redirection URLs\r\n\r\n" );
                        break;
                    case '2':
                        WsbAccum.Add( L"\r\n ======= Group 6: Query rejected hit URLs\r\n\r\n" );
                        break;
                    case '1':
                        WsbAccum.Add( L"\r\n ======= Group 7: Unfetched new URLs\r\n\r\n" );
                        break;
                }
            }
            WsbAccum.Add( pMalEntryFullKey + 17 ); // skip over 17 sorting prefix chars

            // Switch again for variable annotation possibilities:
            // Oh! No. That formatting got done during on pass one.

            MyFree( 1613, UNPREDICTABLE, pMalEntryFullKey );
        }
        MyFree( 1613, UNPREDICTABLE, pMalAssyVector );
        pMalAssyVector = NULL;
    }


    // Now clone the file-saver from somewhere:

    size_t nMallocBuf = 0; // Actually, this is wcslen.
    wchar_t * pMallocBuf = WsbAccum.GetBuffer( & nMallocBuf ); // a malloc, user frees

    if( pMallocBuf == NULL )
        return;

    size_t nNewByteStrLen = 0;
    unsigned char * nNewByteMalloc = ConvertWideToMbs( pMallocBuf, nMallocBuf, & nNewByteStrLen );

    if( nNewByteMalloc != NULL
    && nNewByteStrLen > 0 )
    {

        Fio.CommonFileSave( szFile, nNewByteMalloc, nNewByteStrLen );
        MyFree( 1163, zx, nNewByteMalloc );
    }

    MyFree( 637, zx, pMallocBuf );
    pMallocBuf = NULL;

}

unsigned char * ConvertWideToMbs( wchar_t * pWideBuf, size_t nWideBuf, size_t * pNewByteStrLen )
{
    // Input length is in wide chars, but is null terminated.
    // Output length is in bytes, not counting a final null.
    // Oh-oh. I cannot work in place, as some wchar_t make 3 bytes.
    // So make my own malloc, overly fat, to return with strlen.

    *pNewByteStrLen = 0; // in case of failure

    size_t nFatBytes = ( nWideBuf + 2 ) * 3;
    unsigned char * pFatBytes = ( unsigned char * ) MyMalloc( 1184, nFatBytes );

    if( pFatBytes == NULL )
        return NULL;

    // My only caller so far inserted this prefix,
    // But some future caller may just pass chars.

    wchar_t * take = pWideBuf;
    BYTE * fill = pFatBytes;

    if( take[ 0 ] == 0xfeff     // Unicode LSB-first marker code
    ||  take[ 0 ] == 0xfffe )   // Unicode MSB-first marker code
    {
        take ++;
        // Output an MBS ( UTF-8 ) file marker sequence:
        // static BYTE MBSHeader[3] = { 0xef, 0xbb, 0xbf };
        *fill++ = 0xef;
        *fill++ = 0xbb;
        *fill++ = 0xbf;
    }

    //  I will only handle UCS up to 16 bits, UTF-8 thus:
    //  0x00000000 - 0x0000007F: 0xxxxxxx
    //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
    //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx

    for( ;; )
    {
        wchar_t wc = *take++;
        if( wc == NULL )
            break;

        //  UCS for up to 16 bits as UTF-8 is thus:
        //  0x00000000 - 0x0000007F: 0xxxxxxx
        //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
        //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx

        if( wc > 0x07f )
        {
            if( wc > 0x07ff )
            {
                wchar_t wc2 = wc >> ( 6 + 6 + 0 );
                wc2 &= 0x000f;
                wc2 |= 0x00e0;

                wchar_t wc1 = wc >> ( 6 + 0 );
                wc1 &= 0x003f;
                wc1 |= 0x0080;

                wchar_t wc0 = wc >> ( 0 );
                wc0 &= 0x003f;
                wc0 |= 0x0080;

                *fill++ = ( char ) wc2;
                *fill++ = ( char ) wc1;
                *fill++ = ( char ) wc0;
            }
            else
            {
                wchar_t wc1 = wc >> ( 6 + 0 );
                wc1 &= 0x001f;
                wc1 |= 0x00c0;

                wchar_t wc0 = wc >> ( 0 );
                wc0 &= 0x003f;
                wc0 |= 0x0080;

                *fill++ = ( char ) wc1;
                *fill++ = ( char ) wc0;
            }
        }
        else
        {
            *fill++ = ( char ) wc;
        }
    }
    *fill = NULL;
    *pNewByteStrLen = fill - pFatBytes;
    return pFatBytes;
}


