// This is file: CInt.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

CInt::CInt( )
{
    #if DO_DEBUG_CALLS
        Routine( L"487" );
    #endif
    IntEnterCrisis( );
    pList = NULL;
    nMal = 0;
    nList = nLen = 0;
    IntLeaveCrisis( );
}

CInt::~CInt( )
{
    #if DO_DEBUG_CALLS
        Routine( L"488" );
    #endif
    IntEnterCrisis( );
    Reset( );
    IntLeaveCrisis( );
}

void CInt::Reset( )
{
    #if DO_DEBUG_CALLS
        Routine( L"489" );
    #endif
    IntEnterCrisis( );
    if( pList != NULL )
    {
        MyFree( 30, nMal * sizeof( int ), pList );
        pList = NULL;
    }
    nMal = 0;
    nList = nLen = 0;
    IntLeaveCrisis( );
}

int CInt::Found( int Value )
{
    #if DO_DEBUG_CALLS
        Routine( L"497" );
    #endif
    IntEnterCrisis( );
    int bFound = 0;
    size_t i = 0;
    for( ;; )
    {
        if( i == nLen )
            break;
        if( pList[i] == Value )
        {
            bFound = 1;
            break;
        }
        i ++;
    }
    IntLeaveCrisis( );
    return bFound;
}

void CInt::Add( int Value )
{
    #if DO_DEBUG_CALLS
        Routine( L"492" );
    #endif
    IntEnterCrisis( );
    if( ! Found( Value ) )
    {
        if( ! MakeFit( ) )
        {
            IntLeaveCrisis( );
            return; // failure
        }
        if( pList != NULL )
        {
            pList[ nLen ] = Value;
            nLen ++;
            nList = nLen;
        }
    }
    IntLeaveCrisis( );
    return;
}


int CInt::MakeFit( )
{
    #if DO_DEBUG_CALLS
        Routine( L"494" );
    #endif
    IntEnterCrisis( );
    size_t nNeed = nLen + 1;
    if( nNeed > nMal )
    {
        nNeed += ( nNeed / 8 ); // grow liberally 125 %
        void * pvm = MyRealloc( 9989, nMal * sizeof( int ), pList, nNeed * sizeof( int ) );
        if( pvm == NULL )
        {
            // do not change original pList, nMal
            IntLeaveCrisis( );
            return 0; // failure - already messaged
        }
        pList = ( int * ) pvm;
        nMal = nNeed;
    }
    IntLeaveCrisis( );
    return 1; // success
}

int * CInt::GetList( size_t * nMallocBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"495" );
    #endif
    IntEnterCrisis( );
    size_t RetnLen = 0;
    int * pMalloc = NULL;
    if( pList != NULL )
    {
        pMalloc = ( int * ) MyMalloc( 118, nLen * sizeof( int ) );
        if( pMalloc != NULL )
        {
            memcpy( pMalloc, pList, nLen * sizeof( int ) );
            RetnLen = nLen;
        }
    }
    IntLeaveCrisis( );
    *nMallocBuf = RetnLen;
    return pMalloc;
}
