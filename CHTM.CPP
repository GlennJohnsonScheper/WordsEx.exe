// This is file: CHtm.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// During a load cache item, and during an http fetch,
// the new & delete will init & finish the html parse,
// passing in all source bytes via ParseInput routine.
// Hey, no. My callers must flush parse before delete.
// Now I only have 1 caller: ProcessPaper in Call.cpp.

// It is my responsibility to hang and save any BASE tag UrlIndex,
// but my caller's responsibility to use any found BASE UrlIndex.

// I no longer have knowledge of, to modify the pOnePaper,
// but my caller examines my members, like m_BaseUrlIndex.
// I am passed the pOnePaper's two members: text, indexes.

// I am going to begin a changeover from USASCII to ISO LATIN range
// So what does this table still change, now that I've gone Latin?
// Ampersand, Left Angle Bracket, Right Angle Bracket are special.
// Control codes should not appear, but TAB, CR, and LF become SPC.

BYTE CertainRemappings [0x40] = {
SPC, SPC, SPC, SPC,  SPC, SPC, SPC, SPC,  SPC, SPC, SPC, SPC,  SPC, SPC, SPC, SPC,  // 00H = 0
SPC, SPC, SPC, SPC,  SPC, SPC, SPC, SPC,  SPC, SPC, SPC, SPC,  SPC, SPC, SPC, SPC,  // 10H = 16
SPC, '!', DQT, '#',  '$', '%', AM_, SQT,  '(', ')', '*', '+',  ',', '-', '.', SLA,  // 20H = 32
'0', '1', '2', '3',  '4', '5', '6', '7',  '8', '9', ':', ';',  LA_, '=', RA_, '?',  // 30H = 48
};


    //     New revelations from Unicode: ( not yet implemented )
    //
    //
    //     Convert the following to SPACE:
    //
    // 0009          ; Sp # Cc       &lt;control-0009&gt;
    // 000B..000C    ; Sp # Cc   [2] &lt;control-000B&gt;..&lt;control-000C&gt;
    // 0020          ; Sp # Zs       SPACE
    // 1680          ; Sp # Zs       OGHAM SPACE MARK
    // 180E          ; Sp # Zs       MONGOLIAN VOWEL SEPARATOR
    // 2000..200A    ; Sp # Zs  [11] EN QUAD..HAIR SPACE
    // 202F          ; Sp # Zs       NARROW NO-BREAK SPACE
    // 205F          ; Sp # Zs       MEDIUM MATHEMATICAL SPACE
    // 3000          ; Sp # Zs       IDEOGRAPHIC SPACE
    //
    //     Despite calling this OLetter, make it a space too:
    //
    // 00A0          ; OLetter # Zs       NO-BREAK SPACE
    //
    //
    //     Delete the following characters:
    //
    // 00AD          ; Format # Cf       SOFT HYPHEN
    // 0600..0603    ; Format # Cf   [4] ARABIC NUMBER SIGN..ARABIC SIGN SAFHA
    // 06DD          ; Format # Cf       ARABIC END OF AYAH
    // 070F          ; Format # Cf       SYRIAC ABBREVIATION MARK
    // 17B4..17B5    ; Format # Cf   [2] KHMER VOWEL INHERENT AQ..KHMER VOWEL INHERENT AA
    // 200B          ; Format # Cf       ZERO WIDTH SPACE
    // 200E..200F    ; Format # Cf   [2] LEFT-TO-RIGHT MARK..RIGHT-TO-LEFT MARK
    // 202A..202E    ; Format # Cf   [5] LEFT-TO-RIGHT EMBEDDING..RIGHT-TO-LEFT OVERRIDE
    // 2060..2063    ; Format # Cf   [4] WORD JOINER..INVISIBLE SEPARATOR
    // 206A..206F    ; Format # Cf   [6] INHIBIT SYMMETRIC SWAPPING..NOMINAL DIGIT SHAPES
    // FEFF          ; Format # Cf       ZERO WIDTH NO-BREAK SPACE
    //
    //     Find out if the following are iswalpha
    //     I don't think it's worth looking for,
    //     unless maybe just the hebrew & arabic:
    //
    // 01BB          ; OLetter # Lo       LATIN LETTER TWO WITH STROKE
    // 01C0..01C3    ; OLetter # Lo   [4] LATIN LETTER DENTAL CLICK..LATIN LETTER RETROFLEX CLICK
    // 0294          ; OLetter # Lo       LATIN LETTER GLOTTAL STOP
    // 02B9..02BF    ; OLetter # Lm   [7] MODIFIER LETTER PRIME..MODIFIER LETTER LEFT HALF RING
    // 02C6..02D1    ; OLetter # Lm  [12] MODIFIER LETTER CIRCUMFLEX ACCENT..MODIFIER LETTER HALF TRIANGULAR COLON
    // 02EE          ; OLetter # Lm       MODIFIER LETTER DOUBLE APOSTROPHE
    // 0559          ; OLetter # Lm       ARMENIAN MODIFIER LETTER LEFT HALF RING
    // 05D0..05EA    ; OLetter # Lo  [27] HEBREW LETTER ALEF..HEBREW LETTER TAV
    // 05F0..05F2    ; OLetter # Lo   [3] HEBREW LIGATURE YIDDISH DOUBLE VAV..HEBREW LIGATURE YIDDISH DOUBLE YOD
    // 05F3          ; OLetter # Po       HEBREW PUNCTUATION GERESH
    // 0621..063A    ; OLetter # Lo  [26] ARABIC LETTER HAMZA..ARABIC LETTER GHAIN
    // 0640          ; OLetter # Lm       ARABIC TATWEEL
    // 0641..064A    ; OLetter # Lo  [10] ARABIC LETTER FEH..ARABIC LETTER YEH
    // 066E..066F    ; OLetter # Lo   [2] ARABIC LETTER DOTLESS BEH..ARABIC LETTER DOTLESS QAF
    // 0671..06D3    ; OLetter # Lo  [99] ARABIC LETTER ALEF WASLA..ARABIC LETTER YEH BARREE WITH HAMZA ABOVE
    // 06D5          ; OLetter # Lo       ARABIC LETTER AE
    // 06E5..06E6    ; OLetter # Lm   [2] ARABIC SMALL WAW..ARABIC SMALL YEH
    // 06EE..06EF    ; OLetter # Lo   [2] ARABIC LETTER DAL WITH INVERTED V..ARABIC LETTER REH WITH INVERTED V
    // 06FA..06FC    ; OLetter # Lo   [3] ARABIC LETTER SHEEN WITH DOT BELOW..ARABIC LETTER GHAIN WITH DOT BELOW
    // 06FF          ; OLetter # Lo       ARABIC LETTER HEH WITH INVERTED V


wchar_t * NumTypes[] = {
   L"NO_NUMBER",
   L"NUMBER",
   L"NUMBER_POINTS",
   L"NUMBER_COMMAS",
   L"NUMBER_HYPHENS",
   L"NUMBER_SLASHS",
   L"NUMBER_COLONS",
   L"NUMBER_MIXED",
};

CHtm::CHtm( size_t UrlIndex, CWsb * pWsbPaperText, CIdx * pIdxPaperIndex, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int PageIsAQrp, COneSurl * pOneSurl )
{
    #if DO_DEBUG_CALLS
        Routine( L"151" );
    #endif

    // Notice that CHtm doesn't mess with the Paper itself,
    // because that would cause inversions of Crix entries.
    // However, CHtm needs to know his index to XRef links.

    m_PassedUrlIndex = UrlIndex;
    m_BestUrlIndex = m_PassedUrlIndex; // until/unless base href seen
    m_pWsbPaperText = pWsbPaperText;
    m_pIdxPaperIndex = pIdxPaperIndex;
    m_pSolFrames = pSolFrames;
    m_pSolMores = pSolMores;
    m_pSolHits = pSolHits;
    m_PageIsAQrp = PageIsAQrp;

    #if DO_DEBUG_FACTTEST
        Spew( L"CHtm constructor: Setting up parameters..." );
        SpewValue( L"m_PageIsAQrp", m_PageIsAQrp );
    #endif

    // pOneSurl is non-null whenever a page is a query result page,
    // OR any hit page that was discovered from a query result page,
    // so that the governing search engine may be credited.

    m_pOneSurl = pOneSurl;
    m_AppendPageScrapingFacts = 0; // t.b.d in setups...
    m_NoneGoodUntilRuleIsInhibiting = 0; // t.b.d in setups...
    m_NoneGoodAfterRuleIsInhibiting = 0; // until such a rule matches

    if( m_pOneSurl == NULL )
    {
        #if DO_DEBUG_FACTTEST
            Spew( L"I was passed a pOneSurl == NULL" );
        #endif
        m_PageIsAQrp = 0; // CYA: QRP must also have a SURL
    }
    else
    {
        #if DO_DEBUG_FACTTEST
            Spew( L"I was passed a pOneSurl != NULL" );
        #endif
    }

    if( m_PageIsAQrp )
    {
        // Get out some bools of COneSurl for any QRP.
        if( m_pOneSurl->AnnotateFactsForStudy )
        {
            m_AppendPageScrapingFacts = 1;
        }

        #if DO_STUDY_ALL_QRP
            m_AppendPageScrapingFacts = 1; // quick way to turn them all on.
        #endif


        #if DO_DEBUG_FACTTEST
            SpewValue( L"m_AppendPageScrapingFacts", m_AppendPageScrapingFacts );
        #endif

        if( m_pOneSurl->ContainsAnyRuleNoneGoodUntil )
        {
            m_NoneGoodUntilRuleIsInhibiting = 1; // Cleared when rule matched.
            #if DO_DEBUG_FACTTEST
                Spew( L"CHtm constructor: Setting the NoneGoodUntilRule inhibitory bool." );
            #endif
        }

    }
    else
    {
        m_AppendPageScrapingFacts = 0;
    }

    if( m_AppendPageScrapingFacts )
    {
        // non-NULL-ity of m_pWsbQrpFacts is identical to m_AppendPageScrapingFacts true;
        m_pWsbQrpFacts = new CWsb( );
        m_pWsbQrpFacts->Add( L"\r\n\r\n" );
        m_pWsbQrpFacts->Add( L"\r\n==================================" );
        m_pWsbQrpFacts->Add( L"\r\nWordsEx Page Scraping Observations" );
        m_pWsbQrpFacts->Add( L"\r\n==================================" );
        m_pWsbQrpFacts->Add( L"\r\nSelect search engine rules from these idiomatic Facts." );
        m_pWsbQrpFacts->Add( L"\r\nSolve any alternations (good/bad/more and prior/next)." );
        m_pWsbQrpFacts->Add( L"\r\nPlace the rules after your \"GET\" search engine lines." );
        m_pWsbQrpFacts->Add( L"\r\n\r\n" );
    }
    else
    {
        m_pWsbQrpFacts = NULL;
    }

    TextHeldInLayer1AmpSuspense = new wchar_t [ MAX_SUSPENSE + 1 ];
    TextHeldInLayer1LabSuspense = new wchar_t [ MAX_SUSPENSE + 1 ];
    TextHeldInLayer1Markup = new wchar_t [ MAX_MARKUP_TEXT + 1 ];
    TextHeldInLayer2TokenText = new wchar_t [ MAX_TOKEN_TEXT + 1 ];
    szTitleOrAnchorBuffer = new wchar_t [ MAX_TITLE_OR_ANCHOR + 1 ];

    // Since they cannot set m_BaseUrlIndex until after constructor,
    // constructor cannot GetFullKey, but caller must set m_pMalBase.

    m_BaseUrlIndex = 0;  // until set by caller, or TAG_BASE, etc.
    m_pMalBase = NULL; // until set by caller, or TAG_BASE, etc.

    m_nCSetFound = 0;  // until set by TAG_META / http-equiv, etc.
    m_nLangFound = 0;  // until set by TAG_META / http-equiv, etc.

    InitParse( );
    m_pWsbFormParts = NULL;

    m_pWsbTitleText = new CWsb( );

}

CHtm::~CHtm( )
{
    #if DO_DEBUG_CALLS
        Routine( L"152" );
    #endif
    delete TextHeldInLayer1AmpSuspense;
    TextHeldInLayer1AmpSuspense = NULL;
    delete TextHeldInLayer1LabSuspense;
    TextHeldInLayer1LabSuspense = NULL;
    delete TextHeldInLayer1Markup;
    TextHeldInLayer1Markup = NULL;
    delete TextHeldInLayer2TokenText;
    TextHeldInLayer2TokenText = NULL;
    delete szTitleOrAnchorBuffer;
    szTitleOrAnchorBuffer = NULL;

    if( m_pMalBase != NULL )
    {
        MyFree( 63, zx, m_pMalBase );
        m_pMalBase = NULL;
    }

    if( m_pWsbFormParts != NULL )
    {
        delete m_pWsbFormParts;
        m_pWsbFormParts = NULL;
    }

    if( m_pWsbQrpFacts != NULL )
    {
        delete m_pWsbQrpFacts;
        m_pWsbQrpFacts = NULL;
    }

    {
        delete m_pWsbTitleText;
        m_pWsbTitleText = NULL;
    }

}

void CHtm::InitParse( )
{
    #if DO_DEBUG_CALLS
        Routine( L"153" );
    #endif
    InputUtf8State = 0;
    InputUtf8Byte1 = 0;
    InputUtf8Byte2 = 0;
    InputUtf8Byte3 = 0;
    PriorPostUtf8Character = 0;
    InputNewlineCount = 0;
    ConsecutiveInputNewlines = 0;

    BitMaskReasonsToHonorNewlines = 0; // CFio::CommonPaperFileInput sets this
    BitMaskReasonsToHonorIndent = 0; // CFio::CommonPaperFileInput may set this

    Layer1State = LAYER1STATE_NOTHING;

    IndexHeldInLayer1AmpSuspense = 0;
    Layer1AmpPushedState = 0;

    IndexHeldInLayer1LabSuspense = 0;
    ColonCountInLabSuspense = 0;
    LastHyphenCountInComment = 0;

    IndexHeldInLayer1Markup = 0;
    TagEnumHeldInLayer1Markup = 0;
    IndexPastTagInLayer1Markup = 0;

    IndexHeldInLayer2TokenText = 0;

    InsertSpaceIfSayAlphaComes = 0;
    PendingInsertSpaceChar = 0;

    BetweenHtmlScriptTags = 0;
    BetweenHtmlTitleTags = 0;

    ArmToSaveStartAnchorOffset = 0;
    StartAnchorOffsetInText = 0;

    TagA_Had_Name = 0;
    TagA_Had_Href = 0;
    LastTagA_UrlIndex = -1;      // set to -1 at TOP AND end of file.
    SavedAtEntryToTagA = -1;     // I dunno about all this aging yet.
    UrlIndexOfAnyPriorTagA = -1; // set to -1 at top of file ( from above ).

    InsideTagSpanWithLangAttr = 0;
    InsideTagAWithLangAttr = 0;

    BitMaskReasonsToDiscardText = 0;
    CurrentBlockOffsetInText = 0;
    PreferredWrapColumnInOutputText = LOW_WRAP_COLUMN;

    CurrentBlockIsEntered = 0;

    ConsecutiveNewlinesInOutputText = 0;
    CurrentColumnInOutputText = 0;
    IndentationColumnInOutputText = 0;
    IndentationColumnBeforeDlTag = 0;

    SavingTitleOrAnchorText = 0;
    EndingTagAOffsetInText = -1;
    LastTagAIDXHighWater = 0;

    TitleOrAnchorBufferStringLength = 0;

    m_DuringTagForm = 0;
    m_BetweenOpenCloseTagForm = 0;
    m_FormActionPresent = 0;
    m_FormMethodUrlIndex = 0;
    m_FormGet1Post2 = 0;
    m_DuringTagSelect = 0;
    m_BetweenOpenCloseTagSelect = 0;
    m_DuringTagInput = 0;
    m_DuringTagOption = 0;
    m_FormOptionSelected = 0;
    m_FormCheckBoxChecked = 0;
    m_FormRadioChecked = 0;
    m_FormInputType = ENUM_INPUT_TYPE_UNSTATED;
    m_FormInputTypeTextCount = 0;
    m_FormInputTypeSubmitCount = 0;
    m_FormInputTypeTextName = NULL;
    m_FormInputName = NULL;
    m_FormInputValue = NULL;
    m_FormSelectName = NULL;
    m_FormOptionValue = NULL;
    m_FormRejectedBitfield = 0;

    m_FormParamCount = 0;
    PriorTokenUnder50[0] = NULL;
    PriorTokenIsNumberEnum = NO_NUMBER;

    Url_A_RuleType = 0;
    Url_A_RuleNo = 0;
    Url_B_RuleType = 0;
    Url_B_RuleNo = 0;

    if( m_AppendPageScrapingFacts
    && m_NoneGoodUntilRuleIsInhibiting )
        m_pWsbQrpFacts->Add( L"Starting parse with a none-good-until inhibitory rule active.\r\n" );
}

void CHtm::FlushParse( )
{
    #if DO_DEBUG_CALLS
        Routine( L"154" );
    #endif
    // Since there is no more big work on block boundaries,
    // I do not need to force two newlines at end of input:
    // ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
    ProcessTextTokenBuffer( 1 ); // put any word, newline, but no newblock.

    if( m_PageIsAQrp )
    {
        // Age the last URL from 'B' to 'A' position.
        UrlIndexOfAnyPriorTagA = LastTagA_UrlIndex;

        // This -1 indicates that 'B' is actually EOF.
        LastTagA_UrlIndex = -1;      // set to -1 at end of file
        HeadLinkIsCrossingTagABoundary( ); // at end of file
    }

    if( m_AppendPageScrapingFacts )
    {
        // append QRP page scraping observations to results text.
        m_pWsbPaperText->AddWsb( m_pWsbQrpFacts );
    }
}

void CHtm::ParseWideUCSInputBuffer( wchar_t * pBuffer, size_t nBuffer )
{
    // My callers are expected to have prepared buffer so that
    // there are no whitespaces except CR LF and SPACE, and no
    // controls, and no NULLs in whole buffer; Also, any UTF-8
    // was already decoded, but not entities yet, nor charsets.

    #if DO_DEBUG_CALLS
        Routine( L"156" );
    #endif

    size_t i = 0;
    for( ;; )
    {
        if( i == nBuffer )
            break;
        ParseOneInputUCSChar( pBuffer[i] ); // Full Unicode 0x0000 to 0xffff range
        i ++;
    }
}

void CHtm::ParseOneInputUCSChar( wchar_t PostUtf8Char )
{
    #if DO_DEBUG_CALLS
        // Routine( L"157" );
    #endif

    #if DO_DEBUG_HTMLDEEP
    {
        wchar_t wk[80];
        wchar_t cw = PostUtf8Char;
        if( cw < 0x20
        ||  cw > 0x7e )
            cw = '?';
        wsprintf( wk, L"Input char 0x%04x '%c'.",
            PostUtf8Char, cw );
        ; Spew( wk );
    }
    #endif

    // Input is now Unicode. Just fix my 3 special markers.
    // Also change any CR LF into SPACES during HTML parse.

    wchar_t ReplacementChar = PostUtf8Char;

    if( PostUtf8Char < 0x40 )
        ReplacementChar = CertainRemappings [PostUtf8Char]; // FIX AMP, CR, etc.

    BodyCharHtmlParseLayer1( ReplacementChar );

    // I will independently recognize CR, or LF, or CR LF
    // from input to flush out lines or blocks at times.

    switch( PostUtf8Char )
    {
        case '\r':
            InputNewlineCount ++;
            ConsecutiveInputNewlines ++;
            if( BitMaskReasonsToHonorNewlines != 0 )
                ProcessTextTokenBuffer( ConsecutiveInputNewlines ); // put any word, newline, newblock.
            break;

        case '\n':
            if( PriorPostUtf8Character != '\r' ) // prevents CR + LF duplication
            {
                InputNewlineCount ++;
                ConsecutiveInputNewlines ++;
                if( BitMaskReasonsToHonorNewlines != 0 )
                    ProcessTextTokenBuffer( ConsecutiveInputNewlines ); // put any word, newline, newblock.
            }
            break;

        default:
                ConsecutiveInputNewlines = 0;
            break;
    }
    PriorPostUtf8Character = PostUtf8Char; // just to recognize CR LF
}

void CHtm::RedoCharHtmlParseLayer1( wchar_t inputChar )
{
    #if DO_DEBUG_CALLS
        // Routine( L"158" );
    #endif

    #if DO_DEBUG_HTMLDEEP
    {
        wchar_t wk[80];
        wchar_t cw = inputChar;
        if( cw < 0x20
        ||  cw > 0x7e )
            cw = '?';
        wsprintf( wk, L"   Re-do char 0x%04x '%c'.",
            inputChar, cw );
        ; Spew( wk );
    }
    #endif

    // Reprocessing step upon failure to recognize partial tag or entity.
    // My callers are all in BodyCharHtmlParseLayer1, or its subroutines.
    // There is one of my callers that would pass me active marker chars.
    // I don't think the logic would recurse more than two calls deep.
    BodyCharHtmlParseLayer1( inputChar ); // I AM recursion!
}

