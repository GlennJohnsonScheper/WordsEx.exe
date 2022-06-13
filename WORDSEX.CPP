// wordsex.cpp : Defines the entry point for the application.

#ifdef _WIN32_WCE
    // This compiles under MicroSoft Embedded Visual C++ 3.0.
#else
    // This compiles under MicroSoft Visual Studio 6.0 C++.
#endif


    /*....

    =============

    Big surprise - I was redoing vocabulary, so ^D, and I re-sized
    the window until it's vertical size was 0, on the vista, and I
    got a program crash!

    =============

    Still need corporation/brand names.

    I have a new problem that common words that got into the PROPER
    names list, kept them from being added to the per-language word
    lists --- SO DO NOT LOSE THE "AMEN" folder on vista cpu!!!

    SO I will have to re-import AMEN, losing my manual improvements!

    X. beat proper against all during start-up to produce a list of
    multiples, which I might then remove from Proper name lists.
    -- oh, but my word lists are missing very common words.

    so, plan:


    0. ZERO: Start with sucking all lists in, without ridding any.
    |
    1. Then adjust routines to work with the inconsistencies:
    |
    1a. rather than having any N-way conflict of languages turn a
    word to MUL and be useless, have it pick the larger count; and
    maybe keep a sum of, for fraction of, whole n-lang counts for
    a final pass to decide if it is multiple or owned by one lang.
    |
    1a. Similarly, let Proper names compete somehow against other
    counts of word. (Perhaps as if it were just 1 of N langs?)
    |
    |
    |


    =============

    I guess this should go into the yom kippur text, in cuse.cpp:
    My retro canal post:
    http://groups.yahoo.com/group/His-Sheep/message/20499

    ======

    There is one occasional crash I would like to debug!
    1. add parameter checking to some major routines.
    2. Change Find into MustFind / MayFind, which check returns.
    3. then rid the index checks; rid many debug statement blocks.
    4. check all sprintf %s usages, that passed a pointer.
    5. check auto arrays for being exceeded.
    6. add if( !NULL ) around some blocks after calls that can return null.

    ======

    Languages, names are nearly done!
    re-get english, farsi.

    ======

    I want to collect foreign search engines.
    Compile, try, all those found forms....

    ======

    Fixed? As easy as rid a BYTE typecast?

    Clicking on a foreign word ( e.g., in Wordlist ) GOT that word
    as far as the start-of-thread, but the work from that point
    on, to the stemming was AFU: ( this a in sa is foreign: )

Find Words: sa

Thread started.

Search tokens, without stemming:
s - 0 matches

Search tokens, matches after stemming:


There were no token matches in any texts held in memory.


Thread ended.

    ======

    idea: When saving a binary file, also save a .txt file showing original URL.

    ======

    // A purely numeric domain gets filed as an UNDERSCORE filename,
    // in an underscore folder, wrongly I presume:
    // e.g., url: http://195.5.28.97/

    WS evacuates this URL ( and all pure-numeric domains ), putting nothing before "_" for filename:
http://www.747.co.il/default.asp

    Save All has put some _ query filenames into non- _ folders. Why?
    Oh, AHA: perhaps they had an empty domain part after the parse


    ======

    I must add LATIN Language Group
    to avoid scoring these highly:

Nec enim nos misimus Christum sed Deus, cujus nomine Christus ad nos venit,
foedus nobiscum panxit, ejusque promissiones ratas fore spopondit et in se
recepti; ideoque nec sponsor simpliciter, sed foederis sponsor nominatur;
spopondit autem Christus pro foederis divini veritate, non tantum quatenus id
firmum ratumque fore verbis perpetuo testatus est; sed etiam quatenus muneris
sui fidem, maximis rerum ipsarum comprobavit documentis, cum perfecta vitae
innocentia et sanctitte, cum divinis plane quae patravit, operibus; cum mortis
adeo truculentae, quam pro doctrinae suae veritate subiit, perpessione".


    ======

    IDEA: Cause R/L-Click to go right to search without seeing dialog.

    Idea: Let import directories postpone import of UN until after others.

    Many small things... Other things I cannot remember!
    - propose filenames for save file.
    - whenever save binary, add a .txt file showing URL
    - domain with a numeric part made filename domain part _
    - I put off doing some real clean ups of domain recognition


    Spew should make 4 chars hex here, huh? or U+4444?
    // Spew needs to come up to speed on Unicode!:
    // MUL: \777\777\777\777\777\777\777,     53, ...
    // Meanwhile, I should format the same 4-octals
    // as I use in the word dumper that makes lists.
    // e.g., Archive for the \777Latent Semantic Indexing\777 Category


    ===========

    IDEA! -- I want a new option to display word hits "in page order".
    That same concept or code segment could also automatically do a
    KWIC format list of word hits right into the Internet search log.

    IDEA! The inability to follow one specific link out of a page,
    like any normal browser, is a real hamper. I SHould have a
    way to turn on highlighting, and when on, to follow links.
    I could have an inserted character that represents a link,
    perhaps a happy face, always present, but that begs the
    question of how to make a single character work for links
    that have varing extents. Perhaps one mark atop, another
    at the end? I could simply underline them, perhaps draw
    the lines myself, slightly short or with arrowheads, so
    you can distinguish adjacent anchors.

    13, Bakers's dozen: With links marked, I'd also want to
    display the URL, which suggests having a status line in
    the boundary, perhaps up in the toolbar, perhaps directly
    drawn by me during hover. Oh! VOILA! If I provide that,
    I would not have to highlight them in the text! Then I
    could have a hot-key for fetching, fuck the mouse.

    =============

    Two bad-Javascript parses to study ( much JS came into text )
    ( in files js1.htm, js2.htm ):
      -- http://www.thefreedictionary.com/lookup.htm
      Free Website Content - Webmaster tools
      -- http://www.bluebit.gr/downloads.htm
      Bluebit Software - Free Downloads


    ==========


    Good IDEA, not done yet: saved .HTM pages should have <pre> tag,
    say, right after the annotations, so they appear the same in IE.
    How would that affect my own re-read of the file?

    When saving a QRP, after the ordinal, add a page number field. ?

    Can I get RTF documents in as text?
    I need the new domain work in place.
    I need the saving of a text file saying whence a resource.

    I do see this amongs the AllUrls when reload only s.e. qrps:
        "Clear text URL"
    So is my provision adequate?
    I would like to use the sorted domain code parts.

    I want to add relevance and page Q scoring back into engines,
    also the durations, and use that to annotate the QRP -- which
    is another reason to split the header annotations off from the
    main body of text, so that the data can be modified later, and
    formatted j.i.t. Especially then, the score chould be recomputed
    on the burgeoning corpus JIT for display or output ranking.

    For save binary, I should also use the http header description
    of type, and have a table of the major common filename endings.

    I can totally rid the NOT scheme, as I can make BAD rules
    when needed based on domain and path. I should have some
    marker for when path is empty... Oh, but then the quoted
    string of it would provide the ballast for a comparison.

    in CPag...
        // Do a different header annotation for QRP's...
    In query progress log,
    Make the annotation for QRP page itself
    out of/instead of the next line about hits, time.

    ==========

    What do I mean by: fifo these ( tld and cd are not used yet )
    domainname
    C SolTopLevelDomains
    C SolCountryDomains
    C SolBinExtensions
    |
    I think I meant:
    Domains need more work:
        Make one common routine,
        Accept any ptr w count or null.
        Return ptr to full domain name,
        Return ptr past prefix,
        Return ptr past authority w/o ending,
        Return ptr past full domain name,
    |
    Related:
    I should also recognize Cleartext URLs among facts.
    // Perhaps another routine already recognizes Cleartext URLs?
    // Indeed, I already call CPag::ClearTextTokenResemblesUrl.
    // However, that is during the scan as text. I need to get at it.

    ==========

http://www.waste.org/pynchon-l/

        =====

    <a href="http://www.pocketpcwire.com/">
PocketPCWire
    <a href="http://www.windowsmobiletoday.com/">
WindowsMobileToday

    ========

    Check: PPC text exceeds fields?

    ==========

    Some of these things are GONE / need DELETION / RESTORATION or TUNEUP:

    - if( RejectableQueryUrl( szUrl, pMalHit ) )
    - MarkAsRejectedUrl( nSourceIndex );
    - size_t index = pSolHits->AddUniqueRank( 100000 );

    This is how I backtrack the list of which SE found each page,
    with a JIT new integer-list member in the fetched paper.
    BUT - It only works if I immediately follow hits to create papers.
    pOnePaper->pIntFromEngine = new CInt( );
    //was... pOnePaper->pIntFromEngine->Add( pQuery->nQueryOrdinal );

    ==========

    Test: There were no suitable tokens ( 3 to 59 alpha chars ).

    ==========

    idea: remove some of the newlines from usage text so it wraps nicely.

    ==========

    URLs appear in AllUrls w/o referers:
    |
    E.g., These were in Group 5, which are Redirection URLs
    ^http://xslt.alexa.com/cgi-bin/search_form?submit=&term=cheney
    ^http://a9.com/?submit=&q=cheney
    |
    E.g., These were at the end of Group 7, unfetched:
    http://www.lib.ru/DIC/OZHEGOW/
    http://www.phdcomics.com/newspapers/newspapers.html
    http://del.icio.us/post?url=http%3A%2F%2Fwww.phdcomics.com%2Fcomics%2Farchive.php%3Fcomicid%3D180&title=PHD+Comic%3A+Squished+it+with+a+ten-ton+hammer+I+fixed+it.++Bugs.++Code.
    http://slashdot.org/bookmark.pl?url=http%3A%2F%2Fwww.phdcomics.com%2Fcomics%2Farchive.php%3Fcomicid%3D180&title=PHD+Comic%3A+Squished+it+with+a+ten-ton+hammer+I+fixed+it.++Bugs.++Code.
    http://digg.com/submit?phase=2&url=http%3A%2F%2Fwww.phdcomics.com%2Fcomics%2Farchive.php%3Fcomicid%3D180&title=PHD+Comic%3A+Squished+it+with+a+ten-ton+hammer+I+fixed+it.++Bugs.++Code.&bodytext=Funny+comic+about+academia+titled+%27Squished+it+with+a+ten-ton+hammer+I+fixed+it.++Bugs.++Code.%27&topic=general_sciences
    http://reddit.com/submit?url=http%3A%2F%2Fwww.phdcomics.com%2Fcomics%2Farchive.php%3Fcomicid%3D180&title=PHD+Comic%3A+Squished+it+with+a+ten-ton+hammer+I+fixed+it.++Bugs.++Code.
    http://www.phdcomics.com/comics/archive.php?comicid=185
    http://www.phdcomics.com/comics/archive.php?comicid=190
    |
    To fix: formulate an anchor text for each instance of AddKey,
    or its callers, like ClaimUrl... For example, one fabricated
    anchor text might say: "Search Engine 101 query keyword."

    ==========

    I might make these do utf-8 conversion ( or show not needed ):
    void CAsb::AddnWide( wchar_t * szNew, size_t nNewLen )
    void CWsb::AddnByte( BYTE * szNew, size_t nNewLen )

    ==========

    file ( volt.htm ) was "Characterized and refused". Why?
    I notice that it is a search engine in my list:
    x:\dod\_\_voltairenet_spip_531885.htm
    x:\dod\_\_voltairenet_spip_531885.htm
    --File characterized and refused.
      -- Add Directory, recursive: x:\dod

    ==========

    idea: When click and fetch non-text url, suggest filename to save as dialog.
    |
    yet to do:    suggest a filename per url in calling:
    I did that; but notice this list and routine, for...?
    wchar_t * BinaryFileExtensions[] = {
    void SetupBinaryNames( )
    |
    did CPag::ClearTextTokenResemblesUrl( wchar_t * pToken, size_t nChars )
    // I rid the .com, etc rule here. Need to re-add detailed rules.
    // this will be enuf to get CHTM to list absolute exported URLs.
    |
    // chtm 4567 wip - recognize urls, add links.
    adapt existing routine by passing length: 1 call now from csee:
    if( Pag.ClearTextTokenResemblesUrl( pMalCopySelText ) )

    ==========

    add new:
        PointPastDomainPrefix( UrlComponents.lpszHostName ) );

    ==========

    ideas: As I can now SAVE ALL URLS, and have URL cross-link information;
        1. Make same operation add a new view.
        2. Add a command to convert a query/fetch log to such a view.

    ==========

    ideas: Now that I save each anchor text as encountered ( in CLIN.cpp );
        1. Add diligence level to fetch URLs matching a token in anchor text.
        2. Rank search engines, using the web pages' scores as a metric.
        3. I could postpone the fetch of hit URLs until some heuristics run.

    ==========

    tiny fix: since aborted/restarted add cache was fixed:
        a. let phase 1 annotate URLs already in memory, or
        b. let phase 1 include them; phase 2 annotate them.

    ==================

    Glean and clean these up: may be easy fixes:

    Idea: If I drag a Phrase in Phrase result, annotate with original url.

    minor Fix: Arrow up/dn must fix the scroll offset when reach final line.

    Idea: rid previous url/title when doing control+copy from same page.
    Viz., in CSee, somewhere near lines:
    if( ! append )
        WsbClipboardAccumulator.Reset( ); // Oh, that was easy.
    Probably keep a 2nd, baretext, wsb, then join annotation to that.

    I did fetch one web page, get all links, and nontext;
    and I specified a different folder. I re-did the same.
    -Fix 1: Directory chooser is one dialog where ARROW DOWN is
    not effective to pick the name. But other dialogs work.
    -Fix 2: After 2nd fetch of www.tv.com ran, view I was shown
    was totally empty. The log of fetch was at BACK one.

    Did I do this? What is my status on it, and in annotations?
    Idea: If a file contains only < 0x7f, set encoding to us-ascii.
    BUT NOT, If a file contains only < 0xff, set encoding to utf-8.

    rid method prefix from word lists:
    "  mailto:his-sheep-fullfeatured  mailto:his-sheep-digest

    IDEA: allow * on Word search ( before/after/mid? )

    IDEA: Do phrase/near somehow on Word Search, or boost rank way up.

    Fix the phrase recognition to make strictly ascending boundaries.
    Then re-enable the self-checks in IDX class.

    Fix: Do not stem if token has non-USACII all alnum letters.
    No, rather, concept good. use my new international ISVOWEL.
    Test: if I don't stem a word, will it still be processed?
    Fix: rules for no suitable tokens ( 3 to 59 alpha chars ).
    Fix: mouse click to seek UCS alnums when invoke kwic/query.
    Test/Fix: Vocabulary lists must accept full unicode letter range.
    Test/Fix: Adjust vocabulary lists for alphanumeric, hypenated, etc.
    Stuff like 0x0000 should keep the numbers as part of tokens:
    |
    Also, the 3-59 alpha rule needed internationalization.
    It only affects find, not query, so clone that, adjust for stemmer.

    Change the txt/html saver into 2 choices, so have room to amplify.

    fix scan to avoid failure on first pathpart of UNC names, which IS root.
    UNC: -- Add Directory: \\File_Server\Drive_P\users\glenn

    Minor Idea: improve DT/DD indenting: Per comments already in:
    void CHtm::ChangeTextIndentation( )
    void CHtm::RestoreTextIndentation( )

    // Later, I could improve this in CFio::MakeFoldersPaths:
    // I expect failure on first pathpart of UNC names, which IS root.

    =========
    =========
    =========


    Specific URLs to study, except for form/javascript/rss/xml work:
    |
    Saved in directory X:\xec\vc\ws\pages
    |
    - wiki.htm       -- great page to work on UNICODE word lists, search etc.
    |
    - listatop.htm   -- great page to study rejection of non-sentence lists
    |
    No words. why? Big in IE:
    Fix: make the result text explain, e.g., ? contained JavaScript, or....
    Add explanatory comments to such pages, like JS used, or to try in IE:
    http://www.herreros.com.ar/melanco/anatomy.htm
    x:\xec\vc\ws\pages\anatomy.txt
    x:\xec\vc\ws\pages\mel.htm
    |
    why is this almost nothing in WS? JavaScript?
    -- http://www.rumormillnews.com/cgi-bin/forum.cgi?read=102691
    |
    why is this nothing in WS? JavaScript?
    pages\surprise.htm
    -- http://www.rumormillnews.com/HARRY_MARTIN_OCTOBER_SURPRISE.htm
    |
    WCS: study this url -- lots of undefs that s/b apostraphe, etc:
    pages\quotes.htm
    -- http://www.madcowprod.com/05252006.html
    |
    There is some foreign left-and-right quoting. ( Do I care any more? )
    see:    Présidentielle, «Complément d'enquête»
    http://www.n-k-m.com/breves.php
    |
    Is there something I should do with <H?> tags, like set them apart?
    x:\xec\vc\ws\pages\headings.txt
    |
    Idea: Recognize and remove various soft-hyphens;
    Also, I did not remove the email > and >> type quoting, see:
    | &gt;&gt;                   the dominant assumptions of the age. Although he is
    | &gt;&gt;                   a classicist by training who came late to the =20
    | &gt;&gt; study of
    | &gt;&gt;                   Freud and later to mysticism, he has already =20
    | &gt;&gt; created
    | &gt;&gt;                   a revolution in psychological theory."
    http://waste.org/mail/?list=pynchon-l&month=0703&msg=116639
    x:\xec\vc\ws\pages\softhyph.txt
    |
    This url kept many newlines, but <PRE> work looks good overall:
    | Area
    | Shop at Amazon.com]
    |  
      -- http://www.bbsinc.com/iso8859.html
    |
    ( OLD? ): www.tv.com had script-ish garbage right after URL:
    Area
    ' );
    [Click to learn more...]
    ' ); date_ob.setTime( date_ob.getTime( )+43200000 ); dc.cookie='he=llo; path=/;
    expires='+ date_ob.toGMTString( );} // -->
      -- http://www.tv.com
    |
    at    Select Search
    -- http://www.bartleby.com/100/249.html
    |
    ' ); // -->
    ' ); //-->
    [Click Here]
      -- http://www.everypoet.com/archive/poetry/William_Wordsworth/william_wordsworth_contents.htm
    |
    This HUGE web page comes up empty, no words ( but has words in html ):
     -- http://www.pantone.com/pages/pantone/Pantone.aspx?pg=19567&ca=12
    |
    retest mouse on this after rectifying unicode, boundaries, etc:
    But also, click vs. drag ( or lowercasing ) lost vs got the final u:
    bân-lâm-g
    |
    Bân-lâm-gú
      -- http://wikipedia.org/
    |
    It was a simple 0xFA byte:
    000000:  42 E2 6E 2D 6C E2 6D 2D 67 FA 0D 0A 0D 0A 0D 0A   B.n-l.m-g.......
    |
    see file wiki.htm
    005600:  61 6E 22 3E 42 C3 A2 6E 2D 6C C3 A2 6D 2D 67 C3   an">B..n-l..m-g.
    005610:  BA 3C 2F 61 3E 26 23 31 36 30 3B E2 80 A2 0D 0A   .</a>&#160;.....
    |
    I don't know why these are here:
    http://www.everypoet.com/archive/poetry/William_Wordsworth/william_wordsworth_contents.htm
    http://www.domenicoquaranta.net/peam2006.html
    http://www.bbsinc.com/iso8859.html
    http://wikipedia.org/
    http://www.n-k-m.com/breves.php
    http://dictionary.reference.com/browse/vortex
    http://images.google.com/images
    http://waste.org/mail/?list=pynchon-l&month=0703&msg=116639


    =========
    =========
    =========


    Whole new concepts to think about doing:



    Another synopsisizing Idea:
    Key Phrases - Capitalized Phrases ( CAPs ):
    Anita Hill, Pierre Bourdieu, Richard Delgado, Rodney King



    Concept: Add, interests.

Thus, there is the
problem of determining what information is relevant to the user
and how this decision can be taken by a supporting system.
Parametric and qualitative descriptors of user's interest must be
generated.
  -- http://www.softlab.ntua.gr/facilities/public/AD/Text%20Categorization/ATCbibliography_bib.htm
  Untitled

Automatic
classification is traditionally performed by extracting the
information for representing a document ( ``indexing'' ) from the
document itself. The paper describes the novel technique of
categorization by context, which instead extracts useful
information for classifying a document from the context where a
URL referring to it appears.
  -- http://www.softlab.ntua.gr/facilities/public/AD/Text%20Categorization/ATCbibliography_bib.htm
  Untitled


However, while TC deals with documents represented as vectors in
a space of terms, we formulate the task of term categorization as
one in which terms are ( dually ) represented as vectors in a space
of documents, and in which terms ( instead of documents ) are
labelled with domains.},
  -- http://www.softlab.ntua.gr/facilities/public/AD/Text%20Categorization/ATCbibliography_bib.htm
  Untitled



    -- BIG NEW Concept:

Add a new layer of analysis, "Fondle Page" which will use the
new UNICODE sentence terminator rules, and double newlines,
and occurence of links, perhaps also vocabulary, to divide
each page into diverse CHUNKs, grouping all signage together,
maybe dividing old BLOCKs up.
After that, we might do clustering, or record inputs to clustering,
for each chunk. It might record sentence delimiters rather than
sentence extents ( per unicode advice ).
At the integration of these kinds of observations, include the
link counts and domains.


    -- Big concept: like my idea of the auto-correlation of words

A word vector is a list of numbers ( called coordinates ) which encodes
information about how the word is distributed over the corpus. Many experiments
have demonstrated that words with similar word vectors often have similar or
closely related meanings: the Infomap WORDSPACE can therefore be used to model
similarities between different words by automatically comparing their behavior
in text.


    -- Concepts:

1. and indexed it by paragraph rather than as a single document.
2. the Velocity Clustering Engine is used to cluster the search results into
related topic areas.

    --Concepts:
WS does this: TERM FREQUENCY ( tij ): The number of times term j appears in document i.
New: INVERSE DOCUMENT FREQUENCY ( d/dj ): Number of docs / Number of docs containing term j.
semantic content ( CONT ) in p is the complement of the a priori probability of p: CONT( p ) = 1 - P( p )
informativeness ( INF ) of p, is the reciprocal of P( p ) in bits; satisfifies addability conditionalization.

    -- Concept!
Related documents from co-citation:
13:   Building a large annotated corpus of english: The penn treebank - Marcus, Santorini et al. - 1993
9:   developmental model of word recognition and naming ( context ) - Seidenberg, McClelland - 1989
8:   A Stochastic Parts Program and Noun Phrase Parser for Unrestricted Text ( context ) - Church - 1988


    -- Great Idea:
IRP refers to the inverse relation between the probability of p — which may
range over sentences of a given language ( as in Bar-Hillel and Carnap ) or
events, situations or possible worlds ( as in Dretske ) — and the amount of
semantic information carried by p ( recall that Poe's raven, as a unary source
provides no information because its answers are entirely predictable ). It states
that information goes hand in hand with unpredictability. Popper [1935] is often
credited as the first philosopher to have advocated IRP explicitly.
  -- http://www.science.uva.nl/~seop/archives/spr2006/entries/information-semantic/
  Semantic Conceptions of Information ( Stanford Encyclopedia of Philosophy/Spring 2006 Edition )



    -- Great Idea:
Add, Clusters.

    -- Good Idea:
We estimate a document s position on the expertise dimension
as a simple function of the frequency and length of the document s words

    -- Good Idea:
...holds true surprisingly for very short text passages:
if one makes a list of the top 135 most common words from modern
American English usage, and takes any random word from an
American English sentence, there is a greater than 50 chance
that this word will be contained in the list

    -- Good Idea:
familiar symbols and sequences will reflect a falling information rate
unfamilar symbols and symbol arrangements constitute an increase in the rate
a production-based complexity measure

    -- Good idea:
English, a consonant may be analyzed as acting simultaneously
as the coda of one syllable
and the onset of the following syllable, a phenomenon known as
ambisyllabicity.


    -- Idea:
language has the important property that it organizes elements
into recursive structures

    -- Idea
torus is just mod N in x and y!

    -- Idea
A dictionary determines the interesting subset we call information

    -- Idea
Long runs of consistent data are still needed

    -- Idea
    Build down
    from pages,
    to signage/text blocks,
    to sentences,
    to words,
    to CV runs.
    |
    -- Build up from CV runs to paragraphs to blocks to documents.
    -- Can I apply principle of consonantal shifts in pronunciation?
    -- which suggests to do a CV and VC run cross correlation.
    -- Consonant runs have runs; distinguish phonems from digraphs.
    -- perhaps a HUFFMAN tree to group / subgroup consonant runs;
    -- retabulate the subgroups in CCV etc.
    -- Add special FIRST/LAST markers
    -- Bayesian nets on various length letter runs.


    Important concept: Normalization:
    |
    To remove this dependency on the range spanned by each metric,
    it is important to first standardize the values. This means that
    each metric, when compared over the full set of objects will have
    a mean of 0.0 and a variance ( or standard deviation ) of 1.0.
    For each metric, the following steps should be taken.
    |
    Sum the values of the metric over all objects
    and divide the sum by the number of objects.
    |
    Subtract this average value from the metric in all objects.
    |
    Sum the square of these new values over all objects,
    divide the sum by the total number of objects,
    and take its square-root. This is the standard
    deviation of the new values.
    |
    Divide the metric by the standard deviation in each object.
    This process standardizes each metric describing the objects.
      -- http://fconyx.ncifcrf.gov/~lukeb/clusdis.html
      Clustering Distances

    DO THIS!
    in \i\t\boundary.txt
    This annex describes guidelines for determining default boundaries between
    certain significant text elements: grapheme clusters ( “user-perceived
    characters” ), words, and sentences. The process of boundary determination
    is also called segmentation.
    |
    common sense + ...
    |
    MidLetter Any of the following:
    U+0027 ( ' ) APOSTROPHE
    U+00B7 ( · ) MIDDLE DOT
    U+05F4 ( ? ) HEBREW PUNCTUATION GERSHAYIM
    U+2019 ( ’ ) RIGHT SINGLE QUOTATION MARK ( curly apostrophe )
    U+2027 ( ? ) HYPHENATION POINT
    U+003A ( : ) COLON ( used in Swedish )
    |
    Do not break within sequences, such as “3.2” or “3,456.789”.
    |
    To allow acronyms like “U.S.A.”, a tailoring may include
    U+002E FULL STOP in ExtendNumLet.
    |
    Finally, SENTENCES!---
    |
    ATerm U+002E ( . ) F ULL STOP
    |
    Close General_Category = Open_Punctuation ( Po ),
    or General_Category = Close_Punctuation ( Pe ),
    or Linebreak = Quotation ( QU )
    and not U+05F3 ( ? ) HEBREW PUNCTUATION GERESH
    and ATerm = false and STerm = false
    |
    OLetter - ignore it
    |
    STerm STerm = true
    -- huh? ( Oh, I think: a prior-sentence was just terminated )
    |
    Do not break after ambiguous terminators like period,
    if they are immediately followed by a number or lowercase letter,
    if they are between uppercase letters, or if the first following
    letter ( optionally after certain punctuation ) is lowercase.
    For example, a period may be an abbreviation or numeric period,
    and thus may not mark the end of a sentence.
    |
    SB6. ATerm × Numeric
    |
    SB7. Upper ATerm × Upper
    |
    SB8. ATerm Close* Sp* × ( ¬( OLetter | Upper | Lower | Sep ) )* Lower
    |
    SB8a. ( STerm | ATerm ) Close* Sp* × ( STerm | ATerm )
    |
    Break after sentence terminators, but include
    closing punctuation,
    trailing spaces,
    and a paragraph separator ( if present ). [See note below.]
    |
    SB9. ( STerm | ATerm ) Close* × ( Close | Sp | Sep )
    |
    SB10. ( STerm | ATerm ) Close* Sp* × ( Sp | Sep )
    |
    SB11. ( STerm | ATerm ) Close* Sp* ÷
    |
    Otherwise, do not break.
    |
    The context required for determining a break position should never
    extend beyond the surrounding break positions on either side.
    Implementations are recommended to add Sterm and ATerm to the
    righthand side of Rule SB8, which then becomes:
    |
    SB8* ATerm Close* Sp* × ( ¬( OLetter | Upper | Lower | Sep | STerm | ATerm ) )* Lower
    |
    Rules SB6-8 are designed to forbid breaks within strings like
      c.d
      3.4
      U.S.
      ... the resp. leaders are ...
      ... etc. )’ ‘( the ...
    |
    They permit breaks in strings like
      She said “See spot run.” John shook his head. ...
    |
    They cannot detect cases like “...Mr. Jones...”;
    more sophisticated tailoring would be required to detect such cases.
    |
      -- http://www.unicode.org/reports/tr29/
      UAX #29: Text Boundaries



    This ( a-z topic listing ) is NOT a sentence:
    |
    ...
    87 DNA sequence
      21 DNA sequences
      2 Documentation
      3 Dodge-type continuous sampling plan
      4 Dollar unit sampling
      3 Domain estimation
      2 Domain estimator
      42 Domain
    ...


    if I may explain a very simple idea.
    In mathematics, this is to say, 'relation + alternative'
    = m + a = 3+2 = 4+1. The sum of relation plus alternative,
    or in other words mind plus art, equals the sum of
    expression plus life. So this one same idea has the several aspects,
    that it is the domain of a term, the extension of a class,
    and the equivalence and plurality of its referents or members.
      -- http://lists.topica.com/lists/basics/read/message.html?sort=d&mid=1710350271
      Topica Email List Directory


    The Literary Machine consists of a
    1 ) word-dictionary,
    2 ) a database of concepts built by combination of words
    ( not specifically defined in what way they interact )
    3 ) text paragraphs "marked" or "indexed" by the concepts.
      -- http://www.sommestad.com/text/misc/Yvon_Provencal.htm
      Yvon Provençal - Gunnar Sommestad. Letters 1999-03 -


    We also consider the network connectivity and the
    degrees of its nodes, finding a non-trivial scale-free behavior
      -- http://ipnweb.in2p3.fr/~lptms/membres/martino/statphys.html
      Olivier Martin: statistical physics


    The local minima ( inherent structures ) of a system and their
    associated transition links give rise to a network.
      -- http://ipnweb.in2p3.fr/~lptms/membres/martino/statphys.html
      Olivier Martin: statistical physics

    http://sfs.nphil.uni-tuebingen.de/~abney/Papers.html#96i,
      -- http://www.eamt.org/archive/ljubljana/Planas.htm
      Extending Translation Memories


    BIG IDEA, on chunking used in a translation thing:
    |
In a TM system, the text is cut down into text units. Those text units will
become the source part sTU of the translation units TU. The target part tTU
being the translation of sTU. The usual way to do this is to separate the text
in segments according to predefined non linguistic rules. Upper case letters and
marks play a major role in this segmentation, and this results in getting
roughly sentences as text units ( that we call segments because of the
"roughly" ). When trying to find a corresponding TU in the memory, the input
current segment is compared to sTU which are also segments. The redundancy is
then generally based on segments ( sentences ).
  -- http://www.eamt.org/archive/ljubljana/Planas.htm
  Extending Translation Memories
    |
We have proposed in [Planas 1999] to clarify the "fuzzyness" using a proper
defined similarity. This similarity is a multi-layered similarity that measures
the proportion of items matched at each layer of the TELA structure. For a
textual redundancy for example, if surface words, basic words and parts of
speech are the three layers chosen for measuring the similarity, the following
sentences have a similarity of ( 4/6, 5/6, 1 ) because four surface words out of
six are matching ( except for "Click" and Color" ), five basic words ( except
"color" ), and all part of speech do match.
  -- http://www.eamt.org/archive/ljubljana/Planas.htm
  Extending Translation Memories
    |
The notion of chunk has been introduced by Steven Abney [Abney 1991, 1996]. A
current European Project called "Sparkle" [Sparkle 2000] deals with it, and some
psychological evidences [Juola 1995] as well as research in speech repair
[Nakatani & Hirschberg 1993] seems to give it some scientific foundation.
  -- http://www.eamt.org/archive/ljubljana/Planas.htm
  Extending Translation Memories
    |
    An algorithm!!!...
    There are two important notions to use for chunking a text:
    Separators: separators begin or stop a chunk
    Friend categories: friend categories cluster together to form chunks.
      -- http://www.eamt.org/archive/ljubljana/Planas.htm
      Extending Translation Memories
    |
    Here are the list of separators and friend categories we propose
    for the English language:
        Separators
    Articles: a. the
    Other function words: at, in, while, when, and,
        Friend categories
    Separator word category: all separators
    Verb category: verbs and adverbs
    Noun category: articles, nouns, adjectives and adverbs
    |
As there is an ambiguity for the binding of the adverb, we let the verb get the
priority against the adjective in case of duality. Separators gather together to
for a special case of friend category, stop the left side chunk and are part of
the right side chunk. Applying these simple rules, one should be able to cut any
text into chunks. The previous segmented text illustrates these rules.
  -- http://www.eamt.org/archive/ljubljana/Planas.htm
  Extending Translation Memories
    |
    What are linked lattices?
    Surf: linked lattices?
    |
    A first problem that this observation arises is how to represent
    all this data in a convenient way in Translation Memories.
    We have proposed to use a structure of linked lattices
    that we call TELA in [Planas 1998] and [Planas 1999].
      -- http://www.eamt.org/archive/ljubljana/Planas.htm
      Extending Translation Memories


    Concept: per-word lists of neighboring words could be compared.
    surf: distributional similarity
    |
    The idea is simple and intuitive: two objects can be considered similar
    if they tend to appear in the same contexts ( for instance, one can infer
    that coffee and espresso are similar because they both occur after the
    words drink and sip ).
    But despite its simplicity, the notion of distributional similarity
    forms the basis of a very powerful principle for bootstrapping the
    knowledge acquisition process:
      -- http://www.cs.cornell.edu/home/llee/research.html
      Lillian Lee: Research Summary

    list sugests a concept: building a relational ontology on the fly
    |
    Category:Drama webcomics
    Category:Fantasy webcomics
    Category:Horror webcomics
    Category:Infinite canvas webcomics


    REMEMBER FACETS?
    |
    We propose a theory of genres as bundles of facets,
    which correlate with various surface cues,
    and argue that genre detection based on surface cues
    is as successful as detection based on deeper structural properties.
      -- http://www.revue-texto.net/Inedits/Beauvisage/Etude-polar_notes.html
      Morphosyntaxe et étude des genres - le roman policier

    Stylistic items can be found on any level of linguistic abstraction:
    lexical, syntactic, or textual;
    each is of little importance in itself,
    but taken together their variation indicates systematic differences
      -- http://www.revue-texto.net/Inedits/Beauvisage/Etude-polar_notes.html
      Morphosyntaxe et étude des genres - le roman policier

    surf: discriminant analysis
    |
    An automatic multilevel thresholding method,
    based on discriminant analysis,
    is utilized to recursively segment a specified block region
    into several layered image sub-blocks.
      -- http://ieeexplore.ieee.org/Xplore/login.jsp?url=/iel5/9622/30424/01400809.pdf?arnumber=1400809
      IEEE Xplore - Login



    THIS IS NOT A SENTENCE:
    |
    DOI: 10.1360/jos180538 http://www.jos.org.cn/1000-9825/18/538.htm ? ?
    »ùÓ?ä×?ÄÏàËÆÐÔ,Ìá³öÁËÎÞ?µ¼µlºÓï¾?½?Í?¼·½·¨.»ù±¾?ÏëÊÇ:Ê×ÏÈ,Ô?ºÓï¾ä×?âµLù´¡ÉÏ,?¹ý¾ä¶Ô?¼äµKÔÆë,µõ½½»ÌæµÄÏà??¶?ÍÏàÒì?¶Ï.?ºó,¸ù¾ÝÏà??¶ÏÓÅÏ?òÏàÒì?¶ÏÓÅÏ?ßÂÔ,??Ïà?µKÔÆë?¶Ï×÷?¾ä×???ò?,²¢¶?ÉÄÜÒò?¶?»²æ¶øµ¼Ö±?çH²ulò?½øÐÐÆçÒåÏû½â.×îºó,?¹ýÖ?é?¾ä×??Ö,Í?¼³öºÓï¾?½?Ê÷.?Á?ÜÃâ¶ÔÆë¹ý³ÌÖ??Ä?ÊèÎÊÌâ,»¹¶?¿·?ßÓÐÃ÷Ï?æÂ?JÊÊÂÏÈ×÷Á?éÀ?Àí.·?ðÒ??¢´ÊÐÔÒ?°´Êjº?ÊÐÔ×÷?¾ä×?ù±¾¹¹³?¥?,Æ2âÁËÍ?¼µl?½á¹û.²âÊ?á¹û±íÃ÷:¶ÔÓÚ3Ö?¹³?¥?,ÏàÒì?¶ÏÓÅÏ?é?µõ½µLá¹ûµÄF?¶¼³¬¹ýÁË46%,¾ùÓÅÓÚÏà??¶ÏÓÅÏ?é?Ëùµõ½µLá¹û,×îºõJ?ÁË49.52%,ºÃÓÚÒ?¨µ5Lá¹û
      -- Add Phrases

    ========
    ========
    ========
    ========
    ========



    WOW
    in \i\t\cluster.txt
    |
This paper proposes an algorithm to hierarchically cluster documents. Each
cluster is actually a cluster of documents and an associated cluster of words,
thus a document-word co-cluster. Note that, the vector model for documents
creates the document-word matrix, of which every co-cluster is a submatrix. One
would intuitively expect a submatrix made up of high values to be a good
document cluster, with the corresponding word cluster containing its most
distinctive features.
    |
The popular classes of document clustering algorithms are k-means and its
variants, hierarchical agglomerative clustering ( HAC ) methods [1], and more
recently, graph partitioning methods [2,3].
    |
HAC algorithms start with singleton documents as clusters, and iteratively merge
the two most similar clusters.
    |
The model commonly used for documents is the bag-of-words or vector space model.
A set of words is chosen from the set of all words in all documents using some
criterion. Each document is a vector in the feature space formed by these words.
The vector entries can be word frequencies or some other measure. For any
moderately sized document set, the number of words runs into a few thousands.
Thus document vectors are high-dimensional. Putting these vectors together, we
get the document-word matrix which is extremely sparse.
    |
For each word, a count of the number of documents in which it occurs is kept. We
then remove words occurring in less than 0.2% of the documents i.e the
low-frequency words, and those occurring in more than 20% of the documents i.e
the high-frequency words. The remaining words constitute our feature space.
    |
2.1.1 Creation of Document Vectors
    |
Suppose the feature space consists of n words, and we have d documents. Document
i is represented by mi , a vector in the space Rn . The component of this vector
corresponding to the j -th word is given by:
    |
mij = [$\displaystyle {\frac{{t_{ij}}}{{\sqrt{\sum_{j=1}^{n}{t_{ij}}^{2}}}}}$]
log [$\displaystyle {\frac{{d}}{{d_{j}}}}$]
    |
where tij is the number of occurrences of word j in document i or the TF ( Term
Frequency ) , and dj is the number of documents in which the word j occurs. Thus
we first unitize the TF vector, and then multiply its individual components by
the IDF ( inverse document frequency ) values of the corresponding words. For our
algorithm, this gives better performance than the model used by existing
algorithms which use the TFIDF values, with or without unitization. Unitization
is done to account for varying document lenghts, and we feel unitizing the TF
vector rather than the TFIDF vector allows better exploitation of the IDF values
for the clustering. Thus, having got the document vectors, we get the d×n
document-word matrix. From now on, we will denote this matrix by M .
    |
Recall that the i -th cluster is a submatrix Si defined by the row set Ri and
the column set Ci . For clusters i, j , the density of the submatrix formed by
the row set Ri and the column set Cj is a measure of how much we can expect to
find the words of cluster j in the documents making up cluster i . A similar
comment can be made about the submatrix formed by the combination of Rj and Ci .
We define the similarity between clusters i, j as the average of all the
elements making up these two submatrices.
    |
Documents with relatively large lengths were observed to be better leader
documents for the algorithm above. They generally created large and meaningful
document clusters, with the cardinality of the associated word clusters
typically being 2 to 5 times that of the corresponding document clusters.
    |
Its clustering algorithm is suffix tree clustering ( STC ) [13], which models a
document as a string of words, and not as a vector.
    |
    Probabilistic Latent Semantic Analysis ( PLSA ) on a collection,
    building three probability tables: P( z ), P( d|z ), and P( w|z )
    where z in Z are the latent variables ( categories ),
    d ? D are the documents in the collection,
    and w ? W are the terms in the vocabulary over the collection,
      -- http://www.cs.cmu.edu/~lemur/3.0/cluster.html
      Document Clustering
    |
Once the clusters are formed, other techniques such as profiling or discriminant
analysis can be used to see what internal variables account for the clustering.
    |
dependency structure matrix ( "DSM" ) models. A DSM is a matrix representation of
a directed graph that can be used to represent systems and structures, including
a complex system. [0005] FIG. 1 illustrates a simple DSM. The column and row
headings in the matrix ( A-G ) correspond to elements or nodes. "X" marks inside
the matrix cells indicate whether corresponding nodes are related.
Later, X -> p.
    |
the minimum description length ( "MDL" ) scoring metric. The MDL can be
interpreted as follows: among all possible models, choose the model that uses
the minimal length for describing a given data set ( that is, model description
length plus mismatched data description length ).
    |
The data representations of objects to be clustered also take many forms. The
most common are
  a square distance or similarity matrix, in which both rows and columns
correspond to the objects to be clustered. A correlation matrix is an example of
a similarity matrix.
  a coordinate matrix, in which the rows are observations and the columns are
variables, as in the usual SAS multivariate data set. The observations, the
variables, or both may be clustered.


    Dissimilarity matrix.
    ( Optional: see the parameter PRINT ).
    The lower-left triangle of the matrix,
    as input or computed by the program.

    the Meila-Shi algorithm,
    which takes the eigenvectors corresponding to the k largest eigenvalues
    of the matrix P = SD - 1 for some k, and then invokes another
    ( e.g. k-means ) to cluster points by their respective k components in
    these eigenvectors.

    all is... information compression
    by multiple alignment,
    unification and search


    robot learning requires novel methods for learning to identify
    important past events and memorize them until needed.

    Artificial Curiosity.
    Schmidhuber's learning agents like to go where they expect
    to learn something. They lose interest in both predictable
    and unpredictable things.

    Subgoals are represented as desired abstract observations
    which cluster raw input data.

    A similarity function determines similar entity classes
    by using a matching process over
    synonym sets,
    semantic neighborhoods,
    and distinguishing features that are classified into
    parts, functions, and attributes.

    In order to build semantic communities and connect peers
    that share common interests with each other, one needs to
    evaluate the similarity between their subscriptions.

    a system for allowing the meaning of words to be modeled
    on a computer such that computational use of these meanings
    would be possible.

    Semantic networks may be traversed via concept list views,
    via their relations, or by retracing the user's history.

    Fourier used the work of Laplace and Lagrange
    to solve the heat equation by separation of variables

    Definition: Cosine Similarity:
    "scalar product over product of moduli":
    Their scalar product is defined to be the sum a.b = a1b1 + ... + anbn,
    and if we divide this by the moduli ||a|| and ||b|| we obtain
    the cosine of the angle between the two vectors,
    which is called their cosine similarity
    cos( a,b ) = ( a1b1 + ... + anbn ) / ( ||a|| ||b|| ).
    |
    Wow!
    One of the great benefits of the vector formalism is that it allows us
    to combine words into sentences or documents by adding their vectors
    together.
    If article vectors have been built during the preprocessing phase,
    the associate program can also be used to find nearby documents
    and thus for information retrieval.
    |
    a NOT b = a - ( a.b )b
    can be shown to give a vector for the expression "a NOT b"
    which has zero cosine similarity with the unwanted vector b.
    |
    The weight for each bit can be related to some function of its variance.
    In other words, if this descriptor has approximately the same value
    for all N objects to be clustered, it may not be a valuable descriminator.


    =========
    =========
    =========

    Query:

    Later, score the query token matches on each page, and for query.


    =======

    Phrases:

        Lists are not phrases:
                Do more strict punct rules
                ALL CAPS are not phrases.
                All Leading Caps are not phrases.
                Put a size limit on it.
        Get the final . back.

    Examples below were from bogus query: john debbie black white

    Something like this huge dash separated list is common....
    Idea 1: Put a max length on sentences;
    Idea 2: Rank sentences against length.
    Idea 3: Recognize lists by autocorrelation, somehow...?
    Idea 4: Use language part-of-speech dictionaries to test for sentences.

-_-_-_-_-_-_-_-_-_-_-_-

Insane Clown Posse - Great Milenko Insane Clown Posse - Psychopathic From Outer Space Insane Clown Posse - Riddle Box Insane Clown Posse - Ring Master Insane Clown Posse - Sangri-la Insane Clown Posse - The Amazing Jeckel Brothers Insane Clown Posse - Hallowicked V.a J : Jadakiss - Kiss The Game Jadakiss - Kiss Of Dead ( bootleg ) Jump Steady - The chaos Theory Jedi Mind Trick - The Amber Probe ep Jedi Mind Trick - Genghis Khan 12 Inch Jedi Mind Trick - live @ The Playboy Mansion' Mix Tape Jedi Mind Trick -

-_-_-_-_-_-_-_-_-_-_-_-

JOHNNY HAMMOND
JOHNNY HANSON PRESENTS
JOHNNY HATES JAZZ
JOHNNY HEARTSMAN
JOHNNY J
JOHNNY J

-_-_-_-_-_-_-_-_-_-_-_-

Vance Standard Poodle 02/06/81-09/05/05 Elisabeth Gelfund Miss Swiss Mocha Fudge ( Mocha ) Yorkshire Terrier 10 years 07/15/03 Kelley Lee Miss Sybil German Shepherd Mix 10 months 07/98 Darrell, Robin,& Maggie Miss SydnII Sweet Yorkshire Terrier 07/31/87-06/10/01 Shawna Vandebuerie Miss T Dog 02/24/90-05/18/06 Cooks Miss Tabitha ( Toes ) Cat 8/21/98 S Burnam Miss Tamiami Cat 9 years Brenda and Sandie Miss Teek Dog 10/27/97-07/11/05 Harriet & Mike Despain Miss Tess Miss Yellow Lab 05/25/96-06/28/04 Tom and Fella

-_-_-_-_-_-_-_-_-_-_-_-

Cindy & Les Lieurance 06/01/2007
  California Towhee at Baker Meadow Bob & Susan Steele fide Heindel 06/01/2007
  Northern rough-winged swallow along Mammoth creek Donna & Bob Willey 06/01/2007 ( PHOTO )
  Blue Grosbeak and Olive-sided Flycatcher at Cottonwood Creek in the Whites Justin Hite 05/31/2007
  American Dipper: fast-running stream near Mammoth Lakes Donna & Bob Willey 05/30/2007 ( PHOTOS )
  Phainopepla at Greengate Cottonwoods Dixon Lane and Birchim Jim and Debby Parker 05/30/2007 ( PHOTO )
  Birds from Furnace Creek Ranch, DVNP Tom Wurster fide Heindels 05/28/2007
...

-_-_-_-_-_-_-_-_-_-_-_-

    =======

    IDEA: variant keyword to GET, meaning to not UNDERSCORE result page.

    Idea: Add a mode to Word search to keep order within pages.

    IDEA: fetch all urls: make it also do cleartext URLs.
    This is not so easy: CTop just initiates a fetch by URLIndex.
    What I should do instead, is to catalog all clear-text URLs
    into the page's IDX during every html/text parse.
    |
    Strip <> around cleartext urls:
    <http://www.,,,,,etc.....cgi?number=5771>.
    |
    Reject partial cleartext urls ending with ellipsis:
    http://ath.....etc.....com/library/FAQs/religion/blgrk...?
    |
    Strip any "prefix:" before method of cleartext URLs:
    WWW:http://www....etc...ca/usitt/


    Big IDEA: I COULD have the view underline all anchor text per
    the IDX data; Then I could offer to follow a link upon a click!


    ====

    // Other information ideas:

    I read that Shannon's measure of information on [1-n] symbols
    each with probabilities pi ( e.g., 1/n if equal ) is the Sum for
    i = 0 to n of the LOG of the reciprocal of the probability pi.
    I could apply that rule to just the tokens of one sentence or
    block, using the global probabilities from the whole corpus.

    I would like to have a posterior recomputation of valuations,
    otherwise early pages metrics rate too high. Perhaps a thread?
    It could also re-tabulate search engine valuations.

    N.B. Rule: Improbable things contain more information.
    Spanish rates high because words not in my word lists.
    Somehow I must tell improbable REAL things ( like words )
    from low-frequency RANDOM or GARBAGE or PATTERNED things.
    I could have a multi-level strategy:
        1. Somehow, tell the real from random or patterned.
        2. Then rank and search only among the real things.


    ========

    Remaining form parse flaws:

    I never saw "ACTION URL..." atoms. Check logic.

    Add:
      ONSUBMIT=...
      ONCLICK=...
      ONKEYPRESS=...
      ONCHANGE=...

    example of a JS form toughie:
    http://stariq.com/pagetemplate/ArticleSearch.asp
    No submit? It has JS + this construct in form...
     <a href="javascript:document.mainsearch.submit( );">
     <img src="/images/go.gif" border="0" height="26" width="23"
     alt="StarIQ Article Search"></a> </td>

    example of a JS form toughie:
    http://www.getsonic.net/search/search.pl
      TEXT type INPUT tag has no NAME.


    Study:
    Can we extract URLs from JavaScript in general, just to list/fetch?
        1 keydown

    ========

    Idea: Accept asterisk on end of search word; Also at beginning.


    Idea: keep lists of framesets in each paper for when refetch binary?

    Idea: Use rules to do adaptive word wrap, so as to use 60 columns
    if there is not a predominant wider pre-existing word wrap in it.

    =======

    Idea: Whatever this RSS or RDF stuff is, it is RICH!
    I could try to consume it too:

    <?xml version="1.0" encoding="UTF-8"?>
    <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns="http://purl.org/rss/1.0/" xmlns:admin="http://webns.net/mvcb/" xmlns:l="http://purl.org/rss/1.0/modules/link/" xmlns:content="http://purl.org/rss/1.0/modules/content/">
      <!--Generated by Blogger v5.0-->
      <channel rdf:about="http://www.greylodge.org/RSS/">
        <title>GLOR Feed</title>
        <link>http://www.greylodge.org/RSS/</link>
        <description>The RSS feed for Greylodge Occult Review contents.</description>
        <dc:date>2005-06-22T23:30:48Z</dc:date>
        <dc:language>en-US</dc:language>
        <admin:generatorAgent rdf:resource="http://www.blogger.com/" />
        <admin:errorReportsTo rdf:resource="mailto:rss-errors@blogger.com" />
        <items>
          <rdf:Seq>
            <rdf:li rdf:resource="http://www.greylodge.org/RSS/2005/06/issue-seventeen-divine-horsemen-living.htm" />
            <rdf:li rdf:resource="http://www.greylodge.org/RSS/2005/06/issue-seventeen-disinformation-tv.htm" />
            <rdf:li rdf:resource="http://www.greylodge.org/RSS/2005/06/issue-seventeen-titicut-follies.htm" />
            <rdf:li rdf:resource="http://www.greylodge.org/RSS/2005/06/issue-seventeen-hwy-american-pastoral.htm" />
            <rdf:li rdf:resource="http://www.greylodge.org/RSS/2005/06/issue-seventeen-cut-ups.htm" />
        ...

    =======

    Study other kinds of people to publish my freeware:
    Library and Information Technology Association.
    Accessibility browsing.



    Tiresias is a family of sans-serif typefaces that were designed for
    best legibility by people with impaired vision at the
    Royal National Institute of the Blind in London.


    =======

    Idea: RTF looks easy, so parse RTF as a valid text type.

    Make a more lucid exposition of Revelation. Correlate myths.

    Idea: Top menu command Get Terms - to list Words CSol as a result.

    Idea: Top menu command Get Urls - to list URL CSol as a result.

    Idea: Top menu command Show URLs: toggles to put URLS in cleartext;
    Other Idea: Let SHIFT key highlight links, click to fetch/open them.

    Later, add a cleartext URL in the usage text going to my website,
    where I will have a file listing groomed search engine form data.

    Idea: After doing a word/web search, add a correlation of some
    terms that frequently appear with the search terms to results.

    Idea: Try this for the hourglass when needed:
    SetCursor( LoadCursor( NULL, IDC_WAIT ) );
    SetCursor( LoadCursor( NULL, IDC_CROSS ) );
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );

    Idea: novel analysis ideas, etc:
        "These terms only appear in links pointing to this page":


    BIG IDEA: JAVASCRIPT. Recognize a little JS to glean urls:
    |
    And a related challenge: Forms, and options tags, etc.,
    may contain web page URLs to extract and list/follow:
    |
    <FORM METHOD=GET ACTION="http://http://www.gogumogog.com/assets/multimedia/TheErevRav.WMA">
    <INPUT TYPE=SUBMIT VALUE=" Click to play! ">
    |
    <option value="http://cn.pantone.com/pantone_v1.asp">
    |
    <form name='pgOvr'>
    <input type='hidden' name='pgNm' value='/m2/ftp/classics/whokilledtheelectriccar/index.html'>
    |
    <form name="buttonbar" action="../index.html" id="buttonbar">
    ACTION URL:http://www.scicli.com/index.html
    <input type="submit" value="Go to Scicli Net" />
    |
    | The last script we will do in this section will allow you to use your button as a link. It's pretty fun to use every now and then. Just use the following command in your onClick command:
    |
    | window.location='url'
    |
    | Here is a script that will send you to a page I made just for this example:
    |
    | <FORM>
    | <INPUT type="button" value="Go to my other Page!" name="button6" onClick="window.location='http://www.pageresource.com/jscript/newpage.htm'">
    | </FORM>
    |
    | =======
    |
    | Redirection is often used to take viewers to a page depending on their browser's name or version. To redirect a viewer instantly, you just need to add a short command in your head section:
    |
    |
    | <HEAD>
    | <SCRIPT language="JavaScript">
    | <!--
    | window.location="http://someplace.com";
    | //-->
    | </SCRIPT>
    | </HEAD>
    |
    |
    | ======
    | All right, here's an example code for opening a new window:
    |
    | <FORM>
    | <INPUT type="button" value="New Window!" onClick="window.open( 'http://www.pageresource.com/jscript/jex5.htm','mywindow','width=400,height=200' )">
    | </FORM>
    |
    | =======
    |
    | The first method is to access a javascript function within the HREF attribute of your link tag. So, if you want to link to another page, you normally write:
    |
    | <A HREF="nextpage.htm">Click here</A>
    |
    | Well, you can access a javascript function you have written instead by writing the link this way:
    |
    | <A HREF="javascript:myfunction( )">Click Here</A>
    |
    | Yes, now you can open that new window without using the grey button. Here is a script to give you the new window:
    |
    | First, I found that this works much better if you create your own function in the head section first. All this function needs to do is open your new window. So, in your head section, create a function like this:
    |
    | <HEAD>
    | <SCRIPT language="JavaScript">
    | <!--hide
    |
    | function newwindow( )
    | {
    | window.open( 'jex5.htm','jav','width=300,height=200,resizable=yes' );
    | }
    | //-->
    | </SCRIPT>


    =========


        Regarding crashes and hung/hanging:


    It crashes on my Win XP pro at home! -- but pbly not if .net build.


    Watch/study for a past ( crashing ) bug:
    Study ATTR table/enums/usages, see if still off by one anywhere:
    _looks~1.htm: good/bad url has PRIOR/NEXT TAG html ATTR n*gfK4*n#0nb

    1. In VC6.0 build, CombineAndHang DLL may exceed a malloc. Vista build ok.
    1. In VC6.0 build, 100% CPU usage is infinite loop in DLLs. Vista build ok.

    Tip: When RELEASE crashes and DEBUG doesnt, overflowed stack array !!!
    Somebody may be using stuff after free. Study scope of all variables.

    Idea: Fix all AddKey or Find users to test for 0 back on malloc failure.
                            2 FIX -- all AddKey callers,
                            2 FIX -- all Find callers,

    Idea: Fix all malloc users like I did realloc users, to test for null return.
                            wsb 2 FIX: -- all GetBuffer callers
                            sol 2 FIX -- GetFullKey etc. callers

    ======

    Remember indefinitely for UCS tests!
    Test pages for Unicode character ranges
    -- http://www.alanwood.net/unicode/

    Remember indefinitely to see foreigns, broaden vocabulary lists:
    http://wikipedia.org/

    GET 001 http://www.waste.org/mail/?list=pynchon-l&keywords=

    ======


    PPC Sites to check out when I am ready...

We are glad to announce our new mobile friendly Pocket PC free software
downloads catalog located at http://www.pocketpcfreeware.mobi . This little web
site is optimized for mobile browsers and slow mobile internet connections.
PocketPCfreeware.mobi is an extension of our freeware windows mobile software
archive accessible through desktop PC here

More than 6000 free softwares are available for download directly through your
device with any mobile browser. We have tested it with Pocket IE, Opera,
NetFront and Minimo. If you need more mobile friendly resources feel free to
visit our web portal and browser thousands of mobile sites, RSS/XML feeds and
mobile TV channels
  -- http://www.mobiletopsoft.com/

[PDA User Guides & Manuals for Pocket PC and Palm devices] Handheld's user
guides and manuals online help documentation
  -- http://www.mobiletopsoft.com/

Mailing List receive the latest news and software titles
  -- http://www.mobiletopsoft.com/

Software >> Advanced Search | Help More Downloads >> Latest | Popular |
<!--Essentials |--> Search Downloads For >> WindowsMobile | Palm | Blackberry |
SmartPhone | View All
  -- http://www.pocketpccity.com/

http://www.pc-o-k.de/?pocket_pc_freeware
  -- http://www.pc-o-k.de/?pocket_pc_freeware

http://www.pocketgear.com/software_browse.asp?cat=82
  -- http://www.pocketgear.com/software_browse.asp?cat=82

Pocket PC and phone software and freeware
  -- http://www.mobiletopsoft.com/pocket-pc/

?sta es una versi?n de http://www.powa.org/freeware-pda-pocket-pc.phtml
  -- http://cc.msnscache.com/cache.aspx?q=6371374286173&lang=es-US&mkt=es-US&FORM=CVRE

http://www.ppc4you.com/topsites/
  -- http://www.ppc4you.com/topsites/

http://www.ppcmag.com/
  -- http://www.ppcmag.com/

http://www.freewarepocketpc.net/
  -- http://www.freewarepocketpc.net/

Welcome to the largest Pocket PC freeware downloads archive.
  -- http://www.freewarepocketpc.net/ppc-tag-utilities.html

Ésta es una versión de http://www.pocketpcsoft.net/index.php
  -- http://cc.msnscache.com/cache.aspx?q=6036727843622&lang=es-US&mkt=es-US&FORM=CVRE2

http://www.pocket4you.com/freeware/
  -- http://www.pocket4you.com/freeware/

http://pocketpcsoft.net/
  -- http://pocketpcsoft.net/

http://www.smartphone-freeware.com/tag-pocket.html
  -- http://www.smartphone-freeware.com/tag-pocket.html

http://www.giveawayoftheday.com/freeware/pocket/pc-emulator
  -- http://www.giveawayoftheday.com/freeware/pocket/pc-emulator

Welcome to the original community for Microsoft Windows CE and PocketPC users.
  -- http://www.hpc.net/

http://www.softempire.com/mobile-and-pda-software-4.html
  -- http://www.softempire.com/mobile-and-pda-software-4.html

http://www.freewareppc.com/
  -- http://www.freewareppc.com/

http://www.pdazone.net/pocketpc/
  -- http://www.pdazone.net/pocketpc/

http://www.svpocketpc.com/
  -- http://www.svpocketpc.com/

http://www.pocketpc-software.org/
  -- http://www.pocketpc-software.org/

    ==========


....*/




