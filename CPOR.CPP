// This is file: CPor.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// Adapted from a web source. Original comments:

/* This is the Porter stemming algorithm, coded up as thread-safe ANSI C
   by the author.

   It may be be regarded as cononical, in that it follows the algorithm
   presented in

   Porter, 1980, An algorithm for suffix stripping, Program, Vol. 14,
   no. 3, pp 130-137,

   only differing from it at the points maked --DEPARTURE-- below.

   See also http://www.tartarus.org/~martin/PorterStemmer

   The algorithm as described in the paper could be exactly replicated
   by adjusting the points of DEPARTURE, but this is barely necessary,
   because ( a ) the points of DEPARTURE are definitely improvements, and
   ( b ) no encoding of the Porter stemmer I have seen is anything like
   as exact as this version, even with the points of DEPARTURE!

   You can compile it on Unix with 'gcc -O3 -o stem stem.c' after which
   'stem' takes a list of inputs and sends the stemmed equivalent to
   stdout.

   The algorithm as encoded here is particularly fast.

   Release 2 ( the more old-fashioned, non-thread-safe version may be
   regarded as release 1. )
*/


/* Member b is a buffer holding a word to be stemmed. The letters are in
   b[0], b[1] ... ending at b[z->k]. Member k is readjusted downwards as
   the stemming progresses. Zero termination is not in fact used in the
   algorithm.

   Note that only lower case sequences are stemmed. Forcing to lower case
   should be done before stem( ... ) is called.


   Typical usage is:

       struct stemmer * z = create_stemmer( );
       char b[] = "pencils";
       int res = stem( z, b, 6 );
           /- stem the 7 characters of b[0] to b[6]. The result, res,
              will be 5 ( the 's' is removed ). -/
       free_stemmer( z );
*/


// This will not be used in threads, except main pgm thread:
// CRITICAL_SECTION CPorCriticalSection;

CPor::CPor( )
{
    #if DO_DEBUG_CALLS
        Routine( L"206" );
    #endif
    m_z = ( struct stemmer * ) MyMalloc( 64, sizeof( struct stemmer ) );
    // InitializeCriticalSection( & CPorCriticalSection );
}
CPor::~CPor( )
{
    #if DO_DEBUG_CALLS
        Routine( L"207" );
    #endif
    MyFree( 69, zx, m_z );
    m_z = NULL;
    // DeleteCriticalSection( & CPorCriticalSection );
}

void CPor::Stem( wchar_t * s, int StrLen )
{
    #if DO_DEBUG_CALLS
        Routine( L"208" );
    #endif
    // My caller will pass me the wcslen. I fix to N-1 for Porter.

    // PorEnterCrisis( );

    // Because I have expanded from USASCII to ISOLATIN and BEYOND,
    // do not ask the stemmer to work on anything that is not pure
    // USASCII lowercase, from s[0] to s[StrLen-1] inclusive;

    int i = 0;
    for( ;; )
    {
        if( s[i] < 'a'
        || s[i] > 'z' )
            return; // without attempting to modify in/out string.
        if( ++i == StrLen )
            break;
    }

    // His algorithm gets N-1 instead of N, and returns a new length:

    s[stem( m_z, s, StrLen - 1 ) + 1] = 0;

    /* the previous line calls the stemmer and uses its result to
    zero-terminate the string in s */

    // PorLeaveCrisis( );
}


/* cons( z, i ) is TRUE <=> b[i] is a consonant. ( 'b' means 'z->b', but here
   and below we drop 'z->' in comments.
*/

int CPor::cons( struct stemmer * z, int i )
{
    #if DO_DEBUG_CALLS
        Routine( L"209" );
    #endif
   switch ( z->b[i] )
   {  case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
      case 'y': return ( i == 0 ) ? TRUE : !cons( z, i - 1 );
      default: return TRUE;
   }
}

/* m( z ) measures the number of consonant sequences between 0 and j. if c is
   a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
   presence,

      <c><v>       gives 0
      <c>vc<v>     gives 1
      <c>vcvc<v>   gives 2
      <c>vcvcvc<v> gives 3
      ....
*/

