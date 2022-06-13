// This is file: CSol.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// I first coded Red-Black-Sorted-List separately...
// http://home.earthlink.net/~glenn_scheper/rbsl.cpp

CSol::CSol( int UserType )
{
    #if DO_DEBUG_CALLS
        // Routine( L"239" );
    #endif
    SolEnterCrisis( );
    List = NULL; // Just until the reset, then will hold 3 slots
    MallocTailKeyExtPtr = NULL;
    m_UserType = UserType; // 0 = scalar, 1 = malloc/free, 2 = object/delete
    Reset( );
    SolLeaveCrisis( );
    return;
}

void CSol::Reset( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"240" );
    #endif
    SolEnterCrisis( );
    // Tree is not amenable to single node deletes. This deletes all.
    if( List != NULL )
        FreeMallocs( );

    List = NULL;
    MallocTailKeyExtPtr = NULL;
    nList = ListFilledCount = 0;

    ListMallocCount = 3; // head, tail, ...
    List = ( SoIt * ) MyMalloc( 54, ListMallocCount * sizeof( SoIt ) );
    MallocTailKeyExtSize = 10;
    MallocTailKeyExtPtr = ( wchar_t * ) MyMalloc( 64, MallocTailKeyExtSize * sizeof( wchar_t ) );
    nList = ListFilledCount = 2; // including tail ( #0 ) and head ( #1 )

    // TAIL always holds the trial key to look up.
    memset( List[TAIL].KeyStart, 0, sizeof( List[TAIL].KeyStart ) );
    List[TAIL].szKeyExt = MallocTailKeyExtPtr; // sometimes on, sometimes off
    List[TAIL].szKeyExt[0] = NULL;
    List[TAIL].Red = 0;
    List[TAIL].Lower = TAIL;
    List[TAIL].Higher = TAIL;
    List[TAIL].User.Value = 0;

    // HEAD s/b less than any key; But N.B. it matches the empty string.
    memset( List[HEAD].KeyStart, 0, sizeof( List[HEAD].KeyStart ) );
    List[HEAD].szKeyExt = NULL;
    List[HEAD].Red = 0;
    List[HEAD].Lower = TAIL; // although low side never used.
    List[HEAD].Higher = TAIL; // at the edge of the tree now.
    List[HEAD].User.Value = 0;

    SolLeaveCrisis( );
    return;
}

CSol::~CSol( )
{
    #if DO_DEBUG_CALLS
        Routine( L"241" );
    #endif
    SolEnterCrisis( );
    if( List != NULL )
        FreeMallocs( );
    SolLeaveCrisis( );
    return;
}

