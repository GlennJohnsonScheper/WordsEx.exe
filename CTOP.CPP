// This is file: CTop.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

/*
In CSee and CTop, EVERY routine has a return at end to assist fgrep of:
::
Crisis(
  return
*/

#ifdef _WIN32_WCE
wchar_t SplashScreenText[] = {
    L"Welcome to Words, Extended!\r\n"
    L"\r\n"
    L"Use \"Add, Internet Search\" to get started.\r\n"
    L"Type in your search words, press \"Search\".\r\n"
    L"\r\n"
    L"Downloaded web page summaries will appear.\r\n"
    L"Click on any summary to read that web page.\r\n"
    L"\r\n"
    L"Use arrow keys to smooth-scroll:\r\n"
    L"           ^ = stop\r\n"
    L"< = slower          > = faster\r\n"
    L"         v = minimize\r\n"
};
#else // not _WIN32_WCE
wchar_t SplashScreenText[] = {
    L"               Welcome to Words, Extended!\r\n"
    L"      \r\n"
    L"      Use \"Add, Internet Search\" to get started.\r\n"
    L"      Type in your search words, press \"Search\".\r\n"
    L"      \r\n"
    L"      Downloaded web page summaries will appear.\r\n"
    L"      Click on any summary to read that web page.\r\n"
    L"      \r\n"
    L"              Use arrow keys to smooth-scroll:\r\n"
    L"                                ^ = stop\r\n"
    L"              < = slower                   > = faster\r\n"
    L"                              v = minimize\r\n"
};
#endif // _WIN32_WCE


CTop::CTop( )
{
    #if DO_DEBUG_CALLS
        Routine( L"257" );
    #endif
    SatEnterCrisis( );
    CurrentView = NULL;
    #if DO_DEBUG_TOP
        DebugTop( L"constructor" );
    #endif
    SatLeaveCrisis( );
    return;
}