int CPor::m( struct stemmer * z )
{
    #if DO_DEBUG_CALLS
        Routine( L"210" );
    #endif
   int n = 0;
   int i = 0;
   int j = z->j;
   while( TRUE )
   {  if ( i > j ) return n;
      if ( ! cons( z, i ) ) break; i++;
   }
   i++;
   while( TRUE )
   {  while( TRUE )
      {  if ( i > j ) return n;
            if ( cons( z, i ) ) break;
            i++;
      }
      i++;
      n++;
      while( TRUE )
      {  if ( i > j ) return n;
         if ( ! cons( z, i ) ) break;
         i++;
      }
      i++;
   }
}

/* vowelinstem( z ) is TRUE <=> 0,...j contains a vowel */

int CPor::vowelinstem( struct stemmer * z )
{
   int j = z->j;
   int i; for ( i = 0; i <= j; i++ ) if ( ! cons( z, i ) ) return TRUE;
   return FALSE;
}

/* doublec( z, j ) is TRUE <=> j,( j-1 ) contain a double consonant. */

int CPor::doublec( struct stemmer * z, int j )
{
   wchar_t * b = z->b;
   if ( j < 1 ) return FALSE;
   if ( b[j] != b[j - 1] ) return FALSE;
   return cons( z, j );
}

/* cvc( z, i ) is TRUE <=> i-2,i-1,i has the form consonant - vowel - consonant
   and also if the second c is not w,x or y. this is used when trying to
   restore an e at the end of a short word. e.g.

      cav( e ), lov( e ), hop( e ), crim( e ), but
      snow, box, tray.

*/

int CPor::cvc( struct stemmer * z, int i )
{  if ( i < 2 || !cons( z, i ) || cons( z, i - 1 ) || !cons( z, i - 2 ) ) return FALSE;
   {  int ch = z->b[i];
      if ( ch  == 'w' || ch == 'x' || ch == 'y' ) return FALSE;
   }
   return TRUE;
}

/* ends( z, s ) is TRUE <=> 0,...k ends with the string s. */

int CPor::ends( struct stemmer * z, wchar_t * s )
{  int length = s[0];
   wchar_t * b = z->b;
   int k = z->k;
   if ( s[length] != b[k] ) return FALSE; /* tiny speed-up */
   if ( length > k + 1 ) return FALSE;
   if ( memcmp( b + k - length + 1, s + 1, length ) != 0 ) return FALSE;
   z->j = k-length;
   return TRUE;
}

/* setto( z, s ) sets ( j+1 ),...k to the wchar_tacters in the string s, readjusting
   k. */

void CPor::setto( struct stemmer * z, wchar_t * s )
{  int length = s[0];
   int j = z->j;
   memmove( z->b + j + 1, s + 1, length );
   z->k = j+length;
}

/* r( z, s ) is used further down. */

void CPor::r( struct stemmer * z, wchar_t * s ) { if ( m( z ) > 0 ) setto( z, s ); }

/* step1ab( z ) gets rid of plurals and -ed or -ing. e.g.

       caresses->caress
       ponies  ->poni
       ties    ->ti
       caress  ->caress
       cats    ->cat

       feed    ->feed
       agreed  ->agree
       disabled->disable

       matting ->mat
       mating  ->mate
       meeting ->meet
       milling ->mill
       messing ->mess

       meetings->meet

*/

void CPor::step1ab( struct stemmer * z )
{
   wchar_t * b = z->b;
   if ( b[z->k] == 's' )
   {  if ( ends( z, L"\04" L"sses" ) ) z->k -= 2; else
      if ( ends( z, L"\03" L"ies" ) ) setto( z, L"\01" L"i" ); else
      if ( b[z->k - 1] != 's' ) z->k--;
   }
   if ( ends( z, L"\03" L"eed" ) ) { if ( m( z ) > 0 ) z->k--; } else
   if ( ( ends( z, L"\02" L"ed" ) || ends( z, L"\03" L"ing" ) ) && vowelinstem( z ) )
   {  z->k = z->j;
      if ( ends( z, L"\02" L"at" ) ) setto( z, L"\03" L"ate" ); else
      if ( ends( z, L"\02" L"bl" ) ) setto( z, L"\03" L"ble" ); else
      if ( ends( z, L"\02" L"iz" ) ) setto( z, L"\03" L"ize" ); else
      if ( doublec( z, z->k ) )
      {  z->k--;
         {  int ch = b[z->k];
            if ( ch == 'l' || ch == 's' || ch == 'z' ) z->k++;
         }
      }
      else if ( m( z ) == 1 && cvc( z, z->k ) ) setto( z, L"\01" L"e" );
   }
}