size_t CSol::AddKey( wchar_t * InsertKey )
{
    #if DO_DEBUG_CALLS
        // Routine( L"242" );
    #endif
    SolEnterCrisis( );

    if( InsertKey == NULL )
    {
        SetLastError( ERROR_SUCCESS );
        ProgramError( L"AddKey: InsertKey == NULL" );
        SolLeaveCrisis( );
        return 0;
    }
    if( * InsertKey == NULL )
    {
        SetLastError( ERROR_SUCCESS );
        ProgramError( L"AddKey: *InsertKey == NULL" );
        SolLeaveCrisis( );
        return 0;
    }

    size_t x = 0;
    // Passed null terminated wide string will be found, or added to tree.
    // Since all malloc tests are gone, Add, Addn etc, never return 0.
    // Wrong! Now in order to not bail out, make all users test for 0.
    // This work of setting the search key is same in both Add/Find.

    wchar_t * from = InsertKey;
    wchar_t * into = List[TAIL].KeyStart;
    size_t i = 0;
    for( ;; )
    {
        if( ( *into = *from ) != NULL )
            from++;

        into ++;

        if( ++ i == NKEYSTART )
            break;
    }

    size_t LengthOfExtension = 0;

    if( *from == NULL )
    {
        // Null pointer substitutes for a NULL at [NKEYSTRING]
        List[TAIL].szKeyExt = NULL; // No extension is needed.
    }
    else
    {
        // More chars to copy into TAIL extension.
        // Restore non-NULL pointer for extension.

        into = List[TAIL].szKeyExt = MallocTailKeyExtPtr;
        size_t Remains = MallocTailKeyExtSize;

        for( ;; )
        {
            // We always get to copy a NULL into szKeyExt[]:
            if( ( *into ++ = *from ++ ) == NULL )
                break;

            if( -- Remains == 0 )
            {
                // Pause to expand extension before continuing to copy.
                size_t Advanced = into - MallocTailKeyExtPtr;;
                size_t Lacking = wcslen( from ) + 1; // 1 for NULL
                size_t nNeed = MallocTailKeyExtSize + Lacking;
                nNeed += ( nNeed / 8 ); // grow liberally 125 %
                void * pvm = MyRealloc( 9994, MallocTailKeyExtSize * sizeof( wchar_t ), MallocTailKeyExtPtr, nNeed * sizeof( wchar_t ) );
                if( pvm == NULL )
                {
                    // do not change original MallocTailKeyExtPtr, MallocTailKeyExtSize
                    // and abandon this attempt to AddKey( ), return 0.
                    SolLeaveCrisis( );
                    return 0; // failure - already messaged
                }
                MallocTailKeyExtPtr = ( wchar_t * ) pvm;
                MallocTailKeyExtSize = nNeed;
                into = List[TAIL].szKeyExt = MallocTailKeyExtPtr;
                into += Advanced;
            }
        }
        LengthOfExtension = into - MallocTailKeyExtPtr; // including 1 for NULL
    }

    size_t greatgrandfather = HEAD;
    size_t grandfather = HEAD;
    size_t father = HEAD;
    x = HEAD;

    for( ;; )
    {
        // Each search iteration proceeds down the tree one generation.
        // However, a split may set the iterator x up some generations.

        // Original algorithm would have appended an equal key to the right
        // of all existing equal keys. I changed method so equal keys will
        // be discovered in this outermost loop, and the inner subroutines
        // will never encounter a test key that is equal to any tree entry.

        int PlusMinusOrZero = StrcmpTrialValueInTailTo( x );

        if( PlusMinusOrZero == 0 )
        {
            // This is the equality case that will re-use an existing node.
            // Or, match might be to self in TAIL, in which case, add node.
            break;
        }

        // That equality test must occur before these loop re-assignments:
        // Otherwise match to self in TAIL has father=TAIL; All fall down.
        greatgrandfather = grandfather;
        grandfather = father;
        father = x;

        if( PlusMinusOrZero < 0 )
        {
            x = List[x].Lower;
        }
        else
        {
            x = List[x].Higher;
        }

        // At this point, we have chosen/traversed a new x,
        // and the old value of x has become "father" of x.

        // If both children of x are red, then x is a 4-node to be split.
        // Sedgewick calls this test the only "cost" to the inner loop.

        if( List[List[x].Lower].Red
        && List[List[x].Higher].Red )
        {
            // The book _Algorithms_ said the Split routine
            // should access the local variables of Insert,
            // meaning its swaps should be reflected here.
            // Therefore in-line Split into Insert 2 places.

            // x will become red, and both children will become black.
            List[x].Red = 1;
            List[List[x].Lower].Red = 0;
            List[List[x].Higher].Red = 0;

            // If now x and x's father are both red, change that situation.
            // The algorithm depends on no-two-reds in a sequence going down.
            // Therefore, promote the father's redness up to the grandfather.
            // The grandfather MUST have been black before, per principles.

            if( List[father].Red )
            {
                List[grandfather].Red = 1;

                // The original x we tested higher/lower is now the father.
                // Compare the slant of now grandfather to slant of father.

                int LowerThanGrandFather;
                int LowerThanFather;

                if( StrcmpTrialValueInTailTo( grandfather ) <= 0 )
                {
                    LowerThanGrandFather = 1;
                }
                else
                {
                    LowerThanGrandFather = 0;
                }

                if( StrcmpTrialValueInTailTo( father ) <= 0 )
                {
                    LowerThanFather = 1;
                }
                else
                {
                    LowerThanFather = 0;
                }

                // per book, "if ( v < g->Key ) <> ( v < f->Key ), Rotate gf.
                if( LowerThanGrandFather != LowerThanFather )
                {

                    x = Rotate( grandfather );
                }

                // and, in red father, always Rotate ggf:
                x = Rotate( greatgrandfather );
                List[x].Red = 0;

            }
            // ============================================
            // Here ends copy #1 of Split, included inline.
            // ============================================

        }

        List[List[HEAD].Higher].Red = 0; // keep removing any redness at top
    }

    // As expressed in Sedgewick, Insert assumes a prior search failure.
    // Allow for a search success during Insert, whenever x wasn't TAIL.

    if( x == TAIL )
    {
        // We reached this leaf of tree with no match except to tail.
        // Create a new SoIt, using the next slot of the List vector.
        x = ListFilledCount; // next available index/ID number

        size_t HoldOntoTheNewX = x; // Otherwise, next split causes wrong x retn

        if( ListFilledCount == ListMallocCount )
        {
            // we need to expand this list before adding new member.

            size_t nNeed = ListMallocCount + 1 + 10;
            nNeed += ( nNeed / 8 ); // grow liberally 125 %
            void * pvm = MyRealloc( 9995, ListMallocCount * sizeof( SoIt ), List, nNeed * sizeof( SoIt ) );
            if( pvm == NULL )
            {
                // do not change original List, ListMallocCount
                SolLeaveCrisis( );
                return 0; // failure - already messaged
            }
            List = ( SoIt * ) pvm;
            ListMallocCount = nNeed;
        }
        ListFilledCount ++;

        nList = ListFilledCount; // public copy

        // Initialize members of this new x item:

        memcpy( List[x].KeyStart, List[TAIL].KeyStart, sizeof( List[TAIL].KeyStart ) );

        // I'm glad I kept this length, because must copy key now in TAIL.
        if( LengthOfExtension == 0 )
        {
            List[x].szKeyExt = NULL; // No extension is needed.
        }
        else
        {
            // This key's malloc will never grow, so make it precise.
            List[x].szKeyExt = ( wchar_t * ) MyMalloc( 329, LengthOfExtension * sizeof( wchar_t ) ); // including NULL
            memcpy( List[x].szKeyExt, List[TAIL].szKeyExt, LengthOfExtension * sizeof( wchar_t ) ); // including NULL
        }

        // New node at edge of tree points back to Tail sentinel.
        List[x].Higher = TAIL;
        List[x].Lower = TAIL;
        List[x].Red = 0; // this node is still a singleton, not twain

        // Init other members useful to some greater program:
        List[x].User.Value = 0; // Also known as ...User.pVoid = NULL;

        // Split above might have changed x's father's identity.
        // So compare to current father before attaching to him.
        // I assume that means father's slot must be empty here?

        if( StrcmpTrialValueInTailTo( father ) < 0 )
        {
            List[father].Lower = x;
        }
        else
        {
            List[father].Higher = x;
        }

        // I'm just following the recipe. It says to split after adding x.
        // x = Split ( TestKey, greatgrandfather, grandfather, father, x );

        {
            // ============================================
            // Here starts copy #2 of Split, included inline.
            // ============================================
            // x will become red, and both children will become black.
            List[x].Red = 1;
            List[List[x].Lower].Red = 0;
            List[List[x].Higher].Red = 0;

            // If now x and x's father are both red, change that situation.
            // The algorithm depends on no-two-reds in a sequence going down.
            // Therefore, promote the father's redness up to the grandfather.
            // The grandfather MUST have been black before, per principles.

            if( List[father].Red )
            {
                List[grandfather].Red = 1;

                // The original x we tested higher/lower is now the father.
                // Compare the slant of now grandfather to slant of father.

                int LowerThanGrandFather;
                int LowerThanFather;

                if( StrcmpTrialValueInTailTo( grandfather ) <= 0 )
                {
                    LowerThanGrandFather = 1;
                }
                else
                {
                    LowerThanGrandFather = 0;
                }

                if( StrcmpTrialValueInTailTo( father ) <= 0 )
                {
                    LowerThanFather = 1;
                }
                else
                {
                    LowerThanFather = 0;
                }

                // per book, "if ( v < g->Key ) <> ( v < f->Key ), Rotate gf.
                if( LowerThanGrandFather != LowerThanFather )
                {
                    x = Rotate( grandfather );
                }

                // and, in red father, always Rotate ggf:

                x = Rotate( greatgrandfather );
                List[x].Red = 0;

            }
            // ============================================
            // Here ends copy #2 of Split, included inline.
            // ============================================
        }

        x = HoldOntoTheNewX; // Otherwise, that split causes wrong x retn
    }

    SolLeaveCrisis( );
    return x;
}

