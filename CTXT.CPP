// This is file: CTxt.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// a CTxt is created to parse one web page, but CPag Pag is forever.

// Right now, the order of calls is thus:
// CPag::SecondPartOfProcessForText
//     pTxtParser = new CTxt( pOnePaper );
//     WordCount = pTxtParser->SimpleWordScan( ); // Tally words
//     pTxtParser->LanguageGuessingWork( );
//     pTxtParser->AddWordsToOtherLists( );
//     Score = pTxtParser->SentenceScan( ); // Tally phrases, blocks
//     pTxtParser->MostUncommonWords( pOnePaper->pWsbAnnotation );


    //     New revelations from Unicode: ( not yet implemented )

    // These are some other notes from the WORD-BOUNDARY rules:
    // ( see abbreviated STerm list in ScanForSentenceBreaks )

    // Upper General_Category = Titlecase_Letter ( Lt ), or
    // Uppercase = true
    // OLetter Alphabetic = true, or
    // U+00A0 ( NBSP ) NO-BREAK SPACE ( NBSP ), or
    // U+05F3 ( ׳ ) HEBREW PUNCTUATION GERESH
    // and Lower = false
    // and Upper = false
    // and Grapheme_Extend = false
    // ATerm U+002E ( . ) FULL STOP
    // STerm STerm = true

    // Do not break letters across certain punctuation.
    // WB6.ALetter×MidLetter ALetter
    // WB7.ALetter MidLetter×ALetter
    // Do not break within sequences of digits, or digits adjacent to letters
    // ( “3a”, or “A3” ).
    // WB8.Numeric×Numeric
    // WB9.ALetter×Numeric
    // WB10.Numeric×ALetter
    // Do not break within sequences, such as “3.2” or “3,456.789”.
    // WB11.Numeric MidNum×Numeric
    // WB12.Numeric×MidNum Numeric
    // Do not break from extenders.
    // WB13a.( ALetter | Numeric | Katakana | ExtendNumLet ) ×ExtendNumLet
    // WB13b.ExtendNumLet×( ALetter | Numeric | Katakana )
    // Otherwise, break everywhere ( including around ideographs ).

    // For Hebrew, tailoring may include double quotation mark between letters,
    // because legacy data may contain that in place of U+05F4 gershayim. This
    // can be done by adding double quotation mark to MidLetter. U+05F3 HEBREW
    // PUNCTUATION GERESH may also be included in a tailoring.

    // To allow acronyms like U.S.A., a tailoring may include U+002E FULL STOP
    // in ExtendNumLet.

    // Apostrophe includes U+0027 APOSTROPHE and U+2019 RIGHT SINGLE QUOTATION MARK
    // ( curly apostrophe ).

    // Hyphens include U+002D HYPHEN-MINUS, U+2010 HYPHEN, possibly also U+058A
    // ARMENIAN HYPHEN, and U+30A0 KATAKANA-HIRAGANA DOUBLE HYPHEN.

    //  Implementations are encouraged to tailor Sp so as to include NBSP.


// A few defines from the HTML parser

#define SPC 0x20 // whitespace
#define DQT 0x22 // Double quote
#define AMP 0x26 // ampersand
#define SQT 0x27 // Single quote
#define SLA 0x2f // slash
#define LAB 0x3c // left angle bracket
#define RAB 0x3e // right angle bracket
#define BSL 0x5c // backslash


#define ENUM_SIGNIFY_VERY_GOOD      4
#define ENUM_SIGNIFY_SLIGHT_GOOD    3
#define ENUM_SIGNIFY_NULL_HINT      2
#define ENUM_SIGNIFY_ANATHEMA       1

// Here is a class that I only use internally, to hold blocks,
// and all the sentences in blocks, while accumulating a block,
// and while passing through a pipeline to eventual add/delete.

// It's declaration must be in CTXT.h to use as members of CTxt.

CPipeItem::CPipeItem( )
{
    AtopBlock = 0;
    PastBlock = 0;
    IndexBlock = 0;
    pMallocTriplets = NULL;
    nMallocTriplets = 0;
    nFilledTriplets = 0;

}

CPipeItem::~CPipeItem( )
{
    if( pMallocTriplets != NULL )
    {
        MyFree( 102, nMallocTriplets * 3 * sizeof( size_t ), pMallocTriplets );
        pMallocTriplets = NULL;
    }
}

void CPipeItem::AddSentence( size_t Atop, size_t Past, size_t Index )
{
    if( nMallocTriplets == nFilledTriplets )
    {
        // Need to grow vector of Triplets.
        size_t nNeed = nMallocTriplets + 10;
        nNeed += ( nNeed / 8 ); // grow liberally 125 %
        void * pvm = NULL;
        if( pMallocTriplets == NULL )
            pvm = MyMalloc( 125, nNeed * 3 * sizeof( size_t ) );
        else
            pvm = MyRealloc( 9989, nMallocTriplets * 3 * sizeof( size_t ), pMallocTriplets, nNeed * 3 * sizeof( size_t ) );
        if( pvm == NULL )
        {
            return; // failure - already messaged
        }
        pMallocTriplets = ( size_t * ) pvm;
        nMallocTriplets = nNeed;
    }
    size_t index = nFilledTriplets * 3;
    pMallocTriplets [ index + 0 ] = Atop;
    pMallocTriplets [ index + 1 ] = Past;
    pMallocTriplets [ index + 2 ] = Index;
    nFilledTriplets++;
}

CTxt::CTxt( COnePaper * pOnePaper )
{
    #if DO_DEBUG_VALUATION
        Spew( L"- - - - - - - - - NEW FILE - - - - - - - - - -" );
    #endif

    #if DO_DEBUG_CALLS
        Routine( L"272" );
    #endif
    m_pOnePaper = pOnePaper;

    // This is the input text to parse.
    // If asked to infix ( bMerge ) analysis,
    // I will delete old, create new, reload into pFruit:
    // That means also to update the IDXs by size change.
    LocalCopy = m_pOnePaper->pWsbResultText->GetBuffer( & nLocalCopy );
    szSentenceBuffer = NULL;
    nMalSentenceBuffer = 0;
    m_pSolWordList = pOnePaper->pSolWordList;

    #if DO_DEBUG_LANG_GUESS
        m_pWsbGuessWork = new CWsb;
    #endif

    pPiCurrentBlock = NULL;
    pPiPipeLine[ 0 ] = NULL;
    pPiPipeLine[ 1 ] = NULL;
    pPiPipeLine[ 2 ] = NULL;
    WarrantCounter = 3; //

    // You are wondering where my text output goes?
    // You already forgot: The HTML parser made it!

}
CTxt::~CTxt( )
{
    #if DO_DEBUG_CALLS
        Routine( L"273" );
    #endif

    #if DO_DEBUG_VALUATION
        Spew( L"- - - - - - - - - EOF FLUSH - - - - - - - - - -" );
    #endif

    {
        // Do same as ANATHEMA case -- BEFORE rid other members:
        if( pPiPipeLine[ 2 ] != NULL )
        {
            SpewPipeItemToIgnore( pPiPipeLine[ 2 ] );
            delete pPiPipeLine[ 2 ];
            pPiPipeLine[ 2 ] = NULL;
        }
        if( pPiPipeLine[ 1 ] != NULL )
        {
            SpewPipeItemToIgnore( pPiPipeLine[ 1 ] );
            delete pPiPipeLine[ 1 ];
            pPiPipeLine[ 1 ] = NULL;
        }
        if( pPiPipeLine[ 0 ] != NULL )
        {
            SpewPipeItemToIgnore( pPiPipeLine[ 0 ] );
            delete pPiPipeLine[ 0 ];
            pPiPipeLine[ 0 ] = NULL;
        }

        // except this time, conditionally.
        if( pPiCurrentBlock != NULL )
        {
            SpewPipeItemToIgnore( pPiCurrentBlock );
            delete pPiCurrentBlock;
            pPiCurrentBlock = NULL;
        }
    }

    if( LocalCopy != NULL )
    {
        MyFree( 40, zx, LocalCopy );
        LocalCopy = NULL;
    }
    if( szSentenceBuffer != NULL )
    {
        MyFree( 45, nMalSentenceBuffer * sizeof( wchar_t ), szSentenceBuffer );
        szSentenceBuffer = NULL;
    }
    #if DO_DEBUG_LANG_GUESS
        delete m_pWsbGuessWork;
        m_pWsbGuessWork = NULL;
    #endif

}

void CTxt::MostUncommonWords( CWsb * pWsbNotes )
{
    #if DO_DEBUG_CALLS
        Routine( L"274" );
    #endif
    size_t Startlen = pWsbNotes -> StrLen;
    size_t Stoplen = Startlen + 200;

    // Now I only have one caller: ProcessPaper in Call.cpp.
    // This is the final line of the annotations. Therefore
    // add one line of tokens, two newlines, into pWsbNotes.
    // If return early, add 2 newlines. World depends on it.
    // Oops. I forgot. Same notes must serve in result logs.
    // So I will only, but always, add only 1 final newline.

    // First measure should be frequency of word.
    // A second measure could be valuation of letter sequences.
    // Finally, during output, stop after some amount.

    CSol * pSolFreqList = new CSol( CSOL_SCALAR );

    CoIt * pMalVector = m_pSolWordList->GetSortedVector( CSOL_FORWARD );
    if( pMalVector == NULL )
    {
        pWsbNotes->Add( L"\r\n" ); // Caller expects 1 newline.
        return;
    }
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
            wchar_t WorkSpace[ 10 + MAX_LEGITIMATE_WORD_LENGTH + 10 ];

            size_t WordCountInPage = pCoIt->User.Value;
            // The page word bumper did a naive ++, nobody added bit-fields.
            if( WordCountInPage > MAX_17BIT_WORD_COUNT )
                WordCountInPage = MAX_17BIT_WORD_COUNT;

            #if DO_DEBUG_COMMONWORD
                SpewTwo( L"Uncommon...FullKey", FullKey );
                SpewValue( L"WordCountInPage", WordCountInPage );
            #endif

            size_t RelativeWordFreq = WordCountInPage;

            size_t WordValue = 0; // may get it by lookup instead of computation

            // So what does this heuristic do?

            // Suppose one page uses 1000 of word x,
            // and the corpus+salted count for x is 10,000.
            // Then its value is 1000 / ( ( 10000/128 )=~80 ) = ~ 12.

            // Suppose one page uses 100 of word x,
            // and the corpus+salted count for x is 1,000.
            // Then its value is 100 / ( ( 1000/128 )=~8 ) = ~ 12.

            // Suppose one page uses 10 of word x,
            // and the corpus+salted count for x is 1,000.
            // Then its value is 10 / ( ( 1000/128 )=~8 ) = 1.

            // Suppose one page uses 100 of word x,
            // and the corpus+salted count for x is under 128.
            // Then its value is 100 / ( ( ?/128 + 1 )=1 ) = 100.

            // I really should have a floating division, and then
            // a log of that would make bidirectional +/- N range,
            // like a pH value. So my heuristic factor or divisor
            // should be at the sqrt of the 5 digit range below.

            // Naw, I really need to study the subject of weights
            // as discussed in word-chunking vectorite literature.

            // I don't even recall if this page was in common yet.
            // Yes. This page's words have been counted in common.

            size_t ComnIndex = CSolCommonWords.Find( FullKey );
            #if DO_DEBUG_ADDFIND
                if( ComnIndex == 1 )
                    { Spew( L"AddFind 1 at ctxt 78" ); }
            #endif
            if( ComnIndex != 0 )
            {
                // This word of page already exists in CSolCommonWords.
                size_t ComnValue = CSolCommonWords.GetUserValue( ComnIndex );
                size_t ComnCount = ComnValue & MASK_17BIT_WORD_COUNT;
                RelativeWordFreq /= ( ( ComnCount / RELATIVE_WORD_FACTOR ) + 1 );
                // Get value of this old word for free.
                WordValue = ComnValue >> 24; // no need to & MASK_WORD_VALUE_SHL24

                #if DO_DEBUG_COMMONWORD
                    SpewValue( L"ComnCount", ComnCount );
                    SpewValue( L"WordValue = ComnValue >> 24", WordValue );
                #endif
            }
            else
            {
                // Compute value of this novel word.
                // This code path should be un-needed.
                WordValue = WordLetterValuation( FullKey );
                if( WordValue > MAX_WORD_VALUE )
                    WordValue = MAX_WORD_VALUE; // limit is 255, also < 999.

                #if DO_DEBUG_COMMONWORD
                    SpewValue( L"WordValue = WordLetterValuation", WordValue );
                #endif

            }

            if( RelativeWordFreq > 99999 )
                RelativeWordFreq = 99999; // limit to 5 digits

            #if DO_DEBUG_COMMONWORD
                SpewValue( L"RelativeWordFreq", RelativeWordFreq );
            #endif

/// hmmm.. instead of concatenating digits, I should mul x 2 weights.
/// also, I should keep a highest count in each language to scale it.

            wsprintf( WorkSpace, L"%5d%3d%s", RelativeWordFreq, WordValue, FullKey );
            #if DO_DEBUG_COMMONWORD
                Spew( WorkSpace );
            #endif

            size_t index = pSolFreqList->AddKey( WorkSpace );
            #if DO_DEBUG_ADDFIND
                if( index == 1 )
                    { Spew( L"AddFind 1 at ctxt 96" ); }
            #endif
        }
        MyFree( 91, zx, FullKey );
        FullKey = NULL;
    }
    MyFree( 94, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    pMalVector = pSolFreqList->GetSortedVector( CSOL_BACKWARD );
    if( pMalVector == NULL )
    {
        pWsbNotes->Add( L"\r\n" ); // Caller expects 1 newline.
        return;
    }
    size_t take2 = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take2++;
        if( pCoIt->IsSentinel )
            break;
        wchar_t * FullKey = CoItFullKey( pCoIt );
        // word itself is after 8 digits ( 5 count, 3 value ).
        pWsbNotes->Add( L" " );
        pWsbNotes->Add( FullKey + 8 );
        MyFree( 109, zx, FullKey );
        FullKey = NULL;
        if( pWsbNotes->StrLen > Stoplen )
            break;
    }

    MyFree( 114, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    delete pSolFreqList;
    pSolFreqList = NULL;

    pWsbNotes->Add( L"\r\n" ); // Caller expects 1 newline.

    #if DO_DEBUG_LANG_GUESS
        // Make a little suffix to study at end of normal annotation.
        m_pOnePaper->pWsbAnnotation->Add( L"\r\n" );
        m_pOnePaper->pWsbAnnotation->AddWsb( m_pWsbGuessWork );
        m_pOnePaper->pWsbAnnotation->Add( L"\r\n" );
    #endif

    return;
}

size_t CTxt::SimpleWordScan( )
{
    #if DO_DEBUG_CALLS
        Routine( L"275" );
    #endif
    // Recall that the Add File, Folder, Fetch, Cache, etc.,
    // has already attached and run and detached HTML parser
    // which left behind a very simply formatted text buffer.
    // Add File/Folder will have removed any top annotations.
    // My constructor made a whole copy of that just for me.

    // There are no new annotations yet; They await my output.

    // This simple word scan will take every alpha string, and
    // add each word to a CSol for the page, and also globally.

    // SimpleWordScan here determines my strictures on words.

    // Once I had limited words to 3-9? lowercase alpha.

    // I think to not put a lower limit on word length.
    // Certainly, words at this point are full unicode.
    // All iswalpha characters in list were lowercased.
    // Conceivably, I may allow hyphens or apostraphes.
    // Conceivably, I may digits, but not in salt list.

    size_t TotalWordCount = 0;

    if( LocalCopy == NULL
    || nLocalCopy == NULL )
        return TotalWordCount;

    wchar_t * scan = LocalCopy;
    wchar_t * past = scan + nLocalCopy;

    // Elaborating this loop is worth much effort.
    // I would like to list hypenated words as well
    // as their sub-parts, and sometimes apostraphe.
    // I would like to allow initial alpha + alnums.
    // Make similar changes in both CTxt and CFwd.

    // It seems disambiguation of apostraphe versus quote usage
    // is too dificult, so only allow apostraphe between alphas.
    // Similarly, only allow single hyphens when between alphas.

    // Thus, outer loop can be bi-phase, advancing first to an
    // alpha, then advancing over all iswalpha, digits, single
    // hyphens between alpha, single apostraphe between alpha.

    // In case hyphen was found, run an inner reparse of words.

    // Perhaps I should allow initial digits ( 0xff, 79-ers ).
    // That reminds me, should I drop any apostraphe, 's' es?
    // No, but do add the word prior to an appositive ending.

    // Notice a bi-phase loop avoids having to add a finisher
    // after the break from the loop, as with original design.

    // FUCK! iswalpha says c-cedilla, 0xE7, 231, is NOT alpha!
    // I'll bet it's a locale issue. It's time to roll my own!
    // No, that was fixed by _wsetlocale( LC_CTYPE, L".1252" )


    for( ;; )
    {
        // Do phase-one inner loop to scan for any alnum.
        for( ;; )
        {
            if( scan == past ) // determinate if all others ++ safely
                break;
            if( iswalnum( *scan ) )
                break;

            // Char-by-char was deep shit on iswalpha + setlocale:
            // #if DO_DEBUG_VOCADD
            //     ; SpewValue( L"Not alpha", *scan );
            // #endif

            scan++;
        }

        if( scan == past )
            break;

        // DUm Dum Dummy.
        // You do realize this is a declaration statement,
        // which happens once per loop -- to set up loop!
        // It cannot be used to get current value of scan:
        // wchar_t * atop = scan; // remember position of first alnum

        wchar_t * atop; // declaration part... then assignment:
        atop = scan; // remember position of first alnum

        int SawAnyAlpha = 0;
        int MustSubParse = 0;

        // Do phase-two inner loop to find end of various.

        for( ;; )
        {
            // Incoming logic above for first character,
            // and rest of loop, qualified char at scan.
            // So incorporate char before a test to end.
            // I can lowercase in buffer, need not copy.

            // Incorporating the current char means to lowercase any alpha.

            wchar_t wc = *scan;

            // Final NULL will be treated as an unsuitable character:

            if( iswalpha( wc ) )
            {
                // Char-by-char was deep shit on iswalpha + setlocale:
                // #if DO_DEBUG_VOCADD
                //     ; SpewValue( L"iswalpha", wc );
                // #endif

                SawAnyAlpha = 1;

                // Curiously, or not, all the in/out relationships of
                // Windows wtolower are as for USASCII: to OR in ' '.
                // Of course, my experiments assumed some locale too.
                // Maybe I had better not depend on my favorite | ' '.

                if( iswupper( wc ) )
                    wc = towlower( wc ); // having setlocale to .1252

                *scan = wc;
                // Meanwhile, this alpha is also a suitable character.
            }
            else if( iswdigit( wc ) )
            {
                // Or this digit is a suitable character.
                // Char-by-char was deep shit on iswalpha + setlocale:
                // #if DO_DEBUG_VOCADD
                //     ; SpewValue( L"iswdigit", wc );
                // #endif
            }
            else if( wc == SQT
            && scan > atop // CYA for [-1]
            && iswalnum( scan[-1] )
            && iswalnum( scan[1] ) && ! isvowel( scan[1] ) )
            {
                // Or this apostraphe between alphas is suitable.

                // Apostrophe includes U+0027 ( ' ) APOSTROPHE
                // and U+2019 ( � ) RIGHT SINGLE QUOTATION MARK

                // I no longer need to rule out and fix the U+2019
                // here in ctxt.cpp, as I fixed it in the charsets:
                // wc = SQT; // fix in case U+2019

                // According to the UNICODE.ORG suggestion, don't
                // accept an apostraphe followed by a vowel which
                // takes care of l'en... del'a.... of french, etc.

                MustSubParse = 1;
                // Char-by-char was deep shit on iswalpha + setlocale:
                // #if DO_DEBUG_VOCADD
                //     ; SpewValue( L"is apost/dash", wc );
                // #endif
            }
            else if( wc == '-'
            && scan > atop // CYA for [-1]
            && iswalnum( scan[-1] )
            && iswalnum( scan[1] ) )
            {
                // Or various hyphens between alphas is suitable.

                // Unicode.org says: Hyphens include
                // U+002D HYPHEN-MINUS,
                // U+2010 HYPHEN,
                // possibly also U+058A ( ? ) ARMENIAN HYPHEN, -- ignore
                // and U+30A0 KATAKANA-HIRAGANA DOUBLE HYPHEN. -- ignore

                // U+2010 HYPHEN,
                // U+2011 NON-BREAKING HYPHEN,
                // U+2012 FIGURE DASH,
                // U+2013 EN DASH,
                // U+2014 EM DASH,
                // U+2015 HORIZONTAL BAR ( quotation dash ), and
                // U+2212 MINUS SIGN
                // U+002D HYPHEN-MINUS ( '-' )
                // -- http://czyborra.com/unicode/characters.html

                // I no longer need to rule out and fix U+2010 etc
                // here in ctxt.cpp, as I fixed it in the charsets.
                // wc = '-'; // fix in case not simple hyphen

                MustSubParse = 1;
                // Char-by-char was deep shit on iswalpha + setlocale:
                // #if DO_DEBUG_VOCADD
                //     ; SpewValue( L"is apost/dash", wc );
                // #endif
            }
            else if(
                ( wc == ':'
                || wc == 0x00b7
                || wc == 0x05f3
                || wc == 0x05f4
                || wc == 0x2027 )
            && scan > atop // CYA for [-1]
            && iswalnum( scan[-1] )
            && iswalnum( scan[1] ) )
            {
                // Or various "midletters" besides apostraphe, hyphen.

                // MidLetter Any of the following:
                // U+00B7 MIDDLE DOT
                // U+05F3 HEBREW PUNCTUATION GERESH
                // U+05F4 HEBREW PUNCTUATION GERSHAYIM
                // U+2027 HYPHENATION POINT
                // U+003A COLON ( used in Swedish )

                MustSubParse = 1;
                // Char-by-char was deep shit on iswalpha + setlocale:
                // #if DO_DEBUG_VOCADD
                //     ; SpewValue( L"is apost/dash", wc );
                // #endif
            }
            else
            {
                // scan is sitting on the first unsuitable character.
                // That means phase-two inner loop will break now.
                // Having found first unsuitable, I can process word.

                // Char-by-char was deep shit on iswalpha + setlocale:
                // #if DO_DEBUG_VOCADD
                //     ; SpewValue( L"is first unsuitable char", wc );
                // #endif

                // Eliminate purely numeric fields, dates 02-80-2007.
                if( SawAnyAlpha )
                {
                    wchar_t saved = *scan;
                    *scan = NULL;

                    #if DO_DEBUG_VOCADD
                        ; SpewTwo( L"Adding word", atop );
                    #endif

                    size_t index = m_pSolWordList->AddKey( atop );
                    #if DO_DEBUG_ADDFIND
                        if( index == 1 )
                            { Spew( L"AddFind 1 at ctxt 329" ); }
                    #endif
                    // Although the count in CSolAllWords must be
                    // treated as an unsigned, this is safe to ++:
                    m_pSolWordList->IncrementUserValue( index );
                    TotalWordCount ++;

                    if( MustSubParse )
                    {
                        // Just partition at apostraphe/hyphen,
                        // well, now i'd better say ! iswalnum,
                        // and save any alnum runs over 1 char.
                        // Again, require at least one iswalpha.

                        #if DO_DEBUG_VOCADD
                            ; SpewTwo( L"MustSubParse", atop );
                        #endif

                        wchar_t * top = atop;
                        wchar_t * run = atop;
                        int HadAlpha = 0;
                        for( ;; )
                        {
                            wchar_t cw = *run;
                            if( cw == NULL
                            || ! iswalnum( cw ) )
                            {
                                // Consider to do from top to run-1;
                                if( run - top > 1
                                && HadAlpha )
                                {
                                    // I already saved: cw = *run;
                                    *run = NULL;

                                    #if DO_DEBUG_VOCADD
                                        ; SpewTwo( L"Adding sub-word", top );
                                    #endif

                                    size_t index = m_pSolWordList->AddKey( top );
                                    #if DO_DEBUG_ADDFIND
                                        if( index == 1 )
                                            { Spew( L"AddFind 1 at ctxt 372" ); }
                                    #endif
                                    // Although the count in CSolAllWords must be
                                    // treated as an unsigned, this is safe to ++:
                                    m_pSolWordList->IncrementUserValue( index );
                                    TotalWordCount ++;

                                    *run = cw; // restore NULL to character
                                }

                                // If continue, next char is new top.
                                top = run + 1;
                                HadAlpha = 0;
                            }
                            if( cw == NULL )
                                break;
                            if( iswalpha( cw ) )
                                HadAlpha = 1;
                            run++;
                        }
                    }
                    *scan = saved; // restore
                }
                break; // ending phase-two inner loop
            }
            // if here, phase-two inner loop continues
            scan ++;

            // Do not apply the test here: if( scan == past ) break;
            // If that happens, *scan == NULL which ends word above.
        }
        if( scan == past )
            break;
    }
    return TotalWordCount;

}