/* step1c( z ) turns terminal y to i when there is another vowel in the stem. */

void CPor::step1c( struct stemmer * z )
{
   if ( ends( z, L"\01" L"y" ) && vowelinstem( z ) ) z->b[z->k] = 'i';
}


/* step2( z ) maps double suffices to single ones. so -ization ( = -ize plus
   -ation ) maps to -ize etc. note that the string before the suffix must give
   m( z ) > 0. */

void CPor::step2( struct stemmer * z ) { switch ( z->b[z->k-1] )
{
   case 'a': if ( ends( z, L"\07" L"ational" ) ) { r( z, L"\03" L"ate" ); break; }
             if ( ends( z, L"\06" L"tional" ) ) { r( z, L"\04" L"tion" ); break; }
             break;
   case 'c': if ( ends( z, L"\04" L"enci" ) ) { r( z, L"\04" L"ence" ); break; }
             if ( ends( z, L"\04" L"anci" ) ) { r( z, L"\04" L"ance" ); break; }
             break;
   case 'e': if ( ends( z, L"\04" L"izer" ) ) { r( z, L"\03" L"ize" ); break; }
             break;
   case 'l': if ( ends( z, L"\03" L"bli" ) ) { r( z, L"\03" L"ble" ); break; } /*-DEPARTURE-*/

 /* To match the published algorithm, replace this line with
    case 'l': if ( ends( z, L"\04" L"abli" ) ) { r( z, L"\04" L"able" ); break; } */

             if ( ends( z, L"\04" L"alli" ) ) { r( z, L"\02" L"al" ); break; }
             if ( ends( z, L"\05" L"entli" ) ) { r( z, L"\03" L"ent" ); break; }
             if ( ends( z, L"\03" L"eli" ) ) { r( z, L"\01" L"e" ); break; }
             if ( ends( z, L"\05" L"ousli" ) ) { r( z, L"\03" L"ous" ); break; }
             break;
   case 'o': if ( ends( z, L"\07" L"ization" ) ) { r( z, L"\03" L"ize" ); break; }
             if ( ends( z, L"\05" L"ation" ) ) { r( z, L"\03" L"ate" ); break; }
             if ( ends( z, L"\04" L"ator" ) ) { r( z, L"\03" L"ate" ); break; }
             break;
   case 's': if ( ends( z, L"\05" L"alism" ) ) { r( z, L"\02" L"al" ); break; }
             if ( ends( z, L"\07" L"iveness" ) ) { r( z, L"\03" L"ive" ); break; }
             if ( ends( z, L"\07" L"fulness" ) ) { r( z, L"\03" L"ful" ); break; }
             if ( ends( z, L"\07" L"ousness" ) ) { r( z, L"\03" L"ous" ); break; }
             break;
   case 't': if ( ends( z, L"\05" L"aliti" ) ) { r( z, L"\02" L"al" ); break; }
             if ( ends( z, L"\05" L"iviti" ) ) { r( z, L"\03" L"ive" ); break; }
             if ( ends( z, L"\06" L"biliti" ) ) { r( z, L"\03" L"ble" ); break; }
             break;
   case 'g': if ( ends( z, L"\04" L"logi" ) ) { r( z, L"\03" L"log" ); break; } /*-DEPARTURE-*/

 /* To match the published algorithm, delete this line */

} }

/* step3( z ) deals with -ic-, -full, -ness etc. similar strategy to step2. */