size_t CSol::AddUniqueRank( int Rank )
{
    #if DO_DEBUG_CALLS
        Routine( L"243" );
    #endif
    SolEnterCrisis( );
    if( Rank > MAX_RANK )
        Rank = MAX_RANK;
    if( Rank < 1 )
        Rank = 1;
    wchar_t InsertKey [ NKEYSTART + 1 ];
    for( ;; )
    {
        // SpewValue( L"AddUniqueRank Rank", Rank );
        wsprintf( InsertKey, L"%7d", Rank ); // This 7 == NKEYSTART
        size_t Index = Find( InsertKey );
        // SpewValue( L"AddUniqueRank Find Index", Index );
        if( Index == 0 )
        {
            // Rank not previously used, Okay to add Rank.
            Index = AddKey( InsertKey );
            // SpewValue( L"AddUniqueRank Add Index", Index );
            SolLeaveCrisis( );
            return Index;
        }
        if( --Rank == 0 ) // Try next lower value.
            break;
    }
    // Spew( L"AddUniqueRank Broke" );
    SolLeaveCrisis( );
    return 0; // Rank fell off the chart: Not added.
}

size_t CSol::MayFind( wchar_t * InsertKey )
{
    #if DO_DEBUG_CALLS
        Routine( L"514" );
    #endif
    SolEnterCrisis( );
    // Wrap the find routine in parameter and result checking.
    // MayFind's caller is not sure if the key is in the list.
    if( InsertKey == NULL )
    {
        SetLastError( ERROR_SUCCESS );
        ProgramError( L"MayFind: InsertKey == NULL" );
        SolLeaveCrisis( );
        return 0;
    }
    if( * InsertKey == NULL )
    {
        SetLastError( ERROR_SUCCESS );
        ProgramError( L"MayFind: *InsertKey == NULL" );
        SolLeaveCrisis( );
        return 0;
    }
    size_t retn = Find( InsertKey );
    SolLeaveCrisis( );
    return retn;
}