#include "stdafx.h"
#include "resource.h"
#include "CAll.h" // Globals


#define MENU_HEIGHT 26

// Global Variables: ( but most are in CAll.cpp )
HINSTANCE           g_hInst;  // current instance
HWND                g_hWnd;   // main window
HWND                g_hWndStatusDialog; // when visible, else NULL
HWND                g_hWndSaveProgressBar; // when SaveFolder dialog is visible, else NULL

#ifndef _WIN32_WCE
// It turns out that the FIND common dialog is only via MFC libs on PPC.
HWND                g_hWndCommonFindDialog; // when visible, else NULL
UINT                g_uFindReplaceMsg; // message identifier for FINDMSGSTRING
FINDREPLACE         g_fr; // common dialog box structure
int g_fr_Flags_Down = 1; // to remember a g_fr Flags bit.
int g_fr_Flags_Case = 0; // to remember a g_fr Flags bit.
int g_fr_Flags_Word = 0; // to remember a g_fr Flags bit.
int g_fr_Accept_F3 = 0; // to remember ever did FindNext.
wchar_t             g_szFindWhat[80];  // buffer receiving User's Find string
#endif // not _WIN32_WCE


#ifdef _WIN32_WCE
HWND                g_hWndCB; // command bar
static SHACTIVATEINFO s_sai;
#endif

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );
void                ExitingWithCancel( );
void                ProgramErrorMessageBox( );
int                 PopulateComboBox( void * pUser, CoIt * pCoIt );

