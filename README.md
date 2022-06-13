# WordsEx.exe
The code last built with VS6 or VS8; It does not build with VS2019.
Just a 5MB archeological tell, to show how early netizens did feed.

Although it only garnered 2 stars from CNET editors, my 2008 freeware application
"Words,Extended", see https://download.cnet.com/WordsEx/3000-2356_4-10800328.html
demonstrates many bleeding-edge features for its day, and an unpaid labor-of-love:

-Spurning the untrustworthy libraries of the day, every C++ wheel was hand-coded.

-It grew out of one MSDN "tear" demo code, how to "tear" a page off the internet.

-Collections were held in Red-Black lists; My tutorial code was linked from NIST.

-I mastered WChars, Codepages, language recognition, sentence boundary detection.

-WordsEx ex/imports a control script for parsing Search Engine result pages. E.g.
GET 705 http://www.bing.com/search?scope=web&mkt=en-US&FORM=QBLH&q=
  good url has next TAG cite
  more url has anchortext NUMBER
GET 710 http://www.google.com/search?hl=en&ie=ISO-8859-1&btnG=Google+Search&q=
  good url has next TAG cite
  more url has anchortext NUMBER

-WordsEx tabulated www word frequencies of 30 languages. View its exported lists!

-There were many intelligence discrimination heuristics: tables of consonant and
vowel sequence frequencies for real words to discard nonsense tokens; valuations
assigned to words, sentences, paragraphs, and documents; ridding html navigation;
word search with or without Porter stemming, giving KWIC, sentence, or paragraph
results; The gem was a prioritized "best sentences" report, which one could scan
and click through to read the original document; and all using smooth-scrolling!