size_t CSol::MustFind( wchar_t * InsertKey )
{
    #if DO_DEBUG_CALLS
        Routine( L"515" );
    #endif
    SolEnterCrisis( );
    // Wrap the find routine in parameter and result checking.
    // MustFind's caller asserts that this key IS in the list.
    if( InsertKey == NULL )
    {
        SetLastError( ERROR_SUCCESS );
        ProgramError( L"MustFind: InsertKey == NULL" );
        SolLeaveCrisis( );
        return 0;
    }
    if( * InsertKey == NULL )
    {
        SetLastError( ERROR_SUCCESS );
        ProgramError( L"MustFind: *InsertKey == NULL" );
        SolLeaveCrisis( );
        return 0;
    }
    size_t retn = Find( InsertKey );
    SolLeaveCrisis( );
    return retn;
}

size_t CSol::Find( wchar_t * InsertKey )
{
    #if DO_DEBUG_CALLS
        // Routine( L"244" );
    #endif
    SolEnterCrisis( );
    // Passed null terminated wide string will be found, or retn zero.
    size_t x = 0;
    // This work of setting the search key is same in both Add/Find.

    wchar_t * from = InsertKey;
    wchar_t * into = List[TAIL].KeyStart;
    size_t i = 0;
    for( ;; )
    {
        if( ( *into = *from ) != NULL )
            from++;

        into ++;

        if( ++ i == NKEYSTART )
            break;
    }

    size_t LengthOfExtension = 0;

    if( *from == NULL )
    {
        // Null pointer substitutes for a NULL at [NKEYSTRING]
        List[TAIL].szKeyExt = NULL;
    }
    else
    {
        // More chars to copy into TAIL extension.
        // Restore non-NULL pointer for extension.

        into = List[TAIL].szKeyExt = MallocTailKeyExtPtr;
        size_t Remains = MallocTailKeyExtSize;

        for( ;; )
        {
            // We always get to copy a NULL into szKeyExt[]:
            if( ( *into ++ = *from ++ ) == NULL )
                break;

            if( -- Remains == 0 )
            {
                // Pause to expand extension before continuing to copy.
                size_t Advanced = into - MallocTailKeyExtPtr;;
                size_t Lacking = wcslen( from ) + 1; // 1 for NULL
                size_t nNeed = MallocTailKeyExtSize + Lacking;
                nNeed += ( nNeed / 8 ); // grow liberally 125 %
                void * pvm = MyRealloc( 9996, MallocTailKeyExtSize * sizeof( wchar_t ), MallocTailKeyExtPtr, nNeed * sizeof( wchar_t ) );
                if( pvm == NULL )
                {
                    // do not change original MallocTailKeyExtPtr, MallocTailKeyExtSize
                    // and abandon this attempt to AddKey( ), return 0.
                    SolLeaveCrisis( );
                    return 0; // failure - already messaged
                }
                MallocTailKeyExtPtr = ( wchar_t * ) pvm;
                MallocTailKeyExtSize = nNeed;
                into = List[TAIL].szKeyExt = MallocTailKeyExtPtr;
                into += Advanced;
            }
        }
        LengthOfExtension = into - MallocTailKeyExtPtr; // including 1 for NULL
    }

    // But the rest of FindOnly is trivial:

    x = HEAD;

    for( ;; )
    {
        int PlusMinusOrZero = StrcmpTrialValueInTailTo( x );

        if( PlusMinusOrZero == 0 )
        {
            // This is the equality case that will re-use an existing node.
            // Or, match might be to self in TAIL, in which case, retn 0.
            break;
        }
        if( PlusMinusOrZero < 0 )
        {
            x = List[x].Lower;
        }
        else
        {
            x = List[x].Higher;
        }
    }
    SolLeaveCrisis( );
    return x;
}

