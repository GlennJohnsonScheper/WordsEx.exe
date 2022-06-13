// This is file: CHtm.h
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

// The HTML parser has five states ( LAB=Left Angle Bracket; AMP=Ampersand )

enum {
    LAYER1STATE_NOTHING,
    LAYER1STATE_AMP,
    LAYER1STATE_LAB,
    LAYER1STATE_COMMENT,
    LAYER1STATE_GOODTAG,
    LAYER1STATE_XML_TAG
};


class CHtm
{
public:
    // SED_PILE1
    CHtm( size_t UrlIndex, CWsb * pWsbPaperText, CIdx * pIdxPaperIndex, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int PageIsAQrp, COneSurl * pOneSurl );
    // SED_PILE2
    ~CHtm( );

    CWsb * m_pWsbPaperText;
    CIdx * m_pIdxPaperIndex;
    CSol * m_pSolFrames;
    CSol * m_pSolMores;
    CSol * m_pSolHits;
    CWsb * m_pWsbTitleText;
    COneSurl * m_pOneSurl;
    int m_NoneGoodUntilRuleIsInhibiting;
    int m_NoneGoodAfterRuleIsInhibiting;

    void ParseWideUCSInputBuffer( wchar_t * pBuffer, size_t nBuffer );
    void FlushParse( );

    int BitMaskReasonsToHonorNewlines;
    int BitMaskReasonsToHonorIndent;

    int m_PassedUrlIndex;
    int m_BaseUrlIndex;
    int m_BestUrlIndex;
    wchar_t * m_pMalBase;

    size_t m_nCSetFound;
    size_t m_nLangFound;

private:
    void InitParse( );
    void ParseOneInputUCSChar( wchar_t inputChar );
    int UnicodeToIsoLatinFilter( int CodePoint );
    void ParseOnePostUtf8ConversionCharacter( int inputChar );
    void RedoCharHtmlParseLayer1( wchar_t inputChar );
    void BodyCharHtmlParseLayer1( wchar_t inputChar );
    void RedoTextHeldInLayer1LabSuspense( );
    void RedoTextHeldInLayer1AmpSuspense( );
    int ValidTagNameInAccumulatedLabSuspense( );
    int ValidEntityInAccumulatedAmpSuspense( );
    void ProcessWholeMarkupTag( );
    void ProcessTagAttributes( );
    void ProcessOneAttribute( wchar_t * szName, wchar_t * szValue );
    void ProcessAlternateText( wchar_t * szValue );
    void InitializeTitleOrAnchorTextBuffer( );
    void FinalizeTitleOrAnchorTextBuffer( );

    void ProcessTextTokenBuffer( int nNewlines );
    void AddToPlainText( wchar_t * szFragment, int FragmentLength, int nNewlines );
    void ChangeTextIndentation( );
    void RestoreTextIndentation( );
    void FormAddNameValuePair( wchar_t * szName, wchar_t * szValue );
    void AddOrTestQueryFact( int Whereness, wchar_t * szFact );
    void AddOrTestQueryUrlFacts( size_t UrlIndex );
    void UseTokenTextInPageScrapingFacts( );
    void ProcessMatchedRule( int RuleNo, int RuleType, int RuleSubType );
    void HeadLinkIsCrossingTagABoundary( );

    wchar_t * TextHeldInLayer1AmpSuspense;
    wchar_t * TextHeldInLayer1LabSuspense;
    wchar_t * TextHeldInLayer2TokenText;
    wchar_t * TextHeldInLayer1Markup;
    wchar_t * szTitleOrAnchorBuffer;

    int InputUtf8State;
    int InputUtf8Byte1;
    int InputUtf8Byte2;
    int InputUtf8Byte3;
    int PriorPostUtf8Character;
    int InputNewlineCount;
    int ConsecutiveInputNewlines;

    int Layer1State;

    int IndexHeldInLayer1AmpSuspense;
    int Layer1AmpPushedState;

    int IndexHeldInLayer1LabSuspense;
    int ColonCountInLabSuspense;
    int LastHyphenCountInComment;

    int IndexHeldInLayer1Markup;
    int TagEnumHeldInLayer1Markup;
    int IndexPastTagInLayer1Markup;

    int IndexHeldInLayer2TokenText;

    int InsertSpaceIfSayAlphaComes;
    int PendingInsertSpaceChar;

    int BetweenHtmlScriptTags;
    int BetweenHtmlTitleTags;

    int ArmToSaveStartAnchorOffset;
    int StartAnchorOffsetInText;

    int TagA_Had_Name;
    int TagA_Had_Href;
    int LastTagA_UrlIndex;
    int SavedAtEntryToTagA;
    int UrlIndexOfAnyPriorTagA;

    int InsideTagSpanWithLangAttr;
    int InsideTagAWithLangAttr;

    int BitMaskReasonsToDiscardText;
    int CurrentBlockOffsetInText;
    int PreferredWrapColumnInOutputText;

    int CurrentBlockIsEntered;

    int ConsecutiveNewlinesInOutputText;
    int CurrentColumnInOutputText;
    int IndentationColumnInOutputText;
    int IndentationColumnBeforeDlTag;

    int SavingTitleOrAnchorText;

    int EndingTagAOffsetInText;
    int LastTagAIDXHighWater;

    size_t TitleOrAnchorBufferStringLength;

    int Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentType;
    int Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_ContentLanguage;
    int Enable_ATTR_CONTENT_ToDo_ATTR_HTTP_EQUIV_Refresh;

    int m_DuringTagForm;
    int m_BetweenOpenCloseTagForm;
    int m_FormRejectedBitfield;
    size_t m_FormActionPresent;
    size_t m_FormMethodUrlIndex;
    size_t m_FormGet1Post2;
    int m_DuringTagSelect;
    int m_BetweenOpenCloseTagSelect;
    int m_DuringTagOption;
    int m_FormOptionSelected;
    int m_FormCheckBoxChecked;
    int m_FormRadioChecked;
    int m_DuringTagInput;
    size_t m_FormInputType;
    int m_FormInputTypeTextCount;
    int m_FormInputTypeSubmitCount;
    wchar_t * m_FormInputTypeTextName;
    wchar_t * m_FormInputName;
    wchar_t * m_FormInputValue;
    wchar_t * m_FormSelectName;
    wchar_t * m_FormOptionValue;

    CWsb * m_pWsbFormParts;
    size_t m_FormParamCount;
    int m_PageIsAQrp;
    int m_AppendPageScrapingFacts;
    CWsb * m_pWsbQrpFacts;

    // In the beginning, I had used pointers and mallocs instead
    // of arrays, because I thought each page would contain CHTM
    // members, but now CHtm only exists long enough to do parse.
    wchar_t PriorTokenUnder50[51]; // room for just 50 chars.
    int PriorTokenIsNumberEnum;

    int Url_A_RuleType;
    int Url_A_RuleNo;
    int Url_B_RuleType;
    int Url_B_RuleNo;

};



