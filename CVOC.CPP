// This is file: CVoc.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// Note the order CAll.cpp does inits, and that only Salt is done in a thread:
// InitHtmlEntities( );
// InitHtmlTagNames( );
// InitHtmlAttrNames( );
// SetupCharsetNames( );
// SetupLanguageNames( );
// SetupDomainNames( );
// SetupBinaryNames( );
// Url.SetUpDefaultSearchEngineControls( );
// Initiator.InitiateSalts( ); // thread does SaltCommonAndPerLanguageWordLists( );

//   You will need of these lines to clone the final list of all sub-lists:
//   having the proper language-group number associated with each mnemonic.
//   over in clan.cpp.
//
//   { un001, 0 }, -- 0 is special, unknown, used for proper names.
//   { af001, 1 },
//   { am001, 2 },
//   { ar001, 3 },
//   { arn001, 4 },
//   { as001, 5 },
//   { az001, 6 },
//   { ba001, 7 },
//   { be001, 8 },
//   { bg001, 9 },
//   { bn001, 10 },
//   { bo001, 11 },
//   { br001, 12 },
//   { bs001, 13 },
//   { ca001, 14 },
//   { co001, 15 },
//   { cs001, 16 },
//   { cy001, 17 },
//   { da001, 18 },
//   { de001, 19 },
//   { div001, 20 },
//   { el001, 21 },
//   { en001, 22 },
//   { es001, 23 },
//   { et001, 24 },
//   { eu001, 25 },
//   { fa001, 26 },
//   { fi001, 27 },
//   { fil001, 28 },
//   { fo001, 29 },
//   { fr001, 30 },
//   { fy001, 31 },
//   { ga001, 32 },
//   { gl001, 33 },
//   { gsw001, 34 },
//   { gu001, 35 },
//   { ha001, 36 },
//   { he001, 37 },
//   { hi001, 38 },
//   { hr001, 39 },
//   { hu001, 40 },
//   { hy001, 41 },
//   { id001, 42 },
//   { ig001, 43 },
//   { ii001, 44 },
//   { is001, 45 },
//   { it001, 46 },
//   { iu001, 47 },
//   { ja001, 48 },
//   { ka001, 49 },
//   { kk001, 50 },
//   { kl001, 51 },
//   { km001, 52 },
//   { kn001, 53 },
//   { ko001, 54 },
//   { kok001, 55 },
//   { ky001, 56 },
//   { lb001, 57 },
//   { lo001, 58 },
//   { lt001, 59 },
//   { lv001, 60 },
//   { mi001, 61 },
//   { mk001, 62 },
//   { ml001, 63 },
//   { mn001, 64 },
//   { moh001, 65 },
//   { mr001, 66 },
//   { ms001, 67 },
//   { mt001, 68 },
//   { nb001, 69 },
//   { ne001, 70 },
//   { nl001, 71 },
//   { nn001, 72 },
//   { no001, 73 },
//   { nso001, 74 },
//   { oc001, 75 },
//   { or001, 76 },
//   { pa001, 77 },
//   { pl001, 78 },
//   { prs001, 79 },
//   { ps001, 80 },
//   { pt001, 81 },
//   { qut001, 82 },
//   { quz001, 83 },
//   { rm001, 84 },
//   { ro001, 85 },
//   { ru001, 86 },
//   { rw001, 87 },
//   { sa001, 88 },
//   { sah001, 89 },
//   { se001, 90 },
//   { si001, 91 },
//   { sk001, 92 },
//   { sl001, 93 },
//   { sma001, 94 },
//   { smj001, 95 },
//   { smn001, 96 },
//   { sms001, 97 },
//   { sq001, 98 },
//   { sr001, 99 },
//   { sv001, 100 },
//   { sw001, 101 },
//   { syr001, 102 },
//   { ta001, 103 },
//   { te001, 104 },
//   { tg001, 105 },
//   { th001, 106 },
//   { tk001, 107 },
//   { tmz001, 108 },
//   { tn001, 109 },
//   { tr001, 110 },
//   { tt001, 111 },
//   { ug001, 112 },
//   { uk001, 113 },
//   { ur001, 114 },
//   { uz001, 115 },
//   { vi001, 116 },
//   { wee001, 117 },
//   { wen001, 118 },
//   { wo001, 119 },
//   { xh001, 120 },
//   { yo001, 121 },
//   { zh001, 122 },
//   { zu001, 123 },


// Summary on Nov 15 2007, as I prepare to add language guessing:
//
// WordsEx currently contains several kinds of word lists:
//
// - All words in all lists are UCS, lowercase, and may have non-alphas.
//
// - All words in all lists have the same convention for the User.Value,
// which is: 8 msb 'letter score' and 24 lsb count from salt and/or pages.
//
// Nov 27 2007 - 0 to 123 group IDs fit in 7 bits. So I will change
// interpretation of DWORD value ( in some or all lists ) as follows:
// - 8 msb = word value, range 0 to 255,
// - next 7 msb: 1-123 = language of this word; 0 = unk, 127 = multiple.
// - 17 lsb = word count, clamped to 128K, exact range 0 to 0x0001ffff.
//
// CTxt's m_pSolWordList = pOnePaper->pSolWordList for one web page.
// - Every COnePaper has this member, init during COnePaper constructor.
// - Every word in any page's pSolWordList gets added into CSolAllWords.
// - The User.Value in this CSol is raw, unclamped, unmodified ++ count.
// - CFwd Word Search looks up its tokens before doing full text search.
// - CPag annotates 'terms' from pOnePaper->pSolWordList's nList - 2.
//
// CSolAllWords
// - CSolAllWords does NOT get salted at start-up. Counts only web pages.
// - Call's AnyFinalDebugDumps dumps it. Manual work turns it into 1 LGroup.
// - CFwd's TokenStemmingAnalysis searches of it to find match-stems.
// - CFwd's BreakDownTheFindParameterString searches it to predict matches.
// - CFwd's PostTokenScanAnalysis searches it to predict stemmed matches.
// - CFwd's RankItem looks up one word count ( +other ) to value as 1/( counts ).
// - CTxt's ParseBlock sums each word's "value": ( as approx 8msb / 24lsb ).
// --- it seems like these should both use the same valuation metric.
// - w*.cpp status lists 'vocabulary' from CSolAllWordList's nList - 2.
// - The User.Value in this CSol has all three bit-fields as described.
//
// CSolCommonWords
// - CSolCommonWords gets salted at start-up.
// - CSolCommonWords does NOT currently get new words added from web pages.
// CFwd's RankItem looks up one word count ( +other ) to value as 1/( counts ).
// CTxt's MostUncommonWords uses this ( count/128+1 ) to divide page word count.
// - The User.Value in this CSol has all three bit-fields as described.
//
// CSolLGroupWords
// - Their purpose is for language guessing - so do not add from unknown langs.
// - Another purpose is build new language lists for inclusion into WordsEx.
// - However, I do not see their dumper. I manually dump from CSolAllWords!?!
// - Some of CSolLGroupWords [ N ] get salted at start-up.
// - Some of CSolLGroupWords [ N ] get created when language is first seen.
// CTxt's AddWordsToOtherLists does new pSolOneLG creation, JIT as reqd.
// CTxt's AddWordsToOtherLists does add all web page words to pSolOneLG.
// - The User.Value in this CSol has all three bit-fields as described.
//
// Call's routine WordLetterValuation is called to develop 8 msb for:
// - From CVoc's Salter for CSolCommonWords and some CSolLGroupWords [ N ].
// - Whenever CTxt's AddWordsToOtherLists finds a novel word in a web page,
// ( not already in CSolAllWords ) then AddWordsToOtherLists develops 8 msb
// for both the pOnePaper->pSolWordList, and to add into CSolAllWords.


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

const int SomeLanguageIDForGroupID[] = {
      0, //   0=ok (  0-eq ) group 'un' Unknown -> lang 'un-known' Unknown
      2, //   1=ok (  2-eq ) group 'af' Afrikaans -> lang 'af' Afrikaans
      7, //   2=ok (  7-eq ) group 'am' Amharic -> lang 'am-ET' Amharic ( Ethiopia )
      8, //   3=ok (  8-eq ) group 'ar' Arabic -> lang 'ar' Arabic
    166, //   4=ok ( 166-eq ) group 'arn' Mapudungun -> lang 'arn-CL' Mapudungun ( Chile )
     27, //   5=ok ( 27-eq ) group 'as' Assamese -> lang 'as-IN' Assamese ( India )
     28, //   6=ok ( 28-eq ) group 'az' Azeri -> lang 'az' Azeri
     31, //   7=ok ( 31-eq ) group 'ba' Bashkir -> lang 'ba-RU' Bashkir ( Russia )
     34, //   8=ok ( 34-eq ) group 'be' Belarusian -> lang 'be' Belarusian
     41, //   9=ok ( 41-eq ) group 'bg' Bulgarian -> lang 'bg' Bulgarian
     36, //  10=ok ( 36-eq ) group 'bn' Bengali -> lang 'bn-BD' Bengali ( Bangladesh )
    256, //  11=ok ( 256-eq ) group 'bo' Tibetan -> lang 'bo-CN' Tibetan ( PRC )
     40, //  12=ok ( 40-eq ) group 'br' Breton -> lang 'br-FR' Breton ( France )
     38, //  13=ok ( 38-eq ) group 'bs' Bosnian -> lang 'bs-Cyrl-BA' Bosnian ( Cyrillic, Bosnia and Herzegovina )
     43, //  14=ok ( 43-eq ) group 'ca' Catalan -> lang 'ca' Catalan
     52, //  15=ok ( 52-eq ) group 'co' Corsican -> lang 'co-FR' Corsican ( France )
     56, //  16=ok ( 56-eq ) group 'cs' Czech -> lang 'cs' Czech
    271, //  17=ok ( 271-eq ) group 'cy' Welsh -> lang 'cy-GB' Welsh ( United Kingdom )
     58, //  18=ok ( 58-eq ) group 'da' Danish -> lang 'da' Danish
    103, //  19=ok ( 103-eq ) group 'de' German -> lang 'de' German
     61, //  20=ok ( 61-eq ) group 'div' Divehi -> lang 'div' Divehi
    109, //  21=ok ( 109-eq ) group 'el' Greek -> lang 'el' Greek
     66, //  22=ok ( 66-eq ) group 'en' English -> lang 'en' English
    220, //  23=ok ( 220-eq ) group 'es' Spanish -> lang 'es' Spanish
     84, //  24=ok ( 84-eq ) group 'et' Estonian -> lang 'et' Estonian
     32, //  25=ok ( 32-eq ) group 'eu' Basque -> lang 'eu' Basque
    181, //  26=ok ( 181-eq ) group 'fa' Persian -> lang 'fa' Persian
     89, //  27=ok ( 89-eq ) group 'fi' Finnish -> lang 'fi' Finnish
     88, //  28=ok ( 88-eq ) group 'fil' Filipino -> lang 'fil-PH' Filipino ( Philippines )
     86, //  29=ok ( 86-eq ) group 'fo' Faroese -> lang 'fo' Faroese
     91, //  30=ok ( 91-eq ) group 'fr' French -> lang 'fr' French
     98, //  31=ok ( 98-eq ) group 'fy' Frisian -> lang 'fy-NL' Frisian ( Netherlands )
    128, //  32=ok ( 128-eq ) group 'ga' Irish -> lang 'ga-IE' Irish ( Ireland )
     99, //  33=ok ( 99-eq ) group 'gl' Galician -> lang 'gl' Galician
      6, //  34=ok (  6-eq ) group 'gsw' Alsatian -> lang 'gsw-FR' Alsatian ( France )
    112, //  35=ok ( 112-eq ) group 'gu' Gujarati -> lang 'gu' Gujarati
    114, //  36=ok ( 114-eq ) group 'ha' Hausa -> lang 'ha-Latn-NG' Hausa ( Latin, Nigeria )
    115, //  37=ok ( 115-eq ) group 'he' Hebrew -> lang 'he' Hebrew
    117, //  38=ok ( 117-eq ) group 'hi' Hindi -> lang 'hi' Hindi
     53, //  39=ok ( 53-eq ) group 'hr' Croatian -> lang 'hr' Croatian
    119, //  40=ok ( 119-eq ) group 'hu' Hungarian -> lang 'hu' Hungarian
     25, //  41=ok ( 25-eq ) group 'hy' Armenian -> lang 'hy' Armenian
    124, //  42=ok ( 124-eq ) group 'id' Indonesian -> lang 'id' Indonesian
    123, //  43=ok ( 123-eq ) group 'ig' Igbo -> lang 'ig-NG' Igbo ( Nigeria )
    274, //  44=ok ( 274-eq ) group 'ii' Yi -> lang 'ii-CN' Yi ( PRC )
    121, //  45=ok ( 121-eq ) group 'is' Icelandic -> lang 'is' Icelandic
    131, //  46=ok ( 131-eq ) group 'it' Italian -> lang 'it' Italian
    126, //  47=ok ( 126-eq ) group 'iu' Inuktitut -> lang 'iu-Latn-CA' Inuktitut ( Latin, Canada )
    134, //  48=ok ( 134-eq ) group 'ja' Japanese -> lang 'ja' Japanese
    101, //  49=ok ( 101-eq ) group 'ka' Georgian -> lang 'ka' Georgian
    138, //  50=ok ( 138-eq ) group 'kk' Kazakh -> lang 'kk' Kazakh
    111, //  51=ok ( 111-eq ) group 'kl' Greenlandic -> lang 'kl-GL' Greenlandic ( Greenland )
    140, //  52=ok ( 140-eq ) group 'km' Khmer -> lang 'km-KH' Khmer ( Cambodia )
    136, //  53=ok ( 136-eq ) group 'kn' Kannada -> lang 'kn' Kannada
    147, //  54=ok ( 147-eq ) group 'ko' Korean -> lang 'ko' Korean
    145, //  55=ok ( 145-eq ) group 'kok' Konkani -> lang 'kok' Konkani
    149, //  56=ok ( 149-eq ) group 'ky' Kyrgyz -> lang 'ky' Kyrgyz
    157, //  57=ok ( 157-eq ) group 'lb' Luxembourgish -> lang 'lb-LU' Luxembourgish ( Luxembourg )
    151, //  58=ok ( 151-eq ) group 'lo' Lao -> lang 'lo-LA' Lao ( Lao P.D.R. )
    154, //  59=ok ( 154-eq ) group 'lt' Lithuanian -> lang 'lt' Lithuanian
    152, //  60=ok ( 152-eq ) group 'lv' Latvian -> lang 'lv' Latvian
    165, //  61=ok ( 165-eq ) group 'mi' Maori -> lang 'mi-NZ' Maori ( New Zealand )
    158, //  62=ok ( 158-eq ) group 'mk' Macedonian -> lang 'mk' Macedonian
    163, //  63=ok ( 163-eq ) group 'ml' Malayalam -> lang 'ml-IN' Malayalam ( India )
    170, //  64=ok ( 170-eq ) group 'mn' Mongolian -> lang 'mn' Mongolian
    169, //  65=ok ( 169-eq ) group 'moh' Mohawk -> lang 'moh-CA' Mohawk ( Mohawk )
    167, //  66=ok ( 167-eq ) group 'mr' Marathi -> lang 'mr' Marathi
    160, //  67=ok ( 160-eq ) group 'ms' Malay -> lang 'ms' Malay
    164, //  68=ok ( 164-eq ) group 'mt' Maltese -> lang 'mt-MT' Maltese ( Malta )
    175, //  69=ok ( 175-eq ) group 'nb' Norwegian, Bokml -> lang 'nb-NO' Norwegian, Bokml ( Norway )
    173, //  70=ok ( 173-eq ) group 'ne' Nepali -> lang 'ne-NP' Nepali ( Nepal )
     63, //  71=ok ( 63-eq ) group 'nl' Dutch -> lang 'nl' Dutch
    176, //  72=ok ( 176-eq ) group 'nn' Norwegian, Nynorsk -> lang 'nn-NO' Norwegian, Nynorsk ( Norway )
    174, //  73=ok ( 174-eq ) group 'no' Norwegian -> lang 'no' Norwegian
    213, //  74=ok ( 213-eq ) group 'nso' Sesotho sa Leboa -> lang 'nso-ZA' Sesotho sa Leboa ( South Africa )
    177, //  75=ok ( 177-eq ) group 'oc' Occitan -> lang 'oc-FR' Occitan ( France )
    178, //  76=ok ( 178-eq ) group 'or' Oriya -> lang 'or-IN' Oriya ( India )
    187, //  77=ok ( 187-eq ) group 'pa' Punjabi -> lang 'pa' Punjabi
    182, //  78=ok ( 182-eq ) group 'pl' Polish -> lang 'pl' Polish
     60, //  79=ok ( 60-eq ) group 'prs' Dari -> lang 'prs-AF' Dari ( Afghanistan )
    179, //  80=ok ( 179-eq ) group 'ps' Pashto -> lang 'ps-AF' Pashto ( Afghanistan )
    184, //  81=ok ( 184-eq ) group 'pt' Portuguese -> lang 'pt' Portuguese
    141, //  82=ok ( 141-eq ) group 'qut' K'iche -> lang 'qut-GT' K'iche ( Guatemala )
    189, //  83=ok ( 189-eq ) group 'quz' Quechua -> lang 'quz-BO' Quechua ( Bolivia )
    194, //  84=ok ( 194-eq ) group 'rm' Romansh -> lang 'rm-CH' Romansh ( Switzerland )
    192, //  85=ok ( 192-eq ) group 'ro' Romanian -> lang 'ro' Romanian
    195, //  86=ok ( 195-eq ) group 'ru' Russian -> lang 'ru' Russian
    142, //  87=ok ( 142-eq ) group 'rw' Kinyarwanda -> lang 'rw-RW' Kinyarwanda ( Rwanda )
    206, //  88=ok ( 206-eq ) group 'sa' Sanskrit -> lang 'sa' Sanskrit
    273, //  89=ok ( 273-eq ) group 'sah' Yakut -> lang 'sah-RU' Yakut ( Russia )
    200, //  90=ok ( 200-eq ) group 'se' Sami, Northern -> lang 'se-FI' Sami, Northern ( Finland )
    215, //  91=ok ( 215-eq ) group 'si' Sinhala -> lang 'si-LK' Sinhala ( Sri Lanka )
    216, //  92=ok ( 216-eq ) group 'sk' Slovak -> lang 'sk' Slovak
    218, //  93=ok ( 218-eq ) group 'sl' Slovenian -> lang 'sl' Slovenian
    204, //  94=ok ( 204-eq ) group 'sma' Sami, Southern -> lang 'sma-NO' Sami, Southern ( Norway )
    198, //  95=ok ( 198-eq ) group 'smj' Sami, Lule -> lang 'smj-NO' Sami, Lule ( Norway )
    197, //  96=ok ( 197-eq ) group 'smn' Sami, Inari -> lang 'smn-FI' Sami, Inari ( Finland )
    203, //  97=ok ( 203-eq ) group 'sms' Sami, Skolt -> lang 'sms-FI' Sami, Skolt ( Finland )
      4, //  98=ok (  4-eq ) group 'sq' Albanian -> lang 'sq' Albanian
    208, //  99=ok ( 208-eq ) group 'sr' Serbian -> lang 'sr' Serbian
    241, // 100=ok ( 241-eq ) group 'sv' Swedish -> lang 'sv' Swedish
    143, // 101=ok ( 143-eq ) group 'sw' Kiswahili -> lang 'sw' Kiswahili
    244, // 102=ok ( 244-eq ) group 'syr' Syriac -> lang 'syr' Syriac
    248, // 103=ok ( 248-eq ) group 'ta' Tamil -> lang 'ta' Tamil
    252, // 104=ok ( 252-eq ) group 'te' Telugu -> lang 'te' Telugu
    246, // 105=ok ( 246-eq ) group 'tg' Tajik -> lang 'tg-Cyrl-TJ' Tajik ( Cyrillic, Tajikistan )
    254, // 106=ok ( 254-eq ) group 'th' Thai -> lang 'th' Thai
    259, // 107=ok ( 259-eq ) group 'tk' Turkmen -> lang 'tk-TM' Turkmen ( Turkmenistan )
    247, // 108=ok ( 247-eq ) group 'tmz' Tamazight -> lang 'tmz-Latn-DZ' Tamazight ( Latin, Algeria )
    214, // 109=ok ( 214-eq ) group 'tn' Setswana -> lang 'tn-ZA' Setswana ( South Africa )
    257, // 110=ok ( 257-eq ) group 'tr' Turkish -> lang 'tr' Turkish
    250, // 111=ok ( 250-eq ) group 'tt' Tatar -> lang 'tt' Tatar
    260, // 112=ok ( 260-eq ) group 'ug' Uighur -> lang 'ug-CN' Uighur ( PRC )
    261, // 113=ok ( 261-eq ) group 'uk' Ukrainian -> lang 'uk' Ukrainian
    264, // 114=ok ( 264-eq ) group 'ur' Urdu -> lang 'ur' Urdu
    266, // 115=ok ( 266-eq ) group 'uz' Uzbek -> lang 'uz' Uzbek
    269, // 116=ok ( 269-eq ) group 'vi' Vietnamese -> lang 'vi' Vietnamese
    156, // 117=ok ( 156-eq ) group 'wee' Lower Sorbian -> lang 'wee-DE' Lower Sorbian ( Germany )
    263, // 118=ok ( 263-eq ) group 'wen' Upper Sorbian -> lang 'wen-DE' Upper Sorbian ( Germany )
    272, // 119=ok ( 272-eq ) group 'wo' Wolof -> lang 'wo-SN' Wolof ( Senegal )
    129, // 120=ok ( 129-eq ) group 'xh' isiXhosa -> lang 'xh-ZA' isiXhosa ( South Africa )
    275, // 121=ok ( 275-eq ) group 'yo' Yoruba -> lang 'yo-NG' Yoruba ( Nigeria )
     45, // 122=ok ( 45-eq ) group 'zh' Chinese -> lang 'zh-HK' Chinese ( Hong Kong S.A.R. )
    130, // 123=ok ( 130-eq ) group 'zu' isiZulu -> lang 'zu-ZA' isiZulu ( South Africa )
};

// The vocabulary of HTML tags was developed heuristically,
// by trawling the cache / crawling the trash? a few times.

// I moved this structure definition from cvoc.cpp into Call.h:
// typedef struct tagname_tag {
//     wchar_t enumTag;
//     wchar_t * wcp;
// } TAGNAME;

// In order to allow CHtm.cpp to access these tag strings for sprintf,
// They must match the enum order, including to have the zero-th entry.
TAGNAME TagNames[] = {
TAG_ZERO,               L"wordsex-error-tagindex-zero", // program error!
TAG_A,                  L"a",
TAG_ABBR,               L"abbr",
TAG_ACRONYM,            L"acronym",
TAG_ADDRESS,            L"address",
TAG_APPLET,             L"applet",
TAG_AREA,               L"area",
TAG_B,                  L"b",
TAG_BASE,               L"base",
TAG_BASEFONT,           L"basefont",
TAG_BDO,                L"bdo",
TAG_BGSOUND,            L"bgsound",
TAG_BIG,                L"big",
TAG_BLINK,              L"blink",
TAG_BLOCKQUOTE,         L"blockquote",
TAG_BODY,               L"body",
TAG_BR,                 L"br",
TAG_BUTTON,             L"button",
TAG_CAPTION,            L"caption",
TAG_CENTER,             L"center",
TAG_CITE,               L"cite",
TAG_CODE,               L"code",
TAG_COL,                L"col",
TAG_COLGROUP,           L"colgroup",
TAG_COMMENT,            L"comment",
TAG_DD,                 L"dd",
TAG_DEL,                L"del",
TAG_DFN,                L"dfn",
TAG_DIR,                L"dir",
TAG_DIV,                L"div",
TAG_DL,                 L"dl",
TAG_DOCTYPE,            L"!doctype",
TAG_DT,                 L"dt",
TAG_EENDIF,              L"!endif",
TAG_EIF,                L"!if",
TAG_EM,                 L"em",
TAG_EMBED,              L"embed",
TAG_EMPH,               L"emph",
TAG_FIELDSET,           L"fieldset",
TAG_FONT,               L"font",
TAG_FORM,               L"form",
TAG_FRAME,              L"frame",
TAG_FRAMESET,           L"frameset",
TAG_H1,                 L"h1",
TAG_H2,                 L"h2",
TAG_H3,                 L"h3",
TAG_H4,                 L"h4",
TAG_H5,                 L"h5",
TAG_H6,                 L"h6",
TAG_HEAD,               L"head",
TAG_HR,                 L"hr",
TAG_HTML,               L"html",
TAG_I,                  L"i",
TAG_IFRAME,             L"iframe",
TAG_ILAYER,             L"ilayer",
TAG_IMAGE,              L"image",
TAG_IMG,                L"img",
TAG_INPUT,              L"input",
TAG_INS,                L"ins",
TAG_ISINDEX,            L"isindex",
TAG_KBD,                L"kbd",
TAG_LABEL,              L"label",
TAG_LAYER,              L"layer",
TAG_LEGEND,             L"legend",
TAG_LI,                 L"li",
TAG_LINK,               L"link",
TAG_MAP,                L"map",
TAG_MARQUEE,            L"marquee",
TAG_MENU,               L"menu",
TAG_META,               L"meta",
TAG_MULTICOL,           L"multicol",
TAG_NOBR,               L"nobr",
TAG_NODE,               L"node",
TAG_NOEMBED,            L"noembed",
TAG_NOFRAMES,           L"noframes",
TAG_NOINDEX,            L"noindex",
TAG_NOLAYER,            L"nolayer",
TAG_NOSCRIPT,           L"noscript",
TAG_OBJECT,             L"object",
TAG_OL,                 L"ol",
TAG_OPTGROUP,           L"optgroup",
TAG_OPTION,             L"option",
TAG_P,                  L"p",
TAG_PARAGRAPH,          L"paragraph",
TAG_PARAM,              L"param",
TAG_PRE,                L"pre",
TAG_Q,                  L"q",
TAG_QXML,               L"?xml",
TAG_S,                  L"s",
TAG_SAMP,               L"samp",
TAG_SCRIPT,             L"script",
TAG_SELECT,             L"select",
TAG_SERVER,             L"server",
TAG_SMALL,              L"small",
TAG_SPACER,             L"spacer",
TAG_SPAN,               L"span",
TAG_STRIKE,             L"strike",
TAG_STRONG,             L"strong",
TAG_STYLE,              L"style",
TAG_SUB,                L"sub",
TAG_SUP,                L"sup",
TAG_TABLE,              L"table",
TAG_TBODY,              L"tbody",
TAG_TD,                 L"td",
TAG_TEXTAREA,           L"textarea",
TAG_TFOOT,              L"tfoot",
TAG_TH,                 L"th",
TAG_THEAD,              L"thead",
TAG_TITLE,              L"title",
TAG_TR,                 L"tr",
TAG_TT,                 L"tt",
TAG_U,                  L"u",
TAG_UL,                 L"ul",
TAG_VAR,                L"var",
TAG_VERBATIM,           L"verbatim",
TAG_WBR,                L"wbr",
TAG_WEB,                L"web",
TAG_XML,                L"xml",
TAG_PAST_SENTINEL,      L"wordsex-error-tagindex-past", // program error!
NULL, NULL
};


// I moved this structure definition from cvoc.cpp into Call.h:
// typedef struct attrname_tag {
//     wchar_t enumAttr;
//     wchar_t * wcp;
// } ATTRNAME;

// In order to allow CHtm.cpp to access these attr strings for sprintf,
// They must match the enum order, including to have the zero-th entry.
ATTRNAME AttrNames[] = {
ATTR_ZERO,              L"wordsex-error-attrindex-zero", // program error!
ATTR_ACCESSKEY,         L"accesskey",
ATTR_ACTION,            L"action",
ATTR_ALIGN,             L"align",
ATTR_ALINK,             L"alink",
ATTR_ALLOWTRANSPARENCY, L"allowtransparency",
ATTR_ALT,               L"alt",
ATTR_AUTOCOMPLETE,      L"autocomplete",
ATTR_BACKGROUND,        L"background",
ATTR_BGCOLOR,           L"bgcolor",
ATTR_BORDER,            L"border",
ATTR_BORDERCOLOR,       L"bordercolor",
ATTR_BORDERCOLORDARK,   L"bordercolordark",
ATTR_BORDERCOLORLIGHT,  L"bordercolorlight",
ATTR_BOTTOMMARGIN,      L"bottommargin",
ATTR_CELLPADDING,       L"cellpadding",
ATTR_CELLPADING,        L"cellpading",
ATTR_CELLSPACING,       L"cellspacing",
ATTR_CHECKED,           L"checked",
ATTR_CLASS,             L"class",
ATTR_CLASSID,           L"classid",
ATTR_CLEAR,             L"clear",
ATTR_CODEBASE,          L"codebase",
ATTR_COLOR,             L"color",
ATTR_COLS,              L"cols",
ATTR_COLSPAN,           L"colspan",
ATTR_CONTENT,           L"content",
ATTR_COORDS,            L"coords",
ATTR_DEFER,             L"defer",
ATTR_DIR,               L"dir",
ATTR_DISABLED,          L"disabled",
ATTR_ENCTYPE,           L"enctype",
ATTR_F,                 L"f",
ATTR_FACE,              L"face",
ATTR_FOR,               L"for",
ATTR_FRAMEBORDER,       L"frameborder",
ATTR_FRAMESPACING,      L"framespacing",
ATTR_GPING,             L"gping",
ATTR_GRAMMER,           L"grammer",
ATTR_HEIGHT,            L"height",
ATTR_HREF,              L"href",
ATTR_HSPACE,            L"hspace",
ATTR_HSRC,              L"hsrc",
ATTR_HTTP_EQUIV,        L"http-equiv",
ATTR_ID,                L"id",
ATTR_ISDATA,            L"isdata",
ATTR_LABEL,             L"label",
ATTR_LANG,              L"lang",
ATTR_LANGUAGE,          L"language",
ATTR_LEFTMARGIN,        L"leftmargin",
ATTR_LINK,              L"link",
ATTR_MARGINHEIGHT,      L"marginheight",
ATTR_MARGINWIDTH,       L"marginwidth",
ATTR_MAXLENGTH,         L"maxlength",
ATTR_MEDIA,             L"media",
ATTR_METHOD,            L"method",
ATTR_NAME,              L"name",
ATTR_NATURALSIZEFLAG,   L"naturalsizeflag",
ATTR_NOSHADE,           L"noshade",
ATTR_NOWRAP,            L"nowrap",
ATTR_ONBLUR,            L"onblur",
ATTR_ONCHANGE,          L"onchange",
ATTR_ONCLICK,           L"onclick",
ATTR_ONFOCUS,           L"onfocus",
ATTR_ONINPUT,           L"oninput",
ATTR_ONKEYUP,           L"onkeyup",
ATTR_ONLOAD,            L"onload",
ATTR_ONMOUSEDOWN,       L"onmousedown",
ATTR_ONMOUSEOUT,        L"onmouseout",
ATTR_ONMOUSEOVER,       L"onmouseover",
ATTR_ONSRC,             L"onsrc",
ATTR_ONSUBMIT,          L"onsubmit",
ATTR_PLUGINSPAGE,       L"pluginspage",
ATTR_QUALITY,           L"quality",
ATTR_READONLY,          L"readonly",
ATTR_REL,               L"rel",
ATTR_RIGHTMARGIN,       L"rightmargin",
ATTR_ROWS,              L"rows",
ATTR_ROWSPAN,           L"rowspan",
ATTR_SCOPE,             L"scope",
ATTR_SCROLLING,         L"scrolling",
ATTR_SELECTED,          L"selected",
ATTR_SHAPE,             L"shape",
ATTR_SIZE,              L"size",
ATTR_SRC,               L"src",
ATTR_STYLE,             L"style",
ATTR_SUMMARY,           L"summary",
ATTR_TABINDEX,          L"tabindex",
ATTR_TARGET,            L"target",
ATTR_TEXT,              L"text",
ATTR_TITLE,             L"title",
ATTR_TOPMARGIN,         L"topmargin",
ATTR_TYPE,              L"type",
ATTR_USEMAP,            L"usemap",
ATTR_VALIGN,            L"valign",
ATTR_VALUE,             L"value",
ATTR_VISIBILITY,        L"visibility",
ATTR_VLINK,             L"vlink",
ATTR_VSPACE,            L"vspace",
ATTR_WIDTH,             L"width",
ATTR_WRAP,              L"wrap",
ATTR_XMLNS,             L"xmlns",
ATTR_XML_LANG,          L"xml:lang",
ATTR_PAST_SENTINEL,     L"wordsex-error-attrindex-past", // program error!
NULL, NULL
};

typedef struct entity_tag {
    wchar_t * wcp;
    wchar_t wc;
} ENTITY;

    // This list of 253 lines was easily SED from text at URL:
    // http://www.w3.org/TR/xhtml-modularization/dtd_module_defs.html

    // Another great entity viewing test page:
    // http://petesguide.com/WebStandards/entities/

ENTITY Entities[] = {
   L"nbsp", 160, // no-break space = non-breaking space, U+00A0 ISOnum
   L"iexcl", 161, // inverted exclamation mark, U+00A1 ISOnum
   L"cent", 162, // cent sign, U+00A2 ISOnum
   L"pound", 163, // pound sign, U+00A3 ISOnum
   L"curren", 164, // currency sign, U+00A4 ISOnum
   L"yen", 165, // yen sign = yuan sign, U+00A5 ISOnum
   L"brvbar", 166, // broken bar = broken vertical bar, U+00A6 ISOnum
   L"sect", 167, // section sign, U+00A7 ISOnum
   L"uml", 168, // diaeresis = spacing diaeresis, U+00A8 ISOdia
   L"copy", 169, // copyright sign, U+00A9 ISOnum
   L"ordf", 170, // feminine ordinal indicator, U+00AA ISOnum
   L"laquo", 171, // left-pointing double angle quotation mark = left pointing guillemet, U+00AB ISOnum
   L"not", 172, // not sign, U+00AC ISOnum
   L"shy", 173, // soft hyphen = discretionary hyphen, U+00AD ISOnum
   L"reg", 174, // registered sign = registered trade mark sign, U+00AE ISOnum
   L"macr", 175, // macron = spacing macron = overline = APL overbar, U+00AF ISOdia
   L"deg", 176, // degree sign, U+00B0 ISOnum
   L"plusmn", 177, // plus-minus sign = plus-or-minus sign, U+00B1 ISOnum
   L"sup2", 178, // superscript two = superscript digit two = squared, U+00B2 ISOnum
   L"sup3", 179, // superscript three = superscript digit three = cubed, U+00B3 ISOnum
   L"acute", 180, // acute accent = spacing acute, U+00B4 ISOdia
   L"micro", 181, // micro sign, U+00B5 ISOnum
   L"para", 182, // pilcrow sign = paragraph sign, U+00B6 ISOnum
   L"middot", 183, // middle dot = Georgian comma = Greek middle dot, U+00B7 ISOnum
   L"cedil", 184, // cedilla = spacing cedilla, U+00B8 ISOdia
   L"sup1", 185, // superscript one = superscript digit one, U+00B9 ISOnum
   L"ordm", 186, // masculine ordinal indicator, U+00BA ISOnum
   L"raquo", 187, // right-pointing double angle quotation mark = right pointing guillemet, U+00BB ISOnum
   L"frac14", 188, // vulgar fraction one quarter = fraction one quarter, U+00BC ISOnum
   L"frac12", 189, // vulgar fraction one half = fraction one half, U+00BD ISOnum
   L"frac34", 190, // vulgar fraction three quarters = fraction three quarters, U+00BE ISOnum
   L"iquest", 191, // inverted question mark = turned question mark, U+00BF ISOnum
   L"Agrave", 192, // latin capital A with grave = latin capital A grave, U+00C0 ISOlat1
   L"Aacute", 193, // latin capital A with acute, U+00C1 ISOlat1
   L"Acirc", 194, // latin capital A with circumflex, U+00C2 ISOlat1
   L"Atilde", 195, // latin capital A with tilde, U+00C3 ISOlat1
   L"Auml", 196, // latin capital A with diaeresis, U+00C4 ISOlat1
   L"Aring", 197, // latin capital A with ring above = latin capital A ring, U+00C5 ISOlat1
   L"AElig", 198, // latin capital AE = latin capital ligature AE, U+00C6 ISOlat1
   L"Ccedil", 199, // latin capital C with cedilla, U+00C7 ISOlat1
   L"Egrave", 200, // latin capital E with grave, U+00C8 ISOlat1
   L"Eacute", 201, // latin capital E with acute, U+00C9 ISOlat1
   L"Ecirc", 202, // latin capital E with circumflex, U+00CA ISOlat1
   L"Euml", 203, // latin capital E with diaeresis, U+00CB ISOlat1
   L"Igrave", 204, // latin capital I with grave, U+00CC ISOlat1
   L"Iacute", 205, // latin capital I with acute, U+00CD ISOlat1
   L"Icirc", 206, // latin capital I with circumflex, U+00CE ISOlat1
   L"Iuml", 207, // latin capital I with diaeresis, U+00CF ISOlat1
   L"ETH", 208, // latin capital ETH, U+00D0 ISOlat1
   L"Ntilde", 209, // latin capital N with tilde, U+00D1 ISOlat1
   L"Ograve", 210, // latin capital O with grave, U+00D2 ISOlat1
   L"Oacute", 211, // latin capital O with acute, U+00D3 ISOlat1
   L"Ocirc", 212, // latin capital O with circumflex, U+00D4 ISOlat1
   L"Otilde", 213, // latin capital O with tilde, U+00D5 ISOlat1
   L"Ouml", 214, // latin capital O with diaeresis, U+00D6 ISOlat1
   L"times", 215, // multiplication sign, U+00D7 ISOnum
   L"Oslash", 216, // latin capital O with stroke = latin capital O slash, U+00D8 ISOlat1
   L"Ugrave", 217, // latin capital U with grave, U+00D9 ISOlat1
   L"Uacute", 218, // latin capital U with acute, U+00DA ISOlat1
   L"Ucirc", 219, // latin capital U with circumflex, U+00DB ISOlat1
   L"Uuml", 220, // latin capital U with diaeresis, U+00DC ISOlat1
   L"Yacute", 221, // latin capital Y with acute, U+00DD ISOlat1
   L"THORN", 222, // latin capital THORN, U+00DE ISOlat1
   L"szlig", 223, // latin small sharp s = ess-zed, U+00DF ISOlat1
   L"agrave", 224, // latin small a with grave = latin small a grave, U+00E0 ISOlat1
   L"aacute", 225, // latin small a with acute, U+00E1 ISOlat1
   L"acirc", 226, // latin small a with circumflex, U+00E2 ISOlat1
   L"atilde", 227, // latin small a with tilde, U+00E3 ISOlat1
   L"auml", 228, // latin small a with diaeresis, U+00E4 ISOlat1
   L"aring", 229, // latin small a with ring above = latin small a ring, U+00E5 ISOlat1
   L"aelig", 230, // latin small ae = latin small ligature ae, U+00E6 ISOlat1
   L"ccedil", 231, // latin small c with cedilla, U+00E7 ISOlat1
   L"egrave", 232, // latin small e with grave, U+00E8 ISOlat1
   L"eacute", 233, // latin small e with acute, U+00E9 ISOlat1
   L"ecirc", 234, // latin small e with circumflex, U+00EA ISOlat1
   L"euml", 235, // latin small e with diaeresis, U+00EB ISOlat1
   L"igrave", 236, // latin small i with grave, U+00EC ISOlat1
   L"iacute", 237, // latin small i with acute, U+00ED ISOlat1
   L"icirc", 238, // latin small i with circumflex, U+00EE ISOlat1
   L"iuml", 239, // latin small i with diaeresis, U+00EF ISOlat1
   L"eth", 240, // latin small eth, U+00F0 ISOlat1
   L"ntilde", 241, // latin small n with tilde, U+00F1 ISOlat1
   L"ograve", 242, // latin small o with grave, U+00F2 ISOlat1
   L"oacute", 243, // latin small o with acute, U+00F3 ISOlat1
   L"ocirc", 244, // latin small o with circumflex, U+00F4 ISOlat1
   L"otilde", 245, // latin small o with tilde, U+00F5 ISOlat1
   L"ouml", 246, // latin small o with diaeresis, U+00F6 ISOlat1
   L"divide", 247, // division sign, U+00F7 ISOnum
   L"oslash", 248, // latin small o with stroke, = latin small o slash, U+00F8 ISOlat1
   L"ugrave", 249, // latin small u with grave, U+00F9 ISOlat1
   L"uacute", 250, // latin small u with acute, U+00FA ISOlat1
   L"ucirc", 251, // latin small u with circumflex, U+00FB ISOlat1
   L"uuml", 252, // latin small u with diaeresis, U+00FC ISOlat1
   L"yacute", 253, // latin small y with acute, U+00FD ISOlat1
   L"thorn", 254, // latin small thorn with, U+00FE ISOlat1
   L"yuml", 255, // latin small y with diaeresis, U+00FF ISOlat1
   L"lt", 60, // less-than sign, U+003C ISOnum
   L"gt", 62, // greater-than sign, U+003E ISOnum
   L"amp", 38, // ampersand, U+0026 ISOnum
   L"apos", 39, // The Apostrophe ( Apostrophe Quote, APL Quote ), U+0027 ISOnum
   L"quot", 34, // quotation mark ( Quote Double ), U+0022 ISOnum
   L"OElig", 338, // latin capital ligature OE, U+0152 ISOlat2
   L"oelig", 339, // latin small ligature oe, U+0153 ISOlat2
   L"Scaron", 352, // latin capital letter S with caron, U+0160 ISOlat2
   L"scaron", 353, // latin small letter s with caron, U+0161 ISOlat2
   L"Yuml", 376, // latin capital letter Y with diaeresis, U+0178 ISOlat2
   L"circ", 710, // modifier letter circumflex accent, U+02C6 ISOpub
   L"tilde", 732, // small tilde, U+02DC ISOdia
   L"ensp", 8194, // en space, U+2002 ISOpub
   L"emsp", 8195, // em space, U+2003 ISOpub
   L"thinsp", 8201, // thin space, U+2009 ISOpub
   L"zwnj", 8204, // zero width non-joiner, U+200C NEW RFC 2070
   L"zwj", 8205, // zero width joiner, U+200D NEW RFC 2070
   L"lrm", 8206, // left-to-right mark, U+200E NEW RFC 2070
   L"rlm", 8207, // right-to-left mark, U+200F NEW RFC 2070
   L"ndash", U_2013, // en dash, U+2013 ISOpub
   L"mdash", U_2014, // em dash, U+2014 ISOpub
   L"lsquo", U_2018, // left single quotation mark, U+2018 ISOnum
   L"rsquo", U_2019, // right single quotation mark, U+2019 ISOnum
   L"sbquo", U_201A, // single low-9 quotation mark, U+201A NEW
   L"ldquo", U_201C, // left double quotation mark, U+201C ISOnum
   L"rdquo", U_201D, // right double quotation mark, U+201D ISOnum
   L"bdquo", U_201E, // double low-9 quotation mark, U+201E NEW
   L"dagger", 8224, // dagger, U+2020 ISOpub
   L"Dagger", 8225, // double dagger, U+2021 ISOpub
   L"permil", 8240, // per mille sign, U+2030 ISOtech
   L"lsaquo", 8249, // single left-pointing angle quotation mark, U+2039 ISO proposed
   L"rsaquo", 8250, // single right-pointing angle quotation mark, U+203A ISO proposed
   L"euro", 8364, // euro sign, U+20AC NEW
   L"fnof", 402, // latin small f with hook = function
   L"Alpha", 913, // greek capital letter alpha, U+0391
   L"Beta", 914, // greek capital letter beta, U+0392
   L"Gamma", 915, // greek capital letter gamma, U+0393 ISOgrk3
   L"Delta", 916, // greek capital letter delta, U+0394 ISOgrk3
   L"Epsilon", 917, // greek capital letter epsilon, U+0395
   L"Zeta", 918, // greek capital letter zeta, U+0396
   L"Eta", 919, // greek capital letter eta, U+0397
   L"Theta", 920, // greek capital letter theta, U+0398 ISOgrk3
   L"Iota", 921, // greek capital letter iota, U+0399
   L"Kappa", 922, // greek capital letter kappa, U+039A
   L"Lambda", 923, // greek capital letter lambda, U+039B ISOgrk3
   L"Mu", 924, // greek capital letter mu, U+039C
   L"Nu", 925, // greek capital letter nu, U+039D
   L"Xi", 926, // greek capital letter xi, U+039E ISOgrk3
   L"Omicron", 927, // greek capital letter omicron, U+039F
   L"Pi", 928, // greek capital letter pi, U+03A0 ISOgrk3
   L"Rho", 929, // greek capital letter rho, U+03A1
   L"Sigma", 931, // greek capital letter sigma, U+03A3 ISOgrk3
   L"Tau", 932, // greek capital letter tau, U+03A4
   L"Upsilon", 933, // greek capital letter upsilon,
   L"Phi", 934, // greek capital letter phi, U+03A6 ISOgrk3
   L"Chi", 935, // greek capital letter chi, U+03A7
   L"Psi", 936, // greek capital letter psi, U+03A8 ISOgrk3
   L"Omega", 937, // greek capital letter omega, U+03A9 ISOgrk3
   L"alpha", 945, // greek small letter alpha, U+03B1 ISOgrk3
   L"beta", 946, // greek small letter beta, U+03B2 ISOgrk3
   L"gamma", 947, // greek small letter gamma, U+03B3 ISOgrk3
   L"delta", 948, // greek small letter delta, U+03B4 ISOgrk3
   L"epsilon", 949, // greek small letter epsilon, U+03B5 ISOgrk3
   L"zeta", 950, // greek small letter zeta, U+03B6 ISOgrk3
   L"eta", 951, // greek small letter eta, U+03B7 ISOgrk3
   L"theta", 952, // greek small letter theta, U+03B8 ISOgrk3
   L"iota", 953, // greek small letter iota, U+03B9 ISOgrk3
   L"kappa", 954, // greek small letter kappa, U+03BA ISOgrk3
   L"lambda", 955, // greek small letter lambda, U+03BB ISOgrk3
   L"mu", 956, // greek small letter mu, U+03BC ISOgrk3
   L"nu", 957, // greek small letter nu, U+03BD ISOgrk3
   L"xi", 958, // greek small letter xi, U+03BE ISOgrk3
   L"omicron", 959, // greek small letter omicron, U+03BF NEW
   L"pi", 960, // greek small letter pi, U+03C0 ISOgrk3
   L"rho", 961, // greek small letter rho, U+03C1 ISOgrk3
   L"sigmaf", 962, // greek small letter final sigma, U+03C2 ISOgrk3
   L"sigma", 963, // greek small letter sigma, U+03C3 ISOgrk3
   L"tau", 964, // greek small letter tau, U+03C4 ISOgrk3
   L"upsilon", 965, // greek small letter upsilon, U+03C5 ISOgrk3
   L"phi", 966, // greek small letter phi, U+03C6 ISOgrk3
   L"chi", 967, // greek small letter chi, U+03C7 ISOgrk3
   L"psi", 968, // greek small letter psi, U+03C8 ISOgrk3
   L"omega", 969, // greek small letter omega, U+03C9 ISOgrk3
   L"thetasym", 977, // greek small letter theta symbol, U+03D1 NEW
   L"upsih", 978, // greek upsilon with hook symbol, U+03D2 NEW
   L"piv", 982, // greek pi symbol, U+03D6 ISOgrk3
   L"bull", 8226, // bullet = black small circle, U+2022 ISOpub
   L"hellip", 8230, // horizontal ellipsis = three dot leader, U+2026 ISOpub
   L"prime", 8242, // prime = minutes = feet, U+2032 ISOtech
   L"Prime", 8243, // double prime = seconds = inches, U+2033 ISOtech
   L"oline", 8254, // overline = spacing overscore, U+203E NEW
   L"frasl", 8260, // fraction slash, U+2044 NEW
   L"weierp", 8472, // script capital P = power set = Weierstrass p, U+2118 ISOamso
   L"image", 8465, // blackletter capital I = imaginary part, U+2111 ISOamso
   L"real", 8476, // blackletter capital R = real part symbol, U+211C ISOamso
   L"trade", 8482, // trade mark sign, U+2122 ISOnum
   L"alefsym", 8501, // alef symbol = first transfinite cardinal, U+2135 NEW
   L"larr", 8592, // leftwards arrow, U+2190 ISOnum
   L"uarr", 8593, // upwards arrow, U+2191 ISOnum
   L"rarr", 8594, // rightwards arrow, U+2192 ISOnum
   L"darr", 8595, // downwards arrow, U+2193 ISOnum
   L"harr", 8596, // left right arrow, U+2194 ISOamsa
   L"crarr", 8629, // downwards arrow with corner leftwards
   L"lArr", 8656, // leftwards double arrow, U+21D0 ISOtech
   L"uArr", 8657, // upwards double arrow, U+21D1 ISOamsa
   L"rArr", 8658, // rightwards double arrow, U+21D2 ISOtech
   L"dArr", 8659, // downwards double arrow, U+21D3 ISOamsa
   L"hArr", 8660, // left right double arrow, U+21D4 ISOamsa
   L"forall", 8704, // for all, U+2200 ISOtech
   L"part", 8706, // partial differential, U+2202 ISOtech
   L"exist", 8707, // there exists, U+2203 ISOtech
   L"empty", 8709, // empty set = null set, U+2205 ISOamso
   L"nabla", 8711, // nabla = backward difference, U+2207 ISOtech
   L"isin", 8712, // element of, U+2208 ISOtech
   L"notin", 8713, // not an element of, U+2209 ISOtech
   L"ni", 8715, // contains as member, U+220B ISOtech
   L"prod", 8719, // n-ary product = product sign, U+220F ISOamsb
   L"sum", 8721, // n-ary sumation, U+2211 ISOamsb
   L"minus", U_2212, // minus sign, U+2212 ISOtech
   L"lowast", 8727, // asterisk operator, U+2217 ISOtech
   L"radic", 8730, // square root = radical sign, U+221A ISOtech
   L"prop", 8733, // proportional to, U+221D ISOtech
   L"infin", 8734, // infinity, U+221E ISOtech
   L"ang", 8736, // angle, U+2220 ISOamso
   L"and", 8743, // logical and = wedge, U+2227 ISOtech
   L"or", 8744, // logical or = vee, U+2228 ISOtech
   L"cap", 8745, // intersection = cap, U+2229 ISOtech
   L"cup", 8746, // union = cup, U+222A ISOtech
   L"int", 8747, // integral, U+222B ISOtech
   L"there4", 8756, // therefore, U+2234 ISOtech
   L"sim", 8764, // tilde operator = varies with = similar to, U+223C ISOtech
   L"cong", 8773, // approximately equal to, U+2245 ISOtech
   L"asymp", 8776, // almost equal to = asymptotic to, U+2248 ISOamsr
   L"ne", 8800, // not equal to, U+2260 ISOtech
   L"equiv", 8801, // identical to, U+2261 ISOtech
   L"le", 8804, // less-than or equal to, U+2264 ISOtech
   L"ge", 8805, // greater-than or equal to, U+2265 ISOtech
   L"sub", 8834, // subset of, U+2282 ISOtech
   L"sup", 8835, // superset of, U+2283 ISOtech
   L"nsub", 8836, // not a subset of, U+2284 ISOamsn
   L"sube", 8838, // subset of or equal to, U+2286 ISOtech
   L"supe", 8839, // superset of or equal to, U+2287 ISOtech
   L"oplus", 8853, // circled plus = direct sum, U+2295 ISOamsb
   L"otimes", 8855, // circled times = vector product, U+2297 ISOamsb
   L"perp", 8869, // up tack = orthogonal to = perpendicular, U+22A5 ISOtech
   L"sdot", 8901, // dot operator, U+22C5 ISOamsb
   L"lceil", 8968, // left ceiling = apl upstile, U+2308 ISOamsc
   L"rceil", 8969, // right ceiling, U+2309 ISOamsc
   L"lfloor", 8970, // left floor = apl downstile, U+230A ISOamsc
   L"rfloor", 8971, // right floor, U+230B ISOamsc
   L"lang", 9001, // left-pointing angle bracket = bra, U+2329 ISOtech
   L"rang", 9002, // right-pointing angle bracket = ket, U+232A ISOtech
   L"loz", 9674, // lozenge, U+25CA ISOpub
   L"spades", 9824, // black spade suit, U+2660 ISOpub
   L"clubs", 9827, // black club suit = shamrock, U+2663 ISOpub
   L"hearts", 9829, // black heart suit = valentine, U+2665 ISOpub
   L"diams", 9830, // black diamond suit, U+2666 ISOpub
NULL, NULL
};

// Original lists are ok for the Smithsonian, but I have
// to have Unicode runs, so recreate vowel and consonant
// run lists like the words, cloning their same r/w code.
// These counts were gotten from the built-in word lists,
// when DEBUG_REVISE_VOWELS is 1; Start; Save word lists.

unsigned char Vowels001[] = {
"18a18ei18ou18\34418i18y18e18ai18ee18\35118io18aa18u17ia17\34117\34517ue17ea17oo17au17ui17o17\35517\36616oi16eu16\36316oe16ua16ii16\37016\37416\34615\001\00315uu15\35015oa15\34315ae15\37215uo15\001\03315\344\34415eo15\36515\37515ao14\343o14\365i14\352"
"14ie14i\001\03114\34214\344i14iu14ye14i\36313\001\12113ieu13\001\03113ya13\366\36613yi13a\34613\355a13eau13yo13i\35013y\36612\351e12\001\15712\350i12\36412uie12\001\00512i\35212eie12\365e12i\35112iou12\344e12iau12u\35112i\34412\350e11uoi11iei11aie"
"11iii11\365u11aai11y\34611oie11iae11i\34611iai11\351a11eue11iaa11a\35511e\34411\345e11ooi11\350a11e\35011ioi11uue11eeu11iia11a\34311\343u11\351\35111\370e11ioo11a\350e11eia11eea10\374\37410u\34110o\35010\001\16110aio10\366\366a10u\35210ioa10o\350i"
"10a\35010\001\003u10uei10oui10y\34410a\350i10i\34110\346e10O10\001\005e10aue10uai10a\350u10i\350e10\370\35510uee10i\001\00510\372a10o\34610o\346i10\344u10aua10oei10eio10u\35510\366i10e\346i10\366a10oeu10ie\34610\001\0729y\3519\372i9\351u9yu9\351o"
"9i\3669i\350a9\344ie9e\350a9i\343o9\355o9\341u9oua9y\3419i\3439uua9eaa9ou\001\0039u\346i9e\3469uia9u\3509oue9u\3469eou9y\3749\370i9eaae9\001\003i9ioe9ia\3469aiu9a\3729aia9\366e9e\350i9e\350e9o\350e9oia9\350o9eua9\363\3638iee8\350\3558\350u8e\346e8eoe"
"8\344o8i\350i8aea8y\3438e\3458\343e8\365ua8\341i8o\350\3558io\3508o\350a8e\344\3448e\3438eei8iao8a\350a8i\001\005e8ui\3518i\365i8aae8\351ia8u\350i8iua8uaa8e\346a8eoo8\351i8o\3458o\343o8oai8eoa8ooie8e\3418euo8\342i8o\3748\001\121e8u\350a8i\346i8e\372"
"8a\3748\374\374a8i\3728\355u8a\3458\363a8e\001\0058e\3558a\346i8y\3558ueu8u\350e8u\3458i\3708eui8\346i8y\3637o\346u7i\3427iie7\365ue7i\3557e\3517a\001\005a7\350\3417i\346e7eoi7o\001\0057i\365e7iuo7uae7o\3557a\001\0057uea7\363i7o\3447\350aa7i\364"
"7a\001\005e7o\3517oeie7u\355a7o\001\0727e\3637eaua7eai7y\344\3447\341a7y\001\1617oueu7i\351e7\001\161e7oaa7ioia7\351\3427eii7eao7\355\350e7e\3747ou\3437\372\350e7\366\3447\344ia7oiu7aeo7\001\005i7\341\370\3557uou7aaie7u\346a7euau6y\3656oio6eaia6aii"
"6\370ie6y\3726\370\3706ooa6i\3656\374\3526u\346e6\343\3436yei6u\3746a\001\005i6\001\121i6\365\3656iiu6iaia6e\370e6y\3456i\352\3466ieua6eoau6eiu6\001\003ie6\341e6u\001\0036uio6ieue6\001\005\3556\001\005a6uiu6a\001\0726\346a6yuu6\345i6ia\3456\366\366aa"
"6\355e6\341\3556u\3436i\001\0726\370\3416ui\3636o\370e6e\355a6e\343o6a\355a6\001\003ia6ieee6e\3666e\350\3556a\3516a\346e6a\346a5\351ai5y\001\0725e\370\3555\351\351e5ou\3515oaie5i\351i5iea5euae5\374e5\363u5\341\3505ui\3505i\366i5\001\121o5\365ie5\346o"
"5e\3525eeuu5aei5\350\355e5\344\344o5\344\344a5\341\3705\341\3415yia5u\351e5u\3425ueue5o\370\3555ouo5a\3665\370a5\366\366ae5\344ai5\342\3425yuo5u\3655oi\3505i\001\005o5e\3655eouu5eae5a\350o5\370\355\350i5\350\355\3705\341\350e5yui5yai5u\350u5ouai5ieo"
"5euoe5eaue5a\370\3555\001\121a5\365a5\343a5uoe5uiau5ou\3445oii5i\001\005a5a\3705aee4\375u4\374\3554\352e4\346u4u\350io4o\350eo4o\346a4ou\3454e\001\0724aiia4\372u4u\001\0054ua\3454oao4i\001\005i4e\3704euu4ei\3434a\001\005o4a\3414auu4aoi4aoe4\366u"
"4\355\3634\341\3464\341au4yue4yau4u\3444uao4o\3414i\3454\375i4\342u4o\3434au\3430"};

unsigned char Consonants002[] = {
"18n18j18nd18h18nt18ll18b18l18g18ng18ch18pr18f18th18st18tt17k17nn17y17p17rt17q17r17s17ss17ns17w17tr17c17rd17rs17bl17lt16\36016v16rm16gr16fr16mm16sp16sk16nc16z16br16\004\06216rn16kk16dd16ld16dr16t16ct16ls16\004\075\004\06016m16\004\07016x16pl16sv16sl"
"16kt16ks16\37616str16\006\11016rk15nk15nj15rr15rg15ts15sh15ft15cht15\001\01515gl15lj15wh15pp15ntr15rv15\001\17615dn15mp15cr15ff15sz15ck15lk15jn15mb15kr15rl15\34015\35615\006\105\006\10615\006\101\006\11215nf15\011\02515\014\24415\013\225"
"15\005\320\005\35215hv15\004\075\004\06515gn15cl15yn15lm15kl15vr15sc15\013\24415\001\00715wn15\004\10315\005\351\005\33415rst15sm15\003\272\003\261\003\27114\001\14114pt14nl14\004\12614rch14lg14\004\064\004\06014sch14cz14sn14\013\25214gg14rj"
"14\014\25014rc14rb14\004\067\004\06014jk14\014\26014\014\27014ht14\005\342\005\33414hr14\001\13714rh14lv14nts14nst14rf14cc14bb14\014\22714\004\06714\001\17414fl14\006\050\006\10714gt14nv14\011\07114mpl14\004\10114zn14rz14\011\05614mn14ndr14ws14dz"
"14\006\071\006\104\006\11114\003\304\003\27714spr14\013\25614nz14tk14ln14dl14\001\10214\004\076\004\10214\006\057\006\06114vn14\011\06014\003\275\003\26114\004\101\004\06514tw14\011\07014\004\077\004\07614ght14tv14\014\24614\003\304\003\277\003\305"
"14gh14nsk14\356n14\34714ds14\014\22514\011\04414\006\047\006\06214pm14\35714\004\06514kn14\011\05014\014\26514nh14ms14prz14ps14\004\06014ph14\001\141t14\014\25714\006\06714\011\06214www14dv14lf13jd13\006\07013\013\26213\013\23713\004\102\004\06013cs"
"13\013\25113\013\26113sj13\003\304\003\267\003\27513tn13ys13gs13\006\043\006\10613\006\251\006\10713\006\061\006\04713\006\047\006\104\006\104\006\10713ngs13mbr13gw13\013\25713\004\107\004\102\004\07613rw13mt13chr13\37113ngl13\005\333\005\33413wr13tl"
"13gj13\003\26713\003\304\003\267\003\30213\004\064\004\07613\006\105\006\04713lp13\013\26013hj13xt13\011\04613\013\23213http13lb13\006\045\006\104\006\11113\013\225\013\26313nds13\013\26513\005\320\005\32513vs13\011\05713\003\274\003\265"
"13\005\342\005\33513ml13\003\263\003\271\003\26113\36113bs13\003\300\003\277\003\30513\005\326\005\32413r\36013\014\25613mpr13hn13kd13\006\104\006\04713hl13\011\03413rts13\003\27713llt13\014\26213lh13\003\261\003\300\003\31413scr13\005\334\005\320"
"13dj13\006\043\006\11013\013\25013\006\047\006\063\006\05213rth13\004\07413\006\11213xp13\006\071\006\10613\014\25213\006\10513\005\346\005\331\005\336\005\350\005\331\005\33513\004\075\004\114\004\11613\004\07113\011\02713mr13kv13rss13chn13fn"
"13\014\24113js13hk13\003\304\003\26113dt13\004\075\004\07613thr13p\001\13113\004\11713rp13bj13gd13\004\064\004\073\004\11713\004\072\004\060\004\07213\003\265\003\257\003\275\003\261\003\27113hm13\004\101\004\07013hd13vl13\001\143"
"13\004\071\004\076\004\100\004\07213\004\126\004\075\004\102\004\065\004\100\004\075\004\065\004\10213bn13\004\072\004\075\004\070\004\063\004\10313\004\106\004\11613\004\102\004\07613nch13\004\07213d13nm13\004\107\004\06513yd13dm13zd13zt13mj13sr13dg"
"13\006\050\006\04713\011\06513skr13\005\334\005\33213\004\077\004\076\004\110\004\103\004\07213\006\05713\005\346\005\331\005\336\005\35013sw13ndl13\33712\004\101\004\073\004\076\004\06212\011\04512\003\304\003\277\003\27512rdt12zw"
"12\003\264\003\265\003\27512\003\303\003\304\003\27712\004\074\004\073\004\07512lw12rsk12zz12\004\111\004\06512\006\10412\014\23712zv12yr12\011\05412tm12tj12\006\105\006\31412\004\10012\001\015l12\004\067\004\075\004\060\004\071\004\102\004\07012chw"
"12\004\077\004\100\004\07612md12\004\115\004\102\004\07612vlj12\011\05212\003\303\003\26512sts12wsz12lsk12\004\100\004\076\004\10112ppl12\014\227\014\26312ldr12\005\320\005\33412rld12\003\270\003\26112\006\107\006\060\006\04712tz12ntl12\014\263"
"12\006\10712ncl12dw12\005\350\005\331\005\331\005\347\005\33112\003\304\003\311\003\27512\004\101\004\10212zb12stv12\005\324\005\320\005\352\005\35012\004\102\004\076\004\062\004\06012\001\02112ndt12\004\070\004\073\004\07012bt"
"12\005\334\005\334\005\32012\003\303\003\304\003\267\003\27512\014\26612\005\351\005\334\005\33212n\34712\001\13112\003\304\003\277\003\305\003\30212\004\100\004\076\004\101\004\126\004\117\004\075\004\07012\004\072\004\063\004\10612\003\277\003\271"
"12\006\047\006\314\006\10612\006\04712nr12\006\103\006\047\006\10612rds12yt12\006\05212\005\320\005\33512\006\103\006\10712kj12hrv12\014\27112\014\216\014\25312\004\062\004\101\004\06512\005\352\005\336\005\325\005\340\005\325\005\35212\376v12szt"
"12mst12\004\076\004\07512nw12tsk12vj12\004\07612ngt12\001\015n12\004\117\004\07212zl12\005\320\005\352\005\35012vd12dst12rsch12\006\07112nks12\004\107\004\065\004\100\004\065\004\06712nb12\005\322\005\33512\005\320\005\331\005\332"
"12\006\071\006\104\006\112\006\10712prv12kst12ym12\005\336\005\32412tch12zm12\013\26312\006\047\006\104\006\052\006\11212tc12nsch12\004\101\004\06012\004\074\004\07012\005\320\005\340\005\33112\004\070\004\06712lch12lc12rq12stk12cks"
"12\006\047\006\112\006\10612\003\304\003\26712czn12jl12cts12\005\340\005\331\005\324\005\325\005\33412\005\331\005\325\005\352\005\35012nstr12\006\047\006\104\006\071\006\061\006\050\006\112\006\05112t\34012\005\320\005\352\005\32412lq12rdpr"
"12\004\072\004\060\004\102\004\07612\011\06612\006\06112\006\107\006\060\006\10712sq12ngr12\006\042\006\10612db12\004\061\004\065\004\06712\014\265\014\24412\005\321\005\331\005\33712\004\111\004\07612\006\047\006\10412ltr12\011\05512schr"
"12\006\050\006\10612\006\110\006\104\006\04712\006\05012\004\065\004\063\004\07612v\001\14112jv12lls12wnl12ttr12rdd12\014\256\014\24412\005\331\005\351\005\350\005\320\005\33412\005\324\005\325\005\32012\005\320\005\352\005\350\005\331\005\335"
"12\005\320\005\351\005\35012\006\105\006\07112\005\331\005\35112\004\061\004\103\004\064\004\11412nkt12\006\102\006\047\006\10412\003\274\003\261\003\30212km12\006\103\006\10412sg12dk12\014\216\014\27012tg12pc12\006\072\006\112\006\06112php12ntw"
"12\003\314\003\304\003\27112ftw12cj12pd12\006\107\006\10512sd12jz12\005\334\005\333\005\33412\005\326\005\32512\005\32412\004\101\004\102\004\060\004\102\004\102\004\12612\011\03212sf12\011\024\011\06012\014\25412\006\060\006\104\006\10312sb"
"12\005\334\005\333\005\33512\006\10612\004\075\004\07012zr12nq12\005\321\005\331\005\325\005\352\005\35012\004\102\004\060\004\07212\003\304\003\271\003\30212stn12\004\074\004\11312\004\072\004\112\004\07412dr\001\17612\004\062\004\070"
"12\004\077\004\100\004\07012\011\03712pn12\006\050\006\071\006\05712html12ytt12\005\340\005\331\005\347\005\325\005\32312\005\336\005\351\005\327\005\347\005\33112\004\102\004\060\004\072\004\076\004\066"
"12\004\101\004\102\004\060\004\102\004\102\004\11712\004\100\004\076\004\101\004\126\004\117\004\07512\001\07612\006\112\006\04712\006\107\006\11012rkl12nct11ppr11\005\342\005\32311\005\333\005\33111\006\05411jt11\003\274\003\277\003\30511\001\141k"
"11mbl11\004\100\004\065\004\06411nnt11tsch11dnj11\011\01711\004\102\004\06511\004\074\004\075\004\076\004\063\004\07611xpl11\013\252\013\23711rks11\014\23211\356ntr11kw11yht11\001\137t11yw11\004\073\004\07011ttl11\004\103\004\072\004\100"
"11\006\047\006\104\006\060\006\11211zg11ghts11\011\02611\006\104\006\10511\005\331\005\323\005\33111mps11spl11lth11mf11trz11xc11wd11\004\106\004\065\004\075\004\102\004\10011\005\350\005\34711\005\347\005\351\005\35011\005\342\005\341\005\347"
"11\005\336\005\342\005\350\005\333\005\35211\005\336\005\325\005\326\005\331\005\320\005\325\005\33711\004\074\004\076\004\066\004\06511\004\070\004\10511chts11zdr11fd11\005\333\005\323\005\33111\004\063\004\07611nns11\006\047\006\110"
"11\006\043\006\050\006\11011ssz11\004\127\004\10511\004\126\004\07511\004\12411\004\101\004\102\004\060\004\102\004\102\004\11611\004\074\004\060\004\063\004\060\004\067\004\070\004\075\004\07011\004\072\004\075\004\070\004\063"
"11\004\062\004\126\004\06411\006\050\006\112\006\10611\003\303\003\304\003\26111\006\107\006\047\006\31411mlj11\004\102\004\076\004\07111\006\105\006\055\006\105\006\05711\005\352\005\325\005\323\005\32411\005\346\005\331\005\325\005\337"
"11\005\333\005\340\005\331\005\341\005\325\005\35211\005\331\005\350\005\325\005\351\005\334\005\331\005\33511\006\045\006\060\006\04711rkt11\006\10211gm11tzt11\001\133c11\004\072\004\076\004\070\004\102\004\07611\006\056\006\110\006\057"
"11\005\336\005\352\005\340\005\325\005\35211\005\336\005\351\005\327\005\347\005\331\005\33511\005\336\005\323\005\350\005\331\005\33211\005\333\005\320\005\33711\005\321\005\333\005\33411\006\10311\006\050\006\110\006\05711\006\047\006\06111f\360"
"11np11\006\107\006\04711\005\341\005\347\005\34111\005\320\005\331\005\33711\011\05311szcz11cn11\004\066\004\06511\003\255\003\275\003\26111dp11fj11\014\26711jm11\006\10111tst11\006\050\006\06111srpsk"
"11\004\101\004\070\004\101\004\102\004\065\004\074\004\07011\011\04111xtr11\006\050\006\061\006\047\006\31411nsl11mg11\006\064\006\057\006\10711\006\042\006\056\006\06111\005\320\005\327\005\32311\006\053\006\10511\004\074\004\10311lts"
"11\004\076\004\075\004\07011\003\274\003\271\003\26111\005\352\005\325\005\333\005\33711\005\321\005\320\005\352\005\35011\004\072\004\075\004\070\004\063\004\07011\003\25611\006\045\006\10611\011\03311l\34011knj11\013\216\013\25111\006\104\006\107"
"11\004\065\004\101\004\073\004\07011\005\324\005\321\005\331\005\35211\013\26411tb11lr11\003\261\003\305\003\304\003\31411rms11\006\045\006\104\006\04711\011\017\011\02511\006\043\006\11211\005\321\005\352\005\320\005\350\005\331\005\33211rdz"
"11\011\050\011\07111\006\102\006\05711\006\052\006\04711\004\075\004\076\004\062\004\12611\004\075\004\060\004\100\004\076\004\064\004\126\004\06211\004\074\004\060\004\063\004\060\004\067\004\070\004\075\004\126"
"11\004\073\004\060\004\101\004\072\004\06011\006\04311\006\103\006\105\006\04711\005\334\005\324\005\331\005\325\005\35211\005\324\005\331\005\32011\004\101\004\110\004\06011nkk11\013\24311cd11\004\062\004\113"
"11\004\102\004\076\004\073\004\114\004\072\004\07611\003\303\003\261\003\30211\004\075\004\060\004\07111\006\071\006\106\006\05711yl11srb11\004\077\004\100\004\076\004\101\004\102\004\07611bd11xpr11bm11\006\055\006\052\006\111"
"11\003\303\003\304\003\26711\360r11ktr11dvd11wb11\005\324\005\327\005\331\005\331\005\33511\005\320\005\340\005\351\005\331\005\33511\006\050\006\071\006\06611\004\102\004\07011\004\060\004\072\004\07611\001\176n11ndj11\013\252\013\2440"};

unsigned char Consonants003[] = {
"11\011\052\011\06011nss11\004\10211\004\072\004\076\004\063\004\064\004\06011bst11nrh11wl11\003\261\003\273\003\273\003\25411stl11mh11\004\117\004\075\004\07011\004\101\004\070\004\101\004\102\004\065\004\074\004\103"
"11\004\101\004\065\004\100\004\077\004\075\004\11711\004\101\004\062\004\126\004\102\004\10311\004\077\004\126\004\06411\004\077\004\076\004\06211\006\06311ctr11\004\076\004\075\004\06011\014\225\014\26011\011\025\011\06011ldn11\005\331"
"11\005\324\005\342\005\334\005\320\005\35211\005\320\005\340\005\32511gwn11mpt11\004\075\004\060\004\10111\36211\004\061\004\113\004\102\004\11411nth11\006\047\006\050\006\10611pdf11\004\101\004\060\004\074\004\07611jb11\005\333\005\332"
"11\005\327\005\323\005\35111ncr11\011\060\011\07111rpr11rtr11g\36011\004\06311\004\060\004\06711nkr11ffr11pg11\006\101\006\112\006\10711nsp11\004\102\004\076\004\074\004\10311\004\075\004\065\004\063\004\07611skj11\006\105\006\053\006\104"
"11\005\334\005\33111\005\324\005\341\005\344\005\35011\005\323\005\34311msn11\004\061\004\11311nfl11\006\047\006\10611rsz11\003\303\003\304\003\277\003\27511\004\072\004\076\004\065\004\102\004\07611s\001\10211\004\070\004\074\004\06011\014\21611jpg"
"11\005\352\005\336\005\325\005\340\005\32411\005\351\005\331\005\350\005\334\005\33111\005\33411\005\324\005\331\005\32411\005\321\005\320\005\336\005\346\005\342\005\325\005\35211ndm11\003\256\003\304\003\261\003\27511\004\070\004\074"
"11\006\110\006\107\006\11011\003\261\003\27511\006\047\006\104\006\11111\004\126\004\075\004\110\004\12611\004\117\004\072\004\12611\004\106\004\126\004\124\004\12711\004\106\004\06511\004\103\004\107\004\060\004\101\004\102\004\11411\004\101\004\105"
"11\004\100\004\060\004\064\004\126\004\07611\004\077\004\100\004\065\004\10111\004\075\004\060\004\10611\004\074\004\126\004\101\004\106\004\065\004\062\004\070\004\10511\004\072\004\103\004\077\004\070\004\102\004\070"
"11\004\072\004\075\004\070\004\063\004\060\004\100\004\075\004\12611\004\067\004\065\004\074\004\065\004\073\004\11411\004\064\004\070\004\06211\004\062\004\126\004\073\004\114\004\075\004\076\004\12711\004\062\004\060\004\110\004\070\004\105"
"11\004\062\004\060\004\102\004\070\004\072\004\060\004\07511\004\060\004\061\004\07611dsk11\006\047\006\104\006\112\006\110\006\10511\004\061\004\113\004\073\004\07611\006\050\006\110\006\047\006\063\006\067\006\05111vt"
"11\006\105\006\064\006\047\006\061\006\103\006\05111\005\351\005\334\005\33111\005\347\005\330\005\340\005\330\005\340\005\331\005\33511\005\336\005\331\005\323\005\34211\005\333\005\336\005\32511\005\333\005\336\005\324"
"11\005\331\005\334\005\323\005\331\005\33511\005\321\005\331\005\35211\013\225\013\25611\006\102\006\050\006\10411sst11nsw11\004\107\004\102\004\076\004\061\004\11311\004\072\004\076\004\071\004\102\004\07611\013\244\013\25611chc"
"11\006\065\006\104\006\11111tp11zp11n\001\14311\006\071\006\104\006\11211lst11\005\334\005\340\005\32511\005\334\005\327\005\34511\005\331\005\325\005\33511\005\324\005\33511\004\065\004\101\004\102\004\11411mmt11chl11\001\015k11\006\061\006\057"
"11\006\107\006\106\006\04711\004\102\004\060\004\072\004\06011\004\074\004\075\004\06511\006\06511ckl11\014\24311\001\141l11rdr11\005\351\005\334\005\32711\005\346\005\331\005\330\005\325\005\330\005\331\005\335"
"11\005\340\005\325\005\341\005\344\005\331\005\33511\005\334\005\347\005\321\005\33411\005\334\005\320\005\352\005\35011\005\327\005\331\005\331\005\33511\005\320\005\352\005\33511\004\101\004\077\004\070\004\101\004\076\004\07211\006\101\006\111"
"11w\001\10211\011\01011\006\064\006\05711\004\102\004\060\004\07411\004\075\004\060\004\100\004\076\004\064\004\07011mw11\006\047\006\104\006\045\006\105\006\047\006\10511td11\006\047\006\104\006\071\006\047\006\105"
"11\004\072\004\076\004\063\004\060\004\102\004\07611yst11\004\102\004\076\004\067\004\07011\004\102\004\11311\004\065\004\064\004\070\004\07511\006\05111\005\351\005\334\005\333\005\33511\005\350\005\321\005\324"
"11\005\347\005\320\005\350\005\325\005\340\005\32411\005\326\005\320\005\35211\005\320\005\331\005\331\005\33511wrth11lgt11jsm11\006\047\006\104\006\064\006\056\006\065\006\11211\006\105\006\11211\004\101\004\073\004\065\004\06411\006\11111ldt"
"11\004\062\004\101\004\070\004\107\004\072\004\07011\006\314\006\25111\006\107\006\11211\004\126\004\101\004\102\004\076\004\100\004\126\004\11711\004\117\004\075\004\101\004\114\004\072\004\070\004\10511\004\117\004\072\004\070\004\105"
"11\004\103\004\072\004\100\004\060\004\127\004\075\004\12611\004\100\004\076\004\101\004\126\004\12711\004\074\004\126\004\06611\006\061\006\105\006\066\006\047\006\10611\006\110\006\102\006\05711\011\06711\013\237\013\262"
"11\006\101\006\112\006\107\006\04711\001\174n11hw10\005\351\005\334\005\340\005\32510\005\347\005\331\005\351\005\325\005\350\005\331\005\33510\005\342\005\325\005\32310\005\330\005\325\005\330\005\32510\005\324\005\331\005\325\005\335"
"10\005\320\005\327\005\350\005\331\005\33510\005\344\005\33110\001\22210\006\042\006\106\006\107\006\04710\004\103\004\066\004\06510rnm10\004\100\004\060\004\061\004\076\004\102\004\06010\006\071\006\050\006\05710\004\072\004\102\004\076"
"10\003\255\003\307\003\265\003\27110rtn10\006\110\006\063\006\104\006\10510szn10\006\047\006\104\006\105\006\061\006\043\006\05110dsh10\006\064\006\110\006\05710\004\100\004\103\004\101\004\101\004\072\004\070\004\07110\004\075\004\060\004\064"
"10\004\074\004\065\004\075\004\11710\013\207\013\23210\006\102\006\057\006\112\006\10510gf10\011\054\011\07110\006\107\006\06110\003\265\003\273\003\273\003\267\003\275\003\271\003\272\003\25410cv10\014\257\014\25010\014\254\014\263\014\225"
"10\014\234\014\25010\014\23410\004\101\004\112\004\07410\014\21010\005\351\005\336\005\325\005\350\005\325\005\35210\005\35110\005\346\005\325\005\35010\005\342\005\350\005\331\005\333\005\32410\005\342\005\321\005\325\005\350"
"10\005\340\005\331\005\352\005\33710\005\324\005\336\005\323\005\350\005\331\005\33210\005\324\005\326\005\333\005\325\005\331\005\325\005\35210\005\321\005\32510ngh10ddr10mk10\006\107\006\106\006\047\006\10310\006\105\006\106\006\052\006\057\006\111"
"10\006\101\006\102\006\06710\001\015j10rkk10\004\07710\013\265\013\26010\004\077\004\100\004\065\004\06710\003\303\003\277\003\30510\013\225\013\25210\006\054\006\057\006\112\006\05710szk10\006\043\006\106\006\10710t\001\13110cq"
"10\003\303\003\304\003\271\003\30210\006\05610\004\061\004\065\004\110\004\06510rkn10\005\34210\005\336\005\352\005\325\005\33210\005\336\005\33710\005\334\005\347\005\325\005\327\005\325\005\35210\005\324\005\326\005\32410\005\321"
"10\005\320\005\321\005\33410wt10bw10\006\112\006\110\006\10510dcz10\006\047\006\104\006\105\006\110\006\102\006\07110\004\075\004\117\004\074\004\06010lfs10\004\063\004\100\004\103\004\077\004\07010\006\071\006\061\006\06610\013\216\013\256"
"10\011\060\011\04410nkl10\013\23410\006\047\006\104\006\106\006\047\006\06310\001\176d10\004\065\004\111\004\06510ncs10\004\074\004\076\004\066\004\065\004\10210kc10\004\062\004\100\004\065\004\074\004\11710tf10lbst10jf"
"10\006\112\006\103\006\110\006\10610\006\055\006\112\006\05310yf10ghl10\006\047\006\104\006\105\006\104\006\10110\005\351\005\320\005\352\005\32410\005\334\005\325\005\33010\005\331\005\351\005\350\005\320\005\334\005\331"
"10\005\325\005\342\005\325\005\32310\005\325\005\334\005\32010\005\324\005\331\005\340\005\32510\005\324\005\325\005\341\005\34310\005\324\005\320\005\350\005\34510\005\321\005\336\005\325\005\326\005\331\005\320\005\325\005\337"
"10\005\321\005\320\005\350\005\34510\005\320\005\327\005\35010\005\320\005\325\005\352\005\32510\005\320\005\323\005\33510schw10\004\102\004\100\004\117\004\061\004\062\004\06010s\35410cm10\006\052\006\061\006\054\006\105\006\051"
"10\004\102\004\103\004\10210g\001\10210\006\110\006\105\006\10610\004\12710\001\133w10jsc10\006\107\006\10410\004\077\004\076\004\06410\004\061\004\113\004\07310\004\101\004\065\004\061\004\06510\004\072\004\060\004\072\004\062\004\076"
"10\004\061\004\076\004\06310\006\105\006\110\006\102\006\07110\003\272\003\27110ffl10\004\126\004\075\004\110\004\070\004\074\004\07010\004\117\004\072\004\076\004\074\004\10310\004\110\004\102\004\060\004\10210\004\107\004\070"
"10\004\106\004\065\004\07110\004\103\004\074\004\076\004\062\004\07010\004\101\004\102\004\076\004\100\004\126\004\075\004\072\004\07010\004\100\004\065\004\064\004\060\004\072\004\106\004\126\004\116"
"10\004\077\004\065\004\100\004\065\004\071\004\102\004\07010\004\077\004\065\004\100\004\065\004\064\004\060\004\107\004\12610\004\077\004\060\004\062\004\073\004\06010\004\076\004\102\004\100\004\070\004\074\004\060\004\075\004\076"
"10\004\076\004\101\004\102\004\100\004\126\004\06210\004\075\004\060\004\077\004\070\004\110\004\07010\004\074\004\076\004\062\004\060\004\074\004\07010\004\074\004\060\004\102\004\065\004\100\004\126\004\060\004\07310\004\071\004\076\004\063\004\076"
"10\004\067\004\076\004\072\004\100\004\065\004\074\004\06010\004\067\004\075\004\060\004\107\004\075\004\07610\004\067\004\063\004\076\004\064\004\076\004\07410\004\063\004\076\004\073\004\076\004\062\004\075\004\06010\004\060\004\073\004\065"
"10\011\071\011\05610\004\077\004\065\004\100\004\065\004\06410\003\263\003\271\003\261\003\304\003\25710gv10schl10fg10\014\270\014\27110\014\252\014\24110\014\246\014\26010\014\244\014\24610\014\205\014\25010\014\20510\006\063\006\103\006\063"
"10\006\052\006\071\006\047\006\104\006\11110llg10\006\071\006\047\006\10510\005\346\005\325\005\325\005\35210\005\342\005\336\005\325\005\32310\005\342\005\325\005\346\005\336\005\35210\005\334\005\344\005\340\005\331"
"10\005\334\005\340\005\331\005\324\005\325\005\33410\005\333\005\320\005\351\005\35010\005\325\005\320\005\35210\005\324\005\350\005\333\005\325\005\35210\005\324\005\320\005\33510\005\323\005\321\005\350\005\331\005\335"
"10\005\321\005\346\005\325\005\350\005\32410\005\320\005\350\005\346\005\325\005\35210\005\32010mkr10jkh10\006\055\006\047\006\104\006\112\006\04710\004\107\004\073\004\065\004\07510\006\04510\004\076\004\111\004\06510\003\304\003\27110mc10lds"
"10\006\043\006\103\006\053\006\06110\004\065\004\06510rwsz10\013\207\013\25010\006\064\006\105\006\04710nnl10\004\075\004\070\004\10510ndh10\357\001\222\35610zk10\004\067\004\060\004\111\004\076\004\102\004\07610\356nc0"};

unsigned char Consonants004[] = {
"10\006\050\006\047\006\064\006\05710\005\342\005\346\005\336\005\320\005\33110\005\341\005\344\005\35010\005\334\005\320\005\327\005\35010\005\331\005\324\005\331\005\32410\005\330\005\325\005\32110\005\327\005\323\005\351\005\331\005\33510\005\327"
"10\005\324\005\342\005\341\005\34710\005\32310\005\321\005\340\005\331\005\331\005\35210\005\321\005\336\005\323\005\321\005\35010\005\320\005\327\005\341\005\325\005\33710\004\10710dc10\006\064\006\047\006\05210\004\102\004\060\004\067\004\070"
"10\004\076\004\107\004\065\004\075\004\11410\004\062\004\07610\003\314\003\300\003\311\003\30210lfd10fw10bscr10\006\105\006\106\006\052\006\057\006\112\006\047\006\05210\004\077\004\076\004\101\004\073\004\06510vc10\006\047\006\105\006\047"
"10\004\075\004\065\004\10210gdj10brz10\003\314\003\304\003\261\003\27510\003\30310\006\06410\004\076\004\06110crkv10\006\103\006\047\006\106\006\05210\006\071\006\066\006\11010\004\061\004\103\004\064\004\065\004\10210kb10hts"
"10\006\106\006\070\006\06110\004\06110lks10jst10\006\047\006\104\006\065\006\101\006\055\006\05110src10\006\05510\006\050\006\107\006\04710zj10cf10\005\351\005\340\005\331\005\33510\005\351\005\340\005\32410\005\351\005\331\005\350\005\325\005\352"
"10\005\350\005\32110\005\342\005\333\005\351\005\331\005\32510\005\341\005\352\005\35010\005\341\005\350\005\33010\005\336\005\351\005\327\005\34710\005\336\005\320\005\344\005\351\005\350\005\35210\005\336"
"10\005\334\005\336\005\325\005\342\005\323\005\344\005\331\005\33510\005\334\005\327\005\321\005\35010\005\334\005\324\005\346\005\334\005\331\005\32710\005\334\005\32110\005\331\005\333\005\325\005\33410\005\327\005\331\005\344\005\325\005\351"
"10\005\324\005\350\005\331\005\351\005\325\005\33510\005\324\005\350\005\320\005\351\005\325\005\340\005\32410\005\324\005\320\005\336\005\35210\005\321\005\346\005\344\005\325\005\33710\005\320\005\331\005\351\005\331\005\352"
"10\004\065\004\064\004\075\004\06010\004\064\004\06510\004\062\004\076\004\10210\006\043\006\055\006\105\006\05710\004\100\004\103\004\101\004\101\004\072\004\076\004\063\004\07610\006\057\006\047\006\061\006\057"
"10\006\105\006\110\006\066\006\110\006\07110\004\077\004\100\004\076\004\106\004\065\004\10110\003\261\003\305\003\304\003\25610vrh10\011\005\011\052\011\05010\004\072\004\076\004\117\004\102\004\07610\001\10410zc10\006\065\006\110\006\061"
"10\004\074\004\06510\006\107\006\105\006\10710\004\064\004\060\004\066\004\06510\011\03510\006\071\006\106\006\10710\006\047\006\104\006\063\006\104\006\047\006\10510\004\063\004\07010\006\105\006\052\006\065\006\10410sms10skl10rsh10\014\270\014\262"
"10\014\260\014\26010\014\252\014\26010\014\227\014\263\014\25010\005\351\005\33510\005\347\005\330\005\322\005\325\005\350\005\331\005\32410\005\344\005\350\005\330\005\331\005\331\005\33510\005\341\005\344\005\350\005\331"
"10\005\336\005\341\005\344\005\35010\005\336\005\340\005\35210\005\336\005\320\005\325\005\32310\005\326\005\336\005\33710\005\324\005\336\005\347\005\350\005\32010\005\321\005\32410\005\320\005\33110\005\320\005\325\005\323\005\325\005\352"
"10\004\102\004\076\004\063\004\07610yks10\006\105\006\055\006\10410cch10\013\257\013\26010\013\225\013\26010\006\047\006\31410\004\062\004\060\004\10110smrt10nhv10\006\110\006\070\006\047\006\046\006\10110\004\062\004\100\004\065\004\074\004\06510klj"
"10blj10\004\074\004\065\004\066\004\064\004\10310\004\107\004\065\004\07410\004\102\004\065\004\067\004\07010\004\101\004\065\004\061\004\11710ctl10\001\174d10\003\300\003\277\003\273\003\31510xml10\004\102\004\11710\006\063\006\047\006\10410rff"
"10\011\04710\006\071\006\105\006\10410\004\110\004\102\004\060\004\102\004\10310\004\104\004\060\004\071\004\07310\004\103\004\072\004\100\004\060\004\127\004\075\004\106\004\12610\004\103\004\072\004\100\004\060\004\127\004\075\004\070"
"10\004\101\004\100\004\101\004\10010\004\101\004\062\004\126\004\102\004\12610\004\100\004\126\004\07210\004\100\004\103\004\101\004\07010\004\100\004\060\004\067\004\076\004\07410\004\077\004\126\004\067\004\075\004\126\004\110\004\065"
"10\004\077\004\126\004\062\004\075\004\126\004\107\004\075\004\07610\004\077\004\076\004\107\004\060\004\102\004\072\004\10310\004\077\004\076\004\102\004\076\004\107\004\075\004\12610\004\077\004\076\004\100\004\102\004\060\004\073"
"10\004\077\004\076\004\073\004\126\004\102\004\070\004\072\004\06010\004\077\004\076\004\07310\004\077\004\076\004\064\004\126\004\12710\004\077\004\060\004\075\004\065\004\073\004\11410\004\076\004\101\004\076\004\061\004\070\004\101\004\102\004\126"
"10\004\075\004\060\004\067\004\062\004\06010\004\074\004\126\004\101\004\102\004\07610\004\074\004\076\004\062\004\07010\004\072\004\075\004\060\004\071\004\077\004\06010\004\067\004\063\004\126\004\064\004\075\004\07610\004\067\004\060\004\105"
"10\004\064\004\076\004\062\004\126\004\064\004\072\004\06010\004\064\004\065\004\100\004\066\004\060\004\062\004\06010\004\062\004\105\004\126\004\06410\004\061\004\103\004\073\004\06010\001\141tv10mv10\006\064\006\107\006\06110vk10pst10vrl10pf"
"10\004\075\004\065\004\111\004\07610\005\351\005\334\005\32010\005\351\005\331\005\336\005\325\005\35110\005\350\005\321\005\331\005\33510\005\336\005\347\005\325\005\33510\005\334\005\342\005\351\005\325\005\352"
"10\005\324\005\330\005\331\005\344\005\325\005\33410\005\324\005\321\005\350\005\331\005\35210\005\321\005\331\005\351\005\350\005\320\005\33410\005\320\005\327\005\35210\004\075\004\060\004\100\004\076\004\06410cz\001\10210\013\231"
"10\004\101\004\112\004\10110rns10\006\054\006\105\006\112\006\07110\006\050\006\061\006\047\006\105\006\05410\006\047\006\05210\001\133\001\00710tsl10\006\251\006\106\006\05710\004\072\004\076\004\102\004\076\004\100\004\113\004\06510rspr10ddh"
"10\013\244\013\26210\006\047\006\104\006\045\006\063\006\104\006\047\006\10510\004\072\004\060\004\072\004\102\004\07610\004\062\004\101\004\070\004\107\004\072\004\07610\003\311\003\30210\011\006\011\052\011\025"
"10\004\107\004\065\004\073\004\076\004\062\004\065\004\07210vv10\37710\006\105\006\061\006\057\006\10510\006\047\006\104\006\060\006\107\006\047\006\05010\004\065\004\074\004\10310\006\110\006\054\006\110\006\05710stm10skt"
"10\006\105\006\047\006\104\006\10310\003\300\003\301\003\255\003\300\003\265\003\27110\006\047\006\257\006\06110jsk10\006\206\006\10710\003\27210pdr10nsh10wp10gst10\006\106\006\064\006\06710\006\047\006\104\006\042\006\106"
"10\005\352\005\325\005\333\005\340\005\331\005\35210\005\336\005\323\005\325\005\34210\005\334\005\324\005\33510\005\334\005\320\005\336\005\340\005\325\005\35210\005\327\005\331\005\344\005\32410\005\325\005\325\005\321\005\341\005\330\005\350"
"10\005\321\005\344\005\340\005\33110\005\321\005\342\005\347\005\321\005\325\005\35210\005\321\005\342\005\341\005\34710\005\321\005\342\005\33410\005\321\005\320\005\336\005\35210\005\320\005\32610rgr10nskj10ndn10ngst"
"10\004\074\004\076\004\066\004\075\004\07610jsz10ngd10\003\314\003\274\003\311\003\30210jw10\013\205\013\265\013\26010\006\047\006\314\006\061\006\047\006\10610\004\115\004\102\004\076\004\063\004\07610\004\062\004\065\004\107\004\065"
"10\006\050\006\047\006\102\006\052\006\050\006\047\006\06310wch10rbr10\013\225\013\24410\006\110\006\101\006\11210jg10\006\050\006\10410\004\063\004\064\004\06510\004\102\004\100\004\07010gdz10\013\263\013\26010\006\110\006\105\006\047"
"10\006\106\006\052\006\047\006\046\006\05410vst10rll10\006\314\006\04710\006\104\006\112\006\06310nstv10drw10\014\260\014\24410\014\257\014\26210\014\254\014\22710\014\24510\014\22610\006\112\006\105\006\103\006\10610\357\001\222\357"
"10\005\352\005\334\005\325\005\340\005\32410\005\352\005\327\005\350\005\325\005\35210\005\352\005\325\005\33210\005\352\005\322\005\325\005\321\005\32410\005\351\005\331\005\325\005\325\005\34710\005\350\005\320\005\351\005\331"
"10\005\346\005\350\005\331\005\33210\005\342\005\331\005\346\005\325\005\32110\005\334\005\327\005\331\005\331\005\33510\005\327\005\351\005\325\005\32110\005\325\005\336\005\352\005\33710\005\324\005\333\005\334"
"10\005\324\005\327\005\321\005\350\005\32410\005\323\005\321\005\35010\005\321\005\340\005\331\005\324\005\325\005\33410\005\320\005\340\005\327\005\340\005\32510\005\320\005\334\005\33110\005\320\005\334\005\32410\005\320\005\33210lz10jc10bbl"
"10\013\237\013\26010\003\272\003\261\003\304\003\25410rtm10\006\047\006\104\006\105\006\110\006\047\006\066\006\112\006\07110\004\074\004\103\004\067\004\070\004\072\004\06010ksz10\011\007\011\07010\006\105\006\065\006\061"
"10\006\052\006\110\006\070\006\112\006\10110yhd10\004\101\004\060\004\071\004\10210jkt10\006\101\006\045\006\10610k\001\17410d\001\10210\006\102\006\063\006\10510fs10\006\251\006\061\006\05710\006\105\006\110\006\047\006\102\006\071"
"10\006\047\006\104\006\105\006\062\006\112\006\05710\004\115\004\102\004\076\004\07410stj10\011\025\011\07110\006\057\006\11010\006\047\006\104\006\105\006\110\006\066\006\110\006\07110\004\10410css10\003\314\003\273\003\261"
"10\003\272\003\254\003\270\003\26510\003\300\003\311\003\30210t\001\10210ntst10\006\047\006\104\006\064\006\112\006\05610bg10\006\047\006\104\006\063\006\071\006\110\006\057\006\112\006\05110\006\043\006\050\006\11210\006\104\006\103\006\106"
"10\006\052\006\055\006\061\006\112\006\06110\003\274\003\265\003\304\003\25410vz10\006\071\006\057\006\05710\005\351\005\334\005\32510\005\346\005\331\005\334\005\325\005\336\005\331\005\33510\005\344\005\351\005\325\005\330"
"10\005\336\005\340\005\324\005\33410\005\336\005\33110\005\336\005\325\005\33410\005\334\005\331\005\334\005\323\005\331\005\33510\005\333\005\33710\005\333\005\325\005\334\005\33410\005\331\005\351\005\325\005\342"
"10\005\327\005\325\005\347\005\350\005\331\005\33510\005\326\005\331\005\325\005\340\005\331\005\33510\005\324\005\350\005\331\005\331\005\347\005\33110\005\324\005\324\005\346\005\334\005\327\005\32410\005\324\005\323\005\350\005\332"
"10\005\321\005\352\005\327\005\325\005\33510\005\320\005\350\005\34510\005\320\005\331\005\340\005\32510\005\320\005\325\005\352\005\32410\003\304\003\316\003\301\003\26110nhw10\006\105\006\106\006\107\006\04710s\356nt10\006\176\006\06310\001\1330"};

unsigned char Consonants005[] = {
"10\006\31410\006\057\006\104\006\112\006\10410\004\115\004\102\004\076\004\10210\004\064\004\076\004\061\004\100\004\06510pk10nsf10chst10\013\207\013\26010\004\107\004\076\004\062\004\065\004\07210\004\06410\003\274\003\255\003\303\003\26110ssw10rpl"
"10\006\047\006\104\006\06110rsl10\006\103\006\052\006\047\006\05010\006\047\006\104\006\106\006\050\006\11210wsp10\006\047\006\104\006\105\006\064\006\047\006\061\006\103\006\05110\004\102\004\117\004\10510\004\102\004\076\004\07410vrst10bv"
"10\004\062\004\112\004\06210\001\176j10tvrd10jr10\004\061\004\112\004\064\004\06510\001\12310\353l10r\001\01510\006\112\006\052\006\10510\004\077\004\076\004\102\004\076\004\074\004\10310\003\274\003\314\003\275\003\27710vdj10ggj"
"10\006\110\006\107\006\11210\006\104\006\11010\006\104\006\10310\006\102\006\047\006\046\006\105\006\05110\006\047\006\104\006\105\006\110\006\047\006\102\006\07110\006\047\006\101\006\052\006\061\006\047\006\066\006\11210\011\26010\004\127\004\127"
"10\004\126\004\075\004\110\004\070\004\10510\004\126\004\074\004\077\004\065\004\100\004\126\004\12710\004\126\004\062\004\060\004\075\004\07610\004\126\004\062\004\060\004\075\004\06010\004\124\004\062\004\100\004\076\004\077\004\126"
"10\004\117\004\072\004\111\004\07610\004\117\004\067\004\072\004\10310\004\117\004\067\004\060\004\075\004\12610\004\117\004\062\004\070\004\073\004\060\004\101\004\11410\004\116\004\10110\004\110\004\102\004\060\004\102\004\126\004\062"
"10\004\110\004\102\004\060\004\102\004\07010\004\107\004\060\004\101\004\076\004\07410\004\106\004\12610\004\106\004\070\004\10510\004\106\004\065\004\100\004\072\004\062\004\06010\004\102\004\100\004\070\004\062\004\060\004\124"
"10\004\101\004\126\004\100\004\060\004\07210\004\101\004\114\004\076\004\063\004\076\004\064\004\075\004\12610\004\101\004\105\004\126\004\064\004\075\004\070\004\10510\004\101\004\102\004\076\004\073\004\070\004\106\004\117"
"10\004\101\004\075\004\064\004\07410\004\101\004\065\004\10010\004\101\004\062\004\117\004\102\004\06010\004\101\004\060\004\071\004\102\004\10310\004\100\004\103\004\101\004\12610\004\100\004\103\004\101\004\114"
"10\004\100\004\103\004\101\004\101\004\072\004\070\004\06510\004\077\004\126\004\062\004\07510\004\077\004\126\004\062\004\06410\004\077\004\076\004\110\004\103\004\072\004\10310\004\077\004\076\004\110\004\103\004\072\004\076\004\062\004\126"
"10\004\077\004\076\004\102\004\100\004\065\004\06110\004\077\004\076\004\101\004\102\004\126\004\071\004\075\004\06510\004\077\004\076\004\101\004\070\004\073\004\060\004\075\004\11410\004\077\004\065\004\100\004\065\004\064\004\060\004\107"
"10\004\077\004\060\004\077\004\076\004\11610\004\077\004\060\004\077\004\07010\004\076\004\101\004\102\004\100\004\076\004\062\004\07010\004\076\004\073\004\061\004\060\004\075\004\12610\004\076\004\061\004\073\004\060\004\101\004\102\004\114"
"10\004\075\004\060\004\100\004\076\004\064\004\10310\004\074\004\126\004\110\004\060\004\075\004\070\004\10510\004\074\004\076\004\101\004\07210\004\074\004\076\004\073\004\076\004\064\004\12610\004\074\004\076\004\066\004\075\004\060"
"10\004\072\004\126\004\073\004\114\004\072\004\06010\004\072\004\076\004\07310\004\072\004\074\004\12610\004\067\004\06210\004\067\004\060\004\063\004\060\004\073\004\114\004\075\004\060"
"10\004\066\004\076\004\062\004\102\004\075\004\065\004\062\004\06010\004\065\004\102\004\075\004\126\004\107\004\075\004\10310\004\065\004\102\004\075\004\126\004\107\004\075\004\076\004\12710\004\064\004\100\004\103\004\072\004\103"
"10\004\063\004\100\004\065\004\072\004\07610\004\062\004\076\004\075\004\07010\004\062\004\076\004\075\004\06010\004\061\004\103\004\073\004\07610\004\061\004\07310\004\060\004\075\004\063\004\065\004\07310\004\060\004\067\004\126\004\127"
"10\003\300\003\301\003\277\003\30210wsl10\006\047\006\104\006\055\006\05010\006\104\006\107\006\04710\006\103\006\112\006\10110\006\052\006\047\006\061\006\112\006\05610\006\043\006\055\006\05710\003\314\003\307\003\27110\001\176b"
"10\006\055\006\102\006\110\006\10210\005\351\005\342\005\325\005\35210\005\350\005\351\005\331\005\336\005\325\005\35210\005\336\005\352\005\340\005\32410\005\334\005\346\005\344\005\331\005\331\005\32410\005\333\005\334\005\334\005\331"
"10\005\324\005\342\005\33510\005\324\005\342\005\325\005\334\005\33510\005\324\005\326\005\336\005\33710\005\324\005\321\005\320\005\331\005\33510\005\323\005\350\005\33210\005\321\005\340\005\325\005\351\005\32010\005\321\005\334\005\321\005\323"
"10\005\320\005\334\005\32010\005\320\005\325\005\352\005\33510\337t10ssch10\011\006\011\05210\006\105\006\064\006\047\006\107\006\057\006\05110\006\043\006\10510ckb10\006\061\006\063\006\110\006\10410\006\047\006\104\006\052"
"10\004\074\004\076\004\101\004\072\004\062\004\06010\004\074\004\060\004\073\004\072\004\07610\003\26110nnw10\006\050\006\061\006\047\006\11210\004\075\004\070\004\07410\004\061\004\113\004\073\004\07010jft10\006\106\006\107"
"10\006\047\006\104\006\065\006\112\006\047\006\10510\006\055\006\047\006\10410\004\101\004\065\004\063\004\06010\004\077\004\076\004\062\004\065\004\107\004\06510stf10\006\045\006\071\006\104\006\047\006\106\006\047\006\05210\004\075\004\070\004\065"
"10zs10\013\244\013\25110rsp10\004\101\004\07610\006\052\006\104\006\10310\006\047\006\104\006\104\006\072\006\05110\004\066\004\070\004\067\004\075\004\07010\004\105\004\076\004\100\004\06010\006\047\006\106\006\057"
"10\006\047\006\104\006\071\006\104\006\047\006\05410\006\043\006\056\006\061\006\11110\004\072\004\076\004\102\004\076\004\100\004\113\004\07110\003\307\003\311\003\301\003\257\003\30210\003\274\003\300\003\277\003\301\003\265\003\257"
"10\006\105\006\106\006\10710\006\056\006\104\006\047\006\10410\006\043\006\105\006\04710\006\112\006\10310\006\105\006\110\006\061\006\05710fst10dvs10\014\260\014\23410\014\260\014\22510\014\257\014\265\014\25010\014\257\014\24410\014\254\014\262"
"10\014\250\014\22510\014\225\014\260\014\24310\006\102\006\061\006\047\006\06110\006\101\006\047\006\061\006\063\006\31410\356tr10rgm10nsv10\006\047\006\104\006\102\006\061\006\042\006\10610\005\352\005\322\005\325\005\321\005\325\005\352"
"10\005\351\005\331\005\35110\005\351\005\324\005\325\005\32010\005\350\005\325\005\346\005\32410\005\344\005\350\005\330\005\33110\005\344\005\342\005\33510\005\342\005\346\005\336\005\33110\005\341\005\344\005\325\005\350\005\330"
"10\005\340\005\325\005\344\005\35110\005\334\005\344\005\350\005\330\005\331\005\33510\005\333\005\325\005\351\005\35010\005\333\005\325\005\32710\005\33310\005\33210\005\331\005\342\005\331\005\33410\005\327\005\340\005\337"
"10\005\327\005\323\005\351\005\32410\005\327\005\321\005\350\005\325\005\35210\005\324\005\352\005\336\005\325\005\340\005\32410\005\324\005\351\005\334\005\33510\005\324\005\325\005\334\005\323\005\35210\005\322\005\33710\005\321\005\337"
"10\005\321\005\322\005\334\005\331\005\33410\005\321\005\320\005\331\005\340\005\330\005\350\005\340\005\33010\005\320\005\331\005\340\005\330\005\350\005\340\005\33010\005\320\005\331\005\340\005\324"
"10\004\061\004\112\004\073\004\063\004\060\004\100\004\070\004\11710bc10\004\065\004\064\004\075\004\07610\003\303\003\304\003\277\003\305\003\30210\001\141nj10\001\141j10sgr10\006\101\006\102\006\05710\006\047\006\104\006\071\006\110\006\066\006\112"
"10\006\047\006\104\006\071\006\061\006\05010vpr10\006\061\006\110\006\06210\006\057\006\063\006\05210\006\055\006\110\006\10410\006\045\006\104\006\112\006\10710\006\043\006\112\006\066\006\04710rsq10ntt10\006\047\006\104\006\104\006\107\006\105"
"10\006\043\006\106\006\04710\004\074\004\076\004\074\004\065\004\075\004\10210\013\27010\004\102\004\065\004\07410rdw10\006\251\006\061\006\057\006\10710\006\105\006\11110\004\061\004\076\004\073\004\065\004\06510\356l10\006\061\006\102\006\105"
"10\004\074\004\065\004\07510\003\255\003\307\003\277\003\305\003\27510chz10\004\104\004\076\004\100\004\103\004\07410pw10pcs10msk10j\34010ddl10\005\342\005\321\005\350\005\331\005\35210r\34710\006\104\006\11210\006\050\006\055\006\053"
"10\006\043\006\106\006\05210\004\064\004\100\004\103\004\063\004\07010\001\01710\006\071\006\105\006\06110\004\063\004\076\004\064\004\070\004\075\004\07010\004\061\004\113\004\073\004\06010\003\300\003\271\003\27710\003\26510\003\265\003\264\003\316"
"10z\001\10210wrn10ttn10lsh10\006\071\006\057\006\10510\006\056\006\057\006\04710\006\047\006\10510\004\077\004\100\004\060\004\062\004\07610\013\265\013\24410\006\257\006\101\006\05210\006\106\006\105\006\31410\006\052\006\10510nths10l\001\141"
"10\013\225\013\26510\006\047\006\104\006\050\006\055\006\05310\004\102\004\060\004\072\004\070\004\07410vg10rsj10nfr10df9\011\025\011\060\011\0509\011\020\011\0709\005\351\005\320\005\340\005\3319\005\350\005\325\005\346\005\331\005\3359\005\350"
"9\005\347\005\324\005\3349\005\346\005\331\005\330\005\325\005\3309\005\336\005\344\005\3529\005\334\005\3259\005\334\005\324\005\352\005\336\005\325\005\323\005\3239\005\334\005\324\005\325\005\323\005\325\005\352"
"9\005\331\005\351\005\350\005\320\005\334\005\331\005\3359\005\330\005\325\005\344\005\3419\005\327\005\321\005\350\005\3529\005\322\005\350\005\344\005\331\005\3529\005\322\005\334\005\350\005\331\005\324"
"9\005\321\005\350\005\346\005\325\005\340\005\3319\005\321\005\342\005\321\005\350\005\331\005\3529\005\321\005\336\005\341\005\322\005\350\005\3529\005\320\005\3439\005\320\005\334\005\331\005\340\005\3259\005\320\005\326\005\3349\004\061\004\070"
"9svj9\006\110\006\104\006\103\006\1069\006\047\006\104\006\071\006\061\006\050\006\1129\004\102\004\103\004\0729ymdd9nll9\006\071\006\047\006\104\006\1059\006\047\006\104\006\061\006\046\006\112\006\063\006\112\006\0519\004\075\004\060\004\0749\340s"
"9llw9ckt9\013\252\013\2619\011\0069\006\110\006\103\006\047\006\1069\006\047\006\104\006\054\006\050\006\1069\004\100\004\103\004\101\004\101\004\072\004\076\004\074\004\1039\004\100\004\060\004\0679\004\064\004\100\004\103\004\0639rps9nsc"
"9\006\052\006\103\006\110\006\1069zpr9chk9\013\225\013\2649\003\265\003\257\003\307\003\2659\001\037r9scl9\011\025\011\060\011\0449\006\104\006\043\006\1069\006\047\006\104\006\056\006\104\006\047\006\112\006\0470"};

unsigned char Consonants006[] = {
"9\004\102\004\065\004\072\004\101\004\1029\004\077\004\100\004\065\004\064\004\0709ntv9jp9dstv9\013\261\013\2449\004\102\004\076\004\1029\004\102\004\076\004\063\004\064\004\0609s\3719\011\062\011\0279\006\110\006\104\006\105"
"9\004\061\004\112\004\073\004\063\004\060\004\100\004\070\004\0759\004\076\004\064\004\070\004\0759\004\062\004\101\004\065\004\072\004\0709\376r9l\001\1439\006\047\006\104\006\1049\006\071\006\104\006\112\006\107\006\0479\006\052\006\110"
"9\006\047\006\104\006\071\006\047\006\104\006\1059wdz9\356ns9\006\071\006\106\006\110\006\047\006\1069\004\062\004\101\004\065\004\1059\004\061\004\0659xv9\011\054\011\0509\006\112\006\102\006\110\006\1049\006\070\006\171\006\067"
"9\006\056\006\047\006\0659fh9\013\257\013\2569\011\011\011\0509\004\073\004\065\004\1029\003\303\003\265\003\273\003\257\003\264\003\2619ppd9ddw9\006\047\006\104\006\060\006\112\006\1069\005\352\005\334\005\336\005\331\005\3239\005\352\005\334"
"9\005\351\005\334\005\324\005\3359\005\351\005\331\005\350\005\325\005\352\005\331\005\3359\005\350\005\331\005\351\005\325\005\3359\005\350\005\322\005\331\005\3349\005\347\005\331\005\3459\005\346\005\331\005\325\005\323"
"9\005\342\005\346\005\336\005\3259\005\342\005\334\005\331\005\3329\005\342\005\326\005\350\005\3249\005\342\005\325\005\351\005\3249\005\342\005\321\005\325\005\323\005\3249\005\341\005\325\005\3439\005\340\005\341\005\325"
"9\005\340\005\331\005\341\005\331\005\325\005\3379\005\340\005\325\005\341\005\3439\005\336\005\351\005\324\005\3259\005\336\005\346\005\350\005\331\005\3359\005\336\005\336\005\3519\005\336\005\333\005\3349\005\334\005\342\005\325\005\334\005\335"
"9\005\334\005\324\005\351\005\352\005\336\005\3519\005\334\005\324\005\351\005\331\005\3229\005\334\005\320\005\350\005\3459\005\333\005\340\005\331\005\341\005\3249\005\333\005\321\005\3509\005\331\005\351\005\331\005\350\005\325\005\352"
"9\005\331\005\342\005\331\005\334\005\325\005\3529\005\327\005\323\005\351\005\325\005\3529\005\326\005\350\005\331\005\326\005\325\005\3529\005\325\005\322\005\3359\005\325\005\320\005\340\005\331"
"9\005\324\005\352\005\336\005\325\005\340\005\325\005\3529\005\324\005\347\005\324\005\331\005\334\005\3249\005\324\005\331\005\340\005\3249\005\324\005\320\005\326\005\340\005\3529\005\323\005\344\005\3319\005\321\005\342\005\334\005\331"
"9\005\321\005\341\005\344\005\3509\005\321\005\340\005\3319\005\321\005\326\005\3479\005\321\005\326\005\336\005\3379\005\321\005\324\005\3359\005\321\005\323\005\350\005\3329\005\320\005\331\005\340\005\3359\005\320\005\321\005\350\005\324\005\335"
"9\005\320\005\321\005\331\005\3219nnh9\006\251\006\106\006\106\006\0579\006\062\006\047\006\046\006\0619\006\050\006\061\006\106\006\047\006\105\006\0549\006\047\006\104\006\102\006\063\006\1059\004\075\004\076\004\062\004\070\004\075\004\070"
"9\003\261\003\305\003\304\003\2549\003\255\003\304\003\303\003\2719psk9\006\110\006\107\006\060\006\0479\006\064\006\112\006\0419\006\047\006\112\006\061\006\047\006\1069\006\045\006\066\006\047\006\101\006\0519\004\075\004\060\004\064\004\076"
"9\003\2749zh9rsd9\011\011\011\050\011\0259\011\005\011\0329\006\047\006\104\006\071\006\047\006\0509\011\052\011\071\011\0629\003\275\003\255\003\2619trw9sql9\013\222\013\2609\006\067\006\061\006\112\006\1029\006\047\006\063\006\1059stw9nbl9c\356nd"
"9\006\050\006\064\006\103\006\1049nscr9gb9\013\205\013\2519\006\043\006\107\006\1049\003\303\003\261\003\2759skk9rtj9\006\105\006\054\006\104\006\0519\004\101\004\112\004\111\004\0769\004\101\004\060\004\0749\004\062\004\101\004\065\004\063\004\076"
"9\001\104stw9\013\205\013\2449\006\056\006\110\006\047\006\107\006\0579\006\050\006\105\006\0479\006\050\006\057\006\110\006\1069\004\067\004\060\004\111\004\0769ljn9\006\107\006\063\006\052\006\106\006\0579\006\052\006\055\006\105\006\112\006\104"
"9\003\304\003\314\003\304\003\2659ftv9fb9\016\104\016\027\016\0429\006\057\006\314\006\257\006\0619\006\047\006\104\006\043\006\105\006\0619\004\075\004\060\004\100\004\076\004\064\004\0609\004\061\004\076\004\073\004\114\004\110\004\065"
"9\003\303\003\256\003\274\003\265\003\301\003\2619yb9rstv9lms9jpr9\006\103\006\106\006\0529\006\056\006\047\006\065\006\0519\004\127\004\105\004\075\004\126\004\1059\004\117\004\072\004\076\004\1279\004\117\004\072\004\0609\004\106\004\117"
"9\004\106\004\114\004\076\004\074\004\1039\004\106\004\065\004\100\004\0729\004\106\004\060\004\100\004\126\004\0629\004\105\004\102\004\0769\004\105\004\076\004\100\004\060\004\102\004\0609\004\104\004\126\004\075\004\101\004\114\004\072\004\126"
"9\004\103\004\101\004\114\004\076\004\074\004\1039\004\103\004\100\004\060\004\0739\004\102\004\126\004\073\004\114\004\072\004\0709\004\102\004\070\004\0749\004\102\004\065\004\077\004\065\004\100"
"9\004\101\004\102\004\076\004\101\004\076\004\062\004\075\004\0769\004\101\004\102\004\076\004\100\004\126\004\075\004\072\004\0609\004\101\004\100\004\077\004\101\004\072\004\0709\004\101\004\077\004\100\004\070\004\117\004\075\004\075\004\117"
"9\004\101\004\076\004\0629\004\101\004\070\004\061\004\126\004\100\004\1039\004\100\004\126\004\064\004\072\004\0769\004\100\004\076\004\101\004\126\004\1179\004\100\004\076\004\067\004\064\004\126\004\073\004\070"
"9\004\100\004\065\004\073\004\126\004\063\004\126\004\1179\004\077\004\126\004\062\004\075\004\126\004\107\004\075\004\0609\004\077\004\126\004\062\004\064\004\065\004\075\004\075\004\0609\004\077\004\100\004\076\004\065\004\072\004\102\004\103"
"9\004\077\004\100\004\070\004\071\004\075\004\117\004\102\004\0609\004\077\004\100\004\060\004\077\004\076\004\1009\004\077\004\076\004\107\004\060\004\073\004\076\004\101\004\1179\004\077\004\076\004\107"
"9\004\077\004\076\004\064\004\065\004\072\004\103\004\064\004\0709\004\077\004\0679\004\077\004\065\004\100\004\110\004\0609\004\077\004\065\004\100\004\065\004\077\004\070\004\101\004\1039\004\077\004\065\004\100\004\065\004\077\004\070\004\101"
"9\004\076\004\101\004\075\004\076\004\062\004\1269\004\075\004\126\004\0749\004\075\004\065\004\062\004\065\004\073\004\070\004\072\004\1269\004\075\004\060\004\110\004\1269\004\075\004\060\004\100\004\076\004\064\004\060\004\074\004\070"
"9\004\074\004\076\004\101\004\072\004\062\004\0709\004\074\004\076\004\062\004\076\004\1169\004\074\004\076\004\062\004\0609\004\074\004\060\004\116\004\102\004\1149\004\074\004\060\004\073\004\0609\004\073\004\126\004\075\004\072\004\070"
"9\004\072\004\076\004\074\004\0779\004\072\004\0749\004\067\004\075\004\060\004\107\004\065\004\075\004\075\004\1179\004\067\004\074\004\126\004\101\004\1029\004\067\004\074\004\126\004\075"
"9\004\067\004\060\004\101\004\065\004\073\004\070\004\073\004\0709\004\065\004\102\004\075\004\076\004\075\004\126\004\074\004\0709\004\064\004\103\004\066\004\072\004\060\004\1059\004\064\004\100\004\103\004\063\004\126\004\071"
"9\004\064\004\076\004\064\004\060\004\102\004\0709\004\064\004\065\004\100\004\0669\004\064\004\060\004\075\004\070\004\1059\004\063\004\103\004\064\004\067\004\076\004\0759\004\062\004\126\004\073\004\114\004\075\004\076\004\063\004\076"
"9\004\062\004\065\004\100\004\101\004\126\004\1179\004\062\004\0629\004\061\004\126\004\073\004\1179\004\061\004\126\004\073\004\076\004\100\004\103\004\101\004\0709\004\061\004\126\004\073\004\076\004\1009\004\061\004\103\004\073\004\070"
"9\004\061\004\103\004\064\004\070\004\075\004\076\004\0729\004\060\004\102\004\076\004\0739\004\060\004\101\004\101\004\1009\001\133l9pj9\013\232\013\2569\011\027\011\0579\006\061\006\1109\006\047\006\104\006\045\006\106\006\063\006\047\006\106"
"9\004\102\004\060\004\072\004\070\004\1059\004\070\004\074\004\065\004\075\004\075\004\0769\004\064\004\062\004\0609\004\062\004\060\004\0749rdn9\004\102\004\060\004\072\004\066\004\0659bbc9\014\271\014\2259\014\270\014\2569\014\262\014\2259\014\255"
"9\014\246\014\227\014\2639\014\244\014\260\014\2439\014\244\014\2609\014\241\014\2629\014\2409\014\216\014\2629\014\207\014\244\014\2609\014\205\014\2559\014\205\014\245\014\2659\006\105\006\104\006\1019\004\077\004\100\004\065\004\064"
"9\004\064\004\076\004\100\004\0709\003\274\003\257\003\2619\356mp9gk9\013\244\013\2449\006\105\006\065\006\067\006\101\006\1119\006\050\006\047\006\314\006\0579\005\351\005\352\005\3319\005\351\005\340\005\3319\005\351\005\334\005\325\005\335"
"9\005\351\005\331\005\327\005\3249\005\351\005\324\005\3359\005\351\005\320\005\334\005\325\005\3529\005\350\005\351\005\3529\005\346\005\331\005\325\005\350\005\331\005\3359\005\344\005\342\005\325\005\334\005\324"
"9\005\342\005\341\005\347\005\331\005\3359\005\340\005\325\005\351\005\3209\005\340\005\3209\005\336\005\342\005\350\005\325\005\3529\005\336\005\333\005\331\005\350\005\325\005\3529\005\336\005\323\005\324\005\331\005\335"
"9\005\334\005\342\005\326\005\325\005\3509\005\334\005\331\005\336\005\325\005\3239\005\334\005\324\005\321\005\331\005\3379\005\333\005\331\005\351\005\325\005\350\005\3319\005\331\005\342\005\326\005\325\005\3509\005\331\005\325\005\350\005\347"
"9\005\330\005\333\005\340\005\331\005\347\005\325\005\3529\005\327\005\331\005\340\005\3359\005\327\005\331\005\331\005\3219\005\327\005\325\005\347\005\3509\005\325\005\342\005\3349\005\324\005\351\005\334\005\331\005\327"
"9\005\324\005\350\005\320\005\351\005\325\005\3379\005\324\005\336\005\350\005\333\005\3269\005\324\005\327\005\334\005\330\005\325\005\3529\005\324\005\327\005\323\005\351\005\3249\005\323\005\340\005\3319\005\322\005\323\005\325\005\334"
"9\005\321\005\350\005\320\005\351\005\331\005\3529\005\321\005\342\005\325\005\334\005\3359\005\321\005\340\005\325\005\341\005\3439\005\321\005\320\005\3219\004\067\004\060\004\100\004\060\004\064\004\0709lln9\011\034\011\054"
"9\004\075\004\070\004\074\004\0709rkz9\013\211\013\2609\006\105\006\105\006\0479\006\105\006\054\006\105\006\110\006\071\006\0519\006\050\006\106\006\0529\006\047\006\063\006\052\006\056\006\057\006\047\006\105"
"9\004\102\004\076\004\073\004\072\004\076\004\062\004\0609\004\077\004\060\004\0729\004\072\004\060\004\072\004\076\004\0719\004\064\004\065\004\075\004\1149\003\264\003\315\003\2779prvn9llm9\006\057\006\110\006\1069\006\052\006\102\006\1049dll0"};

unsigned char Consonants007[] = {
"9\004\075\004\060\004\103\004\072\004\0609\006\251\006\047\006\0619\006\106\006\101\006\063\006\1079\006\047\006\104\006\110\006\054\006\1079wcz9kh9hrt9\011\044\011\0259\004\064\004\065\004\0759\004\101\004\102\004\065"
"9\004\101\004\102\004\060\004\073\004\0709\003\2639\011\052\011\0709\006\106\006\314\006\0629\006\101\006\104\006\0479\006\056\006\112\006\0619\003\3009zgl9\004\067\004\064\004\065\004\101\004\1149npr9llst9\006\061\006\047\006\107"
"9\006\055\006\061\006\1019llr9\011\057\011\0719\006\047\006\104\006\105\006\106\006\052\006\057\006\1119\004\107\004\065\004\073\004\076\004\062\004\065\004\072\004\0609slj9nnst9\011\071\011\0609\006\056\006\050\006\061"
"9\004\102\004\065\004\061\004\0659\004\101\004\065\004\071\004\107\004\060\004\1019\003\300\003\254\003\275\003\3119drz9\006\112\006\064\006\047\006\107\006\057\006\1079\004\077\004\100\004\060\004\0629\003\274\003\267\003\275"
"9\006\103\006\104\006\105\006\0519\006\064\006\050\006\103\006\0519\004\075\004\070\004\107\004\065\004\063\004\0769\004\0759\004\073\004\116\004\064\004\0709\003\277\003\300\003\277\003\257\003\2619\356nt9llb9gwr9\006\251\006\052\006\047\006\050"
"9\006\062\006\105\006\047\006\1069\004\115\004\102\004\0709\004\102\004\076\004\063\004\060\004\062\004\0609\006\104\006\105\006\0479\006\101\006\071\006\1049\006\061\006\066\006\1129\004\064\004\060\004\102\004\0609xcl9mz9lld"
"9\006\110\006\102\006\047\006\1049\006\105\006\054\006\104\006\0639\006\047\006\104\006\071\006\105\006\1049\003\255\003\275\003\261\003\3029\011\011\011\0709\006\055\006\1029\005\352\005\325\005\333\005\3349\005\351\005\320\005\352\005\335"
"9\005\350\005\342\005\331\005\325\005\340\005\325\005\3529\005\347\005\346\005\3529\005\342\005\346\005\336\005\331\005\3529\005\342\005\331\005\325\005\3379\005\342\005\323\005\331\005\331\005\3379\005\340\005\350\005\320\005\324"
"9\005\340\005\322\005\3239\005\336\005\350\005\333\005\3269\005\336\005\347\005\346\005\325\005\342\005\3319\005\336\005\342\005\3349\005\336\005\340\005\324\005\331\005\322\005\325\005\3529\005\336\005\334\005\320\005\3249\005\336\005\334\005\320"
"9\005\336\005\322\005\323\005\3349\005\336\005\320\005\3529\005\334\005\344\005\342\005\325\005\3349\005\334\005\344\005\3319\005\334\005\336\005\3249\005\334\005\324\005\351\005\322\005\3529\005\334\005\3249\005\334\005\322\005\321\005\331"
"9\005\334\005\321\005\340\005\3319\005\333\005\352\005\331\005\321\005\3249\005\333\005\323\005\350\005\3329\005\333\005\322\005\325\005\3379\005\331\005\333\005\325\005\334\005\331\005\3359\005\331\005\3259\005\324\005\350\005\321\005\324"
"9\005\324\005\342\005\341\005\347\005\331\005\3359\005\324\005\341\005\321\005\3509\005\324\005\336\005\350\005\322\005\334\005\331\005\3359\005\324\005\336\005\333\005\331\005\350\005\3249\005\324\005\331\005\325"
"9\005\324\005\330\005\325\005\321\005\331\005\3359\005\324\005\320\005\326\005\340\005\325\005\3529\005\323\005\325\005\3239\005\322\005\336\005\331\005\351\005\325\005\3529\005\321\005\350\005\336\005\325\005\352"
"9\005\321\005\336\005\331\005\323\005\3249\005\321\005\320\005\325\005\344\005\3379\005\320\005\352\005\333\005\3359\005\320\005\331\005\3519\005\320\005\325\005\352\005\3319\005\320\005\325\005\336\005\340\005\325\005\352"
"9\004\073\004\116\004\064\004\065\004\0719pts9chtg9\006\054\006\057\006\0479\006\050\006\047\006\112\006\0579\004\107\004\070\004\101\004\073\004\0769\004\101\004\074\004\0659\004\101\004\072\004\060\004\067\004\060\004\073"
"9\004\063\004\076\004\064\004\0609\003\305\003\300\003\254\003\301\003\307\003\265\003\2719\371\001\1419ddf9\006\052\006\055\006\0529\006\050\006\1039\006\047\006\104\006\103\006\061\006\112\006\1059\373\001\1239nghr9\013\252\013\262"
"9\006\101\006\102\006\047\006\1049\006\071\006\104\006\1059\004\077\004\112\004\1029\004\066\004\070\004\067\004\075\004\1149\004\064\004\062\004\0659\004\062\004\076\004\064\004\0709kcj9\006\110\006\047\006\055\006\0579\006\110\006\045\006\106"
"9\006\047\006\1129\006\047\006\104\006\101\006\047\006\063\006\1129\006\047\006\104\006\064\006\112\006\071\006\0519\006\042\006\112\006\0519\004\076\004\072\004\076\004\073\004\0769thn9gps9chrz9\013\225\013\2629\006\112\006\054\006\050"
"9\006\102\006\110\006\1049\004\075\004\1039tth9ndd9mpf9ltt9crn9chm9\006\112\006\2519wsv9hrh9\006\047\006\104\006\105\006\063\006\104\006\105\006\112\006\1069\006\047\006\104\006\056\006\047\006\065\006\0519\006\047\006\104\006\043\006\110\006\104"
"9\356t9fft9\013\205\013\2619\006\110\006\102\006\0529\006\106\006\0529\006\057\006\047\006\064\006\052\006\1079\004\077\004\076\004\107\004\065\004\074\004\1039\003\265\003\263\003\3169\004\074\004\070\004\1009\001\1109rsv9njsk"
"9\006\050\006\110\006\057\006\106\006\0579\006\043\006\106\006\107\006\0479\004\115\004\102\004\076\004\0719\004\100\004\076\004\101\004\101\004\070\004\0709\004\062\004\101\004\065\004\063\004\064\004\060"
"9\003\307\003\301\003\314\003\275\003\271\003\2619jh9\006\047\006\104\006\071\006\104\006\1059\004\077\004\076\004\102\004\076\004\0749\004\075\004\117\004\072\004\076\004\0719\004\075\004\060\004\077\004\100\004\070\004\074\004\065\004\100"
"9\001\141lj9rhwng9k\001\141n9hwn9\004\102\004\076\004\066\004\0659\004\062\004\070\004\0659\006\057\006\047\006\056\006\1049\006\052\006\106\006\107\006\0479\006\050\006\043\006\1069\006\047\006\063\006\052\006\101\006\047\006\057\006\107"
"9\004\061\004\076\004\063\004\0609\360j9pv9\006\104\006\104\006\1079\004\100\004\076\004\073\004\1149\004\077\004\076\004\115\004\102\004\076\004\074\004\1039\004\075\004\070\004\111\004\0769\004\064\004\060\004\073\004\065\004\072\004\076"
"9\004\061\004\117\004\105\004\0609trg9sml9r\3409\006\107\006\105\006\047\006\1069\006\050\006\1129ppg9nhl9\006\050\006\106\006\047\006\0529\006\047\006\104\006\065\006\104\006\047\006\0519\006\043\006\101\006\066\006\104"
"9\004\072\004\103\004\073\004\114\004\102\004\103\004\100\004\0609\004\062\004\076\004\077\004\100\004\076\004\1019\001\1459yng9tbl9szl9ldk9\006\054\006\057\006\112\006\057\006\0519\006\050\006\110\006\057\006\107"
"9\004\102\004\065\004\077\004\065\004\100\004\1149\004\100\004\103\004\101\004\101\004\072\004\076\004\0719\004\077\004\076\004\107\004\102\004\0709\004\066\004\070\004\062\004\076\004\1029\005\352\005\336\005\331\005\323"
"9\005\351\005\325\005\340\005\331\005\3359\005\351\005\325\005\321\005\321\005\340\005\3319\005\350\005\344\005\325\005\320\005\3249\005\347\005\331\005\331\005\3359\005\347\005\331\005\323\005\325\005\3359\005\347\005\331\005\321\005\325\005\345"
"9\005\347\005\325\005\321\005\3459\005\346\005\325\005\330\005\3309\005\344\005\352\005\350\005\325\005\340\005\325\005\3529\005\344\005\341\005\331\005\344\005\341\005\3309\005\344\005\331\005\352\005\325\005\3279\005\344\005\324"
"9\005\342\005\350\005\331\005\333\005\3529\005\341\005\344\005\350\005\331\005\3359\005\341\005\325\005\325\005\331\005\330\005\325\005\3529\005\340\005\351\005\331\005\3359\005\340\005\325\005\333\005\327\005\3319\005\340\005\323\005\350\005\351"
"9\005\3409\005\336\005\351\005\3209\005\336\005\344\005\340\005\3319\005\336\005\334\005\327\005\336\005\3249\005\336\005\331\005\325\005\3359\005\336\005\325\005\336\005\334\005\346\005\331\005\3359\005\336\005\320\005\336\005\350\005\331\005\335"
"9\005\334\005\351\005\344\005\3509\005\334\005\346\005\325\005\325\005\3529\005\334\005\342\005\341\005\3479\005\334\005\336\005\346\005\325\005\3209\005\334\005\331\005\350\005\3269\005\334\005\331\005\325\005\3359\005\334\005\323\005\343"
"9\005\334\005\323\005\321\005\3509\005\333\005\342\005\3529\005\333\005\3279\005\331\005\350\005\325\005\351\005\3249\005\331\005\342\005\347\005\3219\005\327\005\351\005\331\005\321\005\325\005\3529\005\327\005\351\005\331\005\321\005\324"
"9\005\327\005\325\005\344\005\351\005\3249\005\325\005\340\005\331\005\324\005\325\005\3349\005\325\005\333\005\3379\005\324\005\351\005\340\005\331\005\3529\005\324\005\346\005\331\005\321\005\325\005\3509\005\324\005\346"
"9\005\324\005\342\005\331\005\3509\005\324\005\3379\005\324\005\336\005\340\005\324\005\3349\005\324\005\336\005\331\005\334\005\3249\005\324\005\336\005\325\005\350\005\3249\005\324\005\333\005\3319\005\324\005\331\005\331\005\352\005\324"
"9\005\324\005\327\005\325\005\3479\005\324\005\326\005\320\005\3529\005\324\005\320\005\331\005\351\005\3319\005\324\005\320\005\322\005\350\005\3529\005\323\005\331\005\350\005\325\005\3229\005\322\005\325\005\334\005\351\005\331\005\335"
"9\005\321\005\352\005\325\005\3329\005\321\005\342\005\331\005\325\005\3529\005\321\005\327\005\3509\005\320\005\334\005\347\005\330\005\350\005\325\005\340\005\3319\005\320\005\331\005\350\005\325\005\342\005\3319\005\320\005\325\005\352\005\332"
"9\003\274\003\261\003\266\003\2579tsz9czt9\006\106\006\314\006\063\006\0529\006\106\006\101\006\0639\006\105\006\061\006\0519\006\061\006\110\006\3149tv\371j9ttf9skm9r\3629\004\101\004\062\004\076\004\065\004\0719v\001\0159htm9\013\265\013\256"
"9\013\244\013\225\013\265\013\2629\006\206\006\106\006\0579\006\105\006\056\006\052\006\104\006\1019\006\063\006\0619\006\057\006\047\006\057\006\1079lph9bz9\006\043\006\110\006\1049nspr9\013\260\013\2329\011\062\011\044"
"9\004\064\004\065\004\073\004\0769\004\063\004\076\004\073\004\076\004\1019tschl9rkv9ndb9\006\206\006\110\006\1069\006\057\006\061\006\057\006\064\006\0519\006\052\006\101\006\063\006\112\006\0619\006\047\006\104\006\055\006\057\006\112\006\053"
"9\004\102\004\076\004\107\004\075\004\0769\004\102\004\065\004\061\004\1179\004\076\004\101\004\075\004\076\004\062\004\0609\003\272\003\254\003\304\003\2719ndst9bk9\013\237\013\2449\006\110\006\104\006\314"
"9\006\105\006\110\006\047\006\066\006\112\006\0719\006\071\006\106\006\057\006\105\006\0479\006\070\006\171\006\0709\006\063\006\106\006\0519\003\264\003\2659\003\2649\013\216\013\2649\013\205\013\2569\003\261\003\272\003\314\003\274\003\2619ymm9yc"
"9pph9\006\106\006\070\006\047\006\1059\006\103\006\052\006\0509\006\065\006\110\006\061\006\0529\006\057\006\112\006\257\006\0619\006\056\006\057\006\105\006\047\006\0529\006\047\006\104\006\050\006\061\006\047\006\105\006\054"
"9\004\074\004\076\004\066\004\065\004\102\004\0659\004\063\004\076\004\064\004\070\004\075\004\0609\003\265\003\275\003\3169\013\244\013\2529\006\056\006\047\006\104\006\0579\004\075\004\060\004\107\004\070\004\0759pln9dzk9\011\070\011\0560"};

unsigned char Consonants008[] = {
"9\006\106\006\047\006\1059\006\066\006\0579\006\052\006\110\006\063\006\0679\004\062\004\112\004\100\004\105\004\1039\001\007n9tsc9\013\265\013\2379\011\052\011\0419\006\065\006\101\006\055\006\051"
"9\006\047\006\104\006\071\006\104\006\105\006\047\006\0419\003\314\003\300\003\277\003\3059\003\300\003\301\003\271\003\2759\003\300\003\261\003\271\003\264\003\271\003\2549\003\265\003\300\003\257\003\303\003\267\003\3029htl9\013\225\013\243"
"9\006\107\006\047\006\1129\006\106\006\055\006\1109\006\104\006\112\006\104\006\0519\006\052\006\061\006\052\006\112\006\0509\004\075\004\070\004\061\004\103\004\064\004\1149\003\255\003\307\003\3119\001\104c9\001\102n9ynt9wg9\006\106\006\110\006\071"
"9\006\103\006\061\006\0579\006\057\006\110\006\104\006\0529\006\047\006\106\006\054\006\047\006\1059\003\270\003\255\003\274\003\2619wc9jks9\006\112\006\071\006\106\006\1129\006\105\006\106\006\0609\006\101\006\112\006\105\006\0479\006\060"
"9\006\057\006\047\006\061\006\106\006\0579\006\047\006\104\006\067\006\101\006\1049\006\107\006\047\006\1109\006\050\006\112\006\0719\004\117\004\075\004\070\004\0659\004\102\004\060\004\072\004\076\004\071"
"9\004\070\004\101\004\102\004\076\004\100\004\070\004\1179\003\277\003\315\003\304\003\2659\003\272\003\254\003\275\003\265\003\2719\003\265\003\272\003\265\003\2579\003\261\003\3009\376j9\356\3569v\001\176d9ttv9rsm9flw9\011\005\011\055"
"9\006\057\006\047\006\064\006\0529\006\047\006\104\006\057\006\112\006\1069\004\107\004\065\004\063\004\0769\004\107\004\060\004\101\004\1029\003\304\003\314\003\303\003\2779\003\272\003\261\037\1669\357\3569plz9\014\266\014\227\014\263"
"9\014\265\014\2609\014\265\014\2509\014\262\014\252\014\2609\014\260\014\2569\014\260\014\2529\014\260\014\227\014\2639\014\257\014\2659\014\257\014\2609\014\257\014\257\014\2509\014\257\014\2469\014\256\014\250\014\2659\014\254\014\263\014\270"
"9\014\254\014\246\014\2629\014\2539\014\252\014\260\014\2659\014\250\014\2629\014\250\014\2569\014\250\014\2279\014\246\014\2629\014\232\014\2509\014\232\014\2469\014\2239\014\217\014\2509\014\216\014\2529\014\211\014\252\014\2579\014\211\014\246"
"9\014\211\014\2329\014\206\014\252\014\2609\014\206\014\2479\014\205\014\2609\014\205\014\2469\013\260\013\2629\013\207\013\2519\006\050\006\061\006\061\006\063\006\3149\006\047\006\104\006\053\006\047\006\106\006\112"
"9\005\352\005\344\005\350\005\331\005\3309\005\352\005\334\005\325\005\340\005\325\005\3529\005\351\005\331\005\325\005\352\005\3509\005\351\005\325\005\340\005\325\005\3529\005\351\005\325\005\3219\005\351\005\323\005\324"
"9\005\351\005\320\005\340\005\3259\005\350\005\346\005\331\005\352\005\3319\005\350\005\325\005\336\005\340\005\330\005\3319\005\350\005\320\005\351\005\325\005\340\005\3249\005\347\005\3349\005\346\005\331\005\334\005\325\005\3359\005\342\005\345"
"9\005\342\005\341\005\347\005\3319\005\342\005\331\005\350\005\320\005\3479\005\342\005\325\005\334\005\3359\005\341\005\325\005\3229\005\341\005\323\005\340\005\320\005\325\005\3529\005\340\005\333\005\325\005\340\005\324"
"9\005\336\005\352\005\327\005\350\005\331\005\3359\005\336\005\351\005\336\005\342\005\325\005\352\005\331\005\3529\005\336\005\351\005\3249\005\336\005\347\005\325\005\350\005\331\005\325\005\352"
"9\005\336\005\331\005\325\005\327\005\323\005\331\005\3359\005\336\005\327\005\331\005\3509\005\336\005\323\005\331\005\340\005\3529\005\336\005\322\005\325\005\325\005\3379\005\336\005\320\005\331\005\3509\005\334\005\351\005\335"
"9\005\334\005\350\005\320\005\325\005\3529\005\334\005\347\005\350\005\325\005\3209\005\334\005\344\005\340\005\325\005\3529\005\334\005\340\005\331\005\3259\005\334\005\336\005\351\005\344\005\327\005\325\005\352"
"9\005\334\005\324\005\342\005\334\005\325\005\3529\005\334\005\324\005\342\005\321\005\331\005\3509\005\334\005\324\005\322\005\331\005\3429\005\334\005\323\005\342\005\3529\005\334\005\321\005\331\005\3379\005\334\005\321\005\327\005\325\005\350"
"9\005\333\005\352\005\321\005\3259\005\333\005\331\005\346\005\3239\005\331\005\3239\005\330\005\331\005\341\005\325\005\3529\005\330\005\325\005\350\005\3319\005\330\005\325\005\321\005\3249\005\327\005\334\005\3479\005\327\005\325\005\347"
"9\005\324\005\351\005\340\005\331\005\3249\005\324\005\351\005\331\005\350\005\325\005\3529\005\324\005\351\005\325\005\340\005\325\005\3529\005\324\005\347\005\336\005\3529\005\324\005\341\005\344\005\350\005\331\005\3359\005\324\005\334\005\332"
"9\005\324\005\333\005\334\005\325\005\334\005\331\005\3359\005\324\005\331\005\351\005\350\005\320\005\334\005\3319\005\324\005\327\005\331\005\344\005\325\005\3519\005\324\005\325\005\350\005\331\005\3359\005\324\005\325\005\341\005\344\005\352"
"9\005\324\005\323\005\320\005\3419\005\324\005\320\005\326\005\340\005\3249\005\323\005\325\005\3509\005\322\005\334\005\350\005\331\005\325\005\3529\005\322\005\325\005\334\005\3259\005\3229\005\321\005\347\005\352\005\325\005\352"
"9\005\321\005\344\005\340\005\331\005\3329\005\321\005\336\005\351\005\3329\005\321\005\324\005\346\005\334\005\327\005\3249\005\321\005\321\005\331\005\3529\005\320\005\352\005\350\005\3319\005\320\005\346\005\334\005\340\005\325"
"9\005\320\005\344\005\351\005\350\005\325\005\3529\005\320\005\340\005\350\005\322\005\331\005\331\005\3529\005\320\005\331\005\351\005\331\005\331\005\3359\005\320\005\331\005\350\005\325\005\3429\005\320\005\327\005\350\005\352"
"9\005\320\005\327\005\350\005\331\005\325\005\3529\005\320\005\325\005\352\005\340\005\3259\005\320\005\325\005\334\005\3319\004\100\004\060\004\061\004\076\004\102\004\1139t\001\1419\013\207\013\262\013\2319\011\057\011\025"
"9\003\265\003\273\003\273\003\254\003\264\003\2619\003\261\003\300\003\2779\013\261\013\2659\013\256\013\2619\006\105\006\063\006\104\006\1059\006\050\006\0639\006\047\006\104\006\061\006\055\006\105\006\1069ngth9\013\250\013\2379\011\043"
"9\006\101\006\110\006\1029\006\0629\006\055\006\057\006\112\006\0539\006\054\006\071\006\101\006\0619\006\047\006\104\006\106\006\070\006\047\006\1059\006\047\006\104\006\047\006\052\006\065\006\047\006\1049\006\047\006\066\006\072\006\067"
"9\004\105\004\076\004\102\004\1179\004\075\004\065\004\1179\004\061\004\070\004\0739sf\356nt9rx9\006\112\006\103\006\1069\006\071\006\061\006\050\006\1129\004\077\004\100\004\076\004\102\004\070\004\0629ppn9ddwl9\011\070\011\060"
"9\011\062\011\027\011\0449\006\050\006\3149\006\047\006\055\006\105\006\0579\004\075\004\117\004\072\004\076\004\0709\004\074\004\076\004\0719\006\105\006\064\006\047\006\061\006\103\006\047\006\0529\006\104\006\107\006\105"
"9\006\052\006\071\006\057\006\112\006\1049\006\047\006\104\006\071\006\105\006\0619\006\047\006\104\006\054\006\105\006\071\006\0519\004\063\004\076\004\062\004\076\004\100\004\070\004\1029rgh9rcr9ppf9ndw9\011\070\011\056\011\057"
"9\011\070\011\025\011\0449\006\106\006\112\006\0629\006\061\006\063\006\047\006\104\006\0519\006\055\006\063\006\1069\006\052\006\105\006\047\006\1059\006\047\006\104\006\057\006\1059\006\047\006\104\006\047\006\063\006\105"
"9\006\042\006\102\006\047\006\3149\004\073\004\070\004\110\004\1149\003\277\003\300\003\277\003\257\003\2779\003\254\003\301\003\270\003\301\003\2779\373t9psz9nsm9c\356t9\006\071\006\061\006\0509rgt9msl9\011\044\011\060"
"9\006\071\006\061\006\050\006\112\006\0519\006\063\006\110\006\061\006\0519\006\055\006\060\006\1019dh9\013\260\013\2569\013\256\013\2519\011\065\011\0719\006\102\006\065\006\0659\006\070\006\1719\006\052\006\065\006\105\006\112\006\105"
"9\006\047\006\104\006\071\006\047\006\105\006\0519\005\3259\004\102\004\112\004\0719\004\075\004\117\004\072\004\076\004\073\004\072\004\0769tthv9nsd9bbr9\006\057\006\047\006\0579\006\047\006\110\006\1049\003\316\003\303\003\304\003\265"
"9\003\254\003\273\003\273\003\2779y\3609lft9\011\0059\006\071\006\061\006\110\006\0669\006\052\006\106\006\062\006\112\006\1049\006\047\006\063\006\052\006\105\006\047\006\0719\004\072\004\076\004\102\004\076\004\100\004\060\004\117"
"9\004\066\004\070\004\062\004\076\004\102\004\0609nrk9lt\3409\013\265\013\2509\011\2529\004\101\004\102\004\060\004\062\004\0609\004\075\004\076\004\062\004\076\004\101\004\102\004\0709\004\062\004\065\004\064\004\114"
"9\004\060\004\062\004\102\004\076\004\1009\003\261\003\305\003\304\003\314\003\3029sgw9rgsm9p\001\0079ksj9hlt9\011\052\011\0449\006\071\006\104\006\112\006\1039\006\064\006\056\006\0659\006\047\006\104\006\105\006\110\006\063\006\110\006\071\006\051"
"9\006\047\006\104\006\102\006\104\006\0509\006\047\006\104\006\063\006\110\006\057\006\047\006\1069\004\101\004\062\004\076\004\1169\004\101\004\062\004\065\004\102\004\0609\004\074\004\076\004\063\004\060\004\102"
"9\004\064\004\100\004\103\004\063\004\0609\001\174l9ksk9bch9\013\211\013\244\013\2659\006\104\006\042\006\056\006\0619\006\071\006\064\006\0619\006\060\006\047\006\0529\006\055\006\0579\006\047\006\104\006\103\006\052\006\050"
"9\006\047\006\104\006\065\006\110\006\0619\006\047\006\104\006\055\006\112\006\047\006\0519\006\047\006\102\006\052\006\050\006\047\006\0639\004\101\004\062\004\076\004\0709\003\2629sph9rft9ndg9lfr9\004\127\004\105\004\075\004\114\004\076\004\127"
"9\004\126\004\1269\004\117\004\067\004\113\004\072\004\076\004\0629\004\117\004\067\004\060\004\075\004\0609\004\116\004\100\004\126\004\0719\004\111\004\076\004\071\004\075\004\0769\004\111\004\076\004\064\004\065\004\075\004\075\004\076"
"9\004\110\004\103\004\072\004\060\004\102\004\0709\004\110\004\065\004\072\004\101\004\075\004\0609\004\110\004\060\004\074\004\077\004\073\004\065\004\071\004\0759\004\107\004\073\004\065\004\075\004\076\004\074"
"9\004\107\004\070\004\102\004\060\004\071\004\102\004\0659\004\107\004\070\004\075\004\076\004\0749\004\107\004\065\004\100\004\062\004\075\004\1269\004\107\004\060\004\101\004\102\004\070\004\075\004\1039\004\106\004\126\004\071"
"9\004\106\004\070\004\102\004\103\004\062\004\060\004\102\004\0709\004\106\004\070\004\074\004\0709\004\106\004\065\004\075\004\102\004\100\004\076\004\0749\004\105\004\1059\004\105\004\062\004\070\004\073\004\114"
"9\004\105\004\062\004\070\004\073\004\070\004\0759\004\104\004\076\004\100\004\074\004\060\004\106\004\126\004\1169\004\104\004\076\004\075\004\064\004\126\004\0629\004\104\004\060\004\072\004\102\004\070\004\107\004\075\004\0760"};

unsigned char Consonants009[] = {
"9\004\103\004\100\004\101\004\1009\004\103\004\100\004\070\004\062\004\072\004\0709\004\103\004\100\004\060\004\073\004\1269\004\103\004\072\004\100\004\060\004\127\004\075\004\0609\004\102\004\100\004\114\004\076\004\105"
"9\004\102\004\100\004\065\004\102\004\126\004\0719\004\102\004\070\004\1019\004\102\004\065\004\100\004\074\004\126\004\0759\004\102\004\065\004\072\004\101\004\102\004\1039\004\102\004\065\004\067\004\1269\004\102\004\060\004\102"
"9\004\102\004\060\004\072\004\1269\004\102\004\060\004\071\004\221\004\0709\004\101\004\126\004\073\004\0609\004\101\004\105\004\126\004\064\004\075\004\1269\004\101\004\105\004\126\004\064\004\075\004\076"
"9\004\101\004\102\004\076\004\100\004\126\004\075\004\106\004\1269\004\101\004\102\004\076\004\073\004\070\004\106\004\1269\004\101\004\102\004\062\004\076\004\100\004\070\004\102\004\070"
"9\004\101\004\102\004\062\004\076\004\100\004\070\004\073\004\0709\004\101\004\102\004\060\004\102\004\102\004\117\004\1059\004\101\004\102\004\060\004\102\004\0709\004\101\004\073\004\126\004\062"
"9\004\101\004\073\004\103\004\105\004\060\004\102\004\0709\004\101\004\073\004\103\004\105\004\060\004\075\004\075\004\1179\004\101\004\073\004\103\004\063\004\0709\004\101\004\073\004\076\004\062\004\065\004\075\004\126\004\062"
"9\004\101\004\070\004\101\004\102\004\065\004\074\004\1269\004\101\004\070\004\063\004\073\004\0609\004\101\004\070\004\061\004\126\004\100\004\117\004\072\004\0709\004\101\004\070\004\061\004\126\004\100"
"9\004\101\004\065\004\101\004\102\004\100\004\0709\004\101\004\065\004\101\004\102\004\065\004\1009\004\101\004\065\004\100\004\065\004\064\004\075\004\126\004\1059\004\101\004\065\004\100\004\065\004\064"
"9\004\101\004\065\004\073\004\117\004\075\004\072\004\0709\004\101\004\062\004\117\004\102\004\126\004\110\004\070\004\0749\004\101\004\062\004\117\004\102\004\126\004\110\004\070\004\0719\004\101\004\062\004\117\004\102\004\076\004\127"
"9\004\101\004\062\004\117\004\102\004\076\004\1169\004\101\004\060\004\071\004\102\004\1269\004\101\004\060\004\071\004\102\004\0709\004\101\004\060\004\071\004\102\004\060\004\074\004\0709\004\100\004\126\004\074\004\126\004\075\004\126"
"9\004\100\004\126\004\072\004\0709\004\100\004\126\004\064\004\075\004\1039\004\100\004\103\004\101\004\114\004\072\004\1269\004\100\004\103\004\101\004\114\004\072\004\076\004\1279\004\100\004\103\004\101\004\101\004\072\004\070\004\105"
"9\004\100\004\103\004\067\004\062\004\065\004\073\004\114\004\1029\004\100\004\076\004\107\004\065\004\101\004\102\004\065\004\1009\004\100\004\076\004\101\004\126\004\124\004\1169\004\100\004\076\004\074\004\060\004\075\004\076"
"9\004\100\004\076\004\067\004\100\004\076\004\061\004\072\004\0709\004\100\004\076\004\067\004\072\004\073\004\060\004\0649\004\100\004\076\004\067\004\064\004\126\004\073\004\1269\004\100\004\076\004\067\004\062\004\070\004\102\004\076\004\072"
"9\004\100\004\076\004\067\004\062\004\070\004\102\004\072\004\1039\004\100\004\065\004\101\004\0779\004\100\004\065\004\072\004\102\004\076\004\1009\004\100\004\060\004\105\004\103\004\075\004\076\004\072"
"9\004\100\004\060\004\105\004\103\004\062\004\060\004\102\004\0709\004\100\004\060\004\075\004\126\004\110\004\0659\004\077\004\126\004\101\004\073\004\1179\004\077\004\126\004\062\004\075\004\126\004\107\004\075\004\1269\004\077\004\112\004\072"
"9\004\077\004\100\004\117\004\074\004\076\004\074\004\1039\004\077\004\100\004\076\004\117\004\062\004\0709\004\077\004\100\004\076\004\065\004\072\004\102\004\1269\004\077\004\100\004\076\004\065\004\072\004\102\004\070"
"9\004\077\004\100\004\070\004\072\004\074\004\065\004\1029\004\077\004\076\004\107\004\060\004\102\004\076\004\0729\004\077\004\076\004\101\004\102\004\126\004\071\004\075\004\0769\004\077\004\076\004\101\004\102\004\060\004\075\004\075\004\117"
"9\004\077\004\076\004\075\004\060\004\0649\004\077\004\076\004\072\004\060\004\067\004\060\004\102\004\0709\004\077\004\076\004\064\004\076\004\100\004\076\004\066\004\1269\004\077\004\076\004\061\004\060\004\107\004\070\004\102\004\065"
"9\004\077\004\073\004\076\004\111\004\0609\004\077\004\073\004\065\004\074\004\065\004\0759\004\077\004\070\004\102\004\060\004\075\004\075\004\1179\004\077\004\065\004\107\004\060\004\102\004\072\004\0609\004\077\004\065\004\100\004\103"
"9\004\077\004\065\004\100\004\065\004\077\004\070\004\101\004\0709\004\077\004\065\004\100\004\065\004\0629\004\077\004\060\004\077\004\101\004\114\004\072\004\1269\004\077\004\060\004\077\004\060"
"9\004\076\004\107\004\070\004\101\004\102\004\070\004\102\004\0709\004\076\004\102\004\106\004\065\004\0749\004\076\004\102\004\065\004\106\004\1149\004\076\004\101\004\126\004\0619\004\076\004\101\004\102\004\060\004\075\004\075\004\117"
"9\004\076\004\101\004\076\004\061\004\073\004\070\004\062\004\0769\004\076\004\072\004\100\004\065\004\074\004\1269\004\076\004\064\004\100\004\060\004\067\004\1039\004\076\004\061\004\073\004\060\004\101\004\102\004\1269\004\076\004\061\004\073"
"9\004\075\004\114\004\076\004\074\004\1039\004\075\004\103\004\066\004\075\004\0769\004\075\004\076\004\062\004\070\004\0759\004\075\004\070\004\102\004\0769\004\075\004\065\004\100\004\076\004\101\004\126\004\117\004\075"
"9\004\075\004\065\004\064\004\126\004\073\004\1269\004\075\004\065\004\064\004\126\004\073\004\1169\004\075\004\065\004\064\004\076\004\073\004\126\004\072\004\0709\004\075\004\060\004\111\004\060\004\064\004\072\004\070"
"9\004\075\004\060\004\110\004\076\004\074\004\1039\004\075\004\060\004\106\004\126\004\0719\004\075\004\060\004\101\004\073\004\126\004\064\004\076\004\0729\004\075\004\060\004\064\004\102\004\076"
"9\004\075\004\060\004\064\004\062\004\076\004\073\004\066\004\1269\004\075\004\060\004\062\004\065\004\064\004\065\004\075\004\0769\004\074\004\126\004\102\004\070\004\075\004\0639\004\074\004\126\004\101\004\106"
"9\004\074\004\126\004\101\004\102\004\0609\004\074\004\076\004\073\004\076\004\064\004\1149\004\074\004\076\004\073\004\070\004\102\004\062\004\0609\004\074\004\076\004\066\004\073\004\070\004\062\004\0769\004\074\004\076\004\062\004\103"
"9\004\074\004\065\004\075\004\110\004\0659\004\074\004\060\004\1249\004\074\004\060\004\063\004\060\004\067\004\070\004\0759\004\073\004\126\004\102\004\103\004\100\004\063\004\126\004\1179\004\073\004\117\004\077\004\103\004\075\004\076\004\062"
"9\004\073\004\114\004\062\004\076\004\062\004\1269\004\073\004\070\004\1029\004\073\004\060\004\101\004\0729\004\072\004\126\004\075\004\106\004\1179\004\072\004\076\004\100\004\076\004\102\004\072\004\070\004\105"
"9\004\072\004\076\004\100\004\064\004\076\004\075\004\076\004\0749\004\072\004\076\004\075\004\065\004\107\004\075\004\0769\004\072\004\076\004\0719\004\072\004\076\004\067\004\060\004\072\004\0709\004\072\004\075\004\070\004\067\004\126"
"9\004\072\004\073\004\117\004\067\004\114\004\074\004\1269\004\072\004\070\004\124\004\062\004\1269\004\072\004\065\004\1109\004\072\004\060\004\102\004\060\004\073\004\076\004\0639\004\072\004\060\004\101\004\102\004\065\004\073\004\114"
"9\004\072\004\060\004\100\004\076\004\073\004\126\004\075\004\0609\004\072\004\060\004\062\004\072\004\060\004\067\004\1269\004\070\004\100\004\076\004\075\004\060\004\1039\004\070\004\0719\004\067\004\1269\004\067\004\075\004\076\004\062\004\103"
"9\004\067\004\075\004\060\004\071\004\064\004\065\004\102\004\0659\004\067\004\074\004\126\004\075\004\0609\004\067\004\065\004\074\004\073\004\1269\004\067\004\060\004\105\004\070\004\101\004\102\004\103"
"9\004\067\004\060\004\101\004\076\004\061\004\126\004\0629\004\067\004\060\004\075\004\0609\004\067\004\060\004\073\004\1269\004\067\004\060\004\072\004\073\004\070\004\072\004\0709\004\066\004\076\004\064\004\075\004\070\004\074"
"9\004\066\004\070\004\102\004\065\004\073\004\1269\004\066\004\070\004\073\004\0769\004\066\004\070\004\062\004\103\004\102\004\1149\004\066\004\065\004\100\004\102\004\062\004\060\004\0749\004\065\004\104\004\126\004\100\004\126"
"9\004\065\004\102\004\075\004\126\004\107\004\075\004\0769\004\065\004\102\004\075\004\126\004\107\004\075\004\0609\004\065\004\100\004\1269\004\065\004\067\004\070\004\0729\004\064\004\103\004\105\004\076\004\062\004\075\004\126\004\071"
"9\004\064\004\103\004\105\004\076\004\062\004\075\004\076\004\1279\004\064\004\100\004\103\004\066\004\061\004\1039\004\064\004\076\004\101\004\070\004\102\004\1149\004\064\004\076\004\100\004\076\004\063\004\126"
"9\004\064\004\066\004\065\004\100\004\065\004\073\004\0609\004\064\004\066\004\065\004\071\004\074\004\1019\004\064\004\065\004\100\004\066\004\060\004\062\004\0709\004\064\004\062\004\1269\004\064\004\060\004\075\004\070\004\071"
"9\004\064\004\060\004\073\004\065\004\072\004\070\004\0719\004\064\004\060\004\072\004\076\004\102\004\0609\004\063\004\126\004\074\004\0759\004\063\004\100\004\103\004\0779\004\063\004\100\004\065\004\106\004\126\004\117"
"9\004\063\004\076\004\100\004\0709\004\063\004\075\004\1039\004\063\004\073\004\117\004\075\004\103\004\102\004\0709\004\063\004\065\004\075\004\100\004\1269\004\062\004\126\004\073\004\114\004\075\004\065"
"9\004\062\004\126\004\064\004\101\004\076\004\102\004\076\004\0729\004\062\004\126\004\064\004\076\004\074\004\1269\004\062\004\126\004\064\004\074\004\126\004\075\004\1039\004\062\004\126\004\064\004\064\004\126\004\073"
"9\004\062\004\101\004\126\004\124\004\1279\004\062\004\101\004\126\004\1059\004\062\004\101\004\114\004\076\004\074\004\1039\004\062\004\101\004\114\004\076\004\063\004\0769\004\062\004\077\004\073\004\070\004\075\004\103\004\073\004\076"
"9\004\062\004\074\004\126\004\101\004\1029\004\062\004\073\004\060\004\064\004\0709\004\062\004\072\004\073\004\116\004\107\004\075\004\0769\004\062\004\070\004\111\004\076\004\1169\004\062\004\070\004\077\004\100\004\060\004\062\004\102\004\065"
"9\004\062\004\070\004\064\004\060\004\102\004\075\004\1269\004\062\004\065\004\073\004\070\004\072\004\0609\004\062\004\065\004\0619\004\062\004\060\004\102\004\070\004\072\004\060\004\075\004\126"
"9\004\062\004\060\004\102\004\070\004\072\004\060\004\075\004\1039\004\061\004\126\004\073\004\076\004\063\004\0769\004\061\004\103\004\104\004\104\004\060\004\073\004\0769\004\061\004\103\004\102\004\0709\004\061\004\103\004\0620"};

unsigned char Consonants010[] = {
"9\004\061\004\100\004\060\004\102\004\0709\004\061\004\100\004\060\004\067\004\070\004\073\004\126\004\1279\004\061\004\076\004\073\004\065\004\100\004\0769\004\061\004\076\004\072\004\1039\004\061\004\076\004\066\004\076\004\063\004\076"
"9\004\061\004\076\004\064\004\075\004\060\004\1009\004\061\004\060\004\067\004\0709\004\060\004\075\004\060\004\102\004\076\004\073\004\126\004\1279\004\060\004\074\004\065\004\100\004\070\004\072\004\070"
"9\004\060\004\074\004\061\004\100\004\076\004\101\004\126\004\1279\004\060\004\071\004\073\004\065\004\075\004\0649\004\060\004\067\004\126\004\1169\013\257\013\2449\006\104\006\107\006\060\006\0479\006\102\006\047\006\104\006\052"
"9\006\050\006\106\006\0479\006\047\006\104\006\103\006\052\006\047\006\0509\004\101\004\072\004\076\004\100\004\0769\004\100\004\060\004\061\004\076\004\102\004\1039\004\064\004\076\004\073\004\066\004\075\004\1139strz9kg9\013\252\013\225"
"9\013\216\013\2709\011\056\011\0509\006\206\006\061\006\0479\006\104\006\102\006\0579\004\062\004\101\004\1219\003\265\003\274\003\265\003\257\003\3029\006\103\006\1059\005\352\005\3409\005\352\005\336\005\331\005\333\005\324"
"9\005\352\005\326\005\325\005\340\005\3249\005\351\005\350\005\325\005\3529\005\351\005\340\005\325\005\3529\005\351\005\331\005\327\005\325\005\3529\005\351\005\331\005\3279\005\351\005\327\005\347\005\340\005\331\005\335"
"9\005\351\005\325\005\320\005\3439\005\351\005\321\005\325\005\3429\005\350\005\351\005\331\005\336\005\3529\005\350\005\327\005\3219\005\350\005\321\005\325\005\3529\005\347\005\325\005\350\005\3419\005\347\005\325\005\3359\005\347\005\325\005\323"
"9\005\346\005\350\005\3259\005\346\005\325\005\350\005\3329\005\346\005\323\005\331\005\3479\005\344\005\334\005\325\005\3419\005\344\005\331\005\340\005\3249\005\342\005\350\005\3219\005\342\005\346\005\336\005\340\005\325"
"9\005\342\005\334\005\331\005\3259\005\342\005\334\005\331\005\3249\005\342\005\325\005\351\005\331\005\3359\005\341\005\330\005\325\005\350\005\3319\005\340\005\347\005\325\005\323\005\325\005\3529\005\340\005\344\005\325\005\346\005\325\005\352"
"9\005\340\005\331\005\3259\005\336\005\351\005\334\005\325\005\3279\005\336\005\346\005\3219\005\336\005\342\005\350\005\333\005\325\005\3529\005\336\005\342\005\340\005\331\005\331\005\3379\005\336\005\331\005\3359\005\336\005\326\005\324"
"9\005\336\005\321\005\346\005\3429\005\336\005\320\005\336\005\3509\005\336\005\320\005\336\005\331\005\3379\005\336\005\320\005\3239\005\334\005\351\005\325\005\340\005\331\005\3529\005\334\005\344\005\327\005\325\005\352"
"9\005\334\005\342\005\331\005\352\005\331\005\3359\005\334\005\340\005\324\005\3349\005\334\005\336\005\342\005\334\005\3249\005\334\005\327\005\346\005\3259\005\334\005\325\005\3279\005\334\005\324\005\344\005\325\005\332"
"9\005\334\005\324\005\331\005\333\005\340\005\3419\005\334\005\324\005\323\005\344\005\341\005\3249\005\334\005\321\005\346\005\3429\005\333\005\352\005\325\005\346\005\320\005\3249\005\333\005\344\005\3319\005\331\005\346\005\331\005\350\005\352"
"9\005\331\005\342\005\323\005\331\005\3359\005\331\005\331\005\351\005\3509\005\331\005\331\005\342\005\325\005\3459\005\331\005\323\005\3429\005\330\005\331\005\344\005\325\005\3349\005\327\005\3319\005\327\005\325\005\347\005\331\005\325\005\352"
"9\005\326\005\333\005\325\005\331\005\325\005\3529\005\325\005\350\005\342\005\331\005\325\005\340\005\325\005\3529\005\325\005\334\005\333\005\3379\005\325\005\333\005\3259\005\325\005\331\005\334\005\344\005\350\005\331\005\323"
"9\005\325\005\331\005\325\005\352\005\3509\005\325\005\326\005\320\005\3529\005\324\005\352\005\344\005\352\005\327\005\325\005\3529\005\324\005\350\005\346\005\320\005\325\005\3529\005\324\005\350\005\331\005\325\005\337"
"9\005\324\005\344\005\352\005\325\005\3279\005\324\005\344\005\350\005\342\005\325\005\3529\005\324\005\344\005\325\005\3329\005\324\005\340\005\331\005\324\005\325\005\3349\005\324\005\336\005\351\005\344\005\327\005\324"
"9\005\324\005\336\005\342\005\350\005\321\005\3319\005\324\005\336\005\341\005\3509\005\324\005\336\005\325\005\326\005\331\005\320\005\325\005\3379\005\324\005\334\005\347\005\325\005\327\005\325\005\3529\005\324\005\331\005\351\005\337"
"9\005\324\005\327\005\325\005\323\005\3519\005\324\005\327\005\323\005\3519\005\324\005\326\005\325\005\350\005\3429\005\324\005\322\005\325\005\334\005\3379\005\324\005\321\005\351\005\325\005\350\005\324"
"9\005\324\005\320\005\331\005\340\005\330\005\350\005\340\005\3309\005\324\005\320\005\323\005\3359\005\323\005\350\005\333\005\3259\005\323\005\350\005\322\005\3249\005\323\005\331\005\320\005\330\005\324"
"9\005\322\005\350\005\336\005\340\005\331\005\3249\005\322\005\323\005\325\005\334\005\3249\005\321\005\352\005\325\005\3509\005\321\005\351\005\342\005\325\005\3529\005\321\005\350\005\336\005\3529\005\321\005\350\005\325\005\333\005\331\005\335"
"9\005\321\005\347\005\351\005\3509\005\321\005\346\005\331\005\336\005\3509\005\321\005\342\005\331\005\347\005\3509\005\321\005\336\005\350\005\333\005\3269\005\321\005\336\005\347\005\325\005\3359\005\321\005\324\005\352\005\320\005\335"
"9\005\321\005\323\005\331\005\325\005\3479\005\321\005\3239\005\321\005\320\005\350\005\346\005\325\005\3529\005\320\005\344\005\351\005\3509\005\320\005\336\005\3509\005\320\005\334\005\344\005\3319\005\320\005\333\005\337"
"9\005\320\005\331\005\351\005\3319\005\320\005\327\005\350\005\3319\005\320\005\327\005\350\005\325\005\3529\005\320\005\325\005\336\005\3509\005\320\005\324\005\321\005\3249\004\117\004\062\004\073\004\117\004\065\004\102\004\101\004\117"
"9\004\101\004\065\004\063\004\076\004\064\004\075\004\1179\004\101\004\060\004\071\004\102\004\0609\360s9rtl9rbl9fdst9\013\237\013\2519\006\106\006\112\006\063\006\0529\006\101\006\107\006\1109\006\071\006\251\006\0639\006\067\006\276\006\070"
"9\004\064\004\076\004\072\004\060\004\102\004\0769\004\062\004\101\004\065\004\0749\003\263\003\2719\361\3609ykk9vmp9ndsz9ctrl9\011\054\011\0429\006\251\006\106\006\1059\006\110\006\071\006\104\006\1119\006\106\006\110\006\064\006\052\006\107"
"9\006\062\006\1069\006\047\006\104\006\105\006\052\006\055\006\057\006\0519\006\047\006\104\006\043\006\056\006\061\006\1119\004\073\004\070\004\061\004\0769snt9k\001\1029chv9\006\055\006\103\006\1059\006\050\006\047\006\062"
"9\004\101\004\062\004\076\004\0719\004\077\004\076\004\072\004\0609tt\3409rhw9\011\054\011\0419\011\044\011\060\011\0719\006\112\006\105\006\103\006\106\006\1039\006\105\006\102\006\047\006\104\006\047\006\0529\006\102\006\110\006\104\006\107"
"9\006\063\006\1079\006\060\006\103\006\0619\006\054\006\105\006\0719\006\047\006\104\006\054\006\057\006\112\006\057\006\0519\006\047\006\104\006\045\006\054\006\105\006\047\006\0719\004\074\004\076\004\063\004\060"
"9\004\064\004\065\004\073\004\060\004\102\004\1149\004\062\004\100\004\065\004\074\004\065\004\075\004\0709tkr9pz9nft9\011\0409\011\032\011\0629\006\110\006\047\006\104\006\104\006\1079\006\110\006\0479\006\106\006\110\006\0619\006\066"
"9\006\057\006\071\006\047\006\0419\006\047\006\104\006\106\006\063\006\047\006\0419\004\066\004\065\004\075\004\0609\004\065\004\0719\004\064\004\060\004\073\004\0709\003\266\003\311\003\2569\360st9ymd9xch9rls9lsc"
"9\006\047\006\106\006\063\006\047\006\1069\006\047\006\104\006\057\006\110\006\104\006\0519\004\074\004\076\004\063\004\103\004\1029\003\300\003\316\003\3029\003\274\003\255\003\307\003\301\003\2719ysg9nnsk9\013\225\013\237"
"9\006\105\006\050\006\106\006\1129\006\104\006\103\006\1049\006\050\006\056\006\0649\006\047\006\104\006\065\006\047\006\057\006\1029\004\107\004\100\004\065\004\0679\004\102\004\065\004\1059\004\062\004\070\004\075\004\060\004\063\004\0709rtf"
"9\006\054\006\107\006\047\006\1069\006\047\006\104\006\104\006\112\006\1049\006\047\006\104\006\057\006\071\006\110\006\112\006\0519\006\047\006\104\006\055\006\105\006\0579\006\045\006\060\006\107\006\0509\004\064\004\075\004\0709\353nt9j\001\141"
"9gmt9\013\206\013\2609\011\054\011\0449\006\047\006\105\006\047\006\1059\004\061\004\117\004\1059\003\300\003\261\003\303\003\277\003\2729\003\255\003\307\003\277\003\305\003\274\003\2659wpr9st\001\1319ppt9jgt9gns"
"9\006\102\006\061\006\047\006\041\006\0519\006\064\006\057\006\1069\006\056\006\047\006\106\006\1079\006\052\006\106\006\047\006\110\006\1049\006\052\006\102\006\110\006\1059\004\102\004\060\004\072\004\076\004\0659\004\075\004\060\004\110\004\070"
"9\001\133n9phr9ddwn9\011\034\011\0509\011\027\011\0109\006\251\006\106\006\314\006\0579\006\104\006\072\006\0519\006\067\006\104\006\050\006\047\006\0529\006\057\006\107\006\0579\006\047\006\104\006\105\006\054\006\052\006\105\006\071"
"9\006\047\006\104\006\064\006\071\006\0619\006\047\006\104\006\0479\006\047\006\104\006\043\006\110\006\104\006\1119\004\076\004\061\004\060\004\107\004\0659\004\075\004\065\004\0719\004\074\004\076\004\1179\004\072\004\060\004\072\004\070\004\065"
"9\004\061\004\103\004\064\004\103\004\1029\3539srdc9lsz9krv9dsl9\011\054\011\0609\006\106\006\102\006\1049\006\105\006\047\006\1079\006\101\006\112\006\057\006\112\006\1109\004\101\004\062\004\117\004\102"
"9\004\073\004\116\004\061\004\076\004\062\004\1149grz9\006\103\006\053\006\112\006\0619\006\103\006\052\006\047\006\050\006\0519\006\061\006\055\006\105\006\1079\006\061\006\046\006\112\006\0639\006\047\006\104\006\105"
"9\006\047\006\104\006\061\006\0579\004\101\004\062\004\076\004\070\004\1059\004\067\004\065\004\074\004\117\004\102\004\0609\003\274\003\2679\003\264\003\267\003\273\003\261\003\264\003\2569xx9prst9\011\006\011\044"
"9\006\047\006\104\006\043\006\071\006\066\006\047\006\0419\004\064\004\075\004\065\004\1019\001\133m9schn9r\001\1419l\3769dkr9\006\064\006\071\006\0619\006\056\006\047\006\061\006\0549\004\067\004\060\004\102\004\076\004\062\004\060"
"9\003\314\003\273\003\265\003\3029\003\272\003\261\003\270\003\316\003\3029wf9wdd9bh9\006\314\006\251\006\3149\006\110\006\104\006\1109\006\105\006\053\006\104\006\0479\006\105\006\047\006\060\006\0479\006\104\006\060\006\104\006\103"
"9\006\061\006\101\006\0719\006\061\006\0509\006\054\006\047\006\105\006\071\006\1079\006\047\006\104\006\065\006\110\006\061\006\0519\006\047\006\050\006\1109\004\074\004\117\004\101\004\102\004\0769\004\064\004\076\004\073\004\066\004\065\004\0750"};

unsigned char Consonants011[] = {
"9\003\300\003\301\003\316\003\304\003\2679\356n\001\1439wth9ths9sbn9czb9b\001\1319\013\261\013\2569\013\232\013\2629\006\110\006\060\006\104\006\1039\006\106\006\057\006\047\006\061\006\0579\006\105\006\314\006\064\006\110\006\057"
"9\006\062\006\106\006\057\006\257\006\3149\006\047\006\104\006\063\006\047\006\071\006\0519\006\043\006\112\006\047\006\1059\004\115\004\102\004\0609\004\102\004\065\004\074\004\0609\004\101\004\062\004\076\004\065\004\063\004\076"
"9\004\072\004\112\004\064\004\065\004\102\004\0769\004\061\004\070\004\073\004\0769\003\274\003\265\003\263\003\254\003\273\003\2679\357\001\123\3569\356\3579vm9shv8\011\005\011\027\011\0608\006\257\006\061\006\101\006\052\006\107"
"8\006\062\006\050\006\047\006\1068\006\055\006\1048\005\352\005\351\005\342\005\3248\005\352\005\350\005\321\005\325\005\3528\005\352\005\336\005\346\005\3208\005\3528\005\351\005\350\005\325\005\3378\005\351\005\342\005\350\005\331\005\335"
"8\005\351\005\333\005\3348\005\351\005\331\005\334\005\325\005\3218\005\351\005\331\005\330\005\325\005\3528\005\351\005\331\005\325\005\325\005\347\005\331\005\3528\005\351\005\325\005\3358\005\351\005\325\005\320\005\3318\005\351\005\322\005\335"
"8\005\351\005\321\005\3248\005\350\005\344\005\325\005\320\005\3318\005\350\005\342\005\331\005\325\005\3378\005\350\005\320\005\351\005\325\005\3378\005\347\005\340\005\321\005\3418\005\347\005\330\005\340\005\331\005\335"
"8\005\346\005\336\005\346\005\325\005\336\005\331\005\3358\005\346\005\321\005\3208\005\344\005\351\005\325\005\330\005\3248\005\344\005\350\005\341\005\325\005\3358\005\344\005\342\005\336\005\331\005\3358\005\344\005\340\005\322"
"8\005\344\005\327\005\325\005\3528\005\342\005\347\005\350\005\325\005\340\005\325\005\3528\005\342\005\346\005\336\005\3358\005\342\005\336\005\325\005\3378\005\342\005\325\005\336\005\3508\005\342\005\325\005\334\005\336\005\331"
"8\005\341\005\344\005\3208\005\340\005\331\005\327\005\325\005\3518\005\336\005\351\005\352\005\336\005\3518\005\336\005\351\005\340\005\3248\005\336\005\351\005\336\005\342\005\325\005\3528\005\336\005\350\005\331\005\325"
"8\005\336\005\342\005\351\005\3318\005\336\005\342\005\331\005\331\005\3378\005\336\005\341\005\342\005\323\005\325\005\3528\005\336\005\341\005\322\005\350\005\3528\005\336\005\341\005\322\005\350\005\325\005\3528\005\336\005\336\005\340\005\325"
"8\005\336\005\333\005\325\005\3378\005\336\005\331\005\334\005\3248\005\336\005\331\005\331\005\344\005\3348\005\336\005\331\005\325\005\336\005\340\005\325\005\3528\005\336\005\331\005\325\005\327\005\323"
"8\005\336\005\327\005\351\005\321\005\331\005\3358\005\336\005\324\005\331\005\3508\005\336\005\323\005\331\005\340\005\3248\005\336\005\320\005\331\005\352\005\340\005\3258\005\334\005\352\005\3528\005\334\005\351\005\331\005\327\005\324"
"8\005\334\005\350\005\320\005\3518\005\334\005\347\005\321\005\334\005\3528\005\334\005\346\005\325\005\350\005\3328\005\334\005\344\005\340\005\331\005\3328\005\334\005\336\005\342\005\3378\005\334\005\333\005\337"
"8\005\334\005\333\005\321\005\325\005\3518\005\334\005\331\005\331\005\3378\005\334\005\331\005\325\005\327\005\340\005\3378\005\334\005\324\005\341\005\323\005\3508\005\334\005\321\005\3378\005\334\005\320\005\325\005\350"
"8\005\333\005\352\005\325\005\3218\005\333\005\336\005\342\005\3308\005\333\005\334\005\351\005\324\005\3318\005\333\005\334\005\331\005\3358\005\333\005\334\005\325\005\336\005\3508\005\331\005\351\005\3508\005\331\005\344\005\340\005\331"
"8\005\331\005\333\005\325\005\334\005\3528\005\331\005\324\005\325\005\351\005\3428\005\331\005\324\005\325\005\323\005\331\005\3528\005\331\005\324\005\325\005\323\005\3248\005\330\005\347\005\341\005\330\005\331\005\335"
"8\005\330\005\347\005\341\005\3308\005\330\005\334\005\344\005\325\005\3378\005\330\005\330\005\350\005\331\005\3418\005\327\005\340\005\325\005\3528\005\327\005\325\005\323\005\3518\005\327\005\321\005\350\005\325\005\337"
"8\005\325\005\321\005\333\005\3328\005\324\005\352\005\347\005\351\005\350\005\3258\005\324\005\352\005\334\005\336\005\331\005\3238\005\324\005\352\005\326\005\325\005\340\005\3248\005\324\005\346\005\334\005\327\005\324"
"8\005\324\005\346\005\331\005\336\005\3508\005\324\005\340\005\327\005\3248\005\324\005\336\005\352\005\340\005\3248\005\324\005\336\005\327\005\331\005\3508\005\324\005\324\005\321\005\323\005\334\005\331\005\3358\005\324\005\323\005\343"
"8\005\324\005\321\005\3208\005\324\005\320\005\340\005\351\005\331\005\3358\005\324\005\320\005\331\005\351\005\331\005\3528\005\324\005\320\005\325\005\3508\005\323\005\350\005\325\005\3358\005\323\005\347\005\325\005\352"
"8\005\323\005\325\005\336\005\331\005\331\005\3378\005\323\005\325\005\320\005\3508\005\322\005\331\005\350\005\341\005\3208\005\322\005\325\005\323\005\3348\005\321\005\352\005\327\005\350\005\325\005\3528\005\321\005\352"
"8\005\321\005\350\005\351\005\325\005\3528\005\321\005\350\005\331\005\333\005\3248\005\321\005\347\005\350\005\325\005\3218\005\321\005\342\005\334\005\3528\005\321\005\342\005\331\005\3248\005\321\005\342\005\326\005\350\005\352"
"8\005\321\005\341\005\344\005\325\005\350\005\3308\005\321\005\336\005\324\005\334\005\3328\005\321\005\336\005\323\005\331\005\340\005\3248\005\321\005\334\005\352\005\3318\005\321\005\331\005\336\005\331\005\335"
"8\005\320\005\351\005\323\005\325\005\3238\005\320\005\340\005\351\005\3318\005\320\005\340\005\350\005\322\005\331\005\3248\005\320\005\336\005\3528\005\320\005\331\005\350\005\325\005\3278\005\320\005\331\005\340\005\344\005\325"
"8\005\320\005\325\005\336\005\350\005\3528\005\320\005\325\005\336\005\340\005\331\005\3358\005\320\005\325\005\333\005\3348\004\075\004\070\004\072\004\076\004\063\004\064\004\0608\003\273\003\277\003\271\003\300\003\314\003\275"
"8\003\265\003\257\003\274\003\261\003\2718ssf8spj8cwmn8\013\257\013\2628\013\237\013\2568\013\237\013\2258\006\102\006\1048\006\071\006\057\006\0518\006\047\006\104\006\065\006\055\006\112\006\055"
"8\006\047\006\104\006\052\006\063\006\054\006\112\006\1048\004\072\004\076\004\102\004\076\004\100\004\113\004\1058\004\062\004\076\004\076\004\061\004\111\004\0658r\001\1438p\001\1418pb8n\001\015n8ntsch8ndk8lpf8ggv8ckh8chs8b\001\0158\011\065\011\060"
"8\006\104\006\1068\006\103\006\106\006\0578\006\052\006\110\006\054\006\1078\006\047\006\104\006\106\006\063\006\047\006\046\006\112\006\0518\006\047\006\104\006\054\006\057\006\112\006\0578\006\047\006\057\006\047\006\105\006\107"
"8\004\076\004\075\004\0768\004\064\004\103\004\074\004\0708\004\061\004\112\004\064\004\060\004\1028\003\274\003\265\003\304\003\261\003\276\003\3158stb8\006\251\006\061\006\057\006\1068\004\101\004\072\004\060\004\067\004\060\004\102\004\114"
"8\004\064\004\100\004\103\004\063\004\076\004\0718\004\063\004\076\004\0648\003\261\003\300\003\277\003\303\003\304\003\277\003\273\003\2568\003\255\003\275\003\261\003\2758dwr8\006\251\006\064\006\110\006\0618\006\106\006\055\006\106"
"8\006\104\006\105\006\1068\006\063\006\110\006\047\006\0418\006\055\006\112\006\1068\006\054\006\047\006\0418\006\047\006\314\006\106\006\251\006\1078\006\047\006\104\006\105\006\063\006\104\006\1058\006\047\006\104\006\063\006\047\006\107\006\061"
"8\004\107\004\065\004\101\004\102\004\0768\004\072\004\112\004\101\004\075\004\0768\004\072\004\060\004\067\004\0608\003\314\003\303\003\2778rmt8nkc8\006\065\006\055\006\112\006\0558\006\056\006\112\006\047\006\061\006\047\006\0528\006\055\006\050"
"8\006\047\006\104\006\063\006\112\006\047\006\063\006\112\006\0518\006\047\006\104\006\045\006\071\006\104\006\047\006\1068\006\043\006\063\006\052\006\061\006\047\006\104\006\112\006\0478\004\102\004\060\004\072\004\070"
"8\004\076\004\064\004\075\004\060\004\072\004\0768\004\075\004\060\004\1108\003\273\003\255\003\265\003\2718\003\272\003\254\003\304\003\3118\356nv8vb8rnst8lgr8lgj8ffn8\013\265\013\264\013\2318\013\252\013\2608\013\205\013\236"
"8\004\115\004\102\004\070\004\1058\004\101\004\077\004\076\004\100\004\065\004\0648\004\101\004\073\004\076\004\062\004\0768\004\072\004\076\004\102\004\076\004\100\004\076\004\0718\004\072\004\060\004\066\004\064\004\113\004\071"
"8\004\064\004\076\004\074\004\0608\003\300\003\277\003\273\003\271\003\304\003\271\003\272\003\2568\001\123c8\356\001\1378s\373r8rfl8ndsk8lkn8htk8\011\0078\006\047\006\104\006\043\006\067\006\101\006\047\006\104"
"8\004\101\004\102\004\100\004\060\004\075\004\0608\003\300\003\254\003\275\003\304\003\2618smj8rgp8blw8\013\251\013\2608\013\205\013\2328\006\106\006\064\006\047\006\1068\006\067\006\104\006\0508\006\057\006\047\006\061"
"8\004\074\004\076\004\066\004\065\004\0748\003\300\003\254\003\273\003\2718\003\265\003\300\003\265\003\271\003\264\003\2568\001\141\001\0158rwh8k\001\1768\006\110\006\054\006\1078\006\104\006\104\006\050\006\112\006\0718\006\067\006\276"
"8\006\056\006\110\006\0508\006\056\006\104\006\1028\006\050\006\062\006\061\006\2578\006\047\006\104\006\063\006\112\006\047\006\063\006\1128\006\047\006\104\006\057\006\071\006\047\006\0418\006\043\006\106\006\110\006\047\006\071"
"8\006\043\006\105\006\0618\004\105\004\076\004\100\004\076\004\110\004\0768\004\076\004\061\004\100\004\060\004\067\004\076\004\0748\004\075\004\060\004\070\004\101\004\102\004\070\004\075\004\0608\004\072\004\076\004\073\004\072\004\076"
"8\004\062\004\074\004\065\004\101\004\102\004\0658\003\314\003\273\003\277\003\2718\001\141n8lgd8hp8fdd8cw8\011\005\011\0548\006\251\006\061\006\057\006\106\006\0578\006\110\006\066\006\0718\006\061\006\047\006\050\006\067"
"8\006\057\006\061\006\110\006\0638\006\055\006\062\006\0508\006\047\006\104\006\050\006\112\006\0528\004\101\004\100\004\065\004\111\004\1038\004\076\004\102\004\075\004\076\004\062\004\0768\003\271\003\303\003\304\003\277\003\301\003\257\003\261"
"8\003\261\003\272\003\314\003\274\003\2678m\001\1028kkr8\011\025\011\0628\006\176\006\314\006\0648\006\050\006\047\006\0618\004\101\004\064\004\065\004\073\004\060\004\102\004\1148\004\077\004\100\004\060\004\062\004\070"
"8\004\075\004\065\004\111\004\0608\004\075\004\060\004\107\004\060\004\073\004\0768\004\064\004\100\004\103\004\063\004\070\004\1058\003\274\003\300\003\277\003\301\003\265\003\257\003\304\003\2658\360v8\013\262\013\2440"};

unsigned char Consonants012[] = {
"8\006\110\006\043\006\106\006\0478\006\071\006\106\006\107\006\0478\006\056\006\104\006\047\006\112\006\0478\006\055\006\106\006\112\006\101\006\0518\006\047\006\104\006\103\006\053\006\112\006\0618\006\047\006\104\006\061\006\112\006\047\006\066"
"8\006\047\006\104\006\057\006\103\006\052\006\110\006\0618\006\047\006\104\006\052\006\1118\006\047\006\104\006\043\006\061\006\0668\006\047\006\056\006\050\006\047\006\0618\006\043\006\104\006\0478\006\043\006\103\006\050\006\061"
"8\004\101\004\076\004\104\004\070\004\1178\004\072\004\076\004\063\004\0768k\001\1418chtb8\006\206\006\106\006\314\006\1068\006\110\006\104\006\112\006\0638\006\107\006\105\006\314\006\1068\006\105\006\103\006\052\006\050\006\051"
"8\006\103\006\061\006\057\006\1078\006\071\006\050\006\0618\006\071\006\050\006\047\006\0638\006\065\006\110\006\061\006\0518\006\064\006\110\006\106\006\0578\006\056\006\112\006\061\006\0478\006\047\006\106\006\047"
"8\006\047\006\104\006\052\006\071\006\104\006\112\006\1028\006\047\006\063\006\104\006\047\006\105\006\3148\004\070\004\075\004\102\004\065\004\100\004\075\004\065\004\1028\004\062\004\065\004\101\004\1148\003\306\003\277\003\301\003\2548wz"
"8\013\256\013\2258\013\205\013\2508\006\102\006\047\006\106\006\110\006\1068\006\071\006\053\006\105\006\047\006\1068\004\115\004\102\004\1038\004\101\004\073\004\076\004\062\004\0608\004\074\004\075\004\065\004\075\004\070\004\065"
"8\004\064\004\100\004\103\004\063\004\070\004\102\004\0658\003\303\003\300\003\257\003\304\003\2718stg8rskj8rdl8lbr8ftl8ckgr8\006\110\006\043\006\1068\006\104\006\043\006\106\006\1078\006\102\006\104\006\0528\006\065\006\047\006\055\006\050"
"8\006\047\006\105\006\061\006\110\006\0628\006\047\006\104\006\065\006\055\006\047\006\101\006\0518\006\047\006\104\006\043\006\056\006\067\006\047\006\0418\006\047\006\063\006\104\006\047\006\1058\006\045\006\104\006\107"
"8\006\043\006\071\006\104\006\047\006\1058\006\043\006\056\006\1128\006\043\006\055\006\103\006\047\006\1058\006\042\006\057\006\1058\004\102\004\065\004\0618\004\102\004\060\004\072\004\070\004\062\004\060"
"8\004\075\004\070\004\072\004\076\004\063\004\0608\004\067\004\075\004\060\004\107\004\070\004\1028\003\265\003\257\003\303\003\261\003\2718\003\265\003\257\003\274\003\261\003\303\003\304\003\2658\357\001\1238nx8msc8bstr8\013\244\013\265\013\260"
"8\013\207\013\2638\006\257\006\060\006\064\006\052\006\1078\006\105\006\066\006\047\006\1018\006\102\006\057\006\061\006\0528\006\071\006\104\006\112\006\103\006\1058\006\061\006\063\006\047\006\046\006\104"
"8\006\047\006\104\006\104\006\072\006\047\006\0528\006\047\006\104\006\102\006\066\006\047\006\0418\006\047\006\104\006\101\006\071\006\1048\006\047\006\104\006\055\006\102\006\110\006\1028\004\101\004\062\004\076\004\117"
"8\004\061\004\070\004\073\004\0708\003\316\003\301\003\2618\003\270\003\255\003\274\003\261\003\304\003\2618\001\015t8yr\3608v\356nt8tspr8rkg8hwnnw8\013\256\013\2378\011\056\011\0628\006\066\006\105\006\1068\006\055\006\101\006\070"
"8\006\050\006\314\006\1068\006\050\006\061\006\106\006\047\006\105\006\1078\004\100\004\065\004\072\004\073\004\060\004\074\004\0608\004\062\004\074\004\065\004\101\004\102\004\0768\003\305\003\300\003\254\003\301\003\307\003\277\003\305\003\275"
"8\003\274\003\300\003\277\003\301\003\3168\003\265\003\254\003\2758llf8dtr8\014\270\014\2278\014\267\014\2508\014\267\014\2438\014\266\014\2468\014\266\014\227\014\263\014\2508\014\265\014\2658\014\265\014\2638\014\265\014\2628\014\265\014\257"
"8\014\265\014\234\014\2508\014\265\014\2348\014\262\014\2468\014\260\014\2658\014\260\014\246\014\2608\014\256\014\2608\014\254\014\262\014\2658\014\254\014\2608\014\252\014\2448\014\247\014\2508\014\2478\014\246\014\2468\014\244\014\244"
"8\014\244\014\2258\014\237\014\2508\014\237\014\2468\014\234\014\225\014\2448\014\232\014\260\014\2438\014\227\014\2708\014\227\014\2628\014\225\014\2508\014\222\014\2448\014\220\014\2528\014\211\014\2638\014\207\014\2508\014\207\014\246\014\250"
"8\014\207\014\246\014\2258\014\2078\014\205\014\2628\014\205\014\2528\014\205\014\2478\011\006\011\0348\006\105\006\110\006\063\006\110\006\071\006\0518\006\104\006\106\006\0478\006\067\006\061\006\112\006\102\006\0518\006\067\006\047\006\104\006\050"
"8\006\063\006\110\006\061\006\112\006\0478\006\063\006\047\006\071\006\0528\006\061\006\110\006\047\006\1078\006\057\006\110\006\1058\006\056\006\057\006\047\006\110\006\106\006\0578\006\052\006\063\006\052\006\067\006\112\006\071"
"8\006\047\006\104\006\105\006\105\006\104\006\103\006\0518\006\047\006\104\006\102\006\057\006\0618\006\047\006\104\006\054\006\063\006\1058\006\047\006\071\006\104\006\047\006\106\006\047\006\052"
"8\006\047\006\055\006\065\006\047\006\046\006\112\006\047\006\0528\005\352\005\347\005\340\005\325\005\3378\005\352\005\347\005\325\005\344\005\3528\005\352\005\340\005\320\005\3318\005\352\005\331\005\347\005\325"
"8\005\352\005\331\005\333\005\325\005\3378\005\352\005\327\005\3528\005\352\005\325\005\346\005\320\005\325\005\3528\005\352\005\324\005\331\005\3248\005\351\005\334\005\336\005\3248\005\351\005\334\005\325\005\327\005\324"
"8\005\351\005\331\005\340\005\331\005\331\005\3358\005\351\005\331\005\340\005\325\005\3318\005\351\005\325\005\3478\005\351\005\322\005\331\005\320\005\325\005\3528\005\351\005\320\005\334\005\3248\005\351\005\320\005\325\005\334"
"8\005\350\005\336\005\3528\005\347\005\330\005\322\005\325\005\350\005\331\005\325\005\3528\005\347\005\321\005\334\005\3528\005\347\005\321\005\325\005\346\005\3528\005\346\005\350\005\331\005\333\005\331\005\335"
"8\005\346\005\331\005\331\005\350\005\331\005\3358\005\346\005\331\005\325\005\3508\005\346\005\3248\005\344\005\350\005\346\005\325\005\344\005\331\005\3358\005\344\005\350\005\330\005\331\005\3528\005\344\005\350\005\330\005\331\005\335"
"8\005\344\005\350\005\325\005\3448\005\344\005\341\005\325\005\347\005\331\005\3358\005\344\005\340\005\331\005\3358\005\342\005\351\005\3258\005\342\005\350\005\3328\005\342\005\334\005\3248\005\342\005\331\005\3378\005\342\005\325\005\336\005\323"
"8\005\342\005\325\005\321\005\323\005\331\005\3358\005\342\005\323\005\3378\005\342\005\323\005\333\005\325\005\3378\005\341\005\350\005\330\005\331\005\3358\005\341\005\334\005\347\005\325\005\3358\005\341\005\331\005\344\005\325\005\350"
"8\005\341\005\3248\005\340\005\3418\005\340\005\336\005\346\005\3208\005\340\005\331\005\352\005\325\005\327\005\3318\005\336\005\347\005\346\005\325\005\342\005\331\005\3528\005\336\005\347\005\325\005\325\005\324"
"8\005\336\005\346\005\334\005\331\005\327\005\331\005\3358\005\336\005\346\005\325\005\325\005\3528\005\336\005\342\005\334\005\3248\005\336\005\341\005\3328\005\336\005\334\005\327\005\336\005\3528\005\336\005\331\005\334\005\331\005\335"
"8\005\336\005\325\005\342\005\323\005\325\005\3378\005\336\005\324\005\336\005\327\005\351\005\3218\005\336\005\324\005\3258\005\336\005\321\005\346\005\342\005\331\005\3358\005\336\005\320\005\341\005\330\005\3508\005\336\005\320\005\325\005\352"
"8\005\334\005\352\005\327\005\350\005\325\005\3528\005\334\005\351\005\331\005\344\005\325\005\3508\005\334\005\350\005\333\005\325\005\3518\005\334\005\347\005\321\005\325\005\3428\005\334\005\344\005\352\005\325\005\350"
"8\005\334\005\344\005\342\005\336\005\331\005\3358\005\334\005\341\005\344\005\3478\005\334\005\340\005\346\005\3278\005\334\005\334\005\336\005\325\005\3238\005\334\005\333\005\3328\005\334\005\331\005\342\005\334"
"8\005\334\005\331\005\336\005\325\005\323\005\3318\005\334\005\331\005\3238\005\334\005\327\005\326\005\325\005\3508\005\334\005\326\005\3248\005\334\005\324\005\352\005\347\005\351\005\3508\005\334\005\324\005\351\005\344\005\331\005\342"
"8\005\334\005\324\005\346\005\334\005\327\005\3248\005\334\005\324\005\336\005\351\005\331\005\3328\005\334\005\324\005\331\005\336\005\340\005\3428\005\334\005\323\005\325\005\322\005\336\005\3208\005\334\005\320\005\344\005\351\005\350"
"8\005\334\005\320\005\325\005\350\005\3328\005\333\005\352\005\321\005\325\005\3528\005\333\005\334\005\3348\005\333\005\331\005\325\005\3358\005\333\005\324\005\3378\005\333\005\3248\005\331\005\346\005\331\005\350\005\324"
"8\005\331\005\324\005\325\005\323\005\3318\005\330\005\331\005\344\005\331\005\3358\005\330\005\331\005\325\005\3318\005\327\005\351\005\336\005\3348\005\327\005\351\005\325\005\321\005\3248\005\327\005\351\005\321\005\325\005\337"
"8\005\327\005\347\005\331\005\350\005\325\005\3528\005\327\005\325\005\351\005\3218\005\327\005\325\005\336\005\3508\005\327\005\325\005\321\005\350\005\325\005\3528\005\326\005\336\005\340\005\3328\005\325\005\351\005\334"
"8\005\325\005\344\005\352\005\350\005\325\005\340\005\325\005\3528\005\325\005\341\005\331\005\344\005\325\005\350\005\331\005\3358\005\325\005\324\005\351\005\333\005\340\005\325\005\3428\005\325\005\324\005\325\005\320"
"8\005\324\005\352\005\347\005\340\005\3248\005\324\005\352\005\342\005\351\005\331\005\331\005\3248\005\324\005\352\005\325\005\333\005\340\005\331\005\3528\005\324\005\352\005\325\005\333\005\3378\005\324\005\351\005\340\005\331"
"8\005\324\005\351\005\331\005\327\005\3248\005\324\005\350\005\344\005\352\005\347\005\320\005\325\005\3528\005\324\005\350\005\342\005\331\005\325\005\3378\005\324\005\3508\005\324\005\346\005\3238\005\324\005\342\005\350\005\321"
"8\005\324\005\342\005\321\005\3508\005\324\005\336\005\352\005\331\005\3358\005\324\005\336\005\351\005\350\005\3238\005\324\005\336\005\350\005\346\005\3248\005\324\005\336\005\347\005\325\005\335"
"8\005\324\005\336\005\333\005\331\005\350\005\325\005\3528\005\324\005\336\005\330\005\350\005\3248\005\324\005\336\005\325\005\3378\005\324\005\336\005\323\005\331\005\340\005\3248\005\324\005\333\005\331\005\321\005\325\005\351"
"8\005\324\005\333\005\321\005\325\005\3238\005\324\005\331\005\333\005\325\005\334\005\3528\005\324\005\330\005\333\005\340\005\331\005\347\005\325\005\3528\005\324\005\327\005\351\005\3478\005\324\005\327\005\321\005\350\005\331\005\335"
"8\005\324\005\325\005\323\005\342\005\325\005\3528\005\324\005\324\005\331\005\336\005\325\005\350\005\331\005\3358\005\324\005\323\005\344\005\341\005\3248\005\324\005\323\005\321\005\3508\005\324\005\321\005\342\005\331\005\3240"};

unsigned char Consonants013[] = {
"8\005\324\005\320\005\340\005\350\005\322\005\331\005\3248\005\324\005\320\005\326\005\331\005\340\005\3258\005\323\005\350\005\325\005\351\005\331\005\3358\005\323\005\325\005\325\005\347\005\3208\005\322\005\331\005\351\005\324"
"8\005\322\005\331\005\3348\005\321\005\351\005\3358\005\321\005\350\005\351\005\3528\005\321\005\3508\005\321\005\342\005\3528\005\321\005\340\005\325\005\3528\005\321\005\336\005\352\005\340\005\3248\005\321\005\336\005\342\005\350\005\333\005\352"
"8\005\321\005\336\005\333\005\331\005\350\005\325\005\3528\005\321\005\336\005\331\005\325\005\327\005\3238\005\321\005\336\005\324\005\331\005\350\005\325\005\3528\005\321\005\333\005\3328\005\321\005\333\005\323\005\331"
"8\005\321\005\331\005\342\005\331\005\334\005\325\005\3528\005\321\005\331\005\3258\005\321\005\327\005\350\005\3258\005\321\005\327\005\321\005\350\005\3248\005\321\005\322\005\334\005\3348\005\321\005\321\005\342\005\331\005\324"
"8\005\321\005\320\005\331\005\334\005\3528\005\320\005\351\005\333\005\325\005\3348\005\320\005\350\005\321\005\342\005\331\005\3358\005\320\005\346\005\3348\005\320\005\344\005\331\005\334\005\325"
"8\005\320\005\341\005\330\005\350\005\330\005\322\005\331\005\3248\005\320\005\340\005\347\005\323\005\325\005\330\005\325\005\3528\005\320\005\336\005\350\005\331\005\347\005\3248\005\320\005\336\005\340\005\325\005\3528\005\320\005\334\005\325"
"8\005\320\005\325\005\350\005\3278\005\320\005\325\005\3508\005\320\005\325\005\324\005\321\005\331\005\3358\005\320\005\322\005\350\005\3528\005\320\005\321\005\350\005\3358\004\075\004\060\004\067\004\060\004\064"
"8\003\304\003\301\003\314\003\300\003\2778\003\300\003\257\003\303\003\3118\003\265\003\272\003\272\003\273\003\267\003\303\003\257\003\2618\003\263\003\257\003\275\003\265\003\304\003\261\003\2718p\001\141t8nschl8mtr8chd8\011\005\011\047"
"8\006\110\006\103\006\047\006\106\006\0528\006\110\006\045\006\060\006\0478\006\105\006\056\006\066\006\061\006\1058\006\101\006\104\006\047\006\1068\006\101\006\045\006\060\006\0478\006\057\006\061\006\050\006\047\006\061\006\107"
"8\006\047\006\104\006\0638\006\047\006\104\006\043\006\104\006\050\006\047\006\106\006\1128\004\077\004\100\004\060\004\062\004\0608\004\070\004\074\004\060\004\1028\004\070\004\061\004\0768\004\065\004\102\004\076"
"8\004\062\004\101\004\117\004\072\004\0608\003\274\003\2748\003\261\003\300\003\254\003\275\003\304\003\267\003\303\003\2678mbn8\006\057\006\1078\006\050\006\047\006\0508\004\115\004\102\004\070\004\0748\004\076\004\101\004\062\004\065\004\075"
"8\004\063\004\076\004\062\004\076\004\100\004\117\004\1028\003\300\003\275\003\265\003\315\003\274\003\2618\001\174cz8\356m8ymp8wrz8sks8llv8jnl8\013\205\013\2628\013\205\013\2378\011\025\011\071\011\0448\011\025\011\056"
"8\006\110\006\047\006\055\006\057\006\0518\006\107\006\314\006\2068\006\054\006\106\006\2578\006\047\006\104\006\055\006\063\006\1068\006\047\006\104\006\054\006\107\006\047\006\0628\006\047\006\104\006\050\006\052\006\107"
"8\006\047\006\104\006\043\006\102\006\063\006\047\006\1058\006\047\006\050\006\055\006\0538\004\117\004\101\004\075\004\0768\004\101\004\060\004\074\004\0708\004\076\004\061\004\111\004\065\004\101\004\102\004\062\004\076"
"8\004\074\004\070\004\100\004\0608\004\0668\004\062\004\100\004\065\004\074\004\065\004\102\004\0768\003\303\003\304\003\271\003\263\003\274\003\2568\003\300\003\277\003\304\003\2558\003\277\003\274\003\254\003\264\003\261"
"8\003\272\003\314\003\303\003\274\003\2778\003\272\003\261\003\273\003\2548\003\264\003\265\003\257\003\304\003\2658\003\261\003\306\003\277\003\3158ntd8jstv8jnd8chtn8\013\252\013\2438\013\252\013\237\013\2448\011\011\011\070\011\025"
"8\006\107\006\112\006\1058\006\105\006\105\006\112\006\0628\006\104\006\060\006\0478\006\071\006\0628\006\053\006\104\006\047\006\0538\006\050\006\047\006\064\006\106\006\0578\006\047\006\104\006\104\006\112"
"8\006\047\006\104\006\064\006\050\006\047\006\0508\006\047\006\104\006\057\006\071\006\110\006\0518\006\047\006\104\006\050\006\106\006\047\006\0528\006\043\006\056\006\0618\003\265\003\275\003\314\003\3028\003\254\003\273\003\273\003\2678hh"
"8\006\064\006\061\006\0558\006\052\006\107\006\061\006\047\006\1068\006\042\006\106\006\206\006\1078\004\107\004\060\004\101\004\102\004\1148\004\101\004\070\004\101\004\102\004\065\004\074\004\0608\004\073\004\116\004\061\004\076\004\062"
"8\004\072\004\060\004\067\004\062\004\0608\004\063\004\073\004\060\004\062\004\0608\003\263\003\273\003\316\003\303\003\303\003\2618\003\261\003\275\003\316\003\275\003\305\003\274\003\277\003\3028ysgr8xxx8vh8trd8rvj8llk8fts8\006\176\006\112\006\064"
"8\006\101\006\110\006\047\006\046\006\0578\006\071\006\104\006\112\006\107\006\1058\006\071\006\047\006\105\006\0518\006\055\006\066\006\061\006\0528\006\047\006\104\006\105\006\104\006\1038\006\045\006\063\006\050\006\047\006\106\006\112\006\047"
"8\004\100\004\060\004\067\004\062\004\070\004\102\004\070\004\0658\004\074\004\065\004\101\004\102\004\0768\004\070\004\101\004\072\004\060\004\0748\004\067\004\060\004\072\004\076\004\0758\003\261\003\305\003\304\003\255\003\3028\001\341"
"8\357\001\123\3578wst8vnt8rbj8nwr8mchw8\011\062\011\0418\011\046\011\0608\011\0238\006\110\006\103\006\060\006\104\006\1038\006\104\006\057\006\1118\006\103\006\047\006\101\006\0518\006\055\006\063\006\0508\006\050\006\060\006\104\006\103"
"8\006\050\006\055\006\110\006\0538\006\047\006\106\006\102\006\104\006\047\006\0508\006\047\006\104\006\065\006\101\006\055\006\047\006\0528\006\047\006\104\006\063\006\112\006\047\006\061\006\047\006\052"
"8\006\047\006\104\006\050\006\061\006\106\006\047\006\105\006\0548\006\045\006\104\006\112\006\107\006\0478\006\043\006\056\006\050\006\047\006\0618\004\101\004\073\004\103\004\107\004\060\004\0658\004\101\004\070\004\073\004\060"
"8\004\064\004\100\004\103\004\063\004\070\004\0658\004\064\004\076\004\0748\004\063\004\076\004\073\004\117\004\074\004\0608\003\273\003\257\003\263\003\2778\003\272\003\254\003\275\003\277\003\305\003\2758tkl8ssl8\013\262\013\2568\011\005\011\050"
"8\006\257\006\101\006\052\006\1078\006\107\006\105\006\112\006\1068\006\042\006\0508\004\101\004\076\004\061\004\076\004\0718\004\075\004\065\004\0658\004\075\004\060\004\110\004\065\004\0718\004\074\004\070\004\100\004\065"
"8\004\073\004\103\004\107\004\110\004\0658\004\063\004\076\004\101\004\077\004\076\004\0648\003\277\003\304\003\2718\003\2758\003\257\003\264\003\271\003\2618\001\174k8ynd8t\3738rkj8dbz8\013\237\013\2318\006\112\006\102\006\110\006\105"
"8\006\110\006\047\006\104\006\052\006\1128\006\105\006\061\006\103\006\0628\006\103\006\060\006\104\006\1038\006\047\006\104\006\105\006\104\006\101\006\047\006\0528\006\047\006\104\006\103\006\104\006\047\006\105"
"8\006\047\006\104\006\050\006\104\006\047\006\0578\006\045\006\061\006\063\006\047\006\1048\004\101\004\062\004\076\004\0658\004\076\004\102\004\065\004\1068\004\074\004\060\004\0718\004\072\004\076\004\102\004\076\004\100\004\076\004\065"
"8\004\063\004\076\004\101\004\077\004\076\004\064\004\070\004\0758\003\265\003\257\003\307\003\261\003\2758\003\264\003\271\003\314\003\304\003\2718v\371l8tzl8thf8skrb8n\001\1768lstv8llsch8\011\007\011\0508\006\251\006\063\006\314"
"8\006\103\006\047\006\0618\006\101\006\251\006\0618\006\064\006\047\006\0418\006\063\006\314\006\047\006\063\006\3148\006\053\006\104\006\047\006\053\006\0518\006\050\006\047\006\104\006\106\006\063\006\050\006\051"
"8\006\047\006\104\006\105\006\102\006\047\006\104\006\047\006\0528\006\047\006\104\006\103\006\110\006\112\006\0528\006\047\006\104\006\102\006\104\006\110\006\0508\006\047\006\104\006\071\006\104\006\110\006\105"
"8\006\047\006\104\006\061\006\063\006\110\006\1048\006\047\006\104\006\056\006\104\006\112\006\0548\006\047\006\104\006\054\006\062\006\112\006\061\006\0518\006\047\006\104\006\052\006\047\006\104\006\112\006\051"
"8\006\047\006\101\006\104\006\047\006\1058\006\045\006\0608\006\042\006\106\006\047\006\1068\004\102\004\100\004\103\004\064\004\075\004\0768\004\102\004\060\004\072\004\070\004\0658\004\072\004\075\004\070\004\063\004\0608\004\062\004\070\004\064"
"8\003\300\003\277\003\273\003\273\003\2548\003\265\003\273\003\273\003\267\003\275\003\271\003\272\003\2568\003\254\003\273\003\273\003\2618\353n8tgl8glw8ggr8\013\244\013\2258\006\251\006\106\006\314\006\1058\006\050\006\063\006\314\006\047\006\061"
"8\006\050\006\061\006\047\006\1118\004\062\004\075\004\070\004\074\004\060\004\075\004\070\004\0658\003\270\003\265\003\314\003\3028\003\270\003\265\003\277\003\3158\003\265\003\271\003\275\003\261\003\2718\003\257\003\264\003\271\003\277"
"8\003\255\003\263\003\271\003\275\003\2658rnt8ngf8htn8\006\110\006\102\006\052\006\3148\006\103\006\110\006\112\006\0528\006\057\006\110\006\063\006\0528\006\047\006\104\006\065\006\110\006\1058\006\047\006\104\006\043\006\071\006\105\006\047\006\104"
"8\006\045\006\106\006\1128\006\043\006\053\006\106\006\047\006\0418\004\107\004\060\004\101\004\0608\004\074\004\065\004\101\004\102\004\0608\004\073\004\116\004\061\004\062\004\0708\004\067\004\060\004\072\004\076\004\075\004\0608yh8rchw8lvf8gwl"
"8\011\071\011\0448\011\052\011\0428\011\030\011\0608\006\257\006\061\006\110\006\1078\006\105\006\106\006\107\006\1058\006\102\006\057\006\0618\006\101\006\105\006\0478\006\071\006\104\006\3148\006\071\006\050\006\057\006\047\006\104\006\104\006\107"
"8\006\063\006\110\006\1018\006\061\006\110\006\047\006\050\006\0678\006\057\006\047\006\1058\006\052\006\0618\006\047\006\104\006\105\006\057\006\110\006\106\006\047\006\0528\006\047\006\104\006\057\006\112\006\106\006\112\006\051"
"8\004\101\004\102\004\060\004\0738\004\072\004\076\004\073\004\072\004\076\004\102\004\0768\004\064\004\076\004\101\004\102\004\0608\004\063\004\076\004\064\004\1038\004\063\004\076\004\062\004\076\004\100\004\070\004\102\004\114"
"8\003\263\003\257\003\275\003\265\003\2718ysk8kstr8kkj8jnt8jj8dns8\013\256\013\2628\013\211\013\2318\011\056\011\0718\006\063\006\050\006\0508\006\055\006\052\006\3148\004\101\004\062\004\076\004\070\004\102\004\0658\004\077\004\112\004\102\004\070"
"8\004\076\004\064\004\075\004\076\004\0718\004\075\004\070\004\072\004\076\004\0718\004\072\004\100\004\076\004\074\004\0658\004\061\004\070\004\067\004\075\004\065\004\1018\356mpr8ntj8m\001\1768lcs8hns8dsj8\006\110\006\107\006\060\006\1070"};

unsigned char Consonants014[] = {
"8\006\105\006\105\006\104\006\103\006\0518\006\105\006\050\006\047\006\064\006\061\006\0518\006\105\006\047\006\061\006\0638\006\102\006\065\006\0518\006\101\006\052\006\061\006\0518\006\063\006\104\006\063\006\104\006\051"
"8\006\047\006\104\006\105\006\061\006\112\006\0668\006\047\006\104\006\103\006\052\006\047\006\050\006\0518\006\047\006\104\006\102\006\105\006\0618\006\047\006\104\006\071\006\064\006\0618\006\047\006\104\006\071\006\061\006\047\006\102"
"8\006\047\006\104\006\057\006\110\006\1048\006\047\006\104\006\057\006\106\006\112\006\0478\006\047\006\104\006\056\006\047\006\0658\006\047\006\104\006\047\006\067\006\101\006\047\006\1048\004\070\004\074\004\065\004\065\004\102"
"8\004\064\004\103\004\110\004\0708\004\063\004\076\004\101\004\077\004\076\004\064\004\0608\003\3048\003\265\003\257\003\300\003\2658\001\172n8z\001\1318rschr8rldw8nvr8d\3548dth8\013\232\013\2518\006\251\006\061\006\057\006\105"
"8\006\110\006\050\006\071\006\0578\006\110\006\047\006\104\006\065\006\110\006\047\006\0508\006\107\006\112\006\2068\006\107\006\063\006\0528\006\103\006\104\006\047\006\1058\006\103\006\050\006\112\006\0618\006\101\006\103\006\061\006\051"
"8\006\101\006\045\006\106\006\1078\006\071\006\104\006\105\006\047\006\0418\006\055\006\066\006\110\006\0618\006\055\006\063\006\112\006\1068\006\055\006\047\006\104\006\0518\006\052\006\071\006\104\006\112\006\105"
"8\006\050\006\045\006\064\006\061\006\047\006\1018\006\047\006\104\006\045\006\063\006\104\006\047\006\105\006\1128\006\047\006\063\006\052\006\102\006\104\006\047\006\1048\006\042\006\105\006\057\006\1078\004\075\004\070\004\072\004\102\004\076"
"8\004\073\004\070\004\106\004\0658\004\064\004\100\004\103\004\063\004\0768\003\2738\003\255\003\272\003\264\003\277\003\303\003\2678zbr8wsk8sth8rtg8rkr8n\3768ntspr8ncj8lsch8llwch8llts8htj8brn8\011\070\011\0258\011\044\011\050"
"8\006\106\006\063\006\050\006\0528\006\102\006\104\006\0508\006\056\006\314\006\104\006\3148\006\047\006\101\006\061\006\047\006\0578\006\047\006\053\006\0618\004\0738\004\070\004\075\004\060\004\107\004\0658\004\064\004\103\004\074\004\060"
"8\004\062\004\076\004\073\004\1178\003\265\003\271\003\272\003\314\003\275\003\265\003\3028\337ts8wk8vstv8ntm8ngj8gtv8\011\052\011\0508\011\007\011\044\011\0508\006\112\006\1068\006\105\006\047\006\106\006\106\006\057"
"8\006\104\006\105\006\047\006\060\006\0478\006\103\006\047\006\106\006\110\006\0478\006\063\006\050\006\047\006\1028\006\063\006\047\006\062\006\105\006\047\006\1068\006\054\006\047\006\105\006\071\006\051"
"8\006\052\006\101\006\047\006\065\006\112\006\1048\006\050\006\104\006\0478\006\050\006\047\006\061\006\1038\006\047\006\314\006\061\006\047\006\106\006\3148\006\047\006\106\006\1078\006\047\006\104\006\063\006\071\006\110\006\057\006\112"
"8\006\047\006\0648\005\352\005\351\005\334\005\325\005\3358\005\352\005\347\005\325\005\344\005\3248\005\352\005\344\005\325\005\346\005\3248\005\352\005\336\005\351\005\331\005\3328\005\352\005\333\005\340\005\331\005\352"
"8\005\352\005\327\005\350\005\325\005\352\005\3318\005\352\005\325\005\333\005\340\005\3528\005\352\005\325\005\333\005\334\005\3258\005\351\005\350\005\3478\005\351\005\346\005\350\005\331\005\3328\005\351\005\340\005\347\005\321\005\342"
"8\005\351\005\336\005\325\005\3528\005\351\005\334\005\336\005\331\005\3358\005\351\005\334\005\321\005\331\005\3358\005\351\005\331\005\352\005\325\005\344\005\3318\005\351\005\331\005\352\005\325\005\3438\005\351\005\331\005\351\005\331"
"8\005\351\005\331\005\350\005\325\005\352\005\3318\005\351\005\331\005\336\005\3258\005\351\005\331\005\330\005\3528\005\351\005\331\005\330\005\3248\005\351\005\327\005\325\005\3508\005\351\005\325\005\352\005\344\005\331\005\335"
"8\005\351\005\321\005\3258\005\350\005\351\005\325\005\336\005\331\005\3358\005\350\005\336\005\325\005\3528\005\350\005\331\005\322\005\325\005\3348\005\350\005\326\005\3248\005\350\005\323\005\331\005\3258\005\350\005\320\005\325"
"8\005\347\005\351\005\3248\005\347\005\350\005\340\005\325\005\3528\005\347\005\340\005\331\005\325\005\3528\005\347\005\334\005\344\005\331\005\3358\005\347\005\331\005\351\005\325\005\3508\005\347\005\331\005\331\005\336\005\331\005\335"
"8\005\347\005\325\005\350\005\3208\005\347\005\325\005\326\005\3318\005\347\005\325\005\323\005\3358\005\346\005\344\005\3258\005\346\005\342\005\3238\005\346\005\334\005\336\005\331\005\3358\005\346\005\331\005\321\005\325\005\350"
"8\005\344\005\352\005\325\005\3278\005\344\005\342\005\331\005\334\005\325\005\3528\005\344\005\340\005\331\005\325\005\3528\005\344\005\325\005\341\005\330\005\350\005\331\005\3358\005\344\005\325\005\340\005\3248\005\344\005\320\005\326\005\334"
"8\005\342\005\351\005\352\005\325\005\340\005\325\005\3528\005\342\005\351\005\350\005\325\005\3528\005\342\005\347\005\3218\005\342\005\340\005\3478\005\342\005\334\005\331\005\340\005\3258\005\342\005\334\005\331\005\333\005\335"
"8\005\342\005\331\005\3508\005\342\005\325\005\321\005\3238\005\342\005\321\005\325\005\350\005\333\005\3358\005\342\005\321\005\325\005\323\005\3528\005\341\005\352\005\3358\005\341\005\350\005\330\005\331"
"8\005\341\005\336\005\333\005\325\005\331\005\325\005\3528\005\341\005\323\005\325\005\3358\005\3418\005\340\005\352\005\352\005\3318\005\340\005\352\005\331\005\3218\005\340\005\347\005\350\005\320\005\3528\005\340\005\347\005\325\005\323\005\352"
"8\005\340\005\344\005\334\005\3208\005\340\005\341\005\331\005\325\005\3378\005\340\005\331\005\331\005\3508\005\340\005\325\005\3438\005\340\005\324\005\331\005\322\005\3248\005\340\005\324\005\323\005\350\005\3528\005\340\005\321\005\340\005\324"
"8\005\336\005\352\005\347\005\323\005\336\005\3528\005\336\005\352\005\347\005\323\005\3358\005\336\005\352\005\3318\005\336\005\352\005\327\005\331\005\334\005\331\005\3358\005\336\005\351\005\350\005\3238\005\336\005\351\005\344\005\330"
"8\005\336\005\347\005\346\005\325\005\3428\005\336\005\346\005\331\005\3428\005\336\005\344\005\322\005\3518\005\336\005\341\005\344\005\331\005\3478\005\336\005\341\005\3428\005\336\005\340\005\324\005\334\005\331\005\335"
"8\005\336\005\336\005\334\005\331\005\346\005\331\005\3358\005\336\005\336\005\3328\005\336\005\331\005\351\005\324\005\3258\005\336\005\331\005\3238\005\336\005\330\005\350\005\325\005\3528\005\336\005\327\005\351\005\321"
"8\005\336\005\327\005\334\005\347\005\3528\005\336\005\327\005\331\005\350\005\325\005\3378\005\336\005\325\005\352\005\3258\005\336\005\325\005\327\005\334\005\3308\005\336\005\325\005\323\005\342\005\331\005\335"
"8\005\336\005\324\005\336\005\350\005\333\005\3268\005\336\005\323\005\325\005\321\005\3508\005\336\005\322\005\331\005\342\005\331\005\3358\005\336\005\321\005\334\005\3318\005\336\005\320\005\327\005\325\005\350\005\331"
"8\005\334\005\352\005\336\005\325\005\340\005\3248\005\334\005\352\005\325\005\333\005\340\005\331\005\3528\005\334\005\351\005\340\005\325\005\3528\005\334\005\350\005\322\005\3348\005\334\005\347\005\340\005\325\005\352"
"8\005\334\005\347\005\327\005\3528\005\334\005\346\005\336\005\331\005\327\005\3248\005\334\005\346\005\331\005\331\005\3378\005\334\005\344\005\330\005\350\005\325\005\3418\005\334\005\344\005\325\005\350\005\325\005\335"
"8\005\334\005\342\005\346\005\336\005\3328\005\334\005\342\005\325\005\336\005\3528\005\334\005\342\005\321\005\325\005\3508\005\334\005\341\005\3348\005\334\005\341\005\331\005\331\005\3428\005\334\005\336\005\334\005\3208\005\334\005\336\005\331"
"8\005\334\005\334\005\347\005\325\005\3278\005\334\005\333\005\352\005\325\005\3218\005\334\005\333\005\325\005\334\005\3358\005\334\005\330\005\344\005\3348\005\334\005\330\005\331\005\344\005\325\005\3348\005\334\005\327\005\351\005\325\005\321"
"8\005\334\005\327\005\344\005\3518\005\334\005\326\005\333\005\325\005\3508\005\334\005\326\005\325\005\322\005\325\005\3528\005\334\005\324\005\341\005\321\005\331\005\3508\005\334\005\324\005\334\005\337"
"8\005\334\005\324\005\327\005\334\005\331\005\3308\005\334\005\324\005\326\005\336\005\331\005\3378\005\334\005\324\005\325\005\341\005\331\005\3438\005\334\005\324\005\322\005\331\005\3238\005\334\005\324\005\322\005\323\005\331\005\334"
"8\005\334\005\322\005\334\005\325\005\3528\005\334\005\320\005\350\005\346\005\325\005\3528\005\334\005\320\005\340\005\351\005\331\005\3358\005\334\005\320\005\336\005\3378\005\333\005\341\005\3438\005\333\005\336\005\325\005\352"
"8\005\333\005\336\005\325\005\321\005\3378\005\333\005\331\005\350\005\325\005\350\005\322\005\331\005\3248\005\333\005\326\005\3248\005\333\005\323\005\320\005\3318\005\333\005\322\005\331\005\351\005\3248\005\333\005\320\005\334\005\324"
"8\005\331\005\352\005\3508\005\331\005\351\005\340\005\3248\005\331\005\346\005\327\005\3478\005\331\005\344\005\3248\005\331\005\336\005\331\005\3358\005\331\005\336\005\3318\005\331\005\327\005\341\005\331\005\3358\005\331\005\325\005\341\005\343"
"8\005\331\005\325\005\334\005\3318\005\331\005\325\005\327\005\340\005\3378\005\331\005\324\005\331\005\3258\005\331\005\3228\005\330\005\333\005\340\005\331\005\3528\005\330\005\325\005\321\005\331\005\3358\005\330\005\320\005\331"
"8\005\327\005\350\005\323\005\3528\005\327\005\336\005\3518\005\327\005\331\005\3318\005\327\005\325\005\347\005\331\005\3528\005\327\005\325\005\336\005\350\005\331\005\3358\005\327\005\325\005\323\005\351\005\331\005\335"
"8\005\327\005\321\005\350\005\3248\005\327\005\321\005\3348\005\326\005\331\005\333\005\350\005\325\005\3378\005\325\005\347\005\321\005\346\005\331\005\3358\005\325\005\344\005\350\005\341\005\325\005\3358\005\325\005\342\005\323"
"8\005\325\005\336\005\325\005\320\005\3218\005\325\005\334\005\347\005\330\005\325\005\350\005\3248\005\325\005\334\005\331\005\323\005\3248\005\325\005\333\005\3348\005\325\005\333\005\331\005\346\005\3238\005\325\005\331\005\323\005\320\005\325"
"8\005\325\005\330\005\331\005\344\005\331\005\3358\005\325\005\324\005\3358\005\325\005\320\005\340\005\351\005\3318\005\324\005\352\005\347\005\351\005\325\005\350\005\3528\005\324\005\352\005\334\005\325\005\340\005\324"
"8\005\324\005\352\005\325\005\350\005\3248\005\324\005\352\005\325\005\346\005\320\005\325\005\3528\005\324\005\352\005\322\005\325\005\321\005\325\005\3528\005\324\005\351\005\344\005\342\005\3528\005\324\005\351\005\331\005\336\005\325\005\3510"};

unsigned char Consonants015[] = {
"8\005\324\005\351\005\331\005\330\005\3248\005\324\005\350\005\351\005\3528\005\324\005\350\005\351\005\336\005\3248\005\324\005\350\005\344\005\325\005\320\005\3318\005\324\005\350\005\3218\005\324\005\347\005\334\005\347"
"8\005\324\005\347\005\334\005\330\005\3528\005\324\005\346\005\334\005\3218\005\324\005\346\005\331\005\330\005\325\005\330\005\331\005\3358\005\324\005\346\005\325\005\344\005\3378\005\324\005\344\005\350\005\341\005\325\005\335"
"8\005\324\005\342\005\350\005\331\005\333\005\3248\005\324\005\342\005\346\005\325\005\336\005\3248\005\324\005\342\005\323\005\3378\005\324\005\340\005\333\005\325\005\3378\005\324\005\340\005\331\005\341\005\331\005\325\005\337"
"8\005\324\005\340\005\324\005\331\005\322\005\3248\005\324\005\336\005\352\005\327\005\350\005\331\005\3358\005\324\005\336\005\351\005\344\005\3308\005\324\005\336\005\351\005\327\005\347\005\331\005\3358\005\324\005\336\005\351\005\320"
"8\005\324\005\336\005\342\005\350\005\333\005\3528\005\324\005\336\005\341\005\327\005\3508\005\324\005\336\005\327\005\331\005\350\005\331\005\3358\005\324\005\336\005\326\005\350\005\3278\005\324\005\336\005\324\005\331\005\350"
"8\005\324\005\336\005\323\005\331\005\340\005\325\005\3528\005\324\005\334\005\347\005\325\005\3278\005\324\005\331\005\346\005\325\005\3208\005\324\005\331\005\340\005\3358\005\324\005\331\005\336\005\325\005\350\005\331\005\335"
"8\005\324\005\331\005\331\005\352\005\3318\005\324\005\331\005\331\005\340\005\3258\005\324\005\331\005\324\005\325\005\323\005\3318\005\324\005\331\005\3238\005\324\005\327\005\323\005\351\005\331\005\3358\005\324\005\325\005\350\005\323\005\352"
"8\005\324\005\325\005\350\005\323\005\325\005\3528\005\324\005\325\005\347\005\3358\005\324\005\325\005\342\005\334\005\3248\005\324\005\325\005\341\005\344\005\3248\005\324\005\324\005\3508\005\324\005\324\005\347\005\334\005\330\005\324"
"8\005\324\005\324\005\331\005\336\005\325\005\3508\005\324\005\322\005\350\005\325\005\342\005\331\005\3358\005\324\005\322\005\331\005\351\005\3248\005\324\005\321\005\331\005\330\005\325\005\3318\005\324\005\320\005\352\005\331\005\347\005\324"
"8\005\324\005\320\005\334\005\3248\005\323\005\350\005\331\005\351\005\325\005\3528\005\323\005\325\005\336\005\331\005\3358\005\322\005\3348\005\322\005\323\005\325\005\334\005\331\005\3358\005\321\005\352\005\351\005\342\005\324"
"8\005\321\005\350\005\320\005\3518\005\321\005\342\005\341\005\347\005\331\005\3358\005\321\005\340\005\325\005\322\005\3428\005\321\005\340\005\3258\005\321\005\336\005\327\005\331\005\3508\005\321\005\336\005\3248\005\321\005\334\005\331"
"8\005\321\005\333\005\340\005\350\005\3528\005\321\005\330\005\321\005\350\005\331\005\3248\005\321\005\327\005\331\005\350\005\3528\005\321\005\327\005\3238\005\321\005\324\005\3378\005\321\005\324\005\325\005\346\005\320\005\3528\005\321\005\320"
"8\005\320\005\352\005\3378\005\320\005\351\005\350\005\320\005\3318\005\320\005\347\005\350\005\320\005\331\005\331\005\3358\005\320\005\340\005\3208\005\320\005\334\005\331\005\333\005\3358\005\320\005\334\005\331\005\325"
"8\004\107\004\076\004\062\004\065\004\072\004\0608\004\101\004\076\004\062\004\101\004\065\004\0748\004\076\004\102\004\062\004\065\004\102\004\070\004\102\004\1148\004\075\004\076\004\062\004\113\004\071"
"8\004\075\004\060\004\077\004\100\004\060\004\062\004\0708\004\067\004\075\004\060\004\0748\003\314\003\273\003\277\003\305\003\3028\003\270\003\255\003\303\003\2678\003\265\003\2758zgr8ykj8ksn8blwc8\013\2718\013\263\013\2448\011\060\011\025"
"8\006\107\006\106\006\110\006\0628\006\106\006\101\006\0618\006\105\006\314\006\251\006\106\006\0578\006\105\006\104\006\3148\006\064\006\061\006\0678\006\055\006\110\006\062\006\1078\006\050\006\314\006\064\006\052\006\061"
"8\004\101\004\060\004\074\004\076\004\0748\004\077\004\076\004\073\004\070\004\102\004\070\004\072\004\0608\004\067\004\075\004\060\004\107\004\065\004\075\004\070\004\0658\003\3078\003\303\003\307\003\265\003\304\003\271\003\272\003\254"
"8\003\265\003\263\003\263\003\301\003\261\003\306\003\2568\003\261\003\305\003\304\003\314\003\2758\003\261\003\305\003\304\003\277\003\2578\003\261\003\3028\003\261\003\301\003\307\003\271\003\272\003\2568yml8rmd8dsd8\011\027\011\017"
"8\011\025\011\071\011\0508\011\005\011\062\011\0278\006\257\006\061\006\101\006\0528\006\112\006\071\006\105\006\1048\006\105\006\061\006\101\006\110\006\0718\006\103\006\050\006\112\006\061\006\0518\006\101\006\107\006\1128\006\101\006\105\006\106"
"8\006\071\006\064\006\061\006\0518\006\066\006\105\006\112\006\0618\006\065\006\057\006\061\006\0528\006\052\006\072\006\112\006\112\006\0618\006\052\006\071\006\105\006\1048\006\050\006\103\006\1048\006\050\006\047\006\104\006\104\006\107"
"8\006\047\006\106\006\102\006\0618\006\047\006\104\006\067\006\071\006\047\006\1058\006\047\006\104\006\063\006\112\006\0578\006\047\006\104\006\061\006\054\006\1048\006\047\006\104\006\054\006\110\006\047\006\1048\006\042\006\314\006\047"
"8\004\100\004\060\004\067\004\061\004\070\004\100\004\0608\004\074\004\076\004\063\004\1038\004\072\004\076\004\102\004\076\004\100\004\076\004\063\004\0768\004\070\004\074\004\065\004\102\004\0768\004\070\004\074\004\060\004\074"
"8\004\065\004\111\004\1218\003\314\003\275\003\277\003\274\003\2618\003\274\003\271\003\261\003\3028ppb8pgr8ljd8d\001\1768ckn8chtv8\013\261\013\2528\013\244\013\2608\011\057\011\0608\011\037\011\0608\006\257\006\110\006\106\006\107"
"8\006\176\006\047\006\063\006\0568\006\112\006\110\006\054\006\0578\006\112\006\102\006\047\006\1048\006\105\006\112\006\064\006\110\006\0578\006\105\006\103\006\047\006\1068\006\104\006\104\006\064\006\112\006\0568\006\102\006\047\006\050\006\104"
"8\006\071\006\110\006\057\006\0518\006\071\006\104\006\112\006\106\006\0478\006\071\006\047\006\057\006\0518\006\071\006\047\006\046\006\064\006\0518\006\057\006\056\006\110\006\1048\006\056\006\110\006\047\006\107\006\106\006\057"
"8\006\052\006\067\006\110\006\112\006\0618\006\050\006\110\006\057\006\1068\006\050\006\105\006\071\006\106\006\1118\006\047\006\104\006\057\006\047\006\071\006\112\006\0518\006\047\006\104\006\056\006\104\006\047\006\101"
"8\006\047\006\104\006\055\006\063\006\112\006\1068\006\047\006\104\006\045\006\112\006\105\006\047\006\1068\006\043\006\105\006\047\006\1058\004\103\004\101\004\073\004\076\004\062\004\070\004\117"
"8\004\101\004\102\004\076\004\100\004\076\004\075\004\1138\004\101\004\100\004\060\004\067\004\1038\004\100\004\060\004\067\004\073\004\070\004\107\004\075\004\0708\004\075\004\060\004\107\004\060\004\073\004\076\004\102\004\076"
"8\004\073\004\070\004\107\004\075\004\0768\004\070\004\074\004\060\004\110\004\0658\004\062\004\112\004\077\004\100\004\076\004\1018\003\304\003\255\003\273\003\277\003\3028xb8tdr8ptr8nskl8ndf8mkv8ldst8fv8\011\070\011\054\011\0708\006\110\006\314"
"8\006\110\006\047\006\061\006\0578\006\067\006\061\006\1018\006\054\006\107\006\0528\004\117\004\075\004\062\004\060\004\100\004\1178\004\104\004\076\004\102\004\0768\004\077\004\065\004\100\004\062\004\113\004\071"
"8\004\076\004\101\004\076\004\061\004\065\004\075\004\0768\004\067\004\075\004\060\004\1168\003\306\003\277\003\301\003\255\003\3028\003\274\003\255\003\301\003\277\003\3028\003\262\003\277\003\256\003\270\003\265\003\271\003\261"
"8\003\261\003\305\003\304\003\277\003\315\003\3028tzv8thw8rstr8ngsfr8lfst8ghtl8crt8\006\104\006\050\006\106\006\047\006\1068\006\102\006\104\006\050\006\1128\006\064\006\057\006\106\006\0578\006\056\006\110\006\057\006\064"
"8\006\047\006\106\006\052\006\056\006\047\006\0508\006\047\006\104\006\101\006\107\006\1058\006\047\006\104\006\071\006\102\006\047\006\102\006\112\006\0618\006\047\006\104\006\067\006\050\006\112\006\0508\006\047\006\104\006\064\006\105\006\063"
"8\006\047\006\104\006\057\006\110\006\104\006\1128\004\106\004\117\004\073\004\060\004\102\004\0608\004\102\004\060\004\072\004\112\004\0628\004\067\004\060\004\065\004\064\004\075\004\0768\004\064\004\103\004\105"
"8\003\303\003\307\003\255\003\303\003\2678\003\303\003\267\003\274\003\261\003\257\003\275\003\265\003\2718ww8tts8pps8ksm8\013\265\013\2318\013\222\013\2518\013\207\013\237\013\2568\011\060\011\0568\006\112\006\054\006\110\006\062"
"8\006\110\006\054\006\1048\006\105\006\067\006\104\006\110\006\0508\006\105\006\061\006\0578\006\064\006\112\006\046\006\0478\006\063\006\050\006\052\006\105\006\050\006\0618\006\061\006\105\006\066\006\047\006\106\006\112\006\051"
"8\006\061\006\054\006\1048\006\052\006\061\006\112\006\0578\006\047\006\104\006\105\006\065\006\061\006\1128\006\047\006\104\006\105\006\064\006\061\006\101\006\112\006\1068\006\047\006\104\006\063\006\106\006\051"
"8\006\047\006\104\006\054\006\112\006\1068\006\047\006\104\006\043\006\110\006\061\006\047\006\1058\006\047\006\104\006\042\006\056\006\0618\006\047\006\071\006\104\006\047\006\1058\006\047\006\063\006\052\006\102\006\0618\006\047\006\060\006\047"
"8\006\042\006\1048\004\101\004\062\004\076\004\070\004\0748\004\075\004\065\004\073\004\114\004\067\004\1178\004\072\004\112\004\064\004\0658\004\063\004\076\004\062\004\076\004\100\004\0708\003\303\003\265\003\273\003\257\003\264\003\265\003\302"
"8\003\300\003\301\003\316\003\304\003\2778\003\267\003\274\003\255\003\301\003\2618\003\254\003\273\003\273\003\265\003\3028\001\123l8\337l8lwg8lwch8ltf8lj\001\1418d\354n8ddwr8bzw8\006\106\006\257\006\047\006\1078\006\106\006\064\006\061"
"8\006\065\006\101\006\055\006\1078\006\063\006\110\006\3148\006\063\006\047\006\314\006\0528\006\055\006\047\006\104\006\0478\004\117\004\067\004\113\004\0728\004\101\004\100\004\065\004\064\004\0708\004\076\004\064\004\075\004\076\004\063\004\076"
"8\004\075\004\065\004\0748\004\067\004\075\004\060\004\0658\003\304\037\1708\003\261\003\273\003\273\003\261\003\263\003\255\003\3028\003\254\003\275\003\270\003\301\003\311\003\300\003\277\003\2718sbr8r\3738rlj8ngrw8hlj8dpr8b\354h8\013\261\013\251"
"8\013\244\013\2378\011\060\011\071\011\0448\011\050\011\0448\011\025\011\0558\006\110\006\1068\006\110\006\103\006\1048\006\110\006\061\006\0578\006\104\006\057\006\112\006\1038\006\101\006\066\006\1048\006\063\006\071\006\112\006\057"
"8\006\057\006\047\006\046\006\105\006\0478\006\054\006\107\006\047\006\0628\006\0538\006\052\006\071\006\061\006\1018\006\050\006\107\006\060\006\0478\006\050\006\102\006\104\006\1058\006\050\006\063\006\050\006\0500"};

unsigned char Consonants016[] = {
"8\006\047\006\104\006\106\006\047\006\0618\006\047\006\104\006\101\006\061\006\071\006\112\006\0518\006\047\006\104\006\064\006\061\006\1028\006\047\006\104\006\055\006\047\006\1048\006\047\006\104\006\042\006\112\006\051"
"8\006\043\006\101\006\103\006\047\006\0618\004\107\004\060\004\101\004\102\004\0768\004\076\004\102\004\062\004\065\004\1028\004\076\004\064\004\075\004\0608\004\064\004\065\004\075\004\114\004\063\004\0708\004\062\004\060\004\066\004\075\004\076"
"8\003\300\003\314\003\303\003\2778\003\274\003\261\003\270\003\256\003\274\003\261\003\304\003\2618\003\272\003\261\003\275\003\265\003\257\003\3028\003\254\003\273\003\273\003\277\003\305\003\3028nvl8ndc8mnj8jns8ghb8\013\236\013\2608\013\211\013\263"
"8\013\207\013\2438\011\070\011\056\011\0358\011\011\011\0448\006\206\006\106\006\112\006\1068\006\110\006\045\006\106\006\105\006\0478\006\106\006\063\006\056\006\0518\006\105\006\314\006\047\006\1068\006\105\006\063\006\047\006\071\006\057\006\051"
"8\006\105\006\056\006\052\006\104\006\101\006\0518\006\105\006\055\006\101\006\110\006\070\006\0518\006\104\006\105\006\057\006\0518\006\102\006\057\006\1058\006\071\006\065\006\0618\006\064\006\061\006\110\006\0718\006\057\006\061\006\063\006\052"
"8\006\050\006\104\006\251\006\1078\006\047\006\104\006\071\006\112\006\1068\006\047\006\104\006\067\006\061\006\112\006\102\006\0518\006\047\006\104\006\064\006\110\006\1028\006\047\006\104\006\064\006\056\006\065\006\112\006\051"
"8\006\047\006\104\006\055\006\103\006\110\006\105\006\0518\006\047\006\104\006\047\006\102\006\047\006\105\006\0518\006\047\006\052\006\065\006\1048\006\045\006\071\006\104\006\047\006\1068\006\043\006\106\006\1038\004\075\004\076\004\062\004\070"
"8\004\074\004\075\004\076\004\063\004\070\004\0658\004\070\004\101\004\072\004\0608\004\064\004\103\004\110\004\0608\003\300\003\301\003\277\003\306\003\257\003\2738\003\261\003\301\003\307\003\2568\003\255\003\307\003\265\003\304\003\2658\001\137c"
"8thl8srp8nsb8ndsch8mph8ggl8fnd8ccs8\006\106\006\047\006\105\006\1078\006\101\006\061\006\107\006\106\006\2578\006\064\006\0508\006\050\006\047\006\062\006\3148\004\101\004\102\004\100\004\060\004\075\004\070\004\106\004\060"
"8\004\101\004\062\004\076\004\117\004\102\004\0608\003\307\003\301\003\314\003\275\003\2778\003\274\003\265\003\263\003\254\003\273\003\2778vzt8vsr8rtv8rnh8rglw8nkcj8ndv8mlwg8dnt8cp8\013\260\013\2448\013\252\013\237\013\2318\011\060\011\0658\011\030"
"8\006\112\006\102\006\110\006\104\006\110\006\1068\006\106\006\071\006\1058\006\106\006\063\006\050\006\0518\006\104\006\047\006\1058\006\103\006\052\006\050\006\1078\006\071\006\105\006\104\006\112\006\0518\006\067\006\276\006\067"
"8\006\067\006\110\006\0618\006\067\006\0678\006\063\006\104\006\112\006\105\006\047\006\1068\006\054\006\062\006\047\006\1038\006\052\006\054\006\047\006\061\006\0518\006\050\006\055\006\047\006\054\006\051"
"8\006\047\006\104\006\063\006\061\006\067\006\047\006\1068\006\047\006\104\006\055\006\065\006\110\006\1048\006\047\006\064\006\052\006\061\006\047\006\1038\004\101\004\112\004\062\004\101\004\065\004\0748\004\101\004\102\004\076\004\070\004\102"
"8\004\077\004\076\004\100\004\060\004\064\004\0708\004\077\004\076\004\075\004\0658\004\076\004\064\004\075\004\0768\004\074\004\060\004\072\004\060\004\1008\004\072\004\060\004\066\004\065\004\102\004\101\004\1178\004\064\004\065\004\073\004\060"
"8\004\063\004\076\004\101\004\102\004\0708\003\265\003\271\003\272\003\314\003\275\003\2618\003\261\003\305\003\304\003\277\003\3158\001\176k8zvl8ymw8tstv8rthw8mrs8gpl8dds8c\001\1238\011\062\011\0508\011\060\011\0708\011\005\011\056\011\060"
"8\006\314\006\071\006\106\006\3148\006\112\006\106\006\050\006\072\006\1128\006\110\006\054\006\107\006\0518\006\105\006\104\006\047\006\055\006\070\006\0518\006\105\006\071\006\106\006\1118\006\105\006\071\006\070\006\105"
"8\006\104\006\072\006\047\006\0528\006\102\006\047\006\071\006\0518\006\101\006\104\006\1058\006\071\006\104\006\314\006\1078\006\063\006\050\006\047\006\046\006\1038\006\062\006\106\006\047\006\1068\006\057\006\110\006\061"
"8\006\057\006\047\006\061\006\1058\006\052\006\110\006\047\006\1068\006\047\006\104\006\105\006\103\006\052\006\050\006\0518\006\047\006\104\006\105\006\072\006\061\006\0508\006\047\006\104\006\101\006\052\006\055"
"8\006\047\006\104\006\071\006\057\006\112\006\0578\006\047\006\104\006\056\006\050\006\0618\006\047\006\104\006\052\006\101\006\047\006\065\006\112\006\1048\006\047\006\104\006\052\006\071\006\047\006\105\006\104"
"8\006\043\006\057\006\110\006\047\006\0528\006\042\006\105\006\0578\004\101\004\100\004\065\004\0648\004\075\004\060\004\110\004\070\004\1058\004\070\004\101\004\102\004\076\004\100\004\070\004\0708\004\070\004\074\004\0658\004\065\004\121"
"8\004\064\004\075\004\1178\003\303\003\304\003\277\003\271\003\307\003\265\003\257\003\2618\003\274\003\300\003\277\003\301\003\277\003\315\003\2758\003\265\003\270\003\275\003\271\003\272\003\2568\003\265\003\257\003\304\003\265"
"8\003\261\003\263\003\254\003\300\003\2678zdj8nkb8lsm8lps8ldh8jmn8gbl8\013\252\013\257\013\2518\013\207\013\260\013\2438\013\206\013\2258\011\027\011\0608\006\110\006\104\006\1128\006\110\006\042\006\104\006\1078\006\101\006\314\006\104\006\105"
"8\006\052\006\110\006\047\006\106\006\0578\006\052\006\047\006\061\006\314\006\0568\006\050\006\061\006\056\006\3148\004\103\004\0668\004\101\004\060\004\074\004\0608\004\075\004\065\004\072\004\0608\004\074\004\065\004\075\004\065"
"8\004\064\004\065\004\073\004\0658\003\304\037\164\003\2758\003\304\003\277\037\3468\003\275\003\261\003\2718\003\262\003\267\003\274\003\2618\003\254\003\275\003\270\003\301\003\311\003\300\003\277\003\3028\001\102k8yj8vr\001\1418n\354kd8drk"
"8cwmn\3578cnn8chtl8bbt8\013\2678\013\260\013\2258\013\257\013\265\013\2608\013\256\013\2448\013\232\013\2608\011\026\011\0508\011\025\011\0108\006\112\006\110\006\063\006\1018\006\110\006\050\006\047\006\104\006\052\006\047\006\104\006\112"
"8\006\105\006\106\006\065\006\110\006\0508\006\105\006\063\006\043\006\104\006\0518\006\105\006\056\006\052\006\047\006\061\006\047\006\0528\006\103\006\104\006\105\006\047\006\0528\006\102\006\067\006\0618\006\070\006\176\006\070\006\171"
"8\006\067\006\0708\006\061\006\110\006\055\006\1128\006\061\006\050\006\105\006\0478\006\057\006\047\006\057\006\1068\006\056\006\101\006\1068\006\050\006\110\006\057\006\1058\006\047\006\104\006\064\006\112\006\041"
"8\006\047\006\104\006\045\006\056\006\110\006\047\006\1068\006\047\006\064\006\047\006\061\006\1078\006\043\006\050\006\0478\004\100\004\065\004\110\004\065\004\075\004\070\004\0658\004\077\004\060\004\100\004\0708\004\074\004\076\004\063"
"8\004\064\004\060\004\073\004\114\004\110\004\0658\003\300\003\301\003\277\003\303\003\270\003\256\003\272\003\2678\003\300\003\265\003\301\003\271\003\277\003\307\003\2568\003\255\003\307\003\265\003\271\003\3028\001\137n8pls8pfl8fm8fk8dch"
"8\006\112\006\055\006\057\006\0538\006\105\006\071\006\104\006\110\006\105\006\047\006\0528\006\105\006\0648\006\065\006\055\006\0518\006\065\006\047\006\104\006\0558\006\057\006\314\006\257\006\061\006\3148\006\057\006\071\006\110\006\112\006\051"
"8\006\057\006\047\006\110\006\0578\006\057\006\047\006\063\006\052\006\047\006\1068\006\047\006\104\006\106\006\0658\006\047\006\104\006\105\006\056\006\052\006\104\006\101\006\0518\006\047\006\104\006\102\006\110\006\104"
"8\006\047\006\104\006\071\006\047\006\104\006\105\006\112\006\0518\006\047\006\104\006\065\006\047\006\046\006\1058\006\047\006\104\006\045\006\065\006\104\006\047\006\0558\006\047\006\104\006\042\006\056\006\061\006\112\006\106"
"8\004\101\004\075\004\076\004\062\004\0608\004\101\004\073\004\103\004\107\004\060\004\0718\004\101\004\060\004\074\004\076\004\0658\004\075\004\076\004\1118\004\072\004\103\004\064\004\0608\004\072\004\076\004\102\004\076\004\100\004\103\004\116"
"8\004\067\004\075\004\060\004\065\004\102\004\0658\004\066\004\070\004\102\004\1148\003\307\003\316\003\301\003\2618\001\076n8vzr8vzn8thwr8sprz8rvl8ntb8llc8cms8\037\1018\015\0638\015\056\015\062\015\0578\013\260\013\243\013\2568\013\257\013\265"
"8\013\206\013\2438\006\206\006\047\006\1768\006\107\006\105\006\061\006\047\006\1078\006\106\006\102\006\0648\006\105\006\104\006\0528\006\071\006\102\006\0508\006\057\006\047\006\106\006\064\006\257\006\047\006\107"
"8\004\115\004\102\004\076\004\074\004\1038\004\101\004\072\004\076\004\073\004\114\004\072\004\0768\004\076\004\102\004\063\004\076\004\062\004\076\004\1008\004\075\004\060\004\110\004\070\004\102\004\065"
"8\004\074\004\076\004\101\004\072\004\062\004\0658\004\070\004\075\004\076\004\063\004\064\004\0608\004\062\004\112\004\077\004\100\004\065\004\072\004\0708\004\062\004\101\004\112\004\111\004\075\004\076\004\101\004\1028\004\062\004\060\004\110"
"8\003\314\003\273\003\2778\003\304\037\306\003\3028\003\274\003\300\003\277\003\301\003\277\003\315\003\274\003\2658\003\272\003\277\003\301\003\305\003\306\003\2568\003\272\003\254\003\300\003\277\003\271\003\277\003\302"
"8\003\272\003\254\003\300\003\277\003\271\003\2618\003\265\003\303\003\265\003\257\003\3028\003\265\003\300\003\2578\003\264\003\271\003\254\003\301\003\272\003\265\003\271\003\2618\003\261\003\305\003\304\003\256\003\302"
"8\003\261\003\275\003\254\003\300\003\304\003\305\003\276\003\2678\003\254\003\301\003\270\003\301\003\2618xf8szp8smw8p\356n8mcs8lpp8lkk8h\001\0158ddwch8\020\345\020\320\020\340\020\327\020\343\020\332\020\330"
"8\016\040\016\062\016\051\016\062\016\104\016\027\016\0428\011\2678\011\2578\011\256\011\2438\011\2548\011\2438\011\2408\011\207\011\2568\011\007\011\070\011\0258\006\110\006\043\006\106\006\0528\006\107\006\101\006\052\006\107"
"8\006\106\006\052\006\112\006\054\006\0518\006\105\006\107\006\1058\006\105\006\061\006\047\006\0528\006\103\006\110\006\061\006\057\006\3148\006\071\006\112\006\1068\006\065\006\104\006\047\006\0518\006\063\006\112\006\057"
"8\006\062\006\112\006\047\006\057\006\0518\006\057\006\110\006\061\006\1078\006\056\006\065\006\110\006\065\006\0478\006\055\006\105\006\047\006\112\006\0518\006\054\006\047\006\106\006\0508\006\052\006\070\006\107\006\061"
"8\006\050\006\104\006\110\006\052\006\110\006\0538\006\050\006\063\006\1058\006\050\006\057\006\0438\006\050\006\047\006\063\006\052\006\104\006\047\006\1058\006\047\006\314\006\064\006\047\006\1060"};

unsigned char Consonants017[] = {
"8\006\047\006\104\006\105\006\050\006\047\006\061\006\1038\006\047\006\104\006\102\006\061\006\1068\006\047\006\104\006\071\006\104\006\105\006\112\006\0518\006\047\006\104\006\071\006\066\006\110"
"8\006\047\006\104\006\055\006\063\006\112\006\106\006\1128\006\047\006\104\006\052\006\055\006\102\006\112\006\1028\006\047\006\104\006\050\006\055\006\061\006\112\006\1068\006\047\006\104\006\045\006\064\006\052\006\061\006\047\006\103"
"8\006\047\006\104\006\043\006\065\006\104\006\112\006\0518\006\047\006\104\006\043\006\056\006\112\006\061\006\0518\006\047\006\102\006\061\006\0438\006\047\006\0508\006\045\006\106\006\1078\006\043\006\105\006\0638\005\331\005\331"
"8\005\323\005\331\005\3518\004\221\004\100\004\103\004\075\004\102\004\1268\004\127\004\105\004\075\004\126\004\0718\004\127\004\105\004\075\004\1268\004\127\004\105\004\075\004\114\004\076\004\063\004\076"
"8\004\126\004\102\004\060\004\073\004\126\004\1278\004\126\004\101\004\102\004\076\004\100\004\126\004\1278\004\126\004\101\004\102\004\076\004\100\004\070\004\072\004\0708\004\126\004\101\004\102\004\076\004\100\004\070\004\072"
"8\004\126\004\101\004\1028\004\126\004\100\004\062\004\126\004\075\004\063\004\0608\004\126\004\075\004\111\004\1268\004\126\004\075\004\110\004\0658\004\126\004\075\004\072\004\076\004\073\004\0708\004\126\004\075\004\064\004\126\004\127"
"8\004\126\004\075\004\064\004\126\004\060\004\075\004\0608\004\126\004\073\004\073\004\126\004\075\004\076\004\071\004\1018\004\126\004\0678\004\126\004\064\004\065\004\1278\004\126\004\064\004\065\004\071\004\075\004\070\004\105"
"8\004\126\004\064\004\0658\004\124\004\064\004\075\004\060\004\075\004\075\004\1178\004\124\004\064\004\070\004\075\004\1038\004\124\004\064\004\070\004\075\004\076\004\063\004\0768\004\124\004\064\004\070\004\075\004\070\004\071"
"8\004\124\004\064\004\070\004\075\004\0608\004\124\004\062\004\100\004\076\004\077\004\0708\004\124\004\062\004\100\004\065\004\127\004\0628\004\117\004\102\004\070\004\107\004\126\004\0628\004\117\004\075\004\101\004\114\004\072\004\126"
"8\004\117\004\075\004\101\004\114\004\072\004\076\004\074\004\1038\004\117\004\075\004\101\004\114\004\072\004\070\004\074\004\0708\004\117\004\075\004\101\004\114\004\072\004\070\004\0748\004\117\004\075\004\101\004\114\004\072\004\070\004\071"
"8\004\117\004\072\004\126\004\0718\004\117\004\072\004\103\004\102\004\0708\004\117\004\072\004\076\004\063\004\0768\004\117\004\072\004\070\004\074\004\0708\004\117\004\067\004\060\004\075\004\0658\004\117\004\064\004\100\004\076\004\074"
"8\004\117\004\064\004\100\004\0768\004\117\004\062\004\073\004\117\004\1248\004\117\004\062\004\070\004\073\004\076\004\101\004\1178\004\116\004\102\004\065\004\100\004\126\004\0628\004\116\004\102\004\065\004\100\004\075\004\076\004\063\004\076"
"8\004\116\004\102\004\065\004\100\004\0708\004\116\004\102\004\0608\004\111\004\076\004\077\004\100\004\060\004\062\004\064\004\0608\004\111\004\076\004\064\004\0768\004\111\004\076\004\0618\004\110\004\103\004\072\004\060\004\124"
"8\004\110\004\103\004\072\004\060\004\071\004\102\004\0658\004\110\004\073\004\117\004\105\004\102\004\0708\004\110\004\072\004\076\004\073\004\0708\004\110\004\072\004\076\004\073\004\0608\004\110\004\070\004\100\004\076\004\102\004\060"
"8\004\110\004\070\004\100\004\076\004\072\004\0768\004\110\004\070\004\100\004\070\004\075\004\0608\004\110\004\062\004\070\004\064\004\110\004\070\004\0748\004\110\004\062\004\070\004\064\004\072\004\060"
"8\004\110\004\062\004\065\004\071\004\075\004\0608\004\110\004\062\004\060\004\0618\004\107\004\060\004\101\004\102\004\0708\004\100\004\060\004\061\004\076\004\102\004\0708\004\075\004\065\004\061\004\103\004\064\004\114"
"8\004\064\004\076\004\073\004\066\004\075\004\0768\004\062\004\113\004\101\004\110\004\060\004\1178\004\062\004\064\004\100\004\103\004\0638\004\061\004\070\004\073\004\0608\004\060\004\100\004\105\004\070\004\0628\003\305\003\300\003\314"
"8\003\300\003\301\003\316\003\304\003\2618\003\274\003\256\003\275\003\305\003\274\003\2618\003\270\003\255\003\273\003\3118\003\270\003\255\003\273\003\265\003\2718\003\261\003\305\003\304\003\256\003\2758\003\254\003\273\003\273\003\277\003\2718tsv"
"8rkw8nkn8ngsp8\013\260\013\2528\013\205\013\2608\011\070\011\060\011\0258\006\106\006\107\006\047\006\112\006\0518\006\104\006\107\006\060\006\1078\006\103\006\053\006\112\006\061\006\0518\006\056\006\047\006\067\006\0618\006\054\006\107\006\051"
"8\006\054\006\105\006\071\006\112\006\0518\006\054\006\0478\006\052\006\102\006\057\006\1058\006\050\006\112\006\064\006\052\006\0618\006\047\006\104\006\054\006\062\006\0418\006\047\006\104\006\045\006\106\006\052\006\061\006\106\006\052"
"8\006\045\006\106\006\105\006\0478\006\043\006\055\006\057\006\0538\006\043\006\054\006\1048\004\102\004\060\004\072\004\076\004\062\004\0608\004\101\004\102\004\060\004\075\004\0608\004\077\004\100\004\065\004\066\004\064\004\065"
"8\004\077\004\100\004\060\004\062\004\064\004\0608\004\062\004\076\004\067\004\074\004\076\004\066\004\075\004\0768\003\274\003\300\003\301\003\277\003\303\003\304\003\2548\003\273\003\255\003\276\003\265\003\271\003\3028trv8npl8crh8chtw"
"8\013\265\013\2518\013\217\013\2618\006\251\006\105\006\2518\006\206\006\314\006\062\006\3148\006\106\006\050\006\110\006\0578\006\105\006\050\006\047\006\061\006\062\006\1078\006\056\006\104\006\047\006\1018\006\050\006\063\006\112\006\047\006\061"
"8\006\050\006\061\006\047\006\050\006\0618\006\042\006\072\006\047\006\0628\004\102\004\060\004\072\004\060\004\062\004\0608\004\074\004\070\004\101\004\073\004\1178\004\072\004\060\004\066\004\0608\004\067\004\060\004\102\004\065\004\074"
"8\004\064\004\060\004\062\004\0608\004\062\004\073\004\060\004\101\004\102\004\0708\003\307\003\316\003\301\003\261\003\3028\003\300\003\301\003\254\003\263\003\274\003\261\003\304\003\2618\003\275\003\255\003\265\003\3028\003\264\003\305\003\277"
"8\376rj8zg\001\1028ydd8vrt8sdr8rthd8rkst8pr\371hl8nnr8ngsl8krz8d\354t8dsch8ctw8\011\054\011\034\011\0378\006\112\006\071\006\061\006\1018\006\105\006\063\006\052\006\071\006\105\006\1048\006\105\006\057\006\112\006\106\006\051"
"8\006\102\006\047\006\1058\006\064\006\103\006\061\006\0478\006\064\006\061\006\103\006\0518\006\063\006\050\006\112\006\1048\006\061\006\072\006\1058\006\050\006\047\006\071\006\0538\006\047\006\104\006\105\006\110\006\047\006\057"
"8\006\047\006\104\006\105\006\047\006\057\006\0518\006\047\006\104\006\103\006\104\006\105\006\0518\006\047\006\104\006\065\006\055\006\047\006\050\006\0518\006\047\006\104\006\064\006\047\006\071\006\061"
"8\006\047\006\104\006\050\006\061\006\110\006\047\006\0628\006\047\006\104\006\043\006\112\006\047\006\1058\004\101\004\102\004\060\004\075\004\0658\004\076\004\102\004\074\004\065\004\102\004\072\004\060"
"8\004\072\004\073\004\060\004\101\004\101\004\075\004\103\004\1168\004\066\004\065\004\073\004\060\004\075\004\070\004\0658\004\064\004\065\004\102\004\065\004\0718\004\062\004\112\004\067\004\074\004\076\004\066\004\075\004\076"
"8\004\062\004\076\004\064\004\0608\003\303\003\315\003\274\003\306\003\311\003\275\003\2618\003\303\003\305\003\275\003\255\003\307\003\265\003\271\003\2618p\3478mcc8lfb8chwp8\013\216\013\2378\011\066\011\0508\011\065\011\0508\011\062\011\025"
"8\011\060\011\0268\011\057\011\0468\006\110\006\072\006\112\006\061\006\107\006\0478\006\110\006\062\006\047\006\061\006\0518\006\105\006\104\006\052\006\102\006\1118\006\105\006\071\006\1078\006\105\006\067\006\061\006\055"
"8\006\105\006\055\006\067\006\0518\006\105\006\052\006\065\006\101\006\055\006\1078\006\104\006\112\006\063\006\0528\006\103\006\110\006\1058\006\103\006\106\006\106\006\0578\006\071\006\063\006\1048\006\065\006\101\006\055\006\047\006\052"
"8\006\063\006\106\006\110\006\047\006\0528\006\062\006\112\006\047\006\061\006\0518\006\052\006\061\006\1118\006\052\006\061\006\050\006\112\006\0518\006\050\006\047\006\104\006\066\006\105\006\0518\006\047\006\104\006\106\006\055\006\110"
"8\006\047\006\104\006\103\006\110\006\1068\006\047\006\104\006\063\006\047\006\050\006\1028\006\047\006\104\006\052\006\072\006\112\006\112\006\0618\006\047\006\104\006\050\006\071\006\0668\006\047\006\104\006\043\006\063\006\046\006\104\006\051"
"8\006\045\006\066\006\072\006\0678\006\045\006\050\006\061\006\047\006\107\006\112\006\1058\006\042\006\106\006\054\006\0478\004\106\004\065\004\073\004\0708\004\101\004\0628\004\075\004\060\004\072\004\076\004\075\004\065\004\106"
"8\004\072\004\100\004\060\004\1178\004\062\004\065\004\100\004\101\004\070\004\1178\004\060\004\074\004\0608\003\304\003\274\003\256\003\274\003\2618\003\300\003\277\003\271\003\277\003\3028\003\275\003\255\003\2778yp8t\3548rgl8rcl8ppst8ndp8dbr"
"8\013\205\013\2528\011\006\011\0578\006\257\006\101\006\052\006\1058\006\257\006\062\006\047\006\061\006\0648\006\104\006\047\006\062\006\1058\006\102\006\061\006\042\006\1068\006\063\006\056\006\1068\006\062\006\314\006\061"
"8\006\054\006\105\006\104\006\1078\006\047\006\110\006\1068\004\105\004\076\004\107\004\1038\004\100\004\103\004\072\004\0708\004\077\004\100\004\076\004\061\004\073\004\065\004\0748\003\277\003\300\003\277\003\257\003\277\003\302"
"8\003\274\003\314\003\273\003\271\003\3028\003\274\003\255\003\301\003\2618\003\273\003\255\003\275\003\2658\003\265\003\272\003\304\003\314\003\3028\003\262\003\271\003\262\003\273\003\257\003\2778\360l8sz\001\1028ssn8skrblj8schm8rrn8n\001\0158nng"
"8ngz8llp8ktn8gmv8dfr8chg8\006\110\006\062\006\1068\006\110\006\047\006\102\006\0718\006\105\006\105\006\1068\006\105\006\102\006\047\006\104\006\0518\006\105\006\052\006\065\006\101\006\0558\006\104\006\043\006\106\006\107\006\047"
"8\006\103\006\104\006\1078\006\103\006\053\006\112\006\061\006\0478\006\0728\006\057\006\1128\006\054\006\105\006\107\006\110\006\061\006\3148\006\054\006\0618\006\052\006\063\006\052\006\056\006\057\006\1058\006\050\006\107\006\060\006\107"
"8\006\050\006\071\006\066\006\107\006\1058\006\047\006\104\006\105\006\044\006\105\006\106\006\112\006\1068\006\047\006\104\006\071\006\063\006\103\006\061\006\1128\006\047\006\104\006\053\006\047\006\106\006\112\006\051"
"8\006\047\006\104\006\045\006\063\006\050\006\047\006\1068\006\047\006\067\006\104\006\047\006\071\006\047\006\0528\006\043\006\110\006\104\006\0478\004\104\004\076\004\100\004\074\004\0608\004\077\004\065\004\100\004\070\004\076\004\0640"};

unsigned char Consonants018[] = {
"8\004\076\004\107\004\070\004\102\004\0658\004\075\004\060\004\077\004\112\004\073\004\075\004\0768\004\073\004\070\004\106\004\0608\004\063\004\076\004\073\004\117\004\0748\004\063\004\076\004\062\004\076\004\100\004\117"
"8\004\063\004\073\004\060\004\067\004\0608\004\062\004\101\004\1168\004\062\004\076\004\071\004\075\004\0608\004\062\004\065\004\107\004\065\004\1008\004\061\004\112\004\100\004\067\004\0768\004\061\004\060\004\111\004\0608\003\314\003\273\003\267"
"8\003\307\003\301\003\267\003\303\003\304\003\316\003\2758\003\300\003\277\003\273\003\273\003\255\003\3028\003\300\003\261\003\301\003\2548\003\273\003\314\003\263\003\3118\003\272\003\277\003\271\003\275\003\311\003\275\003\257\003\261"
"8\003\265\003\305\003\301\003\3168\003\261\003\263\003\277\003\301\003\2548\356\001\1438yg8txt8rt\3408rskn8rlw8ngw8mbt8ljstv8hnl8dstr8czc8\013\252\013\2508\013\225\013\2618\013\207\013\2448\011\005\011\0628\006\105\006\106\006\067\006\102\006\107"
"8\006\105\006\061\006\050\006\110\006\0678\006\102\006\061\006\112\006\0508\006\071\006\112\006\110\006\1068\006\071\006\104\006\047\006\102\006\0518\006\064\006\056\006\065\006\112\006\0518\006\062\006\047\006\057"
"8\006\061\006\047\006\050\006\067\006\1078\006\056\006\067\006\0438\006\055\006\047\006\054\006\0518\006\052\006\102\006\110\006\1048\006\052\006\072\006\112\006\0618\006\050\006\055\006\112\006\0538\006\047\006\104\006\106\006\070\006\061"
"8\006\047\006\104\006\105\006\064\006\047\006\071\006\0618\006\047\006\104\006\105\006\063\006\047\006\046\006\1048\006\047\006\104\006\103\006\112\006\105\006\047\006\110\006\1128\006\047\006\104\006\103\006\050\006\112\006\061"
"8\006\047\006\104\006\056\006\105\006\112\006\0638\006\047\006\104\006\055\006\061\006\0518\006\047\006\104\006\055\006\0548\006\047\006\104\006\053\006\047\006\104\006\0538\006\047\006\104\006\045\006\105\006\047\006\061\006\047\006\052"
"8\006\043\006\107\006\1058\004\107\004\060\004\1018\004\101\004\102\004\100\004\060\004\075\004\1138\004\101\004\073\004\065\004\064\004\103\004\065\004\1028\004\076\004\077\004\117\004\102\004\1148\004\075\004\060\004\110\004\065\004\063\004\076"
"8\004\075\004\060\004\073\004\0708\004\075\004\060\004\071\004\102\004\0708\004\074\004\065\004\075\004\065\004\0658\004\072\004\100\004\060\004\0718\004\064\004\076\004\071\004\064\004\0658\004\062\004\101\004\1178\003\265\003\2728\356nd8rtsch"
"8p\001\1028llj8jls8jdspl8jds8\011\070\011\0548\011\054\011\0708\011\054\011\0328\006\105\006\112\006\047\006\1068\006\056\006\047\006\106\006\110\006\047\006\057\006\1078\006\050\006\047\006\104\006\047"
"8\004\076\004\101\004\076\004\061\004\065\004\075\004\075\004\0768\004\075\004\076\004\065\004\074\004\062\004\100\004\0708\004\075\004\076\004\062\004\113\004\0658\004\075\004\076\004\062"
"8\004\070\004\067\004\062\004\065\004\101\004\102\004\075\004\0768\004\064\004\062\004\103\004\1058\003\300\003\314\003\273\003\2678\003\272\003\314\003\303\003\274\003\277\003\3028\003\272\003\254\003\275\003\311"
"8\003\265\003\271\003\264\003\256\003\303\003\265\003\271\003\3028\003\265\003\261\003\305\003\304\003\3148vzd8vgl8tvrt8ssm8pth8ngsspr8ljsk8jgsg8ftr8c\3478bwr8\013\265\013\2258\011\005\011\0608\006\206\006\107\006\047\006\061"
"8\006\112\006\044\006\057\006\1128\006\107\006\106\006\047\006\104\006\1038\006\107\006\057\006\107\006\0578\006\106\006\105\006\1128\006\106\006\102\006\061\006\0438\006\105\006\110\006\102\006\071\006\1038\006\105\006\106\006\067\006\102\006\051"
"8\006\105\006\102\006\047\006\050\006\1048\006\105\006\057\006\110\006\106\006\052\006\1038\006\105\006\052\006\110\006\047\006\054\006\0578\006\105\006\047\006\0668\006\102\006\1058\006\102\006\066\006\047\006\112\006\0478\006\072\006\314\006\061"
"8\006\063\006\047\006\071\006\0518\006\062\006\047\006\1048\006\061\006\110\006\062\006\106\006\047\006\105\006\1078\006\057\006\107\006\106\006\0578\006\057\006\056\006\1048\006\055\006\061\006\103\006\0518\006\047\006\112\006\106\006\103\006\107"
"8\006\047\006\104\006\103\006\104\006\105\006\047\006\0528\006\047\006\104\006\102\006\112\006\047\006\1058\006\047\006\104\006\055\006\102\006\1068\006\047\006\104\006\054\006\106\006\063\006\112\006\051"
"8\006\047\006\104\006\047\006\052\006\055\006\047\006\0578\006\047\006\104\006\043\006\063\006\047\006\063\006\112\006\0518\006\047\006\104\006\0438\006\047\006\061\006\063\006\047\006\1048\004\112m8\004\101\004\065\004\100\004\064\004\106\004\065"
"8\004\101\004\060\004\074\004\076\004\063\004\0768\004\100\004\112\004\072\004\0608\004\100\004\065\004\071\004\102\004\070\004\075\004\0638\004\077\004\103\004\102\004\1148\004\077\004\100\004\060\004\062\004\070\004\073\004\060"
"8\004\076\004\075\004\076\004\062\004\0608\004\076\004\067\004\075\004\060\004\107\004\060\004\062\004\0608\004\073\004\116\004\061\004\076\004\062\004\102\004\0608\004\072\004\076\004\102\004\076\004\100\004\076\004\0748\004\072\004\076"
"8\004\064\004\103\004\074\004\060\004\1168\004\064\004\065\004\106\004\060\004\102\004\0608\004\064\004\065\004\106\004\0608\004\064\004\065\004\072\004\065\004\074\004\062\004\100\004\0708\004\064\004\062\004\065\004\102\004\065"
"8\004\062\004\100\004\112\004\067\004\072\004\0608\003\274\003\277\003\305\003\303\003\271\003\272\003\2568\003\272\003\261\003\273\003\3148xh8vsl8rfr8rcz8d\356ns8dsp8cb8\013\232\013\2508\013\225\013\263\013\2568\013\225\013\2318\013\222\013\262"
"8\011\006\011\052\011\0508\006\112\006\061\006\1118\006\106\006\101\006\107\006\1058\006\106\006\065\006\1018\006\106\006\054\006\0578\006\105\006\067\006\047\006\104\006\0508\006\105\006\047\006\057\006\1078\006\104\006\057\006\112\006\106\006\047"
"8\006\103\006\061\006\112\006\1058\006\101\006\112\006\1038\006\072\006\112\006\061\006\1078\006\071\006\064\006\1028\006\071\006\0578\006\065\006\105\006\0528\006\065\006\055\006\050\006\0528\006\062\006\105\006\314\006\1068\006\061\006\052\006\050"
"8\006\047\006\104\006\105\006\064\006\103\006\104\006\0518\006\047\006\104\006\105\006\063\006\052\006\056\006\057\006\1058\006\047\006\104\006\105\006\057\006\110\006\106\006\0518\006\047\006\104\006\102\006\065\006\051"
"8\006\047\006\104\006\057\006\056\006\110\006\1048\006\047\006\104\006\052\006\047\006\104\006\1128\006\047\006\104\006\050\006\0618\006\047\006\104\006\047\006\106\006\052\006\061\006\106\006\052"
"8\006\047\006\104\006\043\006\056\006\050\006\047\006\0618\006\047\006\054\006\052\006\105\006\047\006\071\006\3148\006\043\006\106\006\107\006\1058\006\043\006\055\006\0508\006\042\006\105\006\061\006\314\006\251\006\047"
"8\004\102\004\060\004\072\004\076\004\063\004\0768\004\101\004\102\004\100\004\060\004\075\004\0708\004\101\004\077\004\076\004\100\004\1028\004\077\004\112\004\100\004\062\004\0768\004\077\004\076\004\070\004\101\004\072"
"8\004\063\004\076\004\073\004\065\004\074\004\0708\003\310\003\267\003\306\003\257\003\303\003\304\003\2658\003\307\003\255\003\301\003\271\003\2618\003\304\037\366\003\2758\003\304\003\267\003\3038\003\304\003\257\003\300\003\277\003\304\003\261"
"8\003\275\037\1608\003\274\003\277\003\301\003\306\003\2568\003\274\003\255\003\273\003\277\003\3028\003\264\003\315\003\275\003\261\003\274\003\2678\003\262\003\271\003\262\003\273\003\257\003\2618ynn8xts8wm8stst8r\360r8ppm8ndsl8k\001\0150"};


unsigned char * VowsConsLists[] = {
    Vowels001,
    // The output list went to 034, but I cut them off here...
    Consonants002,
    Consonants003,
    Consonants004,
    Consonants005,
    Consonants006,
    Consonants007,
    Consonants008,
    Consonants009,
    Consonants010,
    Consonants011,
    Consonants012,
    Consonants013,
    Consonants014,
    Consonants015,
    Consonants016,
    Consonants017,
    Consonants018,
    NULL };



#if DEBUG_REVISE_VOWELS
void BuildConsonantVowelRuns( wchar_t * szWord, int CountInLanguage )
{
    // This also must be temporary, because it is too late
    // for me to be building CSolConsonantRunValues and
    // CSolVowelRunValues using the word lists, because
    // this same salt loop wants to know the VALUATIONS.
    // So I can build one to output for a recompilation.
    // I can write, fix NULLs in my callers wk[] buffer.

    // Now I can perfect that rule about y is/not vowel.

    // my own unicode ( but not CJK ) int isvowel( wchar_t wc ) does:
    // Return 1 if wc is a vowel, 0 for a consonant ( assumes isalpha ).
    // Return 2 for 'Y', 'y' which hard case user must further sort out.

    // A consonant in a word is a letter other than A, E, I, O or U,
    // and other than Y preceded by a consonant.
    // -- http://snowball.tartarus.org/algorithms/porter/stemmer.html

    // This should have the same loop as WordLetterValuation,
    // only perfect it for 'y' with my own isvowel( ) routine.

    wchar_t * from = szWord;
    int priorConsonant = 0;
    wchar_t run[ MAX_CONSONANT_OR_VOWEL_RUN + 1 ]; // Consonant or Vowel runs
    int nFillingVows = 0;
    int nFillingCons = 0;

    for( ;; )
    {
        wchar_t wc = *from++;

        switch( isvowel( wc ) )
        {
        case 2:
            // Is a Y. -- Y is a vowel after a prior consonant.
            if( priorConsonant )
            {
                #if DO_DEBUG_VALUATION
                    Spew( L"Treat Y as vowel after a prior consonant." );
                #endif

                goto TreatYAsVowel;
            }
            else
            {
                #if DO_DEBUG_VALUATION
                    Spew( L"treat all first/other Y as a consonant." );
                #endif
                goto TreatYAsConsonant;
            }
            break;
        case 1:
            // Is a vowel.
            {
                TreatYAsVowel : ;
                // So here, process ( another ) vowel.
                priorConsonant = 0;

                // Finish any valid consonants in progress.
                if( nFillingCons > 0
                && nFillingCons < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingCons ] = NULL;
                    #if DO_DEBUG_VALUATION
                        SpewTwo( L"Cons", run );
                    #endif
                    unsigned int index = CSolNewCons.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        int some = CSolNewCons.GetUserValue( index );
                        some += CountInLanguage;
                        CSolNewCons.SetUserValue( index, some );
                    }
                }
                nFillingCons = 0;

                // Save this vowel into run.

                if( nFillingVows < MAX_CONSONANT_OR_VOWEL_RUN )
                    run[ nFillingVows++ ] = wc;
            }
            break;
        case 0:
            // Not a vowel. Test if even an alpha.
            if( iswalpha( wc ) )
            {
                TreatYAsConsonant : ;
                // So here, process ( another ) consonant.
                priorConsonant = 1;

                // Finish any valid Vowels in progress.
                if( nFillingVows > 0
                && nFillingVows < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingVows ] = NULL;
                    #if DO_DEBUG_VALUATION
                        SpewTwo( L"Vows", run );
                    #endif
                    unsigned int index = CSolNewVows.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        int some = CSolNewVows.GetUserValue( index );
                        some += CountInLanguage;
                        CSolNewVows.SetUserValue( index, some );
                    }
                }
                nFillingVows = 0;

                // Save this consonant into run.

                if( nFillingCons < MAX_CONSONANT_OR_VOWEL_RUN )
                    run[ nFillingCons++ ] = wc;
            }
            else
            {
                // May be numbers, puntuation.
                // N.B. May be at final NULL.
                priorConsonant = 0;

                // No character to save, but two finishers.

                // Finish any valid consonants in progress.
                if( nFillingCons > 0
                && nFillingCons < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingCons ] = NULL;
                    #if DO_DEBUG_VALUATION
                        SpewTwo( L"Cons", run );
                    #endif
                    unsigned int index = CSolNewCons.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        int some = CSolNewCons.GetUserValue( index );
                        some += CountInLanguage;
                        CSolNewCons.SetUserValue( index, some );
                    }
                }
                nFillingCons = 0;

                // Finish any valid Vowels in progress.
                if( nFillingVows > 0
                && nFillingVows < MAX_CONSONANT_OR_VOWEL_RUN )
                {
                    run[ nFillingVows ] = NULL;
                    #if DO_DEBUG_VALUATION
                        SpewTwo( L"Vows", run );
                    #endif
                    unsigned int index = CSolNewVows.AddKey( run );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at call 1392" ); }
                    #endif
                    if( index > 1 )
                    {
                        int some = CSolNewVows.GetUserValue( index );
                        some += CountInLanguage;
                        CSolNewVows.SetUserValue( index, some );
                    }
                }
                nFillingVows = 0;
            }
            break;
        }

        if( wc == NULL ) // Don't miss this opportunity!
            break;
    }
}
#endif // DEBUG_REVISE_VOWELS


void SaltingPart1( )
{
    #if DO_DEBUG_SALTING
        Spew( L"Beginning salting task 1: Add Vowels and Consonants to CSol." );
    #endif

    // start of vows-cons clone of loop.

    int i = 0;
    CSol * pSol = & CSolVowelRunValues; // After 1 loop, change to cons.
    for( ;; )
    {
        // For each word list, scan the alternating count/letters.
        // Letters have been encoded: 0x01 to 0x20 = MSB prefix,
        // Otherwise, 0x21 to 0xff = character ( but NO digits ).

        unsigned char * scan = VowsConsLists[i];
        i++;

        if( scan == NULL )
            break;
        if( g_bStopInitThread ) // because of very fast program exit
            break;

        wchar_t wk[MAX_LEGITIMATE_WORD_LENGTH + 1 + 10]; // 10 is CYA
        // I have no CYA fill stop test, as input data is under limit.
        wchar_t letter;
        int fill = 0;
        int inalpha = 0;
        int CountInLanguage = 0;
        for( ;; )
        {
            BYTE c = ( BYTE ) *scan;

            if( c == NULL ) // Go add 1 digit after each last word...
                break;

            // First, a run of digits will accumulate the 'count'.
            // Then, a run of walpha will accumulate in wk[].
            // No word in salt lists will include any digits.
            // Just in case, change walpha to !isdigit.
            // Then, an INITIAL digit will add wk/count to list.

            if( c < 0x20 )
            {
                // get next byte, assemble letter over 0x00ff
                letter = c << 8;
                c = ( BYTE ) *++scan;
                if( c == NULL )
                    break;
                letter |= c;
                if( ! inalpha )
                {
                    inalpha = 1;
                    fill = 0;
                }
                wk[fill++] = letter;
            }
            else if( c > '9' || c < '0' )
            {
                // use c as letter up to 0x00ff
                if( ! inalpha )
                {
                    inalpha = 1;
                    fill = 0;
                }
                wk[fill++] = c;
            }
            else
            {
                // use c as a digit of a count

                if( inalpha )
                {
                    // First, wrap up any prior word.
                    wk[fill] = NULL;

                    size_t index = pSol->AddKey( wk );
                    #if DO_DEBUG_ADDFIND
                        if( index <= 1 )
                            { Spew( L"AddFind 1 at cvoc 2307" ); }
                    #endif

                    pSol->SetUserValue( index, CountInLanguage );

                    // Second, zero the next word counter

                    inalpha = 0;
                    CountInLanguage = 0;

                    // Deep enough in per-char loop to not be a burdensome test.
                    if( g_bStopInitThread ) // because of very fast program exit
                        break;
                }
                CountInLanguage *= 10;
                CountInLanguage += *scan - '0';
            }
            scan ++;
        }
        pSol = & CSolConsonantRunValues; // After 1 loop, change to cons.
    }
    // end of vows-cons clone.

    #if DO_DEBUG_SALTING
        SpewValue( L"CSolVowelRunValues.nList", CSolVowelRunValues.nList );
        SpewValue( L"CSolConsonantRunValues.nList", CSolConsonantRunValues.nList );
    #endif

    #if DO_DEBUG_SALTING
        Spew( L"Finished salting task 1." );
    #endif
}

#ifndef _WIN32_WCE
void SaltingPart2( )
{
    #if DO_DEBUG_SALTING
        Spew( L"Beginning salting task 2: Reserved, Proper, and Place names" );
    #endif


    // This was a really afu idea for awhile...
    // Instead of putting them here...
    // CSolProperNames.AddKey( wk );
    // I can simply put them here...

    CSol * pSolOneLG = Pag.CSolLGroupWords[ LANGUAGE_GROUP_UNKNOWN ];
    if( pSolOneLG == NULL )
    {
        // Later, create this CSol JIT when see a web page.
        pSolOneLG = new CSol( CSOL_SCALAR ); // create CSol jit
        Pag.CSolLGroupWords[ LANGUAGE_GROUP_UNKNOWN ] = pSolOneLG; // remember/own it
        TotalLanguageCount++;
    }


    extern UCPUI ProperNameList[];

    // start of proper name clone of loop.
    int i = 0;
    for( ;; )
    {
        // For each word list, scan the alternating count/letters.
        // Letters have been encoded: 0x01 to 0x20 = MSB prefix,
        // Otherwise, 0x21 to 0xff = character ( but NO digits ).

        unsigned char * scan = ProperNameList[i].ucp;
        i++;

        if( scan == NULL )
            break;
        if( g_bStopInitThread ) // because of very fast program exit
            break;

        wchar_t wk[MAX_LEGITIMATE_WORD_LENGTH + 1 + 10]; // 10 is CYA
        // I have no CYA fill stop test, as input data is under limit.
        wchar_t letter;
        int fill = 0;
        int inalpha = 0;
        int CountInLanguage = 99; // The same for all proper names
        int HadHigh = 0;
        for( ;; )
        {
            BYTE c = ( BYTE ) *scan;

            if( c == NULL ) // Go add 1 digit after each last word...
                break;

            // First, a run of digits will accumulate the 'count'.
            // Then, a run of walpha will accumulate in wk[].
            // No word in salt lists will include any digits.
            // Just in case, change walpha to !isdigit.
            // Then, an INITIAL digit will add wk/count to list.

            if( c < 0x20 )
            {
                // get next byte, assemble letter over 0x00ff
                letter = c << 8;
                c = ( BYTE ) *++scan;
                if( c == NULL )
                    break;
                letter |= c;
                if( ! inalpha )
                {
                    inalpha = 1;
                    fill = 0;
                }
                wk[fill++] = letter;
                HadHigh = 1;
            }
            else if( c > '9' || c < '0' )
            {
                // use c as letter up to 0x00ff
                if( ! inalpha )
                {
                    inalpha = 1;
                    fill = 0;
                }
                wk[fill++] = c;
                if( c > 0x7f )
                    HadHigh = 1;
            }
            else
            {
                // use c as a digit of a count

                if( inalpha )
                {
                    // First, wrap up any prior word.
                    wk[fill] = NULL;

                    // I decided to purge the proper name list
                    // during a re-build, by only keeping such
                    // names as are between 4 and 10 characters.

                    if( fill >= 4
                    && ! HadHigh
                    && fill <= 10 )
                    {
                        // Not...
                        // CSolProperNames.AddKey( wk );
                        // Now:
                        // pSolOneLG->AddKey( wk );
                        // No, not quite that simple...


                        // 8 MSB of UserValue hold an empirical measure of
                        // the commonality of the letter sequences in word.

                        // Then, next 7 bits << 17 encode language group id.

                        // 17 LSB of UserValue hold relative word frequency.

                        // Notice that although CSols's User.Value is signed,
                        // I am working in my own copied unsigned int variable.

                        size_t WordValue = WordLetterValuation( wk );
                        if( WordValue > MAX_WORD_VALUE )
                            WordValue = MAX_WORD_VALUE;
                        size_t ValShl24 = WordValue << 24;

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

                        {
                            // Addition to the per-language word list [ 0 ]:

                            #if OKAY_TO_SALT_PROPER

                                // Done atop this special loop...
                                // CSol * pSolOneLG = Pag.CSolLGroupWords[ LANGUAGE_GROUP_UNKNOWN ];
                                // if( pSolOneLG == NULL )
                                // {
                                //     // Later, create this CSol JIT when see a web page.
                                //     pSolOneLG = new CSol( CSOL_SCALAR ); // create CSol jit
                                //     Pag.CSolLGroupWords[ LANGUAGE_GROUP_UNKNOWN ] = pSolOneLG; // remember/own it
                                //     TotalLanguageCount++;
                                // }

                                size_t index1 = pSolOneLG->AddKey( wk );
                                #if DO_DEBUG_ADDFIND
                                    if( index1 == 1 )
                                        { Spew( L"AddFind 1 at cvoc 3587" ); }
                                #endif

                                // The salt counts have been normalized to 9999 max,
                                // so they always fit in 17 bits, which is ~128,000.

                                pSolOneLG->SetUserValue( index1, CountInLanguage | LANGUAGE_GROUP_UNKNOWN | ValShl24 ); //strictly, unk<<17

                            #endif // OKAY_TO_SALT_LISTS

                        }
                    }

                    // Second, zero the next word counter

                    inalpha = 0;

                    HadHigh = 0;

                    // all stuck at 99 ... CountInLanguage = 0;

                    // Deep enough in per-char loop to not be a burdensome test.
                    if( g_bStopInitThread ) // because of very fast program exit
                        break;
                }
                // all stuck at 99 ... CountInLanguage *= 10;
                // all stuck at 99 ... CountInLanguage += *scan - '0';
            }
            scan ++;
        }
    }
    // end of proper name clone.


    // To make counts of 100 not become 9999,
    // I need to add one word of 9999 counts:

    {
        // Addition to the per-language word list [ 0 ]:

        #if OKAY_TO_SALT_PROPER

            size_t index1 = pSolOneLG->AddKey( L"XXX" );
            pSolOneLG->SetUserValue( index1, 9999 );

        #endif // OKAY_TO_SALT_LISTS

    }


    #if DO_DEBUG_SALTING
        SpewValue( L"CSolProperNames.nList", CSolProperNames.nList );
    #endif

    #if DO_DEBUG_SALTING
        Spew( L"Finished salting task 2." );
    #endif
}
#endif // not _WIN32_WCE


void SaltingPart3( )
{
    #if DO_DEBUG_SALTING
        Spew( L"Beginning salting task 3: Build multi-lang CSols of words." );
    #endif

    extern UCPUI SaltLists[];

    int i = 0;
    for( ;; )
    {
        // For each word list, scan the alternating count/letters.
        // Letters have been encoded: 0x01 to 0x20 = MSB prefix,
        // Otherwise, 0x21 to 0xff = character ( but NO digits ).

        unsigned char * scan = SaltLists[i].ucp;
        unsigned int GroupID = SaltLists[i].ui;

        unsigned int GroupIDShl17 = GroupID << 17;

        if( scan == NULL )
            break;
        if( g_bStopInitThread ) // because of very fast program exit
            break;

        int WordsDone = 0;
        int NotNames = 0;
        int SumOfQty = 0;
        int SumOfVal = 0;

        wchar_t wk[MAX_LEGITIMATE_WORD_LENGTH + 1 + 10]; // 10 is CYA
        // I have no CYA fill stop test, as input data is under limit.
        wchar_t letter;
        int fill = 0;
        int inalpha = 0;
        int CountInLanguage = 0;
        for( ;; )
        {
            BYTE c = ( BYTE ) *scan;

            if( c == NULL ) // Go add 1 digit after each last word...
                break;

            // First, a run of digits will accumulate the 'count'.
            // Then, a run of walpha will accumulate in wk[].
            // No word in salt lists will include any digits.
            // Just in case, change walpha to !isdigit.
            // Then, an INITIAL digit will add wk/count to list.

            if( c < 0x20 )
            {
                // get next byte, assemble letter over 0x00ff
                letter = c << 8;
                c = ( BYTE ) *++scan;
                if( c == NULL )
                    break;
                letter |= c;
                if( ! inalpha )
                {
                    inalpha = 1;
                    fill = 0;
                }
                wk[fill++] = letter;
            }
            else if( c > '9' || c < '0' )
            {
                // use c as letter up to 0x00ff
                if( ! inalpha )
                {
                    inalpha = 1;
                    fill = 0;
                }
                wk[fill++] = c;
            }
            else
            {
                // use c as a digit of a count

                if( inalpha )
                {
                    // First, wrap up any prior word.
                    wk[fill] = NULL;

                    // 8 MSB of UserValue hold an empirical measure of
                    // the commonality of the letter sequences in word.

                    // Then, next 7 bits << 17 encode language group id.

                    // 17 LSB of UserValue hold relative word frequency.

                    // Notice that although CSols's User.Value is signed,
                    // I am working in my own copied unsigned int variable.

                    size_t WordValue = WordLetterValuation( wk );
                    if( WordValue > MAX_WORD_VALUE )
                        WordValue = MAX_WORD_VALUE;
                    size_t ValShl24 = WordValue << 24;

                    WordsDone ++;
                    SumOfQty += CountInLanguage;
                    SumOfVal += WordValue;

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

                    {
                        NotNames ++;

                        // Addition to the per-language word list:

                        #if OKAY_TO_SALT_LISTS

                            CSol * pSolOneLG = Pag.CSolLGroupWords[ GroupID ];
                            if( pSolOneLG == NULL )
                            {
                                // Later, create this CSol JIT when see a web page.
                                pSolOneLG = new CSol( CSOL_SCALAR ); // create CSol jit
                                Pag.CSolLGroupWords[ GroupID ] = pSolOneLG; // remember/own it
                                TotalLanguageCount++;
                            }
                            size_t index1 = pSolOneLG->AddKey( wk );
                            #if DO_DEBUG_ADDFIND
                                if( index1 == 1 )
                                    { Spew( L"AddFind 1 at cvoc 3587" ); }
                            #endif

                            // The salt counts have been normalized to 9999 max,
                            // so they always fit in 17 bits, which is ~128,000.

                            pSolOneLG->SetUserValue( index1, CountInLanguage | GroupIDShl17 | ValShl24 );

                        #endif // OKAY_TO_SALT_LISTS

                        // Addition to the single common word list:


                        #if OKAY_TO_SALT_COMMON

                            size_t index2 = CSolCommonWords.AddKey( wk );
                            #if DO_DEBUG_ADDFIND
                                if( index2 == 1 )
                                    { Spew( L"AddFind 1 at cvoc 3587" ); }
                            #endif

                            // CSolCommonWords might very well have this
                            // word from another language already salted.

                            // I doubt that all the salt lists can add up to max,
                            // but this sets the general example of how to do it.

                            size_t OldValue = CSolCommonWords.GetUserValue( index2 );

                            size_t ComnGidShl17 = OldValue & LGID_SHL17_MASK;
                            if( ComnGidShl17 == LGID_SHL17_UNKNOWN )
                                ComnGidShl17 = GroupIDShl17;
                            else
                                ComnGidShl17 = LGID_SHL17_MULTIPLE;

                            size_t CountInAll = OldValue & MASK_17BIT_WORD_COUNT;
                            CountInAll += CountInLanguage;
                            if( CountInAll > MAX_17BIT_WORD_COUNT )
                                CountInAll = MAX_17BIT_WORD_COUNT;

                            CSolCommonWords.SetUserValue( index2, CountInAll | ComnGidShl17 | ValShl24 );

                        #endif // OKAY_TO_SALT_COMMON


                        // This also must be temporary, because it is too late
                        // for me to be building CSolConsonantRunValues and
                        // CSolVowelRunValues using the word lists, because
                        // this same salt loop wants to know the VALUATIONS.
                        // So I can build one to output for a recompilation.

                        #if DEBUG_REVISE_VOWELS
                            BuildConsonantVowelRuns( wk, CountInLanguage );
                        #endif

                    }

                    // Second, zero the next word counter

                    inalpha = 0;
                    CountInLanguage = 0;

                    // Deep enough in per-char loop to not be a burdensome test.
                    if( g_bStopInitThread ) // because of very fast program exit
                        break;
                }
                CountInLanguage *= 10;
                CountInLanguage += *scan - '0';
            }
            scan ++;
        }

        // Having finished one i, tell me about it...
        #if DO_DEBUG_SALTING
            SpewValue( L"GroupID", GroupID );
            SpewValue( L"WordsDone", WordsDone );
            SpewValue( L"NotNames", NotNames );
            SpewValue( L"SumOfQty", SumOfQty );
            SpewValue( L"SumOfVal", SumOfVal );
        #endif

        i++;
    }

    #if DO_DEBUG_SALTING
        Spew( L"Finished salting task 3." );
    #endif
}


void SaltingPart4( )
{
    #if DO_DEBUG_SALTING
        Spew( L"Beginning salting task 4: " );
    #endif

    // Now that I have created them, prepare a debugging output
    // describing all the words that appear in multiple lists.
    // For example, many NOT and THAT are perverting EN to MUL!

    #if DO_DEBUG_LANG_MULT
    {
        int CountOfMultipleWords = 0;
        CoIt * pMalVector = CSolCommonWords.GetSortedVector( CSOL_FORWARD );
        if( pMalVector != NULL )
        {
            size_t take = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector + take++;
                if( pCoIt->IsSentinel )
                    break;
                wchar_t * FullKey = CoItFullKey( pCoIt );

                size_t ThreeBitFields = pCoIt->User.Value;
                size_t ComnGidShl17 = ThreeBitFields & LGID_SHL17_MASK;
                if( ComnGidShl17 == LGID_SHL17_MULTIPLE )
                {
                    // This is one of my schizo problem words.
                    // Look it up in all the per-language lists.
                    // Report it in a good line-sortable fashion:
                    // TheWordItself, fixed-size-count, which-language-abbrev.

                    CountOfMultipleWords ++;

                    // Wow! I got huge problems: CountOfMultipleWords: 15456
                    //  a,    203, af
                    //  a,     64, ar
                    //  a,   5609, az
                    //  a,     52, bg
                    //  a,   5616, ca
                    //  a,   9999, cs
                    //  a,   1451, da
                    //  a,    552, de
                    //  a,    152, el
                    // Useless: \777\777\777\777\777\777\777,     74, uk
                    // Since that foobars my print columns, rid per rule:
                    // Also rid digits, for a simplistic SED of output.

                    int HasBigs = 0;
                    wchar_t * scan = FullKey;
                    for( ;; )
                    {
                        wchar_t wc = *scan ++;
                        if( wc == NULL )
                            break;
                        if( wc < ' '
                        || wc > '~' // Same rule as in Spew.
                        || isdigit( wc ) ) // ascii test okay here
                        {
                            HasBigs = 1;
                            break;
                        }
                    }

                    if( ! HasBigs )
                    {
                        int i = 0;
                        for( ;; )
                        {
                            CSol * pSolOneLG = Pag.CSolLGroupWords[ i ];
                            // At this time, many language CSols are null ptrs.
                            if( pSolOneLG != NULL )
                            {
                                size_t index = pSolOneLG->Find( FullKey );
                                #if DO_DEBUG_ADDFIND
                                    if( index == 1 )
                                        { Spew( L"AddFind 1 at cvoc 3949" ); }
                                #endif
                                if( index > 1 )
                                {
                                    // The word was listed in this language.
                                    int ThreeBitFields = pSolOneLG->GetUserValue( index );
                                    int WordCount = ThreeBitFields & MASK_17BIT_WORD_COUNT;
                                    wchar_t * LangAbbrev = GroupAbbreviationforGroupIndex( i );
                                    wchar_t wk[ 120 + MAX_LEGITIMATE_WORD_LENGTH ];

                                    // Spew needs to come up to speed on Unicode!:
                                    // MUL: \777\777\777\777\777\777\777,     53, ...
                                    // Meanwhile, I should format the same 4-octals
                                    // as I use in the word dumper that makes lists.

                                    wsprintf( wk, L"MUL: %60s, %6d, %s", FullKey, WordCount, LangAbbrev );
                                    Spew( wk );
                                }
                            }
                            if( ++i == PAST_LANGUAGE_GROUP_IDS )
                                break;
                        }
                    }
                }
                MyFree( 3931, zx, FullKey );
                FullKey = NULL;
            }
            MyFree( 3934, UNPREDICTABLE, pMalVector );
            pMalVector = NULL;
        }

        SpewValue( L"CountOfMultipleWords", CountOfMultipleWords );

    }
    #endif

    #if DO_DEBUG_SALTING
        Spew( L"Finished salting task 4." );
    #endif
}

void SaltCommonAndPerLanguageWordLists( )
{
    // This is called by a thread after program start-up.

    // No, now CLan.cpp contains many per-language lists,
    // and this vector which points to all of the blocks:
    // It also identifies their language group ID number.
    // typedef struct _UCPUI
    // {
    //     unsigned char * ucp;
    //     unsigned int ui;
    // } UCPUI;
    extern UCPUI SaltLists[];

    // Add them all into CSolCommonWords, so that foreign words
    // will not inflate a paper's value over known common words.

    // Add them each into one Pag.CSolLGroupWords[ i ] now,
    // so that I may use those CSols for language guessing.
    // Later, I must decide how to differentiate real words
    // and languages encounter counts from the salted lists.

    // Before doing that, salt the proper names

    // While doing all of that, build new Consonant-Vowel runs data.
    // -- That is, to output and re-include upon a later compile.

    // Before doing ANY of that, use another clone of these loops
    // to salt the vows and cons from the lists above.

    // Wadda mess.

    SaltingPart1( );

    #ifndef _WIN32_WCE
        SaltingPart2( );
    #endif // not _WIN32_WCE

    SaltingPart3( );

    SaltingPart4( );

}

void InitHtmlEntities( )
{
    int i = 0;
    for( ;; )
    {
        ENTITY * pEntity = Entities + i++;
        if( pEntity->wcp == NULL )
            break;

        // This BC in name means BOTH CASES were preserved:

        size_t index = CSolHtmlBcEntities.AddKey( pEntity->wcp ); // &<verbatim>;
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 3614" ); }
        #endif
        CSolHtmlBcEntities.SetUserValue( index, pEntity->wc ); // replacement byte

        wchar_t wk[20];
        wchar_t * into = wk;
        wchar_t * from = pEntity->wcp;
        for( ;; )
        {
            wchar_t c = *from++;
            if( c == NULL )
                break;
            *into++ = c | ' '; // Lc = lowercase
        }
        *into = NULL;

        // This LC in name means LOWERCASED for two lookup tries:

        index = CSolHtmlLcEntities.AddKey( wk ); // &<lowercased>;
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 3633" ); }
        #endif
        CSolHtmlLcEntities.SetUserValue( index, pEntity->wc ); // replacement byte
    }
}

typedef struct _charsets {
    wchar_t * PreferredName;
    char * Aliases;
    int ExactCodePage;
    int FamilyCodePage;
} CHARSETS;

CHARSETS CharSets[] = {
  { L"Unknown", "", 1252, 1252 },
  { L"ASMO-708", "", 708, 1256 },
  { L"DOS-720", "", 720, 1256 },
  { L"iso-8859-6", "arabic, csISOLatinArabic, ECMA-114, ISO_8859-6, ISO_8859-6:1987, iso-ir-127 ", 28596, 1256 },
  { L"x-mac-arabic", "", 10004, 1256 },
  { L"windows-1256", "cp1256 ", 1256, 1256 },
  { L"ibm775", "CP500 ", 775, 1257 },
  { L"iso-8859-4", "csISOLatin4, ISO_8859-4, ISO_8859-4:1988, iso-ir-110, l4, latin4 ", 28594, 1257 },
  { L"windows-1257", "", 1257, 1257 },
  { L"ibm852", "cp852 ", 852, 1250 },
  { L"iso-8859-2", "csISOLatin2, iso_8859-2, iso_8859-2:1987, iso8859-2, iso-ir-101, l2, latin2 ", 28592, 1250 },
  { L"x-mac-ce", "", 10029, 1250 },
  { L"windows-1250", "x-cp1250 ", 1250, 1250 },
  { L"EUC-CN", "x-euc-cn ", 51936, 936 },
  { L"gb2312", "chinese, CN-GB, csGB2312, csGB231280, csISO58GB231280, GB_2312-80, GB231280, GB2312-80, GBK, iso-ir-58 ", 936, 936 },
  { L"hz-gb-2312", "", 52936, 936 },
  { L"x-mac-chinesesimp", "", 10008, 936 },
  { L"big5", "cn-big5, csbig5, x-x-big5 ", 950, 950 },
  { L"x-Chinese-CNS", "", 20000, 950 },
  { L"x-Chinese-Eten", "", 20002, 950 },
  { L"x-mac-chinesetrad", "", 10002, 950 },
  { L"cp866", "ibm866 ", 866, 1251 },
  { L"iso-8859-5", "csISOLatin5, csISOLatinCyrillic, cyrillic, ISO_8859-5, ISO_8859-5:1988, iso-ir-144, l5 ", 28595, 1251 },
  { L"koi8-r", "csKOI8R, koi, koi8, koi8r ", 20866, 1251 },
  { L"koi8-u", "koi8-ru ", 21866, 1251 },
  { L"x-mac-cyrillic", "", 10007, 1251 },
  { L"windows-1251", "x-cp1251 ", 1251, 1251 },
  { L"x-Europa", "", 29001, 1252 },
  { L"x-IA5-German", "", 20106, 1252 },
  { L"ibm737", "", 737, 1253 },
  { L"iso-8859-7", "csISOLatinGreek, ECMA-118, ELOT_928, greek, greek8, ISO_8859-7, ISO_8859-7:1987, iso-ir-126 ", 28597, 1253 },
  { L"x-mac-greek", "", 10006, 1253 },
  { L"windows-1253", "", 1253, 1253 },
  { L"ibm869", "", 869, 1253 },
  { L"DOS-862", "", 862, 1255 },
  { L"iso-8859-8-i", "logical ", 38598, 1255 },
  { L"iso-8859-8", "csISOLatinHebrew, hebrew, ISO_8859-8, ISO_8859-8:1988, ISO-8859-8, iso-ir-138, visual ", 28598, 1255 },
  { L"x-mac-hebrew", "", 10005, 1255 },
  { L"windows-1255", "ISO_8859-8-I, ISO-8859-8, visual ", 1255, 1255 },
  { L"x-EBCDIC-Arabic", "", 20420, 1256 },
  { L"x-EBCDIC-CyrillicRussian", "", 20880, 1251 },
  { L"x-EBCDIC-CyrillicSerbianBulgarian", "", 21025, 1251 },
  { L"x-EBCDIC-DenmarkNorway", "", 20277, 1252 },
  { L"x-ebcdic-denmarknorway-euro", "", 1142, 1252 },
  { L"x-EBCDIC-FinlandSweden", "", 20278, 1252 },
  { L"x-ebcdic-finlandsweden-euro", "X-EBCDIC-France ", 1143, 1252 },
  { L"x-ebcdic-france-euro", "", 1147, 1252 },
  { L"x-EBCDIC-Germany", "", 20273, 1252 },
  { L"x-ebcdic-germany-euro", "", 1141, 1252 },
  { L"x-EBCDIC-GreekModern", "", 875, 1253 },
  { L"x-EBCDIC-Greek", "", 20423, 1253 },
  { L"x-EBCDIC-Hebrew", "", 20424, 1255 },
  { L"x-EBCDIC-Icelandic", "", 20871, 1252 },
  { L"x-ebcdic-icelandic-euro", "", 1149, 1252 },
  { L"x-ebcdic-international-euro", "", 1148, 1252 },
  { L"x-EBCDIC-Italy", "", 20280, 1252 },
  { L"x-ebcdic-italy-euro", "", 1144, 1252 },
  { L"x-EBCDIC-JapaneseAndKana", "", 50930, 932 },
  { L"x-EBCDIC-JapaneseAndJapaneseLatin", "", 50939, 932 },
  { L"x-EBCDIC-JapaneseAndUSCanada", "", 50931, 932 },
  { L"x-EBCDIC-JapaneseKatakana", "", 20290, 932 },
  { L"x-EBCDIC-KoreanAndKoreanExtended", "", 50933, 949 },
  { L"x-EBCDIC-KoreanExtended", "", 20833, 949 },
  { L"CP870", "", 870, 1250 },
  { L"x-EBCDIC-SimplifiedChinese", "", 50935, 936 },
  { L"X-EBCDIC-Spain", "", 20284, 1252 },
  { L"x-ebcdic-spain-euro", "", 1145, 1252 },
  { L"x-EBCDIC-Thai", "", 20838, 874 },
  { L"x-EBCDIC-TraditionalChinese", "", 50937, 950 },
  { L"CP1026", "", 1026, 1254 },
  { L"x-EBCDIC-Turkish", "", 20905, 1254 },
  { L"x-EBCDIC-UK", "", 20285, 1252 },
  { L"x-ebcdic-uk-euro", "", 1146, 1252 },
  { L"ebcdic-cp-us", "", 37, 1252 },
  { L"x-ebcdic-cp-us-euro", "", 1140, 1252 },
  { L"ibm861", "", 861, 1252 },
  { L"x-mac-icelandic", "", 10079, 1252 },
  { L"x-iscii-as", "", 57006, 57006 },
  { L"x-iscii-be", "", 57003, 57003 },
  { L"x-iscii-de", "", 57002, 57002 },
  { L"x-iscii-gu", "", 57010, 57010 },
  { L"x-iscii-ka", "", 57008, 57008 },
  { L"x-iscii-ma", "", 57009, 57009 },
  { L"x-iscii-or", "", 57007, 57007 },
  { L"x-iscii-pa", "", 57011, 57011 },
  { L"x-iscii-ta", "", 57004, 57004 },
  { L"x-iscii-te", "", 57005, 57005 },
  { L"euc-jp", "csEUCPkdFmtJapanese, Extended_UNIX_Code_Packed_Format_for_Japanese, x-euc, x-euc-jp ", 51932, 932 },
  { L"iso-2022-jp", "_iso-2022-jp$SIO ", 50222, 932 },
  { L"csISO2022JP", "_iso-2022-jp ", 50221, 932 },
  { L"x-mac-japanese", "", 10001, 932 },
  { L"shift_jis", "csShiftJIS, csWindows31J, ms_Kanji, shift-jis, x-ms-cp932, x-sjis ", 932, 932 },
  { L"ks_c_5601-1987", "csKSC56011987, euc-kr, iso-ir-149, korean, ks_c_5601, ks_c_5601_1987, ks_c_5601-1989, KSC_5601, KSC5601 ", 949, 949 },
  { L"euc-kr", "csEUCKR ", 51949, 949 },
  { L"iso-2022-kr", "csISO2022KR ", 50225, 949 },
  { L"Johab", "", 1361, 1361 },
  { L"x-mac-korean", "", 10003, 949 },
  { L"iso-8859-3", "csISO, Latin3, ISO_8859-3, ISO_8859-3:1988, iso-ir-109, l3, latin3 ", 28593, 1254 },
  { L"iso-8859-15", "csISO, Latin9, ISO_8859-15, l9, latin9 ", 28605, 1252 },
  { L"x-IA5-Norwegian", "", 20108, 1252 },
  { L"IBM437", "437, cp437, csPC8, CodePage437 ", 437, 1252 },
  { L"x-IA5-Swedish", "", 20107, 1252 },
  { L"windows-874", "DOS-874, iso-8859-11, TIS-620 ", 874, 874 },
  { L"ibm857", "", 857, 1254 },
  { L"iso-8859-9", "csISO, Latin5, ISO_8859-9, ISO_8859-9:1989, iso-ir-148, l5, latin5 ", 28599, 1254 },
  { L"x-mac-turkish", "", 10081, 1254 },
  { L"windows-1254", "ISO_8859-9, ISO_8859-9:1989, iso-8859-9, iso-ir-148, latin5 ", 1254, 1254 },
  { L"unicode", "utf-16 ", 1200, 1200 },
  { L"unicodeFFFE", "", 1201, 1200 },
  { L"utf-7", "csUnicode11UTF7, unicode-1-1-utf-7, x-unicode-2-0-utf-7 ", 65000, 1200 },
  { L"utf-8", "unicode-1-1-utf-8, unicode-2-0-utf-8, x-unicode-2-0-utf-8 ", 65001, 1200 },
  { L"windows-1258", "", 1258, 1258 },
  { L"ibm850", "", 850, 1252 },
  { L"x-IA5", "", 20105, 1252 },
  { L"iso-8859-1", "cp819, csISO, Latin1, ibm819, iso_8859-1, iso_8859-1:1987, iso8859-1, iso-ir-100, l1, latin1 ", 28591, 1252 },
  { L"macintosh", "", 10000, 1252 },
  { L"Windows-1252", "ANSI_X3.4-1968, ANSI_X3.4-1986, ascii, cp367, cp819, csASCII, IBM367, ibm819, ISO_646.irv:1991, iso_8859-1, iso_8859-1:1987, ISO646-US, iso8859-1, iso-8859-1, iso-ir-100, iso-ir-6, latin1, us, us-ascii, x-ansi ", 1252, 1252 },
    // I moved the ascii line to after the 1252 line, to prefer this label:
  { L"us-ascii", "ANSI_X3.4-1968, ANSI_X3.4-1986, ascii, cp367, csASCII, IBM367, ISO_646.irv:1991, ISO646-US, iso-ir-6us ", 20127, 1252 },
  { NULL, NULL, 0, 0 }
};

void SetupCharsetNames( )
{
    // I need to add all the preferred names, with their indexes;
    // And add all the non-preferred names mapped into the index
    // of their preferred name. Add LC variants w/o punctuation.

    // I will put all the names, and variants, into CSolCharsets;
    // The CSolCharsets User.Value can be an index in list above;
    // Notice that my #0 item above will serve for all not-found.
    // Callers will pass names to my helpers for lookup via list.

    // Non-zeros in pOnePaper->HttpHeaderCharset can get labeled.
    // Note that it will be possible for zero to get labeled, as:
    // Unknown: 0 -- Which is the value of enum, CHARSET_UNKNOWN

    // typedef struct _charsets {
    //     wchar_t * PreferredName;
    //     char * Aliases;
    //     int ExactCodePage;
    //     int FamilyCodePage;
    // } CHARSETS;
    //
    // CHARSETS CharSets[] = {
    //   { "Unknown", "", 1252, 1252 },
    //   ...
    //   { "Windows-1252", "ANSI_X3.4-1968, ANSI_X3.4-1986, ascii, cp367, cp819, csASCII, IBM367, ibm819, ISO_646.irv:1991, iso_8859-1, iso_8859-1:1987, ISO646-US, iso8859-1, iso-8859-1, iso-ir-100, iso-ir-6, latin1, us, us-ascii, x-ansi ", 1252, 1252 },
    //   { NULL, NULL, 0, 0 }

    // sorting spewed output shows a few redundancies, like ascii, csiso.

    int i = 0;
    for( ;; )
    {
        CHARSETS * p = CharSets + i;
        if( p->ExactCodePage == 0 )
            break;
        wchar_t wk[60];

        // 1. Verbatim copy of preferred name.
        {
            wchar_t * into = wk;
            wchar_t * from = p->PreferredName;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                *into++ = c;
            }
            size_t index = CSolCharsets.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3832" ); }
            #endif
            // give me an analysis of alias conflicts ( 4 plcs ):
            #if DO_DEBUG_CONTENT
            {
                int was = CSolCharsets.GetUserValue( index );
                if( was != 0
                && was != i )
                {
                    ; SpewValue( L"CONFLICT:", was );
                }
            }
            #endif
            CSolCharsets.SetUserValue( index, i ); // save CharSets[i] index.
            #if DO_DEBUG_CONTENT
                ; SpewValue( wk, i );
            #endif
        }

        // 2. Lowercased isalum-only letters of preferred name.
        {
            wchar_t * into = wk;
            wchar_t * from = p->PreferredName;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                if( iswdigit( c ) )
                    *into++ = c;
                else if( iswalpha( c ) )
                    *into++ = c | ' ';
            }
            size_t index = CSolCharsets.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3858" ); }
            #endif
            // give me an analysis of alias conflicts ( 4 plcs ):
            #if DO_DEBUG_CONTENT
            {
                int was = CSolCharsets.GetUserValue( index );
                if( was != 0
                && was != i )
                {
                    ; SpewValue( L"CONFLICT:", was );
                }
            }
            #endif
            CSolCharsets.SetUserValue( index, i ); // save CharSets[i] index.
            #if DO_DEBUG_CONTENT
                ; SpewValue( wk, i );
            #endif
        }

        // 3. Scan across each comma-delimited part of aliases, if any.
        if( p->Aliases[0] != NULL )
        {
            // Aliases string is not empty.
            // Scan to locate boundaries of each alias name.
            // Stop on each comma, or NULL, or also a space;
            // Skip over any next comma or space characters.
            char * scan = p->Aliases;
            char * atop = scan;
            for( ;; )
            {
                if( *scan == NULL
                || *scan == ','
                || *scan == ' ' ) // safe: in my table
                {

                    // In debug mode, I could not write a NULL over
                    // the initializer string, got memory violation.
                    // So fix loops to not require a NULL in copies.

                    // char held = *scan;
                    // Put final NULL at scan.
                    // *scan = NULL;

                    // Do atop to final NULL.

                    // 3.1. Verbatim copy of alias name.
                    {
                        wchar_t * into = wk;
                        char * from = atop;
                        for( ;; )
                        {
                            if( from == scan )
                            {
                                *into = NULL;
                                break;
                            }
                            wchar_t c = *from++;
                            *into++ = c;
                        }
                        size_t index = CSolCharsets.AddKey( wk );
                        #if DO_DEBUG_ADDFIND
                            if( index <= 1 )
                                { Spew( L"AddFind 1 at cvoc 3901" ); }
                        #endif
                        // give me an analysis of alias conflicts ( 4 plcs ):
                        #if DO_DEBUG_CONTENT
                        {
                            int was = CSolCharsets.GetUserValue( index );
                            if( was != 0
                            && was != i )
                            {
                                ; SpewValue( L"CONFLICT:", was );
                            }
                        }
                        #endif
                        CSolCharsets.SetUserValue( index, i ); // save CharSets[i] index.
                        #if DO_DEBUG_CONTENT
                            ; SpewValue( wk, i );
                        #endif
                    }

                    // 3.2. Lowercased isalum-only letters of alias name.
                    {
                        wchar_t * into = wk;
                        char * from = atop;
                        for( ;; )
                        {
                            if( from == scan )
                            {
                                *into = NULL;
                                break;
                            }
                            wchar_t c = *from++;
                            if( iswdigit( c ) )
                                *into++ = c;
                            else if( iswalpha( c ) )
                                *into++ = c | ' ';
                        }
                        size_t index = CSolCharsets.AddKey( wk );
                        #if DO_DEBUG_ADDFIND
                            if( index <= 1 )
                                { Spew( L"AddFind 1 at cvoc 3929" ); }
                        #endif
                        // give me an analysis of alias conflicts ( 4 plcs ):
                        #if DO_DEBUG_CONTENT
                        {
                            int was = CSolCharsets.GetUserValue( index );
                            if( was != 0
                            && was != i )
                            {
                                ; SpewValue( L"CONFLICT:", was );
                            }
                        }
                        #endif
                        CSolCharsets.SetUserValue( index, i ); // save CharSets[i] index.
                        #if DO_DEBUG_CONTENT
                            ; SpewValue( wk, i );
                        #endif
                    }

                    // Done. undo final NULL.
                    // No... *scan = held;

                    if( *scan == NULL )
                        break;
                    // Skip over any comma.
                    // Skip over any space.
                    // Retest for NULL.
                    if( *scan == ',' )
                        scan++;
                    if( *scan == ' ' ) // safe: in my table
                        scan++;
                    if( *scan == NULL )
                        break;
                    atop = scan;
                    continue; // already did the scan++.
                }
                scan++;
            }
        }

        i++;
    }
}


size_t BestIndexforCharsetString( wchar_t * pName, wchar_t * pPast )
{
    // Note that the "BestIndex" is not into a CSol, but my data array.
    int arrayindex = 0;
    wchar_t wk[60];

    // If try 1 fails for nomatch, do not RETURN, but go on to try 2.

    // 1. Verbatim copy of preferred name.
    {
        #if DO_DEBUG_CONTENT
            ; Spew( L"BestIndex: try 1. Verbatim" );
        #endif
        wchar_t * into = wk;
        wchar_t * from = pName;
        for( ;; )
        {
            if( from == pPast )
            {
                *into = NULL;
                break;
            }
            wchar_t c = *from++;
            if( c == NULL )
            {
                *into = NULL;
                break;
            }
            if( into == wk + 58 )
            {
                #if DO_DEBUG_CONTENT
                    ; Spew( L"BestIndex: too long" );
                #endif
                return 0; // too long a name? return 0 for "??"
            }
            *into++ = c;
        }
        if( into == wk )
        {
            #if DO_DEBUG_CONTENT
                ; Spew( L"BestIndex: empty" );
            #endif
            return 0; // empty name? return 0 for "??"
        }
        size_t index = CSolCharsets.Find( wk );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 4003" ); }
        #endif
        #if DO_DEBUG_CONTENT
            ; SpewValue( wk, index );
        #endif
        if( index == 0 )
        {
            #if DO_DEBUG_CONTENT
                ; Spew( L"BestIndex: unrecognized" );
            #endif
            // Oops. Go on to phase 2...
            // return 0; // unrecognized name? return 0 for "??"
        }
        else
        {
            arrayindex = CSolCharsets.GetUserValue( index );
            #if DO_DEBUG_CONTENT
                ; SpewValue( L"BestIndex: Return value = ", arrayindex );
            #endif
            return arrayindex;
        }
    }

    // 2. Lowercased isalum-only letters of preferred name.
    {
        #if DO_DEBUG_CONTENT
            ; Spew( L"BestIndex: try 2. Lowercased" );
        #endif
        wchar_t * into = wk;
        wchar_t * from = pName;
        for( ;; )
        {
            if( from == pPast )
            {
                *into = NULL;
                break;
            }
            wchar_t c = *from++;
            if( c == NULL )
            {
                *into = NULL;
                break;
            }
            if( into == wk + 58 )
            {
                #if DO_DEBUG_CONTENT
                    ; Spew( L"BestIndex: too long" );
                #endif
                return 0; // too long a name? return 0 for "??"
            }
            if( iswdigit( c ) )
                *into++ = c;
            else if( iswalpha( c ) )
                *into++ = c | ' ';
        }
        if( into == wk )
        {
            #if DO_DEBUG_CONTENT
                ; Spew( L"BestIndex: empty" );
            #endif
            return 0; // empty name? return 0 for "??"
        }
        size_t index = CSolCharsets.Find( wk );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 4041" ); }
        #endif
        #if DO_DEBUG_CONTENT
            ; SpewValue( wk, index );
        #endif
        if( index == 0 )
        {
            #if DO_DEBUG_CONTENT
                ; Spew( L"BestIndex: unrecognized" );
            #endif
            return 0; // unrecognized name? return 0 for "??"
        }
        arrayindex = CSolCharsets.GetUserValue( index );
        #if DO_DEBUG_CONTENT
            ; SpewValue( L"Value", arrayindex );
        #endif
        return arrayindex;
    }
}

wchar_t * BestCharsetNameforIndex( size_t index )
{
    #if DO_DEBUG_CONTENT
        ; SpewValue( L"BestCharsetName: Parameter = ", index );
    #endif
    // Note that the "BestIndex" is not into a CSol, but my data array.
    if( index >= sizeof( CharSets ) / sizeof( * CharSets ) - 1 )
        index = 0;

    #if DO_DEBUG_CONTENT
        ; Spew( CharSets[ index ].PreferredName );
    #endif
    return CharSets[ index ].PreferredName;
}


size_t FamilyCodePageforCharsetIndex( size_t index )
{
    #if DO_DEBUG_CONTENT
        ; SpewValue( L"FamilyCodePage: Parameter = ", index );
    #endif
    if( index >= sizeof( CharSets ) / sizeof( * CharSets ) - 1 )
        index = 0;
    #if DO_DEBUG_CONTENT
        ; SpewValue( L"FamilyCodePage: Returning ", CharSets[ index ].FamilyCodePage );
    #endif
    return CharSets[ index ].FamilyCodePage;
}

// Legends to use for the LGroupID, which no. is in table after this.
// These strings serve to make subfolder names in CFio::SaveFolder.
// The LGroupID itself may index a vector of word list CSols, etc.

wchar_t * LanguageGroupList[ PAST_LANGUAGE_GROUP_IDS ] = {
    L"un",     //   0 = unknown
    L"af",     //   1
    L"am",     //   2
    L"ar",     //   3
    L"arn",    //   4
    L"as",     //   5
    L"az",     //   6
    L"ba",     //   7
    L"be",     //   8
    L"bg",     //   9
    L"bn",     //  10
    L"bo",     //  11
    L"br",     //  12
    L"bs",     //  13
    L"ca",     //  14
    L"co",     //  15
    L"cs",     //  16
    L"cy",     //  17
    L"da",     //  18
    L"de",     //  19
    L"div",    //  20
    L"el",     //  21
    L"en",     //  22 -- GROUP 22 is english, a fact I hard coded...
    L"es",     //  23
    L"et",     //  24
    L"eu",     //  25
    L"fa",     //  26
    L"fi",     //  27
    L"fil",    //  28
    L"fo",     //  29
    L"fr",     //  30
    L"fy",     //  31
    L"ga",     //  32
    L"gl",     //  33
    L"gsw",    //  34
    L"gu",     //  35
    L"ha",     //  36
    L"he",     //  37
    L"hi",     //  38
    L"hr",     //  39
    L"hu",     //  40
    L"hy",     //  41
    L"id",     //  42
    L"ig",     //  43
    L"ii",     //  44
    L"is",     //  45
    L"it",     //  46
    L"iu",     //  47
    L"ja",     //  48
    L"ka",     //  49
    L"kk",     //  50
    L"kl",     //  51
    L"km",     //  52
    L"kn",     //  53
    L"ko",     //  54
    L"kok",    //  55
    L"ky",     //  56
    L"lb",     //  57
    L"lo",     //  58
    L"lt",     //  59
    L"lv",     //  60
    L"mi",     //  61
    L"mk",     //  62
    L"ml",     //  63
    L"mn",     //  64
    L"moh",    //  65
    L"mr",     //  66
    L"ms",     //  67
    L"mt",     //  68
    L"nb",     //  69
    L"ne",     //  70
    L"nl",     //  71
    L"nn",     //  72
    L"no",     //  73
    L"nso",    //  74
    L"oc",     //  75
    L"or",     //  76
    L"pa",     //  77
    L"pl",     //  78
    L"prs",    //  79
    L"ps",     //  80
    L"pt",     //  81
    L"qut",    //  82
    L"quz",    //  83
    L"rm",     //  84
    L"ro",     //  85
    L"ru",     //  86
    L"rw",     //  87
    L"sa",     //  88
    L"sah",    //  89
    L"se",     //  90
    L"si",     //  91
    L"sk",     //  92
    L"sl",     //  93
    L"sma",    //  94
    L"smj",    //  95
    L"smn",    //  96
    L"sms",    //  97
    L"sq",     //  98
    L"sr",     //  99
    L"sv",     // 100
    L"sw",     // 101
    L"syr",    // 102
    L"ta",     // 103
    L"te",     // 104
    L"tg",     // 105
    L"th",     // 106
    L"tk",     // 107
    L"tmz",    // 108
    L"tn",     // 109
    L"tr",     // 110
    L"tt",     // 111
    L"ug",     // 112
    L"uk",     // 113
    L"ur",     // 114
    L"uz",     // 115
    L"vi",     // 116
    L"wee",    // 117
    L"wen",    // 118
    L"wo",     // 119
    L"xh",     // 120
    L"yo",     // 121
    L"zh",     // 122
    L"zu",     // 123
};

wchar_t * LanguageGroupName[ PAST_LANGUAGE_GROUP_IDS ] = {
    L"Unknown",           //    0
    L"Afrikaans",         //    1
    L"Amharic",           //    2
    L"Arabic",            //    3
    L"Mapudungun",        //    4
    L"Assamese",          //    5
    L"Azeri",             //    6
    L"Bashkir",           //    7
    L"Belarusian",        //    8
    L"Bulgarian",         //    9
    L"Bengali",           //   10
    L"Tibetan",           //   11
    L"Breton",            //   12
    L"Bosnian",           //   13
    L"Catalan",           //   14
    L"Corsican",          //   15
    L"Czech",             //   16
    L"Welsh",             //   17
    L"Danish",            //   18
    L"German",            //   19
    L"Divehi",            //   20
    L"Greek",             //   21
    L"English",           //   22
    L"Spanish",           //   23
    L"Estonian",          //   24
    L"Basque",            //   25
    L"Persian",           //   26
    L"Finnish",           //   27
    L"Filipino",          //   28
    L"Faroese",           //   29
    L"French",            //   30
    L"Frisian",           //   31
    L"Irish",             //   32
    L"Galician",          //   33
    L"Alsatian",          //   34
    L"Gujarati",          //   35
    L"Hausa",             //   36
    L"Hebrew",            //   37
    L"Hindi",             //   38
    L"Croatian",          //   39
    L"Hungarian",         //   40
    L"Armenian",          //   41
    L"Indonesian",        //   42
    L"Igbo",              //   43
    L"Yi",                //   44
    L"Icelandic",         //   45
    L"Italian",           //   46
    L"Inuktitut",         //   47
    L"Japanese",          //   48
    L"Georgian",          //   49
    L"Kazakh",            //   50
    L"Greenlandic",       //   51
    L"Khmer",             //   52
    L"Kannada",           //   53
    L"Korean",            //   54
    L"Konkani",           //   55
    L"Kyrgyz",            //   56
    L"Luxembourgish",     //   57
    L"Lao",               //   58
    L"Lithuanian",        //   59
    L"Latvian",           //   60
    L"Maori",             //   61
    L"Macedonian",        //   62
    L"Malayalam",         //   63
    L"Mongolian",         //   64
    L"Mohawk",            //   65
    L"Marathi",           //   66
    L"Malay",             //   67
    L"Maltese",           //   68
    L"Norwegian, Bokml",  //   69
    L"Nepali",            //   70
    L"Dutch",             //   71
    L"Norwegian, Nynorsk",//   72
    L"Norwegian",         //   73
    L"Sesotho sa Leboa",  //   74
    L"Occitan",           //   75
    L"Oriya",             //   76
    L"Punjabi",           //   77
    L"Polish",            //   78
    L"Dari",              //   79
    L"Pashto",            //   80
    L"Portuguese",        //   81
    L"K'iche",            //   82
    L"Quechua",           //   83
    L"Romansh",           //   84
    L"Romanian",          //   85
    L"Russian",           //   86
    L"Kinyarwanda",       //   87
    L"Sanskrit",          //   88
    L"Yakut",             //   89
    L"Sami, Northern",    //   90
    L"Sinhala",           //   91
    L"Slovak",            //   92
    L"Slovenian",         //   93
    L"Sami, Southern",    //   94
    L"Sami, Lule",        //   95
    L"Sami, Inari",       //   96
    L"Sami, Skolt",       //   97
    L"Albanian",          //   98
    L"Serbian",           //   99
    L"Swedish",           //  100
    L"Kiswahili",         //  101
    L"Syriac",            //  102
    L"Tamil",             //  103
    L"Telugu",            //  104
    L"Tajik",             //  105
    L"Thai",              //  106
    L"Turkmen",           //  107
    L"Tamazight",         //  108
    L"Setswana",          //  109
    L"Turkish",           //  110
    L"Tatar",             //  111
    L"Uighur",            //  112
    L"Ukrainian",         //  113
    L"Urdu",              //  114
    L"Uzbek",             //  115
    L"Vietnamese",        //  116
    L"Lower Sorbian",     //  117
    L"Upper Sorbian",     //  118
    L"Wolof",             //  119
    L"isiXhosa",          //  120
    L"Yoruba",            //  121
    L"Chinese",           //  122
    L"isiZulu",           //  123
};

// These lines were constructed by the program x:\xec\ve\lang\lang.cpp
// From web page at http://www.microsoft.com/globaldev/nlsweb/default.mspx

typedef struct _languages {
    int LocaleID;
    int LGroupID; // new: language group id, the same up to any hyphen
    wchar_t * Abbreviation;
    wchar_t * TLA_LANG;
    wchar_t * TLA_CTRY;
    size_t OEM_CodePage;
    size_t ANSI_CodePage;
    wchar_t * ScreenName;
} LANGUAGES;

    // GENERALLY, lsb of language localeID always matches
    // the same pre-hyphenated suffix, but there are some
    // exceptions: bosnia/serbia/herzo* is just one such:
    // 1A, bs-Cyrl-BA
    // 1A, bs-Latn-BA
    // 1A, hr
    // 1A, hr-BA
    // 1A, hr-HR
    // 1A, sr
    // 1A, sr-Cyrl-BA
    // 1A, sr-Cyrl-SP
    // 1A, sr-Latn-BA
    // 1A, sr-Latn-SP

    // Those are, unfortunately, going to map onto a single ID number?
    // No, every unique name or alias returns an index into Languages.
    // Only if I shorten the hyphenated names will the lists collapse.
    //
    // In the case above, there are 10 unique ID numbers, and I would
    // munge them into just 3 categories ( bs, hr, sr ) for word lists.
    // But those 3 cannot reconstitute the 10 cases for page headers.
    //
    // As long as I only need one-way mapping, it will be okay with me.
    // ( I can map pages into word lists, but not word lists into pages. )
    //
    // Old Way:
    // See how I already map into pre-hyphenated terms for Save, All:
    // In CFio SaveFolder, I lowercase leading alphas, then stop.
    //
    // New Way:
    // I have precomputed a list of all abbreviations, and assigned new
    // members in Languages[] to be a LanguageGroup Index, which I will
    // save in every web page, along with the more exact language index
    // and can use a paper's LanguageGroup ( 0=unk to some N-1 ) to index
    // a simple vector[s] of ... new class objects, which can contain
    // pointers to ... word lists, and CVC run lists ( pretabulated ),
    // and a count of the web pages using it, to count languages, etc.


LANGUAGES LanguageList[] = {
   // First line gets added at index CSOL_INDEX_FOR_LANGUAGE_UNKNOWN == 2:
   { 0x0000,   0, L"un-known", L"UNK", L"UNK", 850, 1252, L"Unknown" },
   { 0x0000,   0, L"mul", L"MUL", L"MUL", 850, 1252, L"Multiple" }, // Attr MUL seen at www.wikipedia.org
   { 0x0036,   1, L"af", L"AFK", L"ZAF", 850, 1252, L"Afrikaans" },
   { 0x0436,   1, L"af-ZA", L"AFK", L"ZAF", 850, 1252, L"Afrikaans ( South Africa )" },
   { 0x001C,  98, L"sq", L"SQI", L"ALB", 852, 1250, L"Albanian" },
   { 0x041C,  98, L"sq-AL", L"SQI", L"ALB", 852, 1250, L"Albanian ( Albania )" },
   { 0x0484,  34, L"gsw-FR", L"GSW", L"FRA", 850, 1252, L"Alsatian ( France )" },
   { 0x045E,   2, L"am-ET", L"AMH", L"eth", 1, 0, L"Amharic ( Ethiopia )" },
   { 0x0001,   3, L"ar", L"ARA", L"SAU", 720, 1256, L"Arabic" },
   { 0x1401,   3, L"ar-DZ", L"ARG", L"DZA", 720, 1256, L"Arabic ( Algeria )" },
   { 0x3C01,   3, L"ar-BH", L"ARH", L"BHR", 720, 1256, L"Arabic ( Bahrain )" },
   { 0x0C01,   3, L"ar-EG", L"ARE", L"EGY", 720, 1256, L"Arabic ( Egypt )" },
   { 0x0801,   3, L"ar-IQ", L"ARI", L"IRQ", 720, 1256, L"Arabic ( Iraq )" },
   { 0x2C01,   3, L"ar-JO", L"ARJ", L"JOR", 720, 1256, L"Arabic ( Jordan )" },
   { 0x3401,   3, L"ar-KW", L"ARK", L"KWT", 720, 1256, L"Arabic ( Kuwait )" },
   { 0x3001,   3, L"ar-LB", L"ARB", L"LBN", 720, 1256, L"Arabic ( Lebanon )" },
   { 0x1001,   3, L"ar-LY", L"ARL", L"LBY", 720, 1256, L"Arabic ( Libya )" },
   { 0x1801,   3, L"ar-MA", L"ARM", L"MAR", 720, 1256, L"Arabic ( Morocco )" },
   { 0x2001,   3, L"ar-OM", L"ARO", L"OMN", 720, 1256, L"Arabic ( Oman )" },
   { 0x4001,   3, L"ar-QA", L"ARQ", L"QAT", 720, 1256, L"Arabic ( Qatar )" },
   { 0x0401,   3, L"ar-SA", L"ARA", L"SAU", 720, 1256, L"Arabic ( Saudi Arabia )" },
   { 0x2801,   3, L"ar-SY", L"ARS", L"SYR", 720, 1256, L"Arabic ( Syria )" },
   { 0x1C01,   3, L"ar-TN", L"ART", L"TUN", 720, 1256, L"Arabic ( Tunisia )" },
   { 0x3801,   3, L"ar-AE", L"ARU", L"ARE", 720, 1256, L"Arabic ( U.A.E. )" },
   { 0x2401,   3, L"ar-YE", L"ARY", L"YEM", 720, 1256, L"Arabic ( Yemen )" },
   { 0x002B,  41, L"hy", L"HYE", L"ARM", 1, 0, L"Armenian" },
   { 0x042B,  41, L"hy-AM", L"HYE", L"ARM", 1, 0, L"Armenian ( Armenia )" },
   { 0x044D,   5, L"as-IN", L"ASM", L"IND", 1, 0, L"Assamese ( India )" },
   { 0x002C,   6, L"az", L"AZE", L"AZE", 857, 1254, L"Azeri" },
   { 0x082C,   6, L"az-Cyrl-AZ", L"AZE", L"AZE", 866, 1251, L"Azeri ( Cyrillic, Azerbaijan )" },
   { 0x042C,   6, L"az-Latn-AZ", L"AZE", L"AZE", 857, 1254, L"Azeri ( Latin, Azerbaijan )" },
   { 0x046D,   7, L"ba-RU", L"BAS", L"RUS", 866, 1251, L"Bashkir ( Russia )" },
   { 0x002D,  25, L"eu", L"EUQ", L"ESP", 850, 1252, L"Basque" },
   { 0x042D,  25, L"eu-ES", L"EUQ", L"ESP", 850, 1252, L"Basque ( Basque )" },
   { 0x0023,   8, L"be", L"BEL", L"BLR", 866, 1251, L"Belarusian" },
   { 0x0423,   8, L"be-BY", L"BEL", L"BLR", 866, 1251, L"Belarusian ( Belarus )" },
   { 0x0845,  10, L"bn-BD", L"BNG", L"BDG", 1, 0, L"Bengali ( Bangladesh )" },
   { 0x0445,  10, L"bn-IN", L"BNG", L"IND", 1, 0, L"Bengali ( India )" },
   { 0x201A,  13, L"bs-Cyrl-BA", L"BSC", L"BIH", 855, 1251, L"Bosnian ( Cyrillic, Bosnia and Herzegovina )" },
   { 0x141A,  13, L"bs-Latn-BA", L"BSB", L"BIH", 852, 1250, L"Bosnian ( Latin, Bosnia and Herzegovina )" },
   { 0x047E,  12, L"br-FR", L"BRE", L"FRA", 850, 1252, L"Breton ( France )" },
   { 0x0002,   9, L"bg", L"BGR", L"BGR", 866, 1251, L"Bulgarian" },
   { 0x0402,   9, L"bg-BG", L"BGR", L"BGR", 866, 1251, L"Bulgarian ( Bulgaria )" },
   { 0x0003,  14, L"ca", L"CAT", L"ESP", 850, 1252, L"Catalan" },
   { 0x0403,  14, L"ca-ES", L"CAT", L"ESP", 850, 1252, L"Catalan ( Catalan )" },
   { 0x0C04, 122, L"zh-HK", L"ZHH", L"HKG", 950, 950, L"Chinese ( Hong Kong S.A.R. )" },
   { 0x1404, 122, L"zh-MO", L"ZHM", L"MCO", 950, 950, L"Chinese ( Macao S.A.R. )" },
   { 0x0804, 122, L"zh-CN", L"CHS", L"CHN", 936, 936, L"Chinese ( People's Republic of China )" },
   { 0x0004, 122, L"zh-Hans", L"CHS", L"CHN", 936, 936, L"Chinese ( Simplified )" },
   { 0x1004, 122, L"zh-SG", L"ZHI", L"SGP", 936, 936, L"Chinese ( Singapore )" },
   { 0x0404, 122, L"zh-TW", L"CHT", L"TWN", 950, 950, L"Chinese ( Taiwan )" },
   { 0x7C04, 122, L"zh-Hant", L"CHT", L"TWN", 950, 950, L"Chinese ( Traditional )" },
   { 0x0483,  15, L"co-FR", L"COS", L"FRA", 850, 1252, L"Corsican ( France )" },
   { 0x001A,  39, L"hr", L"HRV", L"HRV", 852, 1250, L"Croatian" },
   { 0x041A,  39, L"hr-HR", L"HRV", L"HRV", 852, 1250, L"Croatian ( Croatia )" },
   { 0x101A,  39, L"hr-BA", L"HRB", L"BIH", 852, 1250, L"Croatian ( Latin, Bosnia and Herzegovina )" },
   { 0x0005,  16, L"cs", L"CSY", L"CZE", 852, 1250, L"Czech" },
   { 0x0405,  16, L"cs-CZ", L"CSY", L"CZE", 852, 1250, L"Czech ( Czech Republic )" },
   { 0x0006,  18, L"da", L"DAN", L"DNK", 850, 1252, L"Danish" },
   { 0x0406,  18, L"da-DK", L"DAN", L"DNK", 850, 1252, L"Danish ( Denmark )" },
   { 0x048C,  79, L"prs-AF", L"PRS", L"AFG", 720, 1256, L"Dari ( Afghanistan )" },
   { 0x0065,  20, L"div", L"DIV", L"MDV", 1, 0, L"Divehi" },
   { 0x0465,  20, L"div-MV", L"DIV", L"MDV", 1, 0, L"Divehi ( Maldives )" },
   { 0x0013,  71, L"nl", L"NLD", L"NLD", 850, 1252, L"Dutch" },
   { 0x0813,  71, L"nl-BE", L"NLB", L"BEL", 850, 1252, L"Dutch ( Belgium )" },
   { 0x0413,  71, L"nl-NL", L"NLD", L"NLD", 850, 1252, L"Dutch ( Netherlands )" },
   { 0x0009,  22, L"en", L"ENU", L"USA", 437, 1252, L"English" },
   { 0x0C09,  22, L"en-AU", L"ENA", L"AUS", 850, 1252, L"English ( Australia )" },
   { 0x2809,  22, L"en-BZ", L"ENL", L"BLZ", 850, 1252, L"English ( Belize )" },
   { 0x1009,  22, L"en-CA", L"ENC", L"CAN", 850, 1252, L"English ( Canada )" },
   { 0x2409,  22, L"en-029", L"ENB", L"CAR", 850, 1252, L"English ( Caribbean )" },
   { 0x4009,  22, L"en-IN", L"ENN", L"IND", 437, 1252, L"English ( India )" },
   { 0x1809,  22, L"en-IE", L"ENI", L"IRL", 850, 1252, L"English ( Ireland )" },
   { 0x2009,  22, L"en-JM", L"ENJ", L"JAM", 850, 1252, L"English ( Jamaica )" },
   { 0x4409,  22, L"en-MY", L"ENM", L"MYS", 437, 1252, L"English ( Malaysia )" },
   { 0x1409,  22, L"en-NZ", L"ENZ", L"NZL", 850, 1252, L"English ( New Zealand )" },
   { 0x3409,  22, L"en-PH", L"ENP", L"PHL", 437, 1252, L"English ( Republic of the Philippines )" },
   { 0x4809,  22, L"en-SG", L"ENE", L"SGP", 437, 1252, L"English ( Singapore )" },
   { 0x1C09,  22, L"en-ZA", L"ENS", L"ZAF", 437, 1252, L"English ( South Africa )" },
   { 0x2C09,  22, L"en-TT", L"ENT", L"TTO", 850, 1252, L"English ( Trinidad and Tobago )" },
   { 0x0809,  22, L"en-GB", L"ENG", L"GBR", 850, 1252, L"English ( United Kingdom )" },
   { 0x0809,  22, L"en-UK", L"ENG", L"GBR", 850, 1252, L"English ( United Kingdom )" }, // another ALIAS per Glenn.
   { 0x0409,  22, L"en-US", L"ENU", L"USA", 437, 1252, L"English ( United States )" },
   { 0x3009,  22, L"en-ZW", L"ENW", L"ZWE", 437, 1252, L"English ( Zimbabwe )" },
   { 0x0025,  24, L"et", L"ETI", L"EST", 775, 1257, L"Estonian" },
   { 0x0425,  24, L"et-EE", L"ETI", L"EST", 775, 1257, L"Estonian ( Estonia )" },
   { 0x0038,  29, L"fo", L"FOS", L"FRO", 850, 1252, L"Faroese" },
   { 0x0438,  29, L"fo-FO", L"FOS", L"FRO", 850, 1252, L"Faroese ( Faroe Islands )" },
   { 0x0464,  28, L"fil-PH", L"FPO", L"PHL", 437, 1252, L"Filipino ( Philippines )" },
   { 0x000B,  27, L"fi", L"FIN", L"FIN", 850, 1252, L"Finnish" },
   { 0x040B,  27, L"fi-FI", L"FIN", L"FIN", 850, 1252, L"Finnish ( Finland )" },
   { 0x000C,  30, L"fr", L"FRA", L"FRA", 850, 1252, L"French" },
   { 0x080C,  30, L"fr-BE", L"FRB", L"BEL", 850, 1252, L"French ( Belgium )" },
   { 0x0C0C,  30, L"fr-CA", L"FRC", L"CAN", 850, 1252, L"French ( Canada )" },
   { 0x040C,  30, L"fr-FR", L"FRA", L"FRA", 850, 1252, L"French ( France )" },
   { 0x140C,  30, L"fr-LU", L"FRL", L"LUX", 850, 1252, L"French ( Luxembourg )" },
   { 0x180C,  30, L"fr-MC", L"FRM", L"MCO", 850, 1252, L"French ( Principality of Monaco )" },
   { 0x100C,  30, L"fr-CH", L"FRS", L"CHE", 850, 1252, L"French ( Switzerland )" },
   { 0x0462,  31, L"fy-NL", L"FYN", L"NLD", 850, 1252, L"Frisian ( Netherlands )" },
   { 0x0056,  33, L"gl", L"GLC", L"ESP", 850, 1252, L"Galician" },
   { 0x0456,  33, L"gl-ES", L"GLC", L"ESP", 850, 1252, L"Galician ( Galician )" },
   { 0x0037,  49, L"ka", L"KAT", L"GEO", 1, 0, L"Georgian" },
   { 0x0437,  49, L"ka-GE", L"KAT", L"GEO", 1, 0, L"Georgian ( Georgia )" },
   { 0x0007,  19, L"de", L"DEU", L"DEU", 850, 1252, L"German" },
   { 0x0C07,  19, L"de-AT", L"DEA", L"AUT", 850, 1252, L"German ( Austria )" },
   { 0x0407,  19, L"de-DE", L"DEU", L"DEU", 850, 1252, L"German ( Germany )" },
   { 0x1407,  19, L"de-LI", L"DEC", L"LIE", 850, 1252, L"German ( Liechtenstein )" },
   { 0x1007,  19, L"de-LU", L"DEL", L"LUX", 850, 1252, L"German ( Luxembourg )" },
   { 0x0807,  19, L"de-CH", L"DES", L"CHE", 850, 1252, L"German ( Switzerland )" },
   { 0x0008,  21, L"el", L"ELL", L"GRC", 737, 1253, L"Greek" },
   { 0x0408,  21, L"el-GR", L"ELL", L"GRC", 737, 1253, L"Greek ( Greece )" },
   { 0x046F,  51, L"kl-GL", L"KAL", L"GRL", 850, 1252, L"Greenlandic ( Greenland )" },
   { 0x0047,  35, L"gu", L"GUJ", L"IND", 1, 0, L"Gujarati" },
   { 0x0447,  35, L"gu-IN", L"GUJ", L"IND", 1, 0, L"Gujarati ( India )" },
   { 0x0468,  36, L"ha-Latn-NG", L"HAU", L"NGA", 437, 1252, L"Hausa ( Latin, Nigeria )" },
   { 0x000D,  37, L"he", L"HEB", L"ISR", 862, 1255, L"Hebrew" },
   { 0x040D,  37, L"he-IL", L"HEB", L"ISR", 862, 1255, L"Hebrew ( Israel )" },
   { 0x0039,  38, L"hi", L"HIN", L"IND", 1, 0, L"Hindi" },
   { 0x0439,  38, L"hi-IN", L"HIN", L"IND", 1, 0, L"Hindi ( India )" },
   { 0x000E,  40, L"hu", L"HUN", L"HUN", 852, 1250, L"Hungarian" },
   { 0x040E,  40, L"hu-HU", L"HUN", L"HUN", 852, 1250, L"Hungarian ( Hungary )" },
   { 0x000F,  45, L"is", L"ISL", L"ISL", 850, 1252, L"Icelandic" },
   { 0x040F,  45, L"is-IS", L"ISL", L"ISL", 850, 1252, L"Icelandic ( Iceland )" },
   { 0x0470,  43, L"ig-NG", L"IBO", L"NGA", 437, 1252, L"Igbo ( Nigeria )" },
   { 0x0021,  42, L"id", L"IND", L"IDN", 850, 1252, L"Indonesian" },
   { 0x0421,  42, L"id-ID", L"IND", L"IDN", 850, 1252, L"Indonesian ( Indonesia )" },
   { 0x085D,  47, L"iu-Latn-CA", L"IUK", L"CAN", 437, 1252, L"Inuktitut ( Latin, Canada )" },
   { 0x045D,  47, L"iu-Cans-CA", L"IUS", L"CAN", 1, 0, L"Inuktitut ( Syllabics, Canada )" },
   { 0x083C,  32, L"ga-IE", L"IRE", L"IRL", 850, 1252, L"Irish ( Ireland )" },
   { 0x0434, 120, L"xh-ZA", L"XHO", L"ZAF", 850, 1252, L"isiXhosa ( South Africa )" },
   { 0x0435, 123, L"zu-ZA", L"ZUL", L"ZAF", 850, 1252, L"isiZulu ( South Africa )" },
   { 0x0010,  46, L"it", L"ITA", L"ITA", 850, 1252, L"Italian" },
   { 0x0410,  46, L"it-IT", L"ITA", L"ITA", 850, 1252, L"Italian ( Italy )" },
   { 0x0810,  46, L"it-CH", L"ITS", L"CHE", 850, 1252, L"Italian ( Switzerland )" },
   { 0x0011,  48, L"ja", L"JPN", L"JPN", 932, 932, L"Japanese" },
   { 0x0411,  48, L"ja-JP", L"JPN", L"JPN", 932, 932, L"Japanese ( Japan )" },
   { 0x004B,  53, L"kn", L"KDI", L"IND", 1, 0, L"Kannada" },
   { 0x044B,  53, L"kn-IN", L"KDI", L"IND", 1, 0, L"Kannada ( India )" },
   { 0x003F,  50, L"kk", L"KKZ", L"KAZ", 866, 1251, L"Kazakh" },
   { 0x043F,  50, L"kk-KZ", L"KKZ", L"KAZ", 866, 1251, L"Kazakh ( Kazakhstan )" },
   { 0x0453,  52, L"km-KH", L"KHM", L"KHM", 1, 0, L"Khmer ( Cambodia )" },
   { 0x0486,  82, L"qut-GT", L"QUT", L"GTM", 850, 1252, L"K'iche ( Guatemala )" },
   { 0x0487,  87, L"rw-RW", L"KIN", L"RWA", 437, 1252, L"Kinyarwanda ( Rwanda )" },
   { 0x0041, 101, L"sw", L"SWK", L"KEN", 437, 1252, L"Kiswahili" },
   { 0x0441, 101, L"sw-KE", L"SWK", L"KEN", 437, 1252, L"Kiswahili ( Kenya )" },
   { 0x0057,  55, L"kok", L"KNK", L"IND", 1, 0, L"Konkani" },
   { 0x0457,  55, L"kok-IN", L"KNK", L"IND", 1, 0, L"Konkani ( India )" },
   { 0x0012,  54, L"ko", L"KOR", L"KOR", 949, 949, L"Korean" },
   { 0x0412,  54, L"ko-KR", L"KOR", L"KOR", 949, 949, L"Korean ( Korea )" },
   { 0x0040,  56, L"ky", L"KYR", L"KGZ", 866, 1251, L"Kyrgyz" },
   { 0x0440,  56, L"ky-KG", L"KYR", L"KGZ", 866, 1251, L"Kyrgyz ( Kyrgyzstan )" },
   { 0x0454,  58, L"lo-LA", L"LAO", L"LAO", 1, 0, L"Lao ( Lao P.D.R. )" },
   { 0x0026,  60, L"lv", L"LVI", L"LVA", 775, 1257, L"Latvian" },
   { 0x0426,  60, L"lv-LV", L"LVI", L"LVA", 775, 1257, L"Latvian ( Latvia )" },
   { 0x0027,  59, L"lt", L"LTH", L"LTU", 775, 1257, L"Lithuanian" },
   { 0x0427,  59, L"lt-LT", L"LTH", L"LTU", 775, 1257, L"Lithuanian ( Lithuania )" },
   { 0x082E, 117, L"wee-DE", L"DSB", L"GER", 850, 1252, L"Lower Sorbian ( Germany )" },
   { 0x046E,  57, L"lb-LU", L"LBX", L"LUX", 850, 1252, L"Luxembourgish ( Luxembourg )" },
   { 0x002F,  62, L"mk", L"MKI", L"MKD", 866, 1251, L"Macedonian" },
   { 0x042F,  62, L"mk-MK", L"MKI", L"MKD", 866, 1251, L"Macedonian ( Former Yugoslav Republic of Macedonia )" },
   { 0x003E,  67, L"ms", L"MSL", L"MYS", 850, 1252, L"Malay" },
   { 0x083E,  67, L"ms-BN", L"MSB", L"BRN", 850, 1252, L"Malay ( Brunei Darussalam )" },
   { 0x043E,  67, L"ms-MY", L"MSL", L"MYS", 850, 1252, L"Malay ( Malaysia )" },
   { 0x044C,  63, L"ml-IN", L"MYM", L"IND", 1, 0, L"Malayalam ( India )" },
   { 0x043A,  68, L"mt-MT", L"MLT", L"MLT", 1, 0, L"Maltese ( Malta )" },
   { 0x0481,  61, L"mi-NZ", L"MRI", L"NZL", 1, 0, L"Maori ( New Zealand )" },
   { 0x047A,   4, L"arn-CL", L"MPD", L"CHL", 850, 1252, L"Mapudungun ( Chile )" },
   { 0x004E,  66, L"mr", L"MAR", L"IND", 1, 0, L"Marathi" },
   { 0x044E,  66, L"mr-IN", L"MAR", L"IND", 1, 0, L"Marathi ( India )" },
   { 0x047C,  65, L"moh-CA", L"MWK", L"CAN", 850, 1252, L"Mohawk ( Mohawk )" },
   { 0x0050,  64, L"mn", L"MON", L"MNG", 866, 1251, L"Mongolian" },
   { 0x0450,  64, L"mn-MN", L"MON", L"MNG", 866, 1251, L"Mongolian ( Cyrillic, Mongolia )" },
   { 0x0850,  64, L"mn-Mong-CN", L"MNG", L"CHN", 1, 0, L"Mongolian ( Traditional Mongolian, PRC )" },
   { 0x0461,  70, L"ne-NP", L"NEP", L"NEP", 1, 0, L"Nepali ( Nepal )" },
   { 0x0014,  73, L"no", L"NOR", L"NOR", 850, 1252, L"Norwegian" },
   { 0x0414,  69, L"nb-NO", L"NOR", L"NOR", 850, 1252, L"Norwegian, Bokml ( Norway )" },
   { 0x0814,  72, L"nn-NO", L"NON", L"NOR", 850, 1252, L"Norwegian, Nynorsk ( Norway )" },
   { 0x0482,  75, L"oc-FR", L"OCI", L"FRA", 850, 1252, L"Occitan ( France )" },
   { 0x0448,  76, L"or-IN", L"ORI", L"IND", 1, 0, L"Oriya ( India )" },
   { 0x0463,  80, L"ps-AF", L"PAS", L"AFG", 1, 0, L"Pashto ( Afghanistan )" },
   { 0x0429,  26, L"fa-IR", L"FAR", L"IRN", 720, 1256, L"Persian" }, // put more specific first
   { 0x0029,  26, L"fa", L"FAR", L"IRN", 720, 1256, L"Persian" }, // second occurance of "Persian"
   { 0x0015,  78, L"pl", L"PLK", L"POL", 852, 1250, L"Polish" },
   { 0x0415,  78, L"pl-PL", L"PLK", L"POL", 852, 1250, L"Polish ( Poland )" },
   { 0x0016,  81, L"pt", L"PTB", L"BRA", 850, 1252, L"Portuguese" },
   { 0x0416,  81, L"pt-BR", L"PTB", L"BRA", 850, 1252, L"Portuguese ( Brazil )" },
   { 0x0816,  81, L"pt-PT", L"PTG", L"PRT", 850, 1252, L"Portuguese ( Portugal )" },
   { 0x0046,  77, L"pa", L"PAN", L"IND", 1, 0, L"Punjabi" },
   { 0x0446,  77, L"pa-IN", L"PAN", L"IND", 1, 0, L"Punjabi ( India )" },
   { 0x046B,  83, L"quz-BO", L"QUB", L"BOL", 850, 1252, L"Quechua ( Bolivia )" },
   { 0x086B,  83, L"quz-EC", L"QUE", L"ECU", 850, 1252, L"Quechua ( Ecuador )" },
   { 0x0C6B,  83, L"quz-PE", L"QUP", L"PER", 850, 1252, L"Quechua ( Peru )" },
   { 0x0018,  85, L"ro", L"ROM", L"ROM", 852, 1250, L"Romanian" },
   { 0x0418,  85, L"ro-RO", L"ROM", L"ROM", 852, 1250, L"Romanian ( Romania )" },
   { 0x0417,  84, L"rm-CH", L"RMC", L"CHE", 850, 1252, L"Romansh ( Switzerland )" },
   { 0x0019,  86, L"ru", L"RUS", L"RUS", 866, 1251, L"Russian" },
   { 0x0419,  86, L"ru-RU", L"RUS", L"RUS", 866, 1251, L"Russian ( Russia )" },
   { 0x243B,  96, L"smn-FI", L"SMN", L"FIN", 850, 1252, L"Sami, Inari ( Finland )" },
   { 0x103B,  95, L"smj-NO", L"SMJ", L"NOR", 850, 1252, L"Sami, Lule ( Norway )" },
   { 0x143B,  95, L"smj-SE", L"SMK", L"SWE", 850, 1252, L"Sami, Lule ( Sweden )" },
   { 0x0C3B,  90, L"se-FI", L"SMG", L"FIN", 850, 1252, L"Sami, Northern ( Finland )" },
   { 0x043B,  90, L"se-NO", L"SME", L"NOR", 850, 1252, L"Sami, Northern ( Norway )" },
   { 0x083B,  90, L"se-SE", L"SMF", L"SWE", 850, 1252, L"Sami, Northern ( Sweden )" },
   { 0x203B,  97, L"sms-FI", L"SMS", L"FIN", 850, 1252, L"Sami, Skolt ( Finland )" },
   { 0x183B,  94, L"sma-NO", L"SMA", L"NOR", 850, 1252, L"Sami, Southern ( Norway )" },
   { 0x1C3B,  94, L"sma-SE", L"SMB", L"SWE", 850, 1252, L"Sami, Southern ( Sweden )" },
   { 0x004F,  88, L"sa", L"SAN", L"IND", 1, 0, L"Sanskrit" },
   { 0x044F,  88, L"sa-IN", L"SAN", L"IND", 1, 0, L"Sanskrit ( India )" },
   { 0x7C1A,  99, L"sr", L"SRB", L"SCG", 855, 1251, L"Serbian" },
   { 0x1C1A,  99, L"sr-Cyrl-BA", L"SRN", L"BIH", 855, 1251, L"Serbian ( Cyrillic, Bosnia and Herzegovina )" },
   { 0x0C1A,  99, L"sr-Cyrl-SP", L"SRB", L"SCG", 855, 1251, L"Serbian ( Cyrillic, Serbia )" },
   { 0x181A,  99, L"sr-Latn-BA", L"SRS", L"BIH", 852, 1250, L"Serbian ( Latin, Bosnia and Herzegovina )" },
   { 0x081A,  99, L"sr-Latn-SP", L"SRL", L"SCG", 852, 1250, L"Serbian ( Latin, Serbia )" },
   { 0x046C,  74, L"nso-ZA", L"NSO", L"ZAF", 850, 1252, L"Sesotho sa Leboa ( South Africa )" },
   { 0x0432, 109, L"tn-ZA", L"TSN", L"ZAF", 850, 1252, L"Setswana ( South Africa )" },
   { 0x045B,  91, L"si-LK", L"SIN", L"LKA", 1, 0, L"Sinhala ( Sri Lanka )" },
   { 0x001B,  92, L"sk", L"SKY", L"SVK", 852, 1250, L"Slovak" },
   { 0x041B,  92, L"sk-SK", L"SKY", L"SVK", 852, 1250, L"Slovak ( Slovakia )" },
   { 0x0024,  93, L"sl", L"SLV", L"SVN", 852, 1250, L"Slovenian" },
   { 0x0424,  93, L"sl-SI", L"SLV", L"SVN", 852, 1250, L"Slovenian ( Slovenia )" },
   { 0x000A,  23, L"es", L"ESP", L"ESP", 850, 1252, L"Spanish" },
   { 0x2C0A,  23, L"es-AR", L"ESS", L"ARG", 850, 1252, L"Spanish ( Argentina )" },
   { 0x400A,  23, L"es-BO", L"ESB", L"BOL", 850, 1252, L"Spanish ( Bolivia )" },
   { 0x340A,  23, L"es-CL", L"ESL", L"CHL", 850, 1252, L"Spanish ( Chile )" },
   { 0x240A,  23, L"es-CO", L"ESO", L"COL", 850, 1252, L"Spanish ( Colombia )" },
   { 0x140A,  23, L"es-CR", L"ESC", L"CRI", 850, 1252, L"Spanish ( Costa Rica )" },
   { 0x1C0A,  23, L"es-DO", L"ESD", L"DOM", 850, 1252, L"Spanish ( Dominican Republic )" },
   { 0x300A,  23, L"es-EC", L"ESF", L"ECU", 850, 1252, L"Spanish ( Ecuador )" },
   { 0x440A,  23, L"es-SV", L"ESE", L"SLV", 850, 1252, L"Spanish ( El Salvador )" },
   { 0x100A,  23, L"es-GT", L"ESG", L"GTM", 850, 1252, L"Spanish ( Guatemala )" },
   { 0x480A,  23, L"es-HN", L"ESH", L"HND", 850, 1252, L"Spanish ( Honduras )" },
   { 0x080A,  23, L"es-MX", L"ESM", L"MEX", 850, 1252, L"Spanish ( Mexico )" },
   { 0x4C0A,  23, L"es-NI", L"ESI", L"NIC", 850, 1252, L"Spanish ( Nicaragua )" },
   { 0x180A,  23, L"es-PA", L"ESA", L"PAN", 850, 1252, L"Spanish ( Panama )" },
   { 0x3C0A,  23, L"es-PY", L"ESZ", L"PRY", 850, 1252, L"Spanish ( Paraguay )" },
   { 0x280A,  23, L"es-PE", L"ESR", L"PER", 850, 1252, L"Spanish ( Peru )" },
   { 0x500A,  23, L"es-PR", L"ESU", L"PRI", 850, 1252, L"Spanish ( Puerto Rico )" },
   { 0x0C0A,  23, L"es-ES", L"ESN", L"ESP", 850, 1252, L"Spanish ( Spain )" },
   { 0x540A,  23, L"es-US", L"EST", L"USA", 850, 1252, L"Spanish ( United States )" },
   { 0x380A,  23, L"es-UY", L"ESY", L"URY", 850, 1252, L"Spanish ( Uruguay )" },
   { 0x200A,  23, L"es-VE", L"ESV", L"VEN", 850, 1252, L"Spanish ( Venezuela )" },
   { 0x001D, 100, L"sv", L"SVE", L"SWE", 850, 1252, L"Swedish" },
   { 0x081D, 100, L"sv-FI", L"SVF", L"FIN", 850, 1252, L"Swedish ( Finland )" },
   { 0x041D, 100, L"sv-SE", L"SVE", L"SWE", 850, 1252, L"Swedish ( Sweden )" },
   { 0x005A, 102, L"syr", L"SYR", L"SYR", 1, 0, L"Syriac" },
   { 0x045A, 102, L"syr-SY", L"SYR", L"SYR", 1, 0, L"Syriac ( Syria )" },
   { 0x0428, 105, L"tg-Cyrl-TJ", L"TAJ", L"TAJ", 866, 1251, L"Tajik ( Cyrillic, Tajikistan )" },
   { 0x085F, 108, L"tmz-Latn-DZ", L"TMZ", L"DZA", 850, 1252, L"Tamazight ( Latin, Algeria )" },
   { 0x0049, 103, L"ta", L"TAM", L"IND", 1, 0, L"Tamil" },
   { 0x0449, 103, L"ta-IN", L"TAM", L"IND", 1, 0, L"Tamil ( India )" },
   { 0x0044, 111, L"tt", L"TTT", L"RUS", 866, 1251, L"Tatar" },
   { 0x0444, 111, L"tt-RU", L"TTT", L"RUS", 866, 1251, L"Tatar ( Russia )" },
   { 0x004A, 104, L"te", L"TEL", L"IND", 1, 0, L"Telugu" },
   { 0x044A, 104, L"te-IN", L"TEL", L"IND", 1, 0, L"Telugu ( India )" },
   { 0x001E, 106, L"th", L"THA", L"THA", 874, 874, L"Thai" },
   { 0x041E, 106, L"th-TH", L"THA", L"THA", 874, 874, L"Thai ( Thailand )" },
   { 0x0451,  11, L"bo-CN", L"BOB", L"CHN", 1, 0, L"Tibetan ( PRC )" },
   { 0x001F, 110, L"tr", L"TRK", L"TUR", 857, 1254, L"Turkish" },
   { 0x041F, 110, L"tr-TR", L"TRK", L"TUR", 857, 1254, L"Turkish ( Turkey )" },
   { 0x0442, 107, L"tk-TM", L"TUK", L"TKM", 852, 1250, L"Turkmen ( Turkmenistan )" },
   { 0x0480, 112, L"ug-CN", L"UIG", L"CHN", 720, 1256, L"Uighur ( PRC )" },
   { 0x0022, 113, L"uk", L"UKR", L"UKR", 866, 1251, L"Ukrainian" },
   { 0x0422, 113, L"uk-UA", L"UKR", L"UKR", 866, 1251, L"Ukrainian ( Ukraine )" },
   { 0x042E, 118, L"wen-DE", L"HSB", L"GER", 850, 1252, L"Upper Sorbian ( Germany )" },
   { 0x0020, 114, L"ur", L"URD", L"PAK", 720, 1256, L"Urdu" },
   { 0x0420, 114, L"ur-PK", L"URD", L"PAK", 720, 1256, L"Urdu ( Islamic Republic of Pakistan )" },
   { 0x0043, 115, L"uz", L"UZB", L"UZB", 857, 1254, L"Uzbek" },
   { 0x0843, 115, L"uz-Cyrl-UZ", L"UZB", L"UZB", 866, 1251, L"Uzbek ( Cyrillic, Uzbekistan )" },
   { 0x0443, 115, L"uz-Latn-UZ", L"UZB", L"UZB", 857, 1254, L"Uzbek ( Latin, Uzbekistan )" },
   { 0x002A, 116, L"vi", L"VIT", L"VNM", 1258, 1258, L"Vietnamese" },
   { 0x042A, 116, L"vi-VN", L"VIT", L"VNM", 1258, 1258, L"Vietnamese ( Vietnam )" },
   { 0x0452,  17, L"cy-GB", L"CYM", L"GBR", 850, 1252, L"Welsh ( United Kingdom )" },
   { 0x0488, 119, L"wo-SN", L"WOL", L"SEN", 850, 1252, L"Wolof ( Senegal )" },
   { 0x0485,  89, L"sah-RU", L"SAH", L"RUS", 866, 1251, L"Yakut ( Russia )" },
   { 0x0478,  44, L"ii-CN", L"III", L"CHN", 1, 0, L"Yi ( PRC )" },
   { 0x046A, 121, L"yo-NG", L"YOR", L"NGA", 437, 1252, L"Yoruba ( Nigeria )" },
   { -1,       0, NULL, NULL, NULL, 0, 0, NULL }
};

void SetupLanguageNames( )
{
    // First, put all standard .Abbreviation into CSolLanguageAliases,
    // changing them to lowercase; Then add same without puncts.
    // The CSolLanguageAliases User.Value can be an index in list above;
    // Notice that my #0 item above will serve for all not-found.
    // Callers will pass names to my helpers for lookup via list.

    // Note that it will be possible for zero to get labeled, as:
    // Unknown: 0 -- Which is the value of enum, LANGUAGE_UNKNOWN

    // First line, Unknown, tables[LANGUAGE_UNKNOWN], gets added to
    // CSolLanguageAliases at index CSOL_INDEX_FOR_LANGUAGE_UNKNOWN == 2.

    // Although CharSets really did have a bevy of aliases for each entry,
    // Languages really only have one abbreviation and one full name each,
    // also them lowercased, which nearly all are unique, this re-locating
    // the very same index into LanguageList without munging them together.

    // Any munging happens when I map those IDs into LanguageGroup IDs.

    // I notice that I only do with lowercasing.
    // The look-up routine will also do lowercasing.

    #if DO_DEBUG_LANGUAGE2
        ; Spew( L"SetupLanguageNames was called." );
    #endif

    int i = 0;
    for( ;; )
    {
        LANGUAGES * p = LanguageList + i;
        if( p->LocaleID == -1 ) // This is my loop sentinel
            break;
        wchar_t wk[60];

        // 0. Lowercased copy of .ScreenName.
        {
            wchar_t * into = wk;
            wchar_t * from = p->ScreenName;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                if( isalpha( c ) ) // all ascii
                    *into++ = c | ' '; // lowercase
                else
                    *into++ = c;
            }
            size_t index = CSolLanguageAliases.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3832" ); }
            #endif
            #if DO_DEBUG_LANGUAGE2
                ; SpewTwo( L"SetupLanguageNames adding", wk );
            #endif
            // give me an analysis of alias conflicts ( 4 plcs ):
            #if DO_DEBUG_LANGUAGE2
            {
                int was = CSolLanguageAliases.GetUserValue( index );
                if( was != 0
                && was != i )
                {
                    ; SpewValue( L"CONFLICT:", was );
                }
            }
            #endif
            #if DO_DEBUG_LANGUAGE2
                ; SpewValue( L"SetupLanguageNames setting", i );
            #endif
            CSolLanguageAliases.SetUserValue( index, i ); // save LanguageList[i] index.
            #if DO_DEBUG_LANGUAGE2
                ; SpewValue( wk, i );
            #endif
        }

        // 1. Lowercased copy of .Abbreviation.
        {
            wchar_t * into = wk;
            wchar_t * from = p->Abbreviation;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                if( isalpha( c ) ) // all ascii
                    *into++ = c | ' '; // lowercase
                else
                    *into++ = c;
            }
            size_t index = CSolLanguageAliases.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3832" ); }
            #endif
            #if DO_DEBUG_LANGUAGE2
                ; SpewTwo( L"SetupLanguageNames adding", wk );
            #endif
            // give me an analysis of alias conflicts ( 4 plcs ):
            #if DO_DEBUG_LANGUAGE2
            {
                int was = CSolLanguageAliases.GetUserValue( index );
                if( was != 0
                && was != i )
                {
                    ; SpewValue( L"CONFLICT:", was );
                }
            }
            #endif
            #if DO_DEBUG_LANGUAGE2
                ; SpewValue( L"SetupLanguageNames setting", i );
            #endif
            CSolLanguageAliases.SetUserValue( index, i ); // save LanguageList[i] index.
        }

        // 2. Lowercased isalum-only letters of Abbreviation.
        {
            wchar_t * into = wk;
            wchar_t * from = p->Abbreviation;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                if( iswdigit( c ) )
                    *into++ = c;
                else if( iswalpha( c ) )
                    *into++ = c | ' ';
            }
            size_t index = CSolLanguageAliases.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3858" ); }
            #endif
            #if DO_DEBUG_LANGUAGE2
                ; SpewTwo( L"SetupLanguageNames adding", wk );
            #endif
            // give me an analysis of alias conflicts ( 4 plcs ):
            #if DO_DEBUG_LANGUAGE2
            {
                int was = CSolLanguageAliases.GetUserValue( index );
                if( was != 0
                && was != i )
                {
                    ; SpewValue( L"CONFLICT:", was );
                }
            }
            #endif
            #if DO_DEBUG_LANGUAGE2
                ; SpewValue( L"SetupLanguageNames setting", i );
            #endif
            CSolLanguageAliases.SetUserValue( index, i ); // save LanguageList[i] index.
        }

        // I have no ALIASES in this list, like for charsets.

        i++;
    }


        // Closing the loop shows me that I need some aliases:
        // Do the abbreviations without any hyphenated suffix.
        // Do the Names again without any parenthesized parts.
        // But this time, if the short name is already present,
        // do not change it, but I might mention if conflicted.
        // Do it after entering all the names the original way.


    #if DO_DEBUG_LANGUAGE2
        ; Spew( L"SetupLanguageNames is now making shortened names." );
    #endif

    i = 0;
    for( ;; )
    {
        LANGUAGES * p = LanguageList + i;
        if( p->LocaleID == -1 ) // This is my loop sentinel
            break;
        wchar_t wk[60];

        // 0. Lowercased copy of .ScreenName.
        {
            wchar_t * into = wk;
            wchar_t * from = p->ScreenName;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                if( c == '('
                || *from == '(' ) // from was already advanced
                {
                    *into = NULL;
                    break;
                }
                if( isalpha( c ) ) // all ascii
                    *into++ = c | ' '; // lowercase
                else
                    *into++ = c;
            }
            size_t index = CSolLanguageAliases.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3832" ); }
            #endif

            int was = CSolLanguageAliases.GetUserValue( index );
            if( was == 0 )
            {
                #if DO_DEBUG_LANGUAGE2
                    ; SpewTwo( L"SetupLanguageNames adding shortened", wk );
                    ; SpewValue( L"SetupLanguageNames setting", i );
                #endif
                CSolLanguageAliases.SetUserValue( index, i ); // save LanguageList[i] index.
                #if DO_DEBUG_LANGUAGE2
                    ; SpewValue( wk, i );
                #endif
            }
        }

        // 1. Lowercased copy of .Abbreviation.
        {
            wchar_t * into = wk;
            wchar_t * from = p->Abbreviation;
            for( ;; )
            {
                wchar_t c = *from++;
                if( c == NULL )
                {
                    *into = NULL;
                    break;
                }
                if( c == '-' )
                {
                    *into = NULL;
                    break;
                }
                if( isalpha( c ) ) // all ascii
                    *into++ = c | ' '; // lowercase
                else
                    *into++ = c;
            }
            size_t index = CSolLanguageAliases.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at cvoc 3832" ); }
            #endif
            int was = CSolLanguageAliases.GetUserValue( index );
            if( was == 0 )
            {
                #if DO_DEBUG_LANGUAGE2
                    ; SpewTwo( L"SetupLanguageNames adding shortened", wk );
                    ; SpewValue( L"SetupLanguageNames setting", i );
                #endif
                CSolLanguageAliases.SetUserValue( index, i ); // save LanguageList[i] index.
            }
        }

        i++;
    }


    // If CTxt decides on a new value for pOnePaper->LanguageGroup,
    // it needs a way to get a pOnePaper->HttpHeaderContentLanguage.
    // I can use this, but I should pre-build a default translation:
    // size_t BestIndexforLanguageString( wchar_t * pName, wchar_t * pPast )

    // I guess I will run a loop down the minimal vector, calling that.
    // After I output results to a spew, copy it directly into program.

    #if DO_DEBUG_LANG_BKWD
    {
        // LanguageGroupList[ PAST_LANGUAGE_GROUP_IDS ]
        // LanguageGroupName[ PAST_LANGUAGE_GROUP_IDS ]
        i = 0;
        for( ;; )
        {
            wchar_t * szAbbrev = GroupAbbreviationforGroupIndex( i );
            wchar_t * szCaption = GroupNameforGroupIndex( i );
            // I can look up either the Abbr or full name. Do both.
            // No. I see that I can only look up the full name.... I hope.
            // For example, my "am" <> L"am-ET", L"Amharic ( Ethiopia )" },

            int l1 = BestIndexforLanguageString( szAbbrev, szAbbrev + wcslen( szAbbrev ) );
            int l2 = BestIndexforLanguageString( szCaption, szCaption + wcslen( szCaption ) );
            wchar_t * szA2 = BestLanguageAbbreviationforIndex( l2 );
            wchar_t * szC2 = BestLanguageNameforIndex( l2 );
            // In fact, let's close the loop on these too:
            int ii = GroupIndexForLanguageIndex( l2 );

            wchar_t wk[200];
            wsprintf( wk, L"    %3d, // %3d%s (%3d%s) group '%s' %s -> lang '%s' %s",
                l2,
                ii, ( ii==i? L"=ok" : L"=ng" ),
                l1, ( l1==l2? L"-eq" : L"-ne" ),
                szAbbrev, szCaption,
                szA2, szC2 );

            ; Spew( wk );

            if( ++i == PAST_LANGUAGE_GROUP_IDS )
                break;
        }

    }
    #endif


}

size_t BestIndexforLanguageString( wchar_t * pName, wchar_t * pPast )
{
    // Note that the "BestIndex" is not into a CSol, but my data array.
    // return 0 == LANGUAGE_UNKNOWN for unrecognized language strings.

    int arrayindex = 0;
    wchar_t wk[60];

    #if DO_DEBUG_LANGUAGE
        ; SpewTwo( L"BestIndexforLanguageString pName", pName );
    #endif

    // If try 1 fails for nomatch, do not RETURN, but go on to try 2.

    // 1. Lowercased copy of preferred name.
    {
        #if DO_DEBUG_LANGUAGE
            ; Spew( L"BestIndex: try 1. Verbatim" );
        #endif
        wchar_t * into = wk;
        wchar_t * from = pName;
        for( ;; )
        {
            if( from == pPast )
            {
                *into = NULL;
                break;
            }
            wchar_t c = *from++;
            if( c == NULL )
            {
                *into = NULL;
                break;
            }
            if( into == wk + 58 )
            {
                #if DO_DEBUG_LANGUAGE
                    ; Spew( L"BestIndex: too long. return LANGUAGE_UNKNOWN" );
                #endif
                return LANGUAGE_UNKNOWN; // too long a name? return 0 for "??"
            }
            if( c <= 0x7f ) // only ascii
            {
                if( isalpha( c ) ) // all ascii
                    *into++ = c | ' '; // lowercase
                else
                    *into++ = c;
            }
        }

        #if DO_DEBUG_LANGUAGE
            ; SpewTwo( L"BestIndexforLanguageString wk1", wk );
        #endif

        if( into == wk )
        {
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"BestIndex: empty. return LANGUAGE_UNKNOWN" );
            #endif
            return LANGUAGE_UNKNOWN; // empty name? return 0 for "??"
        }
        size_t index = CSolLanguageAliases.Find( wk );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 4003" ); }
        #endif
        #if DO_DEBUG_LANGUAGE
            ; SpewValue( wk, index );
        #endif
        if( index == 0 )
        {
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"BestIndex: unrecognized" );
            #endif
            // Oops. Go on to phase 2...
            // return LANGUAGE_UNKNOWN; // unrecognized name? return 0 for "??"
        }
        else
        {
            arrayindex = CSolLanguageAliases.GetUserValue( index );
            #if DO_DEBUG_LANGUAGE
                ; SpewValue( L"Value", arrayindex );
            #endif
            return arrayindex;
        }
    }

    // 2. Lowercased isalum-only letters of preferred name.
    {
        #if DO_DEBUG_LANGUAGE
            ; Spew( L"BestIndex: try 2. Lowercased" );
        #endif
        wchar_t * into = wk;
        wchar_t * from = pName;
        for( ;; )
        {
            if( from == pPast )
            {
                *into = NULL;
                break;
            }
            wchar_t c = *from++;
            if( c == NULL )
            {
                *into = NULL;
                break;
            }
            if( into == wk + 58 )
            {
                #if DO_DEBUG_LANGUAGE
                    ; Spew( L"BestIndex: too long. return LANGUAGE_UNKNOWN" );
                #endif
                return LANGUAGE_UNKNOWN; // too long a name? return 0 for "??"
            }

            if( c <= 0x7f ) // only ascii
            {
                if( isdigit( c ) ) // safe, isascii too.
                    *into++ = c;
                else if( isalpha( c ) ) // all ascii
                    *into++ = c | ' '; // lowercase
                else
                    *into++ = c;
            }

        }

        #if DO_DEBUG_LANGUAGE
            ; SpewTwo( L"BestIndexforLanguageString wk2", wk );
        #endif

        if( into == wk )
        {
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"BestIndex: empty. return LANGUAGE_UNKNOWN" );
            #endif
            return LANGUAGE_UNKNOWN; // empty name? return 0 for "??"
        }
        size_t index = CSolLanguageAliases.Find( wk );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 4041" ); }
        #endif
        #if DO_DEBUG_LANGUAGE
            ; SpewValue( wk, index );
        #endif
        if( index == 0 )
        {
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"BestIndex: unrecognized. return LANGUAGE_UNKNOWN" );
            #endif
            return LANGUAGE_UNKNOWN; // unrecognized name? return 0 for "??"
        }
        arrayindex = CSolLanguageAliases.GetUserValue( index );
        #if DO_DEBUG_LANGUAGE
            ; SpewValue( L"Value", arrayindex );
        #endif
        return arrayindex;
    }
}

wchar_t * BestLanguageNameforIndex( size_t index )
{
    // Note that the "BestIndex" is not into a CSol, but my data array.

    // This CYA test has -1 because last item in list is a sentinel:
    if( index >= sizeof( LanguageList ) / sizeof( * LanguageList ) - 1 )
        index = 0;

    //     int ExactCodePage;
    //     int FamilyCodePage;
    return LanguageList[ index ].ScreenName;
}

wchar_t * BestLanguageAbbreviationforIndex( size_t index )
{
    // Note that the "BestIndex" is not into a CSol, but my data array.
    // Caller may wish to copy lowercased, also to stop at any hyphen.
    // 0 will return L"un-known". Caller may wish to trap zero, guess.

    // This CYA test has -1 because last item in list is a sentinel:
    if( index >= sizeof( LanguageList ) / sizeof( * LanguageList ) - 1 )
        index = 0;

    //     int ExactCodePage;
    //     int FamilyCodePage;
    return LanguageList[ index ].Abbreviation;
}

size_t SomeLanguageIndexForGroupIndex( size_t index )
{
    if( index >= sizeof( SomeLanguageIDForGroupID ) / sizeof( * SomeLanguageIDForGroupID ) )
        index = 0;
    return SomeLanguageIDForGroupID[ index ];
}


size_t FamilyCodePageforLanguageIndex( size_t index )
{
    // Note that the "BestIndex" is not into a CSol, but my data array.
    #if DO_DEBUG_LANGUAGE
        ; SpewValue( L"LanguageCodePage: Parameter = ", index );
    #endif

    // This CYA test has -1 because last item in list is a sentinel:
    if( index >= sizeof( LanguageList ) / sizeof( * LanguageList ) - 1 )
        index = 0;

    #if DO_DEBUG_LANGUAGE
        ; SpewValue( L"LanguageCodePage: Returning ", LanguageList[ index ].ANSI_CodePage );
    #endif
    return LanguageList[ index ].ANSI_CodePage;
}

wchar_t * ccd[] =
{
   L"zw",
   L"zm",
   L"za",
   L"yu",
   L"yt",
   L"ye",
   L"ws",
   L"wf",
   L"vu",
   L"vn",
   L"vi",
   L"vg",
   L"ve",
   L"vc",
   L"va",
   L"uz",
   L"uy",
   L"us",
   L"um",
   L"uk",
   L"ug",
   L"ua",
   L"tz",
   L"tw",
   L"tv",
   L"tt",
   L"tr",
   L"tp",
   L"to",
   L"tn",
   L"tm",
   L"tk",
   L"tj",
   L"th",
   L"tg",
   L"tf",
   L"td",
   L"tc",
   L"sz",
   L"sy",
   L"sv",
   L"su",
   L"st",
   L"sr",
   L"so",
   L"sn",
   L"sm",
   L"sl",
   L"sk",
   L"sj",
   L"si",
   L"sh",
   L"sg",
   L"se",
   L"sd",
   L"sc",
   L"sb",
   L"sa",
   L"rw",
   L"ru",
   L"ro",
   L"re",
   L"qa",
   L"py",
   L"pw",
   L"pt",
   L"pr",
   L"pn",
   L"pm",
   L"pl",
   L"pk",
   L"ph",
   L"pg",
   L"pf",
   L"pe",
   L"pa",
   L"om",
   L"nz",
   L"nu",
   L"nr",
   L"np",
   L"no",
   L"nl",
   L"ni",
   L"ng",
   L"nf",
   L"ne",
   L"nc",
   L"na",
   L"mz",
   L"my",
   L"mx",
   L"mw",
   L"mv",
   L"mu",
   L"mt",
   L"ms",
   L"mr",
   L"mq",
   L"mp",
   L"mo",
   L"mn",
   L"mm",
   L"ml",
   L"mk",
   L"mh",
   L"mg",
   L"md",
   L"mc",
   L"ma",
   L"ly",
   L"lv",
   L"lu",
   L"lt",
   L"ls",
   L"lr",
   L"lk",
   L"li",
   L"lc",
   L"la",
   L"kz",
   L"ky",
   L"kw",
   L"kr",
   L"kp",
   L"kn",
   L"km",
   L"ki",
   L"kh",
   L"kg",
   L"ke",
   L"jp",
   L"jo",
   L"jm",
   L"it",
   L"is",
   L"ir",
   L"iq",
   L"io",
   L"in",
   L"il",
   L"ie",
   L"id",
   L"hu",
   L"ht",
   L"hr",
   L"hn",
   L"hm",
   L"hk",
   L"gy",
   L"gw",
   L"gu",
   L"gt",
   L"gs",
   L"gr",
   L"gq",
   L"gp",
   L"gn",
   L"gm",
   L"gl",
   L"gi",
   L"gh",
   L"gf",
   L"ge",
   L"gd",
   L"gb",
   L"ga",
   L"fr",
   L"fo",
   L"fm",
   L"fk",
   L"fj",
   L"fi",
   L"et",
   L"es",
   L"er",
   L"eh",
   L"eg",
   L"ee",
   L"ec",
   L"dz",
   L"do",
   L"dm",
   L"dk",
   L"dj",
   L"de",
   L"cz",
   L"cy",
   L"cx",
   L"cv",
   L"cu",
   L"cr",
   L"co",
   L"cn",
   L"cm",
   L"cl",
   L"ck",
   L"ci",
   L"ch",
   L"cg",
   L"cf",
   L"cc",
   L"ca",
   L"bz",
   L"by",
   L"bw",
   L"bv",
   L"bt",
   L"bs",
   L"br",
   L"bo",
   L"bn",
   L"bm",
   L"bj",
   L"bi",
   L"bh",
   L"bg",
   L"bf",
   L"be",
   L"bd",
   L"bb",
   L"ba",
   L"az",
   L"aw",
   L"au",
   L"at",
   L"as",
   L"ar",
   L"aq",
   L"ao",
   L"an",
   L"am",
   L"al",
   L"ai",
   L"ag",
   L"af",
   L"ae",
   L"ad",
   L"ac",
   L"zr",
   L"tl",
   L"rs",
   L"ps",
   L"nt",
   L"me",
   L"lb",
   L"je",
   L"im",
   L"gg",
   L"fx",
   L"eu",
   L"cs",
   L"cd",
   L"ax",
};

wchar_t * tld[] =
{
   L"aero",
   L"arts",
   L"biz",
   L"cat",
   L"com",
   L"coop",
   L"edu",
   L"firm",
   L"gov",
   L"info",
   L"int",
   L"jobs",
   L"mil",
   L"mobi",
   L"museum",
   L"name",
   L"net",
   L"nom",
   L"org",
   L"pro",
   L"rec",
   L"shop",
   L"tel",
   L"travel",
   L"web",
};

void SetupDomainNames( )
{
    int i = 0;
    int n = sizeof( tld )/sizeof( *tld );
    for( ;; )
    {
        size_t index = CSolTopLevelDomains.AddKey( tld[i] );
        #if DO_DEBUG_ADDFIND
            if( index <= 1 )
                { Spew( L"AddFind 1 at cvoc 4827" ); }
        #endif
        if( ++i == n )
            break;
    }

    i = 0;
    n = sizeof( ccd )/sizeof( *ccd );
    for( ;; )
    {
        size_t index = CSolCountryDomains.AddKey( ccd[i] );
        #if DO_DEBUG_ADDFIND
            if( index <= 1 )
                { Spew( L"AddFind 1 at cvoc 4827" ); }
        #endif
        if( ++i == n )
            break;
    }
}

void InitHtmlTagNames( )
{
    int i = 0;
    for( ;; )
    {
        TAGNAME * pTagName = TagNames + i++;
        if( pTagName->wcp == NULL )
            break;

        size_t index = CSolHtmlTagNames.AddKey( pTagName->wcp );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 4859" ); }
        #endif
        CSolHtmlTagNames.SetUserValue( index, pTagName->enumTag );

        #if DO_DEBUG_HTMLTAGS
            ; SpewValue( pTagName->wcp, pTagName->enumTag );
        #endif

    }
}

void InitHtmlAttrNames( )
{
    int i = 0;
    for( ;; )
    {
        ATTRNAME * pAttrName = AttrNames + i++;
        if( pAttrName->wcp == NULL )
            break;

        size_t index = CSolHtmlAttrNames.AddKey( pAttrName->wcp );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cvoc 5107" ); }
        #endif
        CSolHtmlAttrNames.SetUserValue( index, pAttrName->enumAttr );

        #if DO_DEBUG_HTMLATTRS
            ; SpewValue( pAttrName->wcp, pAttrName->enumAttr );
        #endif
    }
}


wchar_t * BinaryFileExtensions[] = {
    L"aiff",
    L"asf",
    L"au",
    L"avi",
    L"avs",
    L"bin",
    L"bmp",
    L"cab",
    L"cal",
    L"cfg",
    L"css",
    L"dat",
    L"dll",
    L"doc",
    L"drv",
    L"eps",
    L"exe",
    L"fnt",
    L"gif",
    L"gz",
    L"img",
    L"jpe",
    L"jpeg",
    L"jpg",
    L"js",
    L"mdb",
    L"mid",
    L"miff",
    L"mime",
    L"mov",
    L"mp3",
    L"mpg",
    L"pdf",
    L"ppt",
    L"ps",
    L"psd",
    L"qt",
    L"ra",
    L"ram",
    L"rtf",
    L"sea",
    L"sit",
    L"swf",
    L"tar",
    L"tif",
    L"vbs",
    L"wav",
    L"wmf",
    L"wmv",
    L"xls",
    L"xml",
    L"z",
    L"zip",
};

void SetupBinaryNames( )
{
    int i = 0;
    int n = sizeof( BinaryFileExtensions )/sizeof( *BinaryFileExtensions );
    for( ;; )
    {
        size_t index = CSolBinExtensions.AddKey( BinaryFileExtensions[i] );
        #if DO_DEBUG_ADDFIND
            if( index <= 1 )
                { Spew( L"AddFind 1 at cvoc 5188" ); }
        #endif
        if( ++i == n )
            break;
    }
}


size_t GroupIndexForLanguageIndex( size_t index )
{
    // Note that in/out "index" is not into a CSol, but my data arrays.

    // by design, LANGUAGE_UNKNOWN is 0
    // by design, LANGUAGE_GROUP_UNKNOWN is 0

    // This CYA test has -1 because last item in list is a sentinel:
    if( index >= sizeof( LanguageList ) / sizeof( * LanguageList ) - 1 )
        index = 0;

    return LanguageList[ index ].LGroupID;
}

wchar_t * GroupAbbreviationforGroupIndex( size_t index )
{
    // Note that the in "index" is not into a CSol, but my data array.
    // wchar_t * LanguageGroupList[] = {
    // L"un",     //   0 = unknown
    // ...

    if( index >= sizeof( LanguageGroupList ) / sizeof( * LanguageGroupList ) )
        index = 0;

    return LanguageGroupList[ index ];
}

wchar_t * GroupNameforGroupIndex( size_t index )
{
    // Note that the in "index" is not into a CSol, but my data array.
    // wchar_t * LanguageGroupName[ PAST_LANGUAGE_GROUP_IDS ] = {
    //     L"Unknown",           //    0
    //     L"Afrikaans",         //    1
    //     L"Amharic",           //    2

    if( index >= sizeof( LanguageGroupName ) / sizeof( * LanguageGroupName ) )
        index = 0;

    return LanguageGroupName[ index ];
}


size_t CrudeLog2( int value )
{
    size_t ToShift = value;
    size_t Log2Weight = 1; // start 1 up, as 0 is e-o-table
    for( ;; )
    {
        if( ToShift == 0 )
            break;
        Log2Weight ++;
        ToShift >>= 1;
    }
    return Log2Weight;
}

#ifndef _WIN32_WCE
// A temporary development feature?
// Only on Windows Desktop version.
void SaveVocabulary( )
{
    #if DO_DEBUG_CALLS
        Routine( L"500" );
    #endif

    wchar_t szFile[MAX_PATH];
    szFile[0] = NULL;
    // Let's try setting a default suggested output filename:
    wcscpy( szFile, L"Wordlist.txt" );

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrTitle = L"Save As ASCII C or UTF-8 Text";
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0C source (*.cpp)\0*.cpp\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_NOREADONLYRETURN|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = NULL; // I will handle N default extensions myself

    if ( ! GetSaveFileName( & ofn ) )
    {
        #ifndef _WIN32_WCE
            if( CommDlgExtendedError( ) ) // zero if user canceled
                ProgramError( L"GetSaveFileName" );
        #endif // not _WIN32_WCE

        return;
    }

    int AsCpp = 0; // Save Vocabulary: treat as 1=.cpp 0=.txt

    // Win CE returns a bizarre string containing the desired filename
    // followed by a semicolon, and the filter string; Fix that string:
    // I see now. Win CE appended my whole string from ofn.lpstrFilter.
    // Problem is gone now that I do multiple default extension myself.

    // Handle the multiple default extensions
    // rule 1: ofn.nFilterIndex returns last user filter index choice.
    // rule 2: nFileExtension is offset to first ext char in lpstrFile.
    //
    // Win Help says ( but empirically is untrue ):
    // If the user did not type an extension and lpstrDefExt is NULL,
    // this member specifies an offset to the terminating null character.
    // If the user typed '.' as the last character in the file name,
    // this member specifies zero.

    //     // Find out for myself what Windoze does.
    //     SpewTwo ( L"szFile", szFile );
    //     SpewValue ( L"wcslen( szFile )", wcslen( szFile ) );
    //     SpewValue ( L"ofn.nFileExtension", ofn.nFileExtension );
    //
    // szFile: X:\baretext
    // wcslen( szFile ): 11
    // ofn.nFileExtension: 0
    //
    // szFile: X:\dottext.
    // wcslen( szFile ): 11
    // ofn.nFileExtension: 11
    //
    // szFile: X:\texttext.txt
    // wcslen( szFile ): 15
    // ofn.nFileExtension: 12

    if( ofn.nFileExtension == 0 )
    {
        // Just in this one case, I will append the default extension.
        int n = wcslen( szFile );
        wchar_t * into = szFile + n;
        if( n < sizeof( szFile ) / sizeof( *szFile ) - 4 ) // safe for 4,null
        {
            switch ( ofn.nFilterIndex )
            {
            case 1:
                wcscpy( into, L".txt" );
                break;
            case 2:
                wcscpy( into, L".cpp" );
                break;
            }
        }
    }

    {
        // Do it again, sort of, to see if we all decided on .cpp

        int n = wcslen( szFile );
        wchar_t * scan = szFile + n;
        // Scan is at the terminating null of the filename.
        if( ( scan[ -4 ] | ' ' ) == '.'
        &&  ( scan[ -3 ] | ' ' ) == 'c'
        &&  ( scan[ -2 ] | ' ' ) == 'p'
        &&  ( scan[ -1 ] | ' ' ) == 'p' )
            AsCpp = 1; // Save Vocabulary: treat as 1=.cpp 0=.txt
    }

    // This will all be created as narrow ASCII text output, here:
    // Then I can call Fio.CommonFileSave( wcsFileName, ptr, count )

    CAsb CAsbOut;
    CAsb Salter;

    if( AsCpp )
    {
        CAsbOut.Add( ( BYTE * ) "// C syntax. Continuous (iswdigit/iswalpha) runs, final 0 count ends each long string.\r\n" );
        CAsbOut.Add( ( BYTE * ) "// Coding 0x20-0x7e=USASCII, 0x7f-0xff=octal escape, 0x0100-0x20ff = octal 1-32 prefix.\r\n" );
        CAsbOut.Add( ( BYTE * ) "// Any words that contained digits or characters over 0x20FF were omitted from lists. \r\n" );
    }
    else
    {
        // Output an MBS ( UTF-8 ) file marker sequence:
        static BYTE MBSHeader[3] = { 0xef, 0xbb, 0xbf };
        CAsbOut.Addn( MBSHeader, 3 );

        //  I will only handle UCS up to 16 bits, UTF-8 thus:
        //  0x00000000 - 0x0000007F: 0xxxxxxx
        //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
        //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
    }

    // Plan WAS ( confused about what Aliases means ):
    // Nest the original dump code in a loop to iterate CSolLanguageAliases.
    //
    // Plan has been fixed thus:
    // Iterate predefined vector[s] from 0 to PAST_LANGUAGE_GROUP_IDS - 1.
    // One vector is wchar_t * LanguageGroupList[ PAST_LANGUAGE_GROUP_IDS ].
    // Another vector, in CPag, will hold pointers to CSols for word lists.

    // Process any non-NULL items in vector CSolLGroupWords[].
    // Nevertheless, list all language group IDs with long names.
    int i = 0;
    for( ;; )
    {
        // I am having trouble with the %hs / %ls specifications.
        // ( Remember, %hs = half-width, but %ls = long width. )
        // Oh, because I had it two places, but I only fixed one.

        wchar_t * szAbbrev = GroupAbbreviationforGroupIndex( i );
        wchar_t * szLongLanguageName = GroupNameforGroupIndex( i );

        // L"en", 22 -- GROUP 22 is english, a fact I hard coded...
        int LimitTen = 10;
        int LimitTenK = 10000;
        if( i == LANGUAGE_GROUP_UNKNOWN )
        {
            LimitTen = 100; // show 10 x more
            LimitTenK = 100000;
        }
        if( i == 22 )
        {
            LimitTen = 20; // take twice as many blocks for english
            LimitTenK = 20000;
        }

        CSol * pSol = Pag.CSolLGroupWords[ i ];
        if( pSol == NULL )
        {
            char wk[100];
            sprintf( wk, "// Language Group %ls: %ls: Empty list.\r\n\r\n",
                szAbbrev,
                szLongLanguageName );
            CAsbOut.Add( ( BYTE * ) wk );
        }
        else
        {
            // Output most frequent words in a format
            // ready to reuse in the CLan.cpp module.
            // Or, as Unicode .txt for human perusal.

            CSol pSolReorderAllWords( CSOL_SCALAR );
            size_t nTotalWordCount = 0;
            size_t HighestWordCount = 0;
            int ItemCount = 0;
            int nIgnored = 0;

            // PASS 1:

            CoIt * pMalVector2 = pSol->GetSortedVector( CSOL_BACKWARD );
            if( pMalVector2 == NULL )
            {
                continue; // return;
            }
            size_t take2 = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector2 + take2++;
                if( pCoIt->IsSentinel )
                    break;
                ItemCount ++;
                size_t WordCount = pCoIt->User.Value;
                WordCount &= MASK_17BIT_WORD_COUNT;
                nTotalWordCount += WordCount;
                if( HighestWordCount < WordCount )
                    HighestWordCount = WordCount;

                wchar_t * FullKey = CoItFullKey( pCoIt );

                #if DO_DEBUG_VOCDUMP
                    ; SpewTwo( L"item", FullKey );
                #endif

                int okay = 1;
                {
                    // No word in salt lists can include char > 0x20ff.
                    // No word in salt lists can include any digit 0-9.

                    wchar_t * scan = FullKey;
                    for( ;; )
                    {
                        wchar_t wc = *scan++;
                        if( wc == NULL )
                            break;

                        #if DO_DEBUG_VOCDUMP
                            ; SpewValue( L"-", wc );
                        #endif

                        if( wc > 0x20ff
                        || wc < ' '
                        || iswdigit( wc ) )
                        {
                            #if DO_DEBUG_VOCDUMP
                                ; Spew( L"=Not Okay" );
                            #endif
                            okay = 0;
                            break;
                        }
                    }
                }
                if( okay )
                {
                    // Prepare the word count and word string for pass 2.
                    wchar_t wk[MAX_LEGITIMATE_WORD_LENGTH + 20];
                    wsprintf( wk, L"%13d%s", WordCount & 0x00ffffff, FullKey );
                    pSolReorderAllWords.AddKey( wk );
                }
                else
                {
                    nIgnored ++;
                }

                MyFree( 397, UNPREDICTABLE, FullKey );
                FullKey = NULL;
            }
            MyFree( 400, UNPREDICTABLE, pMalVector2 );
            pMalVector2 = NULL;

            // Suppose I want to scale so highest count is 9999.
            // What pair of a factor and a divisor will stay in
            // 31 bits without losing significance unnecessarily,
            // in math: NewCount = Factor * WordCount / Divisor?

            if( HighestWordCount != 0 ) // prevent divide-by-zero
            {
                // A top count of 9999 leaves last word at 1.
                // But 1 is not a significant word frequency.
                // Change top word count to say, 99,999.
                // Count range in 24 LSB is about 16 million.
                // No, that looks pretty ugly and would dwarf
                // the first few pages, which is all it helps.

                size_t Factor = 0x7fffffff / HighestWordCount;
                size_t Divisor = 0x7fffffff / 9999; // the only place constant appears

                {
                    char wk[200];
                    sprintf( wk, "// Language Group %ls: %ls: %d Words, %d Unique, %d Ignored, %d Highest count.\r\n\r\n",
                    szAbbrev,
                    szLongLanguageName,
                    nTotalWordCount,
                    ItemCount,
                    nIgnored,
                    HighestWordCount );
                    CAsbOut.Add( ( BYTE * ) wk );
                }

                int OpenedBrace = 0;
                int LineLength = 0;
                int StringLength = 0;
                int nStrings = 0;

                // PASS 2:
                int Pass2ItemCount = 0;

                CoIt * pMalVector3 = pSolReorderAllWords.GetSortedVector( CSOL_BACKWARD ); // Big # first
                if( pMalVector3 == NULL )
                {
                    MyFree( 400, UNPREDICTABLE, pMalVector2 );
                    continue; // return;
                }
                size_t take3 = 0;
                int breaking = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector3 + take3++;
                    if( pCoIt->IsSentinel )
                        break;
                    wchar_t * FullKey = CoItFullKey( pCoIt ); // a malloc, user frees
                    // Recover count, word, from format: "%13d%s"
                    wchar_t * scan = FullKey;
                    for( ;; )
                    {
                        if( *scan != ' ' ) // skip leading spaces
                            break;
                        scan++;
                    }
                    size_t WordCount = 0;
                    for( ;; )
                    {
                        if( !iswdigit( *scan ) ) // atoi digits
                            break;
                        WordCount *= 10;
                        WordCount += *scan - '0';
                        scan++;
                    }
                    size_t NewCount = Factor * WordCount / Divisor;

                    // Shall I format the word for .cpp or for .txt?
                    // Remember I code in wchar_t, but output as BYTE.

                    if( AsCpp )
                    {
                        // Format for .cpp

                        if( ! OpenedBrace )
                        {
                            // Changing the array name from Vocabulary%03d[]...
                            char wk[100];

                            // %hs/%ls: wsprintf %hs accepts half-size char strings.
                            // %hs/%ls: sprintf %ls accepts long-size char strings.

                            sprintf( wk, "unsigned char %ls%03d[] = {\r\n\"",
                                szAbbrev,
                                nStrings + 1 );
                            CAsbOut.Add( ( BYTE * ) wk );
                            OpenedBrace = 1;
                            LineLength = 0;
                            StringLength = 0;


                            // I also need to list block name, with group id:
                            //     { zh002, 122 },

                            sprintf( wk, "    { %ls%03d, %d },\r\n",
                                szAbbrev,
                                nStrings + 1,
                                i ); // remember, i? Outer loop?
                            Salter.Add( ( BYTE * ) wk );
                        }

                        {
                            // This double octal escape could produce 8 chars
                            // per input char. Also, output lines at 10 words
                            // per line ran up to 1800 chars, choked Brief.

                            // So let's regulate output line length instead.
                            // The braces will make the first and last quote,
                            // so I just insert quote newline quote as reqd.

                            char wk[MAX_LEGITIMATE_WORD_LENGTH * 8 + 20];
                            char *into = wk;
                            sprintf( wk, "%d", NewCount ); // for .cpp format
                            while( *into != NULL ) into ++;

                            for( ;; )
                            {
                                wchar_t wc = *scan++;
                                if( wc == NULL )
                                    break;
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
                            *into = NULL;

                            // some escaped tokens can be very long, so
                            // try size, output line before adding token

                            int adding = strlen( wk );
                            if( LineLength + adding > 250 )
                            {
                                StringLength += LineLength;
                                LineLength = 0;

                                // Stay well under the 64K compiler limit per string.
                                if( StringLength > 10000 )
                                {
                                    // Close string with one final "0" digit.
                                    OpenedBrace = 0;
                                    CAsbOut.Add( ( BYTE * ) "0\"};\r\n\r\n" );

                                    nStrings ++;
                                    if( nStrings == LimitTen )
                                        breaking = 1;
                                }
                                else
                                {
                                    CAsbOut.Add( ( BYTE * ) "\"\r\n\"" ); // quote nl quote
                                }
                            }

                            if( ! breaking )
                            {
                                // Below dangling word addition ended up outside block.
                                // Cloning this block from above should plug that hole:

                                if( ! OpenedBrace )
                                {
                                    // Changing the array name from Vocabulary%03d[]...
                                    char wk[100];

                                    // %hs/%ls: wsprintf %hs accepts half-size char strings.
                                    // %hs/%ls: sprintf %ls accepts long-size char strings.

                                    sprintf( wk, "unsigned char %ls%03d[] = {\r\n\"",
                                        szAbbrev,
                                        nStrings + 1 );
                                    CAsbOut.Add( ( BYTE * ) wk );
                                    OpenedBrace = 1;
                                    LineLength = 0;
                                    StringLength = 0;


                                    // I also need to list block name, with group id:
                                    //     { zh002, 122 },

                                    sprintf( wk, "    { %ls%03d, %d },\r\n",
                                        szAbbrev,
                                        nStrings + 1,
                                        i ); // remember, i? Outer loop?
                                    Salter.Add( ( BYTE * ) wk );
                                }

                                // Now I can add that token.
                                LineLength += adding;
                                CAsbOut.Add( ( BYTE * ) wk );
                            }
                        }
                    }
                    else
                    {
                        // Format for .txt - one per line - no loop limit

                        char wk[MAX_LEGITIMATE_WORD_LENGTH * 3 + 20];
                        char *into = wk;
                        sprintf( wk, "%4d ", NewCount ); // for .txt format
                        while( *into != NULL ) into ++;

                        for( ;; )
                        {
                            wchar_t wc = *scan++;
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

                                    *into++ = ( char ) wc2;
                                    *into++ = ( char ) wc1;
                                    *into++ = ( char ) wc0;
                                }
                                else
                                {
                                    wchar_t wc1 = wc >> ( 6 + 0 );
                                    wc1 &= 0x001f;
                                    wc1 |= 0x00c0;

                                    wchar_t wc0 = wc >> ( 0 );
                                    wc0 &= 0x003f;
                                    wc0 |= 0x0080;

                                    *into++ = ( char ) wc1;
                                    *into++ = ( char ) wc0;
                                }
                            }
                            else
                            {
                                *into++ = ( char ) wc;
                            }
                        }
                        *into++ = '\r';
                        *into++ = '\n';
                        *into = NULL;
                        CAsbOut.Add( ( BYTE * ) wk );

                        Pass2ItemCount ++;
                        if( Pass2ItemCount > LimitTenK ) // arbitrary limit as .txt
                           breaking = 1;
                    }

                    MyFree( 474, UNPREDICTABLE, FullKey );
                    FullKey = NULL;

                    if( breaking )
                        break;

                }
                MyFree( 478, UNPREDICTABLE, pMalVector3 );
                pMalVector3 = NULL;

                // In case the loop stopped short, finish C syntax

                if( OpenedBrace )
                {
                    CAsbOut.Add( ( BYTE * ) "0\"};\r\n\r\n" );
                }
            }
        }
        if( ++i == PAST_LANGUAGE_GROUP_IDS )
            break;
    }

    // Now I need to create this index to all the blocks:
    // UCPUI SaltLists[] = {
    //     { af001, 1 },
    // ...
    //     { zh002, 122 },
    //     { NULL, 0 },
    // };

    if( AsCpp )
    {
        CAsbOut.Add( ( BYTE * ) "\r\nUCPUI SaltLists[] = {\r\n" );
        size_t nMallocBuf = 0;
        BYTE * pMallocBuf = Salter.GetBuffer( & nMallocBuf ); // a malloc, user frees
        CAsbOut.Addn( pMallocBuf, nMallocBuf );
        MyFree( 2027, zx, pMallocBuf );
        CAsbOut.Add( ( BYTE * ) "    { NULL, 0 },\r\n};\r\n" );
    }

    // One more thing... Use the same techniques to output
    // the vowels and consonants list, if I counted them.
    // Especially, I need the escape idioms to do Unicode.

    #if DEBUG_REVISE_VOWELS

    // Same loop from above can be adapted to do Cons/Vows.
    // Simple two-loop advance: Loop1=Vows, Loop2=Cons.
    CSol * pSol = & CSolNewVows;
    wchar_t * VowsCons = L"Vowels";
    int vcsn = 0;
    for( ;; )
    {

        // Output most frequent words in a format
        // ready to reuse in the CLan.cpp module.
        // Or, as Unicode .txt for human perusal.

        CSol pSolReorderAllWords( CSOL_SCALAR );
        size_t nTotalWordCount = 0;
        size_t HighestWordCount = 0;
        int ItemCount = 0;
        int nIgnored = 0;

        // PASS 1:

        CoIt * pMalVector2 = pSol->GetSortedVector( CSOL_BACKWARD );
        if( pMalVector2 == NULL )
        {
            continue; // return;
        }
        size_t take2 = 0;
        for( ;; )
        {
            CoIt * pCoIt = pMalVector2 + take2++;
            if( pCoIt->IsSentinel )
                break;
            ItemCount ++;
            size_t WordCount = pCoIt->User.Value;
            WordCount &= MASK_17BIT_WORD_COUNT;
            nTotalWordCount += WordCount;
            if( HighestWordCount < WordCount )
                HighestWordCount = WordCount;

            wchar_t * FullKey = CoItFullKey( pCoIt );

            #if DO_DEBUG_VOCDUMP
                ; SpewTwo( L"item", FullKey );
            #endif

            int okay = 1;
            {
                // No word in salt lists can include char > 0x20ff.
                // No word in salt lists can include any digit 0-9.

                wchar_t * scan = FullKey;
                for( ;; )
                {
                    wchar_t wc = *scan++;
                    if( wc == NULL )
                        break;

                    #if DO_DEBUG_VOCDUMP
                        ; SpewValue( L"-", wc );
                    #endif

                    if( wc > 0x20ff
                    || wc < ' '
                    || iswdigit( wc ) )
                    {
                        #if DO_DEBUG_VOCDUMP
                            ; Spew( L"=Not Okay" );
                        #endif
                        okay = 0;
                        break;
                    }
                }
            }
            if( okay )
            {
                // Prepare the word count and word string for pass 2.
                wchar_t wk[MAX_LEGITIMATE_WORD_LENGTH + 20];
                wsprintf( wk, L"%13d%s", WordCount & 0x00ffffff, FullKey );
                pSolReorderAllWords.AddKey( wk );
            }
            else
            {
                nIgnored ++;
            }

            MyFree( 397, UNPREDICTABLE, FullKey );
            FullKey = NULL;
        }
        MyFree( 400, UNPREDICTABLE, pMalVector2 );
        pMalVector2 = NULL;

        // Suppose I want to scale so highest count is 9999.
        // What pair of a factor and a divisor will stay in
        // 31 bits without losing significance unnecessarily,
        // in math: NewCount = Factor * WordCount / Divisor?

        if( HighestWordCount != 0 ) // prevent divide-by-zero
        {
            // A top count of 9999 leaves last word at 1.
            // But 1 is not a significant word frequency.
            // Change top word count to say, 99,999.
            // Count range in 24 LSB is about 16 million.
            // No, that looks pretty ugly and would dwarf
            // the first few pages, which is all it helps.

            int OpenedBrace = 0;
            int LineLength = 0;
            int StringLength = 0;
            int nStrings = 0;

            // PASS 2:
            int Pass2ItemCount = 0;

            CoIt * pMalVector3 = pSolReorderAllWords.GetSortedVector( CSOL_BACKWARD ); // Big # first
            if( pMalVector3 == NULL )
            {
                MyFree( 400, UNPREDICTABLE, pMalVector2 );
                continue; // return;
            }
            size_t take3 = 0;
            int breaking = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector3 + take3++;
                if( pCoIt->IsSentinel )
                    break;
                wchar_t * FullKey = CoItFullKey( pCoIt ); // a malloc, user frees
                // Recover count, word, from format: "%13d%s"
                wchar_t * scan = FullKey;
                for( ;; )
                {
                    if( *scan != ' ' ) // skip leading spaces
                        break;
                    scan++;
                }
                size_t WordCount = 0;
                for( ;; )
                {
                    if( !iswdigit( *scan ) ) // atoi digits
                        break;
                    WordCount *= 10;
                    WordCount += *scan - '0';
                    scan++;
                }

                // Now in this loop, I want a log2 of count:

                size_t NewCount = CrudeLog2( WordCount );

                // Shall I format the word for .cpp or for .txt?
                // Remember I code in wchar_t, but output as BYTE.

                if( AsCpp )
                {
                    // Format for .cpp

                    if( ! OpenedBrace )
                    {
                        // Changing the array name from Vocabulary%03d[]...
                        char wk[100];

                        // %hs/%ls: wsprintf %hs accepts half-size char strings.
                        // %hs/%ls: sprintf %ls accepts long-size char strings.

                        sprintf( wk, "unsigned char %ls%03d[] = {\r\n\"",
                            VowsCons, ++vcsn );
                        CAsbOut.Add( ( BYTE * ) wk );
                        OpenedBrace = 1;
                        LineLength = 0;
                        StringLength = 0;
                    }

                    {
                        // This double octal escape could produce 8 chars
                        // per input char. Also, output lines at 10 words
                        // per line ran up to 1800 chars, choked Brief.

                        // So let's regulate output line length instead.
                        // The braces will make the first and last quote,
                        // so I just insert quote newline quote as reqd.

                        char wk[MAX_LEGITIMATE_WORD_LENGTH * 8 + 20];
                        char *into = wk;
                        sprintf( wk, "%d", NewCount ); // for .cpp format
                        while( *into != NULL ) into ++;

                        for( ;; )
                        {
                            wchar_t wc = *scan++;
                            if( wc == NULL )
                                break;
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
                        *into = NULL;

                        // some escaped tokens can be very long, so
                        // try size, output line before adding token

                        int adding = strlen( wk );
                        if( LineLength + adding > 250 )
                        {
                            StringLength += LineLength;
                            LineLength = 0;

                            // Stay well under the 64K compiler limit per string.
                            if( StringLength > 10000 )
                            {
                                // Close string with one final "0" digit.
                                OpenedBrace = 0;
                                CAsbOut.Add( ( BYTE * ) "0\"};\r\n\r\n" );

                                nStrings ++;
                                // no limit in vow-con
                            }
                            else
                            {
                                CAsbOut.Add( ( BYTE * ) "\"\r\n\"" ); // quote nl quote
                            }
                        }

                        if( ! breaking )
                        {
                            // Below dangling word addition ended up outside block.
                            // Cloning this block from above should plug that hole:

                            if( ! OpenedBrace )
                            {
                                // Changing the array name from Vocabulary%03d[]...
                                char wk[100];

                                // %hs/%ls: wsprintf %hs accepts half-size char strings.
                                // %hs/%ls: sprintf %ls accepts long-size char strings.

                                sprintf( wk, "unsigned char %ls%03d[] = {\r\n\"",
                                    VowsCons, ++vcsn );
                                CAsbOut.Add( ( BYTE * ) wk );
                                OpenedBrace = 1;
                                LineLength = 0;
                                StringLength = 0;

                            }

                            // Now I can add that token.
                            LineLength += adding;
                            CAsbOut.Add( ( BYTE * ) wk );
                        }
                    }
                }
                else
                {
                    // Format for .txt - one per line - no loop limit

                    char wk[MAX_LEGITIMATE_WORD_LENGTH * 3 + 20];
                    char *into = wk;
                    sprintf( wk, "%4d ", NewCount ); // for .txt format
                    while( *into != NULL ) into ++;

                    for( ;; )
                    {
                        wchar_t wc = *scan++;
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

                                *into++ = ( char ) wc2;
                                *into++ = ( char ) wc1;
                                *into++ = ( char ) wc0;
                            }
                            else
                            {
                                wchar_t wc1 = wc >> ( 6 + 0 );
                                wc1 &= 0x001f;
                                wc1 |= 0x00c0;

                                wchar_t wc0 = wc >> ( 0 );
                                wc0 &= 0x003f;
                                wc0 |= 0x0080;

                                *into++ = ( char ) wc1;
                                *into++ = ( char ) wc0;
                            }
                        }
                        else
                        {
                            *into++ = ( char ) wc;
                        }
                    }
                    *into++ = '\r';
                    *into++ = '\n';
                    *into = NULL;
                    CAsbOut.Add( ( BYTE * ) wk );

                    Pass2ItemCount ++;
                    // no limit in vow-con
                }

                MyFree( 474, UNPREDICTABLE, FullKey );
                FullKey = NULL;

                if( breaking )
                    break;

            }
            MyFree( 478, UNPREDICTABLE, pMalVector3 );
            pMalVector3 = NULL;

            // In case the loop stopped short, finish C syntax

            if( OpenedBrace )
            {
                CAsbOut.Add( ( BYTE * ) "0\"};\r\n\r\n" );
            }
        }


        // This is the loop advance: Loop1=Vows, Loop2=Cons.

        if( pSol == & CSolNewCons )
            break;
        pSol = & CSolNewCons;
        VowsCons = L"Consonants";
    }
    #endif // DEBUG_REVISE_VOWELS


    {
        size_t nMallocBuf = 0;
        BYTE * pMallocBuf = CAsbOut.GetBuffer( & nMallocBuf ); // a malloc, user frees
        Fio.CommonFileSave( szFile, pMallocBuf, nMallocBuf );
        MyFree( 2027, zx, pMallocBuf );
    }
}
#endif // not _WIN32_WCE

int IsBinaryExtension( wchar_t * szUrlOrFilename )
{
    // LoadInternetCacheItem calls me, both for URL and cached filename,
    // and LoadInternetCacheItem ignores file if I return true;

    // ObtainOneURLContent calls me, for a URL being downloaded,
    // to revise pOnePaper->HttpHeaderContentType to CONTENT_OTHER
    // even though ParseHttpHeader had set that variable per header.

    // I see no point in making my return assist FullFileNameForUrl.

    #if DO_DEBUG_CALLS
        Routine( L"314" );
    #endif

    #if DO_DEBUG_BINARIES
        ; Spew( L"IsBinaryExtension was passed:" );
        ; Spew( szUrlOrFilename );
    #endif

    wchar_t BackwardsExtension[12]; // .1234 is the longest I have.
    int fill = 10;

    BackwardsExtension[fill] = NULL; // scanning and collecting backwards
    fill --; // So: When fill==9, I am holding zero chars.

    wchar_t * from = szUrlOrFilename;

    // I originally took string length, but, in order to
    // remove query or fragments from URLS, scan forward
    // until null, or until '?' or '#'.
    // That's still okay for filenames.

    wchar_t * scan = from;
    for( ;; )
    {
        if( *scan == NULL ) // doing wcslen: stop at end
            break;
        if( *scan == '?' ) // stop earlier if query part
            break;
        if( *scan == '#' ) // stop earlier if fragment part
            break;
        scan++;
    }

    for( ;; )
    {
        if( --scan == from ) // CYA first
            break;

        wchar_t wc = scan[0];

        if( wc > 'z' )  // only usascii alnums interest me
            break;

        if( !iswalnum ( wc ) )
            break;

        BackwardsExtension[fill] = wc | ' '; // 0-9, a-z

        if( -- fill < 2 )   // only 1-4 chars in my list
            break;
    }


    // When fill==9, I am holding zero chars.
    // So when fill==8, I am holding 1 chars.
    // So when fill==5, I am holding 4 chars.

    if( fill < 5
    || fill > 8 )
        return 0; // 0 = no prejudice

    if( scan[0] != '.' )  // require the dot prior to it
        return 0; // 0 = no prejudice

    #if DO_DEBUG_BINARIES
        ; Spew( L"IsBinaryExtension looking up:" );
        ; Spew( BackwardsExtension + fill + 1 );
    #endif

    size_t index = CSolBinExtensions.Find( BackwardsExtension + fill + 1 );
    #if DO_DEBUG_ADDFIND
        if( index <= 1 )
            { Spew( L"AddFind 1 at cwww 3920" ); }
    #endif

    if( index != NULL )
    {
        #if DO_DEBUG_BINARIES
            ; Spew( L"DislikeExtension - Rejected:" );
            ; Spew( szUrlOrFilename );
        #endif
        return 1; // Add Cache can skip it. Add Fetch treat as binary.
    }
    return 0; // 0 = no prejudice
}

wchar_t * FullFileNameForUrl( wchar_t * szFolderPath, wchar_t * szUrl, int AsBin, int SaveAsAQrp, int FromEngineNo )
{
    // Note: I may return a NULL for failure to determine a filename.

    // I am called from CWww's GetterHelper when called upon to save a
    // downloaded file determined to be binary. ( See IsBinaryExtension. )
    // He tests for null and skips the saving.

    // I am called from CFio's SaveFolder to save a parsed web page:
    // either an html or text file, also internal/pasted text file.
    // SaveFolder never asks me to do a thread progress / result log.
    // He tests for null and skips the saving.

    // I will be called from CFio's SaveFile( ) for ID_FILE_SAVEFILE
    // with an empty path, to suggest a name based on current view.
    // SaveFile might be on a view of a thread progress / result log.
    // He is not written yet...


    #if DO_DEBUG_CALLS
        Routine( L"133" );
    #endif
    // Do something smart to hash a filename on pUrl.
    // Attach to path, which may have a final \ or /.
    // Make it valid, end in .htm, and avoid periods.
    // Return in a new malloc, user frees.

    // AsBin 0 = .htm ( always, for papers in save folder )
    // AsBin 1 = binary / per url extension ( but not .htm )

    // fix any time: regulate in FullFileNameForUrl:
    // What is my maximum filename length? I'm not sure.
    // Empirically, I see saved files about 40-50 chars.

    wchar_t UrlExtension[15];
    int FillExtension = 3; // skip 3 to allow safe [-1,-2,-3] tests

    // For a first cut, concatenate in a fat malloc.

    size_t StrLenPath = wcslen( szFolderPath );
    size_t StrLenUrl = wcslen( szUrl );

    // Allow 1 char for \, 1 for _, 11 chars for Hash and ".htm", 1 for NULL.
    // Raise that by fat 10, calling, say, 12 the maximum StrLenUrl extension.

    size_t nMallocLen = StrLenPath + StrLenUrl + 26;

    wchar_t * FullFileName = ( wchar_t * ) MyMalloc( 340, nMallocLen * sizeof( wchar_t ) );
    wchar_t * into = FullFileName;

    size_t nUsable = StrLenPath;

    if( szFolderPath[ StrLenPath - 1 ] == '/'
    || szFolderPath[ StrLenPath - 1 ] == '\\' )
    {
        nUsable = StrLenPath - 1;
    }
    memcpy( into, szFolderPath, nUsable * sizeof( wchar_t ) );
    into += nUsable;
    *into++ = '\\';

    if( SaveAsAQrp ) // bool: to prefix _ and number to filename
    {
        // Therefore, add the 3-digit value to filename.
        *into++ = '_'; // prefix to search engine filenames
        *into++ = '0' + FromEngineNo / 100 % 10;
        *into++ = '0' + FromEngineNo / 10 % 10;
        *into++ = '0' + FromEngineNo % 10;
        *into++ = '_'; // this underscore separates ordinal from name
    }

    // Up to this point, I have copied the folder path and /.

    // After fiddling with heuristics all night, I remember Crack:

    URL_COMPONENTS UrlComponents;
    memset( & UrlComponents, 0, sizeof( URL_COMPONENTS ) );
    UrlComponents.dwStructSize = sizeof( URL_COMPONENTS );

    // Leave pointers NULL and sizes non-zero to get pointers into my URL.
    UrlComponents.dwHostNameLength = StrLenUrl;
    UrlComponents.dwUrlPathLength = StrLenUrl;

    if( ! InternetCrackUrl( szUrl, 0, 0, & UrlComponents ) )
    {
        // Don't stop program on uncertain error possibility.
        MyFree( 384, zx, FullFileName );
        FullFileName = NULL;
        return NULL; // failure
    }

    // Good chance to screen oddballs, like "ms-its:"
    if( UrlComponents.nScheme != INTERNET_SCHEME_FTP // ftp is okay here
    && UrlComponents.nScheme != INTERNET_SCHEME_HTTP
    && UrlComponents.nScheme != INTERNET_SCHEME_HTTPS )
    {
        MyFree( 394, zx, FullFileName );
        FullFileName = NULL;
        return NULL; // failure
    }

    // UrlComponents.lpszHostName, UrlComponents.dwHostNameLength //
    // UrlComponents.lpszUrlPath,  UrlComponents.dwUrlPathLength  //

    if( UrlComponents.lpszHostName != NULL
    && UrlComponents.dwHostNameLength > 0 )
    {
        // Take part of the domain name into filename.
        // wchar_t * StartDomain = UrlComponents.lpszHostName;
        // wchar_t * PastDomain = StartDomain + UrlComponents.dwHostNameLength;

        // I am changing this to use a new function to exclude prefix:
        // PointPastDomainPrefix( wchar_t * szDomain, wchar_t * * pPastDomain, )
        // Which requires that I insert a temporary NULL byte:
        // Also, it will return a pointer to a new endpoint.

        wchar_t save = UrlComponents.lpszHostName[ UrlComponents.dwHostNameLength ];
        UrlComponents.lpszHostName[ UrlComponents.dwHostNameLength ] = NULL;

        wchar_t * PastDomain = NULL;
        wchar_t * StartDomain = PointPastDomainPrefix( UrlComponents.lpszHostName, & PastDomain );

        wchar_t save2 = NULL;
        if( PastDomain != NULL )
        {
            save2 = *PastDomain;
            *PastDomain = NULL;
        }

        // Now copy up to say 14 chars of domain name.
        if( PastDomain > StartDomain )
        {
            int limit = 0;
            wchar_t * from = StartDomain;
            for( ;; )
            {
                if( from >= PastDomain )
                    break;
                wchar_t c = *from++;
                if( c < 127 && iswalpha( c ) )
                {
                    *into++ = c; // file-safe, us-ascii / alpha only
                    if( ++limit == 14 )
                        break;
                }
            }
        }

        if( PastDomain != NULL )
        {
            *PastDomain = save2;
        }

        UrlComponents.lpszHostName[ UrlComponents.dwHostNameLength ] = save;
    }

    *into++ = '_'; // this underscore separates domain from path in filename

    if( UrlComponents.lpszUrlPath != NULL
    && UrlComponents.dwUrlPathLength > 1 ) // skip initial / character
    {
        // Take part of the path part into filename.
        // Let's take any first word, and then, any last word.
        wchar_t * StartPath = UrlComponents.lpszUrlPath;
        wchar_t * PastPath = StartPath + UrlComponents.dwUrlPathLength;

        // First, scan forward from the top...

        wchar_t * Start1 = StartPath + 1; // skip initial / character
        wchar_t * Past1 = PastPath;

        wchar_t * scan = Start1;
        for( ;; )
        {
            if( scan == Past1 )
                break;
            if( scan[0] == '\\'
            || scan[0] == '/' )
            {
                Past1 = scan;
                break;
            }
            scan ++;
        }


        // Also, scan backward from the end...

        // A fix-later comment claimed this routine's search for an extension
        // in the URL does not stop at a '?' parameter, but that could not be,
        // because I am searching in the path from crack. Extras were removed.

        #if DO_DEBUG_URLFILENM
            ; SpewValue( UrlComponents.lpszUrlPath, UrlComponents.dwUrlPathLength );
        #endif

        wchar_t * Start2 = StartPath + 1; // skip initial / character
        wchar_t * Past2 = PastPath;
        int SawDot = 0;

        scan = Past2;
        for( ;; )
        {
            if( --scan == Start2 )
            {
                #if DO_DEBUG_URLFILENM
                    ; SpewValue( scan, -1 );
                #endif
                break;
            }
            if( scan[0] == '.' )
            {
                Past2 = scan; // drop any filename suffix
                SawDot = 1;
                #if DO_DEBUG_URLFILENM
                    ; SpewValue( scan, -2 );
                #endif
            }

            if( ! SawDot
            && FillExtension < 14 )
                UrlExtension[FillExtension++] = scan[0];

            if( scan[-1] == '\\'
            || scan[-1] == '/' )
            {
                Start2 = scan;
                #if DO_DEBUG_URLFILENM
                    ; SpewValue( scan, -3 );
                #endif
                break;
            }
        }

        if( ! SawDot )
        {
            // What happened here?
            // I ended up with "rawl" as file extension.
            // Oh, because no dot at all.
            // If no dot, clear copied chars.
            // http://www.waste.org/crawl/

            FillExtension = 3; // re-init to empty
        }

        // reject the common folder names
        // index.* and default.*
        // by searching backward again.

        if(
         ( ( Start2[0] | ' ' ) == 'i'
        && ( Start2[1] | ' ' ) == 'n'
        && ( Start2[2] | ' ' ) == 'd'
        && ( Start2[3] | ' ' ) == 'e'
        && ( Start2[4] | ' ' ) == 'x'
        && Start2[5] == '.' )
        ||
         ( ( Start2[0] | ' ' ) == 'd'
        && ( Start2[1] | ' ' ) == 'e'
        && ( Start2[2] | ' ' ) == 'f'
        && ( Start2[3] | ' ' ) == 'a'
        && ( Start2[4] | ' ' ) == 'u'
        && ( Start2[5] | ' ' ) == 'l'
        && ( Start2[6] | ' ' ) == 't'
        && Start2[7] == '.' )
        )
        {
            Past2 = Start2 - 2; // skip bkwds over / character
            Start2 = StartPath + 1; // replace this end.

            if( Past2 > Start2 )
            {
                scan = Past2;
                for( ;; )
                {
                    if( --scan == Start2 )
                        break;
                    if( scan[0] == '.' )
                        Past2 = scan; // drop any filename suffix
                    if( scan[-1] == '\\'
                    || scan[-1] == '/' )
                    {
                        Start2 = scan;
                        break;
                    }
                }
            }
        }

        if( Start1 < Past1
        && Past1 < Start2
        && Start2 < Past2 )
        {
            // There is both an initial and final part. Do initial.
            int limit = 0;
            wchar_t * from = Start1;
            for( ;; )
            {
                if( from >= Past1 )
                    break;
                wchar_t c = *from++;
                if( c < 127 && iswalpha( c ) )
                {
                    *into++ = c; // file-safe, us-ascii / alpha only
                    if( ++limit == 14 )
                        break;
                }
            }
            *into++ = '_'; // this underscore separates initial/middle path parts in filename
        }

        if( Start2 < Past2 )
        {
            // There is a final part. Do it.
            int limit = 0;
            wchar_t * from = Start2;
            for( ;; )
            {
                if( from >= Past2 )
                    break;
                wchar_t c = *from++;
                if( c < 127 && iswalpha( c ) )
                {
                    *into++ = c; // file-safe, us-ascii / alpha only
                    if( ++limit == 14 )
                        break;
                }
            }
            *into++ = '_'; // this underscore separates middle/final path parts in filename
        }
    }

    // Append a hash number
    unsigned int Hash = CrcHash( szUrl );
    *into++ = '0' + Hash % 10;
    *into++ = '0' + ( Hash / 10 ) % 10;
    *into++ = '0' + ( Hash / 100 ) % 10;
    *into++ = '0' + ( Hash / 1000 ) % 10;
    *into++ = '0' + ( Hash / 10000 ) % 10;
    *into++ = '0' + ( Hash / 100000 ) % 10;
    *into++ = '.';

    if( AsBin )
    {
        // Try to get a filename extension from the URL.
        // If the extension is .htm or .html, use .html ( 4 chars ).
        // Whereas, Non-Binary Save Folder uses .htm ( 3 chars )
        // If no extension on url, use .html ( 4 chars )

        // Note init of FillExtension = 3 to allow safe [-1,-2,-3] tests
        if( FillExtension > 3
        && FillExtension < 13 ) // arbitrary limit
        {
            // Separate out the .htm or .html cases:
            if( ( UrlExtension[ FillExtension - 1 ] | ' ' ) == 'h'
            &&  ( UrlExtension[ FillExtension - 2 ] | ' ' ) == 't'
            &&  ( UrlExtension[ FillExtension - 3 ] | ' ' ) == 'm' )
            {
                *into++ = 'h'; // 4-char version serving BINARY case
                *into++ = 't';
                *into++ = 'm';
                *into++ = 'l';
            }
            else
            {
                // Use whatever other URL extension for saving files.
                for( ;; )
                {
                    // pre-decrement as I post-increment
                    wchar_t c = UrlExtension[ --FillExtension ];
                    if( c < 127 && iswalnum( c ) )
                        *into++ = c; // file-safe, us-ascii / ALNUM only
                    if( FillExtension == 3 ) // starting point as init
                        break;
                    // If I got no extension, well, that's life.
                }
            }
        }
        else
        {
            // When no URL extension, use .html for saving unknown files.
            // This arises often for folder paths that serve HTML pages.
            *into++ = 'h'; // 4-char version serving BINARY case
            *into++ = 't';
            *into++ = 'm';
            *into++ = 'l';
        }
    }
    else
    {
        *into++ = 'h'; // 3-char version serving Save Folder.
        *into++ = 't';
        *into++ = 'm'; // Note: Save File does not call here.
    }

    *into = NULL;

    #if DO_DEBUG_ORDINALS
        ; SpewTwo( L"FullFileNameForUrl", FullFileName );
        ; SpewValue( L"SaveAsAQrp", SaveAsAQrp );
        ; SpewValue( L"FromEngineNo", FromEngineNo );
    #endif

    return FullFileName;
}

wchar_t * PointPastDomainPrefix( wchar_t * szDomain, wchar_t * * pPastDomain )
{
    // I am passed a null terminate string to a full domain name,
    // e.g., "search.some-engine.com.au"
    // Return a pointer to the important part of the domain name,
    // after any host-selection prefix, like after "search." above.
    // If total foo data, return the original pointer passed in.
    // If the second argument is non-NULL, return therein a pointer
    // to where caller can put a NULL to drop off the ".com.au" part.

    wchar_t * retn = szDomain;

    size_t len = wcslen( szDomain );
    wchar_t * past = szDomain + len;

    int FoundCountryCode = 0;

    // look for a final 2-char, valid, country code suffix.
    // validation of country code comes later
    if( len > 3 )
    {
        if( past[ -3 ] == '.'
        && iswalpha( past[ -2 ] )
        && iswalpha( past[ -1 ] ) )
        {
            FoundCountryCode = 1;
            past -= 3;
            len -= 3;
        }
    }

    // Some countries have a 2-char domain type, eg, domain.co.za
    // Later, validate these codes and which countries use them.
    if( FoundCountryCode
    && len > 3 )
    {
        if( past[ -3 ] == '.'
        && iswalpha( past[ -2 ] )
        && iswalpha( past[ -1 ] ) )
        {
            FoundCountryCode = 1;
            past -= 3;
            len -= 3;
        }
    }

    // Now, strip any final 3 or 4 char suffix.
    // I must validate to an explicit list of valid types.
    // Otherwise would strip, thing.msn.ca, thing.ilse.nl.

    if( len > 4
    && past[ -4 ] == '.'
    && iswalpha( past[ -3 ] )
    && iswalpha( past[ -2 ] )
    && iswalpha( past[ -1 ] )
    && Matches3CharDomainType( past - 3 ) )
    {
        past -= 4;
        len -= 4;
    }
    else if( len > 5
    && past[ -5 ] == '.'
    && iswalpha( past[ -4 ] )
    && iswalpha( past[ -3 ] )
    && iswalpha( past[ -2 ] )
    && iswalpha( past[ -1 ] )
    && Matches4CharDomainType( past - 4 ) )
    {
        past -= 5;
        len -= 5;
    }

    if( pPastDomain != NULL )
        *pPastDomain = past; // tell caller where to terminate string.

    if( len > 0 )
    {
        // having identified common country and domain type endings,
        // identify any server name and dot, left of the main domain.
        wchar_t * scan = past;
        for( ;; )
        {
            if( --scan == szDomain )
                break;
            if( *scan == '.' ) // e.g., at the dot in "search.msn"
            {
                retn = scan + 1;
                break;
            }
        }
    }

    return retn;
}

int Matches3CharDomainType( wchar_t * pSuffix )
{
    wchar_t wk[4];
    wk[0] = pSuffix[0] | ' '; // lowercase
    wk[1] = pSuffix[1] | ' ';
    wk[2] = pSuffix[2] | ' ';
    wk[3] = NULL;

    size_t index = CSol3CharLcDomains.Find( wk );
    #if DO_DEBUG_ADDFIND
        if( index == 1 )
            { Spew( L"AddFind 1 at cfio 2547" ); }
    #endif
    return index; // as a boolean
}

int Matches4CharDomainType( wchar_t * pSuffix )
{
    wchar_t wk[5];
    wk[0] = pSuffix[0] | ' '; // lowercase
    wk[1] = pSuffix[1] | ' ';
    wk[2] = pSuffix[2] | ' ';
    wk[3] = pSuffix[3] | ' ';
    wk[4] = NULL;

    size_t index = CSol4CharLcDomains.Find( wk );
    #if DO_DEBUG_ADDFIND
        if( index == 1 )
            { Spew( L"AddFind 1 at cfio 2564" ); }
    #endif
    return index; // as a boolean
}