CoIt * CSol::GetSortedVector( int ReverseOutputOrder )
{
    #if DO_DEBUG_CALLS
        Routine( L"245" );
    #endif

    // BTW, the returned vector will NOT contain the head nor tail,
    // so it will be shorter by two than the Sol's nList indicates.
    // That also means no CoIt in vector will have an empty string.

    // With MyMalloc, GetSortedVector never returns NULL ptr.
    // Well, when MyMalloc fails, try to limp out of program.
    //
    // One final shot in the foot: Here I returned a vector
    // of POINTERS to SOIT structures. Caller then can loop
    // through them. My SOIT structures all lie in the main
    // list vector, which, when it grows, will be realloc'd.
    // So, instead, I must return in vector the COIT copies.
    // My caller does not need the lower and higher indexes.
    //
    // Copies ( partial ) of Soit returned from GetSortedVector:
    // typedef struct _CoIt
    // {
    //     wchar_t * szKeyExt; // fattest alignment item
    //     wchar_t   KeyStart [NKEYSTART]; // Any 2N-1 shorts
    //     wchar_t   IsSentinel; // non-zero past last item
    //     union {
    //         int     Value;
    //         void *  pVoid;
    //     } User;
    // } CoIt;
    //
    // Index is into the unsorted CSol vector, 2 up.
    // The returned vector runs from 0 uncorrelated.

    SolEnterCrisis( );
    CoIt * pCoItVector = ( CoIt * ) MyMalloc( 941, ListFilledCount * sizeof( CoIt ) ); // including tail ( #0 ) and head ( #1 )
    if( pCoItVector == NULL )
    {
        SolLeaveCrisis( );
        return NULL;
    }
    size_t fill = 0;

    // Make an iterative version of the recursive tree print.
    // It seems to me, one bit per doubling should be enough.
    // Nevertheless, double it from 8 * byte count to 16 etc.
    size_t IndexStack[ 16 * sizeof( size_t ) ];
    size_t IndexStackCount = 0;

    size_t CurrentIndex = List[HEAD].Higher; // root of tree
    int JustPopped = 0;

    if( CurrentIndex != TAIL ) // == means the list is empty.
    {
        for( ;; )
        {
            // process any lower/left link
            // Did I say left? Generalize:

            if( ! JustPopped ) // prevent re-re-re-re-re-re-re-repush...
            {
                for( ;; )
                {
                    size_t left = ReverseOutputOrder ?
                        List[CurrentIndex].Higher :
                        List[CurrentIndex].Lower  ;

                    if( left == TAIL ) // stop marching left
                        break;

                    IndexStack[ IndexStackCount ++ ] = CurrentIndex;
                    CurrentIndex = left;
                }
            }

            JustPopped = 0; // disarm

            // No more to the left? Then do current node.
            {
                // Copy list item into sorted vector.
                SoIt * pSoIt = List + CurrentIndex;
                CoIt * pCoIt = pCoItVector + fill++;
                memcpy( pCoIt->KeyStart, pSoIt->KeyStart, NKEYSTART * sizeof( wchar_t ) );
                pCoIt->szKeyExt = pSoIt->szKeyExt;
                pCoIt->User.pVoid = pSoIt->User.pVoid;
                pCoIt->IsSentinel = 0;
            }

            // Process any higher/right link

            size_t right = ReverseOutputOrder ?
                List[CurrentIndex].Lower   :
                List[CurrentIndex].Higher  ;

            if( right != TAIL )
            {
                CurrentIndex = right;
                continue;
            }

            // Having done any left, the current, and any right, then pop.

            if( IndexStackCount == 0 )
            {
                // But, if none to pop, loop is done.
                break;
            }

            CurrentIndex = IndexStack[ -- IndexStackCount ];

            // Having popped, prevent another left push out of current node.
            JustPopped = 1;
        }
    }
    pCoItVector[ fill ].IsSentinel = 1; // sentinel for caller loop
    SolLeaveCrisis( );
    return pCoItVector;
}

