// This is file: CWsb.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

CWsb::CWsb( )
{
    #if DO_DEBUG_CALLS
        Routine( L"279" );
    #endif
    WsbEnterCrisis( );
    szBuf = NULL;
    nMal = 0;
    StrLen = nLen = 0;
    WsbLeaveCrisis( );
}

CWsb::~CWsb( )
{
    #if DO_DEBUG_CALLS
        Routine( L"280" );
    #endif
    WsbEnterCrisis( );
    Reset( );
    WsbLeaveCrisis( );
}

void CWsb::Reset( )
{
    #if DO_DEBUG_CALLS
        Routine( L"281" );
    #endif
    WsbEnterCrisis( );
    if( szBuf != NULL )
    {
        MyFree( 30, nMal * sizeof( wchar_t ), szBuf );
        szBuf = NULL;
    }
    nMal = 0;
    StrLen = nLen = 0;
    WsbLeaveCrisis( );
}

void CWsb::Add( wchar_t * szNew )
{
    #if DO_DEBUG_CALLS
        // Routine( L"282" );
    #endif
    WsbEnterCrisis( );
    if( szNew == NULL )
    {
        ProgramError( L"CWsb Add: szNew == NULL" );
        WsbLeaveCrisis( );
        return;
    }
    size_t nNewLen = wcslen( szNew );
    Addn( szNew, nNewLen );
    WsbLeaveCrisis( );
    return;
}

void CWsb::AddWsb( CWsb * pWsb )
{
    #if DO_DEBUG_CALLS
        Routine( L"283" );
    #endif
    WsbEnterCrisis( );
    if( pWsb == NULL )
    {
        ProgramError( L"CWsb AddWsb: pWsb == NULL" );
        WsbLeaveCrisis( );
        return;
    }
    size_t nNewLen = pWsb->StrLen;
    if( nNewLen > 0 )
        Addn( pWsb->szBuf, nNewLen );
    WsbLeaveCrisis( );
    return;
}

void CWsb::Addn( wchar_t * szNew, size_t nNewLen )
{
    #if DO_DEBUG_CALLS
        // Routine( L"284" );
    #endif
    WsbEnterCrisis( );
    if( szNew == NULL )
    {
        ProgramError( L"CWsb Addn: szNew == NULL" );
        WsbLeaveCrisis( );
        return;
    }
    if( nNewLen > 0 )
    {
        if( ! MakeFit( nNewLen ) )
        {
            WsbLeaveCrisis( );
            return; // failure
        }
        if( szBuf != NULL )
        {
            memcpy( szBuf + nLen, szNew, nNewLen * sizeof ( wchar_t ) );
            nLen += nNewLen;
            StrLen = nLen;
            szBuf[ nLen ] = NULL;
        }
    }
    WsbLeaveCrisis( );
    return;
}

void CWsb::AddnByte( BYTE * szNew, size_t nNewLen )
{
    #if DO_DEBUG_CALLS
        Routine( L"285" );
    #endif
    WsbEnterCrisis( );
    if( szNew == NULL )
    {
        ProgramError( L"CWsb AddnByte: szNew == NULL" );
        WsbLeaveCrisis( );
        return;
    }
    if( nNewLen > 0 )
    {
        if( ! MakeFit( nNewLen ) )
        {
            WsbLeaveCrisis( );
            return; // failure
        }
        if( szBuf != NULL )
        {
            int i = nNewLen;
            wchar_t * into = szBuf + nLen;
            BYTE * from = szNew;
            for( ;; )
            {
                *into++ = ( wchar_t ) *from++;
                if( --i == 0 )
                    break;
            }
            nLen += nNewLen;
            StrLen = nLen;
            szBuf[ nLen ] = NULL;
        }
    }
    WsbLeaveCrisis( );
    return;
}

int CWsb::MakeFit( size_t nNewLen )
{
    #if DO_DEBUG_CALLS
        // Routine( L"286" ); // Too frequent to spew.
    #endif
    WsbEnterCrisis( );
    size_t nNeed = nLen + nNewLen + 1;
    if( nNeed > nMal )
    {
        nNeed += ( nNeed / 8 ); // grow liberally 125 %
        void * pvm = MyRealloc( 9998, nMal * sizeof( wchar_t ), szBuf, nNeed * sizeof( wchar_t ) );
        if( pvm == NULL )
        {
            // do not change original szBuf, nMal
            WsbLeaveCrisis( );
            return 0; // failure - already messaged
        }
        szBuf = ( wchar_t * ) pvm;
        nMal = nNeed;
    }
    WsbLeaveCrisis( );
    return 1; // success
}

wchar_t * CWsb::GetBuffer( size_t * nMallocBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"287" );
    #endif
    WsbEnterCrisis( );
    size_t RetnLen = 0;
    wchar_t * pMalloc = NULL;
    if( szBuf != NULL )
    {
        pMalloc = ( wchar_t * ) MyMalloc( 118, ( nLen + 1 ) * sizeof( wchar_t ) );
        if( pMalloc != NULL )
        {
            memcpy( pMalloc, szBuf, ( nLen + 1 ) * sizeof( wchar_t ) );
            RetnLen = nLen;
        }
    }
    WsbLeaveCrisis( );
    *nMallocBuf = RetnLen;
    return pMalloc;
}

void CWsb::Insert( wchar_t * szNew, int nFinalNL )
{
    #if DO_DEBUG_CALLS
        Routine( L"288" );
    #endif
    WsbEnterCrisis( );
    size_t nOldLen = nLen;
    size_t nNewLen = wcslen( szNew );
    size_t nAugLen = nNewLen + nFinalNL * 2; // each CR LF
    if( ! MakeFit( nAugLen ) )
    {
        WsbLeaveCrisis( );
        return; // failure
    }
    size_t i = nOldLen; // right at existing terminating NULL
    size_t j = i + nAugLen; // above i by the amount of shift
    // old part moves to the top.
    for( ;; )
    {
        szBuf[ j-- ] = szBuf[ i ]; // first loop copies final NULL
        if( i == 0 )
            break;
        i--;
    }
    // I left j sitting at nFinalNL location.
    for( i = 0; ( int ) i < nFinalNL; i++ )
    {
        szBuf[ j-- ] = '\n'; // LF first
        szBuf[ j-- ] = '\r'; // CR below
    }
    // new part goes into the bottom.
    memcpy( szBuf, szNew, nNewLen * sizeof ( wchar_t ) );
    nLen += nAugLen;
    StrLen = nLen;
    szBuf[ nLen ] = NULL;
    WsbLeaveCrisis( );
    return;
}