void CPor::step3( struct stemmer * z ) { switch ( z->b[z->k] )
{
   case 'e': if ( ends( z, L"\05" L"icate" ) ) { r( z, L"\02" L"ic" ); break; }
             if ( ends( z, L"\05" L"ative" ) ) { r( z, L"\00" L"" ); break; }
             if ( ends( z, L"\05" L"alize" ) ) { r( z, L"\02" L"al" ); break; }
             break;
   case 'i': if ( ends( z, L"\05" L"iciti" ) ) { r( z, L"\02" L"ic" ); break; }
             break;
   case 'l': if ( ends( z, L"\04" L"ical" ) ) { r( z, L"\02" L"ic" ); break; }
             if ( ends( z, L"\03" L"ful" ) ) { r( z, L"\00" L"" ); break; }
             break;
   case 's': if ( ends( z, L"\04" L"ness" ) ) { r( z, L"\00" L"" ); break; }
             break;
} }

/* step4( z ) takes off -ant, -ence etc., in context <c>vcvc<v>. */

void CPor::step4( struct stemmer * z )
{  switch ( z->b[z->k-1] )
   {  case 'a': if ( ends( z, L"\02" L"al" ) ) break; return;
      case 'c': if ( ends( z, L"\04" L"ance" ) ) break;
                if ( ends( z, L"\04" L"ence" ) ) break; return;
      case 'e': if ( ends( z, L"\02" L"er" ) ) break; return;
      case 'i': if ( ends( z, L"\02" L"ic" ) ) break; return;
      case 'l': if ( ends( z, L"\04" L"able" ) ) break;
                if ( ends( z, L"\04" L"ible" ) ) break; return;
      case 'n': if ( ends( z, L"\03" L"ant" ) ) break;
                if ( ends( z, L"\05" L"ement" ) ) break;
                if ( ends( z, L"\04" L"ment" ) ) break;
                if ( ends( z, L"\03" L"ent" ) ) break; return;
      case 'o': if ( ends( z, L"\03" L"ion" ) && ( z->b[z->j] == 's' || z->b[z->j] == 't' ) ) break;
                if ( ends( z, L"\02" L"ou" ) ) break; return;
                /* takes care of -ous */
      case 's': if ( ends( z, L"\03" L"ism" ) ) break; return;
      case 't': if ( ends( z, L"\03" L"ate" ) ) break;
                if ( ends( z, L"\03" L"iti" ) ) break; return;
      case 'u': if ( ends( z, L"\03" L"ous" ) ) break; return;
      case 'v': if ( ends( z, L"\03" L"ive" ) ) break; return;
      case 'z': if ( ends( z, L"\03" L"ize" ) ) break; return;
      default: return;
   }
   if ( m( z ) > 1 ) z->k = z->j;
}

/* step5( z ) removes a final -e if m( z ) > 1, and changes -ll to -l if
   m( z ) > 1. */

void CPor::step5( struct stemmer * z )
{
   wchar_t * b = z->b;
   z->j = z->k;
   if ( b[z->k] == 'e' )
   {  int a = m( z );
      if ( a > 1 || a == 1 && !cvc( z, z->k - 1 ) ) z->k--;
   }
   if ( b[z->k] == 'l' && doublec( z, z->k ) && m( z ) > 1 ) z->k--;
}

/* In stem( z, b, k ), b is a wchar_t pointer, and the string to be stemmed is
   from b[0] to b[k] inclusive.  Possibly b[k+1] == '\0', but it is not
   important. The stemmer adjusts the wchar_tacters b[0] ... b[k] and returns
   the new end-point of the string, k'. Stemming never increases word
   length, so 0 <= k' <= k.
*/

int CPor::stem( struct stemmer * z, wchar_t * b, int k )
{
   if ( k <= 1 ) return k; /*-DEPARTURE-*/
   z->b = b; z->k = k; /* copy the parameters into z */

   /* With this line, strings of length 1 or 2 don't go through the
      stemming process, although no mention is made of this in the
      published algorithm. Remove the line to match the published
      algorithm. */

   step1ab( z ); step1c( z ); step2( z ); step3( z ); step4( z ); step5( z );
   return z->k;
}