wchar_t * CSol::GetFullKey( size_t index )
{
    #if DO_DEBUG_CALLS
        Routine( L"246" );
    #endif
    SolEnterCrisis( );
    // malloc and retn a null-terminated wide string or NULL. Caller frees.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List GetFullKey", index );
        SolLeaveCrisis( );
        return NULL;
    }
    size_t nExtLen = 0;

    if( List[index].szKeyExt != NULL )
        nExtLen = wcslen( List[index].szKeyExt );

    size_t nMallocLen = NKEYSTART + nExtLen + 1;
    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 677, nMallocLen * sizeof( wchar_t ) );
    memcpy( MallocPtr, List[index].KeyStart, sizeof( List[index].KeyStart ) );
    MallocPtr[NKEYSTART] = NULL;
    if( List[index].szKeyExt != NULL )
        wcscpy( MallocPtr + NKEYSTART, List[index].szKeyExt );
    SolLeaveCrisis( );
    return MallocPtr;
}

void * CSol::GetUserpVoid( size_t index )
{
    #if DO_DEBUG_CALLS
        // Routine( L"247" );
    #endif
    SolEnterCrisis( );
    // one of several routines to access the private User pVoid / Value.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List GetUserpVoid", index );
        SolLeaveCrisis( );
        return NULL;
    }
    if( m_UserType == CSOL_SCALAR )
    {
        ProgramError( L"CSOL_SCALAR - GetUserpVoid" );
        SolLeaveCrisis( );
        return NULL;
    }
    SolLeaveCrisis( );
    return List[index].User.pVoid;
}

void CSol::SetUserpVoid( size_t index, void * UserpVoid )
{
    #if DO_DEBUG_CALLS
        // Routine( L"248" );
    #endif
    SolEnterCrisis( );
    // one of several routines to access the private User pVoid / Value.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List SetUserpVoid", index );
        SolLeaveCrisis( );
        return;
    }
    if( m_UserType == CSOL_SCALAR )
    {
        ProgramError( L"CSOL_SCALAR - SetUserpVoid" );
        SolLeaveCrisis( );
        return;
    }
    List[index].User.pVoid = UserpVoid;
    SolLeaveCrisis( );
    return;
}

