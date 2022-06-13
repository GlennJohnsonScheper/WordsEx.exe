// This is file: CURL.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// CUrl.cpp holds the default list of Search Engine URLs + RULEs + NOTs.

    // reconsider 3: good in clamourous descent:

    // "GET 200 http://search.about.com/fullsearch.htm?TopNode=/&terms=\r\n"
    // "  good url has prior TAG div ATTR clear VALUE \"res\"\r\n"
    // "  more url has anchortext NUMBER\r\n"
    // "GET 210 http://a9.com/?submit=&q=\r\n"
    // "  good url has prior TAG h2\r\n"
    // "  more url has anchortext \"next\"\r\n"
    // "GET 480 http://www.findarticles.com/p/search?tb=art&qt=\r\n"
    // "  good url has next TAG div ATTR clear VALUE \"sOpt\"\r\n"
    // "  more url has anchortext NUMBER\r\n"


    // Since fixing JoinAndHangUrl routine,
    // several queries produced many spam links,
    // especially, SEARCHFORIT spewed much spam,
    // including a link that hangs up on fetch!
    // So I commented out several for now:
    //
    // "GET 530 http://www.searchforit.com/results.html?aff_id=&cat=&searchbutton=&keywords=\r\n"
    // "  good url has anchor TAG a ATTR clear VALUE \"resultlink\"\r\n"
    // "  more url has anchortext NUMBER\r\n"
    //
    // It could be I had too many search tokens at the time.
    // BBC.CO.UK churned out too many articles, on this query:
    // trumpets thunders lightnings voices
    // "GET 430 http://www.bbc.co.uk/cgi-bin/search/results.pl?go=&uri=/home/today/index.live.shtml&q=\r\n"
    // "  good url has next TAG /h3\r\n"
    // "  more url has anchortext NUMBER\r\n"
    //
    // WN.COM produce no valid hits on it, but 65 irrelevant URLS:
    // "GET 490 http://upge.wn.com/?template=cheetah-search%2Findex.txt&language_id=1&query=\r\n"
    // "  good url has anchor PATH IN \"/view\"\r\n"


// notes: nextag.com is out!

// pure spam: http://search.looksmart.com/p/search?tb=web&qt=
// pure spam: http://www.overture.com/d/search/?type=topbar&mkt=us&lang=en_US&sbuttonT=&Keywords=
// pure spam: http://pesquisa.clix.pt/resultado.html?question=

// parse mess: http://www.monstercrawler.com/beta-bin/nph-beta.pl?qry=

// too slow: http://www.rnews.com/search.cfm?keyword=
// too slow: http://www.metacrawler.com/crawler?method=0&region=&rpp=20&timeout=10&hpe=10&general=
// too slow: http://www.search-engines-links.com/metasearch.cgi?keywords=
// too slow: http://ixquick.com/do/metasearch.pl?cat=web&cmd=process_search&language=english&query=
// too slow: http://asp.usatoday.com/search/search.aspx?source=both&submitSearch=&q=
// too slow: http://www.cherchons.com/cgi/cgi.cgi?action=cherchons&start=0&cherchons=&toSearch=
// too slow: http://search.internet.com/query.php?IC_QueryText=
// too slow: http://catalogo.cerca.com/odpSearch/?cerca=&where=all&charset=utf-8&qs=
// too slow: http://cnn.netscape.cnn.com/ns/news/photosearch.jsp?searchType=photos&floc=ne-main-5-l1&cap=
// too slow: http://www.gigablast.com/search?n=100&q=
// too slow: http://r.voila.fr/se?sev=2&ref=V_BOX_essentiel&db=web&dblg=fr&ctx=voila&lg=fr&dt=*&kw=
// too slow: http://www.mp3mama.com/mp3search/metasearch.cgi?keywords=

// out until study: http://www.voltairenet.org/spip.php?lang=en&page=recherche&id_rubrique=0&recherche=
// out until study: http://websearch.cs.com/cs/search?fromPage=cscom&uType=5049181&query=
// out until study: http://images.google.com/images?hl=en&lr=&sa=N&tab=wi&q=

// spam unless follow: http://msnbc.msn.com/?id=3053419&searchfromtoc=&action=fulltext&querytext=

// out duplicate: http://fr.altavista.com/web/results?itag=wrx&kgs=1&kls=0&q=
// out duplicate: http://de.altavista.com/web/results?itag=wrx&kgs=1&kls=0&q=
// out duplicate: http://nl.altavista.com/web/results?itag=wrx&kgs=1&kls=0&q=
// out duplicate: http://es.altavista.com/web/results?itag=wrx&kgs=1&kls=0&q=
// out duplicate: http://it.altavista.com/web/results?itag=wrx&kgs=1&kls=0&q=

// out duplicate: http://aolsearch.aol.com/aol/search?invocationType=topsearchbox./aol/index.jsp&query=

// dumb blonde: http://www.ew.com/ew/search/verity/result/1,9158,,00.html?origin_topic=&origin_type=&origin_brand=0&find=

// no QRP: http://shopping.msn.com/fts/searchrdr.aspx?pt=0&pa1=0&pa2=0&searchtext=


// trying out a new concept: "  Keep Query Result Page As Good\r\n"

// These seemed bad, but maybe an off night--
// buscador was on a 2nd page, first was good.
// Some others merely had no hits for pynchon.

    // "GET 400 http://www.infotiger.com/search?OK=&qs=\r\n"
    // "  good url has next TOKEN \"size:\"\r\n"
    // "  more url has anchortext NUMBER\r\n"
    // "GET 570 http://science.slashdot.org/search.pl?query=\r\n"
    // "  good url has prior TWO TOKENS \"Score:\" NUMBER_POINTS\r\n"
    // "GET 590 http://www.bigblog.com/search.cgi?terms=\r\n"
    // "  good url has next TAG blockquote\r\n"
    // "  more url has anchortext NUMBER\r\n"
    // "GET 610 http://webreference.com/r/cs?query=\r\n"
    // "  good url has next TWO TOKENS \"-\" NUMBER+\"k\"\r\n"
    // "  more url has anchortext NUMBER\r\n"
    // "GET 640 http://www.webopedia.com/SHARED/search_action.asp?Template_Name=www.webopedia.com&submit=&term=\r\n"
    // "  good url has next TWO TOKENS \"-\" NUMBER+\"k\"\r\n"
    // "  more url has anchortext NUMBER\r\n"
    //

    // Internet.com didn't get a fair shake.
    // 600;
    // http://shop.internet.com/index.php?IC_ic=1&IC_query_search=1&IC_QueryText=
    // Fact matched Rule:   good url has prior TAG span ATTR clear VALUE "product_read_more"
    //
    // try it from these fresh parameters at top URL's form:
    // "GET 690 http://search.internet.com/www.jupiterweb.com?IC_Summary=1&IC_StartNumber=0&IC_BatchSize=10&IC_RelevanceThreshold=50&IC_QueryDatabase=all&SUBMIT=&IC_QueryText=\r\n"
    // fact above may be inappropriate in general.
    // It is slow though.

    // more recent observations:

    // "phenomenology of gazes" ( diligence=3 )
    // 650 ( websquash ) got a superset of 120 ( altavista ) & 130 ( yahoo )

    // These engines turned up many valuables pages that none other did:
    // 28  From   1 search engines: 210 ( a9 )
    // 19  From   1 search engines: 410 ( search )
    // 17  From   1 search engines: 550 ( surcha )
    // 16  From   1 search engines: 650 ( websquash -- binary OR? )
    // 16  From   1 search engines: 280 ( ask )
    // 15  From   1 search engines: 620 ( h-net, only within h-net )
    // 15  From   1 search engines: 170 ( alexa )
    // 11  From   1 search engines: 250 ( fr.search.yahoo )
    // 10  From   1 search engines: 110 ( google )
    //  8  From   2 search engines: 280 500
    //  8  From   1 search engines: 540 ( lycos.de )
    //  8  From   1 search engines: 370 ( business -- but kinda spurious )
    //  8  From   1 search engines: 130 ( yahoo )
    //  6  From   3 search engines: 120 130 650
    //  6  From   1 search engines: 500 ( teoma )
    //  6  From   1 search engines: 160 ( netscape )
    //  5  From   1 search engines: 180 ( entireweb )
    //  3  From   8 search engines: 140 190 310 320 330 340 350 360
    //  3  From   1 search engines: 520 ( terra -- but all spurious )
    //  2  From   6 search engines: 190 310 330 350 340 360
    //  2  From   5 search engines: 310 330 350 340 360
    //  2  From   4 search engines: 140 320 340 360
    //  2  From   3 search engines: 650 120 130
    //  2  From   3 search engines: 280 500 170
    //  2  From   3 search engines: 120 650 130
    //  2  From   3 search engines: 110 150 440
    //  2  From   2 search engines: 340 360
    //  2  From   1 search engines: 320 ( t1msn -- binary OR? )