void CHtm::BodyCharHtmlParseLayer1( wchar_t inputChar )
{
    #if DO_DEBUG_CALLS
        // Routine( L"159" );
    #endif

    // inputChar is now UCS ( UNICODE ), however, change NBSP to space:

    if( inputChar == 160 ) // decimal 160 = NBSP
        inputChar = ' ';

    // Recursive. See my two callers above,
    // one of which I or helpers will call.

    // "Layer1" label signified doing HTML analysis, while
    // "Layer2" label signified toing TEXT token analysis.

    // This state machine has to do with the concept of being
    // after an LAB ( Left Angle Bracket ), thus possibly a tag;
    // or after an AMP ( Ampersand ), thus possibly some entity;
    // And entities can occur inside tags. Until a possibility
    // has been decided, each case collects chars in suspense.

    #if DO_DEBUG_HTMLDEEP
    {
        wchar_t wk[500];

        wchar_t cw = inputChar;
        if( cw < 0x20
        ||  cw > 0x7e )
            cw = '?';

        TextHeldInLayer1LabSuspense [IndexHeldInLayer1LabSuspense] = NULL;
        TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense] = NULL;
        TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = NULL;
        TextHeldInLayer2TokenText [IndexHeldInLayer2TokenText] = NULL;

        wsprintf( wk,
            L"     HTML parser state %d new char 0x%04x '%c'."
            L" LAB[%d] = {%.40s}. AMP[%d] = {%.40s}."
            L" MUP[%d] = {%.40s}. TOK[%d] = {%.40s}.",
            // -----
            Layer1State, inputChar, cw,
            IndexHeldInLayer1LabSuspense,
            TextHeldInLayer1LabSuspense,
            IndexHeldInLayer1AmpSuspense,
            TextHeldInLayer1AmpSuspense,
            IndexHeldInLayer1Markup,
            TextHeldInLayer1Markup,
            IndexHeldInLayer2TokenText,
            TextHeldInLayer2TokenText );
        ; Spew( wk );
    }
    #endif

    switch( Layer1State )
    {

    default:
        ProgramError( L"INVALID LAYER1STATE" );
        Layer1State = LAYER1STATE_NOTHING;
        break;


    case LAYER1STATE_NOTHING:
        // In this state, no entities nor tags are in progress.
        switch( inputChar )
        {
        case AM_: // ampersand _ replacement marker
            #if DO_DEBUG_ENTITY
                ; Spew( L"AMPERSAND STARTING AMP SUSPENSE FROM NOTHING." );
            #endif
            TextHeldInLayer1AmpSuspense [0] = AMP;
            IndexHeldInLayer1AmpSuspense = 1;
            Layer1AmpPushedState = LAYER1STATE_NOTHING;
            Layer1State = LAYER1STATE_AMP;
            break;
        case LA_: // left angle bracket _ replacement marker
            TextHeldInLayer1LabSuspense [0] = LAB;
            IndexHeldInLayer1LabSuspense = 1;
            ColonCountInLabSuspense = 0;
            // There is no need for a Layer1LabPushedState; always _Nothing.
            Layer1State = LAYER1STATE_LAB;
            break;
        case RA_: // right angle bracket _ replacement marker
            // Not special. Change from an active marker to a dead character.
            inputChar = RAB;
            // no break; Fall into default process.
        default:
            // this was Surf4me WorkCharTextParseLayer2...
            // this is the only place, so put it inline:
            if( iswspace( inputChar ) )
            {
                InsertSpaceIfSayAlphaComes = 0;
                // whitespace outputs any current token being assembled.
                ProcessTextTokenBuffer( 0 ); // put any word; but not newline, newblock.
                // Afterwards, leave this clue, to space before any next token.
                PendingInsertSpaceChar = 1;
            }
            else
            {
                // Printable inputChar.
                if( InsertSpaceIfSayAlphaComes )
                {
                    InsertSpaceIfSayAlphaComes = 0;
                    if( iswalpha( inputChar ) )
                    {
                        ProcessTextTokenBuffer( 0 ); // put any word; but not newline, newblock.
                        PendingInsertSpaceChar = 1;
                    }
                }
                if( IndexHeldInLayer2TokenText >= MAX_TOKEN_TEXT - 3 )
                {
                    // That -3 means to leave room for what?
                    // Output old token and start new token.
                    ProcessTextTokenBuffer( 0 ); // put any word; but not newline, newblock.
                }
                TextHeldInLayer2TokenText [ IndexHeldInLayer2TokenText ] = inputChar;
                IndexHeldInLayer2TokenText ++;
            }
            break;
        }
        break;

    case LAYER1STATE_LAB:
        // In this state, tag name recognition is in progress.
        switch( inputChar )
        {

        case AM_: // ampersand _ replacement marker
            // The ampersand is special, cannot be in tag names,
            // and initiates other states, so must trigger redo.
            // Upon AMP, re-process non-valid-tag LAB-introduced text.
            Layer1State = LAYER1STATE_NOTHING;
            RedoTextHeldInLayer1LabSuspense( );
            // Then start up the AMP state
            #if DO_DEBUG_ENTITY
                ; Spew( L"AMPERSAND STARTING AMP SUSPENSE FROM LAB STATE." );
            #endif
            TextHeldInLayer1AmpSuspense [0] = AMP;
            IndexHeldInLayer1AmpSuspense = 1;
            Layer1AmpPushedState = LAYER1STATE_NOTHING;
            Layer1State = LAYER1STATE_AMP;
            break;

        case LA_: // left angle bracket _ replacement marker
            // The left angle bracket cannot be in tag names,
            // so trigger redo, and reinitiate the LAB state.
            // Upon LAB, re-process non-valid-tag LAB-introduced text.
            Layer1State = LAYER1STATE_NOTHING;
            RedoTextHeldInLayer1LabSuspense( );
            // Again then, start up the LAB state
            TextHeldInLayer1LabSuspense [0] = LAB;
            IndexHeldInLayer1LabSuspense = 1;
            ColonCountInLabSuspense = 0;
            Layer1State = LAYER1STATE_LAB;
            break;

        case RA_: // right angle bracket _ replacement marker
            // The right angle bracket is special, and terminates any tag.
            {
                // Note: this is only #1 of 2 places
                // HTML tag names will be evaluated.

                // Now that I allowed "<!--" to pass into tag suspense,
                // make a pretest for that, to switch to comment state.

                // We already know TextHeldInLayer1LabSuspense [0] == LAB.
                // But we must test count before testing multiple chars.
                // This is a post-increment. So at 4, could hold [0]-[3].
                if( IndexHeldInLayer1LabSuspense > 3
                && TextHeldInLayer1LabSuspense [1] == '!'
                && TextHeldInLayer1LabSuspense [2] == '-'
                && TextHeldInLayer1LabSuspense [3] == '-' )
                {
                    // Change from TAG to COMMENT processing state.
                    // Discard the LAB-introduced suspense buffer.
                    IndexHeldInLayer1LabSuspense = 0;
                    // Begin a state to discard the html comment.
                    LastHyphenCountInComment = 0;
                    Layer1State = LAYER1STATE_COMMENT;

                    #if DO_DEBUG_COMMENT
                        Spew( L"Entering Comment state at RAB." );
                    #endif

                }
                else

                // Test the accumulated characters for valid tag recognition.
                if( ValidTagNameInAccumulatedLabSuspense( ) )
                {
                    // Add RAB to Markup buffer
                    // Here, RAB comes right after bare tag name.
                    TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = RAB;
                    IndexHeldInLayer1Markup ++; // Cannot be near full after tag name
                    // The whole tag is ready to process:

                    // Between <SCRIPT> .... </SCRIPT> is TOO UGLY!
                    // I need to process the SCRIPT tag to end run.
                    // I need to expect XML form: <SCRIPT ..... />.

                    if( ! BetweenHtmlScriptTags
                    || TagEnumHeldInLayer1Markup == TAG_SCRIPT )
                    {
                        // Here's the charm of the whole process:
                        ProcessWholeMarkupTag( ); // this is #1 of 2 places
                    }

                    // The LAB suspense buffer was consumed:
                    IndexHeldInLayer1LabSuspense = 0;
                    Layer1State = LAYER1STATE_NOTHING;
                }
                else if( ColonCountInLabSuspense == 1 )
                {
                    // Throw away xml-ish tags like "<o:p>".
                    // The LAB suspense buffer was consumed:
                    IndexHeldInLayer1LabSuspense = 0;
                    Layer1State = LAYER1STATE_NOTHING;
                }
                else
                {
                    // Upon RAB, re-process non-valid-tag LAB-introduced text.
                    TextHeldInLayer1LabSuspense [IndexHeldInLayer1LabSuspense] = RAB;
                    IndexHeldInLayer1LabSuspense ++;
                    Layer1State = LAYER1STATE_NOTHING;
                    RedoTextHeldInLayer1LabSuspense( );
                }
            }
            break;

        default: // other chars not stopping a tag name, nor special
            // Accumulate any valid tag name characters.
            // Also recognize html comment state change.
            // Accept the initial slash '/' of end tags.
            // Otherwise accept alphanumerics into tags.
            // Also, allow tag <?xml with question mark.
            // Also, one tag name in my table has colon.
            // Worse, lots of xml use <namespace:thing>.
            // Also, allow tags <x-... for experimental.
            // Nah, that dangerously approaches <tag-->.

            // Let's try to tighten this parse up.
            // Many RAB might be inside a comment:
            // <!--tr><td><br><br><A HREF=...<br></td></tr-->
            // Instead of action upon only "<!", accept that,
            // and another character, and act on fourth char.

            if( iswalnum( inputChar )
            || inputChar == SLA && IndexHeldInLayer1LabSuspense == 1
            || inputChar == '?' && IndexHeldInLayer1LabSuspense == 1
            || inputChar == '!' && IndexHeldInLayer1LabSuspense == 1
            || inputChar == '-' && ( IndexHeldInLayer1LabSuspense == 2
                                  || IndexHeldInLayer1LabSuspense == 3 )
            || inputChar == ':' && ( ++ ColonCountInLabSuspense == 1 ) )
            {
                // Add char to LAB-introduced suspense buffer.
                TextHeldInLayer1LabSuspense [IndexHeldInLayer1LabSuspense] = inputChar;
                IndexHeldInLayer1LabSuspense ++;
                // If reached max length, re-process
                // non-valid-tag LAB-introduced text.
                if( IndexHeldInLayer1LabSuspense == MAX_SUSPENSE - 1 )
                {
                    Layer1State = LAYER1STATE_NOTHING;
                    RedoTextHeldInLayer1LabSuspense( );
                }
            }
            else
            {
                // Process a character that cannot be in a valid tag name.
                // It might be some space or punct right after valid name.
                // This is the path most tags take, those with parameters.
                // Test accumulated characters for valid tag recognition.
                // Since I already tested for RAB, only allow whitespace.
                // But also, allow for this XML-ish construction: <br/>.

                // Note: this is only #2 of 2 places
                // HTML tag names will be evaluated.

                // Now that I allowed "<!--" to pass into tag suspense,
                // make a pretest for that, to switch to comment state.

                // We already know TextHeldInLayer1LabSuspense [0] == LAB.
                // But we must test count before testing multiple chars.
                // This is a post-increment. So at 4, could hold [0]-[3].
                if( IndexHeldInLayer1LabSuspense > 3
                && TextHeldInLayer1LabSuspense [1] == '!'
                && TextHeldInLayer1LabSuspense [2] == '-'
                && TextHeldInLayer1LabSuspense [3] == '-' )
                {

                    // But wait! Throwing this comment tag in html page
                    // kills html parser, coming through this path here:
                    //
                    //    <!--GIHEDSTRT-->
                    //
                    // I must first test if the comment was completed!
                    // If so, toss tag, but do not enter comment state.
                    //
                    // No, it would not be so, as RA_ has its own case.
                    // I would be coming in here atop the second "--".
                    //
                    // So it must be I lose memory of the '-' for RAB.
                    // Yes, I need to fix LastHyphenCountInComment = ?

                    // Change from TAG to COMMENT processing state.
                    // Discard the LAB-introduced suspense buffer.
                    IndexHeldInLayer1LabSuspense = 0;
                    // Begin a state to discard the html comment.
                    if( inputChar == '-' )
                        LastHyphenCountInComment = 1;
                    else
                        LastHyphenCountInComment = 0;
                    Layer1State = LAYER1STATE_COMMENT;

                    #if DO_DEBUG_COMMENT
                        Spew( L"Entering Comment state at default." );
                    #endif

                }
                else

                if( ( inputChar == SPC
                || inputChar == SLA ) )
                {
                    if( ValidTagNameInAccumulatedLabSuspense( ) )
                    {
                        // A valid tag was recognized and copied,
                        // but whole tag is not ready to process.
                        // The LAB suspense buffer was consumed,
                        // and copied to the html markup buffer.
                        IndexHeldInLayer1LabSuspense = 0;
                        // Store this character that ended the tag name.
                        TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = inputChar;
                        IndexHeldInLayer1Markup ++; // Cannot be near full after tag name
                        Layer1State = LAYER1STATE_GOODTAG;
                    }
                    else if( ColonCountInLabSuspense == 1 )
                    {
                        // Throw away xml-ish tags like "<o:p>".
                        // Discard the LAB-introduced suspense buffer.
                        IndexHeldInLayer1LabSuspense = 0;
                        // And set a state to cause me to idle until next RA_.
                        // Now that comment terminator is more strict,
                        // I need a new state to operate like GOODTAG.
                        Layer1State = LAYER1STATE_XML_TAG;
                    }
                    else
                    {
                        // Upon unusable char, re-process non-valid-tag LAB-introduced text.
                        TextHeldInLayer1LabSuspense [IndexHeldInLayer1LabSuspense] = ( char ) inputChar;
                        IndexHeldInLayer1LabSuspense ++;
                        Layer1State = LAYER1STATE_NOTHING;
                        RedoTextHeldInLayer1LabSuspense( );
                    }
                }

            }
            break;
        }
        break;

    case LAYER1STATE_COMMENT:
        // In this state, a comment tag is being skipped.
        // Was too loose if the first '>' ends a comment.
        // Any other character is comment text to ignore.
        // Even active AM_ and LA_ aren't special herein.

        // Let's try to tighten this parse up.
        // Many RAB might be inside a comment:
        // <!--tr><td><br><br><A HREF=...<br></td></tr-->
        // We can merely add a hyphen character counter.

        if( inputChar == RA_
        && LastHyphenCountInComment >= 2 )
        {
            // The right angle bracket is special,
            // and terminates any HTML tag, but in
            // comments, require sequence ..."-->".
            Layer1State = LAYER1STATE_NOTHING;

            #if DO_DEBUG_COMMENT
                Spew( L"Leaving Comment state at RAB." );
            #endif

        }
        if( inputChar == '-' )
            LastHyphenCountInComment ++;
        else
            LastHyphenCountInComment = 0;
        break;

    case LAYER1STATE_GOODTAG:
        // In this state, a tag contents is being accumulated.
        // Loosely, let the first '>' end tag, no matter what.
        // And why do I call that loose? Because quote strings
        // of various flavors may occur in tags, and an RAB in
        // them SHOULD NOT end tag recognition. Can I fix that?
        // Of course, conservative writers will escape the RAB.

        // work more here after studying how quoting works.

        switch( inputChar )
        {
        case AM_: // ampersand _ replacement marker
            // The ampersand is still special to initiate entities.
            #if DO_DEBUG_ENTITY
                ; Spew( L"AMPERSAND STARTING AMP SUSPENSE IN GOODTAG." );
            #endif
            TextHeldInLayer1AmpSuspense [0] = AMP;
            IndexHeldInLayer1AmpSuspense = 1;
            Layer1AmpPushedState = LAYER1STATE_GOODTAG;
            Layer1State = LAYER1STATE_AMP;
            break;
        case RA_: // right angle bracket _ replacement marker
            // The right angle bracket is special, and terminates any tag.
            TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = RAB;
            IndexHeldInLayer1Markup ++; // Cannot be near full. -- See below...
            // The tag with parameters is ready to process:

            // Between <SCRIPT> .... </SCRIPT> is TOO UGLY!
            // I need to process the SCRIPT tag to end run.
            // I need to expect XML form: <SCRIPT ..... />.

            if( ! BetweenHtmlScriptTags
            || TagEnumHeldInLayer1Markup == TAG_SCRIPT )
            {
                // Here's the charm of the whole process:
                ProcessWholeMarkupTag( ); // this is #2 of 2 places
            }

            IndexHeldInLayer1Markup = 0;
            Layer1State = LAYER1STATE_NOTHING;
            break;
        case LA_: // left angle bracket _ replacement marker
            // left angle bracket is not special inside of html tags.
            inputChar = LAB; // replace active LA_ marker with the LAB character.
            // no break; Fall into default process.
        default: // all other characters are markup text to accumulate.
            TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = inputChar;
            IndexHeldInLayer1Markup ++; // Cannot be near full. -- See:
            // Allow extra margin to accomodate two
            // "Cannot be near full" comments above.
            if( IndexHeldInLayer1Markup == MAX_MARKUP_TEXT - 3 )
            {
                // Stop accumulating the well-named, but oversized tag.
                // Discard the Markup buffer, without processing tag.
                IndexHeldInLayer1Markup = 0;
                // And set a state to cause me to idle until next RA_.
                // It used to be LAYER1STATE_COMMENT but he got strict.
                Layer1State = LAYER1STATE_XML_TAG;
            }
            break;
        }
        break;

    case LAYER1STATE_XML_TAG:
        // Throw away xml-ish tags like "<o:p>".
        // Strictly, I got here if have 1 colon.
        // But also, if a tag name is not valid.
        // This is a clone of the GOODTAG code,
        // but at the ending RAB, do nothing.
        // Loosely, let the first '>' end tag, no matter what.
        // And why do I call that loose? Because quote strings
        // of various flavors may occur in tags, and an RAB in
        // them SHOULD NOT end tag recognition. Can I fix that?
        // Of course, conservative writers will escape the RAB.

        // work more here after studying how quoting works.

        switch( inputChar )
        {
        case AM_: // ampersand _ replacement marker
            // The ampersand is still special to initiate entities.
            #if DO_DEBUG_ENTITY
                ; Spew( L"AMPERSAND STARTING AMP SUSPENSE IN XML_TAG." );
            #endif
            TextHeldInLayer1AmpSuspense [0] = AMP;
            IndexHeldInLayer1AmpSuspense = 1;
            Layer1AmpPushedState = LAYER1STATE_XML_TAG;
            Layer1State = LAYER1STATE_AMP;
            break;
        case RA_: // right angle bracket _ replacement marker
            // The right angle bracket is special, and terminates any tag.
            TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = RAB;
            IndexHeldInLayer1Markup ++; // Cannot be near full. -- See below...

            // Here is where I do nothing for an XML tag.
            // I might like to spew them...

            #if DO_DEBUG_XMLTAG
                TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = NULL;
                ; Spew( L"==========XML or UNRECOGNIZED tag detected:" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"----------" );
            #endif

            IndexHeldInLayer1Markup = 0;
            Layer1State = LAYER1STATE_NOTHING;
            break;
        case LA_: // left angle bracket _ replacement marker
            // left angle bracket is not special inside of html tags.
            inputChar = LAB; // replace active LA_ marker with the LAB character.
            // no break; Fall into default process.
        default: // all other characters are markup text to accumulate.
            TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = inputChar;
            IndexHeldInLayer1Markup ++; // Cannot be near full. -- See:
            // Allow extra margin to accomodate two
            // "Cannot be near full" comments above.
            if( IndexHeldInLayer1Markup == MAX_MARKUP_TEXT - 3 )
            {
                // Stop accumulating the well-named, but oversized tag.
                // Discard the Markup buffer, without processing tag.
                IndexHeldInLayer1Markup = 0;
                // And set a state to cause me to idle until next RA_.
                // Same as I am in: Layer1State = LAYER1STATE_XML_TAG;
            }
            break;
        }
        break;

    case LAYER1STATE_AMP:
        // In this state, Ampersand-entity recognition is in progress.

        // Parser barfed on the following Javascript snippet from cache:
        // ....f( b.total<0&&a ){var c=1*a;if( c>=
        // I see the second AMP forgot prior layer1state of LAB down here.
        // I only pushed one state, so do not do another AMP in AMP state.
        // Or rather, I must first re-process the AMP suspense buffer,...

        #if DO_DEBUG_ENTITY
            ; SpewValue( L"AMPERSAND STATE GETS A CHARACTER: ", inputChar );
        #endif
        switch( inputChar )
        {
        case AM_: // ampersand _ replacement marker
        case LA_: // left angle bracket _ replacement marker
        case RA_: // right angle bracket _ replacement marker
            // These 3 chars are special, cannot be in entities,
            // and initiate other states, so must trigger redos.
            // Re-process non-valid-entity AMP-introduced text.
            #if DO_DEBUG_ENTITY
                ; Spew( L"AM_, LA_, or RA_ ending amp state" );
            #endif
            Layer1State = Layer1AmpPushedState;
            RedoTextHeldInLayer1AmpSuspense( );
            // After clearing AMP suspense chars, process this new active marker.
            // ===================================================
            // This is only caller that would Redo active markers.
            // Yes. it is worth starting over, and that calls--ME!
            // ===================================================
            RedoCharHtmlParseLayer1( inputChar ); // re-process an active marker
            break;
        case ';': // semicolon ends entity
            // Recognize valid entities and digest them.
            #if DO_DEBUG_ENTITY
                ; Spew( L"semicolon ending amp state" );
            #endif
            {
                // The use of a CSol for entities, and Find at +1 after &,
                // w/o ;, shows that some times I looked up an empty field.
                // So factor into if that IndexHeldInLayer1AmpSuspense > 1.
                int NewChar = 0;
                if( IndexHeldInLayer1AmpSuspense > 1
                && ( NewChar = ValidEntityInAccumulatedAmpSuspense( ) ) != NULL )
                {
                    // Upon semicolon, process recognized valid entity.
                    // Discard the AMP-introduced valid entity text
                    IndexHeldInLayer1AmpSuspense = 0;
                    // Restore the state which will receive the character.
                    Layer1State = Layer1AmpPushedState;
                    // Process the new character, which is never a marker.
                    RedoCharHtmlParseLayer1( NewChar );
                }
                else
                {
                    // The semicolon did not terminate a valid entity name.
                    // Add semicolon char to AMP-introduced suspense buffer.
                    TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense] = inputChar;
                    IndexHeldInLayer1AmpSuspense ++; // Cannot be near full - see default case.
                    // Restore the state which will receive the AMP suspense text.
                    Layer1State = Layer1AmpPushedState;
                    // Re-process non-valid-entity AMP-introduced text.
                    RedoTextHeldInLayer1AmpSuspense( );
                }
            }
            break;
        default:
            if( iswalnum( inputChar )
            || inputChar == '#' )
            {
                // accumulate characters that are valid in entities.
                TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense] = ( char ) inputChar;
                IndexHeldInLayer1AmpSuspense ++; // Cannot be near full - see:
                // Allow extra margin to accomodate two
                // "Cannot be near full" comments above.
                if( IndexHeldInLayer1AmpSuspense == MAX_SUSPENSE - 3 )
                {
                    // Restore the state which will receive the AMP suspense text.
                    Layer1State = Layer1AmpPushedState;
                    // Re-process non-valid-entity AMP-introduced text.
                    RedoTextHeldInLayer1AmpSuspense( );
                }
            }
            else
            {
                // Upon bad chars, re-process non-valid-entity AMP-introduced text.

                #if DO_DEBUG_ENTITY
                    ; Spew( L"Bad Character ending amp state" );
                #endif

                // Possible plan: accept entities without semicolon?
                // Progressively remove chars from end until a match
                // to some entity name, then reprocess the remnant.
                // That could be dangerous. I might just selectively
                // recognize frequent offenders: I see "&nbsp" a lot.
                // Don't do it here. Put it into common redo routine.

                // Add that first bad char to AMP-introduced suspense buffer.
                TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense] = inputChar;
                IndexHeldInLayer1AmpSuspense ++; // Cannot be near full - see default case.
                // Restore the state which will receive the AMP suspense text.
                Layer1State = Layer1AmpPushedState;
                // Re-process non-valid-entity AMP-introduced text.
                RedoTextHeldInLayer1AmpSuspense( );
            }
            break;
        }
        break;
    }
}

void CHtm::RedoTextHeldInLayer1AmpSuspense( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"160" );
    #endif

    // Possible plan: accept entities without semicolon?
    // Progressively remove chars from end until a match
    // to some entity name, then reprocess the remnant.
    // That could be dangerous. I might just selectively
    // recognize frequent offenders: I see "&nbsp" a lot.

    #if DO_DEBUG_ENTITY
        ; Spew( L"RedoTextHeldInLayer1AmpSuspense. BUFFER:" );
        TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense] = NULL;
        ; Spew( TextHeldInLayer1AmpSuspense );
    #endif

    // We already know TextHeldInLayer1AmpSuspense [0] == AMP.
    // But we must test count before testing multiple chars.
    // This is a post-increment. So at 5, could hold [0]-[4].
    if( IndexHeldInLayer1AmpSuspense > 4
    && ( TextHeldInLayer1AmpSuspense [1] | ' ' ) == 'n'
    && ( TextHeldInLayer1AmpSuspense [2] | ' ' ) == 'b'
    && ( TextHeldInLayer1AmpSuspense [3] | ' ' ) == 's'
    && ( TextHeldInLayer1AmpSuspense [4] | ' ' ) == 'p' )
    {
        // move down any other characters in suspense buffer
        // to make disappear just the [0]-[4] chars "&nbsp".

        int j = 5;
        for( ;; )
        {
            if( j == IndexHeldInLayer1AmpSuspense )
                break;
            TextHeldInLayer1AmpSuspense [j-4] = TextHeldInLayer1AmpSuspense [j];
            j++;
        }
        IndexHeldInLayer1AmpSuspense -= 5;
    }

    int i = 0;
    for( ;; )
    {
        if( i >= IndexHeldInLayer1AmpSuspense )
            break;

        int c = TextHeldInLayer1AmpSuspense [i];
        RedoCharHtmlParseLayer1( c );
        i++;
    }
    IndexHeldInLayer1AmpSuspense = 0;
}

void CHtm::RedoTextHeldInLayer1LabSuspense( )
{
    #if DO_DEBUG_CALLS
        Routine( L"161" );
    #endif
    // All callers due to a LAB-introduced tag
    // name recognition failure have set this:
    // Layer1State = LAYER1STATE_NOTHING;

    int i = 0;
    for( ;; )
    {
        if( i >= IndexHeldInLayer1LabSuspense )
            break;

        int c = TextHeldInLayer1LabSuspense [i];
        RedoCharHtmlParseLayer1( c );
        i++;
    }
    IndexHeldInLayer1LabSuspense = 0;
}

int CHtm::ValidTagNameInAccumulatedLabSuspense( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"162" );
    #endif
    // The buffer includes one first LAB. Oh... But not RAB yet.
    // "LAB" means Left-Angle-Bracket, which introduces HTML tags.

    // check assumption
    if( TextHeldInLayer1LabSuspense[0] != '<' )
    {
        ProgramError( L"ValidTagNameInAccumulatedLabSuspense: atop not LAB" );
        return 0;
    }

    // fix sz:
    TextHeldInLayer1LabSuspense [IndexHeldInLayer1LabSuspense] = NULL;

    if( IndexHeldInLayer1LabSuspense < 2 )
    {
        #if DO_DEBUG_HTMLTAGS
            ; Spew( L"ValidTagNameInAccumulatedLabSuspense: IndexHeldInLayer1LabSuspense < 2" );
        #endif
        return 0; // 0 = did not recognize a valid tag name.
    }

    if( IndexHeldInLayer1LabSuspense > MAX_TAG_OR_ATTR_CHARS )
    {
        #if DO_DEBUG_HTMLTAGS
            ; Spew( L"ValidTagNameInAccumulatedLabSuspense: IndexHeldInLayer1LabSuspense > MAX_TAG_OR_ATTR_CHARS" );
        #endif
        return 0; // 0 = did not recognize a valid tag name.
    }

    // Now that all <! are not automatically comments,
    // I had to add the following non-alpha name too:
    // '!' == 0x21, which is safe if I still OR 0x20.
    // TAG_DOCTYPE, L"!doctype",

    // Skip over any end-tag slash character to the first char:
    int AfterPunct = 1;
    if( TextHeldInLayer1LabSuspense [AfterPunct] == SLA )
        AfterPunct = 2;

    if( IndexHeldInLayer1LabSuspense <= AfterPunct )
    {
        #if DO_DEBUG_HTMLTAGS
            ; Spew( L"ValidTagNameInAccumulatedLabSuspense: IndexHeldInLayer1LabSuspense <= AfterPunct" );
        #endif
        return 0; // 0 = did not recognize a valid tag name.
    }

    // Changing to use a CSol for lookup.
    // First, do my ascii-type lowercase.
    // I better do it after copy instead.

    wchar_t wk[MAX_TAG_OR_ATTR_CHARS + 2];
    wchar_t * into = wk;
    wchar_t * from = TextHeldInLayer1LabSuspense + AfterPunct;
    for( ;; )
    {
        wchar_t c = *from++;
        if( c == NULL )
            break;
        *into++ = c | ' '; // safe for all ascii tag names and chars
        // Size checked above. Need no limit in loop.
    }
    *into = NULL;

    // I am failing at arithmetic today. Ok. I got it...
    IndexPastTagInLayer1Markup = into - wk; // to help ProcessTagAttributes
    IndexPastTagInLayer1Markup += AfterPunct; // adjust for LAB, any slash

    size_t index = CSolHtmlTagNames.Find( wk );
    #if DO_DEBUG_ADDFIND
        if( index == 1 )
            { Spew( L"AddFind 1 at chtm 1992" ); }
    #endif
    if( index > 1 )
    {
        wchar_t enumTag = CSolHtmlTagNames.GetUserValue( index );
        #if DO_DEBUG_HTMLTAGS
            ; SpewValue( wk, enumTag );
        #endif

        // transfer tag name LabSuspense to a longer whole-tag Markup buffer:
        wcscpy( TextHeldInLayer1Markup, TextHeldInLayer1LabSuspense );
        IndexHeldInLayer1Markup = IndexHeldInLayer1LabSuspense;

        // and remember the valid enumTag:
        TagEnumHeldInLayer1Markup = enumTag;

        return 1; // 1 = recognized valid tag, copied it to Markup buffer.
    }


    #if DO_DEBUG_HTMLTAGS || DO_SEE_NOVEL_TAGS
        ; Spew( L"ValidTagNameInAccumulatedLabSuspense: Tag not recognized:" );
        ; Spew( TextHeldInLayer1LabSuspense );
    #endif
    return 0; // 0 = did not recognize a valid tag name.
}