int CSol::ClaimUserpVoid( size_t index, void * NewUserpVoid )
{
    #if DO_DEBUG_CALLS
        Routine( L"249" );
    #endif
    SolEnterCrisis( );

    // The URL work changes CSolAllUrls pVoid under CSol's Critical Section.
    // I will also enforce certain rules, of what values may be claimed:
    // Unless it is a valid pointer to COnePaper, pVoid in CSolAllUrls means:
    // 0 = URL was never claimed nor obtained ( Add file, cache, fetch ).
    // -1 = claimed during ( Add file, cache, fetch ). Work in progress.
    // -2 = unsuccessful or undesirable or redirected. Never try again.
    // I might allow -2, -3, -4, ... to distinguish several such cases.
    // No valid pointer can be greater than 0x10000000 - sizeof( SoIt );
    // What shall I return? a success status, or the old value, or...?

    // one of several routines to access the private User pVoid / Value.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List ClaimUserpVoid", index );
        SolLeaveCrisis( );
        return 0;
    }
    // In fact, I could enforce that the CSol in question is CSolAllUrls.
    if( m_UserType == CSOL_SCALAR )
    {
        ProgramError( L"CSOL_SCALAR - ClaimUserpVoid" );
        SolLeaveCrisis( );
        return 0;
    }

    // Now apply rules that will not sink the multithread ship.
    // Once a valid pointer is hung, it shall never be removed,
    // at least, not while any threads are running. All callers
    // who will scan CSolAllUrls must change their simple NULL test
    // to also match all the -1,-2,-3... cases as DO NOT TOUCH.

    void * nOld = List[index].User.pVoid;
    void * nNew = NewUserpVoid;
    int okay = 0;

    // caller can claim a 0 only with -1.
    // If that step fails, he must not continue. Return 0 for failure.
    // caller can claim a -1 with a -2,3,4... or with a valid pointer.
    // caller may claim a -1 with a NULL too, if add cache is aborted.
    // caller can never claim a -2,3,4....
    // caller can never claim a valid pointer.

    // Surely, pointers must be considered unsigned math...?
    // If not, all will fail: pOnePaper < PVOID_VALID_BELOW.
    // Being unsure, I should typecast them all to unsigned.

    // #define PVOID_UNTRIED       NULL
    // #define PVOID_CLAIMING      ( ( void* )( -1L ) )
    // #define PVOID_NOTFOUNDETC   ( ( void* )( -2L ) )
    // #define PVOID_REDIRECTION   ( ( void* )( -3L ) )
    // #define PVOID_UNDESIRABLE   ( ( void* )( -4L ) )
    // #define PVOID_QUERYREJECT   ( ( void* )( -5L ) )
    // #define PVOID_VALID_BELOW   ( ( void* )( -6L ) )

    if( nNew == PVOID_CLAIMING )
    {
        if( nOld == PVOID_UNTRIED )
        {
            okay = 1;
            List[index].User.pVoid = nNew;
        }
    }
    else
    {
        if( nOld == PVOID_CLAIMING )
        {
            okay = 1;
            List[index].User.pVoid = nNew;
        }
        else
        {
            ProgramError( L"ClaimUserpVoid != PVOID_CLAIMING" );
        }
    }
    SolLeaveCrisis( );
    return okay;
}

size_t CSol::GetUserValue( size_t index )
{
    #if DO_DEBUG_CALLS
        // Routine( L"250" );
    #endif
    SolEnterCrisis( );
    // one of several routines to access the private User pVoid / Value.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List GetUserValue", index );
        SolLeaveCrisis( );
        return 0;
    }
    if( m_UserType != CSOL_SCALAR )
    {
        ProgramError( L"Not CSOL_SCALAR - GetUserValue" );
        SolLeaveCrisis( );
        return NULL;
    }
    SolLeaveCrisis( );
    return List[index].User.Value;
}

void CSol::SetUserValue( size_t index, int UserValue )
{
    #if DO_DEBUG_CALLS
        // Routine( L"251" );
    #endif
    SolEnterCrisis( );
    // one of several routines to access the private User pVoid / Value.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List SetUserValue", index );
        SolLeaveCrisis( );
        return;
    }
    if( m_UserType != CSOL_SCALAR )
    {
        ProgramError( L"Not CSOL_SCALAR - SetUserValue" );
        SolLeaveCrisis( );
        return;
    }
    List[index].User.Value = UserValue;
    SolLeaveCrisis( );
    return;
}

void CSol::IncrementUserValue( size_t index )
{
    #if DO_DEBUG_CALLS
        // Routine( L"252" );
    #endif
    SolEnterCrisis( );
    // one of several routines to access the private User pVoid / Value.
    if( index <= 1 || index >= ListFilledCount )
    {
        InvalidIndex( L"List IncrementUserValue", index );
        SolLeaveCrisis( );
        return;
    }
    if( m_UserType != CSOL_SCALAR )
    {
        ProgramError( L"Not CSOL_SCALAR - IncrUserValue" );
        SolLeaveCrisis( );
        return;
    }
    List[index].User.Value ++;
    SolLeaveCrisis( );
    return;
}

void CSol::FreeMallocs( )
{
    #if DO_DEBUG_CALLS
        Routine( L"253" );
    #endif
    // private function
    // No Crisis needed.
    // List is not NULL now.
    List[TAIL].szKeyExt = MallocTailKeyExtPtr;
    size_t i = 0;
    for( ;; )
    {
        if( i == ListFilledCount )
            break;
        if( List[i].szKeyExt != NULL )
        {
            MyFree( 813, zx, List[i].szKeyExt ); // including MallocTailKeyExtPtr
            List[i].szKeyExt = NULL; // including MallocTailKeyExtPtr
        }
        if( List[i].User.pVoid != NULL
        &&  List[i].User.pVoid < PVOID_VALID_BELOW )
        {
            switch( m_UserType )
            {
                // 0 = scalar, 1 = malloc/free, 2 = object/delete
            case 1:
                {
                    // Getting cranky in my old age. Do this idiom a lot:
                    void * Temp = List[i].User.pVoid; // copy before free
                    List[i].User.pVoid = NULL;        // NULL before free
                    MyFree( 826, zx, Temp );
                    Temp = NULL; // obligatory rigor after delete
                }
                break;
            case 2:
                {
                    // Getting cranky in my old age. Do this idiom a lot:
                    void * Temp = List[i].User.pVoid; // copy before delete
                    List[i].User.pVoid = NULL;        // NULL before delete
                    delete Temp;
                    Temp = NULL; // obligatory rigor after delete
                }
                break;
            }
        }
        i ++;
    }
    MyFree( 843, zx, List );
    List = NULL;
    // No Crisis needed.
    return;
}