// I will wrapper all these to prove assumption that no
// thread off the main ever invokes... CTop, CSat, CPas.

LRESULT CALLBACK    WndProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    AboutDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    QueryDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    FetchDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    KwicDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    PhrasesDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    StatusDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    StoresDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    AddFolderDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    SaveFolderDlgProc1( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    UseFolderDlgProc1( HWND, UINT, WPARAM, LPARAM );

// All Proc2 need to omit CALLBACK keyword, lest get a wierd exception.
// No, it wasn't that. FolderDlgProc2 returned 0, put Proc1 in hot loop.
LRESULT WndProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT AboutDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT QueryDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT FetchDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT KwicDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT PhrasesDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT StatusDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT StoresDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT AddFolderDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT SaveFolderDlgProc2( HWND, UINT, WPARAM, LPARAM );
LRESULT UseFolderDlgProc2( HWND, UINT, WPARAM, LPARAM );

#ifdef _WIN32_WCE
HWND                CreateRpCommandBar( HWND );
#endif


// The cast, in order of appearance...

int CharacterizeMainWindowDeviceContext( );
int CreateFirstOrRevisedFont( );
LRESULT WmSizeHandler( HWND hWnd, WPARAM wParam, LPARAM lParam );
void SelectFirstOrRevisedFont( );
void MeasureFontNowinCompatDC( );
void ComputeScreenLineLayout( );


// De Plane, De plane!

#ifdef _WIN32_WCE
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
{
    #if DO_DEBUG_CALLS
        Routine( L"360" );
    #endif
    if ( !InitInstance ( hInstance, nCmdShow ) )
    {
        #if DO_DEBUG_CALLS
            Routine( L"361" );
        #endif
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators( hInstance, ( LPCTSTR )IDC_WORDSEX );
    MSG msg;
    #if DO_DEBUG_MAIN
        ; Spew( L"WinMain Starting msg loop" );
    #endif
    #if DO_DEBUG_CALLS
        Routine( L"1001" );
    #endif
    InDispatch = 0; // Henceforth, do anti-thread-calling-top/see/pas tests.

    BOOL bRet; // Now following the example of Win Help for IsDialogMessage.
    while ( ( bRet = GetMessage( &msg, NULL, 0, 0 ) ) != 0 )
    {
        #if DO_DEBUG_MAIN
        {
            wchar_t wk[100];
            wsprintf( wk, L"WinMain GetMsg m=0x%08x w=0x%08x l=0x%08x", msg.message, msg.wParam, msg.lParam );
            ; Spew( wk );
        }
        #endif

        if ( bRet == -1 )
        {
            // handle the error and possibly exit. Huh? New concept.
            #if DO_DEBUG_MAIN
                ; Spew( L"WinMain ending for GetMessage error return." );
            #endif
            return 1; // stopping application
        }
        else
        {

#ifndef _WIN32_WCE
            // This provision is for occasional find-in-file operations.
            // If the modeless dialog is up, it processes some messages.
            if( g_hWndCommonFindDialog == NULL
            || ! IsWindow( g_hWndCommonFindDialog )
            || ! IsDialogMessage( g_hWndCommonFindDialog, &msg ) )
#endif // not _WIN32_WCE
            {
                // Otherwise, do the normal message handling process.

                // Well, not yet. Do one other find-or-replace thing.
                // Win help says to do: if( message == g_uFindReplaceMsg )
                // and returns 0 when processed. But I don't know if it
                // should be done here? No, rather, down in the WinProc.

                #if DO_DEBUG_MAIN
                    ; Spew( L"WinMain b4 TA." );
                #endif
                if ( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
                {
                    #if DO_DEBUG_MAIN
                        ; Spew( L"WinMain in TA." );
                    #endif

                    TranslateMessage( &msg );
                    #if DO_DEBUG_MAIN
                        ; Spew( L"WinMain 4b TM." );
                    #endif
                    DispatchMessage( &msg );
                    #if DO_DEBUG_MAIN
                        ; Spew( L"WinMain 4b DM." );
                    #endif
                }
                #if DO_DEBUG_MAIN
                    ; Spew( L"WinMain 4b TA." );
                #endif

                // Every path shall be fault-tolerant enough to get back to here.
                // I changed malloc failures to spew, then exit program directly.
                // Hmmm. If thread exits, I should keep bool to stop main too...?

                // Before the fatal malloc failure, I put a halt on fetches.
                // Tell user once if this happens.

                if( gbMallocLimitExceeded
                && ! gbMallocLimitToldUser )
                {
                    gbMallocLimitToldUser = 1;
                    MessageBox( g_hWnd, L"WordsEx has reached 500 Mb of mallocs.\r\nFurther downloading is stopped during this execution,\r\nlest a system out-of-memory error cause WordsEx to exit,\r\ndiscarding all of your downloaded data.", L"Enough!", MB_OK );
                }

                if( g_szInternalError != NULL )
                {
                    InDispatch ++; // Prevent testing once gone on error path

                    #if DO_DEBUG_MAIN
                        ; Spew( L"WinMain in g_szInternalError" );
                    #endif
                    // Stop all threads...
                    g_bStopAllThreads = 1; // Let them coast to a stop during msg box
                    g_bStopInitThread = 1; // Let them coast to a stop during msg box
                    while( g_bInitThreadRunning ) { } // await his loop-abort-break.

                    ProgramErrorMessageBox( );

                    // offer the user a chance to save his work...
                    if( CSolAllUrls.nList > 2 ) // including tail ( #0 ) and head ( #1 )
                    {
                        if( MessageBox( g_hWnd, L"Save a folder?", L"Program Must Exit", MB_ICONSTOP | MB_YESNO ) == IDYES )
                        {
                            DialogBox( g_hInst, ( LPCTSTR )IDD_SAVE_FOLDER_DIALOG, g_hWnd, ( DLGPROC )SaveFolderDlgProc1 );
                        }
                    }

                    #if DO_DEBUG_MAIN
                        ; Spew( L"WinMain ending for g_szInternalError" );
                    #endif
                    return 1; // stopping application
                }
            }
        }
    }
    #if DO_DEBUG_MAIN
        ; Spew( L"WinMain Ending" );
    #endif
    return msg.wParam;
}

ATOM MyRegisterClass( HINSTANCE hInstance )
{
    #if DO_DEBUG_CALLS
        Routine( L"363" );
    #endif

#ifdef _WIN32_WCE
    WNDCLASS wc;
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = ( WNDPROC ) WndProc1;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInstance;
    wc.hIcon            = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_WORDSEX ) );
    wc.hCursor          = 0;
    wc.hbrBackground    = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName     = 0;
    wc.lpszClassName    = L"WordsEx";
    return RegisterClass( &wc );

#else

    // Window Style CS_OWNDC means what I set in the DC will persist.
    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof( WNDCLASSEX );
    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc    = ( WNDPROC ) WndProc1;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon( hInstance, ( LPCTSTR )IDI_WORDSEX );
    wcex.hCursor        = LoadCursor( NULL, IDC_CROSS );
    wcex.hbrBackground  = ( HBRUSH )( COLOR_WINDOW+1 );
    wcex.lpszMenuName   = ( LPCTSTR )IDC_WORDSEX; // a mis-typecast of integer
    wcex.lpszClassName  = L"WordsEx";
    wcex.hIconSm        = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_SMALL );
    return RegisterClassEx( &wcex );

#endif
}

BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
    #if DO_DEBUG_CALLS
        Routine( L"331" );
    #endif
    g_hInst = hInstance;

    //If it is already running, then focus on the window
    g_hWnd = FindWindow( L"WordsEx", L"WordsEx" );
    if ( g_hWnd )
    {
        #if DO_DEBUG_CALLS
            Routine( L"332" );
        #endif
        SetForegroundWindow ( ( HWND ) ( ( ( DWORD ) g_hWnd ) | 0x01 ) );
        return 0;
    }

    SetLastError( 0 );

    MyRegisterClass( hInstance );

    #if DO_DEBUG_CALLS
        Routine( L"365" );
    #endif

    // This structure specifies which controls to load. I use:
    // 1. the spin button
    // 2. the progress bar

    INITCOMMONCONTROLSEX InitComCtrls;
    InitComCtrls.dwSize = sizeof( InitComCtrls );
    InitComCtrls.dwICC = ICC_UPDOWN_CLASS | ICC_PROGRESS_CLASS;

    if( ! InitCommonControlsEx( & InitComCtrls ) )
    {
        #if DO_DEBUG_CALLS
            Routine( L"366" );
        #endif
        return FALSE;
    }
    #if DO_DEBUG_CALLS
        Routine( L"367" );
    #endif

#ifndef _WIN32_WCE
    // This step is needed to use the Find-or-Replace dialog box.
    g_uFindReplaceMsg = RegisterWindowMessage( FINDMSGSTRING );
    if( g_uFindReplaceMsg == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"485" );
        #endif
        ProgramError( L"RegisterWindowMessage" );
        return FALSE;
    }
#endif // not _WIN32_WCE

    if( ! Www.LoadWinInetDll( ) )
    {
        #if DO_DEBUG_CALLS
            Routine( L"368" );
        #endif
        return FALSE;
    }
    #if DO_DEBUG_CALLS
        Routine( L"369" );
    #endif

#ifdef _WIN32_WCE
    {
        // The Wizard's issue of "moving" the window by the height of menu bar
        // can be avoided by finding out the dimensions from the horse's mouth:
        RECT rc;
        memset( & rc, 0, sizeof( rc ) );

        #if DO_DEBUG_CALLS
            Routine( L"370" );
        #endif

        if( ! SystemParametersInfo( SPI_GETWORKAREA, 0, & rc, 0 ) )
        {
            #if DO_DEBUG_CALLS
                Routine( L"371" );
            #endif
            ProgramError( L"SystemParametersInfo" );
            return FALSE;
        }

        #if DO_DEBUG_CALLS
            Routine( L"372" );
        #endif

        // WinCE uses flags: WS_VISIBLE | WS_VSCROLL,
        // WS_VSCROLL gives a standard scrollbar which is part of
        // the window, not in client rectangle, neither a control.

        // The 3rd and 4th params are not right/bottom, but width/height.
        // I was just lucky before if the top/left happened to be zero.

        g_hWnd = CreateWindow( L"WordsEx", L"WordsEx", WS_VISIBLE | WS_VSCROLL,
            rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            NULL, NULL, hInstance, NULL );

        // That's great and all; But it did not overcome my problem:
        // Scrollbar is full window height, past bottom system tray.
        // Soution: Set the SIP up, then down again, after Show...

        #if DO_DEBUG_CALLS
            Routine( L"373" );
        #endif
    }
#else
    {
        // Win32 uses flags: WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        // WS_VSCROLL gives a standard scrollbar which is part of
        // the window, not in client rectangle, neither a control.

        // I decided to position the window myself.
        // Can't find shit in help system. Wing it.
        // The Wizard's issue of "moving" the window by the height of menu bar
        // can be avoided by finding out the dimensions from the horse's mouth:
        RECT rc;
        memset( & rc, 0, sizeof( rc ) );

        if( ! SystemParametersInfo( SPI_GETWORKAREA, 0, & rc, 0 ) )
        {
            #if DO_DEBUG_CALLS
                Routine( L"371" );
            #endif
            ProgramError( L"SystemParametersInfo" );
            return FALSE;
        }

        #if DO_DEBUG_CALLS
            Routine( L"374" );
        #endif

        // Make adjustments
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
		// Original 1.0 window position layout design
        // int wa = w / 10;
        // int ha = h / 10;
        // rc.right -= wa;
        // rc.left += wa;
        // rc.bottom -= ha;
        // rc.top += ha;
		// New 2013-02-12 window position layout design (was into top right corner)
        //rc.left += w * 60 / 100;
        //rc.bottom -= h/2;
		// New 2016-04-25 window position layout design (bottom right, above chronic line)
		rc.left += w * 60 / 100;
		rc.top += h * 60 / 100;
		rc.bottom -= h * 5 / 100;

        // The 3rd and 4th params are not right/bottom, but width/height.

        g_hWnd = CreateWindow( L"WordsEx", L"WordsEx", WS_OVERLAPPEDWINDOW | WS_VSCROLL,
            rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            NULL, NULL, hInstance, NULL );

        #if DO_DEBUG_CALLS
            Routine( L"375" );
        #endif
    }
#endif

    if ( !g_hWnd )
    {
        #if DO_DEBUG_CALLS
            Routine( L"376" );
        #endif
        return FALSE;
    }

    #if DO_DEBUG_CALLS
        Routine( L"377" );
    #endif


#ifndef _WIN32_WCE

    if( _wsetlocale( LC_CTYPE, L".1252" ) == NULL )
    {
        // At this late date, do not introduce trivial pgm errs:
        // Failed: Say( L"_wsetlocale LC_CTYPE, .1200 failed" );

        Say( L"_wsetlocale LC_CTYPE, .1252 failed" );
    }

#endif // not _WIN32_WCE

    // Characterize must happen before first show.
    // On the PPC, it must be before scrollbar work.
    if( ! CharacterizeMainWindowDeviceContext( ) )
    {
        #if DO_DEBUG_CALLS
            Routine( L"378" );
        #endif
        return FALSE;
    }

    #if DO_DEBUG_CALLS
        Routine( L"379" );
    #endif

    if( ! CreateFirstOrRevisedFont( ) ) // do before WM_SIZE, who selects it.
    {
        #if DO_DEBUG_CALLS
            Routine( L"380" );
        #endif
        return FALSE;
    }

    #if DO_DEBUG_CALLS
        Routine( L"381" );
    #endif

    ShowWindow( g_hWnd, nCmdShow ); // The first show -- calls WM_SIZE

    #if DO_DEBUG_CALLS
        Routine( L"382" );
    #endif

    // Pas.SetPaintInput( ); // It won't fly without these things.
    g_ReadyToAllowPaint = 1; // wip

    #if DO_DEBUG_CALLS
        Routine( L"383" );
    #endif

    UpdateWindow( g_hWnd ); // calls WM_PAINT

    #if DO_DEBUG_CALLS
        Routine( L"384" );
    #endif

#ifdef _WIN32_WCE
    {
        SHINITDLGINFO shidi;
        shidi.dwMask = SHIDIM_FLAGS;
        shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
        shidi.hDlg = g_hWnd;
        SHInitDialog( &shidi );
    }

    #if DO_DEBUG_CALLS
        Routine( L"385" );
    #endif

    SHSipPreference( g_hWnd, SIP_UP );

    #if DO_DEBUG_CALLS
        Routine( L"386" );
    #endif

    SHSipPreference( g_hWnd, SIP_DOWN );

#endif // _WIN32_WCE

    #if DO_DEBUG_CALLS
        Routine( L"387" );
    #endif

#ifdef _WIN32_WCE
    srand( GetTickCount( ) / 1000 );
#else // not _WIN32_WCE
    time_t t = time( NULL );
    srand( (unsigned int) t ) ;

    // After srand, set the random first splash screen colors.
    {
        // For varying yellows, I need to add blue light at some point... wip!
        g_rgbGlyph = RGB( 220, 220, 80 ); // some yellow?

        // For varying near-night skys, run r 40-60, g 40-60, b 60-100
        int r = 30 + rand( ) % 20;
        int g = 30 + rand( ) % 20;
        int b = 60 + rand( ) % 40;
        g_rgbErase = RGB( r, g, b ); // various dark blues
    }

    // I am getting ready to make the released version 1.0.
    // So remove this message...
    // int Limit_t = BUILD_TIME + 60 * 60 * 24 * 30;
    // if( t > Limit_t )
    // {
    //     Say(
    //     L"This pre-release WordsEx is 30 days old.\r\n"
    //     L"I suggest you download a newer version.\r\n"
    //     );
    // }

#endif // _WIN32_WCE

    #if DO_DEBUG_CALLS
        Routine( L"388" );
    #endif

    SetTimer( g_hWnd, MONITOR_THREADS_TIMER, 500, NULL ); // 500 ms

#ifndef _WIN32_WCE
    // GetSystemTimeAdjustment is not available on the Pocket PC.
    {
        // I want to optimize the smooth scrolling timer
        // to be some multiple of the system clock period.

        // lpTimeIncrement
        // [out] Pointer to a DWORD that the function sets to the interval,
        // counted in 100-nanosecond units, between periodic time adjustments.
        // This interval is the time period between a system's clock interrupts.

        // On Win 2000 I got 10.0144 ms: ( so optimize to 10, 20, 30, etc )
        // TimeAdjustment: 100144
        // TimeIncrement: 100144
        // TimeAdjustmentDisabled: 0

        // On Vista laptop I got 15.625 ms. ( so optimize to 15, 31, 46, etc )
        // TimeAdjustment: 156250
        // TimeIncrement: 156250
        // TimeAdjustmentDisabled: 1

        DWORD TimeAdjustment = 0;        // size of time adjustment
        DWORD TimeIncrement = 0;         // time between adjustments
        BOOL TimeAdjustmentDisabled = 0; // disable option

        if( GetSystemTimeAdjustment(
            & TimeAdjustment,
            & TimeIncrement,
            & TimeAdjustmentDisabled ) )
        {
            #if DO_DEBUG_TIMER
                ; SpewValue( L"TimeAdjustment", TimeAdjustment );
                ; SpewValue( L"TimeIncrement", TimeIncrement );
                ; SpewValue( L"TimeAdjustmentDisabled", TimeAdjustmentDisabled );
            #endif

            g_SystemClockQuantaIn100ns = TimeIncrement;
        }
        else
        {
            ProgramError( L"GetSystemTimeAdjustment" );
        }

    }
#endif // not _WIN32_WCE


    #if DO_DEBUG_CALLS
        Routine( L"389" );
    #endif

    Top.ShowSplashScreen( );

    return TRUE;
}

void ExitingWithCancel( )
{
    #if DO_DEBUG_CALLS
        Routine( L"390" );
    #endif
    int ThisIsAnEnder = 0;

    // Let the escape key from main window cleardown without saving.
    // Naw, that only confused me. Adding a threshold count is good.

    // For 1 or 2 web pages, this is a nuisance.
    // Set some higher threshold, say, 20 pages.
    // Wait. Not URL count: CSolAllUrls.nList...

    if( g_nPapersFetchedButNotViewed > 10 )
    {
        switch( MessageBox( g_hWnd, L"Save a folder?", L"Program Will Exit", MB_ICONQUESTION | MB_YESNOCANCEL ) )
        {
            // For case of cancel button, do not exit.

        case IDYES:
            DialogBox( g_hInst, ( LPCTSTR )IDD_SAVE_FOLDER_DIALOG, g_hWnd, ( DLGPROC )SaveFolderDlgProc1 );
            ThisIsAnEnder = 1;
            break;

        case IDNO:
            ThisIsAnEnder = 1;
            break;
        }
    }
    else
    {
        // If no data, just exit.
        ThisIsAnEnder = 1;
    }

    if( ThisIsAnEnder )
    {
        // I think smooth scrolling made memory violations at exit.
        See.QuitSmoothScrolling( 0 );

        KillTimer( g_hWnd, MONITOR_THREADS_TIMER );

        // Oh, this was in a sub-clause. If no threads running,
        // I missed the test, and a quick start + exit crashed.
        g_bStopInitThread = 1; // Because main msg loop is exiting
        while( g_bInitThreadRunning ) { } // await his loop-abort-break.

        if( g_nCountOfCacheThreads == 0
        && g_nCountOfQueryThreads == 0
        && g_nCountOfFetchThreads == 0
        && g_nCountOfFolderThreads == 0
        && g_nCountOfKwicThreads == 0 )
        {
            // No threads are running. Okay to do it myself.
            AnyFinalDebugDumps( );
            if( SpewFile != NULL )
            {
                fclose( SpewFile );
                SpewFile = NULL;
            }
            DestroyWindow( g_hWnd );
        }
        else
        {
            // Some threads are running.
            g_bStopAllThreads = 1; // Cause main msg loop to await all done.
            // No, this is Windows. Nobody can wait in a loop.
            // Cause all threads done to send a message, and
            // upon receipt of that message, continue to call
            // AnyFinalDebugDumps( );
            // DestroyWindow( g_hWnd ); --Do not call it here.
        }
    }
}

void ProgramErrorMessageBox( )
{
    #if DO_DEBUG_CALLS
        Routine( L"333" );
    #endif

#ifndef _WIN32_WCE
    // Only on the desktop, due to use of _beginthread...
    // Even before "First", Check for my errno provision:
    if( g_dwLastError >= 40000000 ) // Is 40 million area unused?
    {
        int was_errno = g_dwLastError - 40000000;
        wchar_t wk[100];
        wchar_t * into = wk;
        char * from = strerror( was_errno );
        for( ;; )
        {
            char c = *from ++;
            if( c < ' ' ) // stop on a strerror promised \n, else stop on null
                break;
            *into = ( wchar_t ) c;
            if( ++into == wk + sizeof( wk ) - 1 )
                break;
        }
        *into = NULL;
        MessageBox( g_hWnd, wk, g_szInternalError, MB_OK );
        ; Spew( L"Program Error Handler:" );
        ; Spew( g_szInternalError );
        ; Spew( wk );
    }
    else
#endif // not _WIN32_WCE

    if( g_dwLastError == ERROR_SUCCESS )
    {
        // Do simpler than calling FormatMessage.
        MessageBox( g_hWnd, g_szInternalError, L"Program Error", MB_OK );
        ; Spew( L"Program Error Handler:" );
        ; Spew( g_szInternalError );
    }
    else
    {
        // First, do it simply, even for "extended" error.
        HMODULE hModule = NULL; // default to system source
        DWORD dwBufferLength = 0;
        DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_FROM_SYSTEM;

        // In the case of WinInet numbers, point to that source.
        if( g_dwLastError >= 12001
        && g_dwLastError <= 12156 ) // probably higher - find a symbol
        {
            hModule = g_hWinInet;
            if( hModule != NULL )
                dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
        }

        wchar_t * MessageBuffer = NULL; // receives the LocalAlloc.

        dwBufferLength = FormatMessage(
            dwFormatFlags,
            hModule,
            g_dwLastError,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
            ( wchar_t * ) & MessageBuffer, // type was mis-cast on purpose
            0,
            NULL );

        // Handle the case that FormatMessage failed.
        if( dwBufferLength == 0 )
        {
            wchar_t wk[80];
            wsprintf( wk, L"Error No. %d", g_dwLastError );
            MessageBox( g_hWnd, wk, g_szInternalError, MB_OK );
            ; Spew( L"Program Error Handler:" );
            ; Spew( wk );
            return;
        }

        // Normal case after FormatMessage succeeded.
        MessageBox( g_hWnd, MessageBuffer, g_szInternalError, MB_OK );
        ; Spew( L"Program Error Handler:" );
        ; Spew( g_szInternalError );
        ; Spew( MessageBuffer );
        if( MessageBuffer != NULL )
            LocalFree( MessageBuffer );
        MessageBuffer = NULL;
    }


    // HEY - this cannot work, only in current thread:
    // if( g_dwLastError == ERROR_INTERNET_EXTENDED_ERROR )
    // {
        // For any "extended" error, show a second message box.
        //
        // InternetGetLastResponseInfo retrieves
        // the last Windows CE Internet function
        // error description or server response
        // on the thread calling this function.
        //
        // wchar_t * MessageBuffer = NULL; // receives the malloc.
        // DWORD dwBufferLength = 0;
        //
        // // call once to learn wcslen.
        // InternetGetLastResponseInfo( & g_dwLastError, MessageBuffer, & dwBufferLength );
        //
        // if( dwBufferLength > 0 )
        // {
        //     dwBufferLength += 1; // was wcslen w/o null; add 1 for null.
        //     MessageBuffer = ( wchar_t * ) MyMalloc( 637, dwBufferLength * sizeof( wchar_t ) );
        //
        //     // call again to get the string.
        //     if( InternetGetLastResponseInfo( & g_dwLastError, MessageBuffer, & dwBufferLength ) )
        //     {
        //         MessageBox( g_hWnd, MessageBuffer, g_szInternalError, MB_OK );
        //     }
        //     MyFree( 1285, zx, MessageBuffer );
        //     MessageBuffer = NULL;
        // }
    // }

}