int CHtm::ValidEntityInAccumulatedAmpSuspense( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"163" );
    #endif
    // "AMP" means Ampersand, which introduces character "entities".

    // The first character in the Amp suspense buffer is the AMP.
    // The terminating semicolon has not been added to suspense.

    // Now that my Unicode liberal HTML parser calls iswalnum,
    // I cannot guarantee I have ascii alphas or numbers here.

    // fix sz:
    TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense] = NULL;

    int NewEntity = 0; // zero return means failure, no valid entity.

    // Hey, Bozo. Cannot test a character until tested fill count.
    // Oh, I can here, because there is a semicolon, no, null end.

    if( TextHeldInLayer1AmpSuspense [ 1 ] == '#' )
    {
        // Accept a numeric entity

        // Now that I am doing full Unicode, I will accept any values
        // between 0x0020 and 0xffff, also CR and LF; BUT NO CONTROLS!

        unsigned int NumberAccumulator = 0;
        int n = 0;

        if( ( TextHeldInLayer1AmpSuspense [ 2 ] | ' ' ) == 'x' )
        {
            // After "#x", accept a 2 or 4 hexadecimal digit numeric entity
            // Because of these examples in cache, allow 1 to 4 characters:
            // unrecognized entity&#xD; -- these are not 2 nor 4 wide.
            // unrecognized entity&#xA; -- these are not 2 nor 4 wide.

            int i = 3;
            for( ;; )
            {
                wchar_t c = TextHeldInLayer1AmpSuspense [ i ];
                if( c == 0 )
                    break;
                NumberAccumulator *= 16;

                // I THINK I can trust if( iswdigit( c ) ).
                // Of course, if( isdigit( c ) ) is unsafe.
                // Nevertheless, my math requires '0' - '9'.

                if( c >= '0' && c <= '9' )
                    NumberAccumulator += c - '0';
                else
                if( c >= 'a' && c <= 'f' )
                    NumberAccumulator += c - 'a' + 10;
                else
                if( c >= 'A' && c <= 'F' )
                    NumberAccumulator += c - 'A' + 10;
                else
                    n = 50; // fails
                i++;
                n++;
            }
        }
        else
        {
            // With '#' but no "#x" prefix, accept a 2 to 4 decimal digit numeric entity
            int i = 2;
            for( ;; )
            {
                wchar_t c = TextHeldInLayer1AmpSuspense [ i ];
                if( c == 0 )
                    break;
                NumberAccumulator *= 10;

                // I THINK I can trust if( iswdigit( c ) ).
                // Of course, if( isdigit( c ) ) is unsafe.
                // Nevertheless, my math requires '0' - '9'.

                if( c >= '0' && c <= '9' )
                    NumberAccumulator += c - '0';
                else
                    n = 50; // fails
                i++;
                n++;
            }
        }

        // Four digits max seemed good, but accept higher count to rid them.
        // Fifty was a bad-char failure flag.
        // To protect all later TEXT parsers and CSee,
        // reject NULLS and controls except CR and LF.

        // Need to accept TAB entity to rid it; but reject from output:
        // &#9;
        if( NumberAccumulator == 0x0009 )
            NumberAccumulator = ' ';

        if( n >= 1
        && n <= 8
        && NumberAccumulator >= 0x0020 )
        {
            #if DO_DEBUG_ENTITY
                ; SpewValue( L"VALID NUMERIC ENTITY: ", NumberAccumulator );
            #endif
            return NumberAccumulator; // a valid entity.
        }
    }
    else
    {
        // Without a '#' prefix, accept a valid named entity

        // To protect all later TEXT parsers and CSee,
        // reject NULLS and controls except CR and LF.
        // That is built into the table, lacking such.

        // After seeing Case-sensitive entity names, I made
        // both a case-sensitive Bc list, and another to try,
        // Lc, all lowercased. Those CSols do not include the
        // leading '&' nor the final ';', so I adjust limits:

        // overwrite semicolon: No! Debugging shows it has no semicolon.
        // TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense - 1] = NULL;

        // This BC in name means BOTH CASES were preserved:

        size_t index = CSolHtmlBcEntities.Find( TextHeldInLayer1AmpSuspense + 1 ); // &<verbatim>;
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at chtm 1992" ); }
        #endif
        if( index > 1 )
        {
            wchar_t EntityValue = CSolHtmlBcEntities.GetUserValue( index );
            #if DO_DEBUG_ENTITY
                ; SpewValue( L"VALID EXACTCASE ENTITY: ", EntityValue );
            #endif
            return EntityValue;
        }

        // Try again after lowercasing, in case web author
        // was unaware of case sensitivity in entity names.

        if( IndexHeldInLayer1AmpSuspense < 20 ) // sanity check
        {
            wchar_t wk[21]; // valid matches are way under 20 max.
            wchar_t * into = wk;
            wchar_t * from = TextHeldInLayer1AmpSuspense + 1;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                    break;

                // Only USASCII match valid names, but UCS in input.

                if( c >= 'A' && c <= 'Z' )
                    *into++ = c | ' '; // Lc = lowercase
                else
                    *into++ = c; // even useless UCS values
            }
            *into = NULL;

            // This LC in name means LOWERCASED for two lookup tries:

            index = CSolHtmlLcEntities.Find( wk ); // &<lowercased>;
            #if DO_DEBUG_ADDFIND
                if( index == 1 )
                    { Spew( L"AddFind 1 at chtm 2019" ); }
            #endif
            if( index > 1 )
            {
                wchar_t EntityValue = CSolHtmlLcEntities.GetUserValue( index );
                #if DO_DEBUG_ENTITY
                    ; SpewValue( L"VALID LOWERCASED ENTITY: ", EntityValue );
                #endif
                return EntityValue;
            }
        }

        #if DO_DEBUG_ENTITY
            ; Spew( TextHeldInLayer1AmpSuspense );
        #endif

        // since we did not find name, restore the semicolon:
        // No! Debugging shows it has no semicolon. Don't unfix NULL!
        // TextHeldInLayer1AmpSuspense [IndexHeldInLayer1AmpSuspense - 1] = ';';
    }

    return 0; // Not a valid entity. Caller to push back suspense buffer.
}

