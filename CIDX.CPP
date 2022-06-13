// This is file: CIdx.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// The "index" to some original text is a vector of INDEXs,
// which each have a start and stop offset in original text,
// and a reference away, by UrlIndex, and an offset to set.

CIdx::CIdx( )
{
    #if DO_DEBUG_CALLS
        Routine( L"174" );
    #endif
    IdxEnterCrisis( );
    Indices = NULL;
    nMal = 0;
    nSlots = nLen = 0;
    IdxLeaveCrisis( );
    return;
}

CIdx::~CIdx( )
{
    #if DO_DEBUG_CALLS
        Routine( L"175" );
    #endif
    IdxEnterCrisis( );
    Reset( );
    IdxLeaveCrisis( );
    return;
}

void CIdx::Reset( )
{
    #if DO_DEBUG_CALLS
        Routine( L"498" );
    #endif
    IdxEnterCrisis( );
    if( Indices != NULL )
    {
        INDEX * temp = Indices;
        Indices = NULL;
        MyFree( 28, nMal * sizeof( INDEX ), temp );
    }
    nMal = 0;
    nSlots = nLen = 0;
    IdxLeaveCrisis( );
}


int CIdx::AddIdx( size_t Atop, size_t Past, size_t Index, size_t Offset )
{
    #if DO_DEBUG_CALLS
        // Routine( L"176" );
    #endif
    IdxEnterCrisis( );
    // Caller is adding some text to a progress log,
    // in which a click should lead to another text.

    if( nLen > 0 )
    {
        // Pgm err in Interleave was due to non-ascending items.
        // Add a check here to guarantee okay, help me find err:
        // IDX SLOT       1: Atop     154 Past     503
        // IDX SLOT       2: Atop     154 Past     505
        // IDX SLOT       3: Atop     809 Past    1158
        // Enforce INequality, strictly ascending.
        // That makes add internals fail 2nd rule:
        // Allow equality of new top with old end.
        // That still fails in sentences which I must re-write.
        // wip - leave out this check until later...
        // if( Past <= Atop )
        // {
        //     ProgramError( L"AddIdx: Past <= Atop" );
        // }
        // if( Atop < Indices[ nLen ] . nPastText )
        // {
        //     ProgramError( L"AddIdx: Atop <= Past[n-1]" );
        // }
    }

    if( nLen == nMal )
    {
        size_t nNeed = nMal + 4 + ( nMal / 8 ); // grow liberally 125 %
        void * pvm = MyRealloc( 9992, nMal * sizeof( INDEX ), Indices, nNeed * sizeof( INDEX ) );
        if( pvm == NULL )
        {
            // do not change original szBuf, nMal
            IdxLeaveCrisis( );
            return 0; // failure - already messaged
        }
        Indices = ( INDEX * ) pvm;
        nMal = nNeed;
    }
    Indices[ nLen ] . nAtopText = Atop;
    Indices[ nLen ] . nPastText = Past;
    Indices[ nLen ] . nSourceIndex = Index;
    Indices[ nLen ] . nSourceOffset = Offset;
    nLen ++;
    nSlots = nLen;
    #if DO_DEBUG_LINKIDX
    {
        wchar_t wk[120];
        wsprintf( wk, L"ADD IDX SLOT %7d: Atop %7d Past %7d -> URL #%7d Offset #%7d.",
            nSlots, Atop, Past, Index, Offset );
        ; Spew( wk );
    }
    #endif
    IdxLeaveCrisis( );
    return 1; // success
}

int CIdx::GetIdx( size_t nLookup, size_t * Atop, size_t * Past, size_t * Index, size_t * Offset )
{
    #if DO_DEBUG_CALLS
        Routine( L"177" );
    #endif
    IdxEnterCrisis( );
    // Caller is looping, to output text with links, in order.
    if( Indices == NULL
    || nLookup >= nLen )
    {
        * Atop = 0x7fffffff; // past text on last caller's loop
        * Past = 0x7fffffff;
        * Index = 0;
        * Offset = 0;
        IdxLeaveCrisis( );
        return 0; // lookup failure, but not a pgm failure
    }
    * Atop =   Indices[ nLookup ] . nAtopText;
    * Past =   Indices[ nLookup ] . nPastText;
    * Index =  Indices[ nLookup ] . nSourceIndex;
    * Offset = Indices[ nLookup ] . nSourceOffset;
    IdxLeaveCrisis( );
    return 1;
}