LRESULT CALLBACK WndProc1( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS || DO_DEBUG_WNDPROC
        Routine( L"1002" );
    #endif
    InDispatch ++;
    LRESULT x = WndProc2( hWnd, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT WndProc2( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_MAIN
    {
        wchar_t wk[100];
        wsprintf( wk, L"WinProc Msg m=0x%08x w=0x%08x l=0x%08x", message, wParam, lParam );
        ; Spew( wk );
    }
    #endif

#ifndef _WIN32_WCE
    // This non-constant identifier value cannot be used in a switch case:
    if ( message == g_uFindReplaceMsg )
    {
        #if DO_DEBUG_WNDPROC
            Routine( L"1500" );
        #endif
        // Get pointer to FINDREPLACE structure from lParam.
        // I'll bet this is same as I used to invoke dialog.
        LPFINDREPLACE lpfr = ( LPFINDREPLACE ) lParam;

        // If the FR_DIALOGTERM flag is set,
        // invalidate the handle identifying the dialog box.
        if ( ( lpfr->Flags & FR_DIALOGTERM ) != 0 )
        {
            #if DO_DEBUG_WNDPROC
                Routine( L"1501" );
            #endif
            g_hWndCommonFindDialog = NULL;
            See.ResumeAnySmoothScrolling( );
            #if DO_DEBUG_WNDPROC
                Routine( L"1502" );
            #endif
            return 0;
        }

        // If the FR_FINDNEXT flag is set,
        // call the application-defined search routine
        // to search for the requested string.
        if( ( lpfr->Flags & FR_FINDNEXT ) != 0 )
        {
            #if DO_DEBUG_WNDPROC
                Routine( L"1503" );
            #endif
            // Save these three states between uses, and for F3 command.
            g_fr_Flags_Down = ( g_fr.Flags & FR_DOWN );
            g_fr_Flags_Case = ( g_fr.Flags & FR_MATCHCASE );
            g_fr_Flags_Word = ( g_fr.Flags & FR_WHOLEWORD );
            g_fr_Accept_F3 = 1;

            See.QuitSmoothScrolling( 1 ); // 1 = Align
            See.OnFindNext( );
            #if DO_DEBUG_WNDPROC
                Routine( L"1504" );
            #endif
        }
        #if DO_DEBUG_WNDPROC
            Routine( L"1505" );
        #endif
        return 0;
    }
#endif // not _WIN32_WCE


    switch ( message )
    {

    case WM_COMMAND:
        #if DO_DEBUG_WNDPROC
            Routine( L"1506" );
        #endif
        // This is the main WndProc...
        switch ( LOWORD( wParam ) )
        {

#ifdef _WIN32_WCE
        case IDOK: // IDOK is the top right circled ( X ) on PPC
            #if DO_DEBUG_WNDPROC
                Routine( L"1601" );
            #endif
            SendMessage( hWnd, WM_ACTIVATE, MAKEWPARAM( WA_INACTIVE, 0 ), ( LPARAM )hWnd );
            // Despite the PPC usage model is that OK will next exit app,
            // for now, I want a real exit app to occur if they click ok.
            SendMessage ( hWnd, WM_CLOSE, 0, 0 );
            break;
#endif

        case ID_HELP_ABOUT:
            #if DO_DEBUG_WNDPROC
                Routine( L"1602" );
            #endif
            DialogBox( g_hInst, ( LPCTSTR )IDD_ABOUTBOX, hWnd, ( DLGPROC )AboutDlgProc1 );
            break;

        case ID_FILE_EXIT:
            #if DO_DEBUG_WNDPROC
                Routine( L"1603" );
            #endif
#ifdef _WIN32_WCE
            SendMessage( hWnd, WM_ACTIVATE, MAKEWPARAM( WA_INACTIVE, 0 ), ( LPARAM )hWnd );
#endif
            SendMessage ( hWnd, WM_CLOSE, 0, 0 );
            break;

        case ID_FIND_WORD:
            #if DO_DEBUG_WNDPROC
                Routine( L"1604" );
            #endif
            if( CSolAllUrls.nList == 2 ) // including head, tail.
            {
                Say( L"Add Words, or Ctrl+W, will search for matching words in all web pages held in memory."
                L"\r\n\r\nHowever, there are no texts in memory. Try another of the Add actions to get some." );
            }
            else
            {
                DialogBox( g_hInst, ( LPCTSTR )IDD_KWIC_DIALOG, hWnd, ( DLGPROC )KwicDlgProc1 );
            }
            break;

#ifndef _WIN32_WCE
        case ID_FIND_IN_PAGE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1605" );
            #endif
            SetUpFindDialog( );
            break;
#endif // not _WIN32_WCE

        case ID_NEXT:
            #if DO_DEBUG_WNDPROC
                Routine( L"1606" );
            #endif
            Top.Next( );
            break;

        case ID_BACK:
            #if DO_DEBUG_WNDPROC
                Routine( L"1607" );
            #endif
            Top.Back( );
            break;

        case ID_DELETE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1608" );
            #endif
            See.OnVkDelete( );
            break;

#ifndef _WIN32_WCE
        case ID_FILE_INVOKE_IE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1609" );
            #endif
            Top.InvokeInternetExplorer( );
            break;
#endif // not _WIN32_WCE

        case ID_HELP_USAGE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1610" );
            #endif
            Top.AddUsage( );
            break;

        case ID_HELP_EZEKIEL1:
            #if DO_DEBUG_WNDPROC
                Routine( L"1611" );
            #endif
            Top.AddEzekiel( );
            break;

        case ID_HELP_REVELATION:
            #if DO_DEBUG_WNDPROC
                Routine( L"1612" );
            #endif
            Top.AddRevelation( );
            break;

        case ID_HELP_REVNOTES:
            #if DO_DEBUG_WNDPROC
                Routine( L"1613" );
            #endif
            Top.AddRevNotes( );
            break;

        case ID_HELP_YOM_TERUAH:
            #if DO_DEBUG_WNDPROC
                Routine( L"1614" );
            #endif
            Top.AddTeruah( );
            break;

        case ID_HELP_STATUS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1615" );
            #endif
            DialogBox( g_hInst, ( LPCTSTR )IDD_STATUS_DIALOG, hWnd, ( DLGPROC )StatusDlgProc1 );
            break;

        case ID_HELP_STORES:
            #if DO_DEBUG_WNDPROC
                Routine( L"1616" );
            #endif
            DialogBox( g_hInst, ( LPCTSTR )IDD_STORES_DIALOG, hWnd, ( DLGPROC )StoresDlgProc1 );
            break;

        case ID_ADD_CACHE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1617" );
            #endif
            Initiator.InitiateCache( );
            break;

        case ID_ADD_WEB_SEARCH:
            #if DO_DEBUG_WNDPROC
                Routine( L"1618" );
            #endif
            // 2013-03-01 do not call at work -- 
				// 2016-04-25 Re-enabling to call at work --
				DialogBox( g_hInst, ( LPCTSTR )IDD_QUERY_DIALOG, hWnd, ( DLGPROC )QueryDlgProc1 );
            break;

        case ID_ADD_DIRECTORY:
            #if DO_DEBUG_WNDPROC
                Routine( L"1619" );
            #endif
            DialogBox( g_hInst, ( LPCTSTR )IDD_ADD_FOLDER_DIALOG, hWnd, ( DLGPROC )AddFolderDlgProc1 );
            break;

        case ID_ADD_FILE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1620" );
            #endif
            Fio.AddFile( );
            break;

        case ID_ADD_WEB_PAGE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1621" );
            #endif
            // 2013-03-01 do not call at work -- 
				// 2016-04-25 Re-enabling to call at work --
				DialogBox(g_hInst, (LPCTSTR)IDD_FETCH_DIALOG, hWnd, (DLGPROC)FetchDlgProc1);
            break;

        case ID_ADD_LINKS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1622" );
            #endif
            // 2013-03-01 do not call at work -- 
				// 2016-04-25 Re-enabling to call at work --
				Top.AddLinks();
            break;

        case ID_ADD_PHRASES:
            #if DO_DEBUG_WNDPROC
                Routine( L"1623" );
            #endif
            if( CSolAllUrls.nList == 2 ) // including head, tail.
            {
                Say( L"Add, Best Sentences, will show best-ranked sentences of all web pages held in memory."
                L"\r\n\r\nHowever, there are no texts in memory. Try another of the Add actions to get some." );
            }
            else
            {
                DialogBox( g_hInst, ( LPCTSTR )IDD_PHRASES_DIALOG, hWnd, ( DLGPROC )PhrasesDlgProc1 );
            }
            break;

        case ID_FILE_SAVEALL:
            #if DO_DEBUG_WNDPROC
                Routine( L"1624" );
            #endif
            DialogBox( g_hInst, ( LPCTSTR )IDD_SAVE_FOLDER_DIALOG, hWnd, ( DLGPROC )SaveFolderDlgProc1 );
            break;

        case ID_FILE_SAVEFILE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1625" );
            #endif
            Fio.SaveFile( );
            break;

        case ID_FILE_SAVE_ALL_URLS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1626" );
            #endif
            SaveListOfAllUrls( );
            break;

#ifndef _WIN32_WCE
        // A temporary development feature?
        // Only on Windows Desktop version.
        case ID_FILE_SAVE_VOCABULARY:
            #if DO_DEBUG_WNDPROC
                Routine( L"1635" );
            #endif
            SaveVocabulary( ); // in CVoc.cpp
            break;
#endif // not _WIN32_WCE

        case ID_FILE_COPY:
            #if DO_DEBUG_WNDPROC
                Routine( L"1627" );
            #endif
            See.CopyCurrentViewToClipboard( );
            break;

        case ID_FILE_PASTE:
            #if DO_DEBUG_WNDPROC
                Routine( L"1628" );
            #endif
            See.CopyClipboardMakeNewPaper( );
            break;

        case ID_HELP_FONTPLUS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1629" );
            #endif
            // This is needed lest wrong scanline foobar mousing:
            See.QuitSmoothScrolling( 1 ); // 1 = align
            g_dFontPointSize *= 1.1;
            CreateFirstOrRevisedFont( );
            SelectFirstOrRevisedFont( );
            ComputeScreenLineLayout( );
            {
                size_t FocusLineOffset = See.GetOffsetofFocusLine( );
                See.SaveBackingText_MakeLineList( NULL, -1, NULL ); // Re-do line list ( font size ).
                See.SetFocusLineToThisOffset( FocusLineOffset );
            }
            See.IMightHaveChangedTheDisplay( ); // font size changed
            break;

        case ID_HELP_FONTMINUS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1630" );
            #endif
            // This is needed lest wrong scanline foobar mousing:
            See.QuitSmoothScrolling( 1 ); // 1 = align
            g_dFontPointSize /= 1.1;
            CreateFirstOrRevisedFont( );
            SelectFirstOrRevisedFont( );
            ComputeScreenLineLayout( );
            {
                size_t FocusLineOffset = See.GetOffsetofFocusLine( );
                See.SaveBackingText_MakeLineList( NULL, -1, NULL ); // Re-do line list ( font size ).
                See.SetFocusLineToThisOffset( FocusLineOffset );
            }
            See.IMightHaveChangedTheDisplay( ); // font size changed
            break;

        case ID_HELP_COLORS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1631" );
            #endif
            {
                g_rgbIndex ++;
                if( g_rgbIndex >= N_RGB_CYCLE )
                    g_rgbIndex = 0;
                g_rgbErase = rgbErase[ g_rgbIndex ];
                g_rgbGlyph = rgbGlyph[ g_rgbIndex ];
            }
            See.SetFirstOrRevisedTextColors( );
            See.IMightHaveChangedTheDisplay( ); // color change
            break;

        case ID_HELP_EXTEND_VIEW:
            // I would have added a checkbox here, but that is a hard or
            // MFC feature. There was a Win Help article on "simulating"
            // menu checkbox by loading system check bitmap icons...Yuck!
            #if DO_DEBUG_WNDPROC
                Routine( L"1636" );
            #endif
            if( g_bOmitSignage )
                g_bOmitSignage = 0;
            else
                g_bOmitSignage = 1;
            {
                size_t FocusLineOffset = See.GetOffsetofFocusLine( );
                See.SaveBackingText_MakeLineList( NULL, -1, NULL ); // Re-do line list ( font size ).
                See.SetFocusLineToThisOffset( FocusLineOffset );
            }
            See.IMightHaveChangedTheDisplay( ); // toggled g_bOmitSignage
            break;

        case ID_HELP_REWRAP:
            #if DO_DEBUG_WNDPROC
                Routine( L"1632" );
            #endif
            Top.RewrapCurrentTopItemViewOnScreen( );
            break;

        case ID_JUMP_BOOKMARK:
            #if DO_DEBUG_WNDPROC
                Routine( L"1633" );
            #endif
            g_JumpActivated = 1; // affects next WM_CHAR digit
            break;

        case ID_FILE_LOAD_ENGINES:
            #if DO_DEBUG_WNDPROC
                Routine( L"1634" );
            #endif
            if( g_bEverStartedAnyQuery )
            {
                Say( L"Search engines cannot be revised after doing Add, Internet Search." );
            }
            else
            {
                Url.LoadSearchEngineForms( );
            }
            break;

        case ID_FILE_SAVE_QUERY_FORMS:
            #if DO_DEBUG_WNDPROC
                Routine( L"1638" ); // keep last before 1699 as highes 16xx
            #endif
            Url.SaveSearchEngineForms( );
            break;

        case ID_CTRL_DELETE:
            // Let CONTROL + DELETE recover the last deleted view.
            if( g_pFruitOfLastDeletedView != NULL )
            {
                Top.Add( g_pFruitOfLastDeletedView ); // Put fruit on display
                See.SetFocusLineToThisOffset( g_OffsetOfLastDeletedView );
                g_pFruitOfLastDeletedView = NULL;
            }
            break;

#ifndef _WIN32_WCE
        case ID_CTRL_DOWN:
            // Let CONTROL + ARROW DOWN replace the lost ARROW DOWN function.
            PostMessage( g_hWnd, WM_VSCROLL, MAKELONG( SB_LINEDOWN, 0 ), 0L );
            break;
#endif // not _WIN32_WCE

        default:
            #if DO_DEBUG_WNDPROC
                Routine( L"1699" );
            #endif
            return DefWindowProc( hWnd, message, wParam, lParam );

        }
        break;


    // Hey... Those above were COMMANDS.
    // Whereas these below are MESSAGES.
    // That's why my keyins disappeared.


    case WM_CHAR:
        #if DO_DEBUG_WNDPROC
            Routine( L"1507" );
        #endif
        // I cannot make ACCEL table entries on digits !?!?
        // But I can get them here, with/without ALT status.
        // wParam - Specifies the character code of the key.
        // lParam - Bit 29 Specifies the context code.
        // The value is 1 if the ALT key is held down while
        // the key is pressed; otherwise, the value is 0.
        if( wParam >= '0' && wParam <= '9' )
        {
            if( g_JumpActivated )
                Top.VisitBookmark( wParam - '0' ); // with ALT+J prior
            else
                Top.SetBookmark( wParam - '0' ); // without ALT+J prior
            g_JumpActivated = 0; // disarm liberally
        }
        else
        {
            g_JumpActivated = 0; // disarm liberally

            return DefWindowProc( hWnd, message, wParam, lParam );
        }
        break;


    case WM_SIZE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1508" );
        #endif
        return WmSizeHandler( hWnd, wParam, lParam );
        break;

    case WM_ERASEBKGND:
        #if DO_DEBUG_WNDPROC
            Routine( L"1509" );
        #endif
        // Just for grins, paint background RED to show when
        // I have a bitmap over 64 kB and did not use BitBlt.
        // 64 Kb is about 1 square inch at desktop resolution.
        // Very curious--
        // Apparently, WM_ERASEBKGND happens BEFORE WM_SIZE,
        // because the RED background I painted here is the
        // size of the window just before each resize.
        // So I moved any erase play into on-paint routine.
        // Anyway, return 1 to avoid the prior erase step.
        return 1; // Say we handled it.
        break;

    case WM_PAINT:
        #if DO_DEBUG_WNDPROC
            Routine( L"1510" );
        #endif
        return See.WmPaintHandler( hWnd, wParam, lParam );
        break;

    case WM_VSCROLL:
        #if DO_DEBUG_WNDPROC
            Routine( L"1511" );
        #endif
        return See.OnWmVscroll( hWnd, message, wParam, lParam );
        break;

    case WM_KEYDOWN:
        #if DO_DEBUG_WNDPROC
            Routine( L"1512" );
        #endif
        return See.OnWmKeydown( hWnd, message, wParam, lParam );
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MOUSEMOVE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1513" );
        #endif
        return See.WmMouseHandler( hWnd, message, wParam, lParam );

#ifdef _WIN32_WCE
    case WM_CREATE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1514" );
        #endif
        g_hWndCB = CreateRpCommandBar( hWnd );
        memset ( &s_sai, 0, sizeof ( s_sai ) );
        s_sai.cbSize = sizeof ( s_sai );
        break;
#endif

    case WM_CLOSE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1515" );
        #endif
        ExitingWithCancel( );
        break;

    case WM_DESTROY:
        #if DO_DEBUG_WNDPROC
            Routine( L"1516" );
        #endif
#ifdef _WIN32_WCE
        CommandBar_Destroy( g_hWndCB );
#endif
        PostQuitMessage( 0 ); // Only ever makes sense in the WM_DESTROY.
        break;

#ifdef _WIN32_WCE
    case WM_SETTINGCHANGE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1517" );
        #endif
        SHHandleWMSettingChange( hWnd, wParam, lParam, &s_sai );
        break;
#endif

    case WM_ACTIVATE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1518" );
        #endif
#ifdef _WIN32_WCE
        SHHandleWMActivate( hWnd, wParam, lParam, &s_sai, FALSE );
#endif
        // first foobar of infinite paint loop is after PauseAnySmooth...
        // So, gate calls off by during program start-up using this bool.
        if( g_ReadyToAllowPaint )
        {
            int fActive = LOWORD( wParam );
            int fMinimized = HIWORD( wParam );
            if( fActive == WA_INACTIVE
            || fMinimized )
            {
                See.PauseAnySmoothScrolling( );
            }
            else
            {
                See.ResumeAnySmoothScrolling( );
            }
        }
        break;

    case WM_TIMER:
        #if DO_DEBUG_WNDPROC
            Routine( L"1519" );
        #endif
        if( wParam == SMOOTH_SCROLL_TIMER )
        {
            See.WmSmoothScrollTimerHandler( );
        }
        else if( wParam == GROW_OMEGA_TIMER )
        {
            OmegaElapsedms += GROW_OMEGA_TIMER;
            // SpewValue( L"Grow", OmegaElapsedms );
            if( ! InvalidateRect( g_hWnd, NULL, 0 ) )
            {
                ProgramError( L"InvalidateRect 3" );
            }
        }
        else if( wParam == MONITOR_THREADS_TIMER )
        {
            See.WmMonitorThreadsTimerHandler( );
        }
        break;

#ifdef _WIN32_WCE
    case WM_HIBERNATE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1520" );
        #endif
        // Later,
        // This message is sent to an application when system resources are running low.
        // An application should attempt to release as many resources as possible when
        // sent this message by unloading dialog boxes, destroying windows, or freeing
        // up as much local storage as possible without killing the internal state.
        // You leave this state upon receiving a WM_ACTIVATE or WM_CLOSE message.
        break;
#endif


    case WM_THREADENDING:
        #if DO_DEBUG_WNDPROC
            Routine( L"1521" );
        #endif
        Initiator.OnThreadEnding( wParam, lParam );
        break;

    case WM_THREADCHANGE:
        #if DO_DEBUG_WNDPROC
            Routine( L"1522" );
        #endif
        // Same message, but this time caught in main DlgProc,
        // means that all threads are stopped, may destroy it.
        AnyFinalDebugDumps( );   // Postponed from ExitingWithCancel
        if( SpewFile != NULL )
        {
            fclose( SpewFile );
            SpewFile = NULL;
        }
        DestroyWindow( g_hWnd ); // Postponed from ExitingWithCancel
        break;

    default:
        #if DO_DEBUG_WNDPROC
            Routine( L"1599" );
        #endif
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}

#ifdef _WIN32_WCE
HWND CreateRpCommandBar( HWND hwnd )
{
    #if DO_DEBUG_CALLS
        Routine( L"336" );
    #endif
    SHMENUBARINFO mbi;
    memset( &mbi, 0, sizeof( SHMENUBARINFO ) );
    mbi.cbSize     = sizeof( SHMENUBARINFO );
    mbi.hwndParent = hwnd;
    mbi.nToolBarId = IDM_MENU;
    mbi.hInstRes   = g_hInst;
    mbi.nBmpId     = 0;
    mbi.cBmpImages = 0;
    if ( !SHCreateMenuBar( &mbi ) )
        return NULL;
    return mbi.hwndMB;
}
#endif


int CharacterizeMainWindowDeviceContext( )
{
    #if DO_DEBUG_CALLS
        Routine( L"390" );
    #endif

    // Called only during InitInstance before Show( SIZE ) or Update( PAINT ).

    // Trying to untangle the DC & FONT mess, I am going to simplify:
    // Pocket PC supports bitblt. It says no 64K bitblt, but works.
    // Win 2000 supports bitblt, and 64K bitblt, and it works well.
    // Assume every modern Windows supports bitblt, and 64K bitblt.
    // Then I will never need to set font nor color in the main DC.

    // Every modern desktop supports CS_OWNDC.
    // Pocket PC does NOT support CS_OWNDC.

    // Per Windows help:
    // 1. An application can retrieve a handle to the private device context
    // by using the GetDC function any time after the window is created.
    // The application must retrieve the handle only once.
    // Thereafter, it can keep and use the handle any number of times.
    // Because a private device context is not part of the display device
    // context cache, an application need never release the device context
    // by using the ReleaseDC function.
    // 2. In Windows NT/Windows 2000, create all your objects when you first
    // need them. Do not destroy the objects until you are done with them.

    // Therefore, start using a global g_hdcMain which I get once,
    // and never restore, on the desktop; but which I will restore
    // and NULL out, and re-get as needed, on the Pocket PC build.

    // Similarly, use one global g_hdcCompat between CSee and CPas.
    // However, I cannot make g_hdcCompat before a WM_SIZE message.

    g_hdcMain = GetDC( g_hWnd );
    if( g_hdcMain == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"391" );
        #endif
        ProgramError( L"GetDC g_hdcMain 1" );
        return 0; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"392" );
    #endif

    // Expected. Nay, Required!
    if( ( GetDeviceCaps( g_hdcMain, RASTERCAPS ) & RC_BITBLT ) == 0 )
    {
        #if DO_DEBUG_CALLS
            Routine( L"393" );
        #endif
        ProgramError( L"WordsEx requires Windows BitBlt support." );
        return 0; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"394" );
    #endif

    // Ignore this capability status lack on the Pocket PC,
    // for I see Pocket PC does do bitblt of over 64K okay.
    #ifndef _WIN32_WCE
        if( ( GetDeviceCaps( g_hdcMain, RASTERCAPS ) & RC_BITMAP64 ) == 0 )
        {
            #if DO_DEBUG_CALLS
                Routine( L"395" );
            #endif
            ProgramError( L"WordsEx requires Windows 64Kb BitBlt support." );
            return 0; // failure
        }
    #endif // not _WIN32_WCE

    #if DO_DEBUG_CALLS
        Routine( L"396" );
    #endif

    // I needed the bits/color and planes to figure out BM size.
    // BITSPIXEL = Number of adjacent color bits for each pixel.
    // PLANES = Number of color planes.
    // I find that my Win 2000 Pro has 24 bits, 1 plane.
    // I find that the PocketPC has 16 bits, 1 plane.

    g_BitsPerPixel = GetDeviceCaps( g_hdcMain, BITSPIXEL );
    g_ColorPlanes = GetDeviceCaps( g_hdcMain, PLANES );
    g_PelsPerInch = GetDeviceCaps( g_hdcMain, LOGPIXELSY );
    #if DO_DEBUG_SCREEN || DO_DEBUG_FONT
        ; SpewValue( L"BitsPerPixel", g_BitsPerPixel );
        ; SpewValue( L"ColorPlanes", g_ColorPlanes );
        ; SpewValue( L"PelsPerInch", g_PelsPerInch );
    #endif

    #if DO_DEBUG_CALLS
        Routine( L"397" );
    #endif

    // Per Windows Help, I never release my private DC.
    // But on Pocket PC, I keep releasing my common DC.

#ifdef _WIN32_WCE
    if( ! ReleaseDC( g_hWnd, g_hdcMain ) )
    {
        #if DO_DEBUG_CALLS
            Routine( L"398" );
        #endif
        ProgramError( L"ReleaseDC g_hdcMain" );
        return 0; // failure
    }
    g_hdcMain = NULL;
#endif // _WIN32_WCE

    #if DO_DEBUG_CALLS
        Routine( L"399" );
    #endif

    return 1; // success
}

int CreateFirstOrRevisedFont( )
{
    #if DO_DEBUG_CALLS
        Routine( L"400" );
    #endif

    // After Characterize, to know screen resolution,
    // but before WM_SIZE, who selects it, make font.
    // This same routine can be used for size change.
    // This routine will not actually select into DC.
    // This routine will neither deselect nor delete.

    // The minus sign is meaningful to CreateFontIndirect,
    // affect size comparison; Maybe needful on Pocket PC?
    // No, not that.
    // Something was needful. The last thing I did was to move
    // the LogicalFont from the stack to a global variable, and
    // some re-arrangements in main, postponing SIP calls, then
    // the CreateFontIndirect stopped failing. Not sure why...

    double dLogicalHeight = g_dFontPointSize / 72.0 * ( double ) g_PelsPerInch;
    int iLogicalHeight = ( int ) floor( dLogicalHeight + 0.5 );
    #if DO_DEBUG_SCREEN || DO_DEBUG_FONT
        ; SpewValue( L"Logical Font Height", iLogicalHeight );
    #endif

    memset( & LogicalFont, 0, sizeof( LogicalFont ) );
    LogicalFont . lfHeight = iLogicalHeight;
    LogicalFont . lfWeight = FW_NORMAL; // ... or ? FW_LIGHT;
    LogicalFont . lfCharSet = DEFAULT_CHARSET;
    LogicalFont . lfOutPrecision = OUT_DEFAULT_PRECIS;
    LogicalFont . lfClipPrecision = CLIP_DEFAULT_PRECIS;
    LogicalFont . lfQuality = DEFAULT_QUALITY;
    LogicalFont . lfPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
#ifdef _WIN32_WCE
    wcscpy( LogicalFont . lfFaceName, L"Tahoma" ); // THE font on Pocket PC
#else // not _WIN32_WCE
    wcscpy( LogicalFont . lfFaceName, L"Arial" ); // Tahoma lacks cyrillic
#endif // _WIN32_WCE

    g_LatestCreatedFont = CreateFontIndirect( & LogicalFont );
    if( g_LatestCreatedFont == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"401" );
        #endif
        ProgramError( L"CreateFontIndirect" );
        return 0; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"402" );
    #endif

    g_MustMeasureLatestCreatedFont = 1; // After Selecting font into DC
    return 1; // success
}

LRESULT WmSizeHandler( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"403" );
    #endif

    // Services WM_SIZE message
    // One WM_SIZE message comes before the first paint.
    // See reorganization comments in Characterize... above.

    size_t FocusLineToRestoreAfterSizeChange = 0;

    switch( wParam )
    {
    case SIZE_MAXHIDE:
        #if DO_DEBUG_CALLS
            Routine( L"404" );
        #endif
        // Message is sent to all pop-up windows when some
        // other window is maximized.
        return 0;

    case SIZE_MAXSHOW:
        #if DO_DEBUG_CALLS
            Routine( L"405" );
        #endif
        // Message is sent to all pop-up windows when some
        // other window has been restored to its former size.
        return 0;

    case SIZE_MINIMIZED:
        #if DO_DEBUG_CALLS
            Routine( L"406" );
        #endif
        // The window has been minimized.
        // Remember the focus offset until next restore or maxize:
        if( g_FocusOffsetToRestoreWhenResize == -1 ) // always -1 unless is iconic
            g_FocusOffsetToRestoreWhenResize = See.GetOffsetofFocusLine( );
        return 0;

    case SIZE_MAXIMIZED:
        #if DO_DEBUG_CALLS
            Routine( L"407" );
        #endif
        // The window has been maximized.
        if( g_FocusOffsetToRestoreWhenResize == -1 ) // always -1 unless is iconic
            FocusLineToRestoreAfterSizeChange = See.GetOffsetofFocusLine( );
        else
            FocusLineToRestoreAfterSizeChange = g_FocusOffsetToRestoreWhenResize; // was iconic
        g_FocusOffsetToRestoreWhenResize = -1; // always -1 unless is iconic
        break;

    case SIZE_RESTORED:
        #if DO_DEBUG_CALLS
            Routine( L"408" );
        #endif
        // The window has been resized, but neither the
        // SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.
        if( g_FocusOffsetToRestoreWhenResize == -1 ) // always -1 unless is iconic
            FocusLineToRestoreAfterSizeChange = See.GetOffsetofFocusLine( );
        else
            FocusLineToRestoreAfterSizeChange = g_FocusOffsetToRestoreWhenResize; // was iconic
        g_FocusOffsetToRestoreWhenResize = -1; // always -1 unless is iconic
        break;
    }

    // Rest of routine treats non-iconic cases ( maximized or restored ).

    #if DO_DEBUG_CALLS
        Routine( L"409" );
    #endif

    // There is a possibility of avoiding some work
    // after first time, if no size change involved.

    if( g_hdcCompat != NULL
    && g_ClientWidth == LOWORD( lParam )
    && g_ClientHeight == HIWORD( lParam ) )
    {
        // Still a perfect match.
        #if DO_DEBUG_CALLS
            Routine( L"410" );
        #endif

        // wip - invalidate or set focus or anything else?
        return 0;
    }

    #if DO_DEBUG_CALLS
        Routine( L"411" );
    #endif

    // I must create the first or a new compatible DC.
    // Such is the whole routine had by not returning.

    if( g_hdcCompat != NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"412" );
        #endif
        // Tear apart, delete, etc., a prior compatible DC, bitmap, etc.

        // Whenever we reached here, then these are all valid:
        // HDC g_hdcCompat; // compatible device context
        // HBITMAP g_hbmCompat; // and compatible bitmap
        // HFONT g_CompatibleDcOriginalSystemFont;
        // HBITMAP g_CompatibleDcOriginalSystemBitmap;

        // But during first WM_SIZE call in InitInstance,
        // NO FONT is selected, although one was created.
        // So then I skip over this destructor IF clause.

        // restore the original system font before deleting the DC.
        // I will not be deleting my own font that I get back here.
        if( SelectObject( g_hdcCompat, g_CompatibleDcOriginalSystemFont ) == NULL )
        {
            ProgramError( L"SelectObject Font 10" );
            return 0; // failure
        }
        g_CompatibleDcOriginalSystemFont = NULL; // I am not holding.
        g_CurrentDcSelectedFont = NULL; // None ( of mine ) is selected.

        // Restore the original system bitmap before deleting the DC.
        // I must delete the returned bitmap also called g_hbmCompat.
        if( ! SelectObject( g_hdcCompat, g_CompatibleDcOriginalSystemBitmap ) )
        {
            ProgramError( L"SelectObject system bitmap" );
            return 0; // failure
        }
        g_CompatibleDcOriginalSystemBitmap = NULL;

        // Now that my bitmap was ransomed out, delete it.
        if( ! DeleteObject( g_hbmCompat ) )
        {
            ProgramError( L"DeleteObject compatible bitmap" );
            return 0; // failure
        }
        g_hbmCompat = NULL;

        // Now that DC is all back to system objects, delete it.
        if( ! DeleteDC( g_hdcCompat ) )
        {
            ProgramError( L"DeleteDC Compatible DC" );
            return 0; // failure
        }
        g_hdcCompat = NULL;
    }

    #if DO_DEBUG_CALLS
        Routine( L"413" );
    #endif

    // And upon reaching here, these are all NULL:
    // HDC g_hdcCompat; // compatible device context
    // HBITMAP g_hbmCompat; // and compatible bitmap
    // HFONT g_CompatibleDcOriginalSystemFont;
    // HBITMAP g_CompatibleDcOriginalSystemBitmap;

    // And I will not leave this path until they are
    // all valid, except by way of any program error.

    g_ClientWidth = LOWORD( lParam );
    g_ClientHeight = HIWORD( lParam );
    #if DO_DEBUG_SCREEN || DO_DEBUG_FONT
        ; SpewValue( L"g_ClientWidth", g_ClientWidth );
        ; SpewValue( L"g_ClientHeight", g_ClientHeight );
    #endif
    // This would be the bitmap size, if I believed
    // that I should check a 64Kb bitmap capability:
    // size_t bmsize = g_ClientWidth * g_ClientHeight * g_BitsPerPixel * g_ColorPlanes / 8;

    // I kept default MM_TEXT. 1 Logical unit = 1 device pixel.
    // Save a global copy of 48% that for KWIC text width limit.
    // At 48%, they JUST fit in the default screen, but wrap in
    // some smaller screens. Take off some % and some - amount.
    g_LineXRef = BORDER_PELS;
    g_LineXEnd = g_ClientWidth - BORDER_PELS;
    g_MaxExtentLogicalUnits = g_LineXEnd - g_LineXRef;
    g_KwicTextWidthLimit = g_MaxExtentLogicalUnits * 475 / 1000 - 5;


    // Begin the new creations.
    // I've DecidedToUseBitBlt.


    // Per Windows Help, I never release my private DC.
    // But on Pocket PC, I keep releasing my common DC.
    // But even on the desktop I'll check for non-null.

    #if DO_DEBUG_CALLS
        Routine( L"413" );
    #endif

    #ifdef _WIN32_WCE
        g_hdcMain = GetDC( g_hWnd );
    #endif // _WIN32_WCE

    if( g_hdcMain == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"413" );
        #endif
        ProgramError( L"GetDC g_hdcMain 2" );
        return 0; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"414" );
    #endif

    // Notice I pass in the main window's hdc,
    // to not get a default monochrome bitmap.

    g_hdcCompat = CreateCompatibleDC( g_hdcMain );
    if ( g_hdcCompat == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"415" );
        #endif
        ProgramError( L"CreateCompatibleDC" );
        return 0; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"416" );
    #endif

    // Win Help Note: When a memory device context is created,
    // it initially has a 1-by-1 monochrome bitmap selected into it.
    // If this memory device context is used in CreateCompatibleBitmap,
    // the bitmap that is created is a monochrome bitmap.
    // To create a color bitmap, use the hDC that was used to create
    // the memory device context. ( That's why my colors were bad! )

    g_hbmCompat = CreateCompatibleBitmap( g_hdcMain, g_ClientWidth, g_ClientHeight );
    if ( g_hbmCompat == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"419" );
        #endif
        ProgramError( L"CreateCompatibleBitmap" );
        return 0; // failure
    }


    #if DO_DEBUG_CALLS
        Routine( L"418" );
    #endif

    // Per Windows Help, I never release my private DC.
    // But on Pocket PC, I keep releasing my common DC.

    #ifdef _WIN32_WCE
        if( ! ReleaseDC( g_hWnd, g_hdcMain ) )
        {
            #if DO_DEBUG_CALLS
                Routine( L"417" );
            #endif
            ProgramError( L"ReleaseDC g_hdcMain" );
            return 0; // failure
        }
        g_hdcMain = NULL;
    #endif // _WIN32_WCE

    // That's all that I needed the main DC.
    // All the rest is on the compatible DC.


    #if DO_DEBUG_CALLS
        Routine( L"420" );
    #endif

    // I must save the original system 1x1 bitmap just to
    // ransom my compatible bitmap out later for deletion.
    g_CompatibleDcOriginalSystemBitmap = ( HBITMAP ) SelectObject( g_hdcCompat, g_hbmCompat );
    if ( g_CompatibleDcOriginalSystemBitmap == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"421" );
        #endif
        ProgramError( L"SelectObject bitmap" );
        return 0; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"422" );
    #endif

    // Since I have just created a new compatible DC,
    // I must also prepare it with a font and colors.

    // This is true on the DESKTOP.
    // This is true on a POCKET PC.
    // This is not a question of CS_OWNDC support.

    See.SetFirstOrRevisedTextColors( );

    #if DO_DEBUG_CALLS
        Routine( L"423" );
    #endif

    // A First Font was created before servicing WM_SIZE.
    // However if ever selected, it got deselected above.
    // It will be installed on the first WM_SIZE service,
    // and whenever a font size change wants it, by this:

    SelectFirstOrRevisedFont( );

    #if DO_DEBUG_CALLS
        Routine( L"424" );
    #endif

    // Hereabouts in 0. WM_SIZE handler:
    // 1. SelectFirstOrRevisedFont must
    // call 2. MeasureFontNowinCompatDC
    // before 3. ComputeScreenLineLayout.

    ComputeScreenLineLayout( );

    #if DO_DEBUG_CALLS
        Routine( L"425" );
    #endif

    See.SaveBackingText_MakeLineList( NULL, -1, NULL ); // Re-do line list ( resized ).
    See.IMightHaveChangedTheDisplay( ); // screen size changed

    #if DO_DEBUG_CALLS
        Routine( L"426" );
    #endif

    See.SetFocusLineToThisOffset( FocusLineToRestoreAfterSizeChange );
    // Which included a call to...
    // SeeMightHaveChangedTheDisplay( ); // resized window

    #if DO_DEBUG_CALLS
        Routine( L"427" );
    #endif

    return 0;
}