void CHtm::ProcessWholeMarkupTag( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"164" );
    #endif
    // Caller has already matched the 1+ character tag name.
    // First char is always '<'.
    // Second char would be '/' for end-tags.
    // Final char is always '>'.
    // I should expect XML form: <tagname ..... />.

    // Between <SCRIPT> .... </SCRIPT> is TOO UGLY!
    // Caller has already verified:
    // if( ! BetweenHtmlScriptTags
    // || TagEnumHeldInLayer1Markup == TAG_SCRIPT )

    // In the case of xml-ish tags like "<o:p>",
    // wherein ( ColonCountInLabSuspense != 0 ),
    // I idled through them, will not call here.

    // fix sz:
    TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = NULL;

    #if DO_DEBUG_HTMLTAGS
        ; Spew( L"ProcessWholeMarkupTag received this markup buffer:" );
        ; Spew( TextHeldInLayer1Markup );
    #endif

    // My more diligent than me tag processor caught parse errors.
    // Let me be the first to know about parsing algorithm errors.

    if( IndexHeldInLayer1Markup < 2 ) // for LAB, RAB
    {
        ProgramError( L"ProcessWholeMarkupTag: size < min" );
        return;
    }

    if( IndexHeldInLayer1Markup > MAX_MARKUP_TEXT )
    {
        ProgramError( L"ProcessWholeMarkupTag: size > max" );
        return;
    }

    if( TextHeldInLayer1Markup[0] != '<' )
    {
        ProgramError( L"ProcessWholeMarkupTag: atop not LAB" );
        return;
    }

    if( TextHeldInLayer1Markup[IndexHeldInLayer1Markup - 1] != '>' )
    {
        ProgramError( L"ProcessWholeMarkupTag: last not RAB" );
        return;
    }

    // New code to access TagNames, or something else, violated memory.
    if( TagEnumHeldInLayer1Markup <= TAG_ZERO )
    {
        ; SpewTwo( L"TextHeldInLayer1Markup", TextHeldInLayer1Markup );
        ; SpewValue( L"TagEnumHeldInLayer1Markup", TagEnumHeldInLayer1Markup );
        ProgramError( L"ProcessWholeMarkupTag: TagEnumHeldInLayer1Markup <= TAG_ZERO" );
        return;
    }
    if( TagEnumHeldInLayer1Markup >= TAG_PAST_SENTINEL )
    {
        ; SpewTwo( L"TextHeldInLayer1Markup", TextHeldInLayer1Markup );
        ; SpewValue( L"TagEnumHeldInLayer1Markup", TagEnumHeldInLayer1Markup );
        ProgramError( L"ProcessWholeMarkupTag: TagEnumHeldInLayer1Markup >= TAG_PAST_SENTINEL" );
        return;
    }

    if( m_PageIsAQrp )
    {
        // For every TAG, format something like one of these two lines:
        // ( good/bad url has )anchor tag x
        // ( good/bad url has )prior/next tag x
        // I will clone this code to suffix attr y, later, value "z".

        // For starters, lets omit end tags.
        // Or I can do them, and prefix '/'.

        int EndTag = ( TextHeldInLayer1Markup [1] == '/' );
        int InTagA = ( TagEnumHeldInLayer1Markup == TAG_A );

        // separate the block of fact lines containing URL and anchor clues
        if( m_AppendPageScrapingFacts
        && InTagA
        && ! EndTag )
            m_pWsbQrpFacts->Add( L"\r\n" );

        // Output the line with TAG, but no ATTR nor value yet.

        // Don't output a TAG_A fact ( but later URL, maybe attributes )

        if( ! InTagA )
        {
            wchar_t wk[100]; // The longest TAG is 10, ATTR is 17

            // Nothing like crash / stack overflow to spoil your day
            #if DO_DEBUG_SCRAPING
                ; SpewValue( L"AddQFact for TAG enum", TagEnumHeldInLayer1Markup );
                ; SpewTwo( L"AddQFact for TAG name", TagNames[ TagEnumHeldInLayer1Markup ].wcp );
            #endif

            wsprintf( wk, L"TAG %s%s",
                ( EndTag ? L"/" : L"" ),
                TagNames[ TagEnumHeldInLayer1Markup ].wcp );

            AddOrTestQueryFact( WHERENESS_TREBLE, wk );
        }
    }

    int xmlishTag = 0;
    if( TextHeldInLayer1Markup [IndexHeldInLayer1Markup - 2] == '/' )
        xmlishTag = 1;

    // Discussion of Quoting:
    //
    // See these ideas applied down in ProcessTagAttributes...
    //
    // Oh! this goes against my simplistic parse of ">" ending any tag:
    // <SCRIPT LANGUAGE='JavaScript' " + "DEFER SRC=http" + "://" + "a248.e.akamai.net/7/836/1132/102403/"+ if_Site_ID +".247realmedia.com/scripts/" + if_Site_ID + "_if.js" + ">
    //
    // How will I parse embedded DQT? There is such a panoply to consider. E.g.,
    // <a class="blogRollLinkStyle" href="http://www.blackberry.com/%20target="_blank"">
    //
    // Also, many script tags contain embedded > inside quotes. Do What?
    //
    // I see a lot of \" in the unprocessable tags: I handled that case.
    // Well. I let BSL quote either SQT or DQT in order to not end string.
    // But that is way not the right answer...
    //
    // These problems arise because I try to HTML-parse between SCRIPT tags:
    // I will have to cease such processing until I parse an END SCRIPT tag:
    //     <script>
    // w( " |
    //     <a href=\"javascript:void( zpu( '','http://clk.about.com/?zi=11/1TH/Wp&sdn='+ch+'_'+gs+'&u='+zE( 'http://camping.about.com/b/a/060943.htm' )+'&t='+zE( 'Camping%20Tip%20of%20the%20Week' ),700,400,'del' ) );\">
    // add to del.icio.us
    //     </a>
    // " );
    //     </script>
    //
    // Even doing that, the following cases of \" appear:
    //
    // This one is an actual BSL desired in output:
    // <a href="/wiki/%5C" title="\">
    //
    // This one is a directory path, actually ending in BSL:
    // <input type="hidden" name="ct" value="D:\Websites\du-khaleej\www\data\" ID="Hidden4">
    //
    // These ( and SQT too! ) show a bizarre 2-char sequence used to be the quoter:
    // <img src=http://www.aikidolink.ch/themes/AikidoLink/images/LogoLeft.gif alt=\"Willkommen bei $sitename\" title=\"Willkommen bei $sitename\" border=\"0\">
    // <a href=http://www.aikido-quedlinburg.de/sites/Aikido/Aikido2.html target=\"_blank\">
    //
    // So, rather than allow BSL to escape inside a string,
    // BSL SQT, or BSL DQT as alternate surrounding quotes.

    // ===========================================================
    // first, consider only those things that affect text layer 2:
    // ===========================================================

    // Only a few tags do not interrupt a word or a paragraph.
    // All tags not cased in this switch will end a paragraph.
    // Most tags listed here ( first half ) do not end anything.
    // A few tags listed here ( second half ) do flush any word.

    switch( TagEnumHeldInLayer1Markup )
    {
        // All tags not cased in this switch will end a paragraph,
        // by falling into default. No. That is a wasteful method.
        // Let default do nothing. List all tags to end paragraph.

        default:
            // end any current word, end line, end any paragraph.
            ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
            EndingTagAOffsetInText = -1; // disarm </a><a ...> recognition
            break;


        case TAG_HTML:
            // Add File/Folder set this bit to parse plain text files:
            BitMaskReasonsToHonorNewlines &= ~ ( 1 << 1 ); // end assumption: plain text
            break;



        // Most tags listed here ( first half ) do not end anything.
        // These represent fonts, italic, or emphasis change, etc.
        // In fact, these have no place in my thinking for output.
        // It was important to bring TAG_B back here, because the
        // BOLD attribute often separated a word from final period.
        // I moved STRONG here because it appears in some URL text.
        // I moved Italics tag from here to there; See what's best:
        // No, leave it here. Some authors just need to put spaces.
        case TAG_I:
        case TAG_B:
        case TAG_BIG:
        case TAG_BLINK:
        case TAG_CODE:
        case TAG_DEL:
        case TAG_EM:
        case TAG_EMBED:
        case TAG_FONT:
        case TAG_INS:
        case TAG_KBD:
        case TAG_Q: // Q? what means Q?
        case TAG_S:
        case TAG_SMALL:
        case TAG_STRIKE:
        case TAG_STRONG:
        case TAG_SUB:
        case TAG_SUP:
        case TAG_TT:
        case TAG_U:
        case TAG_VAR:
        case TAG_SPAN:
            // do not end a current word, nor a paragraph.
            break;

        // Still first-half, But I want to see if these newly added
        // tags were recognized or not...
        case TAG_EIF:
        case TAG_EENDIF:
            #if DO_DEBUG_IF_ENDIF
                Spew( L"I see your new HTML tag !IF or !ENDIF." );
            #endif
            // do not end a current word, nor a paragraph.
            break;


        // I still needed to distinguish between:
        //     1. ending a token, and
        //     2. inserting a space.
        // For example, this /a should not cause a space before comma:
        // ...school</a>,
        // That is a frequent example, so </a> MUST NOT end a token.

        // However, I see a case of continuous lists of links that
        // have no token breakers between them, like this one:
        // Advice for BeginnersCampground DirectoriesCampground Reviews
        // So add a new rule to insert token break upon </a><a>.
        // That is a very perspicacious rule, but end the block.
        // That was too hard to diff; try end a line just to diff.
        // I see this will proliferate small blocks, and not take
        // the first or last link of the set out of it's context.
        // Let me instead insert a fake token "|" as I see in many
        // web pages, which may help a later text analysis feature.

        case TAG_A:
            if( TextHeldInLayer1Markup [1] == '/' )
            {
                // ending tag A:

                // The comments above show great growth, and yet,
                // a simple case adds space that ruins possesive:
                // <a href="http://...">Name</a>'s
                //
                // Yet I want to keep splitting up words, like:
                // <a href="http://...">Two</a>Words

                // New way, keep both:
                if( IndexHeldInLayer2TokenText > 0
                && iswalpha( TextHeldInLayer2TokenText [ IndexHeldInLayer2TokenText - 1 ] )
                && ! TagA_Had_Name )
                    InsertSpaceIfSayAlphaComes = 1;

                // Old way, kept way:
                ProcessTextTokenBuffer( 0 ); // put any word; but not newline, newblock.

                // set this to adjust ending half

                EndingTagAOffsetInText = m_pWsbPaperText->StrLen; // arm </a><a ...> recognition
            }
            else
            {
                // opening tag A:

                // This distinction was vital to clean up text,
                // because some generators put NAME anchors in
                // without regard to the text token boundaries.
                TagA_Had_Name = 0; // clear until see attribute
                TagA_Had_Href = 0; // clear until see attribute

                // Perform adjacent anchor </a><a ...> recognition:
                if( EndingTagAOffsetInText == ( int ) m_pWsbPaperText->StrLen
                && IndexHeldInLayer2TokenText == 0 )
                {
                    // Not perfect yet. Adds too many "|" when not needed.
                    // More rules. Almost there. Now get spaces around it.
                    PendingInsertSpaceChar = 1;
                    if( BitMaskReasonsToDiscardText == 0 )
                        AddToPlainText( L"|", 1, 0 ); // put this word; but not newline, newblock.
                    // Afterwards, leave this clue, to space before any next token.
                    PendingInsertSpaceChar = 1;
                }
                else
                {
                    ProcessTextTokenBuffer( 0 ); // put any word; but not newline, newblock.
                }
                EndingTagAOffsetInText = -1; // disarm </a><a ...> recognition
            }
            break;


        // A few tags listed here ( second half ) do flush any word.
        // These often surround nice title or anchor or citations.
        // Table data TAG_TD is listed lest it flush a whole line.
        // I took away STRONG because it appears in some URL text.

        case TAG_CITE:
        case TAG_ABBR:
        case TAG_ACRONYM:
        case TAG_EMPH:
        case TAG_IMAGE:
        case TAG_IMG:

        // I have seen a web page that puts SPAN around letters,
        // causing it here to break up words. I will get a cache
        // first leaving it here, and then removing it from here,
        // which means, moving it from second half to first half,
        // and see how it affects the saved files.
        // Well, moving it has given more benefits than deficits.
        // case TAG_SPAN:

        case TAG_TD:
        case TAG_TITLE:
        // Overuse of <BR> for format makes me ignore it for block.
        case TAG_BR:

        // I have this case where <DIV> periodically breaks up text.
        // However, ignoring may be worse than keeping DIV as block end.
        // Yes, adding DIV here caused this whole text to run together:
        // http://www.upstreamca.org/ahab.html
        // case TAG_DIV:

            ProcessTextTokenBuffer( 0 ); // put any word; but not newline, newblock.
            // Afterwards, leave this clue, to space before any next token.
            PendingInsertSpaceChar = 1;
            EndingTagAOffsetInText = -1; // disarm </a><a ...> recognition
            break;


        // A chimera: the "third" half. Flush if in line.
        // These are necessary to make indentations work.
        case TAG_BLOCKQUOTE:
        case TAG_UL:
        case TAG_OL:
        case TAG_DL:
        case TAG_DT:
        case TAG_DD:
            ProcessTextTokenBuffer( 1 ); // put any word, newline; but not newblock.
            EndingTagAOffsetInText = -1; // disarm </a><a ...> recognition
            break;

    }


    // ===========================================================
    // In this switch pair, process tags to affect the text layer:
    // ( beyond what earlier switch did to partition text layer ).
    // ===========================================================


    // Since Attributes are kind of self-driving, and output ALT or TITLE text,
    // process any opening <A...> before them, to get good block text analysis.
    // In fact, all this tag name processing will precede Attribute processing.

    if( TextHeldInLayer1Markup [1] != '/' )
    {
        // In this IF clause,
        // process a starting or standalone tag

        switch( TagEnumHeldInLayer1Markup )
        {
        case TAG_APPLET:
            if( ! xmlishTag )
            {
                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to APPLET" );
                #endif
                BitMaskReasonsToDiscardText |= ( 1 << 1 );
            }
            break;
        case TAG_OBJECT:
            if( ! xmlishTag )
            {
                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to OBJECT" );
                #endif
                BitMaskReasonsToDiscardText |= ( 1 << 2 );
            }
            break;
        case TAG_SCRIPT:
            if( ! xmlishTag )
            {
                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to SCRIPT" );
                #endif
                BitMaskReasonsToDiscardText |= ( 1 << 3 );
                BetweenHtmlScriptTags = 1; // gets UGLY
            }
            break;
        case TAG_STYLE:
            if( ! xmlishTag )
            {
                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to STYLE" );
                #endif
                BitMaskReasonsToDiscardText |= ( 1 << 4 );
            }
            break;
        case TAG_HEAD:
            if( ! xmlishTag )
            {
                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to HEAD" );
                #endif
                BitMaskReasonsToDiscardText |= ( 1 << 5 );
            }
            break;
        case TAG_XML:
            if( ! xmlishTag )
            {
                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to XML" );
                #endif
                BitMaskReasonsToDiscardText |= ( 1 << 6 );
            }
            break;

        case TAG_TITLE:
            if( ! xmlishTag )
            {
                BetweenHtmlTitleTags = 1;
            }
            break;

        case TAG_IMAGE:
        case TAG_IMG:
            // Employ attribute ALT=TEXT to add to the output text.

            // Images also appear in forms instead of submit buttons.
            // No, that case would appear as INPUT tag, type="image".
            break;

        case TAG_PRE:
        case TAG_VERBATIM:
            BitMaskReasonsToHonorNewlines |= ( 1 << 2 );
            // It is ugly to coerce plain text wrapped to 80 into 60 cols:
            PreferredWrapColumnInOutputText = MAX_WRAP_COLUMN;
            break;

        case TAG_QXML:
            // Input contains ?xml tag.
            // I forgot why this once interested me.
            break;

        case TAG_BLOCKQUOTE:
        case TAG_UL:
        case TAG_OL:
        case TAG_DL:
        case TAG_DT:
        case TAG_DD:
            ChangeTextIndentation( );
            break;

        case TAG_FORM:
            if( ! xmlishTag )
            {
                // I have huge pages leaving no text, because the text
                // is all inside a FORM. E.g., at the Pantone web site.

                #if DO_DEBUG_DISCARD
                    ; Spew( L"Start Discarding due to FORM" );
                #endif

                // I must consider the effect of not gating text on FORM:
                // Add two greppable tokens to output, then trawl cache:
                // ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
                // AddToPlainText( L"[START_OF_FORM]", 15, 2 ); // put this word; newline, newblock.
                // No, I decided all that was lost was signage:
                BitMaskReasonsToDiscardText |= ( 1 << 7 );
                // I do like the idea of marking text for form:
                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
                AddToPlainText( L"[FORM]", 6, 2 ); // put this word; newline, newblock.
            }
            // Analyzing the attributes on FORM tag, I find only 2 of interest:
            // 162 QZ? method on form =  get
            //   4 QZ? method on form =  Post ( any capitalization )
            //  70 QZ? action on form =  /p/search ( relative or abs url )
            //
            // Between the open and closing TAG_FORMs, I counted tags of interest:
            // 2894 Tag option
            // 2624 Tag input
            //  184 Tag select
            //
            // Forms also contain tables, anchors, images, of no special interest.
            // How are these tags used in forms? Any of interest?
            //  190 Tag label ?
            //   11 Tag textarea ?
            //

            // Here is a typical sequence of tags used in a form:
            // <base href="http://www.waste.org/pynchon-l/">
            // <form method="post" action="http://waste.org/mail/?list=pynchon-l">
            // <input type="hidden" name="list" value="pynchon-l">
            // <input name="keywords" value="" size=40> --?so text=default type?
            // <input type="submit" value="Search">
            // </form>
            // 1. Get the submit url from "action" attribute.
            // 2. Make something to hold other tag type data.
            // For simplicity, only ask for default settings.
            // Init some variables of only local-relevance, until /Form:

            // doing TAG_FORM1 ( 1=start open tag, 2=end open tag, 3=close tag )

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( L"========= NEW START OF FORM ========:" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif

            m_DuringTagForm = 1; // enables attribute work using:
            m_BetweenOpenCloseTagForm = 1;
            m_FormActionPresent = 0; // initialize locally relevant variable
            m_FormMethodUrlIndex = 0; // initialize locally relevant variable
            m_FormGet1Post2 = 0; // initialize locally relevant variable
            m_FormInputTypeTextCount = 0; // initialize locally relevant variable
            m_FormInputTypeSubmitCount = 0; // initialize locally relevant variable
            if( m_FormInputTypeTextName != NULL )
            {
                MyFree( 1689, UNPREDICTABLE, m_FormInputTypeTextName );
                m_FormInputTypeTextName = NULL; // initialize locally relevant variable
            }
            break;


        case TAG_SELECT:
            // doing TAG_SELECT1 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            // during forms, extract SELECT attributes to obtain form knowledge.
            // Just keep the OPTION withing SELECT that is "selected", like:
            // CHtm: SELECT tag:
            // <select  name=search_source>
            // CHtm: OPTION tag:
            // <option  value="WEB" selected>
            // CHtm: OPTION tag:
            // <option  value="B000813UAS" >
            // CHtm: OPTION tag:
            // <option  value="B000813UTE" >
            // CHtm: OPTION tag:
            // <option  value="B0007WF880" >

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif
            m_DuringTagSelect = 1; // enables attribute work using:
            if( m_FormSelectName != NULL )
            {
                MyFree( 1723, UNPREDICTABLE, m_FormSelectName );
                m_FormSelectName = NULL;  // initialize locally relevant variable
            }
            m_BetweenOpenCloseTagSelect = 1;
            break;


        case TAG_TEXTAREA:
            // doing TAG_TEXTAREA1 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            // during forms, extract TEXTAREA attributes to obtain form knowledge.
            // It appears that textarea is an alternate type of form text input field:
            // One seems to have come from a bugzilla page. Ignore them, few, for now.
            // Another one is for submission of comments. ( or earthlink's write email ).
            // Note the fact of a TEXTAREA to disqualify a FORM as a search engine.
            //
            // Tag textarea
            // name on textarea =  comment
            // id on textarea =  comment
            // rows on textarea =  10
            // cols on textarea =  80
            // accesskey on textarea =  c
            // Tag /textarea

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif
            m_FormRejectedBitfield |= FORM_REJECT_HAS_TEXTAREA;
            break;


        case TAG_OPTION:
            // doing TAG_OPTION1 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            if( ! m_BetweenOpenCloseTagSelect )
                break;

            // during forms, extract OPTION attributes to obtain form knowledge.
            // Text processing will improve when I use OPTION tag to eliminate text.
            // However, the clear text must be saved when I let user choose options.
            // e.g., This block of only one word was inside a form option tag pair:
            //
            //  Zimbabwe
            //
            // Tag /option
            // Tag option
            // value on option =  Zimbabwe
            // Nonsense: Zimbabwe

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif
            m_DuringTagOption = 1; // enables attribute work using:
            if( m_FormOptionValue != NULL )
            {
                MyFree( 1787, UNPREDICTABLE, m_FormOptionValue );
                m_FormOptionValue = NULL;  // initialize locally relevant variable
            }
            m_FormOptionSelected = 0; // initialize locally relevant variable

            break;



        case TAG_LABEL:
            // doing TAG_LABEL1 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            // The LABEL tag may be interesting also when presenting user choices.
            // It seems to come before or after the id-associated input feature.
            // I read a LABEL tag is used by JavaScript to refer to form controls.
            // So I don't think I need to consider LABEL tags.
            //
            // case 1:
            // Tag input
            // type on input =  checkbox
            // name on input =  rf
            // id on input =  regionfilt
            // value on input =  1
            // Tag label
            // for on label =  regionfilt
            // Nonsense: Only from Australia
            //
            //  Web Results
            //
            // Tag /label
            //
            // case 2:
            // Tag label
            // for on label =  sTxt
            // Nonsense: Search
            //
            //  [Go]
            //
            // Tag /label
            // Tag input
            // type on input =  text
            // name on input =  terms
            // value on input =  revelation
            // id on input =  sTxt

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif
            break;

        case TAG_INPUT:
            // doing TAG_INPUT1 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            // during forms, extract INPUT attributes to obtain form knowledge.
            //
            // e.g.,
            // Tag input -- This is just a button, and how to label it. Ignore.
            // type on input =  submit
            // value on input =  Search
            //
            // Tag input
            // type on input =  hidden
            // name on input =  filter
            // value on input =  0
            //
            // Tag input
            // name on input =  q
            // type on input =  text
            // id on input =  q
            // size on input =  12
            //
            // Tag input -- Is this an image-map kind of thing? -- which also study.
            // name on input =  imageField
            // type on input =  image
            // src on input =  http://images.contactmusic.com/images/general/goxsmall.gif
            // align on input =  top
            // width on input =  27
            // height on input =  29
            // border on input =  0

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif

            m_DuringTagInput = 1; // enables attribute work to malloc ptrs:
            m_FormInputType = ENUM_INPUT_TYPE_UNSTATED; // initialize locally relevant variable
            if( m_FormInputName != NULL )
            {
                MyFree( 1877, UNPREDICTABLE, m_FormInputName );
                m_FormInputName = NULL; // initialize locally relevant variable
            }
            if( m_FormInputValue != NULL )
            {
                MyFree( 1877, UNPREDICTABLE, m_FormInputValue );
                m_FormInputValue = NULL; // initialize locally relevant variable
            }
            m_FormCheckBoxChecked = 0; // initialize locally relevant variable
            m_FormRadioChecked = 0; // initialize locally relevant variable

            break;

        }
    }
    else
    {
        // In this ELSE clause,
        // process an ending tag

        switch( TagEnumHeldInLayer1Markup )
        {
        case TAG_APPLET: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to APPLET" );
            #endif
            BitMaskReasonsToDiscardText &= ~ ( 1 << 1 );
            break;
        case TAG_OBJECT: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to OBJECT" );
            #endif
            BitMaskReasonsToDiscardText &= ~ ( 1 << 2 );
            break;
        case TAG_SCRIPT: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to SCRIPT" );
            #endif
            BitMaskReasonsToDiscardText &= ~ ( 1 << 3 );
            BetweenHtmlScriptTags = 0; // cancel UGLY
            break;
        case TAG_STYLE: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to STYLE" );
            #endif
            BitMaskReasonsToDiscardText &= ~ ( 1 << 4 );
            break;
        case TAG_HEAD: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to HEAD" );
            #endif
            BitMaskReasonsToDiscardText &= ~ ( 1 << 5 );
            break;
        case TAG_XML: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to XML" );
            #endif
            BitMaskReasonsToDiscardText &= ~ ( 1 << 6 );
            break;

        case TAG_TITLE: // ending
            BetweenHtmlTitleTags = 0;
            break;

        case TAG_PRE: // ending
        case TAG_VERBATIM: // ending
            BitMaskReasonsToHonorNewlines &= ~ ( 1 << 2 );
            PreferredWrapColumnInOutputText = LOW_WRAP_COLUMN;
            break;

        case TAG_BLOCKQUOTE: // ending
        case TAG_UL: // ending
        case TAG_OL: // ending
        case TAG_DL: // ending
            RestoreTextIndentation( );
            break;

        case TAG_FORM: // ending
            #if DO_DEBUG_DISCARD
                ; Spew( L"Stop Discarding due to FORM" );
            #endif

            // I must consider the effect of not gating text on FORM:
            // Add two greppable tokens to output, then trawl cache:
            // ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
            // AddToPlainText( L"[END_OF_FORM]", 13, 2 ); // put this word; newline, newblock.
            // No, I decided all that was lost was signage:
            BitMaskReasonsToDiscardText &= ~ ( 1 << 7 );

            // doing TAG_FORM3 ( 1=start open tag, 2=end open tag, 3=close tag )

            #if DO_DEBUG_FORM0
                ; Spew( L"" );
                ; Spew( L"END OF FORM" );
                ; Spew( TextHeldInLayer1Markup );
                ; Spew( L"" );
            #endif

            if( ! m_BetweenOpenCloseTagForm ) // guarantees mallocs made etc.
                break;

            // Now assemble all the Chtm members that were set
            // due to, and since, the last opening <form> tag.
            m_BetweenOpenCloseTagForm = 0;

            if( m_FormInputTypeTextCount == 0 )
            {
                m_FormRejectedBitfield |= FORM_REJECT_NO_TEXT_INPUT;
            }
            if( m_FormInputTypeTextCount > 1 )
            {
                m_FormRejectedBitfield |= FORM_REJECT_MULTIPLE_TEXT_INPUT;
            }

            if( m_FormInputTypeSubmitCount == 0 )
            {
                m_FormRejectedBitfield |= FORM_REJECT_NO_SUBMIT_IMAGE_BUTTON;
            }
            if( m_FormInputTypeSubmitCount > 1 )
            {
                m_FormRejectedBitfield |= FORM_REJECT_MULTIPLE_SUBMIT_IMAGE_BUTTON;
            }

            // Only create representative form example URL if not rejected.
            if( m_FormRejectedBitfield != 0 )
            {
                // Transfer a rejection from this FORM instance
                // to the top OneForm fruit for this ACTION URL.
                if( m_FormMethodUrlIndex != 0 )
                {
                    COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                    if( pOneForm != NULL )
                    {
                        pOneForm->FormRejectedBitfield |= m_FormRejectedBitfield;
                    }
                }
            }

            // However, output a few more flawed form data w/o specimens.

            // Gate off info for some surely-non-search form conditions.
            if( ( m_FormRejectedBitfield &
            ( FORM_REJECT_NO_TEXT_INPUT
            | FORM_REJECT_MULTIPLE_TEXT_INPUT
            | FORM_REJECT_HAS_TEXTAREA
            | FORM_REJECT_PASSWORD_INPUT
            | FORM_REJECT_FILE_INPUT ) ) != 0 )
            {
                // WayBadForm
            }
            else
            {

                #if DO_FORMS_DUMP
                    // Add mention of this tag in the cleartext to grep later.
                    // I should allow to save the binary file, or at least its
                    // cache filename or URL for later recovery... That's what!
                    // Set a bool so add cache will output such information.
                    g_CHtmFoundAForm = 1;
                #endif

                // I add the InputTypeTextName last, ready to append query.
                // I might accept an unnamed TEXT input field? Ok. Nonsense!
                // Good search form examples have only one INPUT TEXT type.
                // Append TEXT name w/o value to the form example URL last:

                if( m_FormParamCount == 0 ) // controls '?' or '&' introducer
                    m_pWsbFormParts->Add( L"?" );
                else
                    m_pWsbFormParts->Add( L"&" );

                if( m_FormInputTypeTextName != NULL )
                    m_pWsbFormParts->Add( m_FormInputTypeTextName );
                m_pWsbFormParts->Add( L"=" );

                // Free the following malloc if set by INPUT tag work:
                if( m_FormInputTypeTextName != NULL )
                {
                    MyFree( 2073, UNPREDICTABLE, m_FormInputTypeTextName );
                    m_FormInputTypeTextName = NULL;
                }

                // This is the thing that collected a ready-to-use URL.
                // Archive it or whatever, and throw away the temp WSB.
                if( m_pWsbFormParts != NULL )
                {
                    size_t nMalBuf = 0;
                    wchar_t * pMalBuf = m_pWsbFormParts->GetBuffer( & nMalBuf ); // a malloc, user frees
                    if( pMalBuf != NULL )
                    {
                        // Remember the completed form example URL:
                        #if DO_DEBUG_FORM2
                            ; Spew( L"COMPLETED SEARCH ENGINE QUERY URL:" );
                            ; Spew( pMalBuf );
                        #endif

                        // Append assembled example URLs to the OneForm.
                        // Use a CSOL to hold more than one example URL.
                        // None are enabled until user's save/edit/load.
                        // N.B. All accessers must look up the pOneForm.

                        if( m_FormMethodUrlIndex != 0 )
                        {
                            COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                            if( pOneForm != NULL )
                            {
                                pOneForm->pSolFormSpecies->AddKey( pMalBuf );
                            }
                        }

                        MyFree( 1965, UNPREDICTABLE, pMalBuf );
                    }
                    delete m_pWsbFormParts;
                    m_pWsbFormParts = NULL;
                }
            }

            #if DO_DEBUG_FORM1
                ; Spew( L"CHtm: Reached end of FORM =========" );
            #endif
            break;

        case TAG_SELECT: // ending
            // doing TAG_SELECT3 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            m_BetweenOpenCloseTagSelect = 0;

            // Now totalize any selected OPTION value. -- No,
            // I started to add a new variable, but the OPTION tag
            // has the SELECT name, so it can do the work directly:
            // m_FormSelectedOptionValue

            #if DO_DEBUG_FORM1
                ; Spew( L"CHtm: Reached end of SELECT" );
            #endif

            // Having finished the SELECT - /SELECT form section,
            // Free the following mallocs if set by attribute work:
            if( m_FormSelectName != NULL )
            {
                MyFree( 2102, UNPREDICTABLE, m_FormSelectName );
                m_FormSelectName = NULL;
            }

            break;

        }
    }


    // ===========================================================
    // In this switch pair, process tags to affect knowledge of links:
    // They may also affect the comprehension of a current text block.
    // Also in this switch pair, tag names affect ProcessTagAttributes.
    // ===========================================================

    if( TextHeldInLayer1Markup [1] != '/' )
    {
        // In this IF clause,
        // process a starting or standalone tag

        switch( TagEnumHeldInLayer1Markup )
        {

        case TAG_META:
            // I think is is fair to assume that nobody would put the
            // ATTR_CONTENT before the ATTR_HTTP_EQUIV, so I will let
            // ATTR_HTTP_EQUIV arm a bool to permit ATTR_CONTENT work.
            // That bool should be cleared at the Tag level switch by
            // TAG_META recognition before doing ProcessTagAttributes.
            // In ProcessTagAttributes, ATTR_CONTENT tests ( TAG_META ).
            // This bool has only duration-within-TAG_META relevance:
            Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentType = 0;
            Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentLanguage = 0;
            Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_Refresh = 0;
            break;

        case TAG_TITLE:
            if( ! xmlishTag )
            {
                InitializeTitleOrAnchorTextBuffer( );
            }
            break;

        case TAG_A:
            if( ! xmlishTag )
            {
                // The TAG_A case above already emitted any token
                // which was ended by this Starting TAG_A markup.

                // I do not keep anchor text any more. So do not:
                // Yes, restoring this for Link cross references.
                InitializeTitleOrAnchorTextBuffer( );

                // I might reposition it on first letter of a token:
                ArmToSaveStartAnchorOffset = 1;
                // But, just in case it gets no better, set it here:
                StartAnchorOffsetInText = m_pWsbPaperText->StrLen;

                PriorTokenUnder50[0] = NULL; // Start of A prevents "two tokens"
                PriorTokenIsNumberEnum = NO_NUMBER;

                // ( for QRP: ) We are starting a TAG_A.
                // We have not even seen it's URL yet.
                //
                // We don't know if it is head or tail.
                //
                // I cannot do anything yet, except to
                // save a copy of old LastTagA_UrlIndex,
                // which will get changed before TAG_A ends,
                // where I will lade all the real work.

                // The first time coming here is what will will set
                // UrlIndexOfAnyPriorTagA to -1, because the parser
                // initialization has set LastTagA_UrlIndex to -1.

                // Not so fast! I see invalid 0 during page scraping.
                // I imagine it is due to parsing A + NAME w/o HREF.
                // So do not 'AGE' this URL from B to A yet,
                // merely safe a copy ( because HREF attrib changes )
                // and decide whether to age or restore at end tag.
                // UrlIndexOfAnyPriorTagA = LastTagA_UrlIndex;
                SavedAtEntryToTagA = LastTagA_UrlIndex;


            }
            break;
        }
    }
    else
    {
        // In this ELSE clause,
        // process an ending tag

        switch( TagEnumHeldInLayer1Markup )
        {

        case TAG_TITLE: // ending
            {
                // CHtm does not have access to pOnePaper.
                // CHtm user can examine my member after,
                // and copy it to same COnePaper member:

                FinalizeTitleOrAnchorTextBuffer( );
                // which left szTitleOrAnchorBuffer filled,
                // of size TitleOrAnchorBufferStringLength

                m_pWsbTitleText->Addn( szTitleOrAnchorBuffer, TitleOrAnchorBufferStringLength );
            }
            break;

        case TAG_SPAN: // ending
            InsideTagSpanWithLangAttr = 0;
            break;

        case TAG_A: // ending
            {
                // The TAG_A case above already emitted any token
                // which was ended by this Ending TAG_A markup.

                ArmToSaveStartAnchorOffset = 0; // dis-arm

                // I do not keep anchor text any more. So do not:
                // Yes, restoring this for Link cross references.
                FinalizeTitleOrAnchorTextBuffer( );

                // Only save anchors for links out, not links in:
                if( TagA_Had_Href )
                {
                    // Then, proceed to age the index from 'B' to 'A'.
                    // else do nothing, because without seeing tag A + href,
                    // nothing has modified the value in LastTagA_UrlIndex.
                    // But also make sure the URL can'ized okay.
                    if( m_PageIsAQrp
                    && LastTagA_UrlIndex > 1 ) // CYA from -1, 0, 1.
                    {
                        UrlIndexOfAnyPriorTagA = SavedAtEntryToTagA;
                    }

                    // I must ensure that IDX are non-descending.
                    // Since consecutive anchors MAY be adjacent,
                    // I shall check that they enclose some text.

                    if( LastTagAIDXHighWater <= StartAnchorOffsetInText
                    && StartAnchorOffsetInText < EndingTagAOffsetInText
                    && LastTagA_UrlIndex > 1 )
                    {
                        // m_pIdxPaperIndex includes ALL link heads.
                        // For query page scraping, add in parallel,
                        // new method to include just good hit urls.

                        if( m_AppendPageScrapingFacts
                        && LastTagA_UrlIndex > 1 ) // CYA from -1, 0, 1.
                        {
                            // show this TAG_A head URL in the facts.
                            m_pWsbQrpFacts->Add( L"\r\nPossible hit URL: " );

                            wchar_t * pMalTagAUrl = ( wchar_t * ) CSolAllUrls.GetFullKey( LastTagA_UrlIndex );
                            m_pWsbQrpFacts->Add( pMalTagAUrl );
                            MyFree( 2367, UNPREDICTABLE, pMalTagAUrl );

                            // First newline will end the URL text line.
                            // Second newline will separate the block of
                            // fact lines containing URL and anchor clues
                            m_pWsbQrpFacts->Add( L"\r\n\r\n" );
                        }

                        m_pIdxPaperIndex->AddIdx( StartAnchorOffsetInText, EndingTagAOffsetInText, LastTagA_UrlIndex, 0 );
                        LastTagAIDXHighWater = EndingTagAOffsetInText;

                        // New work to cross-reference links:
                        // This one is for TAG_A, so save anchor text from here:
                        // szTitleOrAnchorBuffer, TitleOrAnchorBufferStringLength
                        size_t iAnchorText;
                        if( TitleOrAnchorBufferStringLength > 0 )
                            iAnchorText = CSolAnchorText.AddKey( szTitleOrAnchorBuffer );
                        else
                            iAnchorText = CSolAnchorText.AddKey( L"No anchor text." );
                        #if DO_DEBUG_ADDFIND
                        if( iAnchorText <= 1 )
                            { Spew( L"AddFind <= 1 at chtm 2295" ); }
                        #endif
                        AddAnchorHeadTailText( m_BestUrlIndex, LastTagA_UrlIndex, iAnchorText );
                    }

                    // For now, just do not save "foreign" languages.
                    // You see, I do, did, above.
                    // if( ! InsideTagAWithLangAttr
                    // && ! InsideTagSpanWithLangAttr )
                    // {
                    //     // Add to CSolAllWords? Do it during text parse.
                    // }

                    if( m_PageIsAQrp
                    && TitleOrAnchorBufferStringLength > 0
                    && TitleOrAnchorBufferStringLength < 100 )
                    {
                        // Emit ( or test ) anchor text as a page scraping facts.

                        wchar_t wk[200];

                        // Did anybody terminate this string?
                        szTitleOrAnchorBuffer[ TitleOrAnchorBufferStringLength ] = NULL;

                        // Nothing like crash / stack overflow to spoil your day
                        #if DO_DEBUG_SCRAPING
                            ; SpewValue( L"TitleOrAnchorBufferStringLength", TitleOrAnchorBufferStringLength );
                            ; SpewTwo( L"szTitleOrAnchorBuffer", szTitleOrAnchorBuffer );
                        #endif

                        wsprintf( wk, L"anchortext \"%s\"",
                            szTitleOrAnchorBuffer );

                        AddOrTestQueryFact( WHERENESS_BALD, wk );

                        // Let's detect certain number combinations that may
                        // appear in query results, like these: 1. 10% 44Kb
                        // I think only left-side number strings are needed.
                        // Accept interior dots and commas as part of number.

                        // First, categorize the ANCHOR TEXT as having leading numbers:
                        // NO_NUMBER
                        // NUMBER_STRICT
                        // NUMBER_POINTS
                        // NUMBER_COMMAS
                        // NUMBER_HYPHENS
                        // NUMBER_SLASHS
                        // NUMBER_COLONS
                        // NUMBER_MIXED

                        int AncTextType = NO_NUMBER;
                        wchar_t * scan = szTitleOrAnchorBuffer;

                        if( iswdigit( szTitleOrAnchorBuffer[0] ) )
                        {
                            int point = 0;
                            int comma = 0;
                            int hyphen = 0;
                            int slash = 0;
                            int colon = 0;
                            for( ;; )
                            {
                                scan ++; // pre-increment to next char
                                if( *scan == NULL )
                                    break;

                                if( iswdigit( *scan ) )
                                    continue;

                                // All other punct must be followed by a digit.
                                if( iswdigit( scan[1] ) )
                                {
                                    if( *scan == '.' )
                                    {
                                        point = 1;
                                        AncTextType = NUMBER_POINTS;
                                        continue;
                                    }
                                    if( *scan == ',' )
                                    {
                                        comma = 1;
                                        AncTextType = NUMBER_COMMAS;
                                        continue;
                                    }
                                    if( *scan == '-' )
                                    {
                                        hyphen = 1;
                                        AncTextType = NUMBER_HYPHENS;
                                        continue;
                                    }
                                    if( *scan == '/' )
                                    {
                                        slash = 1;
                                        AncTextType = NUMBER_SLASHS;
                                        continue;
                                    }
                                    if( *scan == ':' )
                                    {
                                        colon = 1;
                                        AncTextType = NUMBER_COLONS;
                                        continue;
                                    }
                                }
                                break; // stop on first non digit, or . or ,
                            }
                            int sum = point + comma + hyphen + slash + colon;
                            if( sum == 0 )
                                AncTextType = NUMBER_STRICT;
                            if( sum > 1 )
                                AncTextType = NUMBER_MIXED;
                            // scan is at first non-number, or final NULL.
                        }

                        // several cases for the current token:
                        if( AncTextType != NO_NUMBER )
                        {
                            if( *scan == NULL )
                                wsprintf( wk, L"anchortext %s", NumTypes[AncTextType] );
                            else
                                wsprintf( wk, L"anchortext %s+\"%s\"", NumTypes[AncTextType], scan );
                            AddOrTestQueryFact( WHERENESS_BALD, wk );
                        }
                    }

                    if( m_PageIsAQrp
                    && LastTagA_UrlIndex > 1 ) // CYA from -1, 0, 1.
                    {
                        // This is a parse of a query result page,
                        // and we are ending TAG_A at a link head.
                        // We finished all tokens, and anchortext,
                        // and that work processed any rule match.
                        // Current URL is hung: LastTagA_UrlIndex.
                        // It is time to do something about match.

                        HeadLinkIsCrossingTagABoundary( );
                    }
                }

                InsideTagAWithLangAttr = 0;

                PriorTokenUnder50[0] = NULL; // End of A prevents "two tokens"
                PriorTokenIsNumberEnum = NO_NUMBER;
            }
            break;
        }
    }



    // --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES ---
    // --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES ---
    //
    // At this point, add a generic process to extract attributes.
    // We might have a predefined vocabulary of attribute names,
    // Or allow a symbol table to collect new ones for analysis.
    //
    // Do not process xml-ish tags like these:
    // <?xml:namespace prefix = o ns = "urn:schema..." />
    // <o:p>
    // </o:p>
    //
    ProcessTagAttributes( );
    //
    // --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES ---
    // --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES --- ATTRIBUTES ---



    // ===========================================================
    // In this switch pair, process tags to totalize attribute values.
    // Most ( 1 tag + 1 attr ) work was done in ProcessTagAttributes.
    // But tags with multiple attributes to mingle, I will handle here:
    // ===========================================================

    if( TextHeldInLayer1Markup [1] != '/' )
    {
        // In this IF clause,
        // process a starting or standalone tag

        switch( TagEnumHeldInLayer1Markup )
        {

        // TAG_FORM has multiple attributes, but I can totalize them
        // and all other inputs, etc., upon reaching ending TAG_FORM.
        // But I like the idea of strict gating as done on TAG_INPUT.

        case TAG_FORM: // finishing an opening FORM tag work
            // doing TAG_FORM2 ( 1=start open tag, 2=end open tag, 3=close tag )

            m_DuringTagForm = 0; // disables attribute work outside tag.

            // Prune deficient forms as early as possible...?
            // Maybe not, for more complete initial research.
            // JavaScript may finish form w/o ACTION, METHOD.

            // This was the thing that did collect one example query URL.
            // It's a MEMBER, and not part of the FRUIT dependent on URL.

            if( m_pWsbFormParts != NULL ) // Just in case left from old FORM
            {
                delete m_pWsbFormParts;
                m_pWsbFormParts = NULL;
            }
            m_pWsbFormParts = new CWsb( ); // place to assemble example url

            // Initialize it with the METHOD + ACTION URL less extras.
            // Because of so many NO_METHOD flaws, I'm going to leave
            // that off the FYI item ( assume GET as default ) and user
            // can change FYI to GET or POST per atoms listed with it.

            // FYI prefix will be added during save file.
            // m_pWsbFormParts->Add( L"FYI: " ); // greppage specimen prefix

            // See that everybody gets a METHOD atom, even if bad:

            switch( m_FormGet1Post2 )
            {
            case 0:
                m_FormRejectedBitfield |= FORM_REJECT_NO_METHOD;
                // Add this 'flaw' as an atom too
                // N.B. All accessers must look up the pOneForm.
                if( m_FormMethodUrlIndex != 0 )
                {
                    COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                    if( pOneForm != NULL )
                    {
                        pOneForm->pSolFormAtoms->AddKey( L"METHOD ABSENT" );
                    }
                }
                break;
            case 1:
                // N.B. All accessers must look up the pOneForm.
                if( m_FormMethodUrlIndex != 0 )
                {
                    COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                    if( pOneForm != NULL )
                    {
                        pOneForm->pSolFormAtoms->AddKey( L"METHOD GET" );
                    }
                }
                break;
            case 2:
                // N.B. All accessers must look up the pOneForm.
                if( m_FormMethodUrlIndex != 0 )
                {
                    COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                    if( pOneForm != NULL )
                    {
                        pOneForm->pSolFormAtoms->AddKey( L"METHOD POST" );
                    }
                }
                break;
            }

            // See that everybody gets an ACTION atom, even if bad:

            // Do not look up a URL index that CombinAndHang left as zero.
            if( m_FormMethodUrlIndex != 0 )
            {
                wchar_t * pMalKey = CSolFormUrls.GetFullKey( m_FormMethodUrlIndex ); // a malloc, user frees
                if( pMalKey != NULL )
                {
                    m_pWsbFormParts->Add( pMalKey );
                    MyFree( 2147, UNPREDICTABLE, pMalKey );
                }
            }
            else
            {
                if( m_FormActionPresent )
                {
                    // Elsewhere in ACTION I recorded a JS/Bad rejection bit:
                    m_pWsbFormParts->Add( L"BAD_ACTION_URL" ); // clarifier only
                    // Add this 'flaw' as an atom too
                    // N.B. All accessers must look up the pOneForm.
                    if( m_FormMethodUrlIndex != 0 )
                    {
                        COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                        if( pOneForm != NULL )
                        {
                            pOneForm->pSolFormAtoms->AddKey( L"ACTION URL UNCANONICALIZABLE" );
                        }
                    }
                }
                else
                {
                    m_pWsbFormParts->Add( L"NO_ACTION_ATTR" ); // for no action
                    m_FormRejectedBitfield |= FORM_REJECT_NO_ACTION_URL;
                    // Add this 'flaw' as an atom too
                    // N.B. All accessers must look up the pOneForm.
                    if( m_FormMethodUrlIndex != 0 )
                    {
                        COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                        if( pOneForm != NULL )
                        {
                            pOneForm->pSolFormAtoms->AddKey( L"ACTION URL ABSENT" );
                        }
                    }
                }
            }
            m_FormParamCount = 0; // initialize to use '?' not '&' introducer

            break;

        case TAG_INPUT: // finishing an opening INPUT tag work
            // doing TAG_INPUT2 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;

            m_DuringTagInput = 0; // disables attribute work outside tag.
            // Totalize the following attribute-set variables:
            // m_FormInputType
            // m_FormInputName
            // m_FormInputValue

            // Process many "input type=..." things similarly:
            //  3047 hidden
            //   763 text
            //   554 submit
            //   473 checkbox
            //   316 image
            //   215 radio
            //   196 button
            //   178 search
            //    17 password
            //    11 file

            if( m_FormMethodUrlIndex != 0 ) // needed to look up fruit...
            {
                wchar_t * szLegend = L"???";
                int DoName = 0;
                int DoValue = 0;
                int DoAppend = 0;

                switch( m_FormInputType )
                {

                    // Lamentable types:

                case ENUM_INPUT_TYPE_OTHER: // Provision for TYPES I knew not.
                    szLegend = L"IN_OTHER";
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 0;
                    m_FormRejectedBitfield |= FORM_REJECT_UNKNOWN_TYPE_INPUT;
                    break;
                case ENUM_INPUT_TYPE_PASSWORD:
                    szLegend = L"IN_PASSWORD";
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 0;
                    m_FormRejectedBitfield |= FORM_REJECT_PASSWORD_INPUT;
                    break;
                case ENUM_INPUT_TYPE_FILE:
                    szLegend = L"IN_FILE";
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 0;
                    m_FormRejectedBitfield |= FORM_REJECT_FILE_INPUT;
                    break;


                    // Innocuous types:

                case ENUM_INPUT_TYPE_HIDDEN:
                    szLegend = L"IN_HIDDEN";
                    DoName = 1;
                    DoValue = 1;
                    DoAppend = 1;
                    break;
                case ENUM_INPUT_TYPE_CHECKBOX:
                    szLegend = L"IN_CHECKBOX";
                    DoName = 1;
                    DoValue = 1;
                    DoAppend = m_FormCheckBoxChecked;
                    break;
                case ENUM_INPUT_TYPE_RADIO:
                    szLegend = L"IN_RADIO";
                    DoName = 1;
                    DoValue = 1;
                    DoAppend = m_FormRadioChecked;
                    break;


                    // Some Required-Once types:

                case ENUM_INPUT_TYPE_SEARCH:
                    // Process the "SEARCH" TYPE of INPUT as a "TEXT" type:
                case ENUM_INPUT_TYPE_UNSTATED:
                    // Process the unstated TYPE of INPUT as a "TEXT" type:
                case ENUM_INPUT_TYPE_TEXT:
                    szLegend = L"IN_TEXT"; // also stands for SEARCH, or UNSTATED type
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 0; // not until totalized at ending form tag
                    m_FormInputTypeTextCount ++;
                    if( m_FormInputName == NULL )
                    {
                        // I see this error despite well-named atoms in dump.
                        // Could be it is from multiple form instances at URL?
                        m_FormRejectedBitfield |= FORM_REJECT_UNNAMED_TEXT_INPUT;
                    }
                    else
                    {
                        // first free the copy destination pointer if any.
                        if( m_FormInputTypeTextName != NULL )
                        {
                            MyFree( 1689, UNPREDICTABLE, m_FormInputTypeTextName );
                            m_FormInputTypeTextName = NULL; // initialize locally relevant variable
                        }
                        // Hand off pointer for totalizer:
                        // m_FormInputTypeTextName = m_FormInputName; // move
                        // m_FormInputName = NULL; // moved. Do not free here.
                        // No, make a whole new copy of the string memory.
                        size_t nMal = wcslen( m_FormInputName ) + 1;
                        m_FormInputTypeTextName = ( wchar_t * ) MyMalloc( 2481, nMal * sizeof( wchar_t ) );
                        wcscpy( m_FormInputTypeTextName, m_FormInputName );
                    }
                    break;


                    // Another Required-Once type:

                case ENUM_INPUT_TYPE_SUBMIT:
                    szLegend = L"IN_SUBMIT";
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 1;
                    m_FormInputTypeSubmitCount ++;
                    break;
                case ENUM_INPUT_TYPE_IMAGE:
                    szLegend = L"IN_IMAGE";
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 1;
                    m_FormInputTypeSubmitCount ++; // treat as submit
                    break;
                case ENUM_INPUT_TYPE_BUTTON:
                    szLegend = L"IN_BUTTON";
                    DoName = 1;
                    DoValue = 0;
                    DoAppend = 1;
                    m_FormInputTypeSubmitCount ++; // treat as submit
                    break;
                }

                // I tested index is not zero far above.
                COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                if( pOneForm != NULL )
                {
                    // There can be NAME without VALUE.
                    // Ignore some VALUEs without NAME.
                    // WEEELLL... No. Include them for analysis...
                    // Value without Name occurs on JavaScript stuff.

                    size_t strlen0 = wcslen( szLegend );
                    size_t strlen1 = 0;
                    size_t strlen2 = 0;

                    if( DoName
                    && m_FormInputName != NULL )
                        strlen1 = wcslen( m_FormInputName );

                    if( DoValue
                    && m_FormInputValue != NULL )
                        strlen2 = wcslen( m_FormInputValue );

                    size_t nLen = strlen0 + 1 + strlen1 + 1 + strlen2 + 1; // space, equals, NULL
                    wchar_t * pMal = ( wchar_t * ) MyMalloc( 2250, nLen * sizeof( wchar_t ) );
                    if( pMal != NULL )
                    {
                        // Add all INPUT types to CSol

                        wchar_t * into = pMal;
                        wcscpy( into, szLegend );
                        into += strlen0;
                        *into++ = ' ';
                        if( strlen1 != 0 )
                        {
                            wcscpy( into, m_FormInputName );
                            into += strlen1;
                        }
                        *into++ = '=';
                        if( strlen2 != 0 )
                        {
                            wcscpy( into, m_FormInputValue );
                            into += strlen2;
                        }
                        *into = NULL;
                        #if DO_DEBUG_FORM0
                            ; Spew( pMal );
                        #endif
                        pOneForm->pSolFormAtoms->AddKey( pMal );

                        // Append some INPUT types to this instance of example URL
                        // I can reuse that Atom string just past the legend.

                        if( DoAppend )
                        {
                            if( strlen2 != 0 )
                                FormAddNameValuePair( m_FormInputName, m_FormInputValue );
                            else
                                FormAddNameValuePair( m_FormInputName, NULL );
                        }
                        MyFree( 2266, UNPREDICTABLE, pMal );
                    }
                }
            }

            // Having finished the end of INPUT tag totalization,
            // Free the following mallocs if set by attribute work:
            if( m_FormInputName != NULL )
            {
                MyFree( 2073, UNPREDICTABLE, m_FormInputName );
                m_FormInputName = NULL;
            }
            if( m_FormInputValue != NULL )
            {
                MyFree( 2078, UNPREDICTABLE, m_FormInputValue );
                m_FormInputValue = NULL;
            }
            break;

        case TAG_SELECT: // finishing an opening SELECT tag work

            // doing TAG_SELECT2 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;
            m_DuringTagSelect = 0; // disables attribute work outside tag.
            // Totalize the following attribute-set variables:
            // m_FormSelectName -- Not until the close tag.
            // No, rather, not until a selected OPTION tag is finished.
            break;

        case TAG_OPTION: // finishing an opening OPTION tag work

            // doing TAG_OPTION2 ( 1=start open tag, 2=end open tag, 3=close tag )

            if( ! m_BetweenOpenCloseTagForm )
                break;
            if( ! m_BetweenOpenCloseTagSelect )
                break;
            m_DuringTagOption = 0; // disables attribute work outside tag.

            // Totalize the following attribute-set variables:
            // m_FormSelectName ( select tags surround option tags )
            // m_FormOptionValue

            if( m_FormMethodUrlIndex != 0 )
            {
                wchar_t * szLegend = L"SEL_OPTION";

                COneForm * pOneForm = ( COneForm * )CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                if( pOneForm != NULL )
                {
                    // There can be NAME without VALUE.
                    // Ignore some VALUEs without NAME.
                    // WEEELLL... No. Include them for analysis...
                    // Value without Name may occur on JavaScript.

                    size_t strlen0 = wcslen( szLegend );
                    size_t strlen1 = 0;
                    size_t strlen2 = 0;

                    if( m_FormSelectName != NULL )
                        strlen1 = wcslen( m_FormSelectName );

                    if( m_FormOptionValue != NULL )
                        strlen2 = wcslen( m_FormOptionValue );

                    size_t nLen = strlen0 + 1 + strlen1 + 1 + strlen2 + 1; // space, equals, NULL
                    wchar_t * pMal = ( wchar_t * ) MyMalloc( 2410, nLen * sizeof( wchar_t ) );
                    if( pMal != NULL )
                    {
                        // Add all SELECT + OPTION pairs to CSol

                        wchar_t * into = pMal;
                        wcscpy( into, szLegend );
                        into += strlen0;
                        *into++ = ' ';
                        if( strlen1 != 0 )
                        {
                            wcscpy( into, m_FormSelectName );
                            into += strlen1;
                        }
                        *into++ = '=';
                        if( strlen2 != 0 )
                        {
                            wcscpy( into, m_FormOptionValue );
                            into += strlen2;
                        }
                        *into = NULL;
                        #if DO_DEBUG_FORM0
                            ; Spew( pMal );
                        #endif

                        // If selected, append SELECT + OPTION pair to example URLs



                        pOneForm->pSolFormAtoms->AddKey( pMal );
                        MyFree( 2430, UNPREDICTABLE, pMal );
                    }
                }
            }

            if( m_FormOptionSelected )
            {
                FormAddNameValuePair( m_FormSelectName, m_FormOptionValue );
            }

            // Free the following mallocs if set by attribute work:
            if( m_FormOptionValue != NULL )
            {
                MyFree( 2118, UNPREDICTABLE, m_FormOptionValue );
                m_FormOptionValue = NULL;
            }
            break;
        }
    }


    // ===========================================================
    // wip - add switch on tag...
    // ===========================================================

    // Later, do some of this other, possible, stuff I cataloged here...



    //
    // wip - study of tags
    //
    //
    // case TAG_META:
    // extract attributes HTTP-EQUIV and CONTENT:
    // <meta http-equiv="Refresh" content="0;url=http://www..."/>
    //
    // Analyzing the attributes on META tag, I find not much of interest.
    // There are some words, some urls, and some emails, and other stuff.
    // Later, consider mining some of these:
    //
    // QZ? http-equiv on meta =  Refresh
    // QZ? http-equiv on meta =  REFRESH
    // QZ? content on meta =  0; URL=/ym/login?nojs=1
    // QZ? content on meta =  0480
    // QZ? content on meta =  5;URL=http://www.ffrevelations.it
    //
    // QZ? name on meta =  description
    // QZ? content on meta =  surcha searches the web for you and combines unique results from the best search engines
    // QZ? content on meta =  A user-friendly, HIG-compliant password manager for the GNOME desktop
    //
    // QZ? name on meta =  keywords
    // QZ? content on meta =  revelation, password, manager, gnome, gtk, account, secure, security, encryption, encrypted
    // QZ? content on meta =  Accueil,CNIL,informatique,commission,6 janvier 1978,nationale,protection,donnees,personnelles,
    // QZ? content on meta =  religion philosophy glossary Revelation Book
    //
    // QZ? name on meta =  Contents
    // QZ? content on meta =  Ancient Wisdom, New Age Teachings, Philosophy, Religion, Esotericism, Theosophy, Blavatasky, Alice A. Bailey, Raja Yoga, Consciousness, Esoteric Glossary, Poetry, Mantrams, Art, Moon Phases, Meditation, Service, Service News,
    //
    // QZ? name on meta =  Classification
    // QZ? content on meta =  Academic Sites, Bible, Religion, Theology, Personal Homepages
    //
    // QZ? name on meta =  email
    // QZ? name on meta =  made
    // QZ? name on meta =  Author
    // QZ? name on meta =  OWNER
    // QZ? http-equiv on meta =  reply-to
    // QZ? content on meta =  abc@msn.com
    // QZ? content on meta =  mailto:abc@msn.com
    //
    //
    // Since that analysis, I have become zealous to know charset.
    // Is there any reason to care now about "content="text/html"?
    // E.g., in:
    // <meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
    //
    // Since META follows the pattern of 1 name, 1 value,
    // it can be handled within the routines called above:
    // ProcessTagAttributes( )
    // who calls:
    // ProcessOneAttribute( wchar_t * szName, wchar_t * szValue )


    // ===========================================================
    // In this switch pair, process tags to affect knowledge of searches:
    // ===========================================================

    // Later.


    IndexHeldInLayer1Markup = 0; // That tag is now consumed.
}