void CTxt::LanguageGuessingWork( )
{
    // Even though the HTTP header may have declared some language,
    // and very often it does not, I sometimes find that pages are
    // not in that language, or have a mix of languages, so I will
    // freely revise the language for the paper.

    // SimpleWordScan has run, and has counted words into m_pSolWordList.

    // I currently have several kinds of word lists:
    // See new descriptions of them all atop cvoc.cpp.
    // I slipped 7 bits shl 17 into the User.Value for language guessing.

    // Remind me: How do these two members of pOnePaper differ?
    // m_pOnePaper->HttpHeaderContentLanguage
    // m_pOnePaper->LanguageGroup
    // The first is an index into ... not a CSol, but the bigger CVoc vector.
    // The first is fully accurate. It is used in my idiomatic annotations.
    // pOnePaper->HttpHeaderContentLanguage = BestIndexforLanguageString( L"en-US", NULL );
    // The second is an index into a vector.
    // The second has fewer values; vector slots may be NULL, created JIT.
    // The second is used to make subfolders, and for per-language word lists.
    // pOnePaper->LanguageGroup = GroupIndexForLanguageIndex( pOnePaper->HttpHeaderContentLanguage );
    // SomeLanguageIndexForGroupIndex( ) can map a group back to some language.
    // Both LANGUAGE_UNKNOWN and LANGUAGE_GROUP_UNKNOWN are zero, especially.
    // I may choose to set LANGUAGE_MULTIPLE along with LANGUAGE_GROUP_UNKNOWN.

    // Five things may set those two differing members of pOnePaper:
    // COnePaper Constructor: LANGUAGE_UNKNOWN and LANGUAGE_GROUP_UNKNOWN.
    // Parse of an HTTP header ( for cache, query, fetch )
    // Parse of the idiomatic header ( load file )
    // Parse of the HTML content ( all paper creators )
    // Soon, CTxt's new routine LanguageGuessingWork( ).

    // Case 1: Language is stated; But that language's word list is empty.
    // Or also, if that language's word list has very few words in it.
    // In this case, do not revise the paper's declared language.
    // It might not be recognized correctly due to the few words.
    // It will be needed to build up the right language word list.

    // Case 2: Language is stated; and I have "lots" of words in language.
    // In this case, permit analysis to run...

    // Case 3: Language is not stated; and I have "lots" of words in language.
    // In this case, permit analysis to run...

    // Iterate all words of the current page.
    // extract word count, and clamp it, as it is a raw count.
    //
    // Find each word in CSolAllWords, and get User.Value
    // Find each word in CSolCommonWords, and get User.Value
    // ( This page's words are not yet added to either )
    // extract language group id ( s/b equal ) and sum of both counts.
    //
    // Use their product to augment a slot per LangID, unless multiple.
    // At the end, sort the slot counts ( report if debugging ).
    // If one weight predominates, revise paper's language and group ID.

    // Now I see many pages with multiple high counts, and many pages
    // with tiny count probably due to bad words in the common lists.
    // So: 1. Revise the header annotation to a new version 2 ( Keep 1 ).
    // It might be like: ... Hebrew 90% English 5% Other 4% Unknown 2%.
    // I think it should be unweighted percentages of total word count.
    // Naw. ( BTW I didn't )
    // 2. Add debugging output about spurious words after header notes.
    // I think keep just the one highest scoring word in each language.
    // 3. Think harder about when to assign new words to each language.

    #if DO_DEBUG_LANG_GUESS
        m_pWsbGuessWork->Add( L"GuessWork: " );
    #endif

    size_t WeightVector[ PAST_LANGUAGE_GROUP_IDS ];
    memset( WeightVector, 0 , sizeof( WeightVector ) );

    size_t WeightUnknown = 0;
    size_t WeightMultiple = 0;
    size_t WeightTotal = 0;

    size_t WordCountVector[ PAST_LANGUAGE_GROUP_IDS ];
    memset( WordCountVector, 0 , sizeof( WordCountVector ) );

    size_t WordCountUnknown = 0;
    size_t WordCountMultiple = 0;
    size_t WordCountTotal = 0;

    size_t HighestWeightVector[ PAST_LANGUAGE_GROUP_IDS ];
    memset( HighestWeightVector, 0 , sizeof( HighestWeightVector ) );

    size_t HighestWeightInMultiple = 0;
    size_t HighestWeightInUnknown = 0;

    // must free all before return:
    wchar_t * HighestpMalWordVector[ PAST_LANGUAGE_GROUP_IDS ];
    memset( HighestpMalWordVector, 0 , sizeof( HighestpMalWordVector ) );

    // must free before return:
    wchar_t * HighestpMalWordInMultiple = 0;
    // must free before return:
    wchar_t * HighestpMalWordInUnknown = 0;

    // So, I see this loop is DOING each word of the current page.
    // They probably only have a count, and that may be unclamped.

    CoIt * pMalVector = m_pSolWordList->GetSortedVector( CSOL_FORWARD );
    if( pMalVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;

        wchar_t * FullKey = CoItFullKey( pCoIt );
        size_t Strlen = wcslen( FullKey );

        #if DO_DEBUG_LANG_MATH
            SpewTwo( L"Word in page to consider", FullKey );
        #endif

        // No CoIt from GetSortedVector ever holds an empty string.
        if( Strlen < MAX_LEGITIMATE_WORD_LENGTH )
        {
            size_t WordCountInPage = pCoIt->User.Value;
            // The page word bumper did a naive ++, nobody added bit-fields.
            // if( WordCountInPage > MAX_17BIT_WORD_COUNT )
            //     WordCountInPage = MAX_17BIT_WORD_COUNT;
            // reduce to 15 bits, so 15+15 < 31 bits when multiplied.
            // Do no work on words with fewer than 4 appearances.
            // WordCountInPage >>= 2;
            // Too many shifts make outcome small. Clamp instead.
            if( WordCountInPage > 0x0007fff )
                WordCountInPage = 0x0007fff;

            #if DO_DEBUG_LANG_MATH
                SpewValue( L"WordCountInPage", WordCountInPage );
            #endif

            if( WordCountInPage > 0 )
            {
                // CSolAllWords surely contains this word by now....?
                // Oh, no. I have not added words from this page yet.

                // Since CSolAllWords is empty at the start of each corpus,
                // and since CSolCommonWords is not augmented by web pages,
                // take their sum, if found, for the corpus word frequency.

                // Get a hold of both CSols which I will be consulting:

                size_t index1 = CSolAllWords.Find( FullKey );
                #if DO_DEBUG_ADDFIND
                    if( index1 == 1 )
                        { Spew( L"AddFind 1 at ctxt 765" ); }
                #endif

                size_t index2 = CSolCommonWords.Find( FullKey );
                #if DO_DEBUG_ADDFIND
                    if( index2 == 1 )
                        { Spew( L"AddFind 1 at ctxt 771" ); }
                #endif

                size_t ValShl24 = 0;
                size_t AllGidShl17 = 0;
                size_t CountInAll = 0;
                size_t CountInComn = 0;

                // What if LGID differ in two lists? Can they?
                // They can differ by either one being empty.
                // Otherwise, make sure they are always equal.
                // Sufficient if adding web page updates both.
                // Or, Sufficient if I just use the best copy.
                // The best/last copy is All's, so get second.

                if( index2 > 1 )
                {
                    // Not a novel word.
                    size_t Value2 = CSolCommonWords.GetUserValue( index2 );
                    ValShl24    = Value2 & MASK_WORD_VALUE_SHL24; // redundant but equal
                    AllGidShl17 = Value2 & LGID_SHL17_MASK; // redundant but equal
                    CountInComn = Value2 & MASK_17BIT_WORD_COUNT;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"ValShl24", ValShl24 );
                    #endif

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"AllGidShl17", AllGidShl17 );
                    #endif

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"CountInComn", CountInComn );
                    #endif
                }

                if( index1 > 1 )
                {
                    // Not a novel word.
                    size_t Value1 = CSolAllWords.GetUserValue( index1 );
                    ValShl24 = Value1 & MASK_WORD_VALUE_SHL24;
                    AllGidShl17 = Value1 & LGID_SHL17_MASK;
                    CountInAll = Value1 & MASK_17BIT_WORD_COUNT;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"ValShl24", ValShl24 );
                    #endif

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"AllGidShl17", AllGidShl17 );
                    #endif

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"CountInAll", CountInAll );
                    #endif
                }

                size_t CorpusSum = CountInAll + CountInComn;

                // reduce to 15 bits, so 15+15 < 31 bits when multiplied.
                // actually, possibly 16 bits due to sum above.
                // CorpusSum >>= 2;
                // Too many shifts make outcome small. Clamp instead.

                if( CorpusSum > 0x0007fff )
                    CorpusSum = 0x0007fff;

                #if DO_DEBUG_LANG_MATH
                    SpewValue( L"CorpusSum", CorpusSum );
                #endif

                if( CorpusSum > 0 )
                {
                    // This is like doing what? I think it's one point of
                    // cross product of the page and corpus term vectors.

                    // I want the MULTIPLE and UNKNOWN to do similar math.
                    // UNKNOWN of course suffers from a zero corpus count.

                    size_t Product = CorpusSum * WordCountInPage;

                    // I think to scale that by valuation, for complex
                    // words are more distinctively in a given language.
                    // Valuation range is 0 to 255. Pre-clamp range again:

                    if( Product > 0x00ffffff )
                        Product = 0x00ffffff;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"CorpusSum * WordCountInPage", Product );
                    #endif

                    Product *= ( ValShl24 >> 24 );

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"Product * ( ValShl24 >> 24 )", Product );
                    #endif

                    // Okay, as the bugs go out, I find HUGE duplication
                    // of words in many languages ( validly, A, DE, etc. )
                    // ( but probably invalidly, NOT, THE, etc. ) In order
                    // to kick these small words down a notch, lets take
                    // their strlen; better, the square of it, as another
                    // factor in product.

                    size_t Square = wcslen( FullKey );
                    Square *= Square;
                    if( Square > 49 ) // only de-emphasize if under 7 chars.
                        Square = 49;

                    // I was hitting about 29 bits, so make some room for *49.
                    // That's six more bits, so take out 3. Or I can clamp it.
                    // Or a little of both...

                    if( Product > 0x0ffffff )
                        Product = 0x0ffffff;

                    Product >>= 2;

                    Product *= Square;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"Product * Value / 4 * Square", Product );
                    #endif

                    // To what slot of vector shall I add this product?

                    size_t LGroupId = AllGidShl17 >> 17;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"LGroupId", LGroupId );
                    #endif

                    if( LGroupId >= PAST_LANGUAGE_GROUP_IDS )
                    {
                        // All/Comn flaged this word as MULTIPLE

                        // MULTIPLE got the same math automatically.

                        #if DO_DEBUG_LANG_MATH
                            Spew( L"All/Comn flaged this word as MULTIPLE" );
                        #endif

                        size_t OldSum = WeightMultiple;
                        size_t NewSum = OldSum + Product;
                        if( NewSum < OldSum )
                        {
                            // unsigned overflow occurred.
                            NewSum = 0xffffffff; // Max unsigned int.
                        }
                        WeightMultiple = NewSum;
                        WeightTotal += ( NewSum - OldSum );

                        WordCountMultiple += WordCountInPage;
                        WordCountTotal += WordCountInPage;

                        // I still want to see my ringers in Multiple
                        if( HighestWeightInMultiple <= Product )
                        {
                            // That is, Highest product of a single word.
                            // I keep this to help find ringers in list.
                            HighestWeightInMultiple = Product;

                            #if DO_DEBUG_LANG_MATH
                                SpewValue( L"New HighestWeightInMultiple", HighestWeightInMultiple );
                            #endif

                            // Absorb the page's word malloc FullKey.
                            if( HighestpMalWordInMultiple != NULL )
                                MyFree( 842, zx, HighestpMalWordInMultiple );
                            HighestpMalWordInMultiple = FullKey;
                            FullKey = NULL;
                        }
                    }
                    else if( LGroupId == LANGUAGE_GROUP_UNKNOWN )
                    {
                        // All/Comn flaged this word as UNKNOWN
                        // This IF clause is not impossible:
                        // CSolAll has words with count > 0, but lang == unk.

                        // This 1 of 2 UNKNOWN paths got the same math automatically.

                        #if DO_DEBUG_LANG_MATH
                            Spew( L"All/Comn flaged this word as UNKNOWN" );
                        #endif

                        size_t OldSum = WeightUnknown;
                        size_t NewSum = OldSum + Product;
                        if( NewSum < OldSum )
                        {
                            // unsigned overflow occurred.
                            NewSum = 0xffffffff; // Max unsigned int.
                        }
                        WeightUnknown = NewSum;
                        WeightTotal += ( NewSum - OldSum );

                        WordCountUnknown += WordCountInPage;
                        WordCountTotal += WordCountInPage;

                        if( HighestWeightInUnknown <= Product )
                        {
                            // That is, Highest product of a single word.
                            // I keep this to help find ringers in list.
                            HighestWeightInUnknown = Product;

                            #if DO_DEBUG_LANG_MATH
                                SpewValue( L"New HighestWeightInUnknown", HighestWeightInUnknown );
                            #endif

                            // Absorb the page's word malloc FullKey.
                            if( HighestpMalWordInUnknown != NULL )
                                MyFree( 842, zx, HighestpMalWordInUnknown );
                            HighestpMalWordInUnknown = FullKey;
                            FullKey = NULL;
                        }
                    }
                    else
                    {
                        // All/Comn list a single language group.

                        #if DO_DEBUG_LANG_MATH
                            Spew( L"All/Comn list a single language group." );
                        #endif

                        size_t OldSum = WeightVector[ LGroupId ];
                        size_t NewSum = OldSum + Product;
                        if( NewSum < OldSum )
                        {
                            // unsigned overflow occurred.
                            NewSum = 0xffffffff; // Max unsigned int.
                        }

                        #if DO_DEBUG_LANG_MATH
                            SpewValue( L"Old WeightVector[ LGroupId ]", OldSum );
                        #endif

                        #if DO_DEBUG_LANG_MATH
                            SpewValue( L"New WeightVector[ LGroupId ]", NewSum );
                        #endif

                        WeightVector[ LGroupId ] = NewSum;
                        WeightTotal += ( NewSum - OldSum );

                        WordCountVector[ LGroupId ] += WordCountInPage;
                        WordCountTotal += WordCountInPage;

                        #if DO_DEBUG_LANG_MATH
                            SpewValue( L"WordCountVector[ LGroupId ]", WordCountVector[ LGroupId ] );
                        #endif

                        if( HighestWeightVector[ LGroupId ] <= Product )
                        {
                            // That is, Highest product of a single word.
                            // I keep this to help find ringers in list.
                            HighestWeightVector[ LGroupId ] = Product;

                            #if DO_DEBUG_LANG_MATH
                                SpewValue( L"New HighestWeightVector[ LGroupId ]", HighestWeightVector[ LGroupId ] );
                            #endif

                            // Absorb the page's word malloc FullKey.
                            if( HighestpMalWordVector[ LGroupId ] != NULL )
                                MyFree( 842, zx, HighestpMalWordVector[ LGroupId ] );
                            HighestpMalWordVector[ LGroupId ] = FullKey;
                            FullKey = NULL;
                        }
                    }
                }
                else
                {
                    // All/Comn have no such word listed

                    // I want the MULTIPLE and UNKNOWN to do similar math.
                    // UNKNOWN of course suffers from a zero corpus count.

                    #if DO_DEBUG_LANG_MATH
                        Spew( L"All/Comn have no such word listed" );
                    #endif

                    // This 2 of 2 UNKNOWN paths must fantasize his own math.
                    // The other UNKNOWN path would have a few count from
                    // whatever other page remaining UNK added the word.
                    // That must be very few counts. Use 10 in this path.

                    size_t Product = 10 * WordCountInPage;

                    // I think to scale that by valuation, for complex
                    // words are more distinctively in a given language.
                    // Valuation range is 0 to 255. Pre-clamp range again:

                    if( Product > 0x00ffffff )
                        Product = 0x00ffffff;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"zero->ten * WordCountInPage", Product );
                    #endif

                    // This path does not have a ( ValShl24 >> 24 ) for free.
                    // But I can compute it myself:
                    size_t UnknownWordValue = WordLetterValuation( FullKey );
                    if( UnknownWordValue > MAX_WORD_VALUE )
                        UnknownWordValue = MAX_WORD_VALUE;

                    Product *= UnknownWordValue;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"Product * UnknownWordValue", Product );
                    #endif

                    size_t Square = wcslen( FullKey );
                    Square *= Square;
                    if( Square > 49 ) // only de-emphasize if under 7 chars.
                        Square = 49;

                    // I was hitting about 29 bits, so make some room for *49.
                    // That's six more bits, so take out 3. Or I can clamp it.
                    // Or a little of both...

                    if( Product > 0x0ffffff )
                        Product = 0x0ffffff;

                    Product >>= 2;

                    Product *= Square;

                    #if DO_DEBUG_LANG_MATH
                        SpewValue( L"Product * Value / 4 * Square", Product );
                    #endif

                    size_t OldSum = WeightUnknown;
                    size_t NewSum = OldSum + Product;
                    if( NewSum < OldSum )
                    {
                        // unsigned overflow occurred.
                        NewSum = 0xffffffff; // Max unsigned int.
                    }
                    WeightUnknown = NewSum;
                    WeightTotal += ( NewSum - OldSum );

                    WordCountUnknown += WordCountInPage;
                    WordCountTotal += WordCountInPage;

                    if( HighestWeightInUnknown <= Product )
                    {
                        // That is, Highest product of a single word.
                        // I keep this to help find ringers in list.
                        HighestWeightInUnknown = Product;

                        #if DO_DEBUG_LANG_MATH
                            SpewValue( L"New HighestWeightInUnknown", HighestWeightInUnknown );
                        #endif

                        // Absorb the page's word malloc FullKey.
                        if( HighestpMalWordInUnknown != NULL )
                            MyFree( 842, zx, HighestpMalWordInUnknown );
                        HighestpMalWordInUnknown = FullKey;
                        FullKey = NULL;
                    }
                }
            }
        }
        if( FullKey != NULL ) // unless absorbed above
        {
            MyFree( 857, zx, FullKey );
            FullKey = NULL;
        }
    }

    MyFree( 286, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    // Now I need to resort the WeightVector, with LGID==slot no.
    // I was thinking to add the %, or wordcount, but that will be
    // available from the vectors when I soon iterate this cSol...
    // Make this CSol sort using language percentages, not weights.
    // Percentages ( *100/total ) are raw counts in WordCountVector.
    CSol ResortUpon( CSOL_SCALAR );
    int i = 0;
    for( ;; )
    {
        // Was sort by weights...
        // size_t SortUpon = WeightVector[ i ];
        // Is sort by percentages...
        size_t SortUpon = WordCountVector[ i ];

        if( SortUpon > 0 )
        {
            wchar_t wk[40];
            // 32 bits. 4gig? 64kb squared? 8,000,000,000? max 10 digits.
            // It might by chance not be unique, so don't use User.Value.
            wsprintf( wk, L"%10d %3d", SortUpon, i );
            size_t index = ResortUpon.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index == 1 )
                    { Spew( L"AddFind 1 at ctxt 814" ); }
            #endif

            #if DO_DEBUG_LANG_MATH
                SpewValue( L"SortUpon of Language > 0 into sort", SortUpon );
            #endif

            #if DO_DEBUG_LANG_MATH
                SpewValue( L"LGroup into sort", i );
            #endif

        }
        if( ++i == PAST_LANGUAGE_GROUP_IDS )
            break;
    }

    // Now iterate ResortUpon backwards, for largest SortUpons first.
    // Do that anyway to emit debugs, but remember just the first two.

    // This debug output is getting good to go in and edit away ringers:
    // 1. Rid specific words found from specific language groups.
    // 2. Rid specific words found from multiple language groups.
    // 3. Add unknown words found to one specific language group.
    // Change %d to %2d for better sort later...
    // Also, make my CSol input sort use language percentages, not weights.
    //   63 @- Swedish 0% ( sovereign 19/19 ),
    //   55 @- French 0% ( apocalypse 24/24 ),
    //   47 @- Slovenian 0% ( mediterranean 19/19 ),
    //   45 @- Romanian 0% ( vile 15/15 ),
    //   45 @- Japanese 0% ( marvelous 19/19 ),
    //   44 @- Norwegian 0% ( amok 15/15 ),
    //   37 @- Chinese 0% ( converted 19/20 ),
    //   37 @- Catalan 0% ( horrible 18/19 ),
    //   35 @- Vietnamese 0% ( scores 19/20 ),
    //   35 @- Tamil 0% ( lord's 17/17 ),
    // You see the octal escapes:
    //   15 @+ English 5% ( christ\040\031s 24/28 ),


    if( WordCountTotal > 0 ) // CYA against several divide by zero
    {
        // I do not need to re-extract SortUpons, they are in SortUponVector.
        size_t LoopCounter = 0; // To know when 1; when 2; and total.
        size_t FirstLGroup = 0;
        size_t SecondLGroup = 0;
        wchar_t LoopMarker = '+';

        #if DO_DEBUG_LANG_MATH
            SpewValue( L"WordCountTotal > 0, so iterate", WordCountTotal );
        #endif

        CoIt * pMalVector2 = ResortUpon.GetSortedVector( CSOL_BACKWARD );
        if( pMalVector2 != NULL )
        {
            size_t take = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector2 + take++;
                if( pCoIt->IsSentinel )
                    break;
                wchar_t * FullKey = CoItFullKey( pCoIt );

                // recover the LGID integer from key.
                FullKey[ 10 ] = NULL; // stop after first %10d.
                size_t LGroup = _wtoi( FullKey + 11 );

                size_t Weight = WeightVector[ LGroup ];

                #if DO_DEBUG_LANG_MATH
                    SpewValue( L"Weight of Language", Weight );
                #endif

                size_t RawCount = WordCountVector[ LGroup ];
                #if DO_DEBUG_LANG_MATH
                    SpewValue( L"RawCount of Language", RawCount );
                #endif

                #if DO_DEBUG_LANG_MATH
                    SpewValue( L"LGroup", LGroup );
                #endif

                LoopCounter ++;
                switch( LoopCounter )
                {
                case 1:
                    FirstLGroup = LGroup;
                    break;
                case 2:
                    SecondLGroup = LGroup;
                    break;
                }

                // For starters, let's see what values arise:
                #if DO_DEBUG_LANG_GUESS
                {
                    wchar_t wk[ 100 + 8 * MAX_LEGITIMATE_WORD_LENGTH ];
                    // Numbers are so inscrutible. Get group names.

                    wchar_t * LGName = GroupNameforGroupIndex( LGroup );
                    int PercentOfAllWords = 100 * WordCountVector[ LGroup ] / WordCountTotal;

                    size_t ToShift = Weight;
                    size_t Log2Weight = 0;
                    for( ;; )
                    {
                        if( ToShift == 0 )
                            break;
                        Log2Weight ++;
                        ToShift >>= 1;
                    }

                    // I had a crash below, TopWord was NULL!
                    // How can that be? Anyway, gate against it.
                    // Think about the meaning of it later...
                    // Oh, because I changed the input sort to
                    // percentage ( raw word count ), and a word
                    // might have a zero weight, but the rule to
                    // save a Highest had a >, which zero is not.
                    // Change rule above to save when >= instead.

                    wchar_t * TopWord = HighestpMalWordVector[ LGroup ];
                    size_t TopWeight = HighestWeightVector[ LGroup ];

                    if( TopWord != NULL
                    && TopWeight > 0 )
                    {
                        // Mysterious CYA indent...

                        size_t ToShift2 = TopWeight;
                        size_t Log2Weight2 = 0;
                        for( ;; )
                        {
                            if( ToShift2 == 0 )
                                break;
                            Log2Weight2 ++;
                            ToShift2 >>= 1;
                        }


                        // To adapt this debug output for ringer elimination,
                        // make the first loop marker @+, rest of loops @-,
                        // then a grep of @- will show contrary words to rid.
                        // Also, make the string as it appears in CLan lists.

                        wchar_t EscTopWord[ 8 * MAX_LEGITIMATE_WORD_LENGTH + 1 ];
                        {
                            // Cloning from CVoc.cpp SaveVocabulary "AsCpp",
                            // Coding 0x20-0x7e=USASCII,
                            // 0x7f-0xff=octal escape,
                            // 0x0100-0x20ff = octal 1-32 prefix.

                            // This double octal escape could produce 8 chars per input char.

                            wchar_t * from = TopWord;
                            wchar_t * into = EscTopWord;
                            for( ;; )
                            {
                                wchar_t wc = *from++;
                                if( wc == NULL )
                                {
                                    *into = NULL;
                                    break;
                                }

                                // This coding scheme can do up to 0x20ff
                                if( wc > 0x00ff ) // introduce an octal prefix
                                {
                                    wchar_t wc2 = wc >> 8;
                                    *into++ = '\\'; // octal escape introducer
                                    *into++ = ( ( wc2 >> 6 ) & 3 ) + '0';
                                    *into++ = ( ( wc2 >> 3 ) & 7 ) + '0';
                                    *into++ = ( wc2 & 7 ) + '0';
                                    // No, do not inhibit octal encoding of lsb:
                                    // wc &= 0xff;
                                }
                                if( wc < ' '
                                || wc > '~' ) // encode the sort of special 0x7f char too
                                {
                                    *into++ = '\\'; // octal escape introducer
                                    *into++ = ( ( wc >> 6 ) & 3 ) + '0';
                                    *into++ = ( ( wc >> 3 ) & 7 ) + '0';
                                    *into++ = ( wc & 7 ) + '0';
                                }
                                else
                                {
                                    *into++ = ( char ) wc; // for 0x21-0xfe range
                                }

                            }
                        }

                        // So, after word is log2( wordweight )/log2( totalweight ):
                        wsprintf( wk, L"\r\n@%c %s %2d%% (%s %2d/%2d), ", // under 30 chars
                            LoopMarker,         // 1 char
                            LGName,             // up to 18 chars
                            PercentOfAllWords,  // up to 3
                            EscTopWord,         // up to 8 * MAX_LEGITIMATE_WORD_LENGTH
                            Log2Weight2,        // up to 2
                            Log2Weight );       // up to 2
                        m_pWsbGuessWork->Add( wk );;
                        #if DO_DEBUG_LANG_MATH
                            SpewTwo( L"GuessWork entry", wk );
                        #endif

                        // ...Mysterious CYA indent
                    }

                    LoopMarker = '-'; // for loops 2 on

                }
                #endif

                MyFree( 4054, zx, FullKey );
                FullKey = NULL;
            }
            MyFree( 4057, UNPREDICTABLE, pMalVector2 );
            pMalVector2 = NULL;
        }

        // Having looped for all languages, mention mul/unk counts.

        #if DO_DEBUG_LANG_GUESS
        {
            wchar_t wk[ 100 + MAX_LEGITIMATE_WORD_LENGTH ];

            if( WordCountMultiple > 0 )
            {
                int PercentOfAllWords = 100 * WordCountMultiple / WordCountTotal;

                size_t ToShift = HighestWeightInMultiple;
                size_t Log2Weight = 0;
                for( ;; )
                {
                    if( ToShift == 0 )
                        break;
                    Log2Weight ++;
                    ToShift >>= 1;
                }

                wchar_t * TopWord = HighestpMalWordInMultiple;
                size_t TopWeight = HighestWeightInMultiple;

                size_t ToShift2 = TopWeight;
                size_t Log2Weight2 = 0;
                for( ;; )
                {
                    if( ToShift2 == 0 )
                        break;
                    Log2Weight2 ++;
                    ToShift2 >>= 1;
                }

                // So, after word is log2( wordweight )/log2( totalweight ):
                wsprintf( wk, L"\r\n@= Multiple %2d%% (%s %2d/%2d), ", // under 30 chars
                    PercentOfAllWords,  // up to 3
                    TopWord,            // up to MAX_LEGITIMATE_WORD_LENGTH
                    Log2Weight2,        // up to 2
                    Log2Weight );       // up to 2
                m_pWsbGuessWork->Add( wk );;

                #if DO_DEBUG_LANG_MATH
                    SpewTwo( L"GuessWork entry", wk );
                #endif
            }

            if( WordCountUnknown > 0 )
            {
                int PercentOfAllWords = 100 * WordCountUnknown / WordCountTotal;

                size_t ToShift = HighestWeightInUnknown;
                size_t Log2Weight = 0;
                for( ;; )
                {
                    if( ToShift == 0 )
                        break;
                    Log2Weight ++;
                    ToShift >>= 1;
                }

                wchar_t * TopWord = HighestpMalWordInUnknown;
                size_t TopWeight = HighestWeightInUnknown;

                size_t ToShift2 = TopWeight;
                size_t Log2Weight2 = 0;
                for( ;; )
                {
                    if( ToShift2 == 0 )
                        break;
                    Log2Weight2 ++;
                    ToShift2 >>= 1;
                }

                // So, after word is log2( wordweight )/log2( totalweight ):
                wsprintf( wk, L"\r\n@? Unknown %2d%% (%s %2d/%2d), ", // under 30 chars
                    PercentOfAllWords,  // up to 3
                    TopWord,            // up to MAX_LEGITIMATE_WORD_LENGTH
                    Log2Weight2,        // up to 2
                    Log2Weight );       // up to 2
                m_pWsbGuessWork->Add( wk );;

                #if DO_DEBUG_LANG_MATH
                    SpewTwo( L"GuessWork entry", wk );
                #endif
            }
        }
        #endif

        // Having found first and second, and after I have thought
        // about proportions and rules, change the page's language.

        // This is really a mess. Here are some 1st and 2nd lines by SED.
        // The big problem is so many words get categorized as multiple.

        // This was all Unstated language pages from \Thyatira\un folder.
        // This was before some adjustments to the math...
        // ++ =? Multiple 100% ( other 27/27 ),  --
        // ++ =? Malay 0% ( seperti 22/23 ),  -- =? Italian 0% ( queste 17/20 ),
        // ++ =? Vietnamese 0% ( assistants 19/19 ),  -- =? Catalan 0% ( governs 13/13 ),
        // ++ =? Multiple 99% ( these 27/27 ),  -- =? Unknown 0% ( revisited 8/8 ),
        // ++ =? Dutch 1% ( regelen 18/20 ),  -- =? Afrikaans 0% ( mensdom 15/17 ),
        // But there were some better lines:
        // ++ =? German 23% ( weitere 28/30 ),  -- =? Catalan 2% ( autors 23/23 ),
        // ++ =? English 4% ( donate 15/17 ),  -- =? Dutch 0% ( gateway 15/15 ),
        // ++ =? English 7% ( until 19/23 ),  -- =? Hungarian 0% ( patience 22/23 ),
        // ++ =? Dutch 5% ( gemeenten 22/24 ),  -- =? Afrikaans 0% ( besef 17/19 ),
        // ++ =? Dutch 2% ( gezegende 20/22 ),  -- =? Basque 0% ( baten 19/19 ),
        // ++ =? German 14% ( dass 24/26 ),  -- =? Basque 0% ( bete 15/16 ),
        // ++ =? Dutch 3% ( reactie 23/24 ),  -- =? German 0% ( wladimir 17/17 ),
        // ++ =? German 7% ( staaten 17/20 ),  -- =? French 0% ( abonnement 16/17 ),

        // So lets set some heuristic thresholds to change page's language.

        // First, There must be a first language listed in the CSol!
        // First language must reach absolute levels of 1% and /20.
        // That /20 also implies some minimum page size to reach it.
        // First language must exceed sum of all other lang's %.
        // First language must exceed sum of all other lang's /WT by 3.
        // ( not sum of log2s; get sum of other as Total-ThisLang-MUL-UNK. )
        // ( similarly with %'s. )

        if( LoopCounter >= 1 )
        {
            // There was a first language with a word in it.

            size_t FirstWeight = WeightVector[ FirstLGroup ];
            size_t OtherWeight = WeightTotal - FirstWeight - WeightMultiple - WeightUnknown;
            size_t FirstWordCount = WordCountVector[ FirstLGroup ];
            size_t OtherWordCount = WordCountTotal - FirstWordCount - WordCountMultiple - WordCountUnknown;

            // Ehh, whatever rules I said, these are two ways of saying
            // That the predominant known language must be 1/2 of known.

            // I am of course still hurt by wrong languages spuriously
            // claiming NOT or THAT or AREA or FORM away from English,
            // but that must be solved by common words table revision.

            #if DO_DEBUG_LANG_MATH
                SpewValue( L"FirstWeight", FirstWeight );
                SpewValue( L"OtherWeight", OtherWeight );
                SpewValue( L"FirstWordCount", FirstWordCount );
                SpewValue( L"OtherWordCount", OtherWordCount );
            #endif

            if( FirstWeight > OtherWeight
            && FirstWordCount > OtherWordCount )
            {
                // Then we can change the language and group assessment.

                #if OKAY_TO_GUESS_LANG
                    m_pOnePaper->LanguageGroup = FirstLGroup;
                    m_pOnePaper->HttpHeaderContentLanguage = SomeLanguageIndexForGroupIndex( FirstLGroup );
                #endif

                // Which will affect the soon header annotation work.
                // Well, not unless I make the assignment, it won't!
                // My pointer IS into the paper. Maybe I have locals?
                // Nope. I need to debug so'mo.

                #if DO_DEBUG_LANG_GUESS
                    SpewValue( L"Setting m_pOnePaper->LanguageGroup", m_pOnePaper->LanguageGroup );
                    SpewValue( L"Setting m_pOnePaper->HttpHeaderContentLanguage", m_pOnePaper->HttpHeaderContentLanguage );
                #endif

            }
        }
    }
    else
    {
        #if DO_DEBUG_LANG_MATH
            Spew( L"WordCountTotal == 0 in page, so did not iterate." );
        #endif
    }

    // Must free all before return:
    int fai = 0;
    for( ;; )
    {
        if( HighestpMalWordVector[ fai ] != NULL )
        {
            MyFree( 899, zx, HighestpMalWordVector[ fai ] );
            HighestpMalWordVector[ fai ] = NULL;
        }
        if( ++fai == PAST_LANGUAGE_GROUP_IDS )
            break;
    }

    // must free before return:
    if( HighestpMalWordInMultiple != NULL )
    {
        MyFree( 1289, zx, HighestpMalWordInMultiple );
        HighestpMalWordInMultiple = NULL;
    }

    // must free before return:
    if( HighestpMalWordInUnknown != NULL )
    {
        MyFree( 1289, zx, HighestpMalWordInUnknown );
        HighestpMalWordInUnknown = NULL;
    }
}