void SelectFirstOrRevisedFont( )
{
    #if DO_DEBUG_CALLS
        Routine( L"427" );
    #endif
    // Set last created font on the compatible DC.
    // If a system font was installed, remember it.
    // If a previous font was installed, delete it.
    // If very same font was installed, fair trade.

    // If WM_SIZE calls me, he already tore up the DC,
    // and the global system font handle will be null,
    // because he reselected system font into the DC;
    // AND he set g_CurrentDcSelectedFont = NULL too.

    // If I am following any other font size ( or maybe
    // someday also typeface, font name, etc. ) changes,
    // Then I both own the font in the compatible DC,
    // and am holding a system font handle for later,
    // so both font handles will be non-NULL together.

    if( g_hdcCompat == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"427" );
        #endif
        ProgramError( L"SelectFont: g_hdcCompat == NULL" );
        return; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"428" );
    #endif

    if( g_LatestCreatedFont == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"429" );
        #endif
        ProgramError( L"SelectFont: CreatedFont == NULL" );
        return; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"430" );
    #endif

    // Who's on first?

    // Originally, I found Pocket PC returned a NULL system font.
    // I hope that was just my error, so I can write clean rules.

    int VirginDC = 0;

    if( g_CompatibleDcOriginalSystemFont == NULL )
    {
        if( g_CurrentDcSelectedFont == NULL )
        {
            #if DO_DEBUG_CALLS
                Routine( L"431" );
            #endif
            VirginDC = 1;
        }
        else
        {
            #if DO_DEBUG_CALLS
                Routine( L"432" );
            #endif
            ProgramError( L"SelectFont: Mixed up 1" );
            return; // failure
        }
    }
    else
    {
        if( g_CurrentDcSelectedFont == NULL )
        {
            #if DO_DEBUG_CALLS
                Routine( L"433" );
            #endif
            ProgramError( L"SelectFont: Mixed up 2" );
            return; // failure
        }
        else
        {
            #if DO_DEBUG_CALLS
                Routine( L"434" );
            #endif
            VirginDC = 0;
        }
    }

    #if DO_DEBUG_CALLS
        Routine( L"435" );
    #endif

    HFONT FontComingOut = ( HFONT ) SelectObject( g_hdcCompat, g_LatestCreatedFont );
    if( FontComingOut == NULL )
    {
        #if DO_DEBUG_CALLS
            Routine( L"436" );
        #endif
        if( VirginDC )
            ProgramError( L"SelectObject SystemFont == NULL" );
        else
            ProgramError( L"SelectObject MyOldFont == NULL" );
        return; // failure
    }
    else
    {
        #if DO_DEBUG_CALLS
            Routine( L"437" );
        #endif
        if( VirginDC )
        {
            g_CompatibleDcOriginalSystemFont = FontComingOut;
            if( g_MustMeasureLatestCreatedFont )
            {
                #if DO_DEBUG_CALLS
                    Routine( L"438" );
                #endif
                MeasureFontNowinCompatDC( );
                g_MustMeasureLatestCreatedFont = 0;
            }
        }
        else
        {
            // Start with anal selfcheck.
            if( g_CurrentDcSelectedFont != FontComingOut )
            {
                #if DO_DEBUG_CALLS
                    Routine( L"439" );
                #endif
                ProgramError( L"g_CurrentDcSelectedFont != FontComingOut" );
                return; // failure
            }

            // Now for the real question.
            if( FontComingOut != g_LatestCreatedFont )
            {
                #if DO_DEBUG_CALLS
                    Routine( L"440" );
                #endif
                // Then I am doing a significant font change.
                // That means the FontComingOut is obsoleted.
                if( ! DeleteObject( FontComingOut ) )
                {
                    ProgramError( L"DeleteObject FontComingOut" );
                    return; // failure
                }
                if( g_MustMeasureLatestCreatedFont )
                {
                    #if DO_DEBUG_CALLS
                        Routine( L"441" );
                    #endif
                    MeasureFontNowinCompatDC( );
                    g_MustMeasureLatestCreatedFont = 0;
                }
            }
        }
        g_CurrentDcSelectedFont = g_LatestCreatedFont;
    }
    #if DO_DEBUG_CALLS
        Routine( L"442" );
    #endif
}

void MeasureFontNowinCompatDC( )
{
    #if DO_DEBUG_CALLS
        Routine( L"443" );
    #endif

    TEXTMETRIC tm;

    if( ! GetTextMetrics( g_hdcCompat, & tm ) )
    {
        #if DO_DEBUG_CALLS
            Routine( L"444" );
        #endif
        ProgramError( L"GetTextMetrics" );
        return; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"445" );
    #endif

    #if DO_DEBUG_FONT
        ; SpewValue( L"Font: tm.tmHeight", tm.tmHeight );
        ; SpewValue( L"Font: tm.tmExternalLeading", tm.tmExternalLeading );
    #endif

    g_LineHeight = tm.tmHeight + tm.tmExternalLeading;

    // I can just leave the rest of the 64K slots zero.
    // An accurate measurement will be caught in paint.

    // Attempting to make this auto variable array [ 2 * 0x2f00 + 2]
    // on the stack crashed the program on POCKET PC. Rather than go
    // to a malloc ( should I? ), I lowered the PPC define to 0x0200.

    wchar_t Input[ 2 * SOME_GLYPHS + 2 ];
    int Output[ 2 * SOME_GLYPHS + 2 ];
    SIZE sz;

    int i;
    for( i = 0; i < SOME_GLYPHS; i++ )
    {
        Input[ 2 * i ] = ' ';
        Input[ 2 * i + 1 ] = i;
    }
    Input[ 2 * SOME_GLYPHS ] = ' ';
    Input[ 2 * SOME_GLYPHS + 1 ] = NULL;

    #if DO_DEBUG_CALLS
        Routine( L"446" );
    #endif

    if( ! GetTextExtentExPoint(
        g_hdcCompat , // Handle to the device context
        Input, // LPCTSTR null-terminated string for which extents are to be retrieved.
        2 * SOME_GLYPHS + 1, // int number of characters in the string
        1234567, // int maximum allowable width in logical units
        NULL, // LPINT to receive a count of chars fitting; or NULL
        Output, // LPINT array of integers to receive partial string extents - in LU.
        & sz ) ) // LPSIZE Cannot be NULL - SIZE structure
    {
        #if DO_DEBUG_CALLS
            Routine( L"447" );
        #endif
        ProgramError( L"GetTextExtentExPoint 1" );
        return; // failure
    }

    #if DO_DEBUG_CALLS
        Routine( L"448" );
    #endif

    int nMin = 12345678;
    int nMax = 0;
    for( i = 0; i < SOME_GLYPHS; i++ )
    {
        int n;
        g_SomeGlyphWidths[ i ] = n =
            Output[ 2 * i + 1 ] - Output[ 2 * i + 0 ];
        if( nMax < n )
            nMax = n;
        if( nMin > n )
            nMin = n;
    }

    #if DO_DEBUG_FONT
        ; SpewValue( L"SomeGlyphWidths: nMin", nMin );
        ; SpewValue( L"SomeGlyphWidths: nMax", nMax );
    #endif

    g_WidestGlyphWidth = nMax;

    // diagnostic: show me typical char widths and screen width.
    // This is for the purpose of CFwd knowing max chars / line.
    // Range on my cpu is 4 to 21, and maximized screen 1006 LU.

    // Based on Win2000, Arial font, the highest number not 18 wide is:
    // Logical Font Height: -24
    // UCS 0x0000 =  18 wide
    // UCS 0x06fe =   8 wide
    //
    #if DO_DEBUG_FONT
    {
        for( i = 0; i < SOME_GLYPHS; i++ )
        {
            wchar_t wk[ 50 ];
            wsprintf( wk, L"UCS 0x%04x = %3d wide", i, g_SomeGlyphWidths[ i ] );
            ; Spew( wk );
        }
    }
    #endif
}