void CHtm::ProcessTagAttributes( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"165" );
    #endif
    wchar_t AttributeName[100];
    wchar_t AttributeValue[500];
    wchar_t * scan = TextHeldInLayer1Markup + IndexPastTagInLayer1Markup;
    wchar_t * past = TextHeldInLayer1Markup + IndexHeldInLayer1Markup;
    wchar_t * last = past - 1;

    // There is a valid HTML TAG name ( TagEnumHeldInLayer1Markup ) atop
    // the text buffer TextHeldInLayer1Markup. Process any attributes.

    // A prior successfull call to ValidTagNameInAccumulatedLabSuspense
    // set IndexPastTagInLayer1Markup for me where I will begin my scan.

    // ProcessWholeMarkupTag is may caller, and I see that he set
    // TextHeldInLayer1Markup [IndexHeldInLayer1Markup] = NULL;

    if( TextHeldInLayer1Markup[0] != '<' )
    {
        ProgramError( L"ProcessTagAttributes: atop not LAB" );
        return;
    }

    #if DO_DEBUG_HTMLATTRS
        ; Spew( L"CHtm: ProcessTagAttributes: Processing buffer, from index:" );
        ; SpewValue( TextHeldInLayer1Markup, IndexPastTagInLayer1Markup );
    #endif

    if( *last != '>' )
    {
        ProgramError( L"ProcessTagAttributes: last not RAB" );
        return;
    }

    if( TextHeldInLayer1Markup[1] == '/' )
    {
        // I found nothing of interest on ending tags.
        return;
    }

    // This is a seat-of-the pants parser.
    // Repeat until reach an expected '>'.

    for( ;; )
    {

        // Move to the next item for next loop, if any.
        while( *scan == ' ' ) // determinate: stops on null
            scan ++; // skip over spaces

        if( *scan == '/' )
            scan ++; // skip over any xml-ish final slash

        if( *scan == '>' )
            break; // no more attributes

        // sanity check
        if( scan >= past )
        {
            // This would be a programming logic error only:
            // AddToDebugTags( "Error: ProcessTagAttributes: ( scan >= past )\r\n" );
            return;
        }

        // Take everything up to ( [spaces] equals ) as an attribute name.
        wchar_t * fill = AttributeName;

        while( *scan > ' ' && *scan != '=' && scan < last ) // determinate: stops on last
        {
            if( iswupper( *scan ) )
                *fill = ( wchar_t ) ( *scan | ' ' ); // lowercase the Attribute name
            else
                *fill = *scan;
            if( fill < AttributeName + sizeof( AttributeName ) / sizeof( * AttributeName ) - 1 )
                fill ++;
            scan ++; // find next ( null, space, or equal )
        }
        *fill = NULL;

        while( *scan == ' ' ) // determinate: stops on null
            scan ++; // skip over spaces

        if( *scan != '=' )
        {
            // This was a surprise: ending attributes without values. e.g.,
            // <option value="10" selected>
            // Even worse, it may not be the last thing before '>'. e.g.,
            // <hr size="1" noshade width="100%">
            // So, just process it, no complaints.
            // Lots of bad punctuation comes here.

            ProcessOneAttribute( AttributeName, NULL );
            break;
        }
        scan ++; // skip over equal


        // Having found the equal sign, there may be a token,
        // presumably without spaces, or a SQT or DQT quoted
        // string.
        //
        // I found many scripts might include DQT themselves,
        // often preceeded by a backslash. DO NOT HEED THEM!
        // They seem to be a case of some mis-spoken \"...\".
        // Allow also BSL,SQT or BSL,DQT to serve as quoters.
        //
        // See above, the comment: // Discussion of Quoting:


        while( *scan == ' ' ) // determinate: stops on null
            scan ++; // skip over spaces

        int Introducer1 = ' '; // if unquoted, stop on space
        int Introducer2 = NULL; // using no second character

        if( *scan == DQT )
        {
            Introducer1 = DQT;
            scan ++; // skip over double quote
        }
        else if( *scan == SQT )
        {
            Introducer1 = SQT;
            scan ++; // skip over single quote
        }
        else if( *scan == BSL && scan[1] == DQT )
        {
            Introducer1 = BSL;
            Introducer2 = DQT;
            scan += 2; // skip over backslash, double quote
        }
        else if( *scan == BSL && scan[1] == SQT )
        {
            Introducer1 = BSL;
            Introducer2 = SQT;
            scan += 2; // skip over backslash, single quote
        }

        fill = AttributeValue;
        *fill++ = ' '; // Leave one space for ProcessOneAttribute to put '['


        // Loop below stops at LAST and not PAST, because LAST->RAB.
        // Original loop was simple ...
        // while( *scan != Introducer && scan < last ) // determinate: stops at last

        for( ;; )
        {
            if( scan == last ) // determinate: stops at last
                break;

            // This concept of BSL escaping the quote had to be given up:
            // if( *scan == Introducer
            // && ( scan[-1] != BSL         // backslash quotes DQT or SQT.
            //     || Introducer == ' ' ) ) // backslash doesn't quote space.
            //     break;

            if( *scan == Introducer1 )
            {
                if( Introducer2 == NULL )
                {
                    scan ++; // skip over one quote byte
                    break; // leaving fill just past attribute value
                }

                if( scan[1] == Introducer2 )
                {
                    scan += 2; // skip over two quote bytes
                    break; // leaving fill just past attribute value
                }
            }

            *fill = *scan;
            if( fill < AttributeValue + sizeof( AttributeValue ) / sizeof( * AttributeValue ) - 2 ) // leave 1 space for "]"
                fill ++;
            scan ++; // find quote introducer, or stop on '>' at last
        }

        *fill++ = ' '; // Leave one space for ProcessOneAttribute to put ']'
        *fill = NULL;

        // Occasionallly, ending tags have attributes.
        // Not enough to even worry about or test for.
        // </div id="wrap">
        ProcessOneAttribute( AttributeName, AttributeValue );

        // Wait, it gets worse...
        // Deal with these after the quote work:
        // A final xmlish slash:
        // / on br ( bare )
        // A final semicolon:
        // <a href='#' onClick="window.open( 'http://license.icopyright.net/3.5721?icx_id=D8IRE3GO1', '_blank', 'width=450,height=600,scrollbars=1' )";>
        // A final comma:
        // <form method="POST" action="http://search.internet.com/www.jupiterweb.com" target="_top",>

        // I will test for only those forms I saw:
        if( *scan == ','
        || *scan == ';' )
            scan ++;
    }
}