CTop::~CTop( )
{
    #if DO_DEBUG_CALLS
        Routine( L"258" );
    #endif
    SatEnterCrisis( );
    // I should delete all views here.
    // Later. But for now invalidate head:
    // If head is null, every access stops.
    // Program is exiting anyway.
    CurrentView = NULL;

    #if DO_DEBUG_TOP
        DebugTop( L"destructor" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::DebugTop( wchar_t * Legend )
{
    #if DO_DEBUG_CALLS
        Routine( L"259" );
    #endif
    SatEnterCrisis( );

    // I started to lose the top ring once the fetch process
    // wanted to rid references to soon-to-be deleted pFruit.
    // Display all view's fruit address, and fruit enum type.

    // Start with currentview, and go around the ring.
    // Also silently verify each ring pointer is okay.

    Views * Run = CurrentView;

    wchar_t wk[500];
    wchar_t * into = wk;

    size_t value = 0;
    if( Run != NULL )
        value = ( size_t ) Run->pFruit % 99 * 10 + Run->pFruit->m_FruitType;
    *into++ = value % 10 + 'a' - 1; value /= 10;
    *into++ = value % 10 + '0'; value /= 10;
    *into++ = value % 10 + '0';
    *into++ = ' ';

    // Letter legend: Boy, if this doesn't map onto Revelation!
    // not used any more... a ENUM_INTERNAL 1
    // b ENUM_ONEPAPER 2
    // c ENUM_ADDCACHE 3
    // d ENUM_ADDDIRECTORY 4
    // e ENUM_ADDPAGE 5
    // f ENUM_ADDSEARCH 6
    // g ENUM_ADDFIND 7
    // h ENUM_ADDPHRASES 8

    if( CurrentView != NULL )
    {
        for( ;; )
        {
            Views * Fwd = Run->Next;

            value = ( size_t ) Fwd->pFruit % 99 * 10 + Fwd->pFruit->m_FruitType;
            *into++ = value % 10 + 'a' - 1; value /= 10;
            *into++ = value % 10 + '0'; value /= 10;
            *into++ = value % 10 + '0';
            *into++ = ' ';

            if( Fwd->Back != Run )
            {
                *into++ = '?';
                *into++ = '?';
                *into++ = '?';
                *into++ = ' ';
                break;
            }

            Run = Run->Next;

            if( Run == CurrentView )
                break;

            if( into > wk + 450 )
            {
                *into++ = '.';
                *into++ = '.';
                *into++ = '.';
                *into++ = ' ';
                break;
            }
        }
    }
    wcscpy( into, Legend ); // caller's phrase
    ; Spew( wk );
    SatLeaveCrisis( );
    return;
}

void CTop::Next( )
{
    #if DO_DEBUG_CALLS
        Routine( L"260" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->Next" );
    #endif
    // Advance current view to next top-level items.
    if( CurrentView == NULL )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"<-Next1" );
        #endif
        SatLeaveCrisis( );
        return;
    }
    See.QuitSmoothScrolling( 1 );

    // Update this when leaving view:
    CurrentView->FocusLineOffsetInText = See.GetOffsetofFocusLine( );

    CurrentView = CurrentView->Next;
    ShowCurrentTopItemViewOnScreen( );

    #if DO_DEBUG_TOP
        DebugTop( L"<-Next2" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::Back( )
{
    #if DO_DEBUG_CALLS
        Routine( L"261" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->Back" );
    #endif
    // Retreat current view to previous top-level items.
    if( CurrentView == NULL )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"<-Back1" );
        #endif
        SatLeaveCrisis( );
        return;
    }
    See.QuitSmoothScrolling( 1 );

    // Update this when leaving view:
    CurrentView->FocusLineOffsetInText = See.GetOffsetofFocusLine( );

    CurrentView = CurrentView->Back;
    ShowCurrentTopItemViewOnScreen( );

    #if DO_DEBUG_TOP
        DebugTop( L"<-Back2" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::FollowClickToAdd( CIdx * pIdxResultIndex, size_t Offset1 )
{
    #if DO_DEBUG_CALLS
        Routine( L"262" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->FollowClickToAdd" );
    #endif

    // User passed CurrentView's pFruit's pIdxResultIndex.
    // User will also pass an Offset I must look up in Idx,
    // determine new pFruit, a pOnePaper, and Add it to ring.
    // This Offset1 is offset of mouse click in the results.

    // I need to act differently depending on Kwic1_Sentc2_Parag3:
    // KWIC ( 1 ): position the kwic-centered word at the top of screen.
    // SENTENCE ( 2 ): position the start of sentence at the top of screen.
    // PARAGRAGH ( 3 ): position the start of block at the top of screen.

    // No. All formulators of results must record the UrlOffset where
    // they want the source page to be displayed. In all three cases,
    // they should make it the offset of the start of the matching
    // whole kwic context, or whole sentence, or whole block.

    // So I don't do anything different here, but the 3 variants
    // of Add Find must pass me the correct 1 of 3 pIDX....Wait,
    // No, I am getting the pIDX of the result, always #1 of 3.

    size_t UrlIndex = 0;
    size_t UrlOffset = 0;

    if( pIdxResultIndex->ScanIdxToFollow( Offset1, & UrlIndex, & UrlOffset ) )
    {
        COnePaper * pFruit = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
        if( pFruit != NULL
        && pFruit < PVOID_VALID_BELOW )
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( UrlOffset );
        }
    }
    #if DO_DEBUG_TOP
        DebugTop( L"<-FollowClickToAdd" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::RemoveReferences( CBud * pFruit )
{
    #if DO_DEBUG_CALLS
        Routine( L"263" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->RemoveReferences" );
    #endif

    // Either for adding a fruit to top ring,
    // or when fetch etc. would delete fruit,
    // Search for identical fruit on display.
    // Delete if present in top ring of views.

    // CURRENTLY, my only caller is in CFio,
    // when adding a file finds that URL is
    // already present in CSolAllUrls, and CFio
    // will discard the old CSolAllUrls pFruit.

    // ESPECIALLY, the act of unlinking a view
    // when re-adding the same fruit never has
    // to delete, delink, or stop threads.
    // More accurately, unlink fruit from ring,
    // But only delete pFruit of INTERNAL type.
    // Actually, they never match, so not done.
    // And delete the unlinked Views structure.
    // No thread stopping. See that work above.
    // Note that the internals made new fruit,
    // so do not match in if pointer identity.
    // I could burden this with a test for it.
    // Since internals will not match, and Add
    // needs to keep using it, and Delete will
    // soon occur during fetches, this routine
    // need not delete the fruit, only a view.

    if( CurrentView == NULL
    || pFruit == NULL )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"<-RemoveReferences1" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    Views * Run = CurrentView;
    for( ;; )
    {
        if( Run->pFruit == pFruit )
        {
            // Run item matches pFruit in question.
            // Remove it from an existing position.

            Views * pGoingAway = Run;

            if( pGoingAway->Back == pGoingAway )
            {
                // The list is now becoming empty.
                CurrentView = NULL;
                #if DO_DEBUG_TOP
                    DebugTop( L"RemoveReferences emptied ring" );
                #endif
            }
            else
            {
                // Only when search matched current item,
                // retreat as Back from old current item.

                if( CurrentView == pGoingAway )
                {
                    CurrentView = pGoingAway->Back;
                    #if DO_DEBUG_TOP
                        DebugTop( L"RemoveReferences is cur view. did back." );
                    #endif
                }

                #if DO_DEBUG_TOP
                    DebugTop( L"RemoveReferences relinking" );
                #endif
                // Relink other items around GoingAway item.
                pGoingAway->Back->Next = pGoingAway->Next;
                pGoingAway->Next->Back = pGoingAway->Back;
                #if DO_DEBUG_TOP
                    DebugTop( L"RemoveReferences relinked" );
                #endif
            }

            // This case of pGoingAway is NOT for the DELETE keystroke.
            // So do not: pGoingAway->pFruit->m_StopThisThread = 1;

            pGoingAway->Back = NULL;
            pGoingAway->Next = NULL;
            pGoingAway->pFruit = NULL;
            {
                Views * temp = pGoingAway;
                pGoingAway = NULL;
                delete temp;
            }

            break; // there is at most one to match in ring.
        }

        Run = Run->Next;
        if( Run == CurrentView )
        {
            #if DO_DEBUG_TOP
                DebugTop( L"RemoveReferences fruit not in ring" );
            #endif
            break; // not found
        }
    }
    #if DO_DEBUG_TOP
        DebugTop( L"<-RemoveReferences2" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::Add( CBud * pFruit )
{
    #if DO_DEBUG_CALLS
        Routine( L"264" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->Add" );
    #endif

    // When I add any new top ( like ^V ), zero the scanline offest.
    // See.m_CurrentSmoothScrollingScanLine = 0;
    // In fact, I delayed this quit until here too.
    See.QuitSmoothScrolling( 1 ); // 1 = align

    // Add Fruit ( Fetch, Query, Internal, etc. ) as top-level item.
    // This means make a new Views and insert it into list of views.
    // However, if this Fruit pointer is already in a view, show it.
    // Position the passed fruit view as NEXT from the current view.

    Views * pNewView = new Views;

    // Each Fruit type is derived from CBud.
    // CBud has member m_FruitType, telling what kind of thing it is.
    // CBud has member m_CSolIndex, telling which specific instance.

    if( pFruit->m_FruitType == ENUM_ONEPAPER )
    {
        COnePaper * pOnePaper = ( COnePaper * ) pFruit;
        if( pOnePaper->FetchedButNotViewed ) // first view of a "significant" page
        {
            pOnePaper->FetchedButNotViewed = 0; // getting its first view
            g_nPapersFetchedButNotViewed --;    // getting its first view
        }
    }

    pNewView->pFruit = pFruit;
    pNewView->FocusLineOffsetInText = 0;

    if( pFruit == NULL )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"<-Add1" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    if( CurrentView == NULL )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"Add first fruit to empty ring" );
        #endif

        // This new only item in the list, will point to itself.
        pNewView->Next = pNewView;
        pNewView->Back = pNewView;
        CurrentView = pNewView;
    }
    else
    {
        #if DO_DEBUG_TOP
            DebugTop( L"Add more fruit to non-empty ring" );
        #endif

        // Update this when leaving view:
        CurrentView->FocusLineOffsetInText = See.GetOffsetofFocusLine( );

        // Search for the identical fruit in ring.
        // Note that the internals made new fruit,
        // so do not match in IF pointer identity.
        // I won't burden this with a test for it.

        Views * Run = CurrentView;
        for( ;; )
        {
            if( Run->pFruit == pFruit )
            {
                // "New" item matched an existing view.
                // Delete it from an existing position.

                #if DO_DEBUG_TOP
                    DebugTop( L"Add more fruit was in ring" );
                #endif

                Views * pGoingAway = Run;

                if( pGoingAway->Back == pGoingAway )
                {
                    // The list is now becoming empty.
                    CurrentView = NULL;
                    #if DO_DEBUG_TOP
                        DebugTop( L"Add more fruit temp empty" );
                    #endif
                }
                else
                {
                    // Relink other items around GoingAway item.

                    #if DO_DEBUG_TOP
                        DebugTop( L"Add more fruit relinking" );
                    #endif

                    // Only when search matched current item,
                    // retreat as Back from old current item.
                    if( CurrentView == pGoingAway )
                    {
                        CurrentView = pGoingAway->Back;
                        #if DO_DEBUG_TOP
                            DebugTop( L"Add more fruit do cur view back" );
                        #endif
                    }

                    pGoingAway->Back->Next = pGoingAway->Next;
                    pGoingAway->Next->Back = pGoingAway->Back;

                    #if DO_DEBUG_TOP
                        DebugTop( L"Add more fruit relinked" );
                    #endif
                }

                // This case of pGoingAway is NOT for the DELETE keystroke.
                // So do not: pGoingAway->pFruit->m_StopThisThread = 1;

                pGoingAway->Back = NULL;
                pGoingAway->Next = NULL;
                pGoingAway->pFruit = NULL;
                {
                    Views * temp = pGoingAway;
                    pGoingAway = NULL;
                    delete temp;
                }

                break; // there is at most one to match in ring.
            }

            Run = Run->Next;

            if( Run == CurrentView )
            {
                #if DO_DEBUG_TOP
                    DebugTop( L"Add more fruit was not in ring" );
                #endif
                break; // not found
            }
        }

        // Again, I must ask if the list became empty.

        if( CurrentView == NULL )
        {
            #if DO_DEBUG_TOP
                DebugTop( L"Add more fruit to emptied ring" );
            #endif
            // This new only item in the list, will point to itself.
            pNewView->Next = pNewView;
            pNewView->Back = pNewView;
            CurrentView = pNewView;
        }
        else
        {
            #if DO_DEBUG_TOP
                DebugTop( L"Adding more fruit to non-emptied ring" );
            #endif

            // Link the new item as next from current item;
            // moving current item to the new item's back.
            // Then re-point to the new item, as current.

            Views * SomeView = CurrentView;
            Views * NextView = CurrentView->Next;

            pNewView->Next = NextView;
            pNewView->Back = SomeView;

            SomeView->Next = pNewView;
            NextView->Back = pNewView;

            CurrentView = pNewView;

            #if DO_DEBUG_TOP
                DebugTop( L"Added more fruit to non-emptied ring" );
            #endif

        }
    }

    // Done at top now.
    // See.QuitSmoothScrolling( 0 );

    ShowCurrentTopItemViewOnScreen( );
    #if DO_DEBUG_TOP
        DebugTop( L"<-Add" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::DeleteCurrentView( )
{
    #if DO_DEBUG_CALLS
        Routine( L"265" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->DeleteCurrentView" );
    #endif

    // Removes the CurrentView item from ring. Some do deletes.
    // CURRENTLY, my only callers are for the DELETE keystroke,
    // and for RemoveReferences, when CFIO adds equal URL file.

    if( CurrentView == NULL )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"<-DeleteCurrentView1" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    Views * pGoingAway = CurrentView;

    if( pGoingAway->Back == pGoingAway )
    {
        #if DO_DEBUG_TOP
            DebugTop( L"DeleteCurrentView fruit was alone in ring" );
        #endif
        // The list is now becoming empty.
        CurrentView = NULL;
    }
    else
    {
        #if DO_DEBUG_TOP
            DebugTop( L"DeleteCurrentView relinking" );
        #endif

        // Relink other items around GoingAway item.
        pGoingAway->Back->Next = pGoingAway->Next;
        pGoingAway->Next->Back = pGoingAway->Back;

        // And retreat as Back from old current item.
        CurrentView = pGoingAway->Back;

        #if DO_DEBUG_TOP
            DebugTop( L"DeleteCurrentView relinked" );
        #endif
    }

    // Shall I delete the GoingAway object? It depends:
    // There was an internal type that I would have deleted.
    // But now it is held as a Paper, so I can rid all this.
    // switch( pGoingAway->pFruit->m_FruitType )
    // {
    //
    // case ENUM_ADDCACHE:
    //     // This Fruit, a COneCache, lasts until program exit.
    // case ENUM_ADDPHRASES:
    //     // This Fruit, a COnePhrases, lasts until program exit.
    // case ENUM_ADDSEARCH:
    // case ENUM_ADDPAGE:
    // case ENUM_ADDFIND:
    // case ENUM_ONEPAPER:
    // case ENUM_ADDDIRECTORY:
    //     // I need not delete these, as they hang from other CSol trees.
    //     // When the trees are deleted, these fruit will be deleted too.
    //     #if DO_DEBUG_TOP
    //         DebugTop( L"DeleteCurrentView need not delete fruit" );
    //     #endif
    //     break;
    //
    // }

    ShowCurrentTopItemViewOnScreen( );

    pGoingAway->Back = NULL;
    pGoingAway->Next = NULL;
    pGoingAway->pFruit = NULL;
    {
        Views * temp = pGoingAway;
        pGoingAway = NULL;
        delete temp;
    }

    #if DO_DEBUG_TOP
        DebugTop( L"<-DeleteCurrentView2" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::UpdateViewIfOnScreen( void * pFruit )
{
    // This routine any all its calls has become obsolete.
    // CSee::WmMonitorThreadsTimerHandler will do similar.
    //
    // #if DO_DEBUG_CALLS
    //     Routine( L"266" );
    // #endif
    // SatEnterCrisis( );
    // #if DO_DEBUG_TOP
    //     DebugTop( L"->UpdateViewIfOnScreen" );
    // #endif
    //
    // if( pFruit != NULL
    // && CurrentView != NULL
    // && pFruit == CurrentView->pFruit )
    // {
    //     // CSee holds a dynamic value for the focused line offset.
    //     // CTop holds a recent value for when returning.
    //     // Put the two together to do nothing.
    //
    //     CurrentView->FocusLineOffsetInText = See.GetOffsetofFocusLine( );
    //     ShowCurrentTopItemViewOnScreen( );
    // }
    // #if DO_DEBUG_TOP
    //     DebugTop( L"<-UpdateViewIfOnScreen" );
    // #endif
    // SatLeaveCrisis( );
    // return;
}

void CTop::ShowCurrentTopItemViewOnScreen( )
{
    #if DO_DEBUG_CALLS
        Routine( L"267" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->ShowCurrentTopItemViewOnScreen" );
    #endif

    // preset for failure cases
    size_t TextOffset = 0;
    wchar_t * pMallocBuf = NULL;
    size_t nMallocBuf = 0; // Misnomer. Actually, is wcslen.

    CIdx * pIdxTextBlocks = NULL;

    if( CurrentView != NULL )
    {
        // Copy this position to restore after re-writing.
        TextOffset = CurrentView->FocusLineOffsetInText;

        CBud * pFruit = CurrentView->pFruit;
        CWsb * pWsbResultText = NULL;

        if( pFruit != NULL )
        {
            pWsbResultText = pFruit->pWsbResultText;
            if( pFruit->m_FruitType == ENUM_ONEPAPER )
                pIdxTextBlocks = pFruit->pIdxTextBlocks;
            else
                pIdxTextBlocks = NULL;
        }

        if( pWsbResultText != NULL )
        {
            // I need to save the ORIGINAL strlen to test for growth.
            See.m_nFruitStrlenBackingDisplay = pWsbResultText->StrLen;
            pMallocBuf = pWsbResultText->GetBuffer( & nMallocBuf );
        }

        // Now, in order to allow the focus ( top screen line ) to be set
        // even on the last line of text, I need to add N or N-1 blank
        // lines to buffer, where N is the current g_nLinesPageAdvances

        if( pMallocBuf != NULL
        && nMallocBuf != NULL )
        {
            // Remember, nMallocBuf is a misnomer. Actually, it is wcslen.
            size_t nLines = g_nLinesPageAdvances;
            size_t nWchars = nLines * 2;
            size_t n2MallocBuf = nMallocBuf + nWchars;
            size_t OldSize = ( nMallocBuf + 1 ) * sizeof( wchar_t );
            size_t NewSize = OldSize + nWchars * sizeof( wchar_t );
            wchar_t * p2MallocBuf = ( wchar_t * ) MyRealloc( 9999, OldSize, pMallocBuf, NewSize );
            // Any realloc failure was already messaged.
            if( p2MallocBuf != NULL )
            {
                // Oh, silly me. Realloc already did this:
                // memcpy( p2MallocBuf, pMallocBuf, OldSize );

                wchar_t * fill = p2MallocBuf + nMallocBuf;
                size_t i = 0;
                for( ;; )
                {
                    *fill++ = '\r';
                    *fill++ = '\n';
                    if( ++i == nLines )
                        break;
                }
                *fill = NULL;

                // double check: Seen many times before I got it right.
                // if( fill != p2MallocBuf + n2MallocBuf )
                //     ProgramError( L"I goofed" );
            }
            // Original malloc was consumed in realloc.
            // Copy back the large buffer to old names.
            nMallocBuf = n2MallocBuf;
            pMallocBuf = p2MallocBuf;
        }
    }

    // Called routine consumes malloc. Do not free it here.
    // I have prepared it to erase in case of NULL or zero.

    // For all view types that are not COnePaper,
    // pass a NULL for pIdxTextBlocks, to see all.

    See.SaveBackingText_MakeLineList( pMallocBuf, ( int ) nMallocBuf, pIdxTextBlocks );

    if( g_CurrentlyShowingSplashScreen )
    {
        KillTimer( g_hWnd, GROW_OMEGA_TIMER );

        g_CurrentlyShowingSplashScreen = 0; // stop extra OnPaint stuff

        // Revert from splash screen colors to working text colors:
        g_rgbErase = rgbErase[ g_rgbIndex ];
        g_rgbGlyph = rgbGlyph[ g_rgbIndex ];
        // Set background and text colors on the compatible DC.
        See.SetFirstOrRevisedTextColors( ); // SetTextColor, SetBkColor.
        {
            COme * temp = g_pOmega;
            g_pOmega = NULL; // idiomatic NULL before delete.
            delete temp;
        }
    }

    See.SetFocusLineToThisOffset( TextOffset );
    // Which routine calls:
    // See.IMightHaveChangedTheDisplay( ); // Received new text

    #if DO_DEBUG_TOP
        DebugTop( L"<-ShowCurrentTopItemViewOnScreen" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::ShowSplashScreen( )
{
    #if DO_DEBUG_CALLS
        Routine( L"513" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->ShowSplashScreen" );
    #endif

    size_t nMallocBuf = wcslen( SplashScreenText ); // misnomer: wcslen
    wchar_t * pMallocBuf = ( wchar_t * ) MyMalloc( 787, ( nMallocBuf + 1 ) * sizeof( wchar_t ) );
    memcpy( pMallocBuf, SplashScreenText, ( nMallocBuf + 1 ) * sizeof( wchar_t ) );

    // Called routine consumes malloc. Do not free it here.
    // I have prepared it to erase in case of NULL or zero.

    See.SaveBackingText_MakeLineList( pMallocBuf, ( int ) nMallocBuf, NULL );

    g_pOmega = new COme;
    g_CurrentlyShowingSplashScreen = 1; // enable extra OnPaint stuff

    // After srand, Wordsex.cpp init the random first splash screen colors.
    // Set splash screen's background and text colors on the compatible DC.
    See.SetFirstOrRevisedTextColors( ); // SetTextColor, SetBkColor.

    SetTimer( g_hWnd, GROW_OMEGA_TIMER, GROW_OMEGA_MS, NULL );

    See.SetFocusLineToThisOffset( 0 );
    // Which routine calls:
    // See.IMightHaveChangedTheDisplay( ); // Received new text

    #if DO_DEBUG_TOP
        DebugTop( L"<-ShowSplashScreen" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::AddLinks( )
{
    #if DO_DEBUG_CALLS
        Routine( L"310" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddLinks" );
    #endif

    // Copy code from the Fetch Dialog Proc,
    // except the URL is from paper on view.

    if( CurrentView == NULL
    || CurrentView->pFruit->m_FruitType != ENUM_ONEPAPER )
    {
        Say( L"Add All Links, Ctrl+A, will fetch all the web pages linked from a web page on display."
        L"\r\n\r\nHowever, the current view is not displaying a fetched/cached/opened web page file." );

        #if DO_DEBUG_TOP
            DebugTop( L"<-AddLinks" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    size_t UrlIndex = CurrentView->pFruit->m_CSolIndex;

    COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
    if( pOnePaper == NULL
    || pOnePaper > PVOID_VALID_BELOW )
    {
        ProgramError( L"AddLinks: pOnePaper invalid" );
        #if DO_DEBUG_TOP
            DebugTop( L"<-AddLinks" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    wchar_t * pMalUrl = ( wchar_t * ) CSolAllUrls.GetFullKey( UrlIndex );
    if( pMalUrl == NULL )
    {
        ProgramError( L"AddLinks: pMalUrl invalid" );
        #if DO_DEBUG_TOP
            DebugTop( L"<-AddLinks" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    // This fetched URL might not yet be in the User Fetches list.

    size_t Index = CSolUserFetchs.AddKey( pMalUrl );
    #if DO_DEBUG_ADDFIND
        if( Index <= 1 )
            { Spew( L"AddFind 1 at wordsex 1477" ); }
    #endif

    // All I need to keep is UserFetchs index number.
    MyFree( 761, zx, pMalUrl );

    // Just in case this is a repeat, try to get fruit.
    COneFetch * pFruit = ( COneFetch * ) CSolUserFetchs.GetUserpVoid( Index );
    // If not, make a new fruit to hang on the tree.
    if( pFruit == NULL )
    {
        pFruit = new COneFetch( Index );
        CSolUserFetchs.SetUserpVoid( Index, pFruit );
    }
    else
    {
        // This is a re-do. Make a bold dividing line in the log.
        pFruit->pWsbResultText->Add( L"\r\n\r\n" );
        pFruit->pWsbResultText->Add( L"===================================\r\n" );
        pFruit->pWsbResultText->Add( L"Reinitiating a previously run task.\r\n" );
        pFruit->pWsbResultText->Add( L"===================================\r\n" );
        pFruit->pWsbResultText->Add( L"\r\n\r\n" );
    }

    pFruit->m_FetchAllLinks = 1; // ignoring global setting g_FetchAllLinks;
    pFruit->m_NonTextLinks = g_NonTextLinks; // obey last global setting NonTextLinks;

    // Prompt for a directory to receive non-texts?
    // No, I will let CINI InitiateFetch do it JIT.

    // This is okay, in main thread, before starting worker thread:
    Top.Add( pFruit ); // Put fruit on display
    Initiator.InitiateFetch( Index );

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddLinks" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::AddUsage( )
{
    #if DO_DEBUG_CALLS
        Routine( L"268" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddUsage" );
    #endif
    // Add the internal text Usage as a top-level item.

    wchar_t * szFileUrl = L"file://InternalText/Help/Usage";
    CWsb WsbTemp;
    WsbTemp.AddnByte( ( BYTE * ) szUsage, strlen( szUsage ) );
    size_t nWBuf = NULL;
    wchar_t * pWBuf = WsbTemp.GetBuffer( & nWBuf );
    AddInternal( pWBuf, nWBuf, szFileUrl );
    MyFree( 739, ( nWBuf + 1 ) * sizeof( wchar_t ), pWBuf );
    pWBuf = NULL;

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddUsage" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::AddEzekiel( )
{
    #if DO_DEBUG_CALLS
        Routine( L"269" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddEzekiel" );
    #endif
    // Add the internal text Usage as a top-level item.

    wchar_t * szFileUrl = L"file://InternalText/Help/Ezekiel1";
    CWsb WsbTemp;
    WsbTemp.AddnByte( ( BYTE * ) szEz1, strlen( szEz1 ) );
    size_t nWBuf = NULL;
    wchar_t * pWBuf = WsbTemp.GetBuffer( & nWBuf );
    AddInternal( pWBuf, nWBuf, szFileUrl );
    MyFree( 739, ( nWBuf + 1 ) * sizeof( wchar_t ), pWBuf );
    pWBuf = NULL;

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddEzekiel" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::AddTeruah( )
{
    #if DO_DEBUG_CALLS
        Routine( L"311" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddTeruah" );
    #endif
    // Add the internal text Usage as a top-level item.

    wchar_t * szFileUrl = L"file://InternalText/Help/YomTeruah";
    CWsb WsbTemp;
    WsbTemp.AddnByte( ( BYTE * ) szTeruah1, strlen( szTeruah1 ) );
    WsbTemp.AddnByte( ( BYTE * ) szTeruah2, strlen( szTeruah2 ) );
    size_t nWBuf = NULL;
    wchar_t * pWBuf = WsbTemp.GetBuffer( & nWBuf );
    AddInternal( pWBuf, nWBuf, szFileUrl );
    MyFree( 739, ( nWBuf + 1 ) * sizeof( wchar_t ), pWBuf );
    pWBuf = NULL;

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddTeruah" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::AddRevelation( )
{
    #if DO_DEBUG_CALLS
        Routine( L"270" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddRevelation" );
    #endif

    // Add the internal text Revelation as a top-level item.

    wchar_t * szFileUrl = L"file://InternalText/Help/Revelation";
    CWsb WsbTemp;
    WsbTemp.AddnByte( ( BYTE * ) szRev1, strlen( szRev1 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev2, strlen( szRev2 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev3, strlen( szRev3 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev4, strlen( szRev4 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev5, strlen( szRev5 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev6, strlen( szRev6 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev7, strlen( szRev7 ) );
    WsbTemp.AddnByte( ( BYTE * ) szRev8, strlen( szRev8 ) );
    size_t nWBuf = NULL;
    wchar_t * pWBuf = WsbTemp.GetBuffer( & nWBuf );
    AddInternal( pWBuf, nWBuf, szFileUrl );
    MyFree( 739, ( nWBuf + 1 ) * sizeof( wchar_t ), pWBuf );
    pWBuf = NULL;

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddRevelation" );
    #endif
    SatLeaveCrisis( );
    return;
}


void CTop::AddRevNotes( )
{
    #if DO_DEBUG_CALLS
        Routine( L"270" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddRevNotes" );
    #endif

    // Add the internal text RevNotes as a top-level item.

    wchar_t * szFileUrl = L"file://InternalText/Help/RevelationNotes";
    CWsb WsbTemp;
    WsbTemp.AddnByte( ( BYTE * ) szRevNotes1, strlen( szRevNotes1 ) );
    // WsbTemp.AddnByte( ( BYTE * ) szRevNotes2, strlen( szRevNotes2 ) );
    // WsbTemp.AddnByte( ( BYTE * ) szRevNotes3, strlen( szRevNotes3 ) );
    size_t nWBuf = NULL;
    wchar_t * pWBuf = WsbTemp.GetBuffer( & nWBuf );
    AddInternal( pWBuf, nWBuf, szFileUrl );
    MyFree( 739, ( nWBuf + 1 ) * sizeof( wchar_t ), pWBuf );
    pWBuf = NULL;

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddRevNotes" );
    #endif
    SatLeaveCrisis( );
    return;
}


void CTop::AddInternal( wchar_t * pWBuf, size_t nWBuf, wchar_t * szFileUrl )
{
    #if DO_DEBUG_CALLS
        Routine( L"271" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->AddInternal" );
    #endif
    // I should be able to clone this from the successful
    // commonfileinput routine, just pass in Text and name.

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

            pOnePaper = new COnePaper( 0 ); // this one is for internal text.
            // Every making of a new COnePaper must create its pWsbAnnotation.
            pOnePaper->pWsbAnnotation = new CWsb;

            // This part parses, analyzes, the source paper.
            // 1 of 4 similar sequences. This one is for add internal.

            pOnePaper->HttpHeaderContentLanguage = BestIndexforLanguageString( L"en-US", NULL );
            pOnePaper->LanguageGroup = GroupIndexForLanguageIndex( pOnePaper->HttpHeaderContentLanguage );
            pOnePaper->HttpHeaderCharset = BestIndexforCharsetString( L"us-ascii", NULL );
            pOnePaper->HttpHeaderDateyyyymmdd = STAR_DATE;

            // This one is for Internal texts ...
            // The CHTM parse ruins my < and > characters!
            // So I just need second half of ProcessPaper.

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
                ProgramError( L"CommonPaperFileInput: ! CSolAllUrls.ClaimUserpVoid" );
            }
        }
    }

    // This is okay, in main thread, no worker thread involved:
    Top.Add( pOnePaper ); // Put fruit on display

    #if DO_DEBUG_TOP
        DebugTop( L"<-AddInternal" );
    #endif
    SatLeaveCrisis( );
    return;
}

static int BookmarkType[10];
static int BookmarkIndex[10];
static int BookmarkOffset[10];

void CTop::SetBookmark( size_t n )
{
    #if DO_DEBUG_CALLS
        Routine( L"325" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->SetBookmark" );
    #endif

    if( CurrentView != NULL )
    {
        BookmarkType[n] = CurrentView->pFruit->m_FruitType;
        BookmarkIndex[n] = CurrentView->pFruit->m_CSolIndex;
        BookmarkOffset[n] = See.GetOffsetofFocusLine( );
    }
    #if DO_DEBUG_TOP
        DebugTop( L"<-SetBookmark" );
    #endif
    SatLeaveCrisis( );
    return;
}

void CTop::VisitBookmark( size_t n )
{
    #if DO_DEBUG_CALLS
        Routine( L"324" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->VisitBookmark" );
    #endif

    CBud * pFruit = NULL;
    switch( BookmarkType[n] )
    {
    case ENUM_ONEPAPER:
        pFruit = ( CBud * ) CSolAllUrls.GetUserpVoid( BookmarkIndex[n] );
        if( pFruit != NULL
        && pFruit < PVOID_VALID_BELOW )
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;

    case ENUM_ADDDIRECTORY:
        pFruit = ( CBud * ) CSolUserFolders.GetUserpVoid( BookmarkIndex[n] );
        if( pFruit != NULL
        && pFruit < PVOID_VALID_BELOW )
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;

    case ENUM_ADDPAGE:
        pFruit = ( CBud * ) CSolUserFetchs.GetUserpVoid( BookmarkIndex[n] );
        if( pFruit != NULL
        && pFruit < PVOID_VALID_BELOW )
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;

    case ENUM_ADDSEARCH:
        pFruit = ( CBud * ) CSolUserQuerys.GetUserpVoid( BookmarkIndex[n] );
        if( pFruit != NULL
        && pFruit < PVOID_VALID_BELOW )
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;

    case ENUM_ADDFIND:
        pFruit = ( CBud * ) CSolUserKwics.GetUserpVoid( BookmarkIndex[n] );
        if( pFruit != NULL
        && pFruit < PVOID_VALID_BELOW )
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;

    case ENUM_ADDCACHE:
        pFruit = & OnlyCache;
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;

    case ENUM_ADDPHRASES:
        pFruit = & OnlyPhrases;
        {
            Add( pFruit );
            See.SetFocusLineToThisOffset( BookmarkOffset[n] );
        }
        break;
    }
    #if DO_DEBUG_TOP
        DebugTop( L"<-VisitBookmark" );
    #endif
    SatLeaveCrisis( );
    return;
}


void CTop::InvokeInternetExplorer( )
{
    // spawn functions are not supported on Win CE.
#ifndef _WIN32_WCE
    #if DO_DEBUG_CALLS
        Routine( L"480" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->InvokeIE" );
    #endif

    int okay = 1;

    if( CurrentView == NULL
    || CurrentView->pFruit->m_FruitType != ENUM_ONEPAPER )
    {
        okay = 0;
    }

    size_t UrlIndex = 0;
    if( okay )
    {
        UrlIndex = CurrentView->pFruit->m_CSolIndex;

        COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
        if( pOnePaper == NULL
        || pOnePaper > PVOID_VALID_BELOW )
        {
            okay = 0;
        }
    }

    wchar_t * pMalUrl = NULL;
    if( okay )
    {
        pMalUrl = ( wchar_t * ) CSolAllUrls.GetFullKey( UrlIndex );
        if( pMalUrl == NULL )
        {
            ProgramError( L"AddLinks: pMalUrl invalid" );
            #if DO_DEBUG_TOP
                DebugTop( L"<-InvokeIE" );
            #endif
            SatLeaveCrisis( );
            return;
        }

        // It's funny, a little line like this can crash a big program:
        // Bringing it in under this clause, so pMalUrl is never null:

        // If any FTP url can get to here, I will allow it to invoke IE:
        if( pMalUrl[1] != 't' ) // http, https, ftp, but not file method
        {
            okay = 0;
        }
    }

    if( ! okay )
    {
        Say( L"File, Iexplore, or Ctrl+I, will invoke Internet Explorer with URL of current view."
        L"\r\n\r\nHowever, the current view is not displaying a fetched/cached/opened web page file." );
        #if DO_DEBUG_TOP
            DebugTop( L"<-InvokeIE" );
        #endif
        SatLeaveCrisis( );
        return;
    }

    WIN32_FIND_DATA FindFileData;

    wchar_t * arg0 = L"iexplore";
    wchar_t * arg1 = pMalUrl;

    HANDLE hFindFile = FindFirstFile( IExploreFileName, & FindFileData );
    if( hFindFile == INVALID_HANDLE_VALUE )
    {
        // Use spawn LP variant which will search for command in user's path.
        // Say( L"Iexplore.exe was NOT found at expected location. Trying path." );

        wchar_t * cmdname = L"iexplore.exe";

        if( _wspawnlp( _P_NOWAIT, cmdname, arg0, arg1, NULL ) == -1 )
        {
            // Do not make a program error that causes exit.
            Say( L"Iexplore.exe was not found in your path.\r\n"
                L"You must locate iexplore.exe yourself, and add\r\n"
                L"the folder containing it to your execution path." );
        }
    }
    else
    {
        // Use spawn L variant with the full path and filename.
        // Say( L"Iexplore.exe WAS found at expected location." );

        wchar_t * cmdname = IExploreFileName;

        if( _wspawnl( _P_NOWAIT, cmdname, arg0, arg1, NULL ) == -1 )
        {
            // Do not make a program error that causes exit.
            Say( L"_wspawnl C:\\Program Files\\Internet Explorer\\iexplore.exe failed" );
        }
    }

    MyFree( 1217, zx, pMalUrl );
    #if DO_DEBUG_TOP
        DebugTop( L"<-InvokeIE" );
    #endif
    SatLeaveCrisis( );
    return;
#endif // not _WIN32_WCE
}

void CTop::RewrapCurrentTopItemViewOnScreen( )
{
    #if DO_DEBUG_CALLS
        Routine( L"483" );
    #endif
    SatEnterCrisis( );
    #if DO_DEBUG_TOP
        DebugTop( L"->RewrapCurrentTopItemViewOnScreen" );
    #endif

    // An easy clone from ShowCurrentTopItemViewOnScreen:
    // By only changing view text, re-view to reverse it.
    // But re-wrap only act on paper types:

    if( CurrentView != NULL
    && CurrentView->pFruit->m_FruitType == ENUM_ONEPAPER )
    {

        // preset for failure cases
        size_t TextOffset = 0;
        wchar_t * pMallocBuf = NULL;
        size_t nMallocBuf = 0; // Misnomer. Actually, is wcslen.

        // Copy this position to restore after re-writing.
        TextOffset = CurrentView->FocusLineOffsetInText;

        CBud * pFruit = CurrentView->pFruit;
        CWsb * pWsbResultText = NULL;

        if( pFruit != NULL )
            pWsbResultText = pFruit->pWsbResultText;

        if( pWsbResultText != NULL )
        {
            // I need to save the ORIGINAL strlen to test for growth.
            See.m_nFruitStrlenBackingDisplay = pWsbResultText->StrLen;
            pMallocBuf = pWsbResultText->GetBuffer( & nMallocBuf );
        }

        // So all I have to do is scan in that buffer, and
        // remove any single newlines before presenting it.
        // But sliding text would screw up the IDX data,
        // so merely change single CR+LF pairs to spaces.

        if( pMallocBuf != NULL
        && nMallocBuf > 0 )
        {
            wchar_t * scan = pMallocBuf;
            wchar_t * stop = pMallocBuf + nMallocBuf;
            for( ;; )
            {
                if( scan == stop )
                    break;
                if( scan[0] == '\r'
                &&  scan[1] == '\n'
                &&  scan[2] != '\r' )
                {
                    scan[0] = ' '; // overwrite CR
                    scan[1] = ' '; // overwrite LF
                }
                scan++;
            }
        }

        // Called routine consumes malloc. Do not free it here.
        // I have prepared it to erase in case of NULL or zero.

        See.SaveBackingText_MakeLineList( pMallocBuf, ( int ) nMallocBuf, NULL );

        See.SetFocusLineToThisOffset( TextOffset );
        // Which routine calls:
        // See.IMightHaveChangedTheDisplay( ); // Received new text
    }

    #if DO_DEBUG_TOP
        DebugTop( L"<-RewrapCurrentTopItemViewOnScreen" );
    #endif
    SatLeaveCrisis( );
    return;
}