void ComputeScreenLineLayout( )
{
    #if DO_DEBUG_CALLS
        Routine( L"449" );
    #endif

    if( g_LineHeight == 0 )
    {
        #if DO_DEBUG_CALLS
            Routine( L"450" );
        #endif
        ProgramError( L"g_LineHeight == 0" );
        return; // failure
    }

    g_nLinesPageAdvances = g_ClientHeight / g_LineHeight; // dropping fraction
    g_nLinesPaintingStop = g_nLinesPageAdvances + 2; // incl partial lines

    // let's adjust this down one now to overlap for reading continuity:
    g_nLinesPageAdvances --;

    if( g_nLinesPaintingStop > MAX_VISIBLE_LINES )
        g_nLinesPaintingStop = MAX_VISIBLE_LINES;

    if( g_nLinesPageAdvances > g_nLinesPaintingStop - 2 )
        g_nLinesPageAdvances = g_nLinesPaintingStop - 2;

    // Set up non-scrolling ( Base ) text line rectangles.
    // Copy g_BaseRects to g_LineRects right away.

    // CALYPS centered the focused line on screen, and allowed
    // for one-half screen of blank lines before or after text.
    // I'll change to more conventional fit of text in window.

    size_t i = 0;
    size_t yRef = 0;

    #if DO_DEBUG_MOUSE
    {
        wchar_t wk[80];
        wsprintf( wk, L"Mouse X Range ( ClientWidth ) = 0 to %d.", g_ClientWidth );
        ; Spew( wk );
        wsprintf( wk, L"Mouse Y Range ( ClientHeight ) = 0 to %d.", g_ClientHeight );
        ; Spew( wk );
    }
    #endif

    for( ;; )
    {
        // Although the .top member serves to draw each line of text,
        // the text left of m_LineXRef is put directly in paint loop.
        // My line rectangle left and right must be full client width,
        // lest I fail to erase those last couple of pels at each end.
        g_LineRects [i] . left   = g_BaseRects [i] . left   = 0; // not- m_LineXRef;
        g_LineRects [i] . top    = g_BaseRects [i] . top    = yRef;
        g_LineRects [i] . right  = g_BaseRects [i] . right  = g_ClientWidth; // not- m_LineXEnd;
        g_LineRects [i] . bottom = g_BaseRects [i] . bottom = yRef + g_LineHeight;

        #if DO_DEBUG_MOUSE
        {
            wchar_t wk[60];
            wsprintf( wk, L"Line[%2d] Y is from %4d to %4d.", i, g_BaseRects [i] . top, g_BaseRects [i] . bottom );
            ; Spew( wk );
        }
        #endif

        if( ++i >= g_nLinesPaintingStop )
            break;
        yRef += g_LineHeight;
    }

    return;
}



LRESULT CALLBACK AboutDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"337" );
    #endif
    InDispatch ++;
    LRESULT x = AboutDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT AboutDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"338" );
    #endif
    switch ( message )
    {
    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        return TRUE;

    case WM_COMMAND:
        // This is the AboutDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDOK: // IDOK is the top right circled ( X ) on PPC
        // IDOK is also for OKAY/CLOSE button on About, Status, Stores.
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ClickShowsDialogs( int which )
{
    #if DO_DEBUG_CALLS
        Routine( L"339" );
    #endif
    // It's easier in wordsex.cpp than in csee.cpp.
    switch( which )
    {
    case ENUM_ADDSEARCH:
        DialogBox( g_hInst, ( LPCTSTR )IDD_QUERY_DIALOG, g_hWnd, ( DLGPROC )QueryDlgProc1 );
        break;
    case ENUM_ADDPAGE:
        DialogBox( g_hInst, ( LPCTSTR )IDD_FETCH_DIALOG, g_hWnd, ( DLGPROC )FetchDlgProc1 );
        break;
    case ENUM_ADDFIND:
        DialogBox( g_hInst, ( LPCTSTR )IDD_KWIC_DIALOG, g_hWnd, ( DLGPROC )KwicDlgProc1 );
        break;
    }
}

int PopulateComboBox( void * pUser, CoIt * pCoIt )
{
    #if DO_DEBUG_CALLS
        Routine( L"340" );
    #endif
    HWND hwCombo = ( HWND ) pUser;
    wchar_t * Key = CoItFullKey( pCoIt ); // a malloc, user frees
    if( Key != NULL )
    {
        SendMessage( hwCombo, CB_ADDSTRING, NULL, ( LPARAM )Key );
        MyFree( 2700, zx, Key );
        Key = NULL;
    }
    return 0; // 0 = keep working
}

LRESULT CALLBACK QueryDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"341" );
    #endif
    InDispatch ++;
    LRESULT x = QueryDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT QueryDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"342" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            // Set the sorted list item into the combo box.
            HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

            CoIt * pMalVector = CSolUserQuerys.GetSortedVector( CSOL_FORWARD );
            if( pMalVector != NULL )
            {
                size_t take = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector + take++;
                    if( pCoIt->IsSentinel )
                        break;
                    if( PopulateComboBox( hwCombo, pCoIt ) != 0 )
                        break;
                }
                MyFree( 2777, UNPREDICTABLE, pMalVector );
                pMalVector = NULL;
            }

            if( g_ClickTextToInitDialogs != NULL )
                SetWindowText( hwCombo, g_ClickTextToInitDialogs );
        }
        {
            // Set the extern g_QueryDiligence range 1 to 10
            // Set the extern values into the dialog controls.
            HWND hwSpin = GetDlgItem( hDlg, IDC_SPIN1 );
            short nUpper = MAX_DILIGENCE;
            short nLower = MIN_DILIGENCE;
            SendMessage( hwSpin, UDM_SETRANGE, 0, ( LPARAM ) MAKELONG( nUpper, nLower ) );
            short nPosition = g_QueryDiligence;
            SendMessage( hwSpin, UDM_SETPOS, 0, ( LPARAM ) MAKELONG( ( short ) nPosition, 0 ) );

#ifndef _WIN32_WCE
            // Copy the non-text provision from fetch case.
            HWND hwChk2 = GetDlgItem( hDlg, IDC_CHECK2 );
            SendMessage( hwChk2, BM_SETCHECK, ( WPARAM ) ( g_NonTextLinks ? BST_CHECKED : BST_UNCHECKED ), 0 );
#endif // not _WIN32_WCE
        }
        return TRUE;

    case WM_COMMAND:
        // This is the QueryDlgProc...
        switch ( LOWORD( wParam ) )
        {
#ifndef _WIN32_WCE
        case IDC_CHECK2:
            {
                HWND hwChk2 = GetDlgItem( hDlg, IDC_CHECK2 );
                if( SendMessage( hwChk2, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
                    g_NonTextLinks = 1;
                else
                    g_NonTextLinks = 0;
            }
            return TRUE;
#endif // not _WIN32_WCE

        case IDC_COMBO1:
            if ( HIWORD( wParam ) != CBN_DBLCLK )
                break;

            // For a double-click, process the OK case.

        case IDC_BUTTON1:
            {
                int OkayToEnd = 0;

                HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

                // Hey! The "buddy" spin does not reflect changes
                // if the user directly edits the text in IDC_EDIT1.
                // Get the text and do an ATOI on it before acting.
                {
                    wchar_t wk[ 10 ]; // It's just 1 digit.
                    wk[ 0 ] = NULL; // Just in case.
                    HWND hwEdit = GetDlgItem( hDlg, IDC_EDIT1 );
                    GetWindowText( hwEdit, wk, 10 );
                    if( iswdigit( wk[ 0 ] ) )
                        g_QueryDiligence = wk[ 0 ] - '0';
                }

                // Allow full spin range:
                if( g_QueryDiligence >= MIN_DILIGENCE
                && g_QueryDiligence <= MAX_DILIGENCE )
                {
                    // Save any user text now in IDC_COMBO1
                    size_t nLength = GetWindowTextLength( hwCombo );

                    if( nLength != 0 )
                    {
                        OkayToEnd = 1;
                        size_t nMallocLen = nLength + 1;
                        wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 1051, nMallocLen * sizeof( wchar_t ) );
                        GetWindowText( hwCombo, MallocPtr, nMallocLen );

                        // Add query parameter to the Add, Internet Search list.
                        size_t Index = CSolUserQuerys.AddKey( MallocPtr );
                        #if DO_DEBUG_ADDFIND
                            if( Index == 1 )
                                { Spew( L"AddFind 1 at wordsex 1226" ); }
                        #endif

                        // Add query parameter to the Add, Word Search list too.
                        size_t Index2 = CSolUserKwics.AddKey( MallocPtr );
                        #if DO_DEBUG_ADDFIND
                            if( Index2 == 1 )
                                { Spew( L"AddFind 1 at wordsex 1226" ); }
                        #endif

                        MyFree( 2833, zx, MallocPtr );
                        MallocPtr = NULL;

                        // Just in case this is a repeat, try to get fruit.
                        COneQuery * pFruit = ( COneQuery * ) CSolUserQuerys.GetUserpVoid( Index );
                        // If not, make a new fruit to hang on the tree.
                        if( pFruit == NULL )
                        {
                            pFruit = new COneQuery( Index );
                            CSolUserQuerys.SetUserpVoid( Index, pFruit );
                        }
                        // Whether a repeat or new, copy current
                        // user preferences to this fruit object.
                        pFruit->m_QueryDiligence = g_QueryDiligence;
                        pFruit->m_NonTextLinks = g_NonTextLinks;
                        // This is okay, in main thread, before query thread:
                        Top.Add( pFruit ); // Put fruit on display
                        Initiator.InitiateQuery( Index );
                    }
                }

                if( OkayToEnd )
                {
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
                else
                {
                    SetFocus( hwCombo ); // Let user keep trying.
                }
            }
            return TRUE;
        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;

    case WM_NOTIFY:
        {
            int idCtrl = ( int ) wParam;
            // This is most indirect!
            LPNMHDR pnmh = ( LPNMHDR ) lParam; // subset
            HWND hwndFrom = pnmh->hwndFrom;
            UINT idFrom   = pnmh->idFrom;
            UINT code     = pnmh->code;

            if( code == UDN_DELTAPOS
            && idFrom == IDC_SPIN1 )
            {
                LPNMUPDOWN lpnmud = ( LPNMUPDOWN ) lParam; // superset
                int CurrentPos = lpnmud->iPos;
                int ProposedChange = lpnmud->iDelta;
                int NewValue = CurrentPos + ProposedChange;

                // Having set range, it is within limits.
                // No, I actually get notifications beyond set limits.

                int Rejection = 1;
                if( NewValue <= MAX_DILIGENCE
                && NewValue >= MIN_DILIGENCE )
                {
                    g_QueryDiligence = NewValue;
                    Rejection = 0;
                }
                // Zero accepts the change -- but wait, there's WAY more!
                // If the message handler is in a dialog box procedure,
                // you must use SetWindowLong function with DWL_MSGRESULT
                // to set a return value. Return TRUE directly afterwards.
                //
                // Help says use SWLPtr for win32/64, but LONG_PTR is undef.
                //
                // When I passed the hwnd of the spin control, it vanished.
                // When I passed the dialog hwnd, it modulates the change.
                //
                // To determine success or failure, clear the last error
                // by calling SetLastError( 0 ), then call SetWindowLongPtr.
                // Function failure will be indicated by a return value of
                // zero and a GetLastError result that is nonzero.
                SetLastError( 0 );
                long old = SetWindowLong(
                    hDlg, // handle to window
                    DWL_MSGRESULT, // offset of value to set
                    Rejection // new value
                );
                int enz = GetLastError( );
                if( old == 0
                && enz != 0 )
                    ProgramError( L"SetWindowLong" );
                return TRUE;
            }
            break;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK FetchDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"343" );
    #endif
    InDispatch ++;
    LRESULT x = FetchDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT FetchDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"344" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            // Set the sorted list item into the combo box.
            HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

            CoIt * pMalVector = CSolUserFetchs.GetSortedVector( CSOL_FORWARD );
            if( pMalVector != NULL )
            {
                size_t take = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector + take++;
                    if( pCoIt->IsSentinel )
                        break;
                    if( PopulateComboBox( hwCombo, pCoIt ) != 0 )
                        break;
                }
                MyFree( 2992, UNPREDICTABLE, pMalVector );
                pMalVector = NULL;
            }

            if( g_ClickTextToInitDialogs != NULL )
                SetWindowText( hwCombo, g_ClickTextToInitDialogs );
        }
        {
            // Set the extern values into the dialog controls.
            // Set the several checkboxes
            HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
            SendMessage( hwChk1, BM_SETCHECK, ( WPARAM ) ( g_FetchAllLinks ? BST_CHECKED : BST_UNCHECKED ), 0 );
#ifndef _WIN32_WCE
            HWND hwChk2 = GetDlgItem( hDlg, IDC_CHECK2 );
            SendMessage( hwChk2, BM_SETCHECK, ( WPARAM ) ( g_NonTextLinks ? BST_CHECKED : BST_UNCHECKED ), 0 );
#endif // not _WIN32_WCE

        }
        return TRUE;

    case WM_COMMAND:
        // This is the FetchDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDC_CHECK1:
            {
                HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
                if( SendMessage( hwChk1, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
                    g_FetchAllLinks = 1;
                else
                    g_FetchAllLinks = 0;
            }
            return TRUE;

#ifndef _WIN32_WCE
        case IDC_CHECK2:
            {
                HWND hwChk2 = GetDlgItem( hDlg, IDC_CHECK2 );
                if( SendMessage( hwChk2, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
                    g_NonTextLinks = 1;
                else
                    g_NonTextLinks = 0;
            }
#endif // not _WIN32_WCE
            return TRUE;

        case IDC_COMBO1:
            if ( HIWORD( wParam ) != CBN_DBLCLK )
                break;

            // For a double-click, process the OK case.

        case IDC_BUTTON1:
            {
                int retn = 0; // a change to -1 means do not exit dialog

                // Save any user text now in IDC_COMBO1
                HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );
                size_t nLength = GetWindowTextLength( hwCombo );
                if( nLength != 0 )
                {
                    // Add 7 in case I must insert "http://"
                    size_t nMallocLen = nLength + 7 + 1;
                    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 1295, nMallocLen * sizeof( wchar_t ) );

                    // Allow 7 in case I must insert "http://"
                    GetWindowText( hwCombo, MallocPtr + 7, nMallocLen );

                    wchar_t * PassArgument = MallocPtr + 7;
                    if( wcsstr( MallocPtr + 7, L"://" ) == NULL )
                    {
                        PassArgument = MallocPtr;
                        memcpy( PassArgument, L"http://", 7 * sizeof( wchar_t ) );
                    }
                    wchar_t * NewMallocPtr = NULL;

                    // returns 1=success, 0=failure, -1=fail but stay in dialog.
                    retn = Www.FixUpUserUrl( & NewMallocPtr, PassArgument );


                    // Note: A clone of this code path exists
                    // over in CTop.cpp AddLinks routine too.


                    if( retn == 1
                    && NewMallocPtr != NULL )
                    {
                        size_t Index = CSolUserFetchs.AddKey( NewMallocPtr );
                        #if DO_DEBUG_ADDFIND
                            if( Index == 1 )
                                { Spew( L"AddFind 1 at wordsex 1477" ); }
                        #endif

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
                        pFruit->m_FetchAllLinks = g_FetchAllLinks;
                        pFruit->m_NonTextLinks = g_NonTextLinks;

                        // Prompt for a directory to receive non-texts?
                        // No, I will let CINI InitiateFetch do it JIT.

                        // This is okay, in main thread, before fetch thread:
                        Top.Add( pFruit ); // Put fruit on display
                        Initiator.InitiateFetch( Index );
                    }
                    if( MallocPtr != NULL )
                    {
                        MyFree( 3115, zx, MallocPtr );
                        MallocPtr = NULL;
                    }
                    if( NewMallocPtr != NULL )
                    {
                        MyFree( 3120, zx, NewMallocPtr );
                        NewMallocPtr = NULL;
                    }
                }
                if( retn == -1 )
                {
                    SetFocus( hwCombo ); // Let user keep trying.
                }
                else
                {
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
            }
            return TRUE;

        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK KwicDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"345" );
    #endif
    InDispatch ++;
    LRESULT x = KwicDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT KwicDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"346" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            // Set the sorted list item into the combo box.
            HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

            CoIt * pMalVector = CSolUserKwics.GetSortedVector( CSOL_FORWARD );
            if( pMalVector != NULL )
            {
                size_t take = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector + take++;
                    if( pCoIt->IsSentinel )
                        break;
                    if( PopulateComboBox( hwCombo, pCoIt ) != 0 )
                        break;
                }
                MyFree( 3216, UNPREDICTABLE, pMalVector );
                pMalVector = NULL;
            }

            if( g_ClickTextToInitDialogs != NULL )
                SetWindowText( hwCombo, g_ClickTextToInitDialogs );
        }
        {
            // Set the extern values into the dialog controls.
            HWND hwRadio = 0;
            switch( g_Kwic1_Sentc2_Parag3 )
            {
                case 1: hwRadio = GetDlgItem( hDlg, IDC_RADIO1 ); break;
                case 2: hwRadio = GetDlgItem( hDlg, IDC_RADIO2 ); break;
                case 3: hwRadio = GetDlgItem( hDlg, IDC_RADIO3 ); break;
            }
            SendMessage( hwRadio, BM_SETCHECK, ( WPARAM ) BST_CHECKED, 0 );

            // Set the checkbox
            HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
            SendMessage( hwChk1, BM_SETCHECK, ( WPARAM ) ( g_Stem ? BST_CHECKED : BST_UNCHECKED ), 0 );

        }
        return TRUE;

    case WM_COMMAND:
        // This is the KwicDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDC_RADIO1:
            g_Kwic1_Sentc2_Parag3 = 1;
            return TRUE;

        case IDC_RADIO2:
            g_Kwic1_Sentc2_Parag3 = 2;
            return TRUE;

        case IDC_RADIO3:
            g_Kwic1_Sentc2_Parag3 = 3;
            return TRUE;

        case IDC_CHECK1:
            {
                HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
                if( SendMessage( hwChk1, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
                    g_Stem = 1;
                else
                    g_Stem = 0;
            }
            return TRUE;

        case IDC_COMBO1:
            if ( HIWORD( wParam ) != CBN_DBLCLK )
                break;

            // For a double-click, process the OK case.

        case IDC_BUTTON1:
            {
                int OkayToEnd = 0;

                // Save any user text now in IDC_COMBO1
                HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );
                size_t nLength = GetWindowTextLength( hwCombo );
                if( nLength != 0 )
                {
                    OkayToEnd = 1;
                    size_t nMallocLen = nLength + 1;
                    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 1551, nMallocLen * sizeof( wchar_t ) );
                    GetWindowText( hwCombo, MallocPtr, nMallocLen );
                    size_t Index = CSolUserKwics.AddKey( MallocPtr );
                    #if DO_DEBUG_ADDFIND
                        if( Index == 1 )
                            { Spew( L"AddFind 1 at wordsex 1708" ); }
                    #endif
                    MyFree( 3288, zx, MallocPtr );
                    MallocPtr = NULL;

                    COneKwic * pFruit = ( COneKwic * ) CSolUserKwics.GetUserpVoid( Index );
                    // If not, make a new fruit to hang on the tree.
                    if( pFruit == NULL )
                    {
                        pFruit = new COneKwic( Index );
                        CSolUserKwics.SetUserpVoid( Index, pFruit );
                    }
                    pFruit->m_Kwic1_Sentc2_Parag3 = g_Kwic1_Sentc2_Parag3;
                    pFruit->m_Stem = g_Stem;
                    // This is okay, in main thread, before Words thread:
                    Top.Add( pFruit ); // Put fruit on display
                    Initiator.InitiateKwic( Index );
                }

                if( OkayToEnd )
                {
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
                else
                {
                    SetFocus( hwCombo ); // Let user keep trying.
                }
            }
            return TRUE;
        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK PhrasesDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"347" );
    #endif
    InDispatch ++;
    LRESULT x = PhrasesDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT PhrasesDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"348" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            // Edit 2 is top read-only label. Show sentence count.
            HWND hwEdit = GetDlgItem( hDlg, IDC_EDIT2 );
            wchar_t wk[100];
            wsprintf( wk, L"There are %2d phrases indexed.", TotalPhraseCount );
            SetWindowText( hwEdit, wk );
        }
        {
            // Set the extern g_PhrasesPercent range 1 to 100.
            // Set the extern values into the dialog controls.
            HWND hwSpin = GetDlgItem( hDlg, IDC_SPIN1 );
            short nUpper = 100;
            short nLower = 1;
            SendMessage( hwSpin, UDM_SETRANGE, 0, ( LPARAM ) MAKELONG( nUpper, nLower ) );
            short nPosition = g_PhrasesPercent;
            SendMessage( hwSpin, UDM_SETPOS, 0, ( LPARAM ) MAKELONG( ( short ) nPosition, 0 ) );
        }
        return TRUE;

    case WM_COMMAND:
        // This is the PhrasesDlgProc...
        switch ( LOWORD( wParam ) )
        {

        case IDC_BUTTON1:
            // Hey! The "buddy" spin does not reflect changes
            // if the user directly edits the text in IDC_EDIT1.
            // Get the text and do an ATOI on it before acting.
            {
                int OkayToEnd = 0;
                wchar_t wk[ 10 ]; // It's just 1-3 digits ( 1-100% ).
                wk[ 0 ] = NULL; // Just in case.
                HWND hwEdit = GetDlgItem( hDlg, IDC_EDIT1 );
                GetWindowText( hwEdit, wk, 10 );
                wchar_t * scan = wk;
                size_t accu = 0;
                for( ;; )
                {
                    if( ! iswdigit( *scan ) )
                        break;
                        accu *= 10;
                        accu += *scan - '0';
                    scan++;
                }
                g_PhrasesPercent = accu;

                // Allow full spin range: 1 to 100 inclusive.
                if( g_PhrasesPercent >= 1
                && g_PhrasesPercent <= 100 )
                    OkayToEnd = 1;

                if( OkayToEnd )
                {
                    Initiator.InitiatePhrases(  );
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
                else
                {
                    SetFocus( hwEdit ); // Let user keep trying.
                }
            }
            return TRUE;

        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;

        }
        break;

    case WM_NOTIFY:
        {
            int idCtrl = ( int ) wParam;
            // This is most indirect!
            LPNMHDR pnmh = ( LPNMHDR ) lParam; // subset
            HWND hwndFrom = pnmh->hwndFrom;
            UINT idFrom   = pnmh->idFrom;
            UINT code     = pnmh->code;

            if( code == UDN_DELTAPOS
            && idFrom == IDC_SPIN1 )
            {
                LPNMUPDOWN lpnmud = ( LPNMUPDOWN ) lParam; // superset
                int CurrentPos = lpnmud->iPos;
                int ProposedChange = lpnmud->iDelta;
                int NewValue = CurrentPos + ProposedChange;

                // Having set range, it is within limits.
                // No, I actually get notifications beyond set limits.

                int Rejection = 1;
                if( NewValue <= 100
                && NewValue >= 1 )
                {
                    g_PhrasesPercent = NewValue;
                    Rejection = 0;
                }
                // Zero accepts the change -- but wait, there's WAY more!
                // If the message handler is in a dialog box procedure,
                // you must use SetWindowLong function with DWL_MSGRESULT
                // to set a return value. Return TRUE directly afterwards.
                //
                // Help says use SWLPtr for win32/64, but LONG_PTR is undef.
                //
                // When I passed the hwnd of the spin control, it vanished.
                // When I passed the dialog hwnd, it modulates the change.
                //
                // To determine success or failure, clear the last error
                // by calling SetLastError( 0 ), then call SetWindowLongPtr.
                // Function failure will be indicated by a return value of
                // zero and a GetLastError result that is nonzero.
                SetLastError( 0 );
                long old = SetWindowLong(
                    hDlg, // handle to window
                    DWL_MSGRESULT, // offset of value to set
                    Rejection // new value
                );
                int enz = GetLastError( );
                if( old == 0
                && enz != 0 )
                    ProgramError( L"SetWindowLong" );
                return TRUE;
            }
            break;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK StatusDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"349" );
    #endif
    InDispatch ++;
    LRESULT x = StatusDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT StatusDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"350" );
    #endif
    switch ( message )
    {
    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        g_hWndStatusDialog = hDlg; // let me shine when visible, else NULL

        // Same text formatting code for WM_INITDIALOG and WM_THREADCHANGE:
        {
            HWND hwEdit = GetDlgItem( hDlg, IDC_EDIT1 );
            wchar_t wk[300];
            wsprintf( wk, L"Threads:\r\n"
                    L"%2d Add Cache \r\n"
                    L"%2d Add Directory \r\n"
                    L"%2d Add Web Page, Links \r\n"
                    L"%2d Add Web Search \r\n"
                    L"%2d Add Words\r\n"
                    L"%2d Add Phrases\r\n",
                g_nCountOfCacheThreads,
                g_nCountOfFolderThreads,
                g_nCountOfFetchThreads,
                g_nCountOfQueryThreads,
                g_nCountOfKwicThreads,
                g_nCountOfPhrasesThreads );
            SetWindowText( hwEdit, wk );
        }
        return TRUE;

    case WM_COMMAND:
        // This is the StatusDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDOK: // IDOK is the top right circled ( X ) on PPC
        // IDOK is also for OKAY/CLOSE button on About, Status, Stores.
        case IDCANCEL:
            g_hWndStatusDialog = NULL; // when visible, else NULL
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        case IDC_BUTTON1:
            g_bStopAllThreads = 1;
            // But not my init thread... g_bStopInitThread
            return TRUE;
        }
        break;
    case WM_THREADCHANGE:
        // Same text formatting code for WM_INITDIALOG and WM_THREADCHANGE:
        {
            HWND hwEdit = GetDlgItem( hDlg, IDC_EDIT1 );
            wchar_t wk[300];
            wsprintf( wk, L"Threads:\r\n"
                    L"%2d Add Cache \r\n"
                    L"%2d Add Directory \r\n"
                    L"%2d Add Web Page, Links \r\n"
                    L"%2d Add Web Search \r\n"
                    L"%2d Add Words\r\n"
                    L"%2d Add Phrases\r\n",
                g_nCountOfCacheThreads,
                g_nCountOfFolderThreads,
                g_nCountOfFetchThreads,
                g_nCountOfQueryThreads,
                g_nCountOfKwicThreads,
                g_nCountOfPhrasesThreads );
            SetWindowText( hwEdit, wk );

            if( g_bStopAllThreads
            && g_nCountOfCacheThreads == 0
            && g_nCountOfQueryThreads == 0
            && g_nCountOfFetchThreads == 0
            && g_nCountOfFolderThreads == 0
            && g_nCountOfKwicThreads == 0 )
            {
                // When all threads die, set focus on okay button again.
                // IDOK is also for OKAY/CLOSE button on About, Status, Stores.
                HWND hwOk = GetDlgItem( hDlg, IDOK ); // BTN in status dialog
                SetFocus( hwOk );
                g_bStopAllThreads = 0; // Done, and waited on them all.
                // But not my init thread... g_bStopInitThread
            }
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK StoresDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"351" );
    #endif
    InDispatch ++;
    LRESULT x = StoresDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT StoresDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"352" );
    #endif
    switch ( message )
    {
    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            HWND hwEdit = GetDlgItem( hDlg, IDC_EDIT1 );

            // Take a naming clue from the finished page annotations:
            // 0 score, 0 phrases, 42731 words, 246 kb, 2883 terms, 0 links.

            wchar_t wk[600];
            wsprintf( wk,

                L" %2d Search engines \r\n"
                L" %2d Html forms \r\n"
                L" %2d Mb of memory \r\n"
                L" %2d URL addresses \r\n"
                L" %2d web pages \r\n"
                L" %2d languages \r\n"
                L" %2d phrases \r\n"
                L" %2d words \r\n"
                L" %2d vocabulary \r\n"

                    ,                   // ---

                CSolSearchUrls.nList - 2,       // Search engines
                CSolFormUrls.nList - 2,         // Html forms
                TotalMallocBytes / 1000000,     // Mb of memory
                CSolAllUrls.nList - 2,          // URL addresses
                TotalPageCount,                 // web pages
                TotalLanguageCount,             // languages
                TotalPhraseCount,               // phrases
                TotalWordCount,                 // words
                CSolAllWords.nList - 2 );       // vocabulary

            SetWindowText( hwEdit, wk );
        }
        return TRUE;

    case WM_COMMAND:
        // This is the StoresDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDOK: // IDOK is the top right circled ( X ) on PPC
        // IDOK is also for OKAY/CLOSE button on About, Status, Stores.
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK AddFolderDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"353" );
    #endif
    InDispatch ++;
    LRESULT x = AddFolderDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT AddFolderDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    // I tripled down the Add, Save and Use flavor dialogs.
    // Add omits progress bar, and its checkbox=Recursive.

    #if DO_DEBUG_CALLS
        Routine( L"354" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            // Set extern value into the checkbox
            HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
            SendMessage( hwChk1, BM_SETCHECK, ( WPARAM ) ( g_bAddRecursively ? BST_CHECKED : BST_UNCHECKED ), 0 );
        }
        {

            // Ensure that DefaultFolderFilename is in the list.
            size_t Index = CSolUserFolders.AddKey( DefaultFolderFilename );
            #if DO_DEBUG_ADDFIND
                if( Index == 1 )
                    { Spew( L"AddFind 1 at wordsex 1968" ); }
            #endif

            // Set the sorted list item into the combo box.
            HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

            CoIt * pMalVector = CSolUserFolders.GetSortedVector( CSOL_FORWARD );
            if( pMalVector != NULL )
            {
                size_t take = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector + take++;
                    if( pCoIt->IsSentinel )
                        break;
                    if( PopulateComboBox( hwCombo, pCoIt ) != 0 )
                        break;
                }
                MyFree( 3794, UNPREDICTABLE, pMalVector );
                pMalVector = NULL;
            }

            // Put that DefaultFolderFilename right in view.
            SetWindowText( hwCombo, DefaultFolderFilename );

        }
        return TRUE;

    case WM_COMMAND:
        // This is the AddFolderDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDC_CHECK1:
            {
                HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
                if( SendMessage( hwChk1, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
                    g_bAddRecursively = 1;
                else
                    g_bAddRecursively = 0;
            }
            return TRUE;

        case IDC_COMBO1:
            if ( HIWORD( wParam ) != CBN_DBLCLK )
                break;

            // For a double-click, process the OK case.

        case IDC_BUTTON1:
            {
                int OkayToEnd = 0;
                // Save any user text now in IDC_COMBO1
                HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );
                size_t nLength = GetWindowTextLength( hwCombo );
                if( nLength != 0 )
                {
                    size_t nMallocLen = nLength + 1;
                    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 1860, nMallocLen * sizeof( wchar_t ) );

                    GetWindowText( hwCombo, MallocPtr, nMallocLen );

                    // For _ADD_ ( not save, use ) path must preexist:

                    if( Fio.FolderPathIsValid( MallocPtr ) )
                    {
                        OkayToEnd = 1;

                        size_t Index = CSolUserFolders.AddKey( MallocPtr );
                        #if DO_DEBUG_ADDFIND
                            if( Index == 1 )
                                { Spew( L"AddFind 1 at wordsex 2042" ); }
                        #endif

                        // Only the _ADD_ version has a pFruit and a thread.

                        // Just in case this is a repeat, try to get fruit.
                        COneFolder * pFruit = ( COneFolder * ) CSolUserFolders.GetUserpVoid( Index );
                        // If not, make a new fruit to hang on the tree.
                        if( pFruit == NULL )
                        {
                            pFruit = new COneFolder( Index );
                            CSolUserFolders.SetUserpVoid( Index, pFruit );

                        }
                        // OneFolder Fruit has no parameters to set:
                        // Now it does:
                        pFruit->m_bAddRecursively = g_bAddRecursively;

                        // This is okay, in main thread, before AddFolder thread:
                        Top.Add( pFruit ); // Put fruit on display

                        // Add folder uses a thread.
                        Initiator.InitiateFolder( Index );

                    }

                    // A free above and a free below is just too free!
                    MyFree( 3932, zx, MallocPtr );
                    MallocPtr = NULL;
                }
                if( OkayToEnd )
                {
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
                else
                {
                    SetFocus( hwCombo ); // Let user keep trying.
                }
            }
            return TRUE;

        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK SaveFolderDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"353" );
    #endif
    InDispatch ++;
    LRESULT x = SaveFolderDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT SaveFolderDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    // I tripled down the Add, Save and Use flavor dialogs.
    // Save has progress bar, and checkbox=subfolders per language.

    #if DO_DEBUG_CALLS
        Routine( L"354" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {
            // Initialize the progress bar:
            HWND hwProgress = GetDlgItem( hDlg, IDC_PROGRESS1 );
            // I think this represents fetched AND unfetched URLs...
            short nUpper = CSolAllUrls.nList;
            short nLower = 0;
            SendMessage( hwProgress, PBM_SETRANGE, 0, ( LPARAM ) MAKELONG( nLower, nUpper ) );
            short nPosition = 0;
            SendMessage( hwProgress, PBM_SETPOS, ( WPARAM ) MAKELONG( ( short ) nPosition, 0 ), 0 );
            g_hWndSaveProgressBar = hwProgress; // let me shine when SaveFolder dialog is visible, else NULL
        }
        {
            HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
            SendMessage( hwChk1, BM_SETCHECK, ( WPARAM ) ( g_bCreateSubfolders ? BST_CHECKED : BST_UNCHECKED ), 0 );
        }
        {

            // Ensure that DefaultFolderFilename is in the list.
            size_t Index = CSolUserFolders.AddKey( DefaultFolderFilename );
            #if DO_DEBUG_ADDFIND
                if( Index == 1 )
                    { Spew( L"AddFind 1 at wordsex 1968" ); }
            #endif

            // Set the sorted list item into the combo box.
            HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

            CoIt * pMalVector = CSolUserFolders.GetSortedVector( CSOL_FORWARD );
            if( pMalVector != NULL )
            {
                size_t take = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector + take++;
                    if( pCoIt->IsSentinel )
                        break;
                    if( PopulateComboBox( hwCombo, pCoIt ) != 0 )
                        break;
                }
                MyFree( 3794, UNPREDICTABLE, pMalVector );
                pMalVector = NULL;
            }

            // Put that DefaultFolderFilename right in view.
            SetWindowText( hwCombo, DefaultFolderFilename );

        }
        // huh? not in save folder... WsbTempPathToSaveNonText.Reset( );
        return TRUE;

    case WM_COMMAND:
        // This is the SaveFolderDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDC_CHECK1:
            {
                HWND hwChk1 = GetDlgItem( hDlg, IDC_CHECK1 );
                if( SendMessage( hwChk1, BM_GETCHECK, 0, 0 ) == BST_CHECKED )
                    g_bCreateSubfolders = 1;
                else
                    g_bCreateSubfolders = 0;
            }
            return TRUE;

        case IDC_COMBO1:
            if ( HIWORD( wParam ) != CBN_DBLCLK )
                break;

            // For a double-click, process the OK case.

        case IDC_BUTTON1:
            {
                int OkayToEnd = 0;
                // Save any user text now in IDC_COMBO1
                HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );
                size_t nLength = GetWindowTextLength( hwCombo );
                if( nLength != 0 )
                {
                    size_t nMallocLen = nLength + 1;
                    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 1860, nMallocLen * sizeof( wchar_t ) );

                    GetWindowText( hwCombo, MallocPtr, nMallocLen );

                    if( Fio.MakeFoldersPaths( MallocPtr ) )
                    {
                        OkayToEnd = 1;

                        size_t Index = CSolUserFolders.AddKey( MallocPtr );
                        #if DO_DEBUG_ADDFIND
                            if( Index == 1 )
                                { Spew( L"AddFind 1 at wordsex 2042" ); }
                        #endif


                        // Saving folder does not use a thread.
                        Fio.SaveFolder( MallocPtr );

                    }

                    // A free above and a free below is just too free!
                    MyFree( 3932, zx, MallocPtr );
                    MallocPtr = NULL;
                }
                if( OkayToEnd )
                {
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
                else
                {
                    SetFocus( hwCombo ); // Let user keep trying.
                }
            }
            return TRUE;

        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            g_hWndSaveProgressBar = NULL; // when SaveFolder dialog is visible, else NULL
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}


