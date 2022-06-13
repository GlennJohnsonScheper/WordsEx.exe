// This is file: CFwd.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// The original intention of CFwd was to hold web pages,
// but they just got held, and parsed, and CFwd is empty.
// So I will use CFwd.cpp for the Kwic ( Find ) function.

CFwd::CFwd( size_t KwicIndex )
{
    #if DO_DEBUG_CALLS
        Routine( L"140" );
    #endif
    // The page content searching class was created by and for
    // the FIND ( aka KWIC ) class. Use the passed index to load
    // some of his settings into my class: k1-s2-p3 and stem.
    // Via index, get search key string, and COneKwic pointer.
    // After this, I use base pointer to log my find results.

    // In the adaptation of this class to perform Add Phrases,
    // Accept as KwicIndex 0, and not do some of these things:

    if( KwicIndex == 0 )
    {
        // This is for Add Phrases:

        m_Kwic1_Sentc2_Parag3 = 2;
        m_Stem = NULL;
        UserSearchString = NULL;
        pBudLog = ( CBud * ) & OnlyPhrases;
        m_pSolSearchTerms = NULL;

    }
    else
    {
        // This is for Add Find Word:

        COneKwic * pDerivedClass = ( COneKwic * ) CSolUserKwics.GetUserpVoid( KwicIndex );
        m_Kwic1_Sentc2_Parag3 = pDerivedClass->m_Kwic1_Sentc2_Parag3;
        m_Stem = pDerivedClass->m_Stem; // Owned by OneKwic; Copy pointer here.

        // Get the original Find parameter clicked or typed in by user.

        // Damn. A Hiding wchar_t * declaration here made access violation.
        // wchar_t * UserSearchString = CSolUserKwics.GetFullKey( KwicIndex );
        // Let that be a lesson to me.

        UserSearchString = CSolUserKwics.GetFullKey( KwicIndex );

        pBudLog = ( CBud * ) pDerivedClass;

        // This CSol holds parsed stemmed lowercased search token list:
        m_pSolSearchTerms = new CSol( CSOL_SCALAR );

        m_Pass1WordCount = 0;

    }
    #if DO_DEBUG_PAGE
        ; Spew( L"CFwd: Constructor" );
    #endif
    return;
}

CFwd::~CFwd( )
{
    #if DO_DEBUG_CALLS
        Routine( L"141" );
    #endif
    #if DO_DEBUG_PAGE
        ; Spew( L"CFwd: Destructor" );
    #endif

    if( m_pSolSearchTerms != NULL )
    {
        delete m_pSolSearchTerms;
        m_pSolSearchTerms = NULL;
    }

    if( UserSearchString != NULL )
    {
        MyFree( 311, UNPREDICTABLE, UserSearchString );
        UserSearchString = NULL;
    }

    pBudLog = NULL;
    return;
}