void CHtm::ProcessOneAttribute( wchar_t * szName, wchar_t * szValue )
{
    #if DO_DEBUG_CALLS
        // Routine( L"166" );
    #endif
    // If szValue is non-NULL, my caller added one first and one final space.
    // szName may be sought in CVoc.cpp AttrNames[], else studied or ignored.

    // Remember for the future; caused a crash on http-equiv, noted below:
    // I need to make all users of szValue test for null end!

    #if DO_DEBUG_HTMLATTRS
        ; Spew( L"CHtm: ProcessOneAttribute: Processing szName and szValue pair:" );
        ; Spew( ( szName==NULL )?L"-null-":szName );
        ; Spew( ( szValue==NULL )?L"-null-":szValue );
    #endif

    // I have a huge number more of unusual names,
    // I would like to add them to a symbol table.
    // Then they could enter into text valuations.

    // Changing to use a CSol for lookup.
    // First, do my ascii-type lowercase.
    // I better do it after copy instead.

    wchar_t wk[MAX_TAG_OR_ATTR_CHARS + 2];
    wchar_t * into = wk;
    wchar_t * from = szName;
    for( ;; )
    {
        wchar_t c = *from++;
        if( c == NULL )
            break;
        *into++ = c | ' '; // safe for all ascii attr names and chars
        if( into > wk + MAX_TAG_OR_ATTR_CHARS )
        {
            #if DO_DEBUG_HTMLATTRS
                *into = NULL;
                ; Spew( L"CHtm: ProcessOneAttribute quit. szName too long." );
                ; SpewValue( wk, -1 );
            #endif
            return;
        }
    }
    *into = NULL;

    if( into == wk )
    {
        #if DO_DEBUG_HTMLATTRS
            ; Spew( L"CHtm: ProcessOneAttribute quit. szName was empty." );
        #endif
        return;
    }

    // OO as in oops! INDEX is only used to get VALUE, which is the enum.

    int AttrIndex = CSolHtmlAttrNames.Find( wk );
    #if DO_DEBUG_ADDFIND
        if( AttrIndex == 1 )
            { Spew( L"AddFind 1 at chtm 2070" ); }
    #endif
    if( AttrIndex == NULL )
    {
        #if DO_DEBUG_HTMLATTRS || DO_SEE_NOVEL_ATTRS
            ; Spew( L"CHtm: ProcessOneAttribute quit. szName not recognized." );
        #endif
        return; // not a recognize name.
    }

    // Attribute name was recognized. Convert index to an ENUM!
    int AttrEnum = CSolHtmlAttrNames.GetUserValue( AttrIndex );

    #if DO_DEBUG_HTMLATTRS
        ; SpewValue( L"CHtm: ProcessOneAttribute recognized szName. Enum is", AttrEnum );
    #endif


    // New code to access TagNames, or something else, violated memory.
    if( TagEnumHeldInLayer1Markup <= TAG_ZERO )
    {
        ; SpewTwo( L"TextHeldInLayer1Markup", TextHeldInLayer1Markup );
        ; SpewValue( L"TagEnumHeldInLayer1Markup", TagEnumHeldInLayer1Markup );
        ProgramError( L"ProcessOneAttribute: TagEnumHeldInLayer1Markup <= TAG_ZERO" );
        return;
    }
    if( TagEnumHeldInLayer1Markup >= TAG_PAST_SENTINEL )
    {
        ; SpewTwo( L"TextHeldInLayer1Markup", TextHeldInLayer1Markup );
        ; SpewValue( L"TagEnumHeldInLayer1Markup", TagEnumHeldInLayer1Markup );
        ProgramError( L"ProcessOneAttribute: TagEnumHeldInLayer1Markup >= TAG_PAST_SENTINEL" );
        return;
    }

    // New code to access TagNames, or something else, violated memory.
    if( AttrEnum <= ATTR_ZERO )
    {
        ; SpewTwo( L"TextHeldInLayer1Markup", TextHeldInLayer1Markup );
        ; SpewValue( L"TagEnumHeldInLayer1Markup", TagEnumHeldInLayer1Markup );
        ; SpewValue( L"AttrEnum <= ATTR_ZERO", AttrEnum );
        ProgramError( L"ProcessOneAttribute: AttrEnum <= ATTR_ZERO" );
        return;
    }
    if( AttrEnum >= ATTR_PAST_SENTINEL )
    {
        ; SpewTwo( L"TextHeldInLayer1Markup", TextHeldInLayer1Markup );
        ; SpewValue( L"TagEnumHeldInLayer1Markup", TagEnumHeldInLayer1Markup );
        ; SpewValue( L"AttrEnum >= ATTR_PAST_SENTINEL", AttrEnum );
        ProgramError( L"ProcessOneAttribute: AttrEnum >= ATTR_PAST_SENTINEL" );
        return;
    }

    if( m_PageIsAQrp )
    {
        // For every ATTR, format ( or test ) something like one of these two lines:
        // ( good/bad url has )anchor tag x attr y
        // ( good/bad url has )prior/next tag x attr y

        // For starters, lets omit end tags.
        // Or I can do them, and prefix '/'.

        int EndTag = ( TextHeldInLayer1Markup [1] == '/' );
        int InTagA = ( TagEnumHeldInLayer1Markup == TAG_A );

        // Don't output a TAG_A + ATTR_HREF fact ( because I later emit URL )
        if( ! InTagA
        || AttrEnum != ATTR_HREF )
        {
            // Output the line with TAG and ATTR, but no value yet.

            wchar_t wk[200]; // The longest TAG is 10, ATTR is 17

            // Nothing like crash / stack overflow to spoil your day
            #if DO_DEBUG_SCRAPING
                ; SpewValue( L"AddQFact for ATTR enum", AttrIndex );
                ; SpewTwo( L"AddQFact for ATTR name", AttrNames[ AttrIndex ].wcp );
            #endif

            wsprintf( wk, L"TAG %s%s ATTR %s",
                ( EndTag ? L"/" : L"" ),
                TagNames[ TagEnumHeldInLayer1Markup ].wcp,
                AttrNames[ AttrIndex ].wcp );

            AddOrTestQueryFact( WHERENESS_TREBLE, wk );

            // If there was a non-NULL szValue,
            // Output the line with TAG and ATTR and value.
            // szValue might be extremely long.
            // Only do this for some limited length of value.

            if( m_PageIsAQrp
            && szValue != NULL )
            {
                int len = wcslen( szValue );
                // szValue has one extra space before and after. so test >= 2.
                if( len >= 2
                && len < 190 - 44 - 17 - 10 )
                {
                    // Nothing like crash / stack overflow to spoil your day
                    #if DO_DEBUG_SCRAPING
                        ; SpewValue( L"wcslen( szValue )", len );
                        ; SpewTwo( L"szValue", szValue );
                    #endif

                    // szValue has one extra space before and after. rid.
                    wchar_t save = szValue[ len - 1 ];
                    szValue[ len - 1 ] = NULL;

                    wsprintf( wk, L"TAG %s%s ATTR %s VALUE \"%s\"", // < 33 chars
                        ( EndTag ? L"/" : L"" ),
                        TagNames[ TagEnumHeldInLayer1Markup ].wcp,
                        AttrNames[ AttrIndex ].wcp,
                        szValue + 1 );

                    szValue[ len - 1 ] = save;

                    AddOrTestQueryFact( WHERENESS_TREBLE, wk );
                }
            }
        }
    }

    switch( AttrEnum )
    {

    case ATTR_ALT:
        // Add all ALT="alternate text..." to text output.
        if( szValue != NULL )
            ProcessAlternateText( szValue );
        break;

    case ATTR_TITLE:
        // Add all TITLE="anchor text..." to text output.
        // Doing this causes a lot of duplicate words.
        // if( szValue != NULL )
        //    ProcessAlternateText( szValue );

        // Use title to assign a title to a novel URL.
        // --Not a hot topic, as may have anchor text.

        // For now, just do not save "foreign" languages.

        // Later, establish a vector of RBSLs;
        // Or, just note foreign lang in RBSI.

        if( ! InsideTagAWithLangAttr
        && ! InsideTagSpanWithLangAttr
        && szValue != NULL )
        {
            // I need to apply the same strictness to szValue
            // as would have been done to anchor text by this:
            // FinalizeTitleOrAnchorTextBuffer( pLiveData );

            // Let us only copy alphas, and UPPER case them.
            // wchar_t * from = szValue;
            // wchar_t TempTitle[ MAX_TITLE_OR_ANCHOR + 1 ]; // small enuf for auto.
            // int idx = 0;
            // for( ;; )
            // {
            //     if( *from == NULL )
            //         break;
            //     if( iswalpha( *from ) )
            //     {
            //         if( idx < MAX_TITLE_OR_ANCHOR )
            //         {
            //             TempTitle[ idx ++ ] = *from & ~ ' ';
            //         }
            //     }
            //     else
            //     {
            //         // treat all non-alphas as a space.
            //         // But not atop buffer, nor multiple whitespaces.
            //         if( idx > 0
            //         && idx < MAX_TITLE_OR_ANCHOR
            //         && TempTitle[ idx - 1 ] != ' ' )
            //             TempTitle[ idx ++ ] = ' ';
            //     }
            //     from ++;
            // }
            // // Remove any final whitespace.
            // if( idx > 0
            // && TempTitle[ idx - 1 ] == ' ' )
            //     idx --;
            // TempTitle[ idx ] = NULL;

            // Got TITLE?
            // AddToDebugTags( "TITLE_ATTR: " );
            // AddToDebugTags( ( char * ) TempTitle );
            // AddToDebugTags( "\r\n" );

            // unsigned int index = AnchorTitle.FindOrInsert( TempTitle );
            // AnchorTitle.List[index].ItemCountTotal ++;
        }
        break;

    case ATTR_TYPE:
        if( m_DuringTagInput
        && szValue != NULL )
        {
            // The TYPE attribute will be totalized afterwards,
            // when finished processing the current INPUT tag.

            // Recognize and remember INPUT TYPE.

            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }

            // In roughly decreasing order of frequency:

            if( ( scan[0] | ' ' ) == 'h'
            && ( scan[1] | ' ' ) == 'i'
            && ( scan[2] | ' ' ) == 'd'
            && ( scan[3] | ' ' ) == 'd'
            && ( scan[4] | ' ' ) == 'e'
            && ( scan[5] | ' ' ) == 'n' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_HIDDEN;
            }
            else
            if( ( scan[0] | ' ' ) == 't'
            && ( scan[1] | ' ' ) == 'e'
            && ( scan[2] | ' ' ) == 'x'
            && ( scan[3] | ' ' ) == 't' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_TEXT;
            }
            else
            if( ( scan[0] | ' ' ) == 's'
            && ( scan[1] | ' ' ) == 'u'
            && ( scan[2] | ' ' ) == 'b'
            && ( scan[3] | ' ' ) == 'm'
            && ( scan[4] | ' ' ) == 'i'
            && ( scan[5] | ' ' ) == 't' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_SUBMIT;
            }
            else
            if( ( scan[0] | ' ' ) == 'c'
            && ( scan[1] | ' ' ) == 'h'
            && ( scan[2] | ' ' ) == 'e'
            && ( scan[3] | ' ' ) == 'c'
            && ( scan[4] | ' ' ) == 'k'
            && ( scan[5] | ' ' ) == 'b'
            && ( scan[6] | ' ' ) == 'o'
            && ( scan[7] | ' ' ) == 'x' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_CHECKBOX;
            }
            else
            if( ( scan[0] | ' ' ) == 'i'
            && ( scan[1] | ' ' ) == 'm'
            && ( scan[2] | ' ' ) == 'a'
            && ( scan[3] | ' ' ) == 'g'
            && ( scan[4] | ' ' ) == 'e' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_IMAGE;
            }
            else
            if( ( scan[0] | ' ' ) == 'r'
            && ( scan[1] | ' ' ) == 'a'
            && ( scan[2] | ' ' ) == 'd'
            && ( scan[3] | ' ' ) == 'i'
            && ( scan[4] | ' ' ) == 'o' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_RADIO;
            }
            else
            if( ( scan[0] | ' ' ) == 'b'
            && ( scan[1] | ' ' ) == 'u'
            && ( scan[2] | ' ' ) == 't'
            && ( scan[3] | ' ' ) == 't'
            && ( scan[4] | ' ' ) == 'o'
            && ( scan[5] | ' ' ) == 'n' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_BUTTON;
            }
            else
            if( ( scan[0] | ' ' ) == 's'
            && ( scan[1] | ' ' ) == 'e'
            && ( scan[2] | ' ' ) == 'a'
            && ( scan[3] | ' ' ) == 'r'
            && ( scan[4] | ' ' ) == 'c'
            && ( scan[5] | ' ' ) == 'n' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_SEARCH;
            }
            else
            if( ( scan[0] | ' ' ) == 'p'
            && ( scan[1] | ' ' ) == 'a'
            && ( scan[2] | ' ' ) == 's'
            && ( scan[3] | ' ' ) == 's'
            && ( scan[4] | ' ' ) == 'w'
            && ( scan[5] | ' ' ) == 'o'
            && ( scan[6] | ' ' ) == 'r'
            && ( scan[7] | ' ' ) == 'd' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_PASSWORD;
            }
            else
            if( ( scan[0] | ' ' ) == 'f'
            && ( scan[1] | ' ' ) == 'i'
            && ( scan[2] | ' ' ) == 'l'
            && ( scan[3] | ' ' ) == 'e' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_FILE;
            }
            else
            if( ( scan[0] | ' ' ) == 'h'
            && ( scan[1] | ' ' ) == 'i'
            && ( scan[2] | ' ' ) == 'd'
            && ( scan[3] | ' ' ) == 'd'
            && ( scan[4] | ' ' ) == 'e'
            && ( scan[5] | ' ' ) == 'n' )
            {
                m_FormInputType = ENUM_INPUT_TYPE_HIDDEN;
            }
            else
            {
                // Unknown is not same as ENUM_INPUT_TYPE_UNSTATED
                m_FormInputType = ENUM_INPUT_TYPE_OTHER;
            }

        }
        break;

    case ATTR_NAME:
        // Attribute NAME appears on A tags.
        // Attribute NAME appears on INPUT tags.
        // Attribute NAME appears on SELECT tags.

        // Do work for when attribute NAME appears on A tags.
        // As I cozy-up the code to what is found in the cache,
        // this bool brought some improvement, but a certain
        // producer idiom suffered from it, splitting words.
        // See...
        //
        //     <a name="Bookmark 3">
        // Among the occult r
        //     </a>
        // evelations,

        // I cannot clear this yet, must set a bool for /A tag:
        // InsertSpaceIfSayAlphaComes = 0; // NAME attr. disarms

        if( TagEnumHeldInLayer1Markup == TAG_A )
        {
            // It was extremely valuable to distinguish A tags
            // used as head anchors versus A tags used as tail
            // anchors, to assist the rules for word splitting.
            TagA_Had_Name = 1;
        }

        // Do work for when attribute NAME appears on INPUT tags.
        if( m_DuringTagInput
        && szValue != NULL )
        {
            // The NAME attribute will be totalized afterwards,
            // when finished processing the current INPUT tag.

            // Malloc to remember INPUT NAME.
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }
            wchar_t * atop = scan;
            for( ;; ) // find final null
            {
                if( *scan == NULL ) // determinate: stops on NULL
                    break;
                scan++;
            }
            for( ;; ) // back over final spaces
            {
                if( scan == atop
                || scan[-1] != ' ' ) // determinate: stops on atop
                    break;
                scan--;
            }
            // Allow to pass back empty strings... if( scan > atop )
            {
                // Save INPUT tag NAME attr in a malloc until totalized.
                // First free any old pointer if somehow redundant name.
                if( m_FormInputName != NULL )
                {
                    MyFree( 3221, UNPREDICTABLE, m_FormInputName );
                    m_FormInputName = NULL;
                }

                size_t nLen = scan - atop;
                size_t nMal = nLen + 1;
                m_FormInputName = ( wchar_t * ) MyMalloc( 2536, nMal * sizeof( wchar_t ) );
                if( m_FormInputName != NULL )
                {
                    memcpy( m_FormInputName, atop, nLen * sizeof( wchar_t ) );
                    m_FormInputName[nLen] = NULL;
                    #if DO_DEBUG_FORM1
                        ; Spew( L"INPUT NAME:" );
                        ; Spew( m_FormInputName );
                    #endif
                }
            }
        }

        // Do work for when attribute NAME appears on SELECT tags.
        if( m_DuringTagSelect
        && szValue != NULL )
        {
            // The NAME attribute will be totalized afterwards,
            // when finished processing the current SELECT tag.

            // Malloc to remember INPUT NAME.
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }
            wchar_t * atop = scan;
            for( ;; ) // find final null
            {
                if( *scan == NULL ) // determinate: stops on NULL
                    break;
                scan++;
            }
            for( ;; ) // back over final spaces
            {
                if( scan == atop
                || scan[-1] != ' ' ) // determinate: stops on atop
                    break;
                scan--;
            }
            // Allow to pass back empty strings... if( scan > atop )
            {
                // Don't malloc until free any old pointer!
                if( m_FormSelectName != NULL )
                {
                    MyFree( 3293, UNPREDICTABLE, m_FormSelectName );
                    m_FormSelectName = NULL;  // initialize locally relevant variable
                }
                size_t nLen = scan - atop;
                size_t nMal = nLen + 1;
                m_FormSelectName = ( wchar_t * ) MyMalloc( 2536, nMal * sizeof( wchar_t ) );
                if( m_FormSelectName != NULL )
                {
                    memcpy( m_FormSelectName, atop, nLen * sizeof( wchar_t ) );
                    m_FormSelectName[nLen] = NULL;
                    #if DO_DEBUG_FORM1
                        ; Spew( L"SELECT NAME:" );
                        ; Spew( m_FormSelectName );
                    #endif
                }
            }
        }
        break;

    case ATTR_SELECTED:
        // Attribute SELECTED appears on OPTION tags.

        // A SELECTED attribute will be totalized afterwards,
        // when finished processing the current OPTION tag.

        m_FormOptionSelected = 1;
        break;

    case ATTR_CHECKED:
        // Attribute CHECKED appears on CHECKBOX tags.

        // A CHECKED attribute will be totalized afterwards,
        // when finished processing the current INPUT tag.

        m_FormCheckBoxChecked = 1; // either-or
        m_FormRadioChecked = 1; // either-or
        break;

    case ATTR_VALUE:
        // Attribute VALUE appears on INPUT tags.
        // Attribute VALUE appears on OPTION tags.

        // Do work for when attribute VALUE appears on INPUT tags.
        if( m_DuringTagInput
        && szValue != NULL )
        {
            // The VALUE attribute will be totalized afterwards,
            // when finished processing the current INPUT tag.

            // wip -- This hidden tag made garbage somehow:
            // at http://stariq.com/pagetemplate/ArticleSearch.asp
            // <input type=Hidden value=1 name="ShowImage">
            // Aha: if( m_FormInputName instead of m_FormInputValue.
            // So failed in such an example of VALUE before NAME!
            // #if DO_DEBUG_FORM1
            //     ; Spew( L"INPUT TAG VALUE ATTR doing input string:" );
            //     ; Spew( szValue );
            // #endif

            // Malloc to remember INPUT NAME.
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }
            wchar_t * atop = scan;
            for( ;; ) // find final null
            {
                if( *scan == NULL ) // determinate: stops on NULL
                    break;
                scan++;
            }
            for( ;; ) // back over final spaces
            {
                if( scan == atop
                || scan[-1] != ' ' ) // determinate: stops on atop
                    break;
                scan--;
            }
            // Allow to pass back empty strings... if( scan > atop )
            {
                // Save INPUT tag VALUE attr in a malloc until totalized.
                // First free any old pointer if somehow redundant value.
                if( m_FormInputValue != NULL )
                {
                    MyFree( 3337, UNPREDICTABLE, m_FormInputValue );
                    m_FormInputValue = NULL;
                }

                size_t nLen = scan - atop;
                size_t nMal = nLen + 1;
                m_FormInputValue = ( wchar_t * ) MyMalloc( 2536, nMal * sizeof( wchar_t ) );
                if( m_FormInputValue != NULL )
                {
                    memcpy( m_FormInputValue, atop, nLen * sizeof( wchar_t ) );
                    m_FormInputValue[nLen] = NULL;
                    #if DO_DEBUG_FORM1
                        ; Spew( L"INPUT VALUE:" );
                        ; Spew( m_FormInputValue );
                    #endif
                }
            }
        }
        // Do work for when attribute VALUE appears on OPTION tags.
        if( m_DuringTagOption
        && szValue != NULL )
        {
            // The VALUE attribute will be totalized afterwards,
            // when finished processing the current OPTION tag.

            // Malloc to remember INPUT NAME.
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }
            wchar_t * atop = scan;
            for( ;; ) // find final null
            {
                if( *scan == NULL ) // determinate: stops on NULL
                    break;
                scan++;
            }
            for( ;; ) // back over final spaces
            {
                if( scan == atop
                || scan[-1] != ' ' ) // determinate: stops on atop
                    break;
                scan--;
            }
            // Allow to pass back empty strings... if( scan > atop )
            {
                // Don't malloc until free any old pointer!
                if( m_FormOptionValue != NULL )
                {
                    MyFree( 3426, UNPREDICTABLE, m_FormOptionValue );
                    m_FormOptionValue = NULL;  // initialize locally relevant variable
                }
                size_t nLen = scan - atop;
                size_t nMal = nLen + 1;
                m_FormOptionValue = ( wchar_t * ) MyMalloc( 2536, nMal * sizeof( wchar_t ) );
                if( m_FormOptionValue != NULL )
                {
                    memcpy( m_FormOptionValue, atop, nLen * sizeof( wchar_t ) );
                    m_FormOptionValue[nLen] = NULL;
                    #if DO_DEBUG_FORM1
                        ; Spew( L"OPTION VALUE:" );
                        ; Spew( m_FormOptionValue );
                    #endif
                }
            }
        }
        break;

    case ATTR_ACTION:
        // There is a use of ACTION in FORM tag.
        // Typical attributes on FORM tag:
        // 162 QZ? method on form =  get
        //   4 QZ? method on form =  Post ( any capitalization )
        //  70 QZ? action on form =  /p/search ( relative or abs url )

        #if DO_DEBUG_FORM1
            ; SpewValue( L"ACTION", TagEnumHeldInLayer1Markup );
        #endif

        // Recognize and remember ACTION URL.
        // The end of the opening FORM tag will totalize this more.

        m_FormActionPresent = 1; // attribute present, not necessarily good.

        if( TagEnumHeldInLayer1Markup == TAG_FORM
        && szValue != NULL )
        {
            // heavy lifting #5

            #if DO_DEBUG_FORM1
                ; Spew( L"ACTION. Treat BASE and ACTION URL:" );
                ; Spew( m_pMalBase );
                ; Spew( szValue );
            #endif

            // I need to can'ize it, hang it on tree, remember it.

            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }

            // A little ACTION URL problem I need to sidestep:
            // action="javascript:redirectform( this.form );

            if( ( scan[0] | ' ' ) == 'j'
            && ( scan[1] | ' ' ) == 'a'
            && ( scan[2] | ' ' ) == 'v'
            && ( scan[3] | ' ' ) == 'a'
            && ( scan[4] | ' ' ) == 's'
            && ( scan[5] | ' ' ) == 'c'
            && ( scan[6] | ' ' ) == 'r'
            && ( scan[7] | ' ' ) == 'i'
            && ( scan[8] | ' ' ) == 'p'
            && ( scan[9] | ' ' ) == 't'
            && ( scan[10] | ' ' ) == ':' )
            {
                m_FormRejectedBitfield |= FORM_REJECT_JAVASCRIPT_ACTION_URL;
            }
            else
            {
                // So ( BASE+ACTION )=METHOD provided unique sorting URL.
                // Add terms "HIDDEN name=value" etc., to pSolFormAtoms.
                // At closing FORM tag, concoct one representative URL.

                int UrlIndex = Www.CombineAndHangUrl( m_pMalBase, szValue, 1 ); // 1=ForForm

                if( UrlIndex == 0 )
                {
                    m_FormRejectedBitfield |= FORM_REJECT_BAD_ACTION_URL;
                }
                else
                {
                    // Set CHtm members until closing /FORM totalizes them.
                    m_FormMethodUrlIndex = UrlIndex;

                    // I need to check for, or set a new COneForm in pVoid.
                    // Don't make a CHtm member for it; CSol owns COneForm.
                    // Therefore, all accessers must access it by index thus:
                    // Do CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );

                    COneForm * pOneForm = ( COneForm * ) CSolFormUrls.GetUserpVoid( m_FormMethodUrlIndex );
                    if( pOneForm == NULL )
                    {
                        pOneForm = new COneForm( m_FormMethodUrlIndex );
                        if( pOneForm != NULL )
                        {
                            CSolFormUrls.SetUserpVoid( m_FormMethodUrlIndex, pOneForm );
                        }
                    }

                    // Now, whether old or newly created, Add Action Atom.
                    // Rather than re-iterate joined/canonical ACTION URL,
                    // show the original ACTION attribute value.
                    if( pOneForm != NULL )
                    {
                        // wchar_t * pMalKey = CSolFormUrls.GetFullKey( m_FormMethodUrlIndex ); // a malloc, user frees
                        size_t nMal = wcslen( szValue ) + 7; // 6 for "ACTION", 1 for NULL
                        wchar_t * wk = ( wchar_t * ) MyMalloc( 2536, nMal * sizeof( wchar_t ) );
                        wcscpy( wk, L"ACTION" ); // space comes from szValue.
                        wcscpy( wk + 6, szValue ); // space comes from szValue.
                        pOneForm->pSolFormAtoms->AddKey( wk );
                        MyFree( 3493, UNPREDICTABLE, wk );
                    }
                }
            }

        }
        break;

    case ATTR_METHOD:
        // There is a use of METHOD in FORM tag.
        // Typical attributes on FORM tag:
        // 162 QZ? method on form =  get
        //   4 QZ? method on form =  Post ( any capitalization )
        //  70 QZ? action on form =  /p/search ( relative or abs url )

        #if DO_DEBUG_FORM1
            ; SpewValue( L"METHOD", TagEnumHeldInLayer1Markup );
        #endif

        // Recognize and remember Get or Post.
        // The end of the opening FORM tag will totalize this more.

        if( TagEnumHeldInLayer1Markup == TAG_FORM
        && szValue != NULL )
        {
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }
            if( ( scan[0] | ' ' ) == 'g'
            && ( scan[1] | ' ' ) == 'e'
            && ( scan[2] | ' ' ) == 't' )
            {
                m_FormGet1Post2 = 1;
            }
            if( ( scan[0] | ' ' ) == 'p'
            && ( scan[1] | ' ' ) == 'o'
            && ( scan[2] | ' ' ) == 's'
            && ( scan[3] | ' ' ) == 't' )
            {
                m_FormGet1Post2 = 2;
            }
            // else, leave 0, same as if no method was present.
        }
        break;

    case ATTR_SRC:
        // There is a use of SRC in IMG tag ( for an image ).
        // There is a use of SRC in INPUT tag ( for an image ).
        // There is a use of SRC in SCRIPT tag ( e.g., Javascript ).
        // None of these are useful to mine text right now...
        // Oh, but this is important -- Framesets!
        // If I were delicate, I'd wait for the name, but nah...
        // <FRAME SRC="crawlmain.html" name="crawl_main">

        #if DO_DEBUG_FRAME
            ; SpewValue( L"CHtm: ProcessOneAttribute: I reached the SRC clause for Tag Enum", TagEnumHeldInLayer1Markup );
        #endif

        if( TagEnumHeldInLayer1Markup == TAG_FRAME
        && szValue != NULL )
        {
            // heavy lifting #4

            #if DO_DEBUG_FRAME
                ; Spew( L"CHtm: ProcessOneAttribute: I reached SRC on FRAME. Ready to join BASE and URL:" );
                ; Spew( m_pMalBase );
                ; Spew( szValue );
            #endif

            // I need to can'ize it, hang it on tree, remember it.
            // CHtm constructor ( or TAG_BASE work ) set m_pMalBase.
            // If had an HTTP header, I have this URL in the tree.
            // During file load, I might not have a URL, get zero.

            int UrlIndex = Www.CombineAndHangUrl( m_pMalBase, szValue, 0 );

            #if DO_DEBUG_FRAME
                ; SpewValue( L"CHtm: ProcessOneAttribute: Hanging this non-null FRAME SRC:", UrlIndex );
            #endif

            // Since FRAME has no clear text for this anchor,
            // I might:
            // Insert "frame" into text, as done in Surf.exe;
            // Ignore frame tags, as far as affecting output;
            // keep frame tags another way, e.g., autofollow.

            // Let's try out the insert "frame" method here:
            if( UrlIndex > 1 )
            {
                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
                StartAnchorOffsetInText = m_pWsbPaperText->StrLen;
                AddToPlainText( L"[Frame]", 7, 0 ); // put this word; but not newline, newblock.
                EndingTagAOffsetInText = m_pWsbPaperText->StrLen;
                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.

                // perhaps unnecessary tests, but clone of another:
                if( LastTagAIDXHighWater <= StartAnchorOffsetInText
                && StartAnchorOffsetInText < EndingTagAOffsetInText )
                {
                    m_pIdxPaperIndex->AddIdx( StartAnchorOffsetInText, EndingTagAOffsetInText, UrlIndex, 0 );
                    LastTagAIDXHighWater = EndingTagAOffsetInText;

                    // New work to cross-reference links:
                    size_t iAnchorText = CSolAnchorText.AddKey( L"Frame" );
                    #if DO_DEBUG_ADDFIND
                    if( iAnchorText <= 1 )
                        { Spew( L"AddFind <= 1 at chtm 3830" ); }
                    #endif
                    AddAnchorHeadTailText( m_BestUrlIndex, UrlIndex, iAnchorText );
                }

                // In order to get FETCH to gather all FRAMEs,
                // I must add the UrlIndex as an itoa string
                // key to the m_pSolFrames that was passed in.

                // Some callers may pass a null m_pSolFrames.
                // In which case, they do not need this data.

                if( m_pSolFrames != NULL )
                {
                    wchar_t wk[20];
                    // wchar_t * _itow( int value, wchar_t *string, int radix );
                    _itow( UrlIndex, wk, 10 );
                    size_t index = m_pSolFrames->AddKey( wk );
                    #if DO_DEBUG_ADDFIND
                        if( index == 1 )
                            { Spew( L"AddFind 1 at chtm 2390" ); }
                    #endif
                }

                LastTagA_UrlIndex = 0; // cancel any TAG_A
                StartAnchorOffsetInText = -1;
                EndingTagAOffsetInText = -1;
            }
        }
        break;

    case ATTR_HREF:
        // Disclose all HREF="url". Use to annotate block.

        // There is a use of HREF in LINK tag. They do not excite me, like:
        // <link href="http://images.contactmusic.com/rss/contactmusic_rss.xml"
        // rel="alternate" type="application/rss+xml" title="Contactmusic Latest News">
        // 286 QZ? rel on link =  stylesheet
        //  97 QZ? rel on link =  shortcut icon
        //  61 QZ? rel on link =  alternate
        //  45 QZ? rel on link =  SHORTCUT ICON
        //  19 QZ? rel on link =  P3Pv1
        //  13 QZ? rel on link =  icon
        //   9 QZ? rel on link =  STYLESHEET
        //   5 QZ? rel on link =  next
        //   5 QZ? rel on link =  alternate stylesheet
        //   5 QZ? rel on link =  StyleSheet
        //   4 QZ? rel on link =  top
        //   4 QZ? rel on link =  search
        //   4 QZ? rel on link =  first
        //   4 QZ? rel on link =  copyright
        //   3 QZ? rel on link =  start
        //   1 QZ? rel on link =  styleSheet
        //   1 QZ? rel on link =  prev
        //

        // There is a use of HREF in A tag.
        // There is a use of HREF in AREA tag.
        // There is a use of HREF in BASE tag.

        if( TagEnumHeldInLayer1Markup == TAG_BASE
        && szValue != NULL )
        {
            // Save BASE HREFs when ready to canonicalize.
            // This can help if parse is due to OPEN FILE.

            #if DO_DEBUG_BASE
                ; Spew( L"CHtm: ProcessOneAttribute: Processing this non-null BASE HREF:" );
                ; Spew( szValue );
            #endif

            // I thought this out at home, but did not bring it.
            // In essence, callers may predict m_pMalBase, but
            // will NOT hang fruit in CSolAllUrls until after CHtm
            // parse may affect m_pMalBase from either TAG_BASE
            // or a first-line URL parsed during a file load.

            // Resolve the new url against empty BASE url:
            // heavy lifting #1

            int UrlIndex = Www.CombineAndHangUrl( NULL, szValue, 0 );
            if( UrlIndex > 1 )
            {
                m_BaseUrlIndex = UrlIndex;
                m_BestUrlIndex = m_BaseUrlIndex; // just for link xref work.

                m_pMalBase = ( wchar_t * ) CSolAllUrls.GetFullKey( m_BaseUrlIndex );
                #if DO_DEBUG_BASE
                    ; Spew( L"CHtm: ProcessOneAttribute: Setting a new m_pMalBase, and m_BaseUrlIndex:" );
                    ; SpewValue( ( ( m_pMalBase==NULL )?m_pMalBase:L"-null-" ), m_BaseUrlIndex );
                #endif
            }
        }

        if( TagEnumHeldInLayer1Markup == TAG_AREA
        && szValue != NULL )
        {
            // heavy lifting #2

            // I need to can'ize it, hang it on tree, remember it.
            // CHtm constructor ( or TAG_BASE work ) set m_pMalBase.
            // If had an HTTP header, I have this URL in the tree.
            // During file load, I might not have a URL, get zero.

            int UrlIndex = Www.CombineAndHangUrl( m_pMalBase, szValue, 0 );

            #if DO_DEBUG_AREA
                ; SpewValue( L"CHtm: ProcessOneAttribute: Hanging this non-null AREA HREF:", UrlIndex );
            #endif

            // Since AREA has no clear text for this anchor,
            // I might:
            // Insert "area" into text, as done in Surf.exe;
            // Ignore area tags, as far as affecting output;
            // keep area tags another way, e.g., autofollow.

            // Let's try out the insert "area" method here:
            if( UrlIndex > 1 )
            {
                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
                StartAnchorOffsetInText = m_pWsbPaperText->StrLen;
                AddToPlainText( L"[Area]", 6, 0 ); // put this word; but not newline, newblock.
                EndingTagAOffsetInText = m_pWsbPaperText->StrLen;
                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.

                // perhaps unnecessary tests, but clone of another:
                if( LastTagAIDXHighWater <= StartAnchorOffsetInText
                && StartAnchorOffsetInText < EndingTagAOffsetInText )
                {
                    m_pIdxPaperIndex->AddIdx( StartAnchorOffsetInText, EndingTagAOffsetInText, UrlIndex, 0 );
                    LastTagAIDXHighWater = EndingTagAOffsetInText;

                    // New work to cross-reference links:
                    size_t iAnchorText = CSolAnchorText.AddKey( L"Area" );
                    #if DO_DEBUG_ADDFIND
                    if( iAnchorText <= 1 )
                        { Spew( L"AddFind <= 1 at chtm 3965" ); }
                    #endif
                    AddAnchorHeadTailText( m_BestUrlIndex, UrlIndex, iAnchorText );
                }

                LastTagA_UrlIndex = 0; // cancel any TAG_A
                StartAnchorOffsetInText = -1;
                EndingTagAOffsetInText = -1;
            }
        }

        if( TagEnumHeldInLayer1Markup == TAG_A
        && szValue != NULL )
        {
            // It was extremely valuable to distinguish A tags
            // used as head anchors versus A tags used as tail
            // anchors, to assist the rules for word splitting.
            TagA_Had_Href = 1;

            // heavy lifting #3

            // I need to can'ize it, hang it on tree, remember it.
            // CHtm constructor ( or TAG_BASE work ) set m_pMalBase.
            // If had an HTTP header, I have this URL in the tree.
            // During file load, I might not have a URL, get zero.

            LastTagA_UrlIndex = Www.CombineAndHangUrl( m_pMalBase, szValue, 0 );

            #if DO_DEBUG_ANCHOR
                ; SpewValue( L"CHtm: ProcessOneAttribute: Hanging this non-null A HREF:", LastTagA_UrlIndex );
            #endif

            if( m_PageIsAQrp
            && LastTagA_UrlIndex > 1 ) // CYA from -1, 0, 1.
                AddOrTestQueryUrlFacts( LastTagA_UrlIndex );
        }
        break;




    case ATTR_LANG:
        // Attribute LANG may be near redundant xml:lang.

        // Ignore the attribute "language", used thus:
        // - language on script =  JavaScript
        // - language on a =  JavaScript

        // Attribute LANG appears mostly on HTML, A, SPAN.
        // Attribute LANG on HTML seems same as in header.
        // Attribute LANG on HTML may serve for open file.

        // Attribute LANG on SPAN often brackets an anchor.
        // Attribute LANG on SPAN may bracket single words.
        // Attribute LANG on A means anchor text uses that.
        // Attribute LANG on A means attr_title uses that.
        // Treat LANG on A or SPAN as queering all inside.

        // ! ============================================================= !
        // ! MAJOR: WE MAY PRESUME THAT TARGET URL IS IN THAT LANGUAGE TOO !
        // ! Therefore: Keep a language attribute on each URL & gate fetch !
        // ! ============================================================= !


        // Right now, treat LANG on HTML as synonymous with
        // META Content-Language. And clone that code here:
        if( TagEnumHeldInLayer1Markup == TAG_HTML
        && szValue != NULL ) // Oops. Here szValue was used w/o NULL test:
        {
            // I should be able to just use the whole value.
            // Remember to skip +1 for leading space.
            // Huh? Also take off the trailing space.
            size_t n = wcslen( szValue );
            szValue[n-1] = NULL;
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"Processing LANG attribute on HTML tag." );
            #endif
            m_nLangFound = BestIndexforLanguageString( szValue + 1, NULL );
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"Processing this LANG attribute on HTML tag:" );
                ; SpewValue( szValue + 1, m_nLangFound );
            #endif
        }
        break;


    case ATTR_HTTP_EQUIV:
        // Finally, getting back to work on the HTML parse/analysis!
        // Extract any charset information from a META tag. E.g.,
        // <meta
        //      http-equiv="Content-Type"
        //      content="text/html; charset=windows-1251"
        // >
        //
        // Now here is a problem. That is not a single attribute.
        // When I spew szValue here, I only see Content-type word.
        //
        // I can either have TAG_META case do a more complex parse,
        // or better, I can add a case for ATTR_CONTENT after this,
        // and either let ATTR_CONTENT or TAG_META use our results.

        // I must allow case-insensitive compare, maybe odd puncts,
        // but must distinguish other similar http-equiv values:
        // Note that a leading space is seen in Spew( szValue ).
        // Oh. One first and last space are passed in by design.
        // Oh. That was so I could surround alt text with [...].
        //  content-type
        //  Content-Type
        //  Content-Script-Type
        //  Content-Style-Type
        // szValue was put in a temporary, and is NULL terminated:
        // wchar_t AttributeValue[500];

        if( TagEnumHeldInLayer1Markup == TAG_META
        && szValue != NULL ) // Oops. Here szValue was used w/o NULL test:
        {
            // I got here with szValue = NULL, in fact, from the
            // ProcessTagAttributes call explicitly passing a NULL,
            // for second parameter, on some page that had a META tag
            // with "HTTP-EQUIV", and no value following it!
            // I need to make all users of szValue test for null end!
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan != ' ' ) // determinate: stops on NULL
                    break;
                scan++;
            }

            // Here, do: http-equiv="Content-Type"
            // Here, do: http-equiv="Content-Language"

            if( ( scan[0] | ' ' ) == 'c'
            && ( scan[1] | ' ' ) == 'o'
            && ( scan[2] | ' ' ) == 'n'
            && ( scan[3] | ' ' ) == 't'
            && ( scan[4] | ' ' ) == 'e'
            && ( scan[5] | ' ' ) == 'n'
            && ( scan[6] | ' ' ) == 't'
            && scan[7]  == '-' )
            {
                // split this into two endings...
                if( ( scan[8] | ' ' ) == 't'
                && ( scan[9] | ' ' ) == 'y'
                && ( scan[10] | ' ' ) == 'p'
                && ( scan[11] | ' ' ) == 'e' )
                {
                    #if DO_DEBUG_CONTENT || DO_DEBUG_LANGUAGE
                        ; Spew( L"ATTR_HTTP_EQUIV -- matched Content-Type." ); //
                    #endif
                    Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentType = 1;
                }
                if( ( scan[8] | ' ' ) == 'l'
                && ( scan[9] | ' ' ) == 'a'
                && ( scan[10] | ' ' ) == 'n'
                && ( scan[11] | ' ' ) == 'g'
                && ( scan[12] | ' ' ) == 'u'
                && ( scan[13] | ' ' ) == 'a'
                && ( scan[14] | ' ' ) == 'g'
                && ( scan[15] | ' ' ) == 'e' )
                {
                    #if DO_DEBUG_LANGUAGE
                        ; Spew( L"ATTR_HTTP_EQUIV -- matched Content-Language." );
                    #endif
                    Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentLanguage = 1;
                }
            }

            // Here, do: http-equiv="Refresh"
            // <meta http-equiv="Refresh" content="0;url=http://www...com/path/"/>

            if( ( scan[0] | ' ' ) == 'r'
            && ( scan[1] | ' ' ) == 'e'
            && ( scan[2] | ' ' ) == 'f'
            && ( scan[3] | ' ' ) == 'r'
            && ( scan[4] | ' ' ) == 'e'
            && ( scan[5] | ' ' ) == 's'
            && ( scan[6] | ' ' ) == 'h' )
            {
                Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_Refresh = 1;
            }

        }
        break;

    case ATTR_CONTENT:
        // Finally, getting back to work on the HTML parse/analysis!
        // Extract any charset information from a META tag. E.g.,
        // <meta
        //      http-equiv="Content-Type"
        //      content="text/html; charset=windows-1251"
        // >
        //
        // See problem comment in ATTR_HTTP_EQUIV.
        //
        // Clearly, ATTR_CONTENT appears on some TAG_META's other than
        // just ATTR_HTTP_EQUIV, e.g., containing textual descriptions.
        //
        // I think is is fair to assume that nobody would put the
        // ATTR_CONTENT before the ATTR_HTTP_EQUIV, so I will let
        // ATTR_HTTP_EQUIV arm a bool to permit ATTR_CONTENT work.
        // That bool should be cleared at the Tag level switch by
        // TAG_META recognition before doing ProcessTagAttributes.
        // In ProcessTagAttributes, ATTR_CONTENT tests ( TAG_META ).

        if( TagEnumHeldInLayer1Markup == TAG_META
        && Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentType
        && szValue != NULL ) // Oops. Here szValue was used w/o NULL test:
        {
            // Scan in szValue to recognize charset:
            // text/html;charset=iso-8859-1
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan == NULL ) // determinate: stops on NULL
                    break;
                if( ( scan[0] | ' ' ) == 'c'
                && ( scan[1] | ' ' ) == 'h'
                && ( scan[2] | ' ' ) == 'a'
                && ( scan[3] | ' ' ) == 'r'
                && ( scan[4] | ' ' ) == 's'
                && ( scan[5] | ' ' ) == 'e'
                && ( scan[6] | ' ' ) == 't' )
                {
                    // This far in, do whatever and break outer loop.
                    scan += 7; // past "charset"
                    for( ;; ) // skip any spaces and equals:
                    {
                        if( *scan != ' '  // determinate: stops on NULL
                        && *scan != '=' ) // determinate: stops on NULL
                        {
                            // I must be at the name to look up:
                            #if DO_DEBUG_CONTENT
                                ; Spew( L"HTTP_EQUIV ContentType Charset" );
                                ; Spew( scan );
                            #endif
                            // What shall I do with it? Validate it?
                            // Subroutine will return something valid.
                            // CHtm does not have access to pOnePaper.
                            // CHtm user can examine this member after:
                            m_nCSetFound = BestIndexforCharsetString( scan, NULL );
                            break;
                        }
                        scan++; // inner loop
                    }
                    break;
                }
                scan++; // outer loop
            }
        }

        if( TagEnumHeldInLayer1Markup == TAG_META
        && Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentLanguage
        && szValue != NULL ) // Oops. Here szValue was used w/o NULL test:
        {
            // I should be able to just use the whole value.
            // Remember to skip +1 for leading space.
            // ATTR_HTTP_EQUIV -- matched Content-Language.
            // BestIndex: try 1. Verbatim
            // en-us : 0
            // BestIndex: unrecognized
            // BestIndex: try 2. Lowercased
            // en-us : 0
            // BestIndex: unrecognized
            // Huh? Also take off the trailing space.
            size_t n = wcslen( szValue );
            szValue[n-1] = NULL;
            m_nLangFound = BestIndexforLanguageString( szValue + 1, NULL );
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"Processing ContentLanuage attribute on meta tag:" );
                ; SpewValue( szValue + 1, m_nLangFound );
            #endif
        }


        if( TagEnumHeldInLayer1Markup == TAG_META
        && Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_Refresh
        && szValue != NULL ) // Oops. Here szValue was used w/o NULL test:
        {
            // Scan in szValue to recognize url. E.g.,
            // <meta http-equiv="Refresh" content="0;url=http://www...com/path/"/>
            wchar_t * scan = szValue;
            for( ;; ) // skip over any leading spaces.
            {
                if( *scan == NULL ) // determinate: stops on NULL
                    break;
                if( ( scan[0] | ' ' ) == 'u'
                && ( scan[1] | ' ' ) == 'r'
                && ( scan[2] | ' ' ) == 'l' )
                {
                    // This far in, do whatever and break outer loop.
                    scan += 3; // past "url"
                    for( ;; ) // skip any spaces and equals:
                    {
                        if( *scan != ' '  // determinate: stops on NULL
                        && *scan != '=' ) // determinate: stops on NULL
                        {
                            // I must be at the name to look up:
                            #if DO_DEBUG_REFRESH
                                ; Spew( L"HTTP_EQUIV Refresh" );
                                ; Spew( scan );
                            #endif
                            // What shall I do with it? Just another URL.
                            // I can either treat it with urgency, as if
                            // 3xx redirection, or less so, like a frame.

                            // heavy lifting #6, refresh ( cloned from #4, frame )

                            #if DO_DEBUG_REFRESH
                                ; Spew( L"CHtm: ProcessOneAttribute: I reached URL on REFRESH. Ready to join BASE and URL:" );
                                ; Spew( m_pMalBase );
                                ; Spew( scan );
                            #endif

                            // I need to can'ize it, hang it on tree, remember it.
                            // CHtm constructor ( or TAG_BASE work ) set m_pMalBase.
                            // If had an HTTP header, I have this URL in the tree.
                            // During file load, I might not have a URL, get zero.

                            int UrlIndex = Www.CombineAndHangUrl( m_pMalBase, scan, 0 );

                            #if DO_DEBUG_FRAME
                                ; SpewValue( L"CHtm: ProcessOneAttribute: Hanging this non-null REFRESH URL:", UrlIndex );
                            #endif

                            // Since REFRESH has no clear text for this anchor,
                            // Insert "refresh" into text as the anchor text;
                            // keep frame tags another way, e.g., autofollow.

                            // Let's try out the insert "refresh" method here:
                            if( UrlIndex > 1 )
                            {
                                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.
                                StartAnchorOffsetInText = m_pWsbPaperText->StrLen;
                                AddToPlainText( L"[Refresh]", 9, 0 ); // put this word; but not newline, newblock.
                                EndingTagAOffsetInText = m_pWsbPaperText->StrLen;
                                ProcessTextTokenBuffer( 2 ); // put any word, newline, newblock.

                                // perhaps unnecessary tests, but clone of another:
                                if( LastTagAIDXHighWater <= StartAnchorOffsetInText
                                && StartAnchorOffsetInText < EndingTagAOffsetInText )
                                {
                                    m_pIdxPaperIndex->AddIdx( StartAnchorOffsetInText, EndingTagAOffsetInText, UrlIndex, 0 );
                                    LastTagAIDXHighWater = EndingTagAOffsetInText;

                                    // New work to cross-reference links:
                                    size_t iAnchorText = CSolAnchorText.AddKey( L"Refresh" );
                                    #if DO_DEBUG_ADDFIND
                                    if( iAnchorText <= 1 )
                                        { Spew( L"AddFind <= 1 at chtm 4300" ); }
                                    #endif
                                    AddAnchorHeadTailText( m_BestUrlIndex, UrlIndex, iAnchorText );
                                }

                                // In order to get FETCH to follow a REFRESH,
                                // I must add the UrlIndex as an itoa string
                                // key to the m_pSolFrames that was passed in.

                                // Some callers may pass a null m_pSolFrames.
                                // In which case, they do not need this data.

                                if( m_pSolFrames != NULL )
                                {
                                    wchar_t wk[20];
                                    // wchar_t * _itow( int value, wchar_t *string, int radix );
                                    _itow( UrlIndex, wk, 10 );
                                    size_t index = m_pSolFrames->AddKey( wk );
                                    #if DO_DEBUG_ADDFIND
                                        if( index == 1 )
                                            { Spew( L"AddFind 1 at chtm 2390" ); }
                                    #endif
                                }

                                LastTagA_UrlIndex = 0; // cancel any TAG_A
                                StartAnchorOffsetInText = -1;
                                EndingTagAOffsetInText = -1;
                            }
                            break;
                        }
                        scan++; // inner loop
                    }
                    break;
                }
                scan++; // outer loop
            }
        }
        break;


    default:
        break;
    }

}