void CTxt::AddWordsToOtherLists( )
{
    #if DO_DEBUG_CALLS
        Routine( L"276" );
    #endif
    // Having finished the loop that scanned across all text,
    // save for any header part and generated m_pSolWordList.
    // 1. Add all of those words to a per-language word list.
    // 2. Add all of those words to a all-language word list.
    // 3. I notice that I am NOT adding page words to common.

    // Up until now, I have added words to just CSolAllWords.
    // Now I am ready to group words into lists per language.
    // I need a vector of CSols, let CPag own/create/destroy.

    // I have already guessed the language if not stated,
    // when I write that code; It might still be UNKNOWN.

    size_t GroupID = m_pOnePaper->LanguageGroup;
    size_t PageGroupIDShl17 = GroupID << 17;

    CSol * pSolOneLG = Pag.CSolLGroupWords[ GroupID ];
    // if I have not seen this group before, create CSol jit.
    if( pSolOneLG == NULL )
    {
        pSolOneLG = new CSol( CSOL_SCALAR ); // create CSol jit
        Pag.CSolLGroupWords[ GroupID ] = pSolOneLG; // remember/own it
        TotalLanguageCount++;

        // If I have NOT salted the word lists at start-up,
        // I should do this one now if I have words for it.

        // Whether I did or not, some foreign user might get many
        // pages in some language I did not salt, and he needs his
        // language salted as soon as possible for further guessing.

        // BTW, suppose this is the second page in stated language.
        // Then it misses this IF clause, but still needs addition.

        // Oh, my bad. I am doing it below.

        // I just now created Pag.CSolLGroupWords[ GroupID ], JIT.
        // Next loop adds words to Pag.CSolLGroupWords[ GroupID ].
    }

    // So, I see this loop is DOING each word of the current page.
    // They probably only have a count, and that may be unclamped.

    CoIt * pMalVector = m_pSolWordList->GetSortedVector( CSOL_FORWARD );
    if( pMalVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;

        // Consumes Words and their counts from text parse.
        // Add words and valuation/counts to CSolAllWords.
        // Add words and valuation/counts to pSolOneLG.

        // The words coming to me from m_pSolWordList should
        // already obey whatever strictures I validate words.
        // Those are determined by rules in SimpleWordScan.

        // Once I had limited words to 3-9? lowercase alpha.

        // I think to not put a lower limit on word length.
        // Certainly, words at this point are full unicode.
        // All iswalpha characters in list were lowercased.
        // Conceivably, I may allow hyphens or apostraphes.
        // Conceivably, I may digits, but not in salt list.

        wchar_t * FullKey = CoItFullKey( pCoIt );
        size_t Strlen = wcslen( FullKey );

        // Remove the minimum length word test here:
        // No CoIt from GetSortedVector ever holds an empty string.
        // if( Strlen > 2 ...

        // Here is my easy place to reject proper names.
        // If I reject them here, they will not get into
        // such per-language lists as I have yet to build.
        //
        // But, I have so many lists built already...
        // To fix that, have the salter eliminate them,
        // just long enough to re-output fresh lists.


        // Just temporarily, until I can purge my wordlists,
        // test to omit all words found on proper name list.
        //
        // No, they've grown like topsy and have many common
        // words, like THE, which I cannot fail to have in
        // the common word list!
        //
        // if( CSolProperNames.Find( wk ) == 0 ) // NOT found among proper names.
        // ... just do it ...
        //
        // Oh, this IF has fucked up all my word lists!
        // I hope I still have the folders of words!


        if( Strlen < MAX_LEGITIMATE_WORD_LENGTH ) // only process such words as are
        {
            size_t WordCountInPage = pCoIt->User.Value;

            // The page word bumper did a naive ++, nobody added bit-fields.
            if( WordCountInPage > MAX_17BIT_WORD_COUNT )
                WordCountInPage = MAX_17BIT_WORD_COUNT;

            size_t DecisionAboutWordLGIDShl17 = LGID_SHL17_UNKNOWN;
            size_t DecisionAboutValShl24 = 0;

            // As long as CSolAllWords contains all the words that are
            // in any of Pag.CSolLGroupWords[ PAST_LANGUAGE_GROUP_IDS ],
            // I can just test All's User.Value as a boolean of novelty,
            // to control whether I must compute letter run valuation.

            // Wait. Not True! Salted Common words also knows valuation.
            // And my failure to consider that here has probably pumped
            // the count of MUL / UNK that I was seeing out of GuessWork.
            // Well, Common has salted all the pSolOneLG too. Use that.
            // No, because then I'd be double-counting the All's in One.
            // So duplicate the code above ( using All and Comn ) here.

            // Get a hold of both CSols which I will be consulting:
            // Get a hold of both CSols which I will be updating:

            // Oh, but not Find, as I definitely am adding here:
            size_t index1 = CSolAllWords.AddKey( FullKey );
            #if DO_DEBUG_ADDFIND
                if( index1 == 1 )
                    { Spew( L"AddFind 1 at ctxt 1573" ); }
            #endif
            size_t Value1 = CSolAllWords.GetUserValue( index1 );
            size_t AllValShl24 = Value1 & MASK_WORD_VALUE_SHL24; // redundant but equal
            size_t AllGidShl17 = Value1 & LGID_SHL17_MASK; // redundant but equal
            size_t CountInAll = Value1 & MASK_17BIT_WORD_COUNT;

            // Common does not get updated by page, so use Find and 'if'...
            size_t index2 = CSolCommonWords.Find( FullKey );
            #if DO_DEBUG_ADDFIND
                if( index2 == 1 )
                    { Spew( L"AddFind 1 at ctxt 1579" ); }
            #endif
            size_t ComnValShl24 = 0;
            size_t ComnGidShl17 = 0;
            size_t CountInComn = 0;
            if( index2 > 1 )
            {
                size_t Value2 = CSolCommonWords.GetUserValue( index2 );
                ComnValShl24 = Value2 & MASK_WORD_VALUE_SHL24; // redundant but equal
                ComnGidShl17 = Value2 & LGID_SHL17_MASK; // redundant but equal
                CountInComn = Value2 & MASK_17BIT_WORD_COUNT;
            }

            size_t index3 = pSolOneLG->AddKey( FullKey );
            #if DO_DEBUG_ADDFIND
                if( index3 == 1 )
                    { Spew( L"AddFind 1 at ctxt 1585" ); }
            #endif
            size_t Value3 = pSolOneLG->GetUserValue( index3 );
            size_t OneValShl24 = Value3 & MASK_WORD_VALUE_SHL24; // redundant but equal
            size_t OneGidShl17 = Value3 & LGID_SHL17_MASK; // redundant but equal
            size_t CountInOne = Value3 & MASK_17BIT_WORD_COUNT;

            // Finally straightening it out...
            // Now prefer ALL to COMN about language.

            if( CountInAll > 0 )
            {
                DecisionAboutWordLGIDShl17 = AllGidShl17;
                DecisionAboutValShl24 = AllValShl24;

                // Now here is the controverisal heuristic, 1 of 2 plcs.
                if( DecisionAboutWordLGIDShl17 == LGID_SHL17_UNKNOWN )
                {
                    DecisionAboutWordLGIDShl17 = PageGroupIDShl17;
                }
                else
                {
                    // I had a prior opinion about this word.
                    // I don't want to be dogmatic about it.
                    // If word is well represented in this page,
                    // and page is in some specific language...
                    // Then go so far as to reassign to multiple.
                    if( PageGroupIDShl17 != LGID_SHL17_UNKNOWN
                    && PageGroupIDShl17 != LGID_SHL17_MULTIPLE
                    && WordCountInPage > 5 ) // arbitary threshold to ignore.
                    {
                        DecisionAboutWordLGIDShl17 = LGID_SHL17_MULTIPLE;
                    }
                }
            }
            else if( CountInComn > 0 )
            {
                DecisionAboutWordLGIDShl17 = ComnGidShl17;
                DecisionAboutValShl24 = ComnValShl24;

                // Now here is the controverisal heuristic, 2 of 2 plcs.
                if( DecisionAboutWordLGIDShl17 == LGID_SHL17_UNKNOWN )
                {
                    DecisionAboutWordLGIDShl17 = PageGroupIDShl17;
                }
                else
                {
                    // I had a prior opinion about this word.
                    // I don't want to be dogmatic about it.
                    // If word is well represented in this page,
                    // and page is in some specific language...
                    // Then go so far as to reassign to multiple.
                    if( PageGroupIDShl17 != LGID_SHL17_UNKNOWN
                    && PageGroupIDShl17 != LGID_SHL17_MULTIPLE
                    && WordCountInPage > 5 ) // arbitary threshold to ignore.
                    {
                        DecisionAboutWordLGIDShl17 = LGID_SHL17_MULTIPLE;
                    }
                }
            }
            else
            {
                DecisionAboutWordLGIDShl17 = PageGroupIDShl17;
                // Novel word. Generate its letter run valuation.
                size_t WordValue = WordLetterValuation( FullKey );
                if( WordValue > MAX_WORD_VALUE )
                    WordValue = MAX_WORD_VALUE;
                DecisionAboutValShl24 = WordValue << 24;
            }

            // Novel or not now, update everybody's counters.
            // I don't update Common, even if I changed the LGroup.

            // Create 17 lsb jointly.

            CountInAll += WordCountInPage;
            if( CountInAll > MAX_17BIT_WORD_COUNT )
                CountInAll = MAX_17BIT_WORD_COUNT;

            CountInOne += WordCountInPage;
            if( CountInOne > MAX_17BIT_WORD_COUNT )
                CountInOne = MAX_17BIT_WORD_COUNT;

            // Finally, update both CSols to which I will be adding words:
            size_t HighPart = DecisionAboutWordLGIDShl17 | DecisionAboutValShl24;
            // Indices: 1 is all, 2 is comn, 3 is one.
            CSolAllWords.SetUserValue( index1, CountInAll | HighPart );
            pSolOneLG->SetUserValue( index3, CountInOne | HighPart );
        }
        MyFree( 282, zx, FullKey );
        FullKey = NULL;
    }

    MyFree( 286, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;
}

size_t CTxt::SentenceScan( )
{
    #if DO_DEBUG_CALLS
        Routine( L"277" );
    #endif
    // There are no new annotations yet in the input text.
    // Find and index boundarys of probably sentences.
    // Like I did for links in CIdx * pIdxResultIndex,
    // add sentence boundaries to CIdx * pIdxSentences
    // and add block boundaries to CIdx * pIdxTextBlocks.
    // Return a page score, which is sum of block scores.

    // Return a "score" metric, say, sum of sentence metrics.
    // Actually, after doing some logs and range adjustments.

    double SumPageValue = 0.0;

    // SimpleWordScan was always called just before me.
    // SimpleWordScan set nulls, lowercased everything.
    // Therefore, I must get a fresh copy of LocalCopy.

    if( LocalCopy != NULL )
    {
        MyFree( 302, zx, LocalCopy );
        LocalCopy = NULL;
    }
    LocalCopy = m_pOnePaper->pWsbResultText->GetBuffer( & nLocalCopy );

    wchar_t * scan = LocalCopy;
    wchar_t * past = scan + nLocalCopy;

    // This loop just scans for blocks, and calls
    // a helper to scan each block for sentences.

    // Later, I should think that "block" analysis might intelligently
    // include a header-type isolated line above some paragraph blocks.

    wchar_t * atop = scan;

    for( ;; )
    {
        if( scan == past )
        {
            // This IS past the end, and I WILL stop.
            if( scan != atop )
            {
                // A final block was left unreported at end.

                // A final double newline should have reported block.

                // check for a final single newline at end, back over it.
                // I believe my newlines have all been rectified to CR LF
                if( scan >= LocalCopy + 2
                && scan[-2] == '\r'
                && scan[-1] == '\n' )
                    scan -= 2;

                *scan = NULL; // stop some look-ahead heuristics.
                if( scan > atop + 1 )
                {
                    SumPageValue += DivideBlockIntoSentences( atop, scan, atop - LocalCopy );
                }
            }
            break;
        }

        // I believe my newlines have all been rectified to CR LF
        if( scan[0] == '\r'
        &&  scan[1] == '\n'
        &&  scan[2] == '\r'
        &&  scan[3] == '\n' )
        {
            *scan = NULL; // stop some look-ahead heuristics.

            if( scan > atop + 1 )
            {
                SumPageValue += DivideBlockIntoSentences( atop, scan, atop - LocalCopy );
            }

            scan += 4; // next char after newline
            atop = scan;
            continue; // avoid ++
        }

        scan ++;
    }

   // // Now figure this page's score.
   // // Logs convert the raw score sum to a manageable range.

   // // For zero in, produce zero out.
   // size_t QMetric = 0;
   // if( SumPageValue > 0 )
   // {
   //     double LogSum = log10( SumPageValue );

   //     // What is a reasonable range of logarithms?
   //     // If every word is unique, and lots of them,
   //     // like a dictionary file, it could be as big
   //     // as the log of word count, say, 5 of 100000.
   //     // If text has one word of 100000 counts, -5.

   //     // Adding a factor of 0-255 for letter values,
   //     // just raised potential top limit about +2.5.

   //     // To be generous, add 20, to range 15 to 25.
   //     // Then multiply by 10000 into max at 250000.

   //     double Qdouble = ( LogSum + 20.0 ) * 10000.0;
   //     // scary stuff. Limit range to 6 digits, +.
   //     if( Qdouble > 400000.0 )
   //         Qdouble = 400000.0;
   //     if( Qdouble < 0.0 )
   //         Qdouble = 0.0;

   //     // I expect the sum of novelty to increase linearly
   //     // with length, in equal rate of novel word usages.
   //     // If I took log of that, also take log of length.
   //     // Hmmm. IT still favors short sentences too much.
   //     // No, add more to LogLen until it seems balanced.

   //     double Length = nLocalCopy + 1;
   //     double LogLen = log10( Length );
   //     LogLen += 4.0; // empirical adjustment, could study more.
   //     QMetric = ( size_t ) ( Qdouble / LogLen ); // never is / 0
   // }

    return ( size_t ) SumPageValue;
}

double CTxt::DivideBlockIntoSentences( wchar_t * atop, wchar_t * past, int AtopOffset )
{
    #if DO_DEBUG_CALLS
        Routine( L"278" );
    #endif
    // Add sentence boundaries to CIdx * pIdxSentences.
    // Add block boundaries to CIdx * pIdxTextBlocks.
    // Return a "score" metric, say, sum of sentence metrics.
    // Actually, after some scaling using logs, as an int. No;
    // We're still summing for a page, so return the pre-log sum.

    double SumBlockValue = 0.0;

    int BlockMeritSum = 0;
    int Significance = ENUM_SIGNIFY_NULL_HINT;
    // If each sentence added does ++, and rejected does --, then:
    // > 1  ENUM_SIGNIFY_VERY_GOOD
    // == 1 ENUM_SIGNIFY_SLIGHT_GOOD
    // == 0 ENUM_SIGNIFY_NULL_HINT
    // < 0  ENUM_SIGNIFY_ANATHEMA


    // This routine will more-or-less use the UNICODE recommendations.

    // Ignoring some rules for languages of no interest to me:
    // These are some notes from Unicode SENTENCE-BOUNDARY rules:

    // Break at the start and end of text.

    // Break after paragraph separators.

    // Do not break after ambiguous terminators like period, if they are
    // immediately followed by a number or lowercase letter, if they are between
    // uppercase letters, or if the first following letter ( optionally after
    // certain punctuation ) is lowercase. For example, a period may be an
    // abbreviation or numeric period, and thus may not mark the end of a
    // sentence.

    // Break after sentence terminators, but include closing punctuation,
    // trailing spaces, and a paragraph separator ( if present ).

    // These rules cannot detect cases like ...Mr. Jones..., which tailor.
    // I would also detect cases of abbreviated names, like "A. O. Spare".

    // Close General_Category = Open_Punctuation ( Po ), or
    // General_Category = Close_Punctuation ( Pe ), or
    // Linebreak = Quotation ( QU )
    // and not U+05F3 ( ׳ ) HEBREW PUNCTUATION GERESH
    // and ATerm = false
    // and STerm = false

    // There are very many "sentence terminators" but glean
    // from this table just the ! and ?, I think:
    // 0021          ; STerm # Po       EXCLAMATION MARK
    // 003F          ; STerm # Po       QUESTION MARK
    // 055C          ; STerm # Po       ARMENIAN EXCLAMATION MARK
    // 055E          ; STerm # Po       ARMENIAN QUESTION MARK
    // 0589          ; STerm # Po       ARMENIAN FULL STOP
    // 061F          ; STerm # Po       ARABIC QUESTION MARK
    // 06D4          ; STerm # Po       ARABIC FULL STOP
    // 0700..0702    ; STerm # Po   [3] SYRIAC END OF PARAGRAPH..SYRIAC SUBLINEAR FULL STOP
    // 07F9          ; STerm # Po       NKO EXCLAMATION MARK
    // 0964..0965    ; STerm # Po   [2] DEVANAGARI DANDA..DEVANAGARI DOUBLE DANDA
    // 104A..104B    ; STerm # Po   [2] MYANMAR SIGN LITTLE SECTION..MYANMAR SIGN SECTION
    // 1362          ; STerm # Po       ETHIOPIC FULL STOP
    // 1367..1368    ; STerm # Po   [2] ETHIOPIC QUESTION MARK..ETHIOPIC PARAGRAPH SEPARATOR
    // 166E          ; STerm # Po       CANADIAN SYLLABICS FULL STOP
    // 1803          ; STerm # Po       MONGOLIAN FULL STOP
    // 1809          ; STerm # Po       MONGOLIAN MANCHU FULL STOP
    // 1944..1945    ; STerm # Po   [2] LIMBU EXCLAMATION MARK..LIMBU QUESTION MARK
    // 1B5A..1B5B    ; STerm # Po   [2] BALINESE PANTI..BALINESE PAMADA
    // 1B5E..1B5F    ; STerm # Po   [2] BALINESE CARIK SIKI..BALINESE CARIK PAREREN
    // 203C..203D    ; STerm # Po   [2] DOUBLE EXCLAMATION MARK..INTERROBANG
    // 2047..2049    ; STerm # Po   [3] DOUBLE QUESTION MARK..EXCLAMATION QUESTION MARK
    // 3002          ; STerm # Po       IDEOGRAPHIC FULL STOP
    // A876..A877    ; STerm # Po   [2] PHAGS-PA MARK SHAD..PHAGS-PA MARK DOUBLE SHAD
    // FE52          ; STerm # Po       SMALL FULL STOP
    // FE56..FE57    ; STerm # Po   [2] SMALL QUESTION MARK..SMALL EXCLAMATION MARK
    // FF01          ; STerm # Po       FULLWIDTH EXCLAMATION MARK
    // FF0E          ; STerm # Po       FULLWIDTH FULL STOP
    // FF1F          ; STerm # Po       FULLWIDTH QUESTION MARK
    // FF61          ; STerm # Po       HALFWIDTH IDEOGRAPHIC FULL STOP

    // Sentence rules about "CLOSE" have this short list,
    // and many more in languages I do not care about:
    // 0022          ; Close # Po       QUOTATION MARK
    // 0027          ; Close # Po       APOSTROPHE
    // 0028          ; Close # Ps       LEFT PARENTHESIS
    // 0029          ; Close # Pe       RIGHT PARENTHESIS
    // 005B          ; Close # Ps       LEFT SQUARE BRACKET
    // 005D          ; Close # Pe       RIGHT SQUARE BRACKET
    // 007B          ; Close # Ps       LEFT CURLY BRACKET
    // 007D          ; Close # Pe       RIGHT CURLY BRACKET
    // 00AB          ; Close # Pi       LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    // 00BB          ; Close # Pf       RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    //     but keep these:
    // 2018          ; Close # Pi       LEFT SINGLE QUOTATION MARK
    // 2019          ; Close # Pf       RIGHT SINGLE QUOTATION MARK
    // 201A          ; Close # Ps       SINGLE LOW-9 QUOTATION MARK
    // 201B..201C    ; Close # Pi   [2] SINGLE HIGH-REVERSED-9 QUOTATION MARK..LEFT DOUBLE QUOTATION MARK
    // 201D          ; Close # Pf       RIGHT DOUBLE QUOTATION MARK
    // 201E          ; Close # Ps       DOUBLE LOW-9 QUOTATION MARK
    // 201F          ; Close # Pi       DOUBLE HIGH-REVERSED-9 QUOTATION MARK

    // Although my new caller above has found double newlines and
    // used them to delimit block boundaries, there may be single
    // newlines ( CR + LF pair ) in a block. Treat like plain space.
    // Block boundary contains a NULL to stop certain look-aheads.

    wchar_t * scan = atop; // until NULL
    int n = past - atop; // No, I will index scan[] until N

    OpenBlockInPipeline( AtopOffset, AtopOffset + n );

    // Since I will re-parse and estimate a "value" for words,
    // the SimpleWordScan must have already run to list words.

    // I think this outer loop should just find the terminators,
    // and subcontract the scan of words in any sentences found.

    // If any terminator is found, block will be subdivided into
    // sentences from the SOB to the EOB, including whole block.

    // Blocks containing no terminators will be unlisted, "signage".
    // Some potential sentences might also be rejected as "signage".

    // afix a NULL terminator at end.
    wchar_t SavedPast = *past;
    *past = NULL;

    // It is sufficient if all ignored fragments are output...
    // #if DO_DEBUG_SENTENCES
    //     Spew( L"" );
    //     SpewTwo( L"BLOCK", atop );
    //     SpewValue( L"Len ", past-atop );
    //     Spew( L"" );
    // #endif

    // New plan:
    // I decided not to use this routine, but it is full of comments
    // as I develop my heuristics. Let the DivideBlockIntoSentences
    // routine, with its accurate rules, also be tasked with anchor
    // delimiter recognition; And let ConsiderPotentialSentence not
    // immediately add sentence to IDX, but just queue them into a
    // two-level pipeline: At one level of pipeline are blocks, up
    // to say, 3 blocks; Within them are sentences, 0 to unlimited;
    // After each block parse, and at end of page, Call new routine
    // to either save or discard items from the pipeline into IDXs.
    //
    // if( PrescanBlockShowsPatterns( atop, past ) )
    // {
    //     // Signage or Tables. Do not divide this block into sentences.
    //
    //     // In fact, that is the very decision I am seeking to
    //     // optionally eliminate "signage" from the view. Just
    //     // omit to add such blocks to pIdxTextBlocks, and then
    //     // the view can use this list to determine which subset
    //     // of all blocks to draw to the view, to avoid signage.
    //
    //     // No... m_pOnePaper->pIdxTextBlocks->AddIdx( AtopOffset, n + AtopOffset, ( size_t ) SumBlockValue, 0 );
    //
    //     return SumBlockValue; // empty, no value.
    // }

    int i = 0;
    // Do a first loop to reject whitespace from atop blocks.
    for( ;; )
    {
        if( i == n )
        {
            #if DO_DEBUG_BLOCK_NESS
                Spew( L"  0   EMPTY BLOCK" );
            #endif
            *past = SavedPast; // restore
            CloseBlockInPipeline( ( size_t ) SumBlockValue, Significance );
            return SumBlockValue; // empty, no value.
        }
        if( scan[i] > ' ' ) // skip SPACE ( expected ), newlines if any.
            break;
        i ++;
    }

    // Now, this is where first sentence might really start.
    int iPastWhitespace = i;
    int iAtopSentence = i;

    wchar_t wcPrior = 0; // to prevent [-1] access
    wchar_t wcPrior2 = 0; // to prevent [-2] access
    wchar_t wcPrior3 = 0; // to prevent [-3] access
    int InAlphaToken = 0;
    int AlphaTokenCountInSentence = 0;
    // Diminish effects of dash and brackets
    // by scaling other link symptoms higher.
    int LinkSymptomCountInSentence = 0;
    int SemicolonCountInSentence = 0;
    int CommaCountInSentence = 0;
    int ColonCountInSentence = 0;

    // This sentence parse is adequate, quite good.
    // Add new variables to the per-wchar loop, to
    // recognize just a few common anchor symptoms,
    // to estimate merit of block for the pipeline.

    // Add an initial "[" recognizer
    if( scan[i] == '[' )
        LinkSymptomCountInSentence += 4;

    for( ;; )
    {
        if( i == n )
        {
            // Add a final "]" recognizer
            if( wcPrior == ']' )
                LinkSymptomCountInSentence +=4;

            // Just because of other sentences in a block
            // does not qualify this as a final sentence.

            // if( i > iAtopSentence
            // && iAtopSentence != iPastWhitespace ) // Only if a prior terminator was found
            // {
            //     SumBlockValue += ConsiderPotentialSentence( scan + iAtopSentence, scan + i, AtopOffset + iAtopSentence );
            // }

            // On the other hand, I get whole blocks that
            // seem like sentences, but end with a colon.
            // Normally, colon does not separate sentences,
            // but recover finals. Perhaps also semicolon.
            // Perhaps also comma. ( Period was handled. )

            // But now I have gotten as sentences,
            // huges comma or sem delimited lists.

            // This is worth a separate exit scan. Just a trivy.

            if( i > iAtopSentence )
            {
                for( ;; )
                {
                    if( --i == iAtopSentence )
                        break; // no colon found.
                    wchar_t wcend = scan[i];
                    if( iswalnum( wcend ) )
                        break; // no colon found.
                    if( wcend == ':'
                    || wcend == ','
                    || wcend == ';' )
                    {
                        #if DO_DEBUG_SENTENCES
                            Spew( L"" );
                            SpewValue( L"Final Colon Semicolon or Comma", i );
                        #endif
                        // Keep it past i, to, well... past!

                        // Diminish effects of dash and brackets
                        // by scaling other link symptoms higher.
                        if( AlphaTokenCountInSentence > 0
                        && AlphaTokenCountInSentence < 500
                        && SemicolonCountInSentence < 20
                        && CommaCountInSentence < 20
                        && ColonCountInSentence < 20
                        && LinkSymptomCountInSentence < 8 )
                        {
                            BlockMeritSum ++;
                            SumBlockValue += ConsiderPotentialSentence( scan + iAtopSentence, past, AtopOffset + iAtopSentence );
                        }
                        else
                        {
                            BlockMeritSum --;
                            RejectedPotentialSentence( scan + iAtopSentence, past, AtopOffset + iAtopSentence );
                            #if DO_DEBUG_SENTENCES
                                SpewValue( L"AlphaTokenCountInSentence", AlphaTokenCountInSentence );
                                SpewValue( L"SemicolonCountInSentence", SemicolonCountInSentence );
                                SpewValue( L"CommaCountInSentence", CommaCountInSentence );
                                SpewValue( L"ColonCountInSentence", ColonCountInSentence );
                                SpewValue( L"LinkSymptomCountInSentence", LinkSymptomCountInSentence );
                            #endif
                        }
                        i = iAtopSentence = n; // for next exit selftest...
                        break; // How could I forget to break!?
                    }
                }
            }


            if( i > iAtopSentence )
            {
                // Since the sentence division parse has no way to leave
                // gaps between sentences; My spewing anything left past
                // last sentence if any, will show me roughly everything.

                if( iAtopSentence > iPastWhitespace )
                {
                    #if DO_DEBUG_SENTENCES | DO_DEBUG_LEFTOVERS
                        SpewTwo( L"Leftovers", scan + iAtopSentence  );
                    #endif
                }
                else
                {
                    #if DO_DEBUG_SENTENCES | DO_DEBUG_LEFTOVERS
                        SpewTwo( L"IGNORED", scan + iAtopSentence  );
                    #endif
                }

                // Whether a whole ignored block, or leftovers, spew it:
                RejectedPotentialSentence( scan + iAtopSentence, past, AtopOffset + iAtopSentence );
            }

            // remember, the outer loop is ending here...
            break;
        }

        wchar_t wc = scan[i];

        // #if DO_DEBUG_SENTENCES
        // {
        //     wchar_t wk[2];
        //     wk[0] = wc;
        //     wk[1] = NULL;
        //     Spew( wk );
        // }
        // #endif

        if( iswalnum( wc ) )
        {
            if( ! InAlphaToken )
            {
                InAlphaToken = 1;
                AlphaTokenCountInSentence ++;
            }
            goto RejectedPeriod; // bottom of loop. Save lots of if statements.
        }
        else
        {
            if( InAlphaToken )
            {
                InAlphaToken = 0;
            }
        }

        // So we are in non-alphas here...

        if( wc == ';' )
            SemicolonCountInSentence ++;

        if( wc == ',' )
            CommaCountInSentence ++;

        if( wc == ':' )
            ColonCountInSentence ++;

        // Just tune up this undesireable pattern recognizer for:
        // " | " - separates links per HTML parse and in web pages
        // " > " - separates hierarchical link sequence in web pages
        // " - " -- heavily used as non-sentence list separator
        // "] " -- and add a final "]" recognizer
        // " [" -- and add an initial "[" recognizer
        // It is safe to examine scan[i+1] here; I didn't reach null.

        // Diminish effects of dash and brackets
        // by scaling other link symptoms higher.
        if( wc == '|' && wcPrior == ' ' && scan[i+1] == ' '
        ||  wc == '>' && wcPrior == ' ' && scan[i+1] == ' ' )
        {
            LinkSymptomCountInSentence += 4;
        }
        if( wc == '-' && wcPrior == ' ' && scan[i+1] == ' ' )
        {
            LinkSymptomCountInSentence += 2;
        }
        // I was allowing to many lines full of link-symptom brackets.
        // Raise mid-text bracket offence to 2;
        // Do not require a space with bracket.
        if( wc == '['
        ||  wc == ']' )
        {
            LinkSymptomCountInSentence += 2;
        }

        if( wc == '?'
        || wc >= 0x055C &&
            (  wc == 0x055C // ; STerm # Po       ARMENIAN EXCLAMATION MARK
            || wc == 0x055E // ; STerm # Po       ARMENIAN QUESTION MARK
            || wc == 0x0589 // ; STerm # Po       ARMENIAN FULL STOP
            || wc == 0x061F // ; STerm # Po       ARABIC QUESTION MARK
            || wc == 0x06D4 // ; STerm # Po       ARABIC FULL STOP
            || wc == 0x0700 // ; STerm # Po   [3] SYRIAC END OF PARAGRAPH..SYRIAC SUBLINEAR FULL STOP
            || wc == 0x0701 // ; STerm # Po   [3] SYRIAC END OF PARAGRAPH..SYRIAC SUBLINEAR FULL STOP
            || wc == 0x0702 // ; STerm # Po   [3] SYRIAC END OF PARAGRAPH..SYRIAC SUBLINEAR FULL STOP
            || wc == 0x1362 // ; STerm # Po       ETHIOPIC FULL STOP
            || wc == 0x1367 // ; STerm # Po   [2] ETHIOPIC QUESTION MARK..ETHIOPIC PARAGRAPH SEPARATOR
            || wc == 0x1368 // ; STerm # Po   [2] ETHIOPIC QUESTION MARK..ETHIOPIC PARAGRAPH SEPARATOR
            || wc == 0x166E // ; STerm # Po       CANADIAN SYLLABICS FULL STOP
            || wc == 0x203C // ; STerm # Po   [2] DOUBLE EXCLAMATION MARK..INTERROBANG
            || wc == 0x203D // ; STerm # Po   [2] DOUBLE EXCLAMATION MARK..INTERROBANG
            || wc == 0x2047 // ; STerm # Po   [3] DOUBLE QUESTION MARK..EXCLAMATION QUESTION MARK
            || wc == 0x2048 // ; STerm # Po   [3] DOUBLE QUESTION MARK..EXCLAMATION QUESTION MARK
            || wc == 0x2049 // ; STerm # Po   [3] DOUBLE QUESTION MARK..EXCLAMATION QUESTION MARK
            || wc == 0x3002 // ; STerm # Po       IDEOGRAPHIC FULL STOP
            )
        || wc == '!' )
        {
            // #if DO_DEBUG_SENTENCES
            //     Spew( L"" );
            //     SpewValue( L"STerm", i );
            // #endif

            int iPastSentence = i + 1;
            // Now I must scan past the terminator to include certain punct.
            // I think accept quotes and close puntuation, until any spaces.
            // "Be more exhaustive." [1] Accept all chars until first space.
            // Now I need to gather in closures and spaced-out ellipses. . .
            for( ;; )
            {
                if( scan[ iPastSentence ] == NULL ) // safe: stop on NULL
                    break;
                if( scan[ iPastSentence ] <= ' ' )
                {
                    // Test next char to keep any closures and . . . .
                    // Quotes will be trickier to assign left & right.
                    // How about dash?
                    wchar_t wcn = scan[ iPastSentence + 1 ];
                    if( wcn != '.'
                    &&  wcn != '?'
                    &&  wcn != '!'
                    &&  wcn != ':'
                    &&  wcn != ';'
                    &&  wcn != '-'
                    &&  wcn != '}'
                    &&  wcn != ')'
                    &&  wcn != ']' )
                        break;
                }
                iPastSentence ++;
            }
            // Diminish effects of dash and brackets
            // by scaling other link symptoms higher.

            // Gee, ! in huge word lists get them into the best-ranked.
            // Add the same token count qualifications as for periods.

            if( AlphaTokenCountInSentence > 0
            && AlphaTokenCountInSentence < 500
            && SemicolonCountInSentence < 20
            && CommaCountInSentence < 20
            && ColonCountInSentence < 20
            && LinkSymptomCountInSentence < 8 )
            {
                BlockMeritSum ++;
                SumBlockValue += ConsiderPotentialSentence( scan + iAtopSentence, scan + iPastSentence, AtopOffset + iAtopSentence );
            }
            else
            {
                BlockMeritSum --;
                RejectedPotentialSentence( scan + iAtopSentence, scan + iPastSentence, AtopOffset + iAtopSentence );
                #if DO_DEBUG_SENTENCES
                    SpewValue( L"AlphaTokenCountInSentence", AlphaTokenCountInSentence );
                    SpewValue( L"SemicolonCountInSentence", SemicolonCountInSentence );
                    SpewValue( L"CommaCountInSentence", CommaCountInSentence );
                    SpewValue( L"ColonCountInSentence", ColonCountInSentence );
                    SpewValue( L"LinkSymptomCountInSentence", LinkSymptomCountInSentence );
                #endif
            }
            // Simple advance of Atop and i -- just pass any whitespace.
            iAtopSentence = iPastSentence;
            for( ;; )
            {
                if( scan[ iAtopSentence ] == NULL // safe: stop on NULL
                ||  scan[ iAtopSentence ] > ' ' )
                    break;
                iAtopSentence ++;
            }
            wcPrior = 0;
            wcPrior2 = 0;
            wcPrior3 = 0;

            // Whether sequence was accepted or rejected,
            // I need to reset these per-sentence things:
            // Hey -- Two places or more!
            AlphaTokenCountInSentence = 0;
            LinkSymptomCountInSentence = 0;
            SemicolonCountInSentence = 0;
            CommaCountInSentence = 0;
            ColonCountInSentence = 0;

            i = iAtopSentence;
            continue; // having replaced normal end-of-loop advance
        }
        else if( wc == '.' )
        {
            // #if DO_DEBUG_SENTENCES
            //     Spew( L"" );
            //     SpewValue( L"Period", i );
            // #endif

            // First, I must qualify the "ambiguous" terminator, a period.

            // Do not break after ambiguous terminators like period, if they are
            // immediately followed by a number or lowercase letter, if they are between
            // uppercase letters, or if the first following letter ( optionally after
            // certain punctuation ) is lowercase. For example, a period may be an
            // abbreviation or numeric period, and thus may not mark the end of a
            // sentence.

            wchar_t wc1 = scan[i+1];

            if( iswlower( wc1 )
            || iswdigit( wc1 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for immediate next lower or number." );
                #endif
                goto RejectedPeriod;
            }

            if( iswupper( wc1 )
            && iswupper( wcPrior ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for uppercase before and after it." );
                #endif
                goto RejectedPeriod;
            }

            if( iswupper( wcPrior ) // This rejects initials like "A. O. Spare"
            && ! isalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for solitary prior uppercase." );
                #endif
                goto RejectedPeriod;
            }

            // I think I need a CSol to hold all the abbrev to ignore.
            // Oh, but only those that tend to have UC following!


            // Oops, don't forget to check that behind them's non-alnum.

            // There's a lot more I could do here:
            // All the bible book abbreviations,
            // 49 ibid.,
            // 22 ( i.e.,
            // 14 a.m.


            // Do the ONE char cases first:
            // ( Test above only excluded singleton if uppercase )

            if( ( wcPrior | ' ' ) == 'p'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for p. abbrev." );
                #endif
                goto RejectedPeriod;
            }


            if( ( wcPrior | ' ' ) == 'i'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for i. Roman Numeral." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior | ' ' ) == 'v'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for v. Roman Numeral." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior | ' ' ) == 'x'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for x. Roman Numeral." );
                #endif
                goto RejectedPeriod;
            }


            // Do the TWO char cases next:

            if( ( wcPrior2 | ' ' ) == 'i'
            && ( wcPrior | ' ' ) == 'i'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for ii. Roman Numeral." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'i'
            && ( wcPrior | ' ' ) == 'v'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for iv. Roman Numeral." );
                #endif
                goto RejectedPeriod;
            }


            if( ( wcPrior | ' ' ) == 'a'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for a. of outline." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior | ' ' ) == 'b'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for b. of outline." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior | ' ' ) == 'c'
            && ! iswalnum( wcPrior2 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for c. of outline." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'p'
            && ( wcPrior | ' ' ) == 'g'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for pg. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'p'
            && ( wcPrior | ' ' ) == 'p'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for pp. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'c'
            && ( wcPrior | ' ' ) == 'f'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for cf. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'r'
            && ( wcPrior | ' ' ) == 'e'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for re. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( wcPrior2 == '&'
            && ( wcPrior | ' ' ) == 'c'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for &c. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'n'
            && ( wcPrior | ' ' ) == 'o'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for no. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'e'
            && ( wcPrior | ' ' ) == 'd'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for ed. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior2 | ' ' ) == 'v'
            && ( wcPrior | ' ' ) == 's'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for vs. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( wcPrior2 == 'D'
            && wcPrior == 'r'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for Dr. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( wcPrior2 == 'M'
            && wcPrior == 'r'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for Mr. abbrev." );
                #endif
                goto RejectedPeriod;
            }
            if( wcPrior2 == 'S'
            && wcPrior == 't'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for St. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( wcPrior2 == 'M'
            && wcPrior == 's'
            && ! iswalnum( wcPrior3 ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for Ms. abbrev." );
                #endif
                goto RejectedPeriod;
            }


            // Do the THREE char cases next:
            //
            // ( Which means I must optionally look backwards to -4. )

            // If I phrased this as I did wanting to see a letter:
            // && i >= iAtopSentence + 4 && scan[i-4] == ...
            // Then I'd lose the case atop sentences.
            // Phrase it just enough to protect access:
            // && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )

            if( ( wcPrior3 | ' ' ) == 'i'
            && ( wcPrior2 | ' ' ) == 'i'
            && ( wcPrior | ' ' ) == 'i'
            && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for iii. Roman numeral." );
                #endif
                goto RejectedPeriod;
            }


            if( wcPrior3 == 'M'
            && wcPrior2 == 'r'
            && wcPrior == 's'
            && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for Mrs. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( wcPrior3 == 'D'
            && wcPrior2 == 'r'
            && wcPrior == 's'
            && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for Drs. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior3 | ' ' ) == 'i'
            && ( wcPrior2 | ' ' ) == 'n'
            && ( wcPrior | ' ' ) == 'c'
            && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for inc. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior3 | ' ' ) == 'r'
            && ( wcPrior2 | ' ' ) == 'e'
            && ( wcPrior | ' ' ) == 'v'
            && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for rev. abbrev." );
                #endif
                goto RejectedPeriod;
            }

            if( ( wcPrior3 | ' ' ) == 'v'
            && ( wcPrior2 | ' ' ) == 'e'
            && ( wcPrior | ' ' ) == 'r'
            && ( i-4 < iAtopSentence || ! iswalnum( scan[i-4] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for ver. abbrev." );
                #endif
                goto RejectedPeriod;
            }


            // To test more then 3 letter, I will have to actively
            // verify how far negative I can index before doing it.


            // In the beginning, I set: i = iAtopSentence;
            // Atop this loop, I did: wchar_t wc = scan[i];
            // At end of this loop, I have yet to do: i ++;
            // wc is of course, on the period. So index -4.

            // Four char item: ibid.
            if( ( wcPrior3 | ' ' ) == 'b' // letter 2 of 4
            && ( wcPrior2 | ' ' ) == 'i'  // letter 3 of 4
            && ( wcPrior | ' ' ) == 'd'   // letter 4 of 4
            && i >= iAtopSentence + 4 && scan[i-4] == 'i' // letter 1 of 4
            && ( i-5 < iAtopSentence || ! iswalnum( scan[i-5] ) ) )
            {
                #if DO_DEBUG_SENTENCES
                    Spew( L"Rejected period for ibid. abbrev." );
                #endif
                goto RejectedPeriod;
            }


            // I need a case to reject prefixes like this:
            // But maybe they are predominantly in headers?
            // Sentence: 1.
            // It is really needful, worth a scan backwards.

            // Now tackle the case of various leading numbers.
            // Only apply these if alone atop sentence so far.
            // Accept any punct chars shown in these examples:
            //
            //  335 -O- 2.<<==
            //   80 -O- ( 2. )<<==
            //   18 -O- [2.]<<==
            //   18    | 2:14 ).<<==
            //   10    | 5:19.<<==
            //    8 -X- 53.3.<<==
            //    1 $139.00

            if( iswdigit( wcPrior )
            || iswdigit( wcPrior2 )
            || iswdigit( wcPrior3 ) )
            {
                // Go in after it: Study all chars from iAtopSentence to i-1.
                int AllSuch = 1;
                int j = iAtopSentence;
                int k = i-i;
                {
                    wchar_t wc = scan[j];
                    if( iswdigit( wc )
                    || wc == ':'
                    || wc == '.'
                    || wc == '$'
                    || wc == '('
                    || wc == ')'
                    || wc == '['
                    || wc == ']' )
                    {
                        // ok.
                    }
                    else
                    {
                        AllSuch = 0;
                        break;
                    }
                    if( ++j == k ) // This is a gamble on my perfect logic.
                        break;
                }
                if( AllSuch )
                {
                    #if DO_DEBUG_SENTENCES
                        Spew( L"Rejected period ending a Numeric prefix." );
                    #endif
                    goto RejectedPeriod;
                }
            }


            int iPastSentence = i + 1;

            // Now I must scan past the terminator to include certain punct.
            // I think accept quotes and close puntuation, until any spaces.
            // "Be more exhaustive." [1] Accept all chars until first space.
            // Also in this loop, look for first following lowercase letter,
            // to reject period. Also stop advance upon an uppercase letter.

            // Oh, look at this sentence that I just wrote:
            // ... to reject whitespace ( e.g., atop block ).
            // If I stop on space, I will miss lowercased "atop".

            for( ;; )
            {
                wchar_t wcp = scan[ iPastSentence ];

                // Initial test was pretty weak:
                // if( wcp <= ' ' ) // safe: stop on NULL
                //    break;
                // This loop is not quite like the loop above,
                // but it can still benefit from the same logic.
                // Now I need to gather in closures and spaced-out ellipses. . .
                if( wcp == NULL ) // safe: stop on NULL
                    break;
                if( wcp <= ' ' )
                {
                    // Test next char to keep any closures and . . . .
                    // Quotes will be trickier to assign left & right.
                    // How about dash?
                    wchar_t wcn = scan[ iPastSentence + 1 ];
                    if( wcn != '.'
                    &&  wcn != '?'
                    &&  wcn != '!'
                    &&  wcn != ':'
                    &&  wcn != ';'
                    &&  wcn != '-'
                    &&  wcn != '}'
                    &&  wcn != ')'
                    &&  wcn != ']' )
                        break;
                }

                if( iswupper( wcp ) )
                    break;

                if( iswlower( wcp ) )
                {
                    #if DO_DEBUG_SENTENCES
                        Spew( L"Rejected period for lowercase soon after it." );
                    #endif
                    goto RejectedPeriod;
                }
                iPastSentence ++;
            }

            // ...So after that scan, do one more for upper/lower case.
            int iBeyond = iPastSentence;
            for( ;; )
            {
                wchar_t wcp = scan[ iBeyond ];

                // space or newline does not stop this scan beyond.

                if( wcp == NULL ) // safe: stop on NULL
                    break;

                if( iswupper( wcp ) )
                    break;

                if( iswlower( wcp ) )
                {
                    #if DO_DEBUG_SENTENCES
                        Spew( L"Rejected period for lowercase further after it." );
                    #endif
                    goto RejectedPeriod;
                }
                iBeyond ++;
            }

            // Diminish effects of dash and brackets
            // by scaling other link symptoms higher.
            if( AlphaTokenCountInSentence > 0
            && AlphaTokenCountInSentence < 500
            && SemicolonCountInSentence < 20
            && CommaCountInSentence < 20
            && ColonCountInSentence < 20
            && LinkSymptomCountInSentence < 8 )
            {
                BlockMeritSum ++;
                SumBlockValue += ConsiderPotentialSentence( scan + iAtopSentence, scan + iPastSentence, AtopOffset + iAtopSentence );
            }
            else
            {
                BlockMeritSum --;
                RejectedPotentialSentence( scan + iAtopSentence, scan + iPastSentence, AtopOffset + iAtopSentence );
                #if DO_DEBUG_SENTENCES
                    SpewValue( L"AlphaTokenCountInSentence", AlphaTokenCountInSentence );
                    SpewValue( L"SemicolonCountInSentence", SemicolonCountInSentence );
                    SpewValue( L"CommaCountInSentence", CommaCountInSentence );
                    SpewValue( L"ColonCountInSentence", ColonCountInSentence );
                    SpewValue( L"LinkSymptomCountInSentence", LinkSymptomCountInSentence );
                #endif
            }

            // Simple advance of Atop and i -- just pass any whitespace.
            iAtopSentence = iPastSentence;
            for( ;; )
            {
                if( scan[ iAtopSentence ] == NULL // safe: stop on NULL
                ||  scan[ iAtopSentence ] > ' ' )
                    break;
                iAtopSentence ++;
            }
            wcPrior = 0;
            wcPrior2 = 0;
            wcPrior3 = 0;

            // Whether sequence was accepted or rejected,
            // I need to reset these per-sentence things:
            // Hey -- Two places or more!
            AlphaTokenCountInSentence = 0;
            LinkSymptomCountInSentence = 0;
            SemicolonCountInSentence = 0;
            CommaCountInSentence = 0;
            ColonCountInSentence = 0;

            i = iAtopSentence;
            continue; // having replaced normal end-of-loop advance
        }

    RejectedPeriod: ;

        wcPrior3 = wcPrior2;
        wcPrior2 = wcPrior;
        wcPrior = wc;
        i ++;
    }

    // I don't need a link UrlIndex, nor destination offset:
    // I could rank them, maybe after ranking all sentences.
    // Oh, I do not possess the offset in text! I could return QMetric.
    // Double Oh, The sentence offsets were within block. Pass me an N.


   // // Now add this block to the catalog, with its score.
   // // Logs convert the raw score sum to a manageable range.

   // // For zero in, produce zero out.
   // size_t QMetric = 0;
   // if( SumBlockValue > 0 )
   // {
   //     double LogSum = log10( SumBlockValue );

   //     // What is a reasonable range of logarithms?
   //     // If every word is unique, and lots of them,
   //     // like a dictionary file, it could be as big
   //     // as the log of word count, say, 5 of 100000.
   //     // If text has one word of 100000 counts, -5.

   //     // Adding a factor of 0-255 for letter values,
   //     // just raised potential top limit about +2.5.

   //     // To be generous, add 20, to range 15 to 25.
   //     // Then multiply by 10000 into max at 250000.

   //     double Qdouble = ( LogSum + 20.0 ) * 10000.0;
   //     // scary stuff. Limit range to 6 digits, +.
   //     if( Qdouble > 400000.0 )
   //         Qdouble = 400000.0;
   //     if( Qdouble < 0.0 )
   //         Qdouble = 0.0;

   //     // I expect the sum of novelty to increase linearly
   //     // with length, in equal rate of novel word usages.
   //     // If I took log of that, also take log of length.
   //     // Hmmm. IT still favors short sentences too much.
   //     // No, add more to LogLen until it seems balanced.

   //     double Length = past - atop + 1;
   //     double LogLen = log10( Length );
   //     LogLen += 3.0; // empirical adjustment, could study more.
   //     QMetric = ( size_t ) ( Qdouble / LogLen ); // never is / 0
   // }

    #if DO_DEBUG_BLOCK_NESS
    {
        // Long lines really tax my text processing ability.
        wchar_t wk[ 200 ]; // see 198 below
        int BMS = BlockMeritSum;
        if( BMS > 999 )
            BMS = 999;
        if( BMS < -99 )
            BMS = -99;
        BMS += 1000;
        wsprintf( wk, L"%4d ", BMS );
        wchar_t * into = wk + 5;
        wchar_t * from = atop;
        for( ;; )
        {
            if( *from == NULL )
                break;
            *into++ = *from++;
            if( into > wk + 198 )
                break;
        }
        *into = NULL;
        Spew( wk );
    }
    #endif

    // wchar_t * pBlurb = L"???";

    if( BlockMeritSum > 1 )
    {
        Significance = ENUM_SIGNIFY_VERY_GOOD;
    //     pBlurb = L"222";
    }
    else if( BlockMeritSum < 0 )
    {
        Significance = ENUM_SIGNIFY_ANATHEMA;
    //     pBlurb = L"---";
    }
    else if( BlockMeritSum == 1 )
    {
        Significance = ENUM_SIGNIFY_SLIGHT_GOOD;
    //     pBlurb = L"111";
    }
    else // BlockMeritSum == 0
    {
        Significance = ENUM_SIGNIFY_NULL_HINT;
    //     pBlurb = L"000";
    }

    CloseBlockInPipeline( ( size_t ) SumBlockValue, Significance );

    *past = SavedPast; // restore
    return SumBlockValue;
}

int CTxt::No_PrescanBlockShowsPatterns( wchar_t * atop, wchar_t * past )
{
    // I decided not to use this routine, but it is full of comments
    // as I develop my heuristics. Let the DivideBlockIntoSentences
    // routine, with its accurate rules, also be tasked with anchor
    // delimiter recognition; And let ConsiderPotentialSentence not
    // immediately add sentence to IDX, but just queue them into a
    // two-level pipeline: At one level of pipeline are blocks, up
    // to say, 3 blocks; Within them are sentences, 0 to unlimited;
    // After each block parse, and at end of page, Call new routine
    // to either save or discard items from the pipeline into IDXs.


    // Caller placed a NULL at past.
    // Return true if repetitive punctuation patterns are detected.
    // This will eliminate blocks that are links ( having [ | ] ).
    // This will eliminate lists of things, and bibliographies.
    // For example:
    //
    // Rose Window - A circular window, with mullions and traceries generally radiating
    // from the centre, and filled with stained glasses Rosea - The official catalogue
    // of the Roman Curia mentioned formerly a titular see of Rosea in Syria Roseau -
    // Diocese; suffragan of Port of Spain, Trinidad, B.W.I Rosecrans, William Starke -
    // American soldier and politician Roseline, Saint - A Carthusian prioress. She
    // died in 1329 Rosh Hashanah - The first day of Tishri ( October ), the seventh
    // month of the Hebrew year Rosicrucians - The original appelation of the alleged
    // members of the occult-cabalistic-theosophic 'Rosicrucian Brotherhood', described
    // in the pamphlet 'Fama Fraternitatis R.C.' Roskilde, Ancient See of, in Denmark -
    // Suffragan to Hamburg, about 991-1104, to Lund, 1104-1536 Roskov�nyi, August -
    // Bishop of Neutra in Hungary, doctor of philosophy and theology ( 1807-1892 )
    //
    // Benjamin Ray,* James Mitchel,
    // Sherman Wattles,* Oliver Jinks,
    // Witter Johnson,* William McCalpin,
    // John Moore, Samuel Lyon,
    // Medad Hunt, William Linsley,
    // Nathaniel Wattles,* Anson Cary,
    // David White, Silas Hutchinson,
    // John McNeil, William Hanna,
    //
    //     Yet that is so much like this real sentence:
    //
    // Eighty-nine were farmers; forty-four, laborers; nine, mechanics; six each,
    // blacksmiths, students and carpenters; five, clerks; four, painters; three each,
    // butchers, shoemakers and teachers; two each, millers, masons cabinet makers,
    // waiters and barbers; and the stone-cutters, physicians, tailors, bartenders,
    // news-dealers, clothiers, harness makers, jewelers, photographers, printers,
    // saloon keepers, gilders, soldiers and sailors, had each one representative.

    // I think to parse all punctuation run patterns, ignoring newlines,
    // making single whitespace runs, and remembering any prior and next
    // alphas as U ( upper ) or l ( lower ), then add them all to CSOLs, and
    // see if the total count exceeds some proportion of size/word count.

    // A pattern containing "l[stuff].[stuff]u" or "l, l" is meritorious.

    // Here is a list from some pages.
    // 126803 l, l -- GOOD! -- Or at least, ignore this common one.
    //  50408 l. U -- GOOD! -- This is probably a sentence break.
    //  35220 l, U -- allow -- may be a list; or just a name next.
    //  20975 l | U - BAD -- This is one side effect of anchors.
    //  16854 U, U -- Bad? -- Aha, PROBABLY, ALL-CAPS - TITLES.
    //  12109 l; l -- allow -- like comma above.
    //  12082 U | U - BAD -- This is one side effect of anchors.
    //   7612 ), U -- allow -- like a plain comma.
    //   5809 l: U -- allow -- like comma above.
    //   4432 l "l -- allow -- an un-comma'ed quote
    //   4291 l ( l -- allow -- may be in sentences
    //   4138 l - U - BAD? -- Symptom of a list, if too abundant.
    //   3981 l ( U -- allow -- may be in sentences
    //   3690 U. U -- allow -- Initials,
    //   3435 l: l -- allow
    //   2930 l? U -- allow, and ! too
    //   2688 l. l -- allow
    //   2269 l" l -- allow
    //   2192 l ) l -- allow
    //   1865 ) , U -- allow
    //   1711 l." U -- allow
    //   1651 ). U -- allow
    //   1642 l - l - BAD -- Symptom of a list, if too abundant.
    //   1441 l \777l -- allow -- Perhaps other quote chars? Also a bullet.
    //   1411 l & U -- allow
    //   1378 l- l -- allow
    //   1355 l; U -- allow
    //   1334 l: "U -- allow
    //   1300 l! U -- allow
    //   1262 l "U -- allow
    //   1226 l-- ) U -- allow -- smilies
    //   1191 l ( --l -- allow
    //   1127 l."[ --HUH? -- I do see real text using brackets [for comments].
    //   1101 U; U -- allow
    //   1033 l, "U -- allow
    //    995 l," l -- allow
    //    964 l] U -- BAD? -- Another side effect of anchors [or comments].
    //    945 - ) U -- allow -- smilie
    //    942 l | l - BAD -- This is one side effect of anchors.
    //    933 l ) U -- allow
    //    904 l ( - ) U
    //    851 l' l -- allow
    //    848 l ); l -- allow
    //  ...and many more...

    // Oh! I forgot about cleartext URLs, very punctuation laden.
    // Perhaps only do punctuation runs that include whitespace?

    // I definitely need that final period of block, for many pages
    // separate all their good sentences, each into one block!

    // Here are various examples from web pages:
    //
    //         I need to exclude pure link blocks:
    //
    // [FORM]
    //
    // [deutsch] | [english]
    //
    // 0-9 | A-D | E-K | L-N | O-S | T-Z
    //
    // <<< | >>> |
    //
    // [Homepage] | [Noord Nieuws] | [Noord Sport] | [Muziek] | [Reclame] | [Uitzending
    // gemist] | [Live] | [Radio] | [Televisie] | [Teletekst] | [Omroep]
    //
    // Woensdag 21 november 2007 [Radio Noord live] | [TV Noord live] | [Chat op RTV
    // Noord]
    //
    // inloggen | registreren | contact
    //
    //
    //         Especially, long blocks of blocks:
    //
    //
    // [dot]
    //
    // about | search | site map | editors | donate | contact | help
    //
    // Announcements
    //
    // Job Guide
    //
    // Reviews
    //
    // Discussion Networks
    //
    // [dot]
    //
    //
    //         And whole pages of signage, although
    //         occasional sentences appear in them:
    //
    //
    // The Saving Remnant: Religion and the Settling of New England by Cedric B. Cowing
    // on page 279
    //
    // See all 9 books citing this book
    //
    // Customers viewing this page may be interested in these Sponsored Links ( What's
    // this? )
    //
    // Get 10 Ringtones Now
    //
    // Flycell.com Pick your songs, enter your number. No credit card needed. 90,000
    // tones
    //
    //
    //
    //         Need to lose lists like these:
    //
    //
    // Techno Trance Massive Live@ Collection
    //
    // TechSmith Camtasia Studio 3 0 1 Keygen Latest
    //
    // Teen girl pees her panties
    //
    // Teenage Mutant Ninja Turtles Collection
    //
    // Tema Cavaleiros do Zodiaco mp3
    //
    //         ...and...
    //
    // Ireland Travel Links
    //
    // http://goireland.about.com/od/dublinandleinster/p/carlow.htm |
    // http://goireland.about.com/od/dublinandleinster/p/leinster.htm |
    // http://goireland.about.com/od/dublinandleinster/p/dublincounty.htm |
    // http://goireland.about.com/od/dublinandleinster/p/kildare.htm
    //
    //
    //         Need to keep prefatory headers:
    //
    //
    // The Seven Seals Examined ( Revelation 4-6 )
    //
    // Part 2
    //
    // by Larry W. Wilson
    //
    // Seal #1 – White Horse – 1798 – The Salvation of Jesus Proclaimed
    //
    // "I watched as the Lamb opened the first of the seven seals. Then I heard one of
    // the four living creatures say in a voice like thunder, ‘Come!’
    //
    //
    //         Another example:
    //
    // Getting Started with the Book of Revelation
    //
    // by Larry W. Wilson
    //
    // "For the revelation awaits an appointed time; it speaks of the end and will not
    // prove false. Though it linger, wait for it; it will certainly come and will not
    // delay." – Habakkuk 2:3
    //
    // The Big Picture
    //
    // In the simplest of terms, the book of Daniel reveals two great truths for the
    // final generation.
    //
    //
    //
    //         Here is typical separation of blog posts:
    //
    //
    // ...I do know. thank you very much. Right I'm off, going away for a few days,
    // Where?? R-OTTER-dam of course... xxx
    //
    // Posted on SQUIDGY THE OTTER at 17:51
    //
    // COUNCIL TAX?? A DEVIL IN DISGUISE???
    //
    // Posted: Wednesday, 03 October 2007
    //
    // 17 comments
    //
    // Good morning, hope you slept well. Today's subject could be a touchy issue....
    //
    //
    //         Here is a toughie: Occasional sentences
    //         in huge runs of pure signage... Exclude!
    //
    //
    // $2,061.84 at 1 PC Network inc
    //
    // [See It] | at 1 PC Network inc
    //
    // 10"x7" Slide Seal Bags
    //
    // Meets FDA requirements for storage of food, Handy slider provides a secure seal
    // every time, Added width at the opening for easy filling ... Read More
    //
    // $35.75 at Gempler's
    //
    //
    //         I see the solution as pipelining the
    //         signage block indexes, and whenever
    //         a significant paragraph occurs, keep
    //         about the last 3 of signage; and keep
    //         every significant paragraph.
    //         But some "minor" sentences as above
    //         will not shift the mode from signage
    //         to significant. In fact, just call it
    //         signage, and require a later paragraph
    //         to output it; Or, keep an integration
    //         of running signage/significant values,
    //         and let it gate off/on such minor text.


    // My parse is screwed up: To many states held in just one bool.

    // I like the pre-CSol-sort and count feature; Use highest count
    // aside from sentence types as criterion of being too patterned.

    // I think to drop the U/l aspect, and categorize each pattern
    // as possibly in sentences, having [--- . , ' " ( ) : ; ? ! ---]
    // or probably in signage, having [--- [ > | ] ---].
    // Brackets are ambiguous, may be in anchors, or in sentences.
    // Various dashes are ambiguous, may be in lists, or in sentences.

    // But if I made all the rules in advance, and kept the analysis
    // immediate during parse, I could make more complex rules, like
    // to recognize . after initials -- G. Scheper -- as no sentence.

    // Maybe I should work right in the original sentence recognizer,
    // adding new rules about [ | ] etc., but then I would have to
    // cache, and not add sentences to IDX until integrate decisions.

    // I am abandoning this approach, not fixed....

// afu     wchar_t * scan = atop;
// afu     wchar_t * stop = past;
// afu     wchar_t WcPrior = 0;
// afu
// afu     int InAlnum = -1; // to trigger either 0 or 1 starting work at top
// afu     int InSpace = 0;
// afu     int TokenCount = 0;
// afu
// afu     wchar_t Accumulator[20]; // must CYA when filling...
// afu     int FillAccu = 0;
// afu     CSol CSolRuns( CSOL_SCALAR );
// afu
// afu     for( ;; )
// afu     {
// afu         if( scan == past )
// afu         {
// afu             // Just let any final puntuation run or token slide.
// afu             // However, that might be the only one, ending a good sentence!
// afu             // So I really should process them...
// afu
// afu             break;
// afu         }
// afu         wchar_t wc = *scan++;
// afu         if( iswalnum( wc ) )
// afu         {
// afu             InSpace = 0;
// afu             // Do in/out of alnum as run delimiters.
// afu             if( InAlnum != 1 )
// afu             {
// afu                 InAlnum = 1;
// afu                 // Starting a new alnum token.
// afu                 TokenCount ++;
// afu
// afu                 if( iswlower( wc ) )
// afu                     Accumulator[ FillAccu ++ ] = 'l'; // for lowercase
// afu                 else if( iswupper( wc ) )
// afu                     Accumulator[ FillAccu ++ ] = 'U'; // for uppercase
// afu
// afu                 // Process run of punctuation/spaces.
// afu                 // Ignore single ( 1 ) spaces and single puncts ( +2 alpha ).
// afu                 // Ignore excessive length patterns.
// afu                 if( FillAccu > 3
// afu                 && FillAccu < 10 )
// afu                 {
// afu                     Accumulator[ FillAccu ] = NULL;
// afu
// afu                     // #if DO_DEBUG_PUNCT_RUNS
// afu                     //    Spew( Accumulator );
// afu                     // #endif
// afu
// afu                     size_t index = CSolRuns.AddKey( Accumulator );
// afu                     #if DO_DEBUG_ADDFIND
// afu                         if( index <= 1 )
// afu                             { Spew( L"AddFind 1 at ctxt 2672" ); }
// afu                     #endif
// afu                     CSolRuns.IncrementUserValue( index );
// afu                 }
// afu             }
// afu         }
// afu         else
// afu         {
// afu             if( InAlnum != 0 )
// afu             {
// afu                 InAlnum = 0;
// afu                 // Start a new run of punctuation/spaces.
// afu                 FillAccu = 0;
// afu                 if( iswlower( WcPrior ) )
// afu                     Accumulator[ FillAccu ++ ] = 'l'; // for lowercase
// afu                 else if( iswupper( WcPrior ) )
// afu                     Accumulator[ FillAccu ++ ] = 'U'; // for uppercase
// afu                 // else may be null, at top of block.
// afu             }
// afu             // Collect runs of punctuation ( and space ) to sort.
// afu             if( iswspace( wc ) )
// afu             {
// afu                 // Collapse all whitespace to one space.
// afu                 if( ! InSpace )
// afu                 {
// afu                     InSpace = 1;
// afu                     // Add a single space.
// afu                     // Ignore a first space? No.
// afu                     if( FillAccu < 10 )
// afu                         Accumulator[ FillAccu ++ ] = ' ';
// afu                 }
// afu             }
// afu             else
// afu             {
// afu                 InSpace = 0;
// afu                 // Collect this ( s/b punctuation ) character.
// afu                 if( FillAccu < 10 )
// afu                     Accumulator[ FillAccu ++ ] = wc;
// afu             }
// afu         }
// afu         WcPrior = wc;
// afu     }
// afu
// afu     // Having filled CSol with patterned runs, analyze it.
// afu
// afu     // Add with ZERO count the couple I would allow,
// afu     // or try to find them and get out counts first,
// afu     // or just recognize them while running list,
// afu     // perhaps using some state-machine parser.
// afu
// afu     // Take a few patterns off the top from known list.
// afu     int TotalCount = 0;
// afu     int GoodCount = 0;
// afu     int BadCount = 0;
// afu
// afu     CoIt * pMalVector = CSolRuns.GetSortedVector( CSOL_FORWARD );
// afu     if( pMalVector != NULL )
// afu     {
// afu         size_t take = 0;
// afu         for( ;; )
// afu         {
// afu             CoIt * pCoIt = pMalVector + take++;
// afu             if( pCoIt->IsSentinel )
// afu                 break;
// afu             wchar_t * FullKey = CoItFullKey( pCoIt );
// afu             int count = pCoIt->User.Value;
// afu             TotalCount += count;
// afu
// afu             // First look for those signage link indications.
// afu             int FoundPipeEtc = 0;
// afu             {
// afu                 wchar_t * scan = FullKey;
// afu                 for( ;; )
// afu                 {
// afu                     wchar_t wc = *scan++;
// afu                     if( wc == NULL )
// afu                         break;
// afu                     if( wc == '|'
// afu                     || wc == ']'
// afu                     || wc == '[' )
// afu                     {
// afu                         FoundPipeEtc = 1;
// afu                         break;
// afu                     }
// afu                 }
// afu             }
// afu
// afu             if( FoundPipeEtc )
// afu             {
// afu                 BadCount += count;
// afu             }
// afu             else
// afu             {
// afu                 if( wcscmp( FullKey, L"l, l" ) )
// afu                     TotalCount -= count; // just un-count this frequent one.
// afu                 else if( wcscmp( FullKey, L"l. U" ) )
// afu                     GoodCount += count;
// afu                 else if( wcscmp( FullKey, L"l? U" ) )
// afu                     GoodCount += count;
// afu                 else if( wcscmp( FullKey, L"l! U" ) )
// afu                     GoodCount += count;
// afu                 else if( wcscmp( FullKey, L"l - U" ) ) // frequent list style
// afu                     BadCount += count;
// afu                 else
// afu                 {
// afu                     // Show me the rest for further study.
// afu                     #if DO_DEBUG_PUNCT_RUNS
// afu                         SpewValue( FullKey, count );
// afu                     #endif
// afu                 }
// afu             }
// afu
// afu             MyFree( 3931, zx, FullKey );
// afu             FullKey = NULL;
// afu         }
// afu         MyFree( 3934, UNPREDICTABLE, pMalVector );
// afu         pMalVector = NULL;
// afu     }
// afu
// afu     // Now by what heuristics shall I ignore blocks?
// afu     // My inputs are these counts:
// afu     // TokenCount
// afu     // TotalCount
// afu     // GoodCount
// afu     // BadCount
// afu
// afu
// afu     #if DO_DEBUG_PUNCT_RUNS
// afu         // I need to limit the debug text output line length,
// afu         // and fix the added NULL before every return below.
// afu         int nEOS = past - atop;
// afu         wchar_t SaveEos = atop[nEOS];
// afu         atop[nEOS] = NULL;
// afu     #endif
// afu
// afu     if( TotalCount < 1 )
// afu     {
// afu         // No punctuation at all cannot have sentences.
// afu         #if DO_DEBUG_PUNCT_RUNS
// afu             SpewTwo( L"ng, t<1", atop );
// afu             atop[nEOS] = SaveEos;
// afu         #endif
// afu         return 1;
// afu     }
// afu     if( TokenCount < 3 )
// afu     {
// afu         // Under three words is no sentence.
// afu         #if DO_DEBUG_PUNCT_RUNS
// afu             SpewTwo( L"ng, w<3", atop );
// afu             atop[nEOS] = SaveEos;
// afu         #endif
// afu         return 1;
// afu     }
// afu     if( GoodCount > BadCount )
// afu     {
// afu         // More possible sentences than bad markers.
// afu         #if DO_DEBUG_PUNCT_RUNS
// afu             SpewTwo( L"ok, g>b", atop );
// afu             atop[nEOS] = SaveEos;
// afu         #endif
// afu         return 0;
// afu     }
// afu     if( BadCount > GoodCount + 1 )
// afu     {
// afu         // More bad than good, above some min count.
// afu         #if DO_DEBUG_PUNCT_RUNS
// afu             SpewTwo( L"ng, b>g", atop );
// afu             atop[nEOS] = SaveEos;
// afu         #endif
// afu         return 1;
// afu     }
// afu     // Now ratios need a certain minimum count.
// afu     if( TotalCount > 10 )
// afu     {
// afu         // too many bad kind
// afu         if( BadCount > GoodCount )
// afu         {
// afu             #if DO_DEBUG_PUNCT_RUNS
// afu                 SpewTwo( L"ng, b>>", atop );
// afu                 atop[nEOS] = SaveEos;
// afu             #endif
// afu             return 1;
// afu         }
// afu         // too many bad kind
// afu         if( BadCount * 4 > TokenCount )
// afu         {
// afu             #if DO_DEBUG_PUNCT_RUNS
// afu                 SpewTwo( L"ng, b>w/4", atop );
// afu                 atop[nEOS] = SaveEos;
// afu             #endif
// afu             return 1;
// afu         }
// afu         // too many in general
// afu         if( ( TotalCount - GoodCount ) * 4 > TokenCount )
// afu         {
// afu             #if DO_DEBUG_PUNCT_RUNS
// afu                 SpewTwo( L"ng, x>w/4", atop );
// afu                 atop[nEOS] = SaveEos;
// afu             #endif
// afu             return 1;
// afu         }
// afu     }
// afu     // default is okay.
// afu     #if DO_DEBUG_PUNCT_RUNS
// afu         SpewTwo( L"ok, def", atop );
// afu         atop[nEOS] = SaveEos;
// afu     #endif
// afu
    return 0;
}

void CTxt::RejectedPotentialSentence( wchar_t * atopPassed, wchar_t * past, int AtopOffset )
{
    // This is the anti-type of ConsiderPotentialSentence,
    // Just called to see the rejected sentences of block.

    // Note that these fragments cannot make it to sentences,
    // so they will never be appearing in the pipeline later.

    #if DO_DEBUG_SENTENCES
        SpewTwo( L"Rejected", atopPassed );
    #endif

    #if DO_DEBUG_VALUATION
    // Draw me up first and last 30 chars, and an -X- mark in left border.
    {
        wchar_t wk[300];
        wchar_t * into = wk;
        *into++ = '-';
        *into++ = 'X';
        *into++ = '-';
        *into++ = ' ';

        wchar_t * from = atopPassed;
        wchar_t * stop = past;
        if( stop > from + 30 )
            stop = from + 30;
        for( ;; )
        {
            if( from == stop )
                break;
            wchar_t wc = *from++;
            if( wc > '~' )
                *into++ = '$';
            else if( wc < ' ' )
                *into++ = '@';
            else
                *into++ = wc;
        }

        stop = past;
        if( from < stop - 30 )
        {
            from = stop - 30;
            *into++ = '=';
            *into++ = '=';
            *into++ = '=';
            *into++ = '.';
            *into++ = '.';
            *into++ = '.';
            *into++ = '=';
            *into++ = '=';
            *into++ = '=';
        }
        for( ;; )
        {
            if( from == stop )
                break;
            wchar_t wc = *from++;
            if( wc > '~' )
                *into++ = '?';
            else if( wc < ' ' )
                *into++ = '!';
            else
                *into++ = wc;
        }
        *into++ = '<';
        *into++ = '<';
        *into++ = '=';
        *into++ = '=';
        *into = NULL;
        Spew( wk );
    }
    #endif

}

double CTxt::ConsiderPotentialSentence( wchar_t * atopPassed, wchar_t * past, int AtopOffset )
{
    // Consider is a mis-nomer. My caller has already decided
    // that passed char sequence meets good sentence criteria.

    #if DO_DEBUG_VALUATION
    // Draw me up first and last 30 chars, and an -O- mark in left border.
    {
        wchar_t wk[300];
        wchar_t * into = wk;
        *into++ = '-';
        *into++ = 'O';
        *into++ = '-';
        *into++ = ' ';

        wchar_t * from = atopPassed;
        wchar_t * stop = past;
        if( stop > from + 30 )
            stop = from + 30;
        for( ;; )
        {
            if( from == stop )
                break;
            wchar_t wc = *from++;
            if( wc > '~' )
                *into++ = '$';
            else if( wc < ' ' )
                *into++ = '@';
            else
                *into++ = wc;
        }

        stop = past;
        if( from < stop - 30 )
        {
            from = stop - 30;
            *into++ = '=';
            *into++ = '=';
            *into++ = '=';
            *into++ = '.';
            *into++ = '.';
            *into++ = '.';
            *into++ = '=';
            *into++ = '=';
            *into++ = '=';
        }
        for( ;; )
        {
            if( from == stop )
                break;
            wchar_t wc = *from++;
            if( wc > '~' )
                *into++ = '?';
            else if( wc < ' ' )
                *into++ = '!';
            else
                *into++ = wc;
        }
        *into++ = '<';
        *into++ = '<';
        *into++ = '=';
        *into++ = '=';
        *into = NULL;
        Spew( wk );
    }
    #endif


    wchar_t * scan = atopPassed; // until reach past ( may not be NULL )

    // Since I will re-parse and estimate a "value" for words,
    // the SimpleWordScan must have already run to list words.

    // While scanning this sentence, I must have an identical
    // word-boundary detection that I used in SimpleWordScan.
    // If not, at least tolerate that word look-ups may fail.

    // I thought to go re-write that. It appears too complex.
    // Maybe it was just right. I will break any hyphenation.

    // This loop I cloned requires a NULL terminator at end.
    wchar_t SavedPast = *past;
    *past = NULL;

    #if DO_DEBUG_SENTENCES
        SpewTwo( L"Sentence", atopPassed );
    #endif

    double SumSentenceValue = 0.0;

	// 2015-02-13 I want to penalize sentences that have
	// many very Foreign Chars > 255 or isPuncts (code).

	int nForeignIspunct = 0;

	// This outer loop contains two phases of inner loop.
	for( ;; )
    {
        // Do phase-one inner loop to scan for any alnum.
        for( ;; )
        {
            if( scan == past ) // determinate if all others ++ safely
                break;
            if( iswalnum( *scan ) )
                break;
			// 2015-02-13 Inserted into this phase looking for start of word, count puncts:
            if( iswpunct( *scan ) )
                nForeignIspunct++;
            scan++;
        }

        if( scan == past )
            break;

        wchar_t * atopWord; // declaration part... then assignment:
        atopWord = scan; // remember position of first alnum

        int SawAnyAlpha = 0;
        int MustSubParse = 0;

        // Do phase-two inner loop to find end of various.

        for( ;; )
        {
            // Incoming logic above for first character,
            // and rest of loop, qualified char at scan.
            // So incorporate char before a test to end.
            // I can lowercase in buffer, need not copy.

            // Incorporating the current char means to lowercase any alpha.

            wchar_t wc = *scan;

            // Final NULL will be treated as an unsuitable character:

            if( iswalpha( wc ) )
            {
                SawAnyAlpha = 1;

                if( iswupper( wc ) )
                    wc = towlower( wc ); // having setlocale to .1252

                *scan = wc;
                // Meanwhile, this alpha is also a suitable character.

				// 2015-02-13 Inserted into this phase processing chars of word, count very foreign:
				if( wc > 255 )
					nForeignIspunct++;

            }
            else if( iswdigit( wc ) )
            {
                // Or this digit is a suitable character.
            }
            else if( wc == SQT
            && scan > atopPassed // CYA for [-1]
            && iswalnum( scan[-1] )
            && iswalnum( scan[1] ) && ! isvowel( scan[1] ) )
            {
                // Or this apostraphe between alphas is suitable.

                // Apostrophe includes U+0027 ( ' ) APOSTROPHE
                // and U+2019 ( � ) RIGHT SINGLE QUOTATION MARK

                // I no longer need to rule out and fix the U+2019
                // here in ctxt.cpp, as I fixed it in the charsets:
                // wc = SQT; // fix in case U+2019

                // According to the UNICODE.ORG suggestion, don't
                // accept an apostraphe followed by a vowel which
                // takes care of l'en... del'a.... of french, etc.

                MustSubParse = 1;
            }
            else if( wc == '-'
            && scan > atopPassed // CYA for [-1]
            && iswalnum( scan[-1] )
            && iswalnum( scan[1] ) )
            {
                // Or various hyphens between alphas is suitable.

                // Unicode.org says: Hyphens include
                // U+002D HYPHEN-MINUS,
                // U+2010 HYPHEN,
                // possibly also U+058A ( ? ) ARMENIAN HYPHEN, -- ignore
                // and U+30A0 KATAKANA-HIRAGANA DOUBLE HYPHEN. -- ignore

                // U+2010 HYPHEN,
                // U+2011 NON-BREAKING HYPHEN,
                // U+2012 FIGURE DASH,
                // U+2013 EN DASH,
                // U+2014 EM DASH,
                // U+2015 HORIZONTAL BAR ( quotation dash ), and
                // U+2212 MINUS SIGN
                // U+002D HYPHEN-MINUS ( '-' )
                // -- http://czyborra.com/unicode/characters.html

                // I no longer need to rule out and fix U+2010 etc
                // here in ctxt.cpp, as I fixed it in the charsets.
                // wc = '-'; // fix in case not simple hyphen

                MustSubParse = 1;
            }
            else if(
                ( wc == ':'
                || wc == 0x00b7
                || wc == 0x05f3
                || wc == 0x05f4
                || wc == 0x2027 )
            && scan > atopPassed // CYA for [-1]
            && iswalnum( scan[-1] )
            && iswalnum( scan[1] ) )
            {
                // Or various "midletters" besides apostraphe, hyphen.

                // MidLetter Any of the following:
                // U+00B7 MIDDLE DOT
                // U+05F3 HEBREW PUNCTUATION GERESH
                // U+05F4 HEBREW PUNCTUATION GERSHAYIM
                // U+2027 HYPHENATION POINT
                // U+003A COLON ( used in Swedish )

                MustSubParse = 1;

				// 2015-02-13 Inserted into this phase 2 at ':' during word, to count 1 punct:
                nForeignIspunct++;
            }
            else
            {
                // scan is sitting on the first unsuitable character.
                // That means phase-two inner loop will break now.
                // Having found first unsuitable, I can process word.

                // Eliminate purely numeric fields, dates 02-80-2007.
                // Aww, let em ride... if( SawAnyAlpha )...

                wchar_t saved = *scan;
                *scan = NULL;

                // I now have one word, a sz starting at atopWord.
                // This loop will not: if( MustSubParse )...

                // Now I might use frequency in CSolAllWords method.
                // As said in CFwd RankItem: value each word by 1/N
                // occurrences if found, or 0 if not found, summing
                // those reciprocals of count into a float across
                // all words. Take log of that Q, offset to positive.

                // I did lowercase in place as I scanned data.

                // Remove the minimum length word test here:
                // But I may need to keep the non-empty test.
                if( scan - atopWord > 0
                && scan - atopWord < MAX_LEGITIMATE_WORD_LENGTH )
                {
                    // Take the word count as the sum of these two:
                    // CSolAllWords
                    // - CSolAllWords does NOT get salted at start-up. Counts only web pages.
                    // - A prior call to pTxtParser->AddWordsToOtherLists included this page.
                    // CSolCommonWords
                    // - CSolCommonWords gets salted at start-up.
                    // - CSolCommonWords does NOT currently get new words added from web pages.

                    size_t Value1 = 0;
                    size_t Value2 = 0;

                    size_t ValShl24 = 0;
                    // not needed here... size_t AllGidShl17 = 0;
                    size_t CountInAll = 0;
                    size_t CountInComn = 0;

                    // First, CSolAllWords

                    size_t index1 = CSolAllWords.Find( atopWord );
                    #if DO_DEBUG_ADDFIND
                        if( index1 == 1 )
                            { Spew( L"AddFind 1 at ctxt 967" ); }
                    #endif
                    if( index1 > 1 )
                    {
                        Value1 = CSolAllWords.GetUserValue( index1 );
                        ValShl24 = Value1 & MASK_WORD_VALUE_SHL24;
                        // not needed here... AllGidShl17 = Value1 & LGID_SHL17_MASK;
                        CountInAll = Value1 & MASK_17BIT_WORD_COUNT;
                    }

                    // Now for CSolCommonWords

                    size_t index2 = CSolCommonWords.Find( atopWord );
                    #if DO_DEBUG_ADDFIND
                        if( index2 == 1 )
                            { Spew( L"AddFind 1 at ctxt 967" ); }
                    #endif
                    if( index2 > 1 )
                    {
                        Value2 = CSolCommonWords.GetUserValue( index2 );
                        CountInComn = Value2 & MASK_17BIT_WORD_COUNT;
                    }


                    size_t WordValuation = ValShl24 >> 24;
                    WordValuation *= 100; // big counts were washing out values

                    size_t SafeDivisor = 1 + CountInComn + CountInAll;

                    {
                        // 1/N is the good novelty measure.
                        // just multiply that by the value.
                        // Hence, just one double division.
                        double WordValue = ( double ) WordValuation / ( double ) SafeDivisor;
                        SumSentenceValue += WordValue;
                    }

                }
                *scan = saved; // restore

                break; // ending phase-two inner loop
            }
            // if here, phase-two inner loop continues
            scan ++;

            // Do not apply the test here: if( scan == past ) break;
            // If that happens, *scan == NULL which ends word above.
        }
        if( scan == past )
            break;
    }
    *past = SavedPast;

   // // Now add this sentence to the catalog, with its score.
   // // Logs convert the raw score sum to a manageable range.

   // // For zero in, produce zero out.
   // size_t QMetric = 0;
   // if( SumSentenceValue > 0 )
   // {
   //     double LogSum = log10( SumSentenceValue );

   //     // What is a reasonable range of logarithms?
   //     // If every word is unique, and lots of them,
   //     // like a dictionary file, it could be as big
   //     // as the log of word count, say, 5 of 100000.
   //     // If text has one word of 100000 counts, -5.

   //     // Adding a factor of 0-255 for letter values,
   //     // just raised potential top limit about +2.5.

   //     // To be generous, add 20, to range 15 to 25.
   //     // Then multiply by 10000 into max at 250000.

   //     double Qdouble = ( LogSum + 20.0 ) * 10000.0;
   //     // scary stuff. Limit range to 6 digits, +.
   //     if( Qdouble > 400000.0 )
   //         Qdouble = 400000.0;
   //     if( Qdouble < 0.0 )
   //         Qdouble = 0.0;

   //     // I expect the sum of novelty to increase linearly
   //     // with length, in equal rate of novel word usages.
   //     // If I took log of that, also take log of length.
   //     // Hmmm. IT still favors short sentences too much.
   //     // No, add more to LogLen until it seems balanced.

   //     double Length = past - atopPassed + 1;
   //     double LogLen = log10( Length );
   //     LogLen += 2.0; // empirical adjustment, could study more.
   //     QMetric = ( size_t ) ( Qdouble / LogLen ); // never is / 0
   // }

    *past = SavedPast; // restore


	// 2015-02-13 I want to penalize sentences that have
	// many very Foreign Chars > 255 or isPuncts (code).

	// This line was not effective? Was it generated above, or below?
	// SumSentenceValue = SumSentenceValue * 5 / (5 + nForeignIspunct);
	// Yes, Above. Maybe I need to degrade them more harshly:
	SumSentenceValue = SumSentenceValue * 1.0 / (1.0 + nForeignIspunct);

    // I am replacing this immediate addition of sentence to IDX...
    // m_pOnePaper->pIdxSentences->AddIdx( AtopOffset, AtopOffset + past - atopPassed, ( size_t ) SumSentenceValue, 0 );
    // ...with this deferred, pipelined, potentially omitted, addition:
    // I can omit the 4th parameter, as this is not a clickable IDX.

    AddSentenceToPipeline( AtopOffset, AtopOffset + past - atopPassed, ( size_t ) SumSentenceValue );

    return SumSentenceValue;
}


void CTxt::OpenBlockInPipeline( size_t Atop, size_t Past )
{
    #if DO_DEBUG_VALUATION
        Spew( L"-------------------------" );
    #endif

    // I'm opening a new block. If pPiCurrentBlock is non-null,
    // push it into the pPiPipeLine[ 3 ], pushing those 3 back.
    // No. Close block will move current into pipeline if reqd.
    // So, in any event, pPiCurrentBlock should be NULL now.

    pPiCurrentBlock = new CPipeItem;
    pPiCurrentBlock->AtopBlock = Atop;
    pPiCurrentBlock->PastBlock = Past;
}



void CTxt::CloseBlockInPipeline( size_t Index, size_t Signify )
{
    // The Index here serves to hold the "score" of block.

    // Signify is input to an integrated save/discard decision, one of:
    // ENUM_SIGNIFY_VERY_GOOD
    // ENUM_SIGNIFY_SLIGHT_GOOD
    // ENUM_SIGNIFY_NULL_HINT
    // ENUM_SIGNIFY_ANATHEMA
    // I figure pipeline will need a bi-level form. Close a block.

    #if DO_DEBUG_VALUATION
        switch( Signify )
        {
        case ENUM_SIGNIFY_VERY_GOOD:
            Spew( L"------ENUM_SIGNIFY_VERY_GOOD" );
            break;
        case ENUM_SIGNIFY_SLIGHT_GOOD:
            Spew( L"------ENUM_SIGNIFY_SLIGHT_GOOD" );
            break;
        case ENUM_SIGNIFY_NULL_HINT:
            Spew( L"------ENUM_SIGNIFY_NULL_HINT" );
            break;
        case ENUM_SIGNIFY_ANATHEMA:
            Spew( L"------ENUM_SIGNIFY_ANATHEMA" );
            break;
        }
    #endif

    // I will be the one to add each block to IDX, conditionally.
    // Decision to add a block will also add all sentences to IDX.
    // This was just my shaping behavior:
    // For starters, just list any 'good' blocks in IDX.
    // if( Signify > ENUM_SIGNIFY_NULL_HINT )
    // {
    //     m_pOnePaper->pIdxTextBlocks->AddIdx( m_AtopBlockOffset, m_PastBlockOffset, Index, 0 );
    //
    //     // Start with a pipeline of just one block, no history.
    //     // Do not add, but hold all sentences during block.
    //     // If the block gets added, add its sentences too.
    //
    //     size_t Atop;
    //     size_t Past;
    //     size_t Index;
    //     size_t take = 0;
    //     for( ;; )
    //     {
    //         if( take == FillThreeThou )
    //             break;
    //         Atop = ThreeThousand[ take++ ];
    //         Past = ThreeThousand[ take++ ];
    //         Index = ThreeThousand[ take++ ];
    //         m_pOnePaper->pIdxSentences->AddIdx( Atop, Past, Index, 0 );
    //     }
    // }


    // So, the implementation plan:
    //
    // VERY GOOD: adds all in pipeline, and itself; Sets warrant to 3.
    //
    // SLIGHT GOOD: If warrant > 0, add it, and re-boost warrant to 3;
    // else, SLIGHT GOOD goes into pipeline.
    //
    // NULL HINT: If warrant > 0, add it, but decrement warrant;
    // else, NULL HINT goes into pipeline.
    //
    // ANATHEMA: flush pipeline; discard it; set warrant to 0.
    //

    // Pipeline: VeryGood/Anathema are never in the pipeline. The act
    // of pushing in a SlightGood/NullHint block pushes out any block
    // from the other end of pipe, so up to 3 such might be recovered.
    // VeryGood/Anathema also reduce the pipeline to holding no items.

    pPiCurrentBlock->IndexBlock = Index;


    // Getting close. Why didn't this Very Good kick out the Slight Good?
    // I can see a warrant ran out prior to it; It should be in pipeline.
    // Aha! This was in a new file... Need that fact in the spew.
    //
    // -------------------------
    // -X- [Contents Issue 44] | [Main Page]<<==
    // ------ENUM_SIGNIFY_NULL_HINT
    //      -- Worthy PipeItem --
    //      -- End PipeItem --
    // -------------------------
    // -X- </NOFRAME><<==
    // ------ENUM_SIGNIFY_NULL_HINT
    //      -- Worthy PipeItem --
    //      -- End PipeItem --
    // -------------------------
    // -X- [Refresh] | [Refresh]<<==
    // ------ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // -O- In Order To Continue to your p===...=== you please fill in your info.<<==
    // -X- ( The page will then load )<<==
    // ------ENUM_SIGNIFY_SLIGHT_GOOD
    // -------------------------
    // -X- [FORM]<<==
    // ------ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // -O- Privacy Policy *by submiting e===...===ffers you can't see elsewhere.<<==
    // ------ENUM_SIGNIFY_SLIGHT_GOOD
    // -------------------------
    // -X- [web tracker]<<==
    // ------ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // -O- Welcome to Sound of Grace & Piper's Notes!<<==
    // -O- The page you are seeking is no longer available.<<==
    // -X- If you are not automatically d===...===Sound of Grace | Piper's Notes<<==
    // ------ENUM_SIGNIFY_VERY_GOOD
    //      -- Worthy PipeItem --
    //    | welcome to sound of grace & piper's notes!<<==
    //    | the page you are seeking is no longer available.<<==
    //      -- End PipeItem --
    // -------------------------



    switch( Signify )
    {

    case ENUM_SIGNIFY_VERY_GOOD:
        // This case uses and flushes the pipeline before itself.

        if( pPiPipeLine[ 2 ] != NULL )
        {
            AddPipeItemToIdxes( pPiPipeLine[ 2 ] );
            delete pPiPipeLine[ 2 ];
            pPiPipeLine[ 2 ] = NULL;
        }
        if( pPiPipeLine[ 1 ] != NULL )
        {
            AddPipeItemToIdxes( pPiPipeLine[ 1 ] );
            delete pPiPipeLine[ 1 ];
            pPiPipeLine[ 1 ] = NULL;
        }
        if( pPiPipeLine[ 0 ] != NULL )
        {
            AddPipeItemToIdxes( pPiPipeLine[ 0 ] );
            delete pPiPipeLine[ 0 ];
            pPiPipeLine[ 0 ] = NULL;
        }

        AddPipeItemToIdxes( pPiCurrentBlock );
        delete pPiCurrentBlock;
        pPiCurrentBlock = NULL;

        WarrantCounter = 3;
        break;

    case ENUM_SIGNIFY_ANATHEMA:
        // This case ignores and flushes the pipeline before itself.

        if( pPiPipeLine[ 2 ] != NULL )
        {
            SpewPipeItemToIgnore( pPiPipeLine[ 2 ] );
            delete pPiPipeLine[ 2 ];
            pPiPipeLine[ 2 ] = NULL;
        }
        if( pPiPipeLine[ 1 ] != NULL )
        {
            SpewPipeItemToIgnore( pPiPipeLine[ 1 ] );
            delete pPiPipeLine[ 1 ];
            pPiPipeLine[ 1 ] = NULL;
        }
        if( pPiPipeLine[ 0 ] != NULL )
        {
            SpewPipeItemToIgnore( pPiPipeLine[ 0 ] );
            delete pPiPipeLine[ 0 ];
            pPiPipeLine[ 0 ] = NULL;
        }

        SpewPipeItemToIgnore( pPiCurrentBlock );
        delete pPiCurrentBlock;
        pPiCurrentBlock = NULL;

        WarrantCounter = 0;
        break;

    case ENUM_SIGNIFY_SLIGHT_GOOD:
        if( WarrantCounter > 0 )
        {
            // Sufficient warrant from above to keep this block.
            // Add this block directly. Pipeline is empty.
            // No, rather, the pipeline can have null hint items.
            // So repeat the code from very good case.

            if( pPiPipeLine[ 2 ] != NULL )
            {
                AddPipeItemToIdxes( pPiPipeLine[ 2 ] );
                delete pPiPipeLine[ 2 ];
                pPiPipeLine[ 2 ] = NULL;
            }
            if( pPiPipeLine[ 1 ] != NULL )
            {
                AddPipeItemToIdxes( pPiPipeLine[ 1 ] );
                delete pPiPipeLine[ 1 ];
                pPiPipeLine[ 1 ] = NULL;
            }
            if( pPiPipeLine[ 0 ] != NULL )
            {
                AddPipeItemToIdxes( pPiPipeLine[ 0 ] );
                delete pPiPipeLine[ 0 ];
                pPiPipeLine[ 0 ] = NULL;
            }

            AddPipeItemToIdxes( pPiCurrentBlock );
            delete pPiCurrentBlock;
            pPiCurrentBlock = NULL;

            // For slight good, re-boost the warrant.
            WarrantCounter = 3;
        }
        else
        {
            // No warrant from above to keep this block.
            // Push this block into pipeline.

            if( pPiPipeLine[ 2 ] != NULL )
            {
                // When I push a block out, it does not get added.
                SpewPipeItemToIgnore( pPiPipeLine[ 2 ] );
                delete pPiPipeLine[ 2 ];
            }
            pPiPipeLine[ 2 ] = pPiPipeLine[ 1 ];
            pPiPipeLine[ 1 ] = pPiPipeLine[ 0 ];
            pPiPipeLine[ 0 ] = pPiCurrentBlock;
            pPiCurrentBlock = NULL;

            // For slight good, leave zero warrant zero.
        }
        break;

    case ENUM_SIGNIFY_NULL_HINT:
        if( WarrantCounter > 0 )
        {
            // Sufficient warrant from above to keep this block.
            // Add this block directly. Pipeline is empty.
            // No, rather, the pipeline can have null hint items.
            // So repeat the code from very good case.

            if( pPiPipeLine[ 2 ] != NULL )
            {
                AddPipeItemToIdxes( pPiPipeLine[ 2 ] );
                delete pPiPipeLine[ 2 ];
                pPiPipeLine[ 2 ] = NULL;
            }
            if( pPiPipeLine[ 1 ] != NULL )
            {
                AddPipeItemToIdxes( pPiPipeLine[ 1 ] );
                delete pPiPipeLine[ 1 ];
                pPiPipeLine[ 1 ] = NULL;
            }
            if( pPiPipeLine[ 0 ] != NULL )
            {
                AddPipeItemToIdxes( pPiPipeLine[ 0 ] );
                delete pPiPipeLine[ 0 ];
                pPiPipeLine[ 0 ] = NULL;
            }

            AddPipeItemToIdxes( pPiCurrentBlock );
            delete pPiCurrentBlock;
            pPiCurrentBlock = NULL;

            // For null hint, decrement the warrant until zero.
            WarrantCounter --;
        }
        else
        {
            // No warrant from above to keep this block.
            // Push this block into pipeline.

            if( pPiPipeLine[ 2 ] != NULL )
            {
                // When I push a block out, it does not get added.
                SpewPipeItemToIgnore( pPiPipeLine[ 2 ] );
                delete pPiPipeLine[ 2 ];
            }
            pPiPipeLine[ 2 ] = pPiPipeLine[ 1 ];
            pPiPipeLine[ 1 ] = pPiPipeLine[ 0 ];
            pPiPipeLine[ 0 ] = pPiCurrentBlock;
            pPiCurrentBlock = NULL;

            // For null hint, leave zero warrant zero.
        }
        break;
    }

    // By any path above, when close block ends,
    // I must have left pPiCurrentBlock == NULL.
}


void CTxt::AddSentenceToPipeline( size_t Atop, size_t Past, size_t Index )
{
    // The Index here serves to hold the "score" of sentence.
    // There IS a valid pPiCurrentBlock!

    // After all the talk below, this is all I do:
    pPiCurrentBlock->AddSentence( Atop, Past, Index );


    // This was just my shaping behavior:
    // For now, just add it directly to the paper's index.
    // Next, instead, add it to pipeline within the block.
    // m_pOnePaper->pIdxSentences->AddIdx( Atop, Past, Index, 0 );
    // Start with a pipeline of just one block, no history.
    // Do not add, but hold all sentences during block.
    // If the block gets added, add its sentences too.
    // I need a realloc-extensible list of 3 size_t.
    // Later, several must pass through the pipeline.
    // For now, just use a fixed vector of 3000 size_t.
    //
    // m_BlockSentenceCount ++;
    //
    // if( FillThreeThou < 2990 )
    // {
    //     ThreeThousand[ FillThreeThou++ ] = Atop;
    //     ThreeThousand[ FillThreeThou++ ] = Past;
    //     ThreeThousand[ FillThreeThou++ ] = Index;
    // }

    // Here's the purpose and principles of the pipeline:
    // ( Granted, my valuations are a bit primitive yet. )

    // 1. A pipeline of 3 blocks recovers past blocks.
    // 2. A warrant counter vouches for future blocks.

    // If blocks are slight good, and blocks before and after
    // are null hint or anathema, the block will not be added:
    // This is by merit of passing in and out of the pipeline.
    // E.g.,

    // -------------------------
    // X [TSGTV Video]<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X [Area]<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // | [Mug Shot Mania: Say Cheese!]<<==
    // ENUM_SIGNIFY_SLIGHT_GOOD
    // -------------------------
    // X [Area]<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // | [Mug Shot Mania: Say Cheese!]<<==
    // ENUM_SIGNIFY_SLIGHT_GOOD
    // -------------------------
    // X [Area]<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X [Join The Mailing List]<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------


    // If blocks are slight/null, and blocks before or after
    // are very good, those blocks in pipeline will be added
    // ( often paragraph headers ), and a warrant counter gets
    // raised to vouch for some future blocks, as occasional
    // lines set off from text are needed to have continuity.
    //
    // Slight goods should re-boost a non-zero warrant count,
    // as some web pages write text in that style continually.
    // Null hints will not boost a decrementing warrant count.
    // E.g.,

    // -------------------------
    // X Romans 5:1, 9; Ephesians 2:8, 9<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X Paragraph VIII - of Sanctification<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // | We believe that every saved person is positionally===...===ed so that his state will conform to his standing.<<==
    // ENUM_SIGNIFY_SLIGHT_GOOD
    // -------------------------
    // | We believe that progressive sanctification involve===...===they in turn maintain fellowship with unbelievers.<<==
    // ENUM_SIGNIFY_SLIGHT_GOOD
    // -------------------------
    // X Romans 8:8; ; I Corinthians 6:11; II Corinthians 6===...===onians 3:14, 15; I Timothy 6:3-5; Philippians 3:21<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X Paragraph IX - of the Ministry and Spiritual Gifts<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // | We believe that God is Sovereign in the bestowment===...===sufficient for the perfection of the saints today.<<==
    // | We believe that speaking in tongues and the workin===...=== completed and their authority became established.<<==
    // ENUM_SIGNIFY_VERY_GOOD
    // -------------------------
    // X I Corinthians 13:8-13; Ephesians 4:7-12<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X Paragraph X - of the Church<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // | We believe that a local church is a congregation o===...=== and duties are clearly defined in the Scriptures.<<==
    // | We believe that the true mission of the church is ===...===ssing of Christ to all men as we have opportunity.<<==
    // | We hold that the local church has the absolute rig===...===olence, and the will of the local church is final.<<==
    // ENUM_SIGNIFY_VERY_GOOD
    // -------------------------
    // X Acts 2:41, 42; 15:13-18; 20:17-28; I Corinthians 1===...===:22, 23; 5:23, 24; Colossians 1:18; I Timothy 3:17<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X Paragraph XI - of the Ordinances<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // | We believe that there are two church ordinances: Baptism and the Lord's Supper.<<==
    // | Baptism is the immersion of a believer in water and is properly called believer's baptism.<<==
    // | It portrays in a beautiful and solemn way our fait===...===f death to sin and the resurrection to a new life.<<==
    // | Baptism is a pre-requisite to the privileges of church membership.<<==
    // | The Lord's Supper is the commemoration of our Lord===...===membership and always by careful self-examination.<<==
    // ENUM_SIGNIFY_VERY_GOOD
    // -------------------------

    // Anathema blocks will not even go into pipeline; They immediately
    // flush any lines in pipeline, and reduce warrant counter to zero.

    // -------------------------
    // | From Our Founder Let each member have patience, rooted in a religious trust in the Lord.<<==
    // | What he sows now in tears, he may some day reap in joy.<<==
    // | It may even be that he will not be granted the joy===...=== for him the harvest will seem impossibly distant.<<==
    // | But let him be convinced that what he has with his===...===ars the Lord Jesus Christ will reap in due season.<<==
    // X H. Lyman Stebbins 1968<<==
    // ENUM_SIGNIFY_VERY_GOOD
    // -------------------------
    // X CATHOLICS UNITED FOR THE FAITH 827 North Fourth St===...=== more | home | about | legal Designed & Powered by<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X ' );<<==
    // ENUM_SIGNIFY_ANATHEMA
    // -------------------------
    // X [Subscribe Today] [Manage your account] Manage you===...===home delivery rates | Subscribe to digital E-State<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // X [TheState.com]<<==
    // ENUM_SIGNIFY_NULL_HINT
    // -------------------------

    // So, the implementation plan:
    //
    // VERY GOOD: adds all in pipeline, and itself; Sets warrant to 3.
    //
    // SLIGHT GOOD: If warrant > 0, add it, and re-boost warrant to 3;
    // else, SLIGHT GOOD goes into pipeline.
    //
    // NULL HINT: If warrant > 0, add it, but decrement warrant;
    // else, NULL HINT goes into pipeline.
    //
    // ANATHEMA: flush pipeline; discard it; set warrant to 0.
    //

    // Pipeline: VeryGood/Anathema are never in the pipeline. The act
    // of pushing in a SlightGood/NullHint block pushes out any block
    // from the other end of pipe, so up to 3 such might be recovered.
    // VeryGood/Anathema also reduce the pipeline to holding no items.

    // So I need FOUR similar objects that can be pointed to in pipe:
    // The current block while accumulating it; Up to 3 prior blocks.
    // A class to new/delete seems best; Else a fifo of four structs.
    // It must have a growable member to hold all sentences of block;
    // Else a huge array. That is a really doltish way. Make a class!
    // A re-alloc of some fixed count, 10 lines, is not difficult.

}

void CTxt::SpewSentenceSafely( size_t Atop, size_t Past, wchar_t wcMarker )
{
    // I am passed offsets of one sentence in LocalCopy.
    // Prefix the marker character and space at left of line:
    // '|' for kept lines,
    // ' ' for ignored lines,
    // Indent a litle, keep all within 80 chars.

    #if DO_DEBUG_VALUATION
    // Draw me up first and last 30 chars, and passed mark in left border.
    {
        wchar_t wk[100];
        wchar_t * into = wk;
        *into++ = ' ';
        *into++ = ' ';
        *into++ = ' ';
        *into++ = wcMarker;
        *into++ = ' ';

        wchar_t * from = LocalCopy + Atop;
        wchar_t * stop = LocalCopy + Past;
        if( stop > from + 30 )
            stop = from + 30;
        for( ;; )
        {
            if( from == stop )
                break;
            wchar_t wc = *from++;
            if( wc > '~' )
                *into++ = '$';
            else if( wc < ' ' )
                *into++ = '@';
            else
                *into++ = wc;
        }

        stop = LocalCopy + Past;
        if( from < stop - 30 )
        {
            from = stop - 30;
            *into++ = '=';
            *into++ = '=';
            *into++ = '=';
            *into++ = '.';
            *into++ = '.';
            *into++ = '.';
            *into++ = '=';
            *into++ = '=';
            *into++ = '=';
        }
        for( ;; )
        {
            if( from == stop )
                break;
            wchar_t wc = *from++;
            if( wc > '~' )
                *into++ = '?';
            else if( wc < ' ' )
                *into++ = '!';
            else
                *into++ = wc;
        }
        *into++ = '<';
        *into++ = '<';
        *into++ = '=';
        *into++ = '=';
        *into = NULL;
        Spew( wk );
    }
    #endif


}

void CTxt::SpewPipeItemToIgnore( CPipeItem * pPipeItem )
{
    // Just to balance Add's Spew for debug analysis;

    #if DO_DEBUG_VALUATION
        Spew( L"     === Yucky Pitem ===" );
    #endif

    size_t i = 0;
    for( ;; )
    {
        if( i == pPipeItem->nFilledTriplets )
            break;
        size_t take = 3 * i;

        size_t Atop  = pPipeItem->pMallocTriplets[ take + 0 ];
        size_t Past  = pPipeItem->pMallocTriplets[ take + 1 ];

        #if DO_DEBUG_VALUATION
            SpewSentenceSafely( Atop, Past, ' ' );
        #endif

        i ++;
    }
    #if DO_DEBUG_VALUATION
        Spew( L"     === End Pitem ===" );
    #endif
}

void CTxt::AddPipeItemToIdxes( CPipeItem * pPipeItem )
{
    // Got members:
    // pPipeItem->AtopBlock
    // pPipeItem->PastBlock
    // pPipeItem->IndexBlock
    // pPipeItem->pMallocTriplets
    // pPipeItem->nMallocTriplets
    // pPipeItem->nFilledTriplets

    // I find that I have "worthy" blocks with no sentences,
    // just because they were promoted through the pipeline.
    // Skip such blocks at this final time.
    // No, I am wrong about that. Would lose headings. See example below:
    // no... if( pPipeItem->nFilledTriplets == 0 )
    // no...     return;
    //
    // -------------------------
    // -X- CHAPTER XV: QUESTION 13: THE POWER OF CHRIST'S SOUL<<==
    // ------ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // -O- If Christ had, as stated, know===...===y did He not have omnipotence?<<==
    // -O- Certain Lutherans who are call===...===ywhere, and always omnipotent.<<==
    // ------ENUM_SIGNIFY_VERY_GOOD
    //      -- Worthy PipeItem --
    //    | if christ had, as stated, know===...===y did he not have omnipotence?<<==
    //    | certain lutherans who are call===...===ywhere, and always omnipotent.<<==
    //      -- End PipeItem --
    // -------------------------
    // -X- First Article: Whether The Sou===...===ipotence In The Absolute Sense<<==
    // ------ENUM_SIGNIFY_NULL_HINT
    // -------------------------
    // -O- Conclusion.<<==
    // -O- The soul of Christ could not h===...===potence in the absolute sense.<<==
    // ------ENUM_SIGNIFY_VERY_GOOD
    //      -- Worthy PipeItem --
    //    | conclusion.<<==
    //    | the soul of christ could not h===...===potence in the absolute sense.<<==
    //      -- End PipeItem --
    // -------------------------

    // Add the block to it's Idx:

    m_pOnePaper->pIdxTextBlocks->AddIdx(
        pPipeItem->AtopBlock, // was m_AtopBlockOffset,
        pPipeItem->PastBlock, // was m_PastBlockOffset,
        pPipeItem->IndexBlock, // was Index,
        0 );


    #if DO_DEBUG_VALUATION
        Spew( L"     -- Worthy PipeItem --" );
    #endif

    size_t i = 0;
    for( ;; )
    {
        if( i == pPipeItem->nFilledTriplets )
            break;
        size_t take = 3 * i;

        size_t Atop  = pPipeItem->pMallocTriplets[ take + 0 ];
        size_t Past  = pPipeItem->pMallocTriplets[ take + 1 ];
        size_t Index = pPipeItem->pMallocTriplets[ take + 2 ];
        m_pOnePaper->pIdxSentences->AddIdx( Atop, Past, Index, 0 );

        #if DO_DEBUG_VALUATION
            SpewSentenceSafely( Atop, Past, '|' );
        #endif

        i ++;
    }
    #if DO_DEBUG_VALUATION
        Spew( L"     -- End PipeItem --" );
    #endif
}