LRESULT CALLBACK UseFolderDlgProc1( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"353" );
    #endif
    InDispatch ++;
    LRESULT x = UseFolderDlgProc2( hDlg, message, wParam, lParam );
    InDispatch --;
    return x;
}

LRESULT UseFolderDlgProc2( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    // I tripled down the Add, Save and Use flavor dialogs.
    // 'Use' version omits progress bar, and omits checkbox.

    #if DO_DEBUG_CALLS
        Routine( L"354" );
    #endif
    switch ( message )
    {
#ifdef _WIN32_WCE
    case WM_SETFOCUS:
        SHSipPreference( hDlg, SIP_UP );
        break;
    case WM_KILLFOCUS:
        SHSipPreference( hDlg, SIP_DOWN );
        break;
#endif

    case WM_INITDIALOG:
#ifdef _WIN32_WCE
        {
            SHINITDLGINFO shidi;
            shidi.dwMask = SHIDIM_FLAGS;
            shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG;
            shidi.hDlg = hDlg;
            SHInitDialog( &shidi );
        }
        SHSipPreference( hDlg, SIP_UP );
#else
        {
            HWND hwndOwner = GetParent( hDlg );
            if ( hwndOwner == NULL )
                hwndOwner = GetDesktopWindow( );
            RECT rc, rcDlg, rcOwner;
            GetWindowRect( hwndOwner, &rcOwner );
            GetWindowRect( hDlg, &rcDlg );
            CopyRect( &rc, &rcOwner );
            OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
            OffsetRect( &rc, -rc.left, -rc.top );
            OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );
            SetWindowPos( hDlg, HWND_TOP,
                rcOwner.left + ( rc.right / 2 ),
                rcOwner.top + ( rc.bottom / 2 ),
                0, 0, SWP_NOSIZE );
        }
#endif
        {

            // Ensure that DefaultFolderFilename is in the list.
            size_t Index = CSolUserFolders.AddKey( DefaultFolderFilename );
            #if DO_DEBUG_ADDFIND
                if( Index == 1 )
                    { Spew( L"AddFind 1 at wordsex 1968" ); }
            #endif

            // Set the sorted list item into the combo box.
            HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );

            CoIt * pMalVector = CSolUserFolders.GetSortedVector( CSOL_FORWARD );
            if( pMalVector != NULL )
            {
                size_t take = 0;
                for( ;; )
                {
                    CoIt * pCoIt = pMalVector + take++;
                    if( pCoIt->IsSentinel )
                        break;
                    if( PopulateComboBox( hwCombo, pCoIt ) != 0 )
                        break;
                }
                MyFree( 3794, UNPREDICTABLE, pMalVector );
                pMalVector = NULL;
            }

            // Put that DefaultFolderFilename right in view.
            SetWindowText( hwCombo, DefaultFolderFilename );

        }
        WsbTempPathToSaveNonText.Reset( );
        return TRUE;

    case WM_COMMAND:
        // This is the UseFolderDlgProc...
        switch ( LOWORD( wParam ) )
        {
        case IDC_COMBO1:
            if ( HIWORD( wParam ) != CBN_DBLCLK )
                break;

            // For a double-click, process the OK case.

        case IDC_BUTTON1:
            {
                int OkayToEnd = 0;
                // Save any user text now in IDC_COMBO1
                HWND hwCombo = GetDlgItem( hDlg, IDC_COMBO1 );
                size_t nLength = GetWindowTextLength( hwCombo );
                if( nLength != 0 )
                {
                    size_t nMallocLen = nLength + 1;
                    wchar_t * MallocPtr = ( wchar_t * ) MyMalloc( 1860, nMallocLen * sizeof( wchar_t ) );
                    GetWindowText( hwCombo, MallocPtr, nMallocLen );
                    if( Fio.MakeFoldersPaths( MallocPtr ) )
                    {
                        OkayToEnd = 1;

                        size_t Index = CSolUserFolders.AddKey( MallocPtr );
                        #if DO_DEBUG_ADDFIND
                            if( Index == 1 )
                                { Spew( L"AddFind 1 at wordsex 2042" ); }
                        #endif

                        // All _USE_ needs to do is record that path...

                        #if DO_DEBUG_BINARIES
                            ; Spew( L"UseFolderDlg...Path:" );
                            ; Spew( ( MallocPtr == NULL ) ? L"-null-" : MallocPtr );
                        #endif

                        // This was always reset on the way in...
                        WsbTempPathToSaveNonText.Add( MallocPtr );
                    }

                    // A free above and a free below is just too free!
                    MyFree( 3932, zx, MallocPtr );
                    MallocPtr = NULL;
                }
                if( OkayToEnd )
                {
                    EndDialog( hDlg, LOWORD( wParam ) );
                }
                else
                {
                    SetFocus( hwCombo ); // Let user keep trying.
                }
            }
            return TRUE;

        case IDOK: // IDOK is the top right circled ( X ) on PPC
        case IDCANCEL:
            EndDialog( hDlg, LOWORD( wParam ) );
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void AskForNonTextDirectory( )
{
    #if DO_DEBUG_CALLS
        Routine( L"355" );
    #endif
    // It is easier to hook up in WordsEx.cpp than in CIni.cpp
    // Dialog may leave a path in WsbTempPathToSaveNonText
    // in which case all folders were created, ready to use.
    // If user canceled, WsbTempPathToSaveNonText.StrLen == 0.
    DialogBox( g_hInst, ( LPCTSTR )IDD_USE_FOLDER_DIALOG, g_hWnd, ( DLGPROC )UseFolderDlgProc1 );
}

#ifndef _WIN32_WCE
void SetUpFindDialog( )
{
    // To use the Find or Replace dialog box, you must perform three separate tasks:
    // 1. Get a message identifier for the FINDMSGSTRING registered message.
    // 2. Display the dialog box.
    // 3. Process FINDMSGSTRING messages when the dialog box is open.

    // To display a Find dialog box, first initialize a FINDREPLACE structure
    // and then call the FindText function. Note that the FINDREPLACE structure
    // and the buffer for the search string should be a global or static variable
    // so it does not go out of scope before the dialog box closes.
    // You must set the hwndOwner member to specify the window that receives
    // the registered messages. After you create the dialog box, you can move
    // or manipulate it by using the returned handle.

    // If the FindText or ReplaceText function successfully creates the dialog box,
    // it returns a handle to the dialog box. You can use this handle to move
    // and communicate with the dialog box.
    // If the function cannot create the dialog box, it returns NULL.
    // You can determine the cause of an error by calling the CommDlgExtendedError
    // function to retrieve the extended error value.

    // I moved the init of FINDREPLACE structure
    // out to InitInstance, so flags should stay.

    // I find that second invocation of dialog
    // isn't passing messages back to WndProc.

    // Maybe I have to re-init g_fr each time.
    // In which case, I'll have to save flags.
    // Yes. That solved a stack overflow pblm.

    // Initialize FINDREPLACE -- again.
    memset( & g_fr, 0, sizeof( FINDREPLACE ) );
    g_fr.lStructSize = sizeof( FINDREPLACE );
    g_fr.hwndOwner = g_hWnd; // what window receives messages
    g_fr.lpstrFindWhat = g_szFindWhat; // must be global/static
    g_fr.wFindWhatLen = 80;
    g_fr.Flags = 0;
    if( g_fr_Flags_Down )
        g_fr.Flags |= FR_DOWN;
    if( g_fr_Flags_Case )
        g_fr.Flags |= FR_MATCHCASE;
    if( g_fr_Flags_Word )
        g_fr.Flags |= FR_WHOLEWORD;

    // This will set up the modeless dialog box.
    // Elsewhere, I must null handle at closure.
    g_hWndCommonFindDialog = FindText( & g_fr );
    if( g_hWndCommonFindDialog == NULL )
    {
        ProgramError( L"FindText returned NULL hWnd" );
    }
    See.PauseAnySmoothScrolling( );
}
#endif // not _WIN32_WCE