void CHtm::ProcessAlternateText( wchar_t * szValue )
{
    #if DO_DEBUG_CALLS
        Routine( L"167" );
    #endif


    // Infinite loop during ^D x:\ie, jap page,
    // shows that atop=past in call to AddToPlainText,
    // and they are on an 0x13 byte value, which my
    // loop cannot get past ....
    //
    // The problem is that these two inner loop phases
    // are not exactly complementary tests:
    //     while( *scan > ' ' ) // determinate: stops on null
    //         scan ++; // skip over printables
    //     while( *scan == ' ' ) // determinate: stops on null
    //         scan ++; // skip over spaces
    //
    // The solution is to change to some perfectly complementary
    // expression, like iswspace( ), and add a separate null test.

    // I have to tokenize szValue.
    // Also bracket it using [, ].
    // Also provide spaces around.
    int DidAny = 0;
    wchar_t * scan = szValue;
    wchar_t * atop = NULL;
    wchar_t * past = NULL;
    for( ;; )
    {
        // My caller left me a first space that I may overwrite.
        while( iswspace( *scan ) ) // determinate: stops on null
            scan ++; // skip over spaces

        if( *scan == NULL )
            break;

        // we have a token.
        atop = scan;

        if( ! DidAny )
        {
            DidAny = 1;
            // this is first token. Prefix '[' into caller's buffer.
            // ...unless the web page aready put one there.
            if( atop[0] != '[' )
            {
                atop --;
                atop[0] = '[';
            }
        }
        PendingInsertSpaceChar = 1;

        while( *scan != NULL
        && ! iswspace( *scan ) ) // determinate: stops on null
            scan ++; // skip over printables

        past = scan;

        // I need to skip spaces now to see if will end.
        while( iswspace( *scan ) ) // determinate: stops on null
            scan ++; // skip over spaces

        if( *scan == NULL
        && DidAny )
        {
            // My caller left me a final space that I may overwrite.
            // ( That beats alternative-overwriting NULL terminator. )
            // this is final token. Append ']' into caller's buffer.
            // ...unless the web page aready put one there.
            if( past[-1] != ']' )
            {
                past ++;
                past[-1] = ']';
            }
        }
        PendingInsertSpaceChar = 1;
        AddToPlainText( atop, past - atop, 0 ); // put this word; but not newline, newblock.
        PendingInsertSpaceChar = 1;
    }
}

void CHtm::InitializeTitleOrAnchorTextBuffer( )
{
    #if DO_DEBUG_CALLS
        // Routine( L"168" );
    #endif

    // I do not do this for anchor text any more.
    // Yes, restoring this for Link cross references.

    // using...szTitleOrAnchorBuffer
    // using...MAX_TITLE_OR_ANCHOR
    // using...TitleOrAnchorBufferStringLength

    TitleOrAnchorBufferStringLength = 0;
    SavingTitleOrAnchorText = 1;

    #if DO_DEBUG_TITLE
        ; Spew( L"InitializeTitle" );
    #endif

}

void CHtm::FinalizeTitleOrAnchorTextBuffer( )
{
    #if DO_DEBUG_CALLS
        Routine( L"169" );
    #endif

    // I do not do this for anchor text any more.
    // Yes, restoring this for Link cross references.

    // Add it to an RBSL, return the index found.
    // using...szTitleOrAnchorBuffer
    // using...MAX_TITLE_OR_ANCHOR
    // using...TitleOrAnchorBufferStringLength

    // RBSL? That was my red-black-sorted-list before they became CSol.
    // Title work has been languishing, until Jun 20 2007.

    #if DO_DEBUG_TITLE
        ; Spew( L"FinalizeTitle" );
    #endif

    SavingTitleOrAnchorText = 0;

    if( TitleOrAnchorBufferStringLength > 0 )
    {
        // rid any final space ( never in [0] ) placed after each token.
        if( szTitleOrAnchorBuffer[ TitleOrAnchorBufferStringLength - 1 ] == ' ' )
            TitleOrAnchorBufferStringLength --;

    }
    szTitleOrAnchorBuffer[ TitleOrAnchorBufferStringLength ] = NULL;

    #if DO_DEBUG_TITLE
        ; Spew( szTitleOrAnchorBuffer );
    #endif

}

void CHtm::ProcessTextTokenBuffer( int nNewlines )
{
    #if DO_DEBUG_CALLS
        // Routine( L"170" );
    #endif
    // A token here means a run of printable chars with no whitespace
    // and no HTML tags, and with any HTML entities already resolved.


    #if DO_DEBUG_TEXTTOKEN
    {
        if( IndexHeldInLayer2TokenText > 0 )
        {
            TextHeldInLayer2TokenText [ IndexHeldInLayer2TokenText ] = NULL;
            ; SpewValue( TextHeldInLayer2TokenText, nNewlines );
        }
        else
        {
            ; SpewValue( L"-empty-", nNewlines );
        }
    }
    #endif

    // Got Tokens?
//    if( IndexHeldInLayer2TokenText > 0 )
//    {
//        TextHeldInLayer2TokenText [ IndexHeldInLayer2TokenText ] = NULL;
//        AddToDebugTags( ( char * ) TextHeldInLayer2TokenText );
//        AddToDebugTags( "\r\n" );
//    }

    // One obvious task is to copy the token to output text buffer.

    // nNewlines enumerates some old-style boundary strategies:
    // 0 = do not force a newline after token.
    // 1 = ensure a first newline after token.
    // 2+ = ensure a second newline after token.
    // However, I may rethink these things.

    if( BitMaskReasonsToDiscardText == 0 )
    {
        // Many of my callers pass a zero meaning only to flush
        // out some token in progress; But if there is no token
        // in progress, all that work of next routine does nada.
        if( nNewlines > 0
        || IndexHeldInLayer2TokenText > 0 )
        {
            AddToPlainText( TextHeldInLayer2TokenText,
                IndexHeldInLayer2TokenText, nNewlines );
        }

        // while still only doing cleartext tokens ( avoid script, titles )
        if( m_PageIsAQrp
        && IndexHeldInLayer2TokenText > 0 )
            UseTokenTextInPageScrapingFacts( );
    }


    // Before we throw tokens away..... ?

    // I may need to add each token to web page's title buffer.
    if( IndexHeldInLayer2TokenText > 0
    && SavingTitleOrAnchorText )
    {
        TextHeldInLayer2TokenText [ IndexHeldInLayer2TokenText ] = NULL;
        #if DO_DEBUG_TITLE
            ; SpewTwo( L"TitleToken", TextHeldInLayer2TokenText );
        #endif

        size_t Len = IndexHeldInLayer2TokenText + 1; // one for postfix space
        if( TitleOrAnchorBufferStringLength + Len < MAX_TITLE_OR_ANCHOR )
        {
            memcpy( szTitleOrAnchorBuffer + TitleOrAnchorBufferStringLength,
            TextHeldInLayer2TokenText, IndexHeldInLayer2TokenText * sizeof( wchar_t ) );

            TitleOrAnchorBufferStringLength += IndexHeldInLayer2TokenText;

            szTitleOrAnchorBuffer [ TitleOrAnchorBufferStringLength++ ] = ' ';
        }
    }



    IndexHeldInLayer2TokenText = 0; // Now consumed.
}


void CHtm::AddToPlainText( wchar_t * szFragment, int FragmentLength, int nNewlines )
{
    #if DO_DEBUG_CALLS
        // Routine( L"171" );
    #endif
    if( FragmentLength != 0 )
    {
        // Append non-empty string to line if it fits, else to next line.
        // If it fits? Token ALWAYS fits. I'm doing no word wrap in here.

        if( CurrentColumnInOutputText == 0 )
        {
            // Favorite is one space before first word of wrapped text.
            // However, better if no leading space, so that Interleave
            // and CHtm parser produce idempotent output / input texts.

            // Life always gets more complicated. Allow for extra indents.
            if( IndentationColumnInOutputText > 0
            && IndentationColumnInOutputText <= 22 )
            {
                // Add from 1 to 22 spaces of this 22 space string:
                m_pWsbPaperText->Addn( L"                      ", IndentationColumnInOutputText );
            }
            CurrentColumnInOutputText += IndentationColumnInOutputText; // for indent
        }
        else if( PendingInsertSpaceChar )
        {
            // Usually, obey clues left from html tags and input whitespace,
            // about the space that comes BEFORE the token now being output.
            m_pWsbPaperText->Add( L" " );
            CurrentColumnInOutputText += 1; // for one space before token
        }

        // Parts of a token MIGHT be added to Title or Anchor text too.
        // No, I think I will find text later using the new IDX scheme.
        if( ArmToSaveStartAnchorOffset )
        {
            ArmToSaveStartAnchorOffset = 0;
            // Move it more precisely to this first letter of a token:
            StartAnchorOffsetInText = m_pWsbPaperText->StrLen;
        }

        // CHTM parser shall now recognize cleartext urls, and add links:
        // No, I made a post-scan of both HTML and TEXT over in CPag.

        {
            // Finally, I add the token to output:
            m_pWsbPaperText->Addn( szFragment, FragmentLength );
        }


        CurrentColumnInOutputText += FragmentLength;

        ConsecutiveNewlinesInOutputText = 0;

        // I must only disarm inside the IF, lest some callers ( Viz., TAG_A )
        // passing 0 cause me to disarm before the next token is processed.
        PendingInsertSpaceChar = 0; // disarm clue for next call
    }

    if( nNewlines > 0
    && ConsecutiveNewlinesInOutputText == 0 )
    {
        m_pWsbPaperText->Add( L"\r\n" );
        ConsecutiveNewlinesInOutputText = 1;
        CurrentColumnInOutputText = 0;
    }

    // Doing 2 is not subordinate to doing 1.
    if( nNewlines > 1
    && ConsecutiveNewlinesInOutputText == 1 )
    {
        m_pWsbPaperText->Add( L"\r\n" );
        ConsecutiveNewlinesInOutputText = 2;
    }
}

void CHtm::ChangeTextIndentation( )
{
    #if DO_DEBUG_CALLS
        Routine( L"172" );
    #endif
    // This services the starting tag.
    // Possibly slightly negative, for DT, dictionary term?

    // Later, make proper state variables so /DL comes back to DL level.
    // I found one joker put two DD's per DT, so rapidly indents to max.
    // Another only used one of tags ( dd or dt, I forget ) over and over.
    // I can fix both those by only honoring one-in-a-row of DT or DD,
    // resetting that flag on alternate tag; but pushing in stack idea:

    // Old comment says that saving indent before DL and setting at /DL
    // made situation worse. That's probably due to nested DL tags, for
    // which I should keep a stack of DL, or in fact, all UL/OL/DL tags.

    // Here is a URL that indents much to test:
    // http://www.friesian.com/apology.htm
    // Here is worse, like 20 DT indents w/o DD to balance:
    // http://dictionary.reference.com/browse/vortex


    switch( TagEnumHeldInLayer1Markup )
    {
        case TAG_BLOCKQUOTE:
        case TAG_UL:
        case TAG_OL:
            if( IndentationColumnInOutputText < 20 )
                IndentationColumnInOutputText += 2;
            break;

        case TAG_DL:
            IndentationColumnBeforeDlTag = IndentationColumnInOutputText;
            if( IndentationColumnInOutputText < 20 )
                IndentationColumnInOutputText += 2;
            break;

        case TAG_DD:
            if( IndentationColumnInOutputText < 20 )
                IndentationColumnInOutputText += 1;
            break;

        case TAG_DT:
            if( IndentationColumnInOutputText >= 3 )
                IndentationColumnInOutputText -= 1;
            break;
    }
}

void CHtm::RestoreTextIndentation( )
{
    #if DO_DEBUG_CALLS
        Routine( L"173" );
    #endif
    // This services the ending tag.
    // Later: Restore it to the balancing open token.
    // For now, only done for visible case of very bad DL usage.

    switch( TagEnumHeldInLayer1Markup )
    {
        case TAG_BLOCKQUOTE:
        case TAG_UL:
        case TAG_OL:
            if( IndentationColumnInOutputText >= 2 )
                IndentationColumnInOutputText -= 2;
            break;

        case TAG_DL:
            // Sorry, this simple approach made it worse yet:
            // IndentationColumnInOutputText = IndentationColumnBeforeDlTag;
            if( IndentationColumnInOutputText >= 2 )
                IndentationColumnInOutputText -= 2;
            break;

        // none for ending... case TAG_DT:
        // none for ending... case TAG_DD:
    }
}

void CHtm::FormAddNameValuePair( wchar_t * szName, wchar_t * szValue )
{
    #if DO_DEBUG_FORM1
        ; Spew( L"FormAddNameValuePair:" );
        ; Spew( ( szName == NULL ) ? L"-null-" :szName );
        ; Spew( ( szValue == NULL ) ? L"-null-" :szValue );
    #endif

    if( szName == NULL ) // It happens, but makes no sense, so do nothing.
        return;

    if( m_FormParamCount == 0 ) // controls '?' or '&' introducer
        m_pWsbFormParts->Add( L"?" );
    else
        m_pWsbFormParts->Add( L"&" );

    if( szName != NULL ) // It makes no sense, but allow for empty here too.
        m_pWsbFormParts->Add( szName );
    m_pWsbFormParts->Add( L"=" );
    if( szValue != NULL ) // frequently value is empty
        m_pWsbFormParts->Add( szValue );

    m_FormParamCount ++;
}