// This was the engine list for a long time
// I dropped #100, the define call.
// Some of these fell out just for not providing any results on CROATIAN!
// A couple of MSN's seemed very redundant, were removed.
// Basically, what I kept/how I ordered was by the quantity of FROMS for
// each of these engines during a 48-thread croation word-triplet search.
// char Original_Omit[] = {
//     "GET 100 http://www.google.com/search?hl=en&ie=ISO-8859-1&btnG=Google+Search&q=define:\r\n"
//     "  good url has prior TAG li\r\n"
//     "  keep query result page as a good web page for reading\r\n"
//     "GET 110 http://www.google.com/search?hl=en&ie=ISO-8859-1&btnG=Google+Search&q=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"l\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 120 http://www.altavista.com/web/results?itag=wrx&kgs=0&kls=0&q=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"res\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 130 http://search.yahoo.com/search?ei=UTF-8&fr=yfp-t-501&cop=mss&p=\r\n"
//     "  none good until TWO TOKENS \"WEB\" \"RESULTS\"\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"yschttl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 140 http://search.sympatico.msn.ca/results.aspx?q=\r\n"
//     "  none good until TAG div ATTR label VALUE \"results\"\r\n"
//     "  good url has prior TAG h3\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 150 http://www.reference.com/search?db=web&q=\r\n"
//     "  none good until TWO TOKENS \"Search\" \"took\"\r\n"
//     "  good url has next TAG /td\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "  none good after anchortext NUMBER\r\n"
//     "GET 160 http://search.netscape.com/ns/search?st=webresults&fromPage=NSCPIndex&x=9&y=11&query=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"find\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 170 http://xslt.alexa.com/cgi-bin/search_form?submit=&term=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"small G\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 180 http://www.entireweb.com/query?q=\r\n"
//     "  good url has anchortext \"Details\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 190 http://search.ninemsn.com.au/results.aspx?q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 200 http://search.about.com/fullsearch.htm?TopNode=/&terms=\r\n"
//     "  good url has prior TAG div ATTR clear VALUE \"res\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 210 http://a9.com/?submit=&q=\r\n"
//     "  good url has prior TAG h2\r\n"
//     "  more url has anchortext \"next\"\r\n"
//     "GET 220 http://news.google.com/news?hl=en&lr=&sa=N&tab=in&q=\r\n"
//     "  good url has prior TAG td ATTR clear VALUE \"j\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 230 http://search.netscape.com/ns/search?fromPage=NSCPTop&x=9&y=15&query=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"find\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 240 http://search.netscape.com/ns/search?fromPage=nsnewssearch&query=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"find\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 250 http://fr.search.yahoo.com/search?ei=ISO-8859-1&fr=cb-ovb&sa=&p=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"yschttl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 260 http://search.ilse.nl/web?search_for=\r\n"
//     "  good url has next TAG /h3\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 270 http://www.onelook.com/?ls=a&w=\r\n"
//     "  good url has anchortext \"info\"\r\n"
//     "GET 280 http://www.ask.com/main/AskJeeves.asp?ask=\r\n"
//     "  good url has next TAG div ATTR onsrc\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 290 http://search.aol.com/aolcom/search?invocationType=topsearchbox./aolcom/index.jsp&query=\r\n"
//     "  good url has next TAG p ATTR clear VALUE \"durl find\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 300 http://education.yahoo.com/bin/ref_search?txt_main=\r\n"
//     "  good url has prior TAG td ATTR xmlns VALUE \"1%\"\r\n"
//     "GET 310 http://g.msn.com/0nwenus0/AQ/16?cp=65001&FORM=AQ&submit1=&q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 320 http://search.t1msn.com.mx/results.aspx?q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 330 http://g.msn.com/0nwenus0/AD/16?cp=1252&submit1=&FORM=AD&q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 340 http://g.msn.com/0nwenus0/AE/16?cp=1252&submit1=&FORM=A8&q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 350 http://search.latino.msn.com/results.aspx?q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 360 http://search.msn.com/results.aspx?q=\r\n"
//     "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 370 http://www.business.com/search/rslt_default.asp?vt=all&search=&type=web&query=\r\n"
//     "  good url has next TAG span ATTR clear VALUE \"url\"\r\n"
//     "GET 380 http://www.abacho.com/switch05/?cmd=search&country=-&coop=&rp=1&rf=&mesearchkey=world&q=\r\n"
//     "  good url has next TWO TOKENS NUMBER_POINTS \"KB\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 390 http://search.lycos.com/?query=\r\n"
//     "  good url has anchor TAG span ATTR clear VALUE \"large\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 410 http://www.search.com/search?q=\r\n"
//     "  good url has anchor PATH \"/click\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 420 http://www.newsday.com/search/dispatcher.front?target=article&Query=\r\n"
//     "  good url has next TAG span ATTR clear VALUE \"byline\"\r\n"
//     // "GET 430 http://www.bbc.co.uk/cgi-bin/search/results.pl?go=&uri=/home/today/index.live.shtml&q=\r\n"
//     // "  good url has next TAG /h3\r\n"
//     // "  more url has anchortext NUMBER\r\n"
//     "GET 440 http://search.virgilio.it/search/cgi/search.cgi?lr=&offset=0&hits=10&switch=0&f=hs&qs=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"link16\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 450 http://search.abacho.com/de/abacho.de/index.cfm?cmd=search&coop=&rp=1&country=es&rf=&mesearchkey=world&x=18&y=10&q=\r\n"
//     "  good url has next TWO TOKENS NUMBER_POINTS \"KB\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 460 http://search.abacho.com/de/abacho.de/index.cfm?cmd=search&coop=&rp=1&country=uk&rf=&mesearchkey=world&x=18&y=10&q=\r\n"
//     "  good url has next TWO TOKENS NUMBER_POINTS \"KB\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 470 http://search.abacho.com/de/abacho.de/index.cfm?cmd=search&coop=&rp=1&country=at&rf=&mesearchkey=world&x=18&y=10&q=\r\n"
//     "  good url has next TWO TOKENS NUMBER_POINTS \"KB\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 480 http://www.findarticles.com/p/search?tb=art&qt=\r\n"
//     "  good url has next TAG div ATTR clear VALUE \"sOpt\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     // "GET 490 http://upge.wn.com/?template=cheetah-search%2Findex.txt&language_id=1&query=\r\n"
//     // "  good url has anchor PATH IN \"/view\"\r\n"
//     "GET 500 http://s.teoma.com/search?submit=&qcat=1&qsrc=1&q=\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "  good url has prior TOKEN NUMBER+\"k\"\r\n"
//     "GET 510 http://www.overstock.com/cgi-bin/d2.cgi?page=search&keyword=\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "  good url has next TWO TOKENS \"Our\" \"Price:\"\r\n"
//     "GET 520 http://buscador.terra.es/default.asp?loc=searchbox&ca=c&query=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"fuenteUrl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     // "GET 530 http://www.searchforit.com/results.html?aff_id=&cat=&searchbutton=&keywords=\r\n"
//     // "  good url has anchor TAG a ATTR clear VALUE \"resultlink\"\r\n"
//     // "  more url has anchortext NUMBER\r\n"
//     "GET 540 http://suche.lycos.de/cgi-bin/pursuit?enc=utf-8&query=\r\n"
//     "  good url has anchor TAG a ATTR clear VALUE \"result\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 550 http://www.surcha.com/search.sa?searchstr=\r\n"
//     "  good url has anchor PATH \"/click.sa\"\r\n"
//     "  more url has anchortext \"next page\"\r\n"
//     "GET 560 http://www.ithaki.net/metasearch.cgi?where=web&Search=&query=\r\n"
//     "  good url has anchor TAG a ATTR title VALUE \"_blank\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 580 http://search.abacho.com/de/abacho.de/index.cfm?cmd=search&coop=&rp=1&country=de&rf=&mesearchkey=world&x=18&y=10&q=\r\n"
//     "  good url has next TWO TOKENS NUMBER_POINTS \"KB\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 620 http://www.h-net.org/multisearch.php?Submit=&searchtype=Discussion&searchquery=\r\n"
//     "  good url has anchor PATH \"/cgi-bin/logbrowse.pl\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 630 http://www.tradingmarkets.com/.site/news/search/?find=\r\n"
//     "  anchor PATH IN \"/.site/news/Stock%20News\"\r\n"
//     "GET 650 http://www.websquash.org/ssearch/smartsearch.cgi?DoSearch=&keywords=\r\n"
//     "  good url has prior TAG li ATTR vlink\r\n"
//     "  more url has anchortext \"Next Results\"\r\n"
//     "GET 660 http://www.welt.de/archiv/?se=&search.execute=true&lucyStemmed=1&lucyField=2&lucySection=21&lucySort=1&lucyMaxNumberResultsSorted=500&lucyOptimized=false&lucyExpr=\r\n"
//     "  good url has next TWO TOKENS \"-\" NUMBER_POINTS+\",\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 670 http://www.yahoo.com/_ylh=X3oDMTFncWJtanZnBF9TAzI3MTYxNDkEcGlkAzExOTQ1MzEwODQEdGVzdAMwBHRtcGwDdGFibGUuaHRtbA--/r/sx/*-http://search.yahoo.com/search?ei=UTF-8&fr=yfp-t-501&cop=mss&p=\r\n"
//     "  good url has next TOKEN NUMBER+\"k\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     "GET 680 http://www.yigg.de/search.php?search=\r\n"
//     "  good url has prior TAG img ATTR background VALUE \"ext link\"\r\n"
//     "  more url has anchortext NUMBER\r\n"
//     // "GET 690 http://search.internet.com/www.jupiterweb.com?IC_Summary=1&IC_StartNumber=0&IC_BatchSize=10&IC_RelevanceThreshold=50&IC_QueryDatabase=all&SUBMIT=&IC_QueryText=\r\n"
//     "\r\n"
//     "NOT example.querydomain.com querydomain.com/undesirable_path_etc\r\n"
//     "\r\n"
// };