int CIdx::ScanIdxToFollow( size_t Lookup, size_t * pIndex, size_t * pOffset )
{
    #if DO_DEBUG_CALLS
        Routine( L"178" );
    #endif
    IdxEnterCrisis( );

    // Caller has mouse click offset, wishes to set new text.
    // Here, Lookup is a text offset, not an nLookup slot no.
    // Here, Atop and Past are offsets in the RESULTS PAGE,
    // matching the click, not in the pointed-to source page.
    // This describes a click on an Add Cache/Directory/etc:
    // AND YET BUT--This also describes a click on an Add FIND,
    // because Add Find must ALSO go to correct page and offset!
    // These callers all pass NULL in the last two params.
    // Not quite: They want the offset past too... Wait...
    // They need the offset past the source fragment, but
    // this PAST is the offset past the results entry.

    if( Indices != NULL )
    {
        size_t i = 0;
        for( ;; )
        {
            if( i >= nLen )
                break;
            // If they will be always ordered, I could break earlier?
            if( Indices[ i ] . nAtopText <= Lookup
            &&  Indices[ i ] . nPastText > Lookup )
            {
                // both callers want these variables.
                // To the mouse click, they are new page index and offset.

                * pIndex = Indices[ i ] . nSourceIndex;
                * pOffset = Indices[ i ] . nSourceOffset;

                IdxLeaveCrisis( );
                return 1;
            }
            i ++;
        }
    }
    * pIndex = 0;
    * pOffset = 0;
    IdxLeaveCrisis( );
    return 0;
}

int CIdx::ScanIdxToTestRange( size_t Lookup, size_t * pScore, size_t * pOffset, size_t * pAtopText, size_t * pPastText )
{
    #if DO_DEBUG_CALLS
        Routine( L"179" );
    #endif
    IdxEnterCrisis( );

    // Here IS A DIFFERENT ASPECT of an ADD FIND task, when we
    // have a matched word text offset, and want to know what
    // sentence or block it falls within, to make up results:
    // This caller in CFwd also wants ATOP boundary returned,
    // which will be the source page OFFSET when follow click.
    // This caller in CFwd also wants PAST boundary returned,
    // to continue his full-text-matching scan after the item.
    // If his matched text is not in any block, he ignores it.

    if( Indices != NULL )
    {
        size_t i = 0;
        for( ;; )
        {
            if( i >= nLen )
                break;
            // If they will be always ordered, I could break earlier?
            if( Indices[ i ] . nAtopText <= Lookup
            &&  Indices[ i ] . nPastText > Lookup )
            {
                // Find sentence or block only use index, to hold a score.

                * pScore = Indices[ i ] . nSourceIndex;
                * pOffset = Indices[ i ] . nSourceOffset;

                // New caller who is formulating the Add Find results
                // wants these data also, but old callers passed NULL:
                if( pAtopText != NULL )
                    * pAtopText = Indices[ i ] . nAtopText;
                if( pPastText != NULL )
                    * pPastText = Indices[ i ] . nPastText;

                IdxLeaveCrisis( );
                return 1;
            }
            i ++;
        }
    }
    * pScore = 0;
    * pOffset = 0;
    IdxLeaveCrisis( );
    return 0;
}

void CIdx::IncreaseOffsets( size_t SizeChange )
{
    #if DO_DEBUG_CALLS
        Routine( L"180" );
    #endif
    IdxEnterCrisis( );
    // Caller has added annotation atop text.
    if( Indices != NULL )
    {
        size_t i = 0;
        for( ;; )
        {
            if( i >= nLen )
                break;
            Indices[ i ] . nAtopText += SizeChange;
            Indices[ i ] . nPastText += SizeChange;
            i ++;
        }
    }
    IdxLeaveCrisis( );
    return;
}


int CIdx::ReturnIdxSlotRange( size_t Index, size_t * pScore, size_t * pAtopText, size_t * pPastText )
{
    #if DO_DEBUG_CALLS
        // Routine( L"308" );
    #endif
    IdxEnterCrisis( );

    // This is a variation on the ScanIdxToTestRange routine:
    // Instead of calling ScanIdxToTestRange with an Offset;
    // Call this variant ReturnIdxSlotRange passing an index;
    // Return 0 when caller passes a number past end of list.

    if( Indices != NULL
    && Index < nLen )
    {
        * pScore = Indices[ Index ] . nSourceIndex;
        * pAtopText = Indices[ Index ] . nAtopText;
        * pPastText = Indices[ Index ] . nPastText;

        IdxLeaveCrisis( );
        return 1;
    }
    // I'm debugging Interleave, who did not test my 0 return,
    // so plug in that return huge data fix as used in GetIdx:
    * pScore = 0;
    * pAtopText = 0x7fffffff; // past text on last caller's loop
    * pPastText = 0x7fffffff; // past text on last caller's loop
    IdxLeaveCrisis( );
    return 0;
}