void CHtm::AddOrTestQueryUrlFacts( size_t UrlIndex )
{
    // Break out interesting facts about the query URL, like:
    // domain, path parts, parameters, whether matches query URL, etc...

    // Nothing like crash / stack overflow to spoil your day
    #if DO_DEBUG_SCRAPING
        ; SpewValue( L"AddQUrlFacts UrlIndex", UrlIndex );
    #endif

    wchar_t * pMalUrl = ( wchar_t * ) CSolAllUrls.GetFullKey( UrlIndex );
    if( pMalUrl == NULL )
    {
        #if DO_DEBUG_SCRAPING
            ; Spew( L"AddQUrlFacts CSolAllUrls.GetFullKey failed" );
        #endif
        AddOrTestQueryFact( WHERENESS_BALD, L"--Error: CSolAllUrls.GetFullKey failed" );
        return;
    }

    URL_COMPONENTS UrlComponents;
    memset( & UrlComponents, 0, sizeof( URL_COMPONENTS ) );
    UrlComponents.dwStructSize = sizeof( URL_COMPONENTS );

    size_t nLen = wcslen( pMalUrl );

    // Leave pointers NULL and sizes non-zero to get pointers into my URL.
    UrlComponents.dwSchemeLength    = nLen;
    UrlComponents.dwHostNameLength  = nLen;
    UrlComponents.dwUrlPathLength   = nLen;
    UrlComponents.dwExtraInfoLength = nLen;

    int success = InternetCrackUrl( pMalUrl, 0, 0, & UrlComponents );

    // I can't free it yet, my sub pointers are all in that malloc!

    if( ! success )
    {
        #if DO_DEBUG_SCRAPING
            ; Spew( L"AddQUrlFacts InternetCrackUrl failed" );
        #endif

        AddOrTestQueryFact( WHERENESS_BALD, L"--Error: InternetCrackUrl failed" );
        MyFree( 5027, zx, pMalUrl );
        return;
    }

    // Nothing like crash / stack overflow to spoil your day
    #if DO_DEBUG_SCRAPING
        ; SpewValue( L"UrlComponents.dwSchemeLength   ", UrlComponents.dwSchemeLength    );
        ; SpewValue( L"UrlComponents.dwHostNameLength ", UrlComponents.dwHostNameLength  );
        ; SpewValue( L"UrlComponents.dwUrlPathLength  ", UrlComponents.dwUrlPathLength   );
        ; SpewValue( L"UrlComponents.dwExtraInfoLength", UrlComponents.dwExtraInfoLength );
    #endif

    if( UrlComponents.lpszHostName != NULL
    && UrlComponents.dwHostNameLength > 0
    && UrlComponents.dwHostNameLength < 100 )
    {
        wchar_t wk[150];

        wchar_t save = UrlComponents.lpszHostName[ UrlComponents.dwHostNameLength ];
        UrlComponents.lpszHostName[ UrlComponents.dwHostNameLength ] = NULL;

        // I will pass a NULL, as I will not discard the final .com etc.
        wsprintf( wk, L"DOMAIN \"%s\"", UrlComponents.lpszHostName );
        AddOrTestQueryFact( WHERENESS_TREBLE, wk ); // full domain

        // I will pass a NULL, as I will not discard the final .com etc.
        wsprintf( wk, L"DOMAIN \"%s\"",
            PointPastDomainPrefix( UrlComponents.lpszHostName, NULL ) );
        AddOrTestQueryFact( WHERENESS_TREBLE, wk ); // trimmed domain

        UrlComponents.lpszHostName[ UrlComponents.dwHostNameLength ] = save;
    }

    if( UrlComponents.lpszUrlPath != NULL
    && UrlComponents.dwUrlPathLength > 0
    && UrlComponents.dwUrlPathLength < 300 )
    {
        wchar_t wk[450];

        wchar_t save = UrlComponents.lpszUrlPath[ UrlComponents.dwUrlPathLength ];
        UrlComponents.lpszUrlPath[ UrlComponents.dwUrlPathLength ] = NULL;

        // They all include the leading slash.
        // Only report bare slash if pathless.
        // For rest, re-report up to each slash.
        // Some may include a final slash ( keep ).
        // good/bad url has PATH "/"
        // good/bad url has PATH "/image/default"
        // good/bad url has PATH "/audio/default"
        // good/bad url has PATH "/video/default"
        // good/bad url has PATH "/web/ffset"
        // good/bad url has PATH "/ent/dead/"
        // good/bad url has PATH "/ent/dead/history/"
        // good/bad url has PATH "/mex_/muertos.html"
        // Oh, that's funky:
        // good/bad url has PATH "/help/search//default"

        // Go ahead and do the whole thing first

        wsprintf( wk, L"PATH \"%s\"",
            UrlComponents.lpszUrlPath );

        AddOrTestQueryFact( WHERENESS_TREBLE, wk );

        UrlComponents.lpszUrlPath[ UrlComponents.dwUrlPathLength ] = save;

        // Now break it after each /, except the first.

        if( UrlComponents.dwUrlPathLength > 1 )
        {
            int i = 1;
            int n = UrlComponents.dwUrlPathLength;
            for( ;; )
            {
                if( UrlComponents.lpszUrlPath[i] == '/' )
                {
                    save = UrlComponents.lpszUrlPath[i];
                    UrlComponents.lpszUrlPath[i] = NULL;

                    wsprintf( wk, L"PATH IN \"%s\"",
                        UrlComponents.lpszUrlPath );
                    AddOrTestQueryFact( WHERENESS_TREBLE, wk );

                    UrlComponents.lpszUrlPath[i] = save;
                }
                if( ++i == n ) // determinate: n > 1.
                    break;
            }
        }

    }

    MyFree( 5061, zx, pMalUrl );

}

void CHtm::UseTokenTextInPageScrapingFacts( )
{
    // Emit the token to the page scraping facts.

    // Aside from clearing them out at the start and end of tag A,
    // this is the only place that deals with the prior token, and
    // with recognizing various common types of NUMBER variants to
    // emit them in parallel with emitting the literal QRP "token".

    // Hey - I should also recognize Cleartext URLs!
    // Perhaps another routine already recognizes Cleartext URLs?
    // Indeed, I already call CPag::ClearTextTokenResemblesUrl.
    // However, that is during the scan as text. I need to get at it.

    // Caller tested to guarantee length > 0.
    // Rather than reject too-long tokens, trim them.
    int nTok = IndexHeldInLayer2TokenText;
    if( nTok > 50 )
        nTok = 50;

    wchar_t TheToken[51];
    memcpy( TheToken, TextHeldInLayer2TokenText, nTok * sizeof( wchar_t ) );
    TheToken[ nTok ] = NULL;

    wchar_t wk[200]; // Room for 2 tokens + other words.

    // Note this fixed size too: Only this routine uses it.
    // wchar_t PriorTokenUnder50[51]; // room for just 50 chars.

    // First, categorize the token as having leading numbers:
    // NO_NUMBER
    // NUMBER_STRICT
    // NUMBER_POINTS
    // NUMBER_COMMAS
    // NUMBER_HYPHENS
    // NUMBER_SLASHS
    // NUMBER_COLONS
    // NUMBER_MIXED

    int TokenType = NO_NUMBER;
    wchar_t * scan = TheToken;

    if( iswdigit( TheToken[0] ) )
    {
        int point = 0;
        int comma = 0;
        int hyphen = 0;
        int slash = 0;
        int colon = 0;
        for( ;; )
        {
            scan ++; // pre-increment to next char
            if( *scan == NULL )
                break;

            if( iswdigit( *scan ) )
                continue;

            // All other punct must be followed by a digit.
            if( iswdigit( scan[1] ) )
            {
                if( *scan == '.' )
                {
                    point = 1;
                    TokenType = NUMBER_POINTS;
                    continue;
                }
                if( *scan == ',' )
                {
                    comma = 1;
                    TokenType = NUMBER_COMMAS;
                    continue;
                }
                if( *scan == '-' )
                {
                    hyphen = 1;
                    TokenType = NUMBER_HYPHENS;
                    continue;
                }
                if( *scan == '/' )
                {
                    slash = 1;
                    TokenType = NUMBER_SLASHS;
                    continue;
                }
                if( *scan == ':' )
                {
                    colon = 1;
                    TokenType = NUMBER_COLONS;
                    continue;
                }
            }
            break; // stop on first non digit, or . or ,
        }
        int sum = point + comma + hyphen + slash + colon;
        if( sum == 0 )
            TokenType = NUMBER_STRICT;
        if( sum > 1 )
            TokenType = NUMBER_MIXED;
        // scan is at first non-number, or final NULL.
    }

    // several cases for the current token:
    wsprintf( wk, L"TOKEN \"%s\"", TheToken );
    AddOrTestQueryFact( WHERENESS_TREBLE, wk );
    if( TokenType != NO_NUMBER )
    {
        if( *scan == NULL )
            wsprintf( wk, L"TOKEN %s", NumTypes[TokenType] );
        else
            wsprintf( wk, L"TOKEN %s+\"%s\"", NumTypes[TokenType], scan );
        AddOrTestQueryFact( WHERENESS_TREBLE, wk );
    }

    // Now let's consider two tokens: prior token, and this token.
    // The cases are multiplying! What riches!
    // To simplify, only treat the prior literally when it is not a number.

    if( PriorTokenIsNumberEnum == NO_NUMBER )
    {
        // Prior token as literal; current token all ways.
        if( PriorTokenUnder50[0] != NULL )
        {
            // Repeat my set of current cases.
            wsprintf( wk, L"TWO TOKENS \"%s\" \"%s\"", PriorTokenUnder50, TheToken );
            AddOrTestQueryFact( WHERENESS_TREBLE, wk );
            if( TokenType != NO_NUMBER )
            {
                if( *scan == NULL )
                    wsprintf( wk, L"TWO TOKENS \"%s\" %s", PriorTokenUnder50, NumTypes[TokenType] );
                else
                    wsprintf( wk, L"TWO TOKENS \"%s\" %s+\"%s\"", PriorTokenUnder50, NumTypes[TokenType], scan );
                AddOrTestQueryFact( WHERENESS_TREBLE, wk );
            }
        }
    }
    else
    {
        // Now I have a 4-way if:
        // outer on prior ( numbers with text, versus numbers only ),
        // inner on current ( non numbers, or numbers w or wo text ):
        if( PriorTokenUnder50[0] == NULL )
        {
            // Prior token as NUMBER only; current token all ways.
            // Repeat my set of current cases.
            wsprintf( wk, L"TWO TOKENS %s \"%s\"", NumTypes[PriorTokenIsNumberEnum], TheToken );
            AddOrTestQueryFact( WHERENESS_TREBLE, wk );
            if( TokenType != NO_NUMBER )
            {
                if( *scan == NULL )
                    wsprintf( wk, L"TWO TOKENS %s %s", NumTypes[PriorTokenIsNumberEnum], NumTypes[TokenType] );
                else
                    wsprintf( wk, L"TWO TOKENS %s %s+\"%s\"", NumTypes[PriorTokenIsNumberEnum], NumTypes[TokenType], scan );
                AddOrTestQueryFact( WHERENESS_TREBLE, wk );
            }
        }
        else
        {
            // Prior token as NUMBER+literal; current token all ways.
            // Repeat my set of current cases.
            wsprintf( wk, L"TWO TOKENS %s+\"%s\" \"%s\"", NumTypes[PriorTokenIsNumberEnum], PriorTokenUnder50, TheToken );
            AddOrTestQueryFact( WHERENESS_TREBLE, wk );
            if( TokenType != NO_NUMBER )
            {
                if( *scan == NULL )
                    wsprintf( wk, L"TWO TOKENS %s+\"%s\" %s", NumTypes[PriorTokenIsNumberEnum], PriorTokenUnder50, NumTypes[TokenType] );
                else
                    wsprintf( wk, L"TWO TOKENS %s+\"%s\" %s+\"%s\"", NumTypes[PriorTokenIsNumberEnum], PriorTokenUnder50, NumTypes[TokenType], scan );
                AddOrTestQueryFact( WHERENESS_TREBLE, wk );
            }
        }
    }

    // Now save the current token to become next half of two tokens.
    // If current token is a number type, only copy the suffix part.

    if( TokenType == NO_NUMBER )
        wcscpy( PriorTokenUnder50, TheToken );
    else
        wcscpy( PriorTokenUnder50, scan );

    PriorTokenIsNumberEnum = TokenType;

}

void CHtm::AddOrTestQueryFact( int Whereness, wchar_t * szFact )
{
    // Whenever m_PageIsAQrp, we will make a test of facts.
    // All my callers have verified this bool is true:
    // if( m_PageIsAQrp )...

    // Furthermore, if( m_AppendPageScrapingFacts ) is also true,
    // add page scraping facts to verified-non-null m_pWsbQrpFacts.

    #if DO_DEBUG_FACTTEST
        SpewTwo( L"AddOrTestQueryFact", szFact );
    #endif

    // examples:
    // good/bad url has prior/next/anchor token "..."number
    // good/bad url has domain x


    // Here is a block of annotations only:
    if( m_AppendPageScrapingFacts )
    {
        // For all my callers, I will supply "Fact: " and newline:
        m_pWsbQrpFacts->Add( L"Fact: " );

        switch( Whereness )
        {
        case WHERENESS_BALD:
            break;
        case WHERENESS_TREBLE:
            if( SavingTitleOrAnchorText )
                m_pWsbQrpFacts->Add( L"anchor " );
            else
                m_pWsbQrpFacts->Add( L"prior/next " );
            break;
        default:
            m_pWsbQrpFacts->Add( L"---ERROR: INVALID WHERENESS---" );
            break;
        }
        m_pWsbQrpFacts->Add( szFact );
        m_pWsbQrpFacts->Add( L"\r\n" );
    }

    {
        // Constructor did CYA: Surl is non-null.
        WCPUI * p = m_pOneSurl->pRuleVector;
        size_t  n = m_pOneSurl->nRuleVector;
        if( p != NULL
        && n > 0 )
        {
            size_t i = 0;
            for( ;; )
            {
                if( i == n )
                    break;
                WCPUI * pi = p + i;
                wchar_t * wcp = pi->wcp;
                size_t ui = pi->ui;

                #if DO_DEBUG_FACTTEST
                    SpewTwo( L"Compare fact to rule", wcp );
                #endif

                if( wcscmp( wcp, szFact ) == 0 )
                {
                    #if DO_DEBUG_FACTTEST
                        Spew( L"Fact matched main part of Rule." );
                    #endif

                    int RuleType = ui & 0xff;
                    int RuleSubType = ui >> 8;

                    // Next to test the sub-rule match:
                    int submatch = 0;
                    switch( RuleSubType )
                    {
                    case RULESUBTYPE_UNUSED:
                        // This case is expected to happen...
                        // just for anchortext ( BALD ) I think.
                        //
                        // Oh, I also get here for none-good-until/after.
                        // But the original fact could be bald or treble.
                        //
                        // So there is no submatch needed, grant it:
                        submatch = 1;
                        break;
                    case RULESUBTYPE_NEXT:
                        if( Whereness == WHERENESS_TREBLE
                        && ! SavingTitleOrAnchorText )
                            submatch = 1;
                        break;
                    case RULESUBTYPE_PRIOR:
                        if( Whereness == WHERENESS_TREBLE
                        && ! SavingTitleOrAnchorText )
                            submatch = 1;
                        break;
                    case RULESUBTYPE_ANCHOR:
                        if( Whereness == WHERENESS_TREBLE
                        && SavingTitleOrAnchorText )
                            submatch = 1;
                        break;
                    }
                    if( submatch )
                    {
                        #if DO_DEBUG_FACTTEST
                            Spew( L"Fact matched sub-part of Rule too." );
                        #endif

                        // Here is the moment to shout into scraping notes.

                        // Format lines similar to these rule examples, the
                        // criterion being my WideParseSearchEngineLine:
                        // good/bad url has prior/next/anchor token cleartext url
                        // good/bad url has domain x

                        if( m_AppendPageScrapingFacts )
                        {
                            m_pWsbQrpFacts->Add( L"Fact matched Rule: " );

                            switch( RuleType )
                            {
                            case RULETYPE_GOOD:
                                m_pWsbQrpFacts->Add( L"  good url has " );
                                break;
                            case RULETYPE_BAD:
                                m_pWsbQrpFacts->Add( L"  bad url has " );
                                break;
                            case RULETYPE_MORE:
                                m_pWsbQrpFacts->Add( L"  more url has " );
                                break;
                            case RULETYPE_NONE_UNTIL:
                                m_pWsbQrpFacts->Add( L"  none good until " );
                                break;
                            case RULETYPE_NONE_AFTER:
                                m_pWsbQrpFacts->Add( L"  none good after " );
                                break;
                            default:
                                m_pWsbQrpFacts->Add( L"--ERROR: INVALID RULETYPE--" );
                                break;
                            }

                            switch( RuleSubType )
                            {
                            case RULESUBTYPE_UNUSED:
                                // This case is expected to happen...
                                // for example, anchortext... domain... path...
                                break;
                            case RULESUBTYPE_NEXT:
                                m_pWsbQrpFacts->Add( L"next " );
                                break;
                            case RULESUBTYPE_PRIOR:
                                m_pWsbQrpFacts->Add( L"prior " );
                                break;
                            case RULESUBTYPE_ANCHOR:
                                m_pWsbQrpFacts->Add( L"anchor " );
                                break;
                            default:
                                m_pWsbQrpFacts->Add( L"--ERROR: INVALID RULESUBTYPE--" );
                                break;
                            }

                            // Show me the money.
                            m_pWsbQrpFacts->Add( wcp );

                            // Finish the RULE line.
                            m_pWsbQrpFacts->Add( L"\r\n" );
                        }

                        // Now for some more invention!
                        // I have the matched rule's type and subtype.
                        // Somehow, I must decide what URL to process.
                        // Next and Prior should take the B and A of A-B.
                        // anchor should take the one coming at TAG /A
                        // anchortext should take the one done at TAG /A
                        // PATH and DOMAIN should add a prefix, anchor.

                        ProcessMatchedRule( i, RuleType, RuleSubType );
                    }
                    else
                    {
                        #if DO_DEBUG_FACTTEST
                            Spew( L"Fact did not match sub-type of Rule." );
                        #endif
                    }
                }

                i++;
            }
        }
    }
}

void CHtm::ProcessMatchedRule( int RuleNo, int RuleType, int RuleSubType )
{
    // This instant of matching rule may not act immediately,
    // say to add a GOOD or MORE url to query process's list.
    // But generally, those will all take place for any link
    // at the beginning of next link ( tag a ) or end of parse.
    // Suppose in that time, I have both a GOOD and BAD rule.
    // The one with the lowest ordinal ( order in rules ) wins.

    // To allow a NEXT FACT to say BAD, and have priority over
    // some PRIOR FACT that said GOOD, I must process each url
    // "B", in the sequence A-B-C, at the start of doing "C".

    // At any moment, for some B, I must save a small sparse
    // vector of matched rule types/subtypes.... No, subtype
    // determines whether I am doing it for B, or any next C.

    // Instead of a vector, I could just hold one pending rule,
    // and discard it if a new match comes with a lower RuleNo.

    // This sliding window of design planning is confusing me.
    // I am only ever between two URLs A-B-C ( or A=sof, C=eof ).
    // Upon 'crossing' each URL, I will pipeline-backwards the
    // things saved in advance for such url, but first, I will
    // act on a set of pre/post-things saved for any prior url.
    // So now let's add routine calls into Tag A start and end.

    #if DO_DEBUG_FACTTEST
        Spew( L"ProcessMatchedRule called, passing:" );
        SpewValue( L"RuleNo", RuleNo );
        SpewValue( L"RuleType", RuleType );
        SpewValue( L"RuleSubType", RuleSubType );
    #endif

    switch( RuleType )
    {
    case RULETYPE_GOOD:
        if( m_NoneGoodUntilRuleIsInhibiting
        ||  m_NoneGoodAfterRuleIsInhibiting )
        {
            #if DO_DEBUG_FACTTEST
                Spew( L"ProcessMatchedRule: A RULETYPE_GOOD was inhibited." );
            #endif

            if( m_AppendPageScrapingFacts )
                m_pWsbQrpFacts->Add( L"An active none-good rule inhibited a good-URL rule.\r\n" );

            return; // done
        }
        break;
    case RULETYPE_BAD:
        break;
    case RULETYPE_MORE:
        break;
    case RULETYPE_NONE_UNTIL:
        #if DO_DEBUG_FACTTEST
            Spew( L"ProcessMatchedRule: Clearing NoneGoodUntilRule inhibitory bool." );
        #endif

        if( m_AppendPageScrapingFacts
        && m_NoneGoodUntilRuleIsInhibiting )
            m_pWsbQrpFacts->Add( L"Clearing an active none-good-until inhibitory rule.\r\n" );

        m_NoneGoodUntilRuleIsInhibiting = 0; // clear the inhibition

        return; // done
        break;
    case RULETYPE_NONE_AFTER:
        #if DO_DEBUG_FACTTEST
            Spew( L"ProcessMatchedRule: Setting NoneGoodAfterRule inhibitory bool." );
        #endif

        if( m_AppendPageScrapingFacts
        && m_NoneGoodAfterRuleIsInhibiting == 0 )
            m_pWsbQrpFacts->Add( L"Setting active a none-good-after inhibitory rule.\r\n" );

        m_NoneGoodAfterRuleIsInhibiting = 1; // set the inhibition
        return; // done
        break;
    default:
        return; // is an error
        break;
    }

    // So we are left with good/bad/more to process.

    switch( RuleSubType )
    {
    case RULESUBTYPE_NEXT:
        // This FACT-is-NEXT, so must process a PRIOR tag a url.
        // Arm HeadLinkIsCrossingTagABoundary regarding url 'A'.

        // All ( 1 ) cases for url 'A':
        if( Url_A_RuleNo == 0
        || Url_A_RuleNo > RuleNo )
        {
            // This new rule has RuleNo priority.
            // What is the rule? Copy the enums:
            Url_A_RuleType = RuleType; // good, bad, or more.
            Url_A_RuleNo = RuleNo; // and remember priority.
        }
        break;

    case RULESUBTYPE_UNUSED:
        // This case is expected to happen...
        // We come here ( for anchortext ) just BEFORE
        // the call to HeadLinkIsCrossingTagABoundary.
        // So this should work like a FACT-is-PRIOR.
        // Arm HeadLinkIsCrossingTagABoundary regarding url 'B'.
    case RULESUBTYPE_PRIOR:
        // This FACT-is-PRIOR, so must process a NEXT tag a url.
        // Arm HeadLinkIsCrossingTagABoundary regarding url 'B'.
    case RULESUBTYPE_ANCHOR:
        // Will now process current url between open/close tag a.
        // We come here ( for tags, tokens, path etc ) just BEFORE
        // the call to HeadLinkIsCrossingTagABoundary.
        // So this should work like a FACT-is-PRIOR.
        // Arm HeadLinkIsCrossingTagABoundary regarding url 'B'.

        // All ( 3 ) cases for url 'B':
        if( Url_B_RuleNo == 0
        || Url_B_RuleNo > RuleNo )
        {
            // This new rule has RuleNo priority.
            // What is the rule? Copy the enums:
            Url_B_RuleType = RuleType; // good, bad, or more.
            Url_B_RuleNo = RuleNo; // and remember priority.
        }
        break;

    default:
        return; // is an error
        break;
    }


}

void CHtm::HeadLinkIsCrossingTagABoundary( )
{
    // In a time sequence of links I call A-B-C, say we are at B now.
    // The URL called A is remembered in any UrlIndexOfAnyPriorTagA.
    // The URL called B is remembered in any LastTagA_UrlIndex;
    // I am the last thing done to end TAG_A in a query result page.
    // We finished all tokens, and anchortext, and any rule-matches.

    int indexA = UrlIndexOfAnyPriorTagA; // set to -1 at top of file
    int indexB = LastTagA_UrlIndex;      // set to -1 at end of file

    wchar_t * pMalUrlA = NULL;
    wchar_t * pMalUrlB = NULL;

    wchar_t * szUrlTextA = NULL;
    wchar_t * szUrlTextB = NULL;

    int AddToMore = 0;
    int AddToHits = 0;
    int AInError = 0;

    #if DO_DEBUG_FACTTEST
        Spew( L"HeadLinkIsCrossingTagABoundary through Url 'B'." );
        SpewValue( L"indexA", indexA );
        SpewValue( L"indexB", indexB );
    #endif

    switch( indexA )
    {
    case -1:
        szUrlTextA = L"Top-of-file marker";
        AInError = 1;
        break;
    case 0:
        szUrlTextA = L"--error: indexA is 0";
        AInError = 1;
        break;
    case 1:
        szUrlTextA = L"--error: indexA is 1";
        AInError = 1;
        break;
    default:
        szUrlTextA = pMalUrlA = ( wchar_t * ) CSolAllUrls.GetFullKey( indexA );
        break;
    }

    switch( indexB )
    {
    case -1:
        szUrlTextB = L"End-of-file marker";
        break;
    case 0:
        szUrlTextB = L"--error: indexB is 0";
        break;
    case 1:
        szUrlTextB = L"--error: indexB is 1";
        break;
    default:
        szUrlTextB = pMalUrlB = ( wchar_t * ) CSolAllUrls.GetFullKey( indexB );
        break;
    }

    #if DO_DEBUG_FACTTEST
        SpewTwo( L"szUrlTextA", szUrlTextA );
        SpewTwo( L"szUrlTextB", szUrlTextB );
    #endif

    // So ignore B for a moment. It is time to wrap up A.

    wchar_t * szRuleType = NULL;
    switch( Url_A_RuleType )
    {
        case 0:
            szRuleType = L"Url?Ignore: ";
            break;
        case RULETYPE_GOOD:
            szRuleType = L"Url?Good: ";
            AddToHits = 1;
            break;
        case RULETYPE_BAD:
            szRuleType = L"Url?Bad: ";
            break;
        case RULETYPE_MORE:
            szRuleType = L"Url?More: ";
            AddToMore = 1;
            break;
        default:
            szRuleType = L"--error: Url_A_RuleType invalid: ";
            break;
    }

    if( m_AppendPageScrapingFacts )
    {
        m_pWsbQrpFacts->Add( L"\r\n" );
        m_pWsbQrpFacts->Add( szRuleType );
        m_pWsbQrpFacts->Add( szUrlTextA );
        m_pWsbQrpFacts->Add( L"\r\n\r\n" );
    }

    #if DO_DEBUG_FACTTEST
        SpewTwo( szRuleType, szUrlTextA );
    #endif

    if( AddToMore
    && ! AInError
    && m_pSolMores != NULL
    && pMalUrlA != NULL )
    {
        int index = m_pSolMores->AddKey( pMalUrlA );
        #if DO_DEBUG_ADDFIND
            if( index <= 1 )
                { Spew( L"AddFind 1 at chtm 5998" ); }
        #endif
        #if DO_DEBUG_NEWQUERY
            Spew( L"Adding the following continuation URL:" );
            SpewValue( pMalUrlA, index );
        #endif
    }

    if( AddToHits
    && ! AInError
    && m_pSolHits != NULL
    && indexA > 1 )
    {
        // For Hits, instead of duplicating the lengthy URL string,
        // format a short itoa of the hit URL index in CSolAllUrls.

        wchar_t wk[20];
        // wchar_t * _itow( int value, wchar_t *string, int radix );
        _itow( indexA, wk, 10 );
        int index = m_pSolHits->AddKey( wk );
        #if DO_DEBUG_ADDFIND
            if( index <= 1 )
                { Spew( L"AddFind 1 at chtm 6022" ); }
        #endif
        #if DO_DEBUG_NEWQUERY
            Spew( L"Adding the following Hit URL:" );
            SpewValue( pMalUrlA, index );
        #endif
    }

    // Now pipeline backwards: B into A; Clear B.

    // Whatever Facts were predicted as coming "prior" to any url 'B'
    // have already been recorded in the B info. After crossing 'B',
    // any Facts predicted as coming "next" to url affect url in 'A'.

    Url_A_RuleType = Url_B_RuleType;
    Url_A_RuleNo = Url_B_RuleNo;

    Url_B_RuleType = 0; // Meaning none is recorded yet.
    Url_B_RuleNo = 0; // Any any new rule replaces zero.

    // Done. Free any mallocs.

    if( pMalUrlA != NULL )
        MyFree( 5844, zx, pMalUrlA );
    if( pMalUrlB != NULL )
        MyFree( 5846, zx, pMalUrlB );
}