char DefaultSearchEngineTable[] = {
    "GET 710 http://www.google.com/search?hl=en&ie=ISO-8859-1&btnG=Google+Search&q=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"l\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 720 http://search.yahoo.com/search?ei=UTF-8&fr=yfp-t-501&cop=mss&p=\r\n"
    "  none good until TWO TOKENS \"WEB\" \"RESULTS\"\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"yschttl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 730 http://www.reference.com/search?db=web&q=\r\n"
    "  none good until TWO TOKENS \"Search\" \"took\"\r\n"
    "  good url has next TAG /td\r\n"
    "  more url has anchortext NUMBER\r\n"
    "  none good after anchortext NUMBER\r\n"
    "GET 740 http://www.altavista.com/web/results?itag=wrx&kgs=0&kls=0&q=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"res\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 750 http://search.sympatico.msn.ca/results.aspx?q=\r\n"
    "  none good until TAG div ATTR label VALUE \"results\"\r\n"
    "  good url has prior TAG h3\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 760 http://search.ninemsn.com.au/results.aspx?q=\r\n"
    "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 770 http://search.latino.msn.com/results.aspx?q=\r\n"
    "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 780 http://search.msn.com/results.aspx?q=\r\n"
    "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 790 http://g.msn.com/0nwenus0/AD/16?cp=1252&submit1=&FORM=AD&q=\r\n"
    "  good url has next TAG li ATTR clear VALUE \"dispUrl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 800 http://xslt.alexa.com/cgi-bin/search_form?submit=&term=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"small G\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 810 http://fr.search.yahoo.com/search?ei=ISO-8859-1&fr=cb-ovb&sa=&p=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"yschttl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 820 http://search.virgilio.it/search/cgi/search.cgi?lr=&offset=0&hits=10&switch=0&f=hs&qs=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"link16\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 830 http://suche.lycos.de/cgi-bin/pursuit?enc=utf-8&query=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"result\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 840 http://buscador.terra.es/default.asp?loc=searchbox&ca=c&query=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"fuenteUrl\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 850 http://www.websquash.org/ssearch/smartsearch.cgi?DoSearch=&keywords=\r\n"
    "  good url has prior TAG li ATTR vlink\r\n"
    "  more url has anchortext \"Next Results\"\r\n"
    "GET 860 http://www.ask.com/main/AskJeeves.asp?ask=\r\n"
    "  good url has next TAG div ATTR onsrc\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 870 http://www.entireweb.com/query?q=\r\n"
    "  good url has anchortext \"Details\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 880 http://www.search.com/search?q=\r\n"
    "  good url has anchor PATH \"/click\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 890 http://www.surcha.com/search.sa?searchstr=\r\n"
    "  good url has anchor PATH \"/click.sa\"\r\n"
    "  more url has anchortext \"next page\"\r\n"
    "GET 900 http://www.business.com/search/rslt_default.asp?vt=all&search=&type=web&query=\r\n"
    "  good url has next TAG span ATTR clear VALUE \"url\"\r\n"
    "GET 910 http://search.netscape.com/ns/search?st=webresults&fromPage=NSCPIndex&x=9&y=11&query=\r\n"
    "  good url has anchor TAG a ATTR clear VALUE \"find\"\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 920 http://search.ilse.nl/web?search_for=\r\n"
    "  good url has next TAG /h3\r\n"
    "  more url has anchortext NUMBER\r\n"
    "GET 930 http://a9.com/?submit=&q=\r\n"
    "  good url has prior TAG h2\r\n"
    "  more url has anchortext \"next\"\r\n"
    "GET 940 http://s.teoma.com/search?submit=&qcat=1&qsrc=1&q=\r\n"
    "  more url has anchortext NUMBER\r\n"
    "  good url has prior TOKEN NUMBER+\"k\"\r\n"
};


CUrl::CUrl( )
{
    #if DO_DEBUG_CALLS
        Routine( L"511" );
    #endif
    GetLineCount = 0;
    NotLineCount = 0;
    pSurlLastGetLine = NULL;
}

CUrl::~CUrl( )
{
    #if DO_DEBUG_CALLS
        Routine( L"512" );
    #endif
}

// Over 4 right now I am only missing .museum and .travel -- per wiki:
// Génériques réglementés .arpa | .edu | .gov | .int | .mil
// Génériques ouverts .biz | .com | .info | .name | .net | .org | .pro
// Commandités ouverts .aero | .cat | .coop | .jobs | .mobi | .museum | .travel
// Commandités en démarrage .tel
// Commandités proposés .asia | .mail | .post | .xxx
// Génériques supprimés .nato | .root
// Réservés .example | .invalid | .localhost | .test
// Pseudo-domaines .bitnet | .csnet | .lan | .local | .onion | .uucp | .tld

wchar_t * DomainTypes3Char[] = {
    L"edu",
    L"gov",
    L"int",
    L"mil",
    L"biz",
    L"com",
    L"net",
    L"org",
    L"pro",
    L"cat",
    L"tel",
    L"xxx",
};

wchar_t * DomainTypes4Char[] = {
    L"arpa",
    L"info",
    L"name",
    L"aero",
    L"coop",
    L"jobs",
    L"mobi",
    L"asia",
    L"mail",
    L"post",
    L"nato",
    L"root",
};