void CFwd::RunFindThread( )
{
    #if DO_DEBUG_CALLS
        Routine( L"142" );
    #endif
    #if DO_DEBUG_PAGE
        ; Spew( L"CFwd: RunFindThread start" );
    #endif

    if( pBudLog == NULL )
    {
        ProgramError( L"RunFindThread: pBudLog == NULL" );
        return;
    }

    // This IF is not checking if it is full, rather did it create.
    // This CSol holds parsed stemmed lowercased search token list.
    // FullText search stops at each token, lowercases, tries Find.
    if( m_pSolSearchTerms == NULL )
    {
        ProgramError( L"RunFindThread: m_pSolSearchTerms == NULL" );
        return;
    }

    // Let's start by breaking down the passed Find parameter string.
    // BreakDown, and the two Analysis routines, fill m_pSolSearchTerms.
    if( ! BreakDownTheFindParameterString( ) )
    {
        #if DO_DEBUG_PAGE
            ; Spew( L"CFwd: RunFindThread ! BreakDownParam" );
        #endif
        return;
    }

    // Now, we have the final search token list, m_pSolSearchTerms.
    // I will need to scan that list, getting keys many times.
    // So pull that work out of inner loop. Build some vector.

    size_t nSearchTerms = m_pSolSearchTerms->nList - 2; // including tail ( #0 ) and head ( #1 )
    wchar_t * * SearchTermPtrs = ( wchar_t * * ) MyMalloc( 63, nSearchTerms * sizeof( wchar_t * ) );
    size_t stop = m_pSolSearchTerms->nList; // including tail ( #0 ) and head ( #1 )
    size_t index = 2;
    size_t FillSearchTerms = 0;
    for( ;; )
    {
        if( index == stop )
            break;
        if( FillSearchTerms == nSearchTerms )
            break;

        SearchTermPtrs[ FillSearchTerms++ ] = m_pSolSearchTerms->GetFullKey( index );
        index ++;
    }

    // diagnostic:
    // FillSearchTerms = 0;
    // for( ;; )
    // {
    //     if( FillSearchTerms == nSearchTerms )
    //         break;
    //     pBudLog->pWsbResultText->Add( L"Vector holds:" );
    //     pBudLog->pWsbResultText->Add( SearchTermPtrs[ FillSearchTerms++ ] );
    //     pBudLog->pWsbResultText->Add( L"\r\n" );
    // }

    // There is no need for two phases, of vocabulary search followed
    // by FullText search, as I can't show anything until I rank them.

    CSol * pSolRankedMatchText = new CSol( CSOL_OBJECT ); // pass it around

    // I will look through all the URLs, as they hold the pOnePapers.
    CoIt * pMalUrlVector = CSolAllUrls.GetSortedVector( CSOL_FORWARD );
    if( pMalUrlVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalUrlVector + take++;
        if( pCoIt->IsSentinel )
            break;

        COnePaper * pOnePaper = ( COnePaper * ) pCoIt->User.pVoid;
        if( pOnePaper != NULL
        &&  pOnePaper < PVOID_VALID_BELOW )
        {
            // I found a paper, which is finished, and is neither
            // rejected, nor actively claimed by another process.

            // Now skip over any seach engine query result pages.
            if( pOnePaper->PageIsAQrpThisIsItsOrdinal == ZERO_ORDINAL_FOR_NON_QRP ) // no word-find in QRP.
            {

                // Iterate vector of lowercase search tokens,
                // looking each of them up in his vocabulary.
                CSol * pSolPageWordList = pOnePaper->pSolWordList;
                size_t TakeSearchTerms = 0;
                for( ;; )
                {
                    if( TakeSearchTerms == nSearchTerms )
                        break;

                    size_t index = pSolPageWordList->Find( SearchTermPtrs[ TakeSearchTerms ] );
                    #if DO_DEBUG_ADDFIND
                        if( index == 1 )
                            { Spew( L"AddFind 1 at cfwd 130" ); }
                    #endif
                    // A returned index of 1 means it matched the head,
                    // which means I wrongly looked up an empty string.

                    // A returned index of 0 means it matched the tail,
                    // which means the Find term was not found in list.

                    if( index > 1 )
                    {
                        // You caught it. What are you going to do with it?
                        // Pass the paper and output receiver to my helper.

                        FullTextSearch( pOnePaper, pSolRankedMatchText );

                        // One call to FullTextSearch looked for all tokens.
                        break; // from the inner loop across search terms.
                    }
                    TakeSearchTerms++;
                }
            }
        }
        if( g_bStopAllThreads || pBudLog->m_StopThisThread ) // for Add Find
            break;
    }
    MyFree( 135, UNPREDICTABLE, pMalUrlVector );
    pMalUrlVector = NULL;

    // Rid this malloc'ed vector of search key mallocs.
    size_t i = 0;
    for( ;; )
    {
        if( i == FillSearchTerms )
            break;
        MyFree( 210, UNPREDICTABLE, SearchTermPtrs[ i ] );
        SearchTermPtrs[ i ] = NULL;
        i++;
    }
    MyFree( 214, nSearchTerms * sizeof( wchar_t * ), SearchTermPtrs );
    SearchTermPtrs = NULL;


    // The loop above added fully rank-sorted
    // match text keys to pSolRankedMatchText.
    // The rank-sort is by a prefix to string.

    // There may be multiple instances of some
    // identical sentence, which rank the same.

    // To deal with multiple identical items, each Csol item
    // points to its own personal small CSol holding all the
    // particulars for all the instances of that text match.
    // These keys hold page score, page UrlIndex, and offset.

    // Now make an outer loop, recovering matches by their rankings,
    // and an inner loop, recovering all the instances' particulars.

    CoIt * pMalRankMatchVector = pSolRankedMatchText->GetSortedVector( CSOL_BACKWARD );
    if( pMalRankMatchVector == NULL )
        return;
    size_t take2 = 0;
    for( ;; )
    {
        // Process one item for the outer loop:
        CoIt * pCoIt = pMalRankMatchVector + take2++;
        if( pCoIt->IsSentinel )
            break;

        // The first 7 characters were ranking digits.
        // The rest of the key is the text in context.
        wchar_t * pMalRankedMatchKey = CoItFullKey( pCoIt );

        // Now, what is held in this pCoIt->User.pVoid?
        // Another pSol, to hold N identical instances.
        // The text is identical, just order the links.

        CSol * pSolLevelTwo = ( CSol * ) pCoIt->User.pVoid;
        CoIt * pMalRankPageVector = pSolLevelTwo->GetSortedVector( CSOL_BACKWARD );
        if( pMalRankPageVector == NULL )
            return;
        size_t take2 = 0;
        for( ;; )
        {
            // Process one item for the inner loop:
            CoIt * pCoIt2 = pMalRankPageVector + take2++ ;
            if( pCoIt2->IsSentinel )
                break;

            // This time we have page ranking, and access info:
            // Recover L"%09d%09d%09d", Rank, UrlOffset, UrlIndex

            wchar_t * RnkOffIndKey = CoItFullKey( pCoIt2 );
            size_t Rank = 0;
            size_t UrlOffset = 0;
            size_t UrlIndex = 0;
            int i = 0;
            for( ;; )
            {
                if( i == 9 )
                    break;
                Rank      *= 10;
                UrlOffset *= 10;
                UrlIndex  *= 10;
                Rank      += RnkOffIndKey[ i +  0 ] - '0';
                UrlOffset += RnkOffIndKey[ i +  9 ] - '0';
                UrlIndex  += RnkOffIndKey[ i + 18 ] - '0';
                i ++;
            }
            MyFree( 181, UNPREDICTABLE, RnkOffIndKey );
            RnkOffIndKey = NULL;

            // Add the same key text once per iteration to the result log.
            // Each time with new indexing value, although identical text.
            // As this is a user visible moment, update screen as we work.

            // In the cases of lengthy sentences and blocks,
            // I need to insert some sort of delimiter line.

            if( m_Kwic1_Sentc2_Parag3 > 1 )
                pBudLog->pWsbResultText->Add( L"\r\n-_-_-_-_-_-_-_-_-_-_-_-\r\n\r\n" );
            size_t StartOffset = pBudLog->pWsbResultText->StrLen;
            pBudLog->pWsbResultText->Add( pMalRankedMatchKey + 7 );
            size_t FinalOffset = pBudLog->pWsbResultText->StrLen;
            pBudLog->pIdxResultIndex->AddIdx( StartOffset, FinalOffset, UrlIndex, UrlOffset );
            pBudLog->pWsbResultText->Add( L"\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }

        MyFree( 194, UNPREDICTABLE, pMalRankPageVector );
        pMalRankPageVector = NULL;

        MyFree( 197, UNPREDICTABLE, pMalRankedMatchKey );
        pMalRankedMatchKey = NULL;

        if( g_bStopAllThreads || pBudLog->m_StopThisThread ) // for Add Find
            break;

    }
    MyFree( 200, UNPREDICTABLE, pMalRankMatchVector );
    pMalRankMatchVector = NULL;

    // Now this is a really spectacular moment:
    // pSolRankedMatchText is a CSol( CSOL_OBJECT )
    // Therefore, when I delete the CSol, it will
    // delete all the objects hanging from pVoids.

    delete pSolRankedMatchText;
    pSolRankedMatchText = NULL;

    #if DO_DEBUG_PAGE
        ; Spew( L"CFwd: RunFindThread done" );
    #endif
    return;
}

int CFwd::BreakDownTheFindParameterString( )
{
    #if DO_DEBUG_CALLS
        Routine( L"143" );
    #endif
    // return 1 if okay to search, 0 to quit.

    // 1. tokenize it; keep alnums, strip "'s", break at punctuation,
    // lowercase, keep length 1 to MAX_LEGITIMATE_WORD_LENGTH chars.
    // Should that be equal to one or more other places? E.g., In mouser?
    // WmMouseHandler did some, but user may have typed anything in field.
    // So do a left-to-right scan for alpha strings. Length 3 rids "'s".
    // What can be found in word lists? CTxt::SimpleWordScan says:
    // A prior loop might do the hypenated words and apostraphes.
    // Later, also put /alpha+alnums.../ in CTxt and CFwd methods.

    // 2. If stem, search CSolAllWords for any words with the same first char
    // as one of the tokens that stem to the same string as does the token.
    // If any found, this list of ~equal tokens replaces the orignal list.
    // If none found that stem to some token, keep user's original token.
    // Probably nothing could be finer than to use another CSol for that.

    // 3. Add such a search token list to the progress log.
    // 3. Next to each word, add the total occurence counts.

    // 4. If there are NO tokens parsed, say that.
    // 4. If there are NO texts to search, say that.
    // ( Note: the three INTERNAL texts are ignored. )
    // 4. If there are NO matches to find, say that. Suggest stem if not used.

    wchar_t * scan = UserSearchString;
    wchar_t * past = scan + wcslen( scan );

    // This loop does the strictly alpha tokens, writing nulls.
    // Keep identical code in these loop( s ) in CTxt and CFwd.

    wchar_t * atop = scan;
    int inalpha = 0;
    for( ;; )
    {
        if( scan == past )
            break;
        wchar_t c = *scan;
        if( iswalpha( c ) )
        {
            if( ! inalpha )
            {
                inalpha = 1;
                atop = scan;
            }
            if( iswupper( c ) )
                *scan = tolower( c );
        }
        else
        {
            if( inalpha )
            {
                inalpha = 0;
                *scan = NULL;
                size_t index = m_pSolSearchTerms->AddKey( atop );
                #if DO_DEBUG_ADDFIND
                    if( index == 1 )
                        { Spew( L"AddFind 1 at cfwd 304" ); }
                #endif
                m_pSolSearchTerms->IncrementUserValue( index );
            }
        }
        scan ++;
    }
    if( inalpha )
    {
        inalpha = 0;
        // I cannot write a NULL past end of buffer.
        // Oh, I can, because GetBuffer added NULL.
        *scan = NULL;
        size_t index = m_pSolSearchTerms->AddKey( atop );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cfwd 320" ); }
        #endif
        m_pSolSearchTerms->IncrementUserValue( index );
    }


    if( ! PostTokenScanAnalysis( ) )
        return 0;

    if( m_Stem )
    {
        if( ! TokenStemmingAnalysis( ) )
            return 0;
    }

    pBudLog->pWsbResultText->Add( L"\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pBudLog );

    return 1; // 1 = okay to search
}

int CFwd::PostTokenScanAnalysis( )
{
    #if DO_DEBUG_CALLS
        Routine( L"144" );
    #endif
    // return 1 if okay to search, 0 to quit.

    // Confused? Token Scan here refers to scanning a user query, not a page.

    // Having finished the loop that scanned input parameter,
    // and ( with or without doing stemming ) added to a CSol,
    // now spill that CSol to progress log. To generate the
    // word counts, I'll have to traverse CSolAllUrls' present
    // files, and do find in their word lists. I will make a
    // CSol ordering all those URL files to finally search.

    // Attach a CSol to receive this new URLs ( by index? ) list.
    // I don't need to traverse CSolAllUrls, but run List[3 - n-1].
    // I could sort them into my CSol by some page Q metric.

    // Rather than consult bool, this first step will always show token list
    // without stemming. Some later step will show another list with stemming.
    pBudLog->pWsbResultText->Add( L"Search tokens, without stemming:\r\n" );
    m_Pass1WordCount = 0;
    size_t Offset1 = pBudLog->pWsbResultText->StrLen;
    CoIt * pMalVector = m_pSolSearchTerms->GetSortedVector( CSOL_FORWARD );
    if( pMalVector == NULL )
        return 0;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;
        wchar_t * FullKey = CoItFullKey( pCoIt );
        size_t Strlen = wcslen( FullKey );

        // Remove the minimum length word test here:
        // No CoIt from GetSortedVector ever holds an empty string.
        // if( Strlen > 2 ...
        if( Strlen < MAX_LEGITIMATE_WORD_LENGTH )
        {
            size_t MatchingWordCount = 0;
            size_t Index = CSolAllWords.Find( FullKey );
            #if DO_DEBUG_ADDFIND
                if( Index == 1 )
                    { Spew( L"AddFind 1 at cfwd 378" ); }
            #endif
            if( Index > 1 )
            {
                // 8 MSB of UserValue hold an empirical measure of
                // the commonality of the letter sequences in word.

                // I slipped in 7 next bits as a language group id.

                // 17 LSB of UserValue hold relative word frequency.

                // Notice that although CSols's User.Value is signed,
                // I am working in my own copied unsigned int variable.

                MatchingWordCount = CSolAllWords.GetUserValue( Index );
                MatchingWordCount &= MASK_17BIT_WORD_COUNT;
                m_Pass1WordCount += MatchingWordCount;
            }
            wchar_t wk[ MAX_LEGITIMATE_WORD_LENGTH + 80 ];
            wsprintf( wk, L"%s - %d match%s\r\n", FullKey, MatchingWordCount,
                ( MatchingWordCount == 1 ? L"" : L"es" ) );
            pBudLog->pWsbResultText->Add( wk );
        }
        MyFree( 372, zx, FullKey );
        FullKey = NULL;
    }
    MyFree( 375, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;
    size_t Offset2 = pBudLog->pWsbResultText->StrLen;
    pBudLog->pWsbResultText->Add( L"\r\n" );

    if( Offset1 == Offset2 )
    {
        pBudLog->pWsbResultText->Add( L"There were no suitable tokens in user search string.\r\n\r\n" );
        return 0;
    }

    if( m_Pass1WordCount == 0
    && ! m_Stem )
    {
        pBudLog->pWsbResultText->Add(
            L"\r\n\r\nThere were no token matches in any texts held in memory.\r\n\r\n" );
        return 0;
    }
    return 1;
}

int CFwd::TokenStemmingAnalysis( )
{
    #if DO_DEBUG_CALLS
        Routine( L"145" );
    #endif
    // After finding all words whose stems match a pass 1 stem,
    // do like PostTokenScanAnalysis which preceeded this call,
    // add the words found and their counts of matches to pBudLog.

    m_Pass1WordCount = 0; // We will discard the original list.

    pBudLog->pWsbResultText->Add( L"Search tokens, matches after stemming:\r\n" );

    // One instance of this class can serve my thread over and over.
    CPor Porter;

    // This first, outer, loop runs through all of the user tokens,
    // producing a list of stems to test against CSolAllWords, stemmed.
    // Place all the test stems into a local CSol ( or a better way? )

    CSol * pSolStemList = new CSol( CSOL_SCALAR );

    size_t stop = m_pSolSearchTerms->nList; // including tail ( #0 ) and head ( #1 )
    size_t index = 2;
    for( ;; )
    {
        if( index == stop )
            break;

        wchar_t * pMalToken = m_pSolSearchTerms->GetFullKey( index );
        if( pMalToken != NULL )
        {
            Porter.Stem( pMalToken, wcslen( pMalToken ) );
            size_t index2 = pSolStemList->AddKey( pMalToken );
            #if DO_DEBUG_ADDFIND
                if( index2 == 1 )
                    { Spew( L"AddFind 1 at cfwd 447" ); }
            #endif
            MyFree( 436, zx, pMalToken );
            pMalToken = NULL;
        }

        index ++;
    }

    // This second, outer, loop runs through vocabulary, CSolAllWords.
    // testing each word stem against CSol of stems created above.
    // Place all the matches into another CSol ( or a better way? )
    // At the same time, augment a count of matches for each stem.

    CSol * pSolMatchList = new CSol( CSOL_SCALAR );

    // This time, traverse CSolAllWords in sorted order, to annotate.
    CoIt * pMalVector = CSolAllWords.GetSortedVector( CSOL_FORWARD );
    if( pMalVector == NULL )
        return 0;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;
        wchar_t * pMalToken = CoItFullKey( pCoIt );
        size_t Strlen = wcslen( pMalToken );

        // Remove the minimum length word test here:
        // No CoIt from GetSortedVector ever holds an empty string.
        // if( Strlen > 2 ...
        if( Strlen < MAX_LEGITIMATE_WORD_LENGTH )
        {
            // Stem this CSolAllWords item, test if matches.
            Porter.Stem( pMalToken, wcslen( pMalToken ) );

            size_t index2 = pSolStemList->Find( pMalToken );
            #if DO_DEBUG_ADDFIND
                if( index2 == 1 )
                    { Spew( L"AddFind 1 at cfwd 482" ); }
            #endif
            if( index2 != NULL )
            {
                // This suitable word's stem matched.
                // The stemmer hurt my token in place.
                // Get a second copy of matching word.
                // I do not have an index in CSolAllWords.
                wchar_t * pMalToken2 = CoItFullKey( pCoIt ); // a malloc, user frees
                size_t index = CSolAllWords.Find( pMalToken2 );
                #if DO_DEBUG_ADDFIND
                    if( index == 1 )
                        { Spew( L"AddFind 1 at cfwd 494" ); }
                #endif
                if( index < 2 )
                {
                    ProgramError( L"TokenStemmingAnalysis: index < 2" );
                }
                size_t index2 = pSolMatchList->AddKey( pMalToken2 );
                #if DO_DEBUG_ADDFIND
                    if( index2 == 1 )
                        { Spew( L"AddFind 1 at cfwd 503" ); }
                #endif
                size_t WordCount = CSolAllWords.GetUserValue( index );
                WordCount &= MASK_17BIT_WORD_COUNT;
                m_Pass1WordCount += WordCount;
                wchar_t wk[ MAX_LEGITIMATE_WORD_LENGTH + 80 ];
                wsprintf( wk, L"%s - %d match%s\r\n", pMalToken2, WordCount,
                    ( WordCount == 1 ? L"" : L"es" ) );
                pBudLog->pWsbResultText->Add( wk );
                MyFree( 484, zx, pMalToken2 );
            }
        }
        MyFree( 487, zx, pMalToken );
        pMalToken = NULL;
    }

    MyFree( 491, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    // The new auto pSolMatchList will replace my member, m_pSolSearchTerms.
    {
        CSol * Temp = m_pSolSearchTerms; // copy before delete
        m_pSolSearchTerms = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    m_pSolSearchTerms = pSolMatchList; // consuming pSolMatchList, so no delete & NULL.

    {
        CSol * Temp = pSolStemList; // copy before delete
        pSolStemList = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    if( m_Pass1WordCount == 0 )
    {
        pBudLog->pWsbResultText->Add(
            L"\r\n\r\nThere were no token matches in any texts held in memory.\r\n\r\n" );
        return 0;
    }

    return 1;
}

void CFwd::FullTextSearch( COnePaper * pOnePaper, CSol * pSolRankedMatchText )
{
    #if DO_DEBUG_CALLS
        Routine( L"146" );
    #endif
    // The Find process has found one or more of the member
    // m_pSolSearchTerms words in word list of the passed pOnePaper.
    // I need to make an effective text search for any of
    // the search terms in the pOnePaper's pWsbResultText,
    // and save some kind of ( kwic, sentence, block ) CSol
    // item, noting the pOnePaper text offset, which items
    // will later enter the pBudLog with an index = offset.
    // I need case insensitivity. Search terms are lowered.
    // I will scan the text once, and upon every alpha run,
    // lowercase the token, search for it in m_pSolSearchTerms.
    // Suppose I limit max token length, and copy to array.
    // I need to first skip over the header annotations.

    wchar_t token[ MAX_LEGITIMATE_WORD_LENGTH + 1 ]; // 1 for NULL
    wchar_t * fill = token;

    size_t nMallocBuf;
    wchar_t * pMallocBuf = pOnePaper->pWsbResultText->GetBuffer( & nMallocBuf );
    wchar_t * scan = pMallocBuf;

    // This is the FIND operation in CPAG,
    // versus the CTXT analysis operation,
    // and NOW there is header annotation.
    // Paper is hung, and has a score too.

    // Get these out to pass to my 3 helpers:
    size_t PageRank = pOnePaper->PageRankingScore;
    size_t UrlIndex = pOnePaper->m_CSolIndex;

    // first loop will skip over header annotations.
    // That is, ( now ), advance past FOUR newlines.

    // Aha! That's my search-finding-header-words problem:
    // Now, with title line, there are FIVE newlines atop.

    int CountNL = 0;
    for( ;; )
    {
        if( *scan == NULL )
            break;
        if( scan[0] == '\r'
        &&  scan[1] == '\n' )
        {
            if( ++CountNL == 5 ) // URL, title, date, score, terms, ...
                break;
        }
        scan ++;
    }

    // Remember this point to stop backwards scans.
    size_t AtopText = scan - pMallocBuf;

    // second loop will scan the full text, right now:
    for( ;; )
    {
        // outer loop will alternate non-alpha and alpha loops
        if( *scan == NULL )
            break;
        for( ;; )
        {
            // This inner loop will skip over non-alpha
            if( iswalpha( *scan ) )
                break;
            if( *scan == NULL )
                break;
            scan ++;
        }
        if( *scan == NULL )
            break;

        fill = token;
        for( ;; )
        {
            // This inner loop will process alpha runs, ask if match.
            if( ! iswalpha( *scan ) )
            {
                *fill = NULL;
                // test the lowercased token[], if 3 to 59 chars.

                // Remove the minimum length word test here:
                // But I may need to keep the non-empty test.
                if( fill - token > 0
                &&  fill - token < MAX_LEGITIMATE_WORD_LENGTH )
                {
                    size_t index = m_pSolSearchTerms->Find( token );
                    #if DO_DEBUG_ADDFIND
                        if( index == 1 )
                            { Spew( L"AddFind 1 at cfwd 627" ); }
                    #endif
                    if( index )
                    {
                        // Token was found at scan - strlen( token ).
                        // Work as signed int, due to <0 test, etc.
                        // Pass as an offset the midpoint of token.
                        int offset = ( scan - pMallocBuf ) - ( fill - token ) / 2;

                        // diagnostic:
                        // wchar_t wk[100];
                        // wsprintf( wk, L"word %s found at offset %d.\r\n", token, offset );
                        // pBudLog->pWsbResultText->Add( wk );

                        // What shall I do with it? Sell the baby.
                        // I have a text offset. Save that with
                        // pOnePaper, or pOnePaper->m_CSolIndex.

                        // Kwic can be according to screen size.
                        // Sentence requires prior scanned Idx.
                        // Block can be between double newlines.

                        // Each version's created text will
                        // go into an array for sorting by Q,
                        // by scanning token values or 1/counts.

                        // To avoid adding a new CBud member,
                        // I will pass in CSol output receiver,
                        // which I will receive from my caller.

                        // None of my helpers need the pOnePaper,
                        // just its UrlIndex and Score to record.

                        // These three critters will work in the
                        // passed buffer, back and forth from the
                        // matched offset, back only so far as the
                        // AtopText offset, and forward until NULL.
                        //
                        // Once either sentence or block has a hit,
                        // those critters must return a new offset
                        // past sentence or block to resume search.

                        int newoffset = offset;

                        switch( m_Kwic1_Sentc2_Parag3 )
                        {
                        case 1:
                            TreatAsKwic( PageRank, UrlIndex, pMallocBuf, AtopText, offset, pSolRankedMatchText );
                            break;

                        case 2:
                            newoffset = TreatAsSentence( PageRank, UrlIndex, pMallocBuf, AtopText, offset, pSolRankedMatchText, pOnePaper->pIdxSentences );
                            break;

                        case 3:
                            newoffset = TreatAsParagraph( PageRank, UrlIndex, pMallocBuf, AtopText, offset, pSolRankedMatchText, pOnePaper->pIdxTextBlocks );
                            break;
                        }
                        scan = pMallocBuf + newoffset;
                    }
                }
                break;
            }
            if( *scan == NULL )
                break;
            if( fill < token + MAX_LEGITIMATE_WORD_LENGTH )
            {
                if( iswupper( *scan ) )
                    *fill++ = towlower( *scan ); // UCS safe lowercasing
                else
                    *fill++ = *scan;
            }
            scan ++;
        }
    }

    MyFree( 649, zx, pMallocBuf );
    pMallocBuf = NULL;
    return;
}

void CFwd::TreatAsKwic( size_t PageRank, size_t UrlIndex, wchar_t * pMallocBuf, int AtopText, int offset, CSol * pSolRankedMatchText )
{
    #if DO_DEBUG_CALLS
        Routine( L"147" );
    #endif
    // scan chars backword, adding sizes
    // until just under screen width/2,
    // then scan forward same way.

    // I think I will store in two auto arrays of some max size, say, 1000.
    // Char width range on my cpu is 4 to 21, and maximized screen 1006 LU.
    // Change whitespace runs to single space. Pad backwards if ends short.

#define HALF_LINE_MAX_CHARS 1000

    wchar_t Backwards[ HALF_LINE_MAX_CHARS ];
    size_t FillBackwards = HALF_LINE_MAX_CHARS;
    Backwards[ --FillBackwards ] = NULL; // string terminating NULL

    wchar_t Forwards[ HALF_LINE_MAX_CHARS ];
    size_t FillForwards = 0;

    // inputs:
    // size_t g_KwicTextWidthLimit ( 48% of screen width )
    // unsigned short g_SomeGlyphWidths[SOME_GLYPHS];
    // offset / pMallocBuf;

    size_t space_LU = g_SomeGlyphWidths[ ' ' ];

    // First loop will scan backward from offset.
    int i = offset;
    int InSpaces = 0;
    size_t TextWidth = 0;
    for( ;; )
    {
        // pre-decrement to not do first char of token.
        if( --i < AtopText )
        {
            // At i == AtopText-1, make a padding loop, and quit.
            for( ;; )
            {
                if( FillBackwards == 0 )
                    break;
                if( TextWidth > g_KwicTextWidthLimit )
                    break;
                TextWidth += space_LU;
                Backwards[ --FillBackwards ] = ' ';
            }
            break;
        }

        if( FillBackwards == 0 )
            break;
        if( TextWidth > g_KwicTextWidthLimit )
            break;

        wchar_t c = pMallocBuf[ i ];

        if( c == ' ' || c == '\r' || c == '\n'  )
        {
            if( ! InSpaces )
            {
                InSpaces = 1;
                TextWidth += space_LU;
                Backwards[ --FillBackwards ] = ' ';
            }
        }
        else
        {
            InSpaces = 0;
            if( c < SOME_GLYPHS )
                TextWidth += g_SomeGlyphWidths[ c ];
            else
                TextWidth += g_WidestGlyphWidth;

            Backwards[ --FillBackwards ] = c;
        }
    }

    // Second loop will scan forward from offset.
    i = offset;
    InSpaces = 0;
    TextWidth = 0;
    for( ;; )
    {
        // post-increment to do first char of token.

        wchar_t c = pMallocBuf[ i++ ];
        if( c == NULL )
            break;

        if( FillForwards == HALF_LINE_MAX_CHARS - 1 )
            break;

        if( TextWidth > g_KwicTextWidthLimit )
            break;

        if( c == ' ' || c == '\r' || c == '\n'  )
        {
            if( ! InSpaces )
            {
                InSpaces = 1;
                TextWidth += space_LU;
                Forwards[ FillForwards++ ] = ' ';
            }
        }
        else
        {
            InSpaces = 0;
            if( c < SOME_GLYPHS )
                TextWidth += g_SomeGlyphWidths[ c ];
            else
                TextWidth += g_WidestGlyphWidth;
            Forwards[ FillForwards++ ] = c;
        }
    }

    Forwards[ FillForwards ] = NULL; // string terminating NULL

    // Assemble text. The easiest way is to create a CWsb.
    {
        CWsb KwicPhrase;
        KwicPhrase.Add( Backwards + FillBackwards );
        KwicPhrase.Add( Forwards );
        size_t nMalBuf;
        wchar_t * pMalBuf = KwicPhrase.GetBuffer( & nMalBuf );

        // Even in this case of KWIC format, instead of logging
        // into RankItem the exact offset of the word matched,
        // I will log an adjusted offset of whole KWIC context.

        RankItem( pSolRankedMatchText, pMalBuf, nMalBuf, PageRank, UrlIndex, offset );
        MyFree( 819, zx, pMalBuf );
        pMalBuf = NULL;
    }
    return;
}

int CFwd::TreatAsSentence( size_t PageRank, size_t UrlIndex, wchar_t * pMallocBuf, int AtopText, int offset, CSol * pSolRankedMatchText, CIdx * pIdxSentences )
{
    #if DO_DEBUG_CALLS
        Routine( L"148" );
    #endif
    // Only process such matches as fall within the boundaries
    // of a slot in the passed IDX vector, by calling ScanIdxToTestRange.
    // Return offset at end of it, or unchanged if do nothing.
    // Find sentence or block only use index, to hold a score.

    size_t Lookup = offset;

    size_t Score;
    size_t Unused;
    size_t nAtopText;
    size_t nPastText;

    if( pIdxSentences->ScanIdxToTestRange( Lookup, & Score, & Unused, & nAtopText, & nPastText ) )
    {
        // That successful result means the four passed variables were filled.
        // Add the selected sentence into the ranking system for presentation.

        wchar_t * pText = pMallocBuf + nAtopText;
        size_t nText = nPastText - nAtopText;
        RankItem( pSolRankedMatchText, pText, nText, Score, UrlIndex, nAtopText );
        offset = nPastText;
    }

    return offset;
}

int CFwd::TreatAsParagraph( size_t PageRank, size_t UrlIndex, wchar_t * pMallocBuf, int AtopText, int offset, CSol * pSolRankedMatchText, CIdx * pIdxTextBlocks )
{
    #if DO_DEBUG_CALLS
        Routine( L"149" );
    #endif
    // Only process such matches as fall within the boundaries
    // of a slot in the passed IDX vector, by calling ScanIdxToTestRange.
    // Return offset at end of it, or unchanged if do nothing.
    // Find sentence or block only use index, to hold a score.

    size_t Lookup = offset;

    size_t Score;
    size_t Unused;
    size_t nAtopText;
    size_t nPastText;

    if( pIdxTextBlocks->ScanIdxToTestRange( Lookup, & Score, & Unused, & nAtopText, & nPastText ) )
    {
        // That successful result means the four passed variables were filled.
        // Add the selected sentence into the ranking system for presentation.

        wchar_t * pText = pMallocBuf + nAtopText;
        size_t nText = nPastText - nAtopText;
        RankItem( pSolRankedMatchText, pText, nText, Score, UrlIndex, nAtopText );
        offset = nPastText;
    }

    return offset;
}

void CFwd::RankItem( CSol * pSolRankedMatchText, wchar_t * Text, size_t StrLen, size_t UrlQ, size_t UrlIndex, size_t UrlOffset )
{
    #if DO_DEBUG_CALLS
        // Routine( L"150" );
    #endif
    // Malloc to hold ranking and text.
    // Copy the text to end of malloc.
    // Then deal with the text ranking.
    // Format the ranking atop malloc.
    // By whatever computation, fit 7.

    size_t nMalKey = 7 + StrLen + 1;
    wchar_t * pMalKey = ( wchar_t * ) MyMalloc( 1000, nMalKey * sizeof( wchar_t ) );
    memcpy( pMalKey + 7, Text, StrLen * sizeof( wchar_t ) );
    pMalKey [ 7 + StrLen ] = NULL;

    // I shall scan the text for alpha runs, and value each word
    // by 1/N occurrences if found, or 0 if not found, summing
    // those reciprocals of count into a float across all words.
    // I will take a log of that Q, offset it to positive range.

    double SumInv = 0.0;

    wchar_t token[ MAX_LEGITIMATE_WORD_LENGTH + 1 ]; // 1 for NULL
    wchar_t * fill = token;

    wchar_t * scan = pMalKey + 7;
    wchar_t * AtopWord = scan;
    for( ;; )
    {
        // outer loop will alternate non-alpha and alpha loops
        if( *scan == NULL )
            break;
        for( ;; )
        {
            // This inner loop will skip over non-alpha
            if( iswalpha( *scan ) )
                break;
            if( *scan == NULL )
                break;
            scan ++;
        }
        if( *scan == NULL )
            break;

        fill = token;
        for( ;; )
        {
            // This inner loop will process alpha runs
            if( ! iswalpha( *scan ) )
            {
                *fill = NULL;
                // test the lowercased token[], if 3 to 59 chars.
                // Remove the minimum length word test here:
                // But I may need to keep the non-empty test.
                if( fill - token > 0
                &&  fill - token < MAX_LEGITIMATE_WORD_LENGTH )
                {
                    size_t index = CSolAllWords.Find( token );
                    #if DO_DEBUG_ADDFIND
                        if( index == 1 )
                            { Spew( L"AddFind 1 at cfwd 918" ); }
                    #endif
                    if( index != NULL )
                    {
                        int AllCount = CSolAllWords.GetUserValue( index );
                        AllCount &= MASK_17BIT_WORD_COUNT;

                        // Now I would like to diminish the value
                        // of common words. Since in this routine,
                        // a high number means less valuable, add
                        // diluting count from CSolCommonWords, if found.

                        size_t index2 = CSolCommonWords.Find( token );
                        #if DO_DEBUG_ADDFIND
                            if( index2 == 1 )
                                { Spew( L"AddFind 1 at cfwd 933" ); }
                        #endif
                        if( index2 != NULL )
                        {
                            size_t ComnCount = CSolCommonWords.GetUserValue( index2 );
                            ComnCount &= MASK_17BIT_WORD_COUNT;
                            AllCount += ComnCount;
                        }

                        // I hate to invoke floating point math.
                        if( AllCount > 0 )
                        {
                            double qty = ( double ) AllCount;
                            double inv = 1.0 / qty;
                            SumInv += inv;
                        }
                    }
                }
                break;
            }
            if( *scan == NULL )
                break;
            if( fill < token + MAX_LEGITIMATE_WORD_LENGTH )
            {
                if( iswupper( *scan ) )
                    *fill++ = towlower( *scan ); // UCS safe lowercasing
                else
                    *fill++ = *scan;
            }
            scan ++;
        }
    }

    // For zero in, produce zero out.
    size_t QMetric = 0;

    if( SumInv > 0 )
    {
        double LogSum = log10( SumInv );

        // What is a reasonable range of logarithms?
        // If every word is unique, and lots of them,
        // like a dictionary file, it could be as big
        // as the log of word count, say, 5 of 100000.
        // If text has one word of 100000 counts, -5.

        // To be generous, add 20, to range 15 to 25.
        // Then multiply by 10000 into max at 250000.

        double Qdouble = ( LogSum + 20.0 ) * 10000.0;
        // scary stuff. Limit range to 6 digits, +.
        if( Qdouble > 400000.0 )
            Qdouble = 400000.0;
        if( Qdouble < 0.0 )
            Qdouble = 0.0;
        QMetric = ( size_t ) Qdouble;
    }

    // format that number into all of the top 7 characters, no NULL.
    pMalKey[0] = ( wchar_t ) ( QMetric / 1000000 % 10 + '0' );
    pMalKey[1] = ( wchar_t ) ( QMetric / 100000 % 10 + '0' );
    pMalKey[2] = ( wchar_t ) ( QMetric / 10000 % 10 + '0' );
    pMalKey[3] = ( wchar_t ) ( QMetric / 1000 % 10 + '0' );
    pMalKey[4] = ( wchar_t ) ( QMetric / 100 % 10 + '0' );
    pMalKey[5] = ( wchar_t ) ( QMetric / 10 % 10 + '0' );
    pMalKey[6] = ( wchar_t ) ( QMetric % 10 + '0' );

    // The pMalKey ( rank + Find result text ) is complete.
    // Add it to the passed pSolRankedMatchText.
    size_t index = pSolRankedMatchText->AddKey( pMalKey );
    #if DO_DEBUG_ADDFIND
        if( index == 1 )
            { Spew( L"AddFind 1 at cfwd 1000" ); }
    #endif
    // And I am done with the text malloc.
    MyFree( 959, zx, pMalKey );
    pMalKey = NULL;

    // Hang one Csol per unique match result key, to rank pages/indexs.

    // I now hold an index, which may have been freshly added.
    // If it was freshly added, I need to hang a CSol from it.

    CSol * pSolLevelTwo = ( CSol * ) pSolRankedMatchText->GetUserpVoid( index );
    if( pSolLevelTwo == NULL )
    {
        pSolLevelTwo = new CSol( CSOL_SCALAR ); // to hold indexing data
        pSolRankedMatchText->SetUserpVoid( index, pSolLevelTwo );
    }

    // In that lower level CSol, I must format the information to
    // rank, and to get, and set offset into, the referenced text.
    // Format a key. It will be unique due to the offset of match.

    wchar_t RoiKey[40];
    wsprintf( RoiKey, L"%09d%09d%09d", UrlQ, UrlOffset, UrlIndex );
    size_t index2 = pSolLevelTwo->AddKey( RoiKey );
    #if DO_DEBUG_ADDFIND
        if( index2 == 1 )
            { Spew( L"AddFind 1 at cfwd 1027" ); }
    #endif
    return;
}

void CFwd::RunPhrasesThread( )
{
    #if DO_DEBUG_CALLS
        Routine( L"307" );
    #endif
    #if DO_DEBUG_PAGE
        ; Spew( L"CFwd: RunPhrasesThread start" );
    #endif

    // This should be an easy clone based on RunFindThread.

    // This will hold the sentences as they are being ranked:

    CSol * pSolRankedMatchText = new CSol( CSOL_OBJECT ); // pass it around

    // I will look through all the URLs, as they hold the pOnePapers.
    CoIt * pMalUrlVector = CSolAllUrls.GetSortedVector( CSOL_FORWARD );
    if( pMalUrlVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalUrlVector + take++;
        if( pCoIt->IsSentinel )
            break;

        COnePaper * pOnePaper = ( COnePaper * ) pCoIt->User.pVoid;
        if( pOnePaper != NULL
        &&  pOnePaper < PVOID_VALID_BELOW )
        {
            // I found a paper, which is finished, and is neither
            // rejected, nor actively claimed by another process.

            // Not like Add Find;
            // I do not need to search for words, then full text;
            // Just iterate paper's IDX to collect all sentences.

            // This is a variation on the TreatAsSentence routine:
            // Instead of calling ScanIdxToTestRange with an Offset;
            // Call some variant ReturnIdxSlotRange passing an index;
            // Find sentence or block only use index, to hold a score.

            size_t UrlIndex = pOnePaper->m_CSolIndex;

            size_t nMallocBuf;
            wchar_t * pMallocBuf = pOnePaper->pWsbResultText->GetBuffer( & nMallocBuf );

            int i = 0;
            for( ;; )
            {
                size_t Score;
                size_t nAtopText;
                size_t nPastText;

                if( pOnePaper->pIdxSentences->ReturnIdxSlotRange( i, & Score, & nAtopText, & nPastText ) )
                {
                    // That successful result means the four passed variables were filled.
                    // Add the selected sentence into the ranking system for presentation.

                    // Perhaps due to unripe sentence recognition heuristics,
                    // I reached here with a final 20 index of nSentences 21
                    // returning Past < Atop, which of course became a StrLen
                    // of about 0xffffffe9, and malloc failed, returning 0.
                    // So, text all four points: Also eliminate any empties.

                    if( nPastText > nAtopText + 1
                    && nPastText <= nMallocBuf )
                    {
                        wchar_t * pText = pMallocBuf + nAtopText;
                        size_t nText = nPastText - nAtopText;
                        RankItem( pSolRankedMatchText, pText, nText, Score, UrlIndex, nAtopText );
                    }
                    else
                    {
                        // Do not hazard user with program errors.
                        #if DO_DEBUG_PHRASE
                            if( nPastText <= nAtopText + 1 )
                            {
                                ; Spew( L"AddPhrases: nPastText <= nAtopText + 1" );
                            }
                            if( nPastText > nMallocBuf )
                            {
                                ; Spew( L"AddPhrases: nPastText > nMallocBuf" );
                            }
                        #endif

                    }
                }
                else
                {
                    break;
                }
                i ++;
            }
            MyFree( 1251, zx, pMallocBuf );
            pMallocBuf = NULL;
        }

        if( g_bStopAllThreads || OnlyPhrases.m_StopThisThread )
            break;
    }
    MyFree( 135, UNPREDICTABLE, pMalUrlVector );
    pMalUrlVector = NULL;

    // The rest of this routine should remain idential to ADD FIND,
    // Oh, except to stop after showing some... g_PhrasesPercent.

    size_t StopCount = 1 + ( g_PhrasesPercent * TotalPhraseCount ) / 100;
    size_t LoopCount = 0;

    // The loop above added fully rank-sorted
    // match text keys to pSolRankedMatchText.
    // The rank-sort is by a prefix to string.

    // There may be multiple instances of some
    // identical sentence, which rank the same.

    // To deal with multiple identical items, each Csol item
    // points to its own personal small CSol holding all the
    // particulars for all the instances of that text match.
    // These keys hold page score, page UrlIndex, and offset.

    // Now make an outer loop, recovering matches by their rankings,
    // and an inner loop, recovering all the instances' particulars.

    CoIt * pMalRankMatchVector = pSolRankedMatchText->GetSortedVector( CSOL_BACKWARD );
    if( pMalRankMatchVector == NULL )
        return;
    size_t take2 = 0;
    for( ;; )
    {
        // Process one item for the outer loop:
        CoIt * pCoIt = pMalRankMatchVector + take2++;
        if( pCoIt->IsSentinel )
            break;

        // The first 7 characters were ranking digits.
        // The rest of the key is the text in context.
        wchar_t * pMalRankedMatchKey = CoItFullKey( pCoIt );

        // Now, what is held in this pCoIt->User.pVoid?
        // Another pSol, to hold N identical instances.
        // The text is identical, just order the links.

        CSol * pSolLevelTwo = ( CSol * ) pCoIt->User.pVoid;
        CoIt * pMalRankPageVector = pSolLevelTwo->GetSortedVector( CSOL_BACKWARD );
        if( pMalRankPageVector == NULL )
            return;
        size_t take2 = 0;
        for( ;; )
        {
            // Process one item for the inner loop:
            CoIt * pCoIt2 = pMalRankPageVector + take2++ ;
            if( pCoIt2->IsSentinel )
                break;

            // This time we have page ranking, and access info:
            // Recover L"%09d%09d%09d", Rank, UrlOffset, UrlIndex

            wchar_t * RnkOffIndKey = CoItFullKey( pCoIt2 );
            size_t Rank = 0;
            size_t UrlOffset = 0;
            size_t UrlIndex = 0;
            int i = 0;
            for( ;; )
            {
                if( i == 9 )
                    break;
                Rank      *= 10;
                UrlOffset *= 10;
                UrlIndex  *= 10;
                Rank      += RnkOffIndKey[ i +  0 ] - '0';
                UrlOffset += RnkOffIndKey[ i +  9 ] - '0';
                UrlIndex  += RnkOffIndKey[ i + 18 ] - '0';
                i ++;
            }
            MyFree( 181, UNPREDICTABLE, RnkOffIndKey );
            RnkOffIndKey = NULL;

            // Add the same key text once per iteration to the result log.
            // Each time with new indexing value, although identical text.
            // As this is a user visible moment, update screen as we work.

            // In the cases of lengthy sentences and blocks,
            // I need to insert some sort of delimiter line.

            if( m_Kwic1_Sentc2_Parag3 > 1 )
                pBudLog->pWsbResultText->Add( L"\r\n-_-_-_-_-_-_-_-_-_-_-_-\r\n\r\n" );
            size_t StartOffset = pBudLog->pWsbResultText->StrLen;
            pBudLog->pWsbResultText->Add( pMalRankedMatchKey + 7 );
            size_t FinalOffset = pBudLog->pWsbResultText->StrLen;
            pBudLog->pIdxResultIndex->AddIdx( StartOffset, FinalOffset, UrlIndex, UrlOffset );
            pBudLog->pWsbResultText->Add( L"\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );

        }

        MyFree( 194, UNPREDICTABLE, pMalRankPageVector );
        pMalRankPageVector = NULL;

        MyFree( 197, UNPREDICTABLE, pMalRankedMatchKey );
        pMalRankedMatchKey = NULL;

        if( g_bStopAllThreads || OnlyPhrases.m_StopThisThread )
            break;

        if( ++ LoopCount == StopCount )
            break;
    }
    MyFree( 200, UNPREDICTABLE, pMalRankMatchVector );
    pMalRankMatchVector = NULL;

    // Now this is a really spectacular moment:
    // pSolRankedMatchText is a CSol( CSOL_OBJECT )
    // Therefore, when I delete the CSol, it will
    // delete all the objects hanging from pVoids.

    delete pSolRankedMatchText;
    pSolRankedMatchText = NULL;

    #if DO_DEBUG_PAGE
        ; Spew( L"CFwd: RunPhrasesThread done" );
    #endif
    return;
}