int CSol::StrcmpTrialValueInTailTo( size_t y )
{
    #if DO_DEBUG_CALLS
        // Routine( L"254" );
    #endif
    // private function
    // No Crisis needed.
    // List is not NULL now.
    // It appears to me that the v=value of original algorithm
    // is always the one started with, and is copied into TAIL.
    // So Initialized List[TAIL].Key... is an implied argument.
    // Every test herein is as if asking of List[ item # y ],
    // retn strcmp( List[TAIL].Key..., List[y].Key.... )

    // I plan to keep first NKEYSTART letters of key in the SoIt,
    // and extend any long words using a separate malloc.

    int PlusMinusOrZero = wcsncmp( List[TAIL].KeyStart, List[y].KeyStart, NKEYSTART );

    if( PlusMinusOrZero != 0 )
    {
        // No Crisis needed.
        return PlusMinusOrZero; // found a difference in the first seven wchar_ts
    }

    wchar_t * pt = List[TAIL].szKeyExt;
    wchar_t * py = List[y].szKeyExt;

    if( pt == NULL )
    {
        if( py == NULL )
        {
            // No Crisis needed.
            return 0; // trial/tail value is equal to y value
        }
        else
        {
            // y value being longer, is higher.
            // No Crisis needed.
            return -1; // trial/tail value is lower than y value
        }
    }
    else
    {
        if( py == NULL )
        {
            // trial/tail value being longer, is higher.
            // No Crisis needed.
            return 1; // trial/tail value is higher than y value
        }
        else
        {
            // No Crisis needed.
            return wcscmp( pt, py ); // find out...
        }
    }
    // No Crisis needed.
    // here is no  return;
}

size_t CSol::Rotate( size_t y )
{
    #if DO_DEBUG_CALLS
        // Routine( L"255" );
    #endif
    // private function
    // No Crisis needed.
    // List is not NULL now.
    // Rotate balances the 2-3-4 tree during the Insert operation.
    // First we test y, to pick a son of interest given trial value;
    // Then we test that son of y, to pick the grandson of interest;
    // Then we swap the son and grandson positions under y, unmoved.

    size_t son;
    size_t grandson;
    size_t TestedLower;

    if ( StrcmpTrialValueInTailTo( y ) < 0 )
    {
        son = List[y].Lower;
        TestedLower = 1;
    }
    else
    {
        son = List[y].Higher;
        TestedLower = 0;
    }

    // Here the son and grandson swap their descendent links.
    // Here the old grandson receives as a child the old son.
    if ( StrcmpTrialValueInTailTo( son ) < 0 )
    {
        grandson = List[son].Lower;
                   List[son].Lower = List[grandson].Higher;
                                     List[grandson].Higher = son;
    }
    else
    {
        grandson = List[son].Higher;
                   List[son].Higher = List[grandson].Lower;
                                      List[grandson].Lower = son;
    }

    // Here the father, y, receives as a child the old grandson.
    // Reuse the "if v < y->key" from above. That hasn't changed.
    if( TestedLower )
    {
        List[y].Lower = grandson;
    }
    else
    {
        List[y].Higher = grandson;
    }
    // No Crisis needed.
    return grandson;
}

void CSol::InvalidIndex( wchar_t * szCaption, size_t index )
{
    #if DO_DEBUG_CALLS
        Routine( L"256" );
    #endif
    // private function
    // No Crisis needed.
    static wchar_t wk[100]; // consciously using this static in a thread.
    wsprintf( wk, L"%s: index %d < 2 or >= ListFilledCount %d", szCaption, index, ListFilledCount );
    SetLastError( ERROR_SUCCESS );
    ProgramError( wk );
    // No Crisis needed.
    return;
}