void CUrl::SetUpDefaultSearchEngineControls( )
{
    // Also I will set up these two lists:
    // CSol CSol3CharLcDomains( CSOL_SCALAR );
    // CSol CSol4CharLcDomains( CSOL_SCALAR );
    int OpenedAFile = 0;

    {
        int i = 0;
        int n = sizeof( DomainTypes3Char )/sizeof( *DomainTypes3Char );
        for( ;; )
        {
            size_t index = CSol3CharLcDomains.AddKey( DomainTypes3Char[i] );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at curl 1508" ); }
            #endif
            if( ++i == n )
                break;
        }
        i = 0;
        n = sizeof( DomainTypes4Char )/sizeof( *DomainTypes4Char );
        for( ;; )
        {
            size_t index = CSol4CharLcDomains.AddKey( DomainTypes4Char[i] );
            #if DO_DEBUG_ADDFIND
                if( index <= 1 )
                    { Spew( L"AddFind 1 at curl 1508" ); }
            #endif
            if( ++i == n )
                break;
        }
    }


    // Toss out the old URL lists. - Reset is not needed here.
    // CSolSearchUrls.Reset( );
    // CSolRejectUrls.Reset( );

    // Routine communication by global
    GetLineCount = 0;
    NotLineCount = 0;

    // If I find a file present at the fixed path c:\wordsex\engines.txt,
    // I will open and use it, otherwise use the build-in wchar_t string.
    // #define DefaultEnginesFilename       L"C:\\WordsEx\\Engines.txt"

    WIN32_FIND_DATA FindFileData;
    HANDLE hFindFile = FindFirstFile( DefaultEnginesFilename, & FindFileData );
    if( hFindFile != INVALID_HANDLE_VALUE
    && ( FindFileData . dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
    {
        // Open that file, get into pWBuf, nWBuf.

        #if DO_DEBUG_LOAD_ENG
            ; Spew( L"SetUpDefaultSearchEngineControls: Loading from file." );
        #endif

        HelperToLoadSearchEngineFile( DefaultEnginesFilename );

        // Just to prove new connections worked. Now go quietly...
        // wchar_t wk[100];
        // wsprintf( wk, L"New lists have %d GET and %d NOT items.", GetLineCount, NotLineCount );
        // Say( wk );

    }
    else
    {
        #if DO_DEBUG_LOAD_ENG
            ; Spew( L"SetUpDefaultSearchEngineControls: Loading from char[]." );
        #endif

        // Convert built-in char string into wides in pWBuf, nWBuf.
        size_t nWBuf = NULL;
        wchar_t * pWBuf = Pag.CategorizeAndConvertInputBytesToWide( & nWBuf, ( unsigned char * ) DefaultSearchEngineTable, sizeof( DefaultSearchEngineTable ) - 1 );

        if( pWBuf != NULL )
        {
            wchar_t * scan = pWBuf;
            wchar_t * atop = scan;
            wchar_t * past = scan + nWBuf;

            for( ;; )
            {
                wchar_t wc = *scan;

                if( wc == '\r' || wc == '\n' )
                {
                    if( scan > atop + 5 )
                        WideParseSearchEngineLine( atop, scan );
                    atop = scan + 1; // advance for next line past this CR or LF
                }

                if( ++scan == past ) // allow for a last line w/o final newline
                {
                    if( scan > atop + 5 )
                        WideParseSearchEngineLine( atop, scan );
                    break;
                }
            }
            MyFree( 1700, UNPREDICTABLE, pWBuf );
            pWBuf = NULL;
        }
    }
}

void CUrl::HelperToLoadSearchEngineFile( wchar_t * szFileName )
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = INVALID_HANDLE_VALUE;

#ifdef _WIN32_WCE
    // N.B. On the PPC, where CreateFileForMapping replaces CreateFile,
    // as soon as that handle gets passed to CreateFileMapping, whether
    // it pass or fail, that handle is invalid, so do not CloseFile it.
    hFile = CreateFileForMapping( // Lookalikes... READING
        szFileName, // LPCTSTR lpFileName,
        GENERIC_READ, // DWORD dwDesiredAccess,
        FILE_SHARE_READ, // DWORD dwShareMode,
        NULL, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        OPEN_EXISTING, // DWORD dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, // DWORD dwFlagsAndAttributes,
        NULL // HANDLE hTemplateFile
    );
#else // not _WIN32_WCE
    hFile = CreateFile( // Lookalikes... READING - in LoadSearchEngineForms
        szFileName, // LPCTSTR lpFileName,
        GENERIC_READ, // DWORD dwDesiredAccess,
        FILE_SHARE_READ, // DWORD dwShareMode,
        NULL, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        OPEN_EXISTING, // DWORD dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, // DWORD dwFlagsAndAttributes,
        NULL // HANDLE hTemplateFile
    );
#endif // _WIN32_WCE

    if( hFile == INVALID_HANDLE_VALUE )
    {
        Say( L"CreateFile GENERIC_READ failed." );
        return; // failure
    }

    // I must know file size. I will limit the size too.
    // If I say too big a size, disk file gets enlarged.
    // Help warns not to CreateFileMapping if size zero.

    DWORD dwSize = GetFileSize( hFile, NULL ); // without high word

    if ( dwSize == 0xFFFFFFFF )
    {
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 1" );
        #endif // not _WIN32_WCE
        ProgramError( L"GetFileSize 1" );
        return; // failure
    }
    if ( dwSize == 0 )
    {
        // Silently return and not try to load any zero byte file.
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 2" );
        #endif // not _WIN32_WCE
        // Not silently any more...
        Say( L"File size is zero. Ignoring file." );
        return; // failure
    }

    // Open and read whole file contents.

    hMap = CreateFileMapping( // Lookalikes... READING
        hFile, // HANDLE hFile,
        NULL, // LPSECURITY_ATTRIBUTES lpAttributes,
        PAGE_READONLY, // DWORD flProtect,
        0, // DWORD dwMaximumSizeHigh,
        dwSize, // DWORD dwMaximumSizeLow,
        NULL // LPCTSTR lpName
    );

    if( hMap == INVALID_HANDLE_VALUE )
    {
        ProgramError( L"CreateFileMapping2 " );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 3" );
        #endif // not _WIN32_WCE
        return; // failure
    }

    BYTE * bBuffer = ( BYTE* ) MapViewOfFile( // Lookalikes... READING
        hMap, // HANDLE hFileMappingObject,
        FILE_MAP_READ, // DWORD dwDesiredAccess,
        0, // DWORD dwFileOffsetHigh,
        0, // DWORD dwFileOffsetLow,
        dwSize // SIZE_T dwNumberOfBytesToMap
    );

    if( bBuffer == NULL )
    {
        ProgramError( L"MapViewOfFile 2" );
        if( ! CloseHandle( hMap ) )
            ProgramError( L"CloseHandle hMap 11" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 4" );
        #endif // not _WIN32_WCE
        return; // failure
    }

    // When I tried to input a 400MB file, I got a malloc
    // returned 0 error ( over at ID#1918 ), so put a limit
    // on file size. Don't return; Just take part of file.

    if ( dwSize > 10000000 )
    {
        dwSize = 10000000; // Ten MB big enuf for you?
        Say( L"Exceeds 10 MB. Taking only 10 MB." );
    }

    // This bBuffer contains dwSize bytes.
    // You cannot write sentinels into it.

    // I do not see the original routine ever freed the buffer!
    // Aha. Unmap must do a free, cause I cannot get byte later.


    // This was scary, but it's essentially all written for free:
    // Accept ASCII, or any of 3 styles of UTF-8 or wide UNICODE,
    // with or without introducer sequences, which get stripped,
    // and produce the wide_t buffer, which malloc might be fat.

    size_t nWBuf = NULL;
    wchar_t * pWBuf = Pag.CategorizeAndConvertInputBytesToWide( & nWBuf, bBuffer, dwSize );

    // Now I can close off the file i/o part before continuing.
    if( ! UnmapViewOfFile( bBuffer ) )
    {
        ProgramError( L"UnmapViewOfFile 2" );
        if( ! CloseHandle( hMap ) )
            ProgramError( L"CloseHandle hMap 13" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 5" );
        #endif // not _WIN32_WCE
        return; // failure
    }
    if( ! CloseHandle( hMap ) )
    {
        ProgramError( L"CloseHandle hMap 15" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 6" );
        #endif // not _WIN32_WCE
        return; // failure
    }

    // After doing CreateFilemapping, system will do the CloseHandle( hFile )
    // and this call will return false.--Only applies to CE, not the desktop.

    #ifndef _WIN32_WCE
        if( ! CloseHandle( hFile ) ) // Never on PPC
            ProgramError( L"CloseHandle hFile 7" );
    #endif // not _WIN32_WCE

    if( pWBuf != NULL )
    {
        #if DO_DEBUG_LOAD_ENG
            ; Spew( L"HelperToLoadSearchEngineFile: Got input data." );
        #endif

        // So, we are ready to scan the wide buffer pWBuf of size nWBuf:
        // Toss out the old URL lists.
        CSolSearchUrls.Reset( );
        CSolRejectUrls.Reset( );

        // Routine communication by global
        GetLineCount = 0;
        NotLineCount = 0;

        wchar_t * scan = pWBuf;
        wchar_t * past = scan + nWBuf;
        wchar_t * atop = scan;

        for( ;; )
        {
            wchar_t wc = *scan;

            if( wc == '\r' || wc == '\n' )
            {
                if( scan > atop + 5 )
                    WideParseSearchEngineLine( atop, scan );
                atop = scan + 1; // advance for next line past this CR or LF
            }

            if( ++scan == past ) // allow for a last line w/o final newline
            {
                if( scan > atop + 5 )
                    WideParseSearchEngineLine( atop, scan );
                break;
            }
        }
        MyFree( 1700, UNPREDICTABLE, pWBuf );
        pWBuf = NULL;
    }
}

void CUrl::LoadSearchEngineForms( )
{
    #if DO_DEBUG_CALLS
        Routine( L"482" );
    #endif

    // Start by cloning a GetOpenFileName example from CFIO.CPP:

    wchar_t szFileName[MAX_PATH];
    szFileName[0] = NULL;
    // Let's try setting a default suggested output filename:
    wcscpy( szFileName, L"Engines.txt" );

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFileName;
    ofn.lpstrTitle = L"Load Search Engine URLs and Rules";
    ofn.nMaxFile = sizeof( szFileName )/sizeof( *szFileName );
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = L"txt"; // let system add the single default ( . )txt


    if ( ! GetOpenFileName( & ofn ) )
    {
#ifndef _WIN32_WCE
        if( CommDlgExtendedError( ) ) // zero if user canceled
            ProgramError( L"GetOpenFileName" );
#endif // not _WIN32_WCE
        return;
    }

    // Win CE returns a bizarre string containing the desired filename
    // followed by a semicolon, and the filter string; Fix that string:
    // I see now. Win CE appended my whole string from ofn.lpstrFilter.
    // That is no problem here, as I do not have a *.htm;*.html option.

    {
        wchar_t * scan = szFileName;
        for( ;; )
        {
            if( * scan == ';'
            || * scan == NULL )
                break;
            scan ++;
        }
        * scan = NULL;
    }

    // Now clone from the example of CommonPaperFileInput in CFIO.CPP
    // Move it into a helper routine, so Default routine can call it.

    #if DO_DEBUG_LOAD_ENG
        ; SpewTwo( L"LoadSearchEngineForms", szFileName );
    #endif

    HelperToLoadSearchEngineFile( szFileName );
    {
        wchar_t wk[100];
        wsprintf( wk, L"New lists have %d GET and %d NOT items.", GetLineCount, NotLineCount );
        Say( wk );
    }
}

void CUrl::WideParseSearchEngineLine( wchar_t * atop, wchar_t * past )
{
    // Serves SetUpDefaultSearchEngineControls
    // Serves LoadSearchEngineForms
    // Do things for input lines starting with GET or NOT.

    // Soon to add many more rules, like GOOD URL, BAD URL, etc.,
    // which will add to the pOneSurl of the most recent GET item.
    // past MAY be pointing at a newline; past MAY be past malloc.

    // I may as well have a local copy I can work in.

    wchar_t CopiedLine[ 600 ];
    size_t nWides = past - atop;
    if( nWides > 590 )
    {
        #if DO_DEBUG_LOAD_ENG
            ; Spew( L"WideParseSearchEngineLine: skipping input line > 590 chars" );
        #endif
        return;
    }

    // past was at the newline, so this copy has no newline:
    memcpy( CopiedLine, atop, nWides * sizeof( wchar_t ) );
    CopiedLine[ nWides ] = NULL;

    #if DO_DEBUG_LOAD_ENG
        ; SpewTwo( L"InputLine", CopiedLine );
    #endif

    wchar_t * scan = CopiedLine;
    wchar_t * stop = scan + nWides;

    for( ;; )
    {
        if( scan == stop )
            break;
        if( *scan != ' '
        &&  *scan != '\t' )
            break;
        scan ++; // skip over leading spaces or tabs
    }
    for( ;; )
    {
        if( scan == stop )
            break;
        if( stop[-1] != ' '
        &&  stop[-1] != '\t' )
            break;
        stop --; // skip over trailing spaces or tabs
    }
    *stop = NULL;

    if( stop - scan < 4 ) // CYA to test first keyword
    {
        // Hey, not 5. KEEP only has four chars!
        #if DO_DEBUG_LOAD_ENG
            ; Spew( L"WideParseSearchEngineLine: skipping input line < 4 chars" );
        #endif
        return;
    }

    int LineType = RULETYPE_INVALID;
    int SubType = RULESUBTYPE_UNUSED;
    int iRest = 0; // where to start a common next word matcher

    if( ( scan [0] | ' ' ) == 'g'
    &&  ( scan [1] | ' ' ) == 'e'
    &&  ( scan [2] | ' ' ) == 't'
    &&  scan [3] == ' '
    &&  isdigit( scan [4] ) // safe, isascii too.
    &&  isdigit( scan [5] ) // safe, isascii too.
    &&  isdigit( scan [6] ) // safe, isascii too.
    &&  scan [7] == ' '
    &&  ( scan [8] | ' ' ) == 'h'
    &&  ( scan [9] | ' ' ) == 't'
    &&  ( scan [10] | ' ' ) == 't'
    &&  ( scan [11] | ' ' ) == 'p' ) // permit https: although unlikely
    {
        LineType = RULETYPE_GET;
        // Process a line that looks like this ( without quotes nor newline ):
        // "GET 101 http://www.google.com/search?hl=en&ie=ISO-8859-1&btnG=Google+Search&q="

        // do not check for the final equals sign.
        // e.g., I now use google ...=define: URL.

        GetLineCount ++;

        int GetOrdinal = ( scan [4] - '0' ) * 100
            + ( scan [5] - '0' ) * 10
            + ( scan [6] - '0' );

        // The purpose of putting a GetOrdinal on sort key
        // is so that it will sort the order of URLs to use.
        // So do not advanceover ###, But user will have to.

        size_t SurlIndex = CSolSearchUrls.AddKey( scan + 4 );
        #if DO_DEBUG_ADDFIND
            if( SurlIndex == 1 )
                { Spew( L"AddFind 1 at curl 4816" ); }
        #endif

        #if DO_DEBUG_LOAD_ENG
            ; SpewValue( L"GET line - GetOrdinal", GetOrdinal );
            ; SpewTwo( L"GET line - GetUrl", scan + 4 );
            ; SpewValue( L"GET line - SurlIndex", SurlIndex );
        #endif

        // Here, CUrl WideParseSearchEngineLine creates a revised S.E. list.
        COneSurl * pSurl = new COneSurl( SurlIndex );

        pSurlLastGetLine = pSurl;

        if( pSurl != NULL )
        {
            // Can there be an old pointer in User.pVoid?
            // Not unless user has a duplicate GET entry.
            // Then shame on him. I reset CSolSearchUrls.

            CSolSearchUrls.SetUserpVoid( SurlIndex, pSurl );
            pSurl->GetOrdinal = GetOrdinal;

            // Constructor put NULL into this new member. Init it:
            // wchar_t * szDomainName;
            // scan is pointing to a whole string, like:
            // "GET 101 http://www.google.com/search?hl=en&...
            // Find the double slash, the slash, extract DOMAIN name.

            wchar_t * StartDomain = NULL;
            wchar_t * PastDomain = NULL;
            for( ;; )
            {
                if( *scan == NULL )
                    break;
                if( *scan == '/' )
                {
                    if( scan[1] == '/' )
                    {
                        scan ++;
                        StartDomain = scan + 1;
                    }
                    else
                    {
                        PastDomain = scan;
                        break;
                    }
                }
                scan ++;
            }
            if( StartDomain != NULL
            && PastDomain != NULL
            && PastDomain > StartDomain )
            {
                // work moved below...
            }
            else
            {
                // Fix the case of garbage right now,
                // so that szDomainName is never NULL.
                StartDomain = L"INVALID";
                PastDomain = StartDomain + 7;
            }
            // right now it just annotates, so do not bother to
            // extract the minimim required part of domain name.
            size_t nLen = PastDomain - StartDomain;
            wchar_t * into = ( wchar_t * ) MyMalloc( 1381, ( nLen + 1 ) * sizeof( wchar_t ) );
            if( into != NULL )
            {
                wcsncpy( into, StartDomain, nLen );
                into[ nLen ] = NULL;
            }
            pSurl->szDomainName = into;
        }

        #if DO_DEBUG_LOAD_ENG
            ; SpewTwo( L"pSurl->szDomainName", pSurl->szDomainName );
            ; SpewValue( L"pSurl->GetOrdinal", pSurl->GetOrdinal );
        #endif
    }
    else
    if( ( scan [0] | ' ' ) == 'n'
    &&  ( scan [1] | ' ' ) == 'o'
    &&  ( scan [2] | ' ' ) == 't'
    &&  scan [3] == ' ' )
    {
        LineType = RULETYPE_NOT;
        // Process a line that looks like this ( without quotes nor newline ):
        // "NOT # /search?q=cache:\r\n"
        // "NOT furl.net furl.net/tools.jsp\r\n"
        // There should be exactly one more space character after scan[3].
        // Find it:
        wchar_t * pSpace = NULL;
        wchar_t * pLook = scan + 4;
        for( ;; )
        {
            if( pLook == stop )
                break;
            if( *pLook == ' ' )
            {
                if( pSpace != NULL )
                {
                    #if DO_DEBUG_LOAD_ENG
                        ; Spew( L"Bad 'NOT' line, more than 2 spaces" );
                    #endif
                    return;
                }
                pSpace = pLook;
            }
            pLook ++;
        }
        if( pSpace == NULL )
        {
            #if DO_DEBUG_LOAD_ENG
                ; Spew( L"Bad 'NOT' line, less than 2 spaces" );
            #endif
            return;
        }
        *pSpace = NULL; // terminate first of two strings


        wchar_t * p1Url = scan + 4;
        wchar_t * p2Url = pSpace + 1;

        size_t RSolIndex = CSolRejectUrls.AddKey( p1Url );
        #if DO_DEBUG_ADDFIND
            if( RSolIndex == 1 )
                { Spew( L"AddFind 1 at curl 4859" ); }
        #endif

        CSol * pSol = ( CSol * ) CSolRejectUrls.GetUserpVoid( RSolIndex );
        if( pSol == NULL )
        {
            pSol = new CSol( CSOL_SCALAR );
            CSolRejectUrls.SetUserpVoid( RSolIndex, pSol );
        }
        if( pSol != NULL )
        {
            size_t R2Index = pSol->AddKey( p2Url );
            #if DO_DEBUG_ADDFIND
                if( R2Index == 1 )
                    { Spew( L"AddFind 1 at curl 4873" ); }
            #endif
            NotLineCount ++;
        }

    }
    else
    if( ( scan [0] | ' ' ) == 'g'
    &&  ( scan [1] | ' ' ) == 'o'
    &&  ( scan [2] | ' ' ) == 'o'
    &&  ( scan [3] | ' ' ) == 'd'
    &&  scan [4] == ' ' )
    {
        // Process a line that looks like this ( w/o spaces, quotes, newline ):
        // "  good url has prior TAG span ATTR clear VALUE \"titre5\"\r\n"
        // "  good url has anchor TAG a ATTR clear VALUE \"find\"\r\n"
        LineType = RULETYPE_GOOD;
        iRest = 5;
    }
    else
    if( ( scan [0] | ' ' ) == 'b'
    &&  ( scan [1] | ' ' ) == 'a'
    &&  ( scan [2] | ' ' ) == 'd'
    &&  scan [3] == ' ' )
    {
        // Process a line that looks like this ( w/o spaces, quotes, newline ):
        // "  bad url has ...
        LineType = RULETYPE_BAD;
        iRest = 4;
    }
    else
    if( ( scan [0] | ' ' ) == 'm'
    &&  ( scan [1] | ' ' ) == 'o'
    &&  ( scan [2] | ' ' ) == 'r'
    &&  ( scan [3] | ' ' ) == 'e'
    &&  scan [4] == ' ' )
    {
        // Process a line that looks like this ( w/o spaces, quotes, newline ):
        // "  more url has anchortext NUMBER\r\n"
        LineType = RULETYPE_MORE;
        iRest = 5;
    }
    else
    if( ( scan [0] | ' ' ) == 'n'
    &&  ( scan [1] | ' ' ) == 'o'
    &&  ( scan [2] | ' ' ) == 'n'
    &&  ( scan [3] | ' ' ) == 'e'
    &&  scan [4] == ' '
    &&  ( scan [5] | ' ' ) == 'g'
    &&  ( scan [6] | ' ' ) == 'o'
    &&  ( scan [7] | ' ' ) == 'o'
    &&  ( scan [8] | ' ' ) == 'd'
    &&  scan [9] == ' ' )
    {
        // Process a line that looks like this ( w/o spaces, quotes, newline ):
        // "  none good until TWO TOKENS NUMBER_COMMAS \"results.\"\r\n"
        if( ( scan [10] | ' ' ) == 'u'
        &&  ( scan [11] | ' ' ) == 'n'
        &&  ( scan [12] | ' ' ) == 't'
        &&  ( scan [13] | ' ' ) == 'i'
        &&  ( scan [14] | ' ' ) == 'l'
        &&  scan [15] == ' ' )
        {
            LineType = RULETYPE_NONE_UNTIL;
            iRest = 16;
            if( pSurlLastGetLine != NULL )
                pSurlLastGetLine->ContainsAnyRuleNoneGoodUntil = 1;
        }
        else
        if( ( scan [10] | ' ' ) == 'a'
        &&  ( scan [11] | ' ' ) == 'f'
        &&  ( scan [12] | ' ' ) == 't'
        &&  ( scan [13] | ' ' ) == 'e'
        &&  ( scan [14] | ' ' ) == 'r'
        &&  scan [15] == ' ' )
        {
            LineType = RULETYPE_NONE_AFTER;
            iRest = 16;
        }
        else
        {
            LineType = RULETYPE_INVALID;
        }
    }
    else
    if( ( scan [0] | ' ' ) == 'k'
    &&  ( scan [1] | ' ' ) == 'e'
    &&  ( scan [2] | ' ' ) == 'e'
    &&  ( scan [3] | ' ' ) == 'p' )
    {
        LineType = RULETYPE_KEEP;

        if( pSurlLastGetLine != NULL )
            pSurlLastGetLine->KeepQueryResultPageAsGoodText = 1;
    }
    else
    if( ( scan [0] | ' ' ) == 's'
    &&  ( scan [1] | ' ' ) == 't'
    &&  ( scan [2] | ' ' ) == 'u'
    &&  ( scan [3] | ' ' ) == 'd'
    &&  ( scan [4] | ' ' ) == 'y' )
    {
        LineType = RULETYPE_STUDY;

        if( pSurlLastGetLine != NULL )
            pSurlLastGetLine->AnnotateFactsForStudy = 1;
    }

    // Now some common helper code for 3 line types
    // No, for all the rule types, just not GET/NOT.

    // Okay, three of them expect "url has" in fact.
    // So test for that first...

    if( LineType == RULETYPE_GOOD
    ||  LineType == RULETYPE_BAD
    ||  LineType == RULETYPE_MORE )
    {
        // parse common "url has next/prior/anchor" segment.
        // if bad parse, set LineType to 0 as if no keyword.
        wchar_t * ss = scan + iRest;
        if( ( ss [0] | ' ' ) == 'u'
        &&  ( ss [1] | ' ' ) == 'r'
        &&  ( ss [2] | ' ' ) == 'l'
        &&  ss [3] == ' '
        &&  ( ss [4] | ' ' ) == 'h'
        &&  ( ss [5] | ' ' ) == 'a'
        &&  ( ss [6] | ' ' ) == 's'
        &&  ss [7] == ' ' )
        {
            iRest += 8;
        }
        else
        {
            LineType = RULETYPE_INVALID;
        }
    }

    // Now for all five types of rules, except KEEP
    // look for an optional modifer keyword.

    if( LineType == RULETYPE_GOOD
    ||  LineType == RULETYPE_BAD
    ||  LineType == RULETYPE_MORE
    ||  LineType == RULETYPE_NONE_UNTIL
    ||  LineType == RULETYPE_NONE_AFTER )
    {
        wchar_t * ss = scan + iRest;
        if( ( ss [0] | ' ' ) == 'n'
        &&  ( ss [1] | ' ' ) == 'e'
        &&  ( ss [2] | ' ' ) == 'x'
        &&  ( ss [3] | ' ' ) == 't'
        &&  ss [4] == ' ' )
        {
            SubType = RULESUBTYPE_NEXT;
            iRest += 5;
        }
        else
        if( ( ss [0] | ' ' ) == 'p'
        &&  ( ss [1] | ' ' ) == 'r'
        &&  ( ss [2] | ' ' ) == 'i'
        &&  ( ss [3] | ' ' ) == 'o'
        &&  ( ss [4] | ' ' ) == 'r'
        &&  ss [5] == ' ' )
        {
            SubType = RULESUBTYPE_PRIOR;
            iRest += 6;
        }
        else
        if( ( ss [0] | ' ' ) == 'a'
        &&  ( ss [1] | ' ' ) == 'n'
        &&  ( ss [2] | ' ' ) == 'c'
        &&  ( ss [3] | ' ' ) == 'h'
        &&  ( ss [4] | ' ' ) == 'o'
        &&  ( ss [5] | ' ' ) == 'r'
        && ss [6] == ' ' )
        {
            // "anchortext" is not a subtype, but is BALD text.
            // It is not processed here, only "anchor " itself.
            SubType = RULESUBTYPE_ANCHOR;
            iRest += 7;
        }
        else
        {
            // Do not invalidate others, like anchortype... domain... path...
            // No: LineType = RULETYPE_INVALID;
        }

    }

    if( LineType == RULETYPE_INVALID )
    {
        #if DO_DEBUG_LOAD_ENG
            ; Spew( L"WideParseSearchEngineLine: skipping input line not fitting any parse formats" );
        #endif
        return;
    }

    // At this point, scan + iRest should be CHtm's AddOrTest string.
    // Except for GET / NOT, which left subtype 0.--Do not spew them.
    #if DO_DEBUG_LOAD_ENG
        ; SpewValue( L"LineType", LineType );
        if( SubType != RULESUBTYPE_UNUSED )
        {
            ; SpewValue( L"SubType", SubType );
            ; SpewTwo( L"scan + iRest", scan + iRest );
        }
    #endif

    if( LineType == RULETYPE_GET
    ||  LineType == RULETYPE_NOT
    ||  LineType == RULETYPE_KEEP
    ||  LineType == RULETYPE_STUDY )
    {
        return; // GET, NOT, KEEP, STUDY were finished in their IF clauses.
    }

    // It is time to invent:
    //
    // I must get small ordered vectors of these scan + iRest strings
    // into the CHtm parser, by merit of passing it a pOneSurl object.
    //
    // Since I want the users' line input order to determine priority,
    // I was thinking of a fixed vector. But a CSol makes so much more
    // sense, as the exact string match would be fast. I can fit type,
    // subtype, and user's input ordinal into the User.Value bitfield.
    //
    // Using a CSol, there will be no "NEXT-GET or EOF" action needed.
    // GET line NULLs global ptr, creates new CSol, hangs it from his
    // newly created pOneSurl, and sets global ptr = CSol if all okay.
    //
    // Here, I save string, munge together the type bits and ordinals.

    // On output end, CHtm will have exactly ( zero or ) one pOneSurl.
    // If is-a-queryresultpage, then CHtm AddOrTest will try to find
    // the passed string ( without adding a scraping oneliner prefix )
    // in the CSol. If found, extract the User.Value and decide how
    // to process this match-event. "None" until/after will clear or
    // set 2-per-pSurl inhibition bools ( one preset, one precleared ).
    // Good/Bad must save bits and ordinals, until an UrlA-UrlB-UrlC
    // sequnce reaches UrlC, and processes UrlB, and outputs reason.
    // "More" can be acted upon immediately? No, it also awaits UrlC,
    // And recognizing MORE will also inhibit recognizing GOOD.

    // No. Changing to vectors, not CSol.
    //
    // I still have the no-EOF-task benefit if I make a realloc right
    // into the pOneSurl member pointer to the vector of rules, and
    // immediately fill the new slot. pOneSurl's vector can be these:
    // typedef struct _WCPUI
    // {
    //     wchar_t * wcp;
    //     unsigned int ui;
    // } WCPUI;
    // These are the new members:
    // WCPUI * pRuleVector;
    // size_t  nRuleVector;
    // which can be located by pointer pSurlLastGetLine
    // On every new fact, I must either malloc or realloc. I add no slop.

    // CYA - there must be a prior GET line to have this pointer.
    if( pSurlLastGetLine != NULL )
    {
        // Get the malloc for string before extending vector to hold it.

        wchar_t * from = scan + iRest;
        size_t nLen = wcslen( from );
        wchar_t * into = ( wchar_t * ) MyMalloc( 2676, ( nLen + 1 ) * sizeof( wchar_t ) );
        if( into == NULL )
            return; // malloc error already messaged
        wcscpy( into, from );

        size_t nOld = pSurlLastGetLine->nRuleVector;
        size_t nNeed = nOld + 1;
        WCPUI * pMalloc = NULL;
        if( nOld == 0 )
        {
            // original malloc.
            pMalloc = ( WCPUI * ) MyMalloc( 2674, ( nNeed ) * sizeof( WCPUI ) );
            if( pMalloc == NULL )
            {
                // Ignore trivial MyFree of into...
                return; // malloc error already messaged
            }
            pSurlLastGetLine->nRuleVector = 1;
            pSurlLastGetLine->pRuleVector = pMalloc;
        }
        else
        {
            // realloc.
            pMalloc = ( WCPUI * ) MyRealloc( 2674, ( nOld ) * sizeof( WCPUI ), pSurlLastGetLine->pRuleVector, ( nNeed ) * sizeof( WCPUI )  );
            if( pMalloc == NULL )
            {
                // Ignore trivial MyFree of into...
                return; // malloc error already messaged
            }
            pSurlLastGetLine->nRuleVector ++;
            pSurlLastGetLine->pRuleVector = pMalloc;
        }
        pSurlLastGetLine->pRuleVector[nOld].wcp = into;
        pSurlLastGetLine->pRuleVector[nOld].ui = ( SubType << 8 ) | ( LineType );
    }
}

void CUrl::SaveSearchEngineForms( )
{
    #if DO_DEBUG_CALLS
        Routine( L"481" );
    #endif

    wchar_t szFile[MAX_PATH];
    szFile[0] = NULL;
    // Let's try setting a default suggested output filename:
    wcscpy( szFile, L"Engines.txt" );

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrTitle = L"Save Search Engine URLs and Rules, and FORMs";
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


    // I need to create one wide text block for an easy file save:
    CWsb WsbAccum; // goes out of scope to delete.

    // static unsigned char UnicodePrefix[3] = { 0xff, 0xfe, 0 };
    static wchar_t UnicodePrefix[2] = { 0xfeff, 0 };
    WsbAccum.Add( UnicodePrefix );


    WsbAccum.Add( L"\r\n\r\n==== Part 1. Search Engines ====\r\n\r\n" );


    // output all entries in CSolSearchUrls, including the 3-digit GetOrdinal.

    {
        CoIt * pMalVector = CSolSearchUrls.GetSortedVector( CSOL_FORWARD );
        if( pMalVector != NULL )
        {
            size_t take = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector + take++;
                if( pCoIt->IsSentinel )
                    break;
                wchar_t * FullKey = CoItFullKey( pCoIt );

                // Prefix GET keyword, space.
                WsbAccum.Add( L"GET " );

                // Rest of output line is whole key, including GetOrdinal.
                WsbAccum.Addn( FullKey, wcslen( FullKey ) );

                // Finish the GET XXX http://... URL line.
                WsbAccum.Add( L"\r\n" );

                // Now there is a COneSurl hanging from the User.pVoid.
                // Get it, and get COneSurl member, CSol * pSolRuleSet;
                // It's crazy, a CoIt does not embed the index. Get it.
                // Wait. I only need index to get User.pVoid. I got it.

                COneSurl * pSurl = ( COneSurl * ) pCoIt->User.pVoid;
                // Never expect NULL here.
                if( pSurl != NULL )
                {
                    // Iterate rules, output after each GET line.
                    // Changing to use vectors, not a CSol.
                    // CSol * pSolRules = pSurl->pSolRuleSet;
                    // I can have a NULL here, when no rules.
                    if( pSurl->pRuleVector != NULL )
                    {
                        // These are the new members:
                        // size_t  nRuleVector;
                        // WCPUI * pRuleVector;
                        // pRuleVector[nOld].wcp = into;
                        // pRuleVector[nOld].ui = ( SubType << 8 ) | ( LineType );
                        size_t i = 0;
                        for( ;; )
                        {
                            if( i == pSurl->nRuleVector )
                                break;
                            WCPUI * pRule = pSurl->pRuleVector + i;
                            wchar_t * wcp = pRule->wcp;
                            unsigned int ui = pRule->ui;
                            int RuleType = ui & 0xff;
                            int RuleSubType = ui >> 8;

                            // Format lines looking like these examples, the
                            // criterion being my WideParseSearchEngineLine:
                            // good/bad url has prior token cleartext url
                            // good/bad url has anchor DOMAIN x

                            switch( RuleType )
                            {
                            // Oh, these two rules are kept as globals...
                            // case RULETYPE_KEEP:
                            //     WsbAccum.Add( L"  keep query result page as a good web page for reading\r\n" );
                            //     break;
                            // case RULETYPE_STUDY:
                            //     WsbAccum.Add( L"  studying engine - annotate facts in query result page\r\n" );
                            //     break;
                            case RULETYPE_GOOD:
                                WsbAccum.Add( L"  good url has " );
                                break;
                            case RULETYPE_BAD:
                                WsbAccum.Add( L"  bad url has " );
                                break;
                            case RULETYPE_MORE:
                                WsbAccum.Add( L"  more url has " );
                                break;
                            case RULETYPE_NONE_UNTIL:
                                WsbAccum.Add( L"  none good until " );
                                break;
                            case RULETYPE_NONE_AFTER:
                                WsbAccum.Add( L"  none good after " );
                                break;
                            default:
                                WsbAccum.Add( L"--ERROR: INVALID RULETYPE--" );
                                break;
                            }

                            switch( RuleSubType )
                            {
                            case RULESUBTYPE_UNUSED:
                                // This case is expected to happen...
                                // just for anchortext ( BALD ) I think.
                                break;
                            case RULESUBTYPE_NEXT:
                                WsbAccum.Add( L"next " );
                                break;
                            case RULESUBTYPE_PRIOR:
                                WsbAccum.Add( L"prior " );
                                break;
                            case RULESUBTYPE_ANCHOR:
                                WsbAccum.Add( L"anchor " );
                                break;
                            default:
                                WsbAccum.Add( L"--ERROR: INVALID RULESUBTYPE--" );
                                break;
                            }

                            // Show me the money.
                            WsbAccum.Add( wcp );

                            // Finish the RULE line.
                            WsbAccum.Add( L"\r\n" );

                            i++;
                        }
                    }
                    // Now do these two rules kept as globals.
                    // case RULETYPE_KEEP:
                    if( pSurl->KeepQueryResultPageAsGoodText )
                        WsbAccum.Add( L"  keep query result page as a good web page for reading\r\n" );
                    // case RULETYPE_STUDY:
                    if( pSurl->AnnotateFactsForStudy )
                        WsbAccum.Add( L"  studying engine - annotate facts in query result page\r\n" );
                }
                MyFree( 4054, zx, FullKey );
                FullKey = NULL;
            }
            MyFree( 4057, UNPREDICTABLE, pMalVector );
            pMalVector = NULL;
        }
    }


    WsbAccum.Add( L"\r\n\r\n==== Part 2. Reject URLs ====\r\n\r\n" );


    // output all entries in CSolRejectUrls, and all the entries under them.

    {
        CoIt * pMalVector = CSolRejectUrls.GetSortedVector( CSOL_FORWARD );
        if( pMalVector != NULL )
        {
            size_t take = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector + take++;
                if( pCoIt->IsSentinel )
                    break;
                wchar_t * FullKey = CoItFullKey( pCoIt );

                // Get at sub-pSol from that top CoIt item's User.pVoid.
                CSol * pSubSol = ( CSol * ) pCoIt->User.pVoid;
                // CYA.
                if( pSubSol != NULL )
                {
                    // Similar code as outside to do the next level of CSol.
                    CoIt * pMalVector2 = pSubSol->GetSortedVector( CSOL_FORWARD );
                    if( pMalVector2 != NULL )
                    {
                        size_t take2 = 0;
                        for( ;; )
                        {
                            CoIt * pCoIt2 = pMalVector2 + take2++;
                            if( pCoIt2->IsSentinel )
                                break;
                            wchar_t * FullKey2 = CoItFullKey( pCoIt2 );

                            // Prefix NOT keyword, space.
                            WsbAccum.Add( L"NOT " );

                            // First token of line is the result page domain.
                            WsbAccum.Addn( FullKey, wcslen( FullKey ) );

                            // separating space.
                            WsbAccum.Add( L" " );

                            // Second part of line is hit url match string.
                            WsbAccum.Addn( FullKey2, wcslen( FullKey2 ) );

                            // Finish line.
                            WsbAccum.Add( L"\r\n" );


                            MyFree( 4152, zx, FullKey2 );
                            FullKey2 = NULL;
                        }
                        MyFree( 4155, UNPREDICTABLE, pMalVector2 );
                        pMalVector2 = NULL;

                        // Post-same-first-token Separator
                        WsbAccum.Add( L"\r\n" );
                    }
                }
                MyFree( 4121, zx, FullKey );
                FullKey = NULL;
            }
            MyFree( 4124, UNPREDICTABLE, pMalVector );
            pMalVector = NULL;
        }
    }


    WsbAccum.Add( L"\r\n\r\n==== Part 3. New Forms ====\r\n\r\n" );


    {
        CoIt * pMalVector = CSolFormUrls.GetSortedVector( CSOL_FORWARD );
        if( pMalVector == NULL )
            return;
        size_t take = 0;
        for( ;; )
        {
            CoIt * pCoIt = pMalVector + take++;
            if( pCoIt->IsSentinel )
                break;

            // Obtain the OneForm fruit hanging from this CSol.

            COneForm * pOneForm = ( COneForm * )pCoIt->User.pVoid;
            if( pOneForm != NULL )
            {
                // Now process the OneForm fruit hanging from that pvoid.

                int WayBadForm = 0;

                // Gate off info for some surely-non-search form conditions.
                if( ( pOneForm->FormRejectedBitfield &
                ( FORM_REJECT_NO_TEXT_INPUT
                | FORM_REJECT_MULTIPLE_TEXT_INPUT
                | FORM_REJECT_HAS_TEXTAREA
                | FORM_REJECT_PASSWORD_INPUT
                | FORM_REJECT_FILE_INPUT ) ) != 0 )
                {
                    WayBadForm = 1;
                }

                // I might comment out both IFs when I desire to study all.

                // Or use this IF to only reject the worst candidates:
                if( ! WayBadForm )

                // Or use this IF to only distill the best candidates:
                // if( pOneForm->FormRejectedBitfield == 0 )

                {
                    // Pre-block Separator
                    WsbAccum.Add( L"\r\n" );

                    // Start every output with ACTION URL used to distinguish forms:
                    {
                        wchar_t * FullKey = CoItFullKey( pCoIt );
                        WsbAccum.Add( L"Location: " );
                        WsbAccum.Addn( FullKey, wcslen( FullKey ) );
                        WsbAccum.Add( L"\r\n" );
                        MyFree( 4205, UNPREDICTABLE, FullKey );
                        FullKey = NULL;
                    }

                    {
                        // Spill the CSol * pSolFormSpecies

                        if( pOneForm->pSolFormSpecies->nList > 2 ) // including tail ( #0 ) and head ( #1 )
                        {
                            WsbAccum.Add( L"\r\n  --Specimens--\r\n" );
                        }

                        CoIt * pMalVector2 = pOneForm->pSolFormSpecies->GetSortedVector( CSOL_FORWARD );
                        if( pMalVector2 != NULL )
                        {
                            size_t take2 = 0;
                            for( ;; )
                            {
                                CoIt * pCoIt2 = pMalVector2 + take2++;
                                if( pCoIt2->IsSentinel )
                                    break;

                                WsbAccum.Add( L"FYI 000 " );

                                wchar_t * FullKey = CoItFullKey( pCoIt2 );
                                WsbAccum.Addn( FullKey, wcslen( FullKey ) );
                                WsbAccum.Add( L"\r\n" );
                                MyFree( 4232, UNPREDICTABLE, FullKey );
                                FullKey = NULL;
                            }
                        }
                    }


                    // Show all the rejection reasons:
                    if( pOneForm->FormRejectedBitfield != 0 )
                    {
                        WsbAccum.Add( L"\r\n  --Possible errors--\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_JAVASCRIPT_ACTION_URL ) != 0 )
                            WsbAccum.Add( L"  Action URL on FORM uses javascript scheme\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_NO_ACTION_URL ) != 0 )
                            WsbAccum.Add( L"  No Action URL on FORM\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_BAD_ACTION_URL ) != 0 )
                            WsbAccum.Add( L"  Action URL on FORM was Uncanonicalizable\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_NO_METHOD ) != 0 )
                            WsbAccum.Add( L"  No METHOD attribute on FORM.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_NO_TEXT_INPUT ) != 0 )
                            WsbAccum.Add( L"  No TEXT type INPUT tag.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_MULTIPLE_TEXT_INPUT ) != 0 )
                            WsbAccum.Add( L"  Multiple TEXT type INPUT tags.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_NO_SUBMIT_IMAGE_BUTTON ) != 0 )
                            WsbAccum.Add( L"  No SUBMIT/IMAGE/BUTTON type INPUT tag.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_MULTIPLE_SUBMIT_IMAGE_BUTTON ) != 0 )
                            WsbAccum.Add( L"  Multiple SUBMIT/IMAGE/BUTTON type INPUT tags.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_HAS_TEXTAREA ) != 0 )
                            WsbAccum.Add( L"  Presence of TEXTAREA tag in form.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_PASSWORD_INPUT ) != 0 )
                            WsbAccum.Add( L"  Presence of PASSWORD type INPUT tag.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_FILE_INPUT ) != 0 )
                            WsbAccum.Add( L"  Presence of FILE type INPUT tag.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_UNKNOWN_TYPE_INPUT ) != 0 )
                            WsbAccum.Add( L"  Presence of UNKNOWN type INPUT tag.\r\n" );

                        if( ( pOneForm->FormRejectedBitfield & FORM_REJECT_UNNAMED_TEXT_INPUT ) != 0 )
                            WsbAccum.Add( L"  TEXT type INPUT tag has no NAME.\r\n" );

                    }



                    {
                        // Spill the CSol * pSolFormAtoms

                        if( pOneForm->pSolFormAtoms->nList > 2 ) // including tail ( #0 ) and head ( #1 )
                        {
                            WsbAccum.Add( L"\r\n  --Alternatives--\r\n" );
                        }

                        CoIt * pMalVector2 = pOneForm->pSolFormAtoms->GetSortedVector( CSOL_FORWARD );
                        if( pMalVector2 != NULL )
                        {
                            size_t take2 = 0;
                            for( ;; )
                            {
                                CoIt * pCoIt2 = pMalVector2 + take2++;
                                if( pCoIt2->IsSentinel )
                                    break;

                                wchar_t * FullKey = CoItFullKey( pCoIt2 );
                                WsbAccum.Add( L"  " ); // indent 2
                                WsbAccum.Addn( FullKey, wcslen( FullKey ) );
                                WsbAccum.Add( L"\r\n" );
                                MyFree( 4309, UNPREDICTABLE, FullKey );
                                FullKey = NULL;
                            }
                        }
                    }
                    // Post-block Separator
                    WsbAccum.Add( L"\r\n" );
                }
            }
        }
        MyFree( 4319, UNPREDICTABLE, pMalVector );
        pMalVector = NULL;
    }

    WsbAccum.Add( L"\r\n\r\n" );

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


