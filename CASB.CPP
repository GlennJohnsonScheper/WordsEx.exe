// This is file: CAsb.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

CAsb::CAsb( )
{
    #if DO_DEBUG_CALLS
        Routine( L"100" );
    #endif
    AsbEnterCrisis( );
    szBuf = NULL;
    nMal = 0;
    StrLen = nLen = 0;
    AsbLeaveCrisis( );
}

CAsb::~CAsb( )
{
    #if DO_DEBUG_CALLS
        Routine( L"101" );
    #endif
    AsbEnterCrisis( );
    if( szBuf != NULL )
    {
        MyFree( 23, nMal * sizeof( BYTE ), szBuf );
        szBuf = NULL;
    }
    nMal = 0;
    StrLen = nLen = 0;
    AsbLeaveCrisis( );
}

void CAsb::Add( BYTE * szNew )
{
    #if DO_DEBUG_CALLS
        Routine( L"102" );
    #endif
    AsbEnterCrisis( );
    if( szNew == NULL )
    {
        ProgramError( L"CAsb Add: szNew == NULL" );
        AsbLeaveCrisis( );
        return;
    }
    size_t nNewLen = strlen( ( char * ) szNew );
    Addn( szNew, nNewLen );
    AsbLeaveCrisis( );
    return;
}

void CAsb::Addn( BYTE * szNew, size_t nNewLen )
{
    #if DO_DEBUG_CALLS
        Routine( L"103" );
    #endif
    AsbEnterCrisis( );
    if( szNew == NULL )
    {
        ProgramError( L"CAsb Addn: szNew == NULL" );
        AsbLeaveCrisis( );
        return;
    }
    if( nNewLen > 0 )
    {
        if( ! MakeFit( nNewLen ) )
        {
            AsbLeaveCrisis( );
            return; // failure
        }
        if( szBuf != NULL )
        {
            memcpy( szBuf + nLen, szNew, nNewLen * sizeof ( BYTE ) );
            nLen += nNewLen;
            StrLen = nLen;
            szBuf[ nLen ] = NULL;
        }
    }
    AsbLeaveCrisis( );
    return;
}

void CAsb::AddnWide( wchar_t * szNew, size_t nNewLen )
{
    #if DO_DEBUG_CALLS
        Routine( L"104" );
    #endif
    AsbEnterCrisis( );
    if( szNew == NULL )
    {
        ProgramError( L"CAsb AddnWide: szNew == NULL" );
        AsbLeaveCrisis( );
        return;
    }
    if( nNewLen > 0 )
    {
        if( ! MakeFit( nNewLen ) )
        {
            AsbLeaveCrisis( );
            return; // failure
        }
        if( szBuf != NULL )
        {
            int i = nNewLen;
            BYTE * into = szBuf + nLen;
            wchar_t * from = szNew;
            for( ;; )
            {
                // I am not doing a full UNICODE to MBS conversion.
                // But it is not right to truncate shorts to chars.
                // Either for above 0x7f or 0xff, replace with '?'.
                // But do that test while still wide.

                wchar_t wc = *from++;
                if( wc > 0x7f
                ||  wc < 0x20 )
                    *into++ = '?';
                else
                    *into++ = ( BYTE ) wc;
                if( --i == 0 )
                    break;
            }
            nLen += nNewLen;
            StrLen = nLen;
            szBuf[ nLen ] = NULL;
        }
    }
    AsbLeaveCrisis( );
    return;
}

int CAsb::MakeFit( size_t nNewLen )
{
    #if DO_DEBUG_CALLS
        Routine( L"105" );
    #endif
    AsbEnterCrisis( );
    size_t nNeed = nLen + nNewLen + 1;
    if( nNeed > nMal )
    {
        nNeed += ( nNeed / 8 ); // grow liberally 125 %
        void * pvm = MyRealloc( 9991, nMal * sizeof( BYTE ), szBuf, nNeed * sizeof( BYTE ) );
        if( pvm == NULL )
        {
            // do not change original szBuf, nMal
            AsbLeaveCrisis( );
            return 0; // failure - already messaged
        }
        szBuf = ( BYTE * ) pvm;
        nMal = nNeed;
    }
    AsbLeaveCrisis( );
    return 1; // success
}

BYTE * CAsb::GetBuffer( size_t * nMallocBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"106" );
    #endif
    AsbEnterCrisis( );
    size_t RetnLen = 0;
    BYTE * pMalloc = NULL;
    if( szBuf != NULL )
    {
        pMalloc = ( BYTE * ) MyMalloc( 118, ( nLen + 1 ) * sizeof( BYTE ) );
        if( pMalloc != NULL )
        {
            memcpy( pMalloc, szBuf, ( nLen + 1 ) * sizeof( BYTE ) );
            RetnLen = nLen;
        }
    }
    AsbLeaveCrisis( );
    *nMallocBuf = RetnLen;
    return pMalloc;
}

