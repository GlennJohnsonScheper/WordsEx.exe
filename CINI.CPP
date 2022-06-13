// This is file: CIni.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

    // Win32 recommends using _beginthread when calling C runtime libs.
    // WinCE does not use _beginthread, so make a CreateThread version.

    // In VC++ 6.0 case of _beginthread' : undeclared identifier,
    // set Project Settings, C++ Code Generation, RunTime Library
    // to multithreaded.

START_THREAD_PENDING * pListStartThreadPending = NULL;


// Regarding g_bStopAllThreads, the button that sets it, and my three
// InitiateXXX routines that clear it, are all in the main pgm thread.
// The same after DELETE key, that may set pFruit->m_StopThisThread.
// Boy, I took a beating on that Delete/Stop code. Did it differently.


CIni::CIni( )
{
    #if DO_DEBUG_CALLS
        Routine( L"181" );
    #endif
}
CIni::~CIni( )
{
    #if DO_DEBUG_CALLS
        Routine( L"182" );
    #endif
}


void CIni::OnThreadEnding( WPARAM MyWParam, LPARAM MyLParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"183" );
    #endif
    size_t ThreadParam = MyLParam;
    // WParam says which kind of thread is ending:
    // WParam ENUM_ADDSEARCH: The ThreadParam is an index into CSol CSolUserQuerys;
    // WParam ENUM_ADDPAGE: The ThreadParam is an index into CSol CSolUserFetchs;
    // WParam ENUM_ADDFIND : The ThreadParam is an index into CSol CSolUserKwics;
    // WParam ENUM_ADDCACHE : The ThreadParam is not used.
    // WParam ENUM_ADDDIRECTORY : The ThreadParam is an index into CSol CSolUserFolders;
    // WParam ENUM_ADDPHRASES : The ThreadParam is not used.

    // Spew( L"Acting on Threadend msg." );

    switch( MyWParam )
    {
    case ENUM_ADDDIRECTORY:
        {
            COneFolder * pFolder = ( COneFolder * ) CSolUserFolders.GetUserpVoid( ThreadParam );
            if( pFolder == NULL )
            {
                ProgramError( L"OnThreadEnding pFolder NULL" );
            }
            else
            {
                pFolder->m_WorkInProgress = 0;
            }
        }
        g_nCountOfFolderThreads --;
        break;
    case ENUM_ADDCACHE:
        // Spew( L"Clearing Cache WorkInP." );
        OnlyCache.m_WorkInProgress = 0;
        g_nCountOfCacheThreads --;
        break;
    case ENUM_ADDPHRASES:
        #if DO_DEBUG_PHRASE
            ; Spew( L"Clearing AddPhrases WorkInProgress." );
        #endif
        OnlyPhrases.m_WorkInProgress = 0;
        g_nCountOfPhrasesThreads --;
        break;
    case ENUM_ADDSEARCH:
        {
            COneQuery * pQuery = ( COneQuery * ) CSolUserQuerys.GetUserpVoid( ThreadParam );
            if( pQuery == NULL )
            {
                ProgramError( L"OnThreadEnding pQuery NULL" );
            }
            else
            {
                pQuery->m_WorkInProgress = 0;
            }
        }
        g_nCountOfQueryThreads --;
        break;
    case ENUM_ADDFIND:
        {
            COneKwic * pKwic = ( COneKwic * ) CSolUserKwics.GetUserpVoid( ThreadParam );
            if( pKwic == NULL )
            {
                ProgramError( L"OnThreadEnding pKwic NULL" );
            }
            else
            {
                pKwic->m_WorkInProgress = 0;
            }
        }
        g_nCountOfKwicThreads --;
        break;
    case ENUM_ADDPAGE:
        {
            COneFetch * pFetch = ( COneFetch * ) CSolUserFetchs.GetUserpVoid( ThreadParam );
            if( pFetch == NULL )
            {
                ProgramError( L"OnThreadEnding pFetch NULL" );
            }
            else
            {
                pFetch->m_WorkInProgress = 0;
            }
        }
        g_nCountOfFetchThreads --;
        break;
    }

    if( ! g_bStopAllThreads )
    {
        // Try to start one thread that was left unstarted and pending.
        if( pListStartThreadPending != NULL )
        {
            START_THREAD_PENDING * one = pListStartThreadPending;
            pListStartThreadPending = one->NextPending; // take ONE out of list
            WPARAM Type_QFK = one->Type_Query1_Fetch2_Kwic3;
            LPARAM ThreadParam = one->CThreadParam;
            delete one;
            one = NULL;

            switch( Type_QFK )
            {
            case ENUM_ADDCACHE:     // never expected here?
                InitiateCache( );
                break;
            case ENUM_ADDDIRECTORY:
                InitiateFolder( ThreadParam );
                break;
            case ENUM_ADDPAGE:
                InitiateFetch( ThreadParam );
                break;
            case ENUM_ADDSEARCH:
                InitiateQuery( ThreadParam );
                break;
            case ENUM_ADDFIND:
                InitiateKwic( ThreadParam );
                break;
            case ENUM_ADDPHRASES:   // never expected here?
                InitiatePhrases( );
                break;
            }
        }
    }

    // If the status window is up, cause the change of counts to be shown.
    if( g_hWndStatusDialog != NULL )
    {
        PostMessage( g_hWndStatusDialog, WM_THREADCHANGE, 0, 0 );
    }
    else
    {
        // If Main is trying to exit, and awaiting completion:

        if( g_bStopAllThreads // this if set means main is exiting...
        && g_nCountOfCacheThreads == 0
        && g_nCountOfQueryThreads == 0
        && g_nCountOfFetchThreads == 0
        && g_nCountOfFolderThreads == 0
        && g_nCountOfKwicThreads == 0 )
        {
            // Same message, but this time caught in main DlgProc,
            // means that all threads are stopped, may destroy it.
            PostMessage( g_hWnd, WM_THREADCHANGE, 0, 0 );
        }
    }
}

void CIni::SaveThreadParamsPending( WPARAM MyWParam, LPARAM MyLParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"184" );
    #endif
    size_t ThreadParam = MyLParam;
    // WParam says which kind of thread could not be started:
    // WParam ENUM_ADDSEARCH: The ThreadParam is an index into CSol CSolUserQuerys;
    // WParam ENUM_ADDPAGE: The ThreadParam is an index into CSol CSolUserFetchs;
    // WParam ENUM_ADDDIRECTORY: The ThreadParam is an index into CSol CSolUserFolders;
    // WParam ENUM_ADDFIND : The ThreadParam is an index into CSol CSolUserKwics;

    // WParam ENUM_ADDCACHE: Do not accept this one.
    if( MyWParam == ENUM_ADDCACHE )
    {
        ProgramError( L"SaveThreadParamsPending: ENUM_ADDCACHE" );
        return;
    }

    // WParam ENUM_ADDPHRASES: Do not accept this one.
    if( MyWParam == ENUM_ADDPHRASES )
    {
        ProgramError( L"SaveThreadParamsPending: ENUM_ADDPHRASES" );
        return;
    }

    START_THREAD_PENDING * * father = & pListStartThreadPending;
    START_THREAD_PENDING * run = pListStartThreadPending;

    while( run != NULL )
    {
        father = & run->NextPending;
        run = run->NextPending;
    }

    run = * father = new START_THREAD_PENDING;
    run->Type_Query1_Fetch2_Kwic3 = MyWParam;
    run->CThreadParam = MyLParam;
    run->NextPending = NULL;
}


void CIni::InitiateSalts( )
{
    #if DO_DEBUG_CALLS
        Routine( L"501" );
    #endif

    // Salts differs from all the other threads.
    // There is no viewable log object at all.
    // It has no associated OnlySalts object.
    // It is not interruptable, goes to end.
    // It does not signal main when done.
    // It just does this long task silently:

    HANDLE hSaltsThread;

    // It will be too late to set this to 1 in the thread.
    // I saw main pgm test this, find 0 before it was set to 1,
    // and start to exit, then init thread got a memory error.
    // So set it now in main pgm, before initiating the thread.
    g_bInitThreadRunning = 1; // prevents program exit until init breaks

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) 0;
    hSaltsThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessSaltsThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hSaltsThread == NULL )
    {
        // CeateThread returns NULL on an error
        g_bInitThreadRunning = 0; // lest program cannot exit
        ProgramError( L"InitiateSalts" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hSaltsThread );
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = 0;
    hSaltsThread = ( HANDLE ) _beginthread( ProcessSaltsThread, 0, ( void * ) ThreadParam );
    if( hSaltsThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        g_bInitThreadRunning = 0; // lest program cannot exit
        ProgramError( L"InitiateSalts" );
    }
    else
    {
        // Win32 has no requirement to close handle like WinCE.
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessSaltsThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"502" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is not used.
    // size_t Index = ( size_t ) ThreadParam;

    // Salts differs from all the other threads.
    // There is no viewable log object at all.
    // It has no associated OnlySalts object.
    // It is not interruptable, goes to end.
    // It does not signal main when done.
    // It just does this long task silently:

    SaltCommonAndPerLanguageWordLists( );
    g_bInitThreadRunning = 0;

    return 0;
}
#else
void __cdecl CIni::ProcessSaltsThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"503" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is not used.
    // size_t Index = ( size_t ) ThreadParam;

    // Salts differs from all the other threads.
    // There is no viewable log object at all.
    // It has no associated OnlySalts object.
    // It is not interruptable, goes to end.
    // It does not signal main when done.
    // It just does this long task silently:

    SaltCommonAndPerLanguageWordLists( );
    g_bInitThreadRunning = 0;
}
#endif




void CIni::InitiateQuery( size_t Index )
{
    #if DO_DEBUG_CALLS
        Routine( L"185" );
    #endif
    g_bStopAllThreads = 0;

    if( ! Www.FirstTimeSetupInternet( ) )
        return;

    // Get the CSolUserQuerys instance controlling this unique CSol expression.
    COneQuery * pQuery = ( COneQuery * ) CSolUserQuerys.GetUserpVoid( Index );
    if( pQuery == NULL )
    {
        ProgramError( L"InitiateQuery pQuery NULL" );
        return;
    }

    // If a thread for this is currently running, I should do nothing.
    if( pQuery->m_WorkInProgress != 0 )
    {
        return;
    }

    g_bEverStartedAnyQuery = 1;

    // It was scary to think of unlinking a Wsb from its Bud,
    // and replacing it with a new one. Reset is easy answer.
    pQuery->pWsbResultText->Reset( );
    pQuery->pIdxResultIndex->Reset( );

    // Get the original query parameter.
    // GetFullKey returns a malloc, I must free.
    wchar_t * pMalQuery = CSolUserQuerys.GetFullKey( Index );
    if( pMalQuery == NULL )
    {
        ProgramError( L"InitiateQuery pArg NULL" );
        return;
    }

    pQuery->m_WorkInProgress = 1;

    // This looks dangerous. Just add to existing log page.
    //
    // if( pQuery->pWsbResultText->StrLen != 0 )
    // {
    //     // Discard a previous result text;
    //     delete pQuery->pWsbResultText;
    //     pQuery->pWsbResultText = NULL;
    //
    //     // Start anew.
    //     pQuery->pWsbResultText = new CWsb( );
    // }

    // I did this in w*.cpp
    pQuery->pWsbResultText->Add( L"Add, Internet Search: " );
    pQuery->pWsbResultText->Add( pMalQuery );
    pQuery->pWsbResultText->Add( L"\r\n\r\n" );
    MyFree( 244, zx, pMalQuery );
    pMalQuery = NULL;

    // Make sure m_NonTextPath is NULL iff m_NonTextLinks is false.
    if( pQuery->m_NonTextPath != NULL )
    {
        wchar_t * temp = pQuery->m_NonTextPath;
        pQuery->m_NonTextPath = NULL;
        MyFree( 409, UNPREDICTABLE, temp );
    }

    // This is a clone into QUERY code:
    if( pQuery->m_NonTextLinks )
    {
        // Prompt for a directory to receive non-texts

        // It is easier to hook up in WordsEx.cpp than in CIni.cpp
        // Dialog may leave a path in WsbTempPathToSaveNonText
        // in which case all folders were created, ready to use.
        AskForNonTextDirectory( );

        if( WsbTempPathToSaveNonText.StrLen == 0 )
        {
            pQuery->m_NonTextLinks = 0; // reset bool if not given a path.
            #if DO_DEBUG_FETCH
                ; Spew( L"WsbTempPathToSaveNonText.StrLen == 0" );
            #endif
            pQuery->pWsbResultText->Add( L"Any non-text resources will not be downloaded.\r\n\r\n" );
        }
        else
        {
            size_t nMalPath = NULL;
            pQuery->m_NonTextPath = WsbTempPathToSaveNonText.GetBuffer( & nMalPath );

            pQuery->pWsbResultText->Add( L"Any non-text resources will be saved into " );
            pQuery->pWsbResultText->Add( pQuery->m_NonTextPath );
            pQuery->pWsbResultText->Add( L"\r\n\r\n" );
        }
    }

    // obs: Top.UpdateViewIfOnScreen( pQuery );

    HANDLE hQueryThread;

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) Index;
    hQueryThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessQueryThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hQueryThread == NULL )
    {
        // CeateThread returns NULL on an error
        ProgramError( L"InitiateQuery" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hQueryThread );
        g_nCountOfQueryThreads ++;
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = Index;
    hQueryThread = ( HANDLE ) _beginthread( ProcessQueryThread, 0, ( void * ) ThreadParam );
    if( hQueryThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        // Also errno is set to EAGAIN if there are too many threads
        if( errno == EAGAIN )
        {
            SaveThreadParamsPending( ( WPARAM ) ENUM_ADDSEARCH, ( LPARAM ) ThreadParam );
        }
        else
        {
            // Preserve only the first error.
            if( g_szInternalError == NULL )
            {
                g_szInternalError = L"Query _beginthread";
                g_dwLastError = 40000000 + errno; // Is 40 million area unused?
            }
        }
    }
    else
    {
        g_nCountOfQueryThreads ++;
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessQueryThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"186" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is an index into CSol CSolUserQuerys;
    size_t QueryIndex = ( size_t ) ThreadParam;

    // Get the OneQuery Fruit from CSolUserQuerys to control and see this thread.
    COneQuery * pQuery = ( COneQuery * ) CSolUserQuerys.GetUserpVoid( QueryIndex );
    if( pQuery == NULL )
    {
        ProgramError( L"ProcessQueryThread pQuery NULL" );
        return 0;
    }
    pQuery->m_StopThisThread = 0; // in case stopped, now restarting
    Www.RunOneQueryThread( QueryIndex );

    // Thread is ending, so inform application.
    // WParam ENUM_ADDSEARCH: The ThreadParam is an index into CSol CSolUserQuerys;
    WPARAM MyWParam = ENUM_ADDSEARCH;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );

    return 0;
}
#else
void __cdecl CIni::ProcessQueryThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"187" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is an index into CSol CSolUserQuerys;
    size_t QueryIndex = ( size_t ) ThreadParam;

    // Get the OneQuery Fruit from CSolUserQuerys to control and see this thread.
    COneQuery * pQuery = ( COneQuery * ) CSolUserQuerys.GetUserpVoid( QueryIndex );
    if( pQuery == NULL )
    {
        ProgramError( L"ProcessQueryThread pQuery NULL" );
        return;
    }
    pQuery->m_StopThisThread = 0; // in case stopped, now restarting
    Www.RunOneQueryThread( QueryIndex );

    // Thread is ending, so inform application.
    // WParam ENUM_ADDSEARCH: The ThreadParam is an index into CSol CSolUserQuerys;
    WPARAM MyWParam = ENUM_ADDSEARCH;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );
}
#endif


void CIni::InitiateFetch( size_t Index )
{
    #if DO_DEBUG_CALLS
        Routine( L"188" );
    #endif
    g_bStopAllThreads = 0;

    if( ! Www.FirstTimeSetupInternet( ) )
        return;

    // Get the UserFetch instance controlling this unique CSol expression.
    COneFetch * pFetch = ( COneFetch * ) CSolUserFetchs.GetUserpVoid( Index );
    if( pFetch == NULL )
    {
        ProgramError( L"InitiateFetch pFetch NULL" );
        return;
    }

    // If a thread for this is currently running, I should do nothing.
    if( pFetch->m_WorkInProgress != 0 )
    {
        return;
    }

    // It was scary to think of unlinking a Wsb from its Bud,
    // and replacing it with a new one. Reset is easy answer.

    // Let's not reset the pFetch->pWsbResultText here.
    // Then, if they re-fetch same URL, they do not lose old log.
    // pFetch->pWsbResultText->Reset( );
    // pFetch->pIdxResultIndex->Reset( );

    // Get the original URL to fetch.
    // GetFullKey returns a malloc, I must free.
    wchar_t * auto_pMallocUrl = CSolUserFetchs.GetFullKey( Index );
    if( auto_pMallocUrl == NULL )
    {
        ProgramError( L"InitiateFetch pUrl NULL" );
        return;
    }

    pFetch->m_WorkInProgress = 1;

    // I did this in w*.cpp
    pFetch->pWsbResultText->Add( L"Add, One Internet Page: " );
    pFetch->pWsbResultText->Add( auto_pMallocUrl );
    pFetch->pWsbResultText->Add( L"\r\n" );
    #if DO_DEBUG_FETCH
        ; Spew( L"Add, One Internet Page:" );
        ; Spew( auto_pMallocUrl );
    #endif

    MyFree( 410, zx, auto_pMallocUrl );
    auto_pMallocUrl = NULL;

    if( pFetch->m_FetchAllLinks )
    {
        pFetch->pWsbResultText->Add( L"and every link found in it.\r\n" );
        #if DO_DEBUG_FETCH
            ; Spew( L"and every link found in it." );
        #endif
    }


    // Make sure m_NonTextPath is NULL iff m_NonTextLinks is false.
    if( pFetch->m_NonTextPath != NULL )
    {
        wchar_t * temp = pFetch->m_NonTextPath;
        pFetch->m_NonTextPath = NULL;
        MyFree( 409, UNPREDICTABLE, temp );
    }

    // This is the original case in FETCH code:
    if( pFetch->m_NonTextLinks )
    {
        // Prompt for a directory to receive non-texts

        // It is easier to hook up in WordsEx.cpp than in CIni.cpp
        // Dialog may leave a path in WsbTempPathToSaveNonText
        // in which case all folders were created, ready to use.
        AskForNonTextDirectory( );

        if( WsbTempPathToSaveNonText.StrLen == 0 )
        {
            pFetch->m_NonTextLinks = 0; // reset bool if not given a path.
            #if DO_DEBUG_FETCH
                ; Spew( L"WsbTempPathToSaveNonText.StrLen == 0" );
            #endif
            pFetch->pWsbResultText->Add( L"Any non-text resources will not be downloaded.\r\n\r\n" );
        }
        else
        {
            size_t nMalPath = NULL;
            pFetch->m_NonTextPath = WsbTempPathToSaveNonText.GetBuffer( & nMalPath );

            pFetch->pWsbResultText->Add( L"Any non-text resources will be saved into " );
            pFetch->pWsbResultText->Add( pFetch->m_NonTextPath );
            pFetch->pWsbResultText->Add( L"\r\n\r\n" );
        }
    }

    pFetch->pWsbResultText->Add( L"\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFetch );

    HANDLE hFetchThread;

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) Index;
    hFetchThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessFetchThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hFetchThread == NULL )
    {
        // CeateThread returns NULL on an error
        ProgramError( L"InitiateFetch" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hFetchThread );
        g_nCountOfFetchThreads ++;
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = Index;
    hFetchThread = ( HANDLE ) _beginthread( ProcessFetchThread, 0, ( void * ) ThreadParam );
    if( hFetchThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        // Also errno is set to EAGAIN if there are too many threads
        if( errno == EAGAIN )
        {
            SaveThreadParamsPending( ( WPARAM ) ENUM_ADDPAGE, ( LPARAM ) ThreadParam );
        }
        else
        {
            // Preserve only the first error.
            if( g_szInternalError == NULL )
            {
                g_szInternalError = L"Fetch _beginthread";
                g_dwLastError = 40000000 + errno; // Is 40 million area unused?
            }
        }
    }
    else
    {
        g_nCountOfFetchThreads ++;
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessFetchThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"189" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is an index into CSol CSolUserFetchs;
    size_t FetchIndex = ( size_t ) ThreadParam;

    // Get the OneFetch Fruit from CSolUserFetchs to control and see this thread.
    COneFetch * pFetch = ( COneFetch * ) CSolUserFetchs.GetUserpVoid( ( size_t ) ThreadParam );
    if( pFetch == NULL )
    {
        ProgramError( L"ProcessFetchThread pFetch NULL" );
        return 0;
    }
    pFetch->m_StopThisThread = 0; // in case stopped, now restarting
    Www.RunOneFetchThread( FetchIndex );

    // Thread is ending, so inform application.
    // WParam ENUM_ADDPAGE: The ThreadParam is an index into CSol CSolUserFetchs;
    WPARAM MyWParam = ENUM_ADDPAGE;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );

    return 0;
}
#else
void __cdecl CIni::ProcessFetchThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"190" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is an index into CSol CSolUserFetchs;
    size_t FetchIndex = ( size_t ) ThreadParam;

    // Get the OneFetch Fruit from CSolUserFetchs to control and see this thread.
    COneFetch * pFetch = ( COneFetch * ) CSolUserFetchs.GetUserpVoid( ( size_t ) ThreadParam );
    if( pFetch == NULL )
    {
        ProgramError( L"ProcessFetchThread pFetch NULL" );
        return;
    }
    pFetch->m_StopThisThread = 0; // in case stopped, now restarting
    Www.RunOneFetchThread( FetchIndex );

    // Thread is ending, so inform application.
    // WParam ENUM_ADDPAGE: The ThreadParam is an index into CSol CSolUserFetchs;
    WPARAM MyWParam = ENUM_ADDPAGE;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );
}
#endif


void CIni::InitiateKwic( size_t Index )
{
    #if DO_DEBUG_CALLS
        Routine( L"191" );
    #endif
    g_bStopAllThreads = 0;

    // Get the CSolUserKwics instance controlling this unique CSol expression.
    COneKwic * pKwic = ( COneKwic * ) CSolUserKwics.GetUserpVoid( Index );
    if( pKwic == NULL )
    {
        ProgramError( L"InitiateKwic pKwic NULL" );
        return;
    }

    // If a thread for this is currently running, I should do nothing.
    if( pKwic->m_WorkInProgress != 0 )
    {
        return;
    }

    // It was scary to think of unlinking a Wsb from its Bud,
    // and replacing it with a new one. Reset is easy answer.
    pKwic->pWsbResultText->Reset( );
    pKwic->pIdxResultIndex->Reset( );

    // Get the original Find parameter.
    // GetFullKey returns a malloc, I must free.
    wchar_t * pMalKwic = CSolUserKwics.GetFullKey( Index );
    if( pMalKwic == NULL )
    {
        ProgramError( L"InitiateKwic pArg NULL" );
        return;
    }

    pKwic->m_WorkInProgress = 1;

    // This looks dangerous. Just add to existing log page.
    //
    // if( pKwic->pWsbResultText->StrLen != 0 )
    // {
    //     // Discard a previous result text;
    //     delete pKwic->pWsbResultText;
    //     pKwic->pWsbResultText = NULL;
    //
    //     // Start anew.
    //     pKwic->pWsbResultText = new CWsb( );
    // }

    // I did this in w*.cpp
    pKwic->pWsbResultText->Add( L"Find Words: " );
    // Spew( pMalKwic );
    pKwic->pWsbResultText->Add( pMalKwic );
    pKwic->pWsbResultText->Add( L"\r\n\r\n" );
    MyFree( 575, zx, pMalKwic );
    pMalKwic = NULL;

    // obs: Top.UpdateViewIfOnScreen( pKwic );

    HANDLE hKwicThread;

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) Index;
    hKwicThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessKwicThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hKwicThread == NULL )
    {
        // CeateThread returns NULL on an error
        ProgramError( L"InitiateKwic" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hKwicThread );
        g_nCountOfKwicThreads ++;
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = Index;
    hKwicThread = ( HANDLE ) _beginthread( ProcessKwicThread, 0, ( void * ) ThreadParam );
    if( hKwicThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        // Also errno is set to EAGAIN if there are too many threads
        if( errno == EAGAIN )
        {
            SaveThreadParamsPending( ( WPARAM ) ENUM_ADDFIND, ( LPARAM ) ThreadParam );
        }
        else
        {
            // Preserve only the first error.
            if( g_szInternalError == NULL )
            {
                g_szInternalError = L"Kwic _beginthread";
                g_dwLastError = 40000000 + errno; // Is 40 million area unused?
            }
        }
    }
    else
    {
        g_nCountOfKwicThreads ++;
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessKwicThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"192" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is an index into CSol CSolUserKwics;
    size_t Index = ( size_t ) ThreadParam;

    // Get the OneKwic Fruit from CSolUserKwics to control and see this thread.
    COneKwic * pKwic = ( COneKwic * ) CSolUserKwics.GetUserpVoid( ( size_t ) ThreadParam );
    pKwic->m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:
    pKwic->pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pKwic );

    {
        CFwd * Find = new CFwd( Index );
        Find->RunFindThread( );
        // Nobody else holds Find pointer, so no need to copy before delete.
        delete Find;
        Find = NULL;
    }

    pKwic->pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pKwic );

    // Thread is ending, so inform application.
    // WParam ENUM_ADDFIND: The ThreadParam is an index into CSol CSolUserKwics;
    WPARAM MyWParam = ENUM_ADDFIND;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );

    return 0;
}
#else
void __cdecl CIni::ProcessKwicThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"193" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is an index into CSol CSolUserKwics;
    size_t Index = ( size_t ) ThreadParam;

    // Get the OneKwic Fruit from CSolUserKwics to control and see this thread.
    COneKwic * pKwic = ( COneKwic * ) CSolUserKwics.GetUserpVoid( ( size_t ) ThreadParam );
    pKwic->m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:
    pKwic->pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pKwic );

    {
        CFwd * Find = new CFwd( Index );
        Find->RunFindThread( );
        // Nobody else holds Find pointer, so no need to copy before delete.
        delete Find;
        Find = NULL;
    }

    pKwic->pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pKwic );

    // Thread is ending, so inform application.
    // WParam ENUM_ADDFIND: The ThreadParam is an index into CSol CSolUserKwics;
    WPARAM MyWParam = ENUM_ADDFIND;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );
}
#endif


void CIni::InitiateCache( )
{
    #if DO_DEBUG_CALLS
        Routine( L"194" );
    #endif
    g_bStopAllThreads = 0;

    // This is okay, in main thread, before starting worker thread:
    Top.Add( & OnlyCache );

    // If a thread for this is currently running, I should do nothing.
    if( OnlyCache.m_WorkInProgress != 0 )
    {
        return;
    }

    OnlyCache.m_WorkInProgress = 1;

    // It was scary to think of unlinking a Wsb from its Bud,
    // and replacing it with a new one. Reset is easy answer.

    // Now that I see aborting add cache, and doing it again
    // will not present the data for URLs aready processed,
    // do not erase the previous Add Cache progress text...
    // OnlyCache.pWsbResultText->Reset( );
    // OnlyCache.pIdxResultIndex->Reset( );

    OnlyCache.pWsbResultText->Add( L"Add Cache:\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyCache );

    HANDLE hCacheThread;

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) 0;
    hCacheThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessCacheThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hCacheThread == NULL )
    {
        // CeateThread returns NULL on an error
        ProgramError( L"InitiateCache" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hCacheThread );
        g_nCountOfCacheThreads ++;
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = 0;
    hCacheThread = ( HANDLE ) _beginthread( ProcessCacheThread, 0, ( void * ) ThreadParam );
    if( hCacheThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        // Also errno is set to EAGAIN if there are too many threads
        if( errno == EAGAIN )
        {
            // Do not postpone a Cache thread:
            Say( L"Cannot start Cache thread. Try again later." );
            // SaveThreadParamsPending( ( WPARAM ) ENUM_ADDCACHE, ( LPARAM ) ThreadParam );
        }
        else
        {
            // Preserve only the first error.
            if( g_szInternalError == NULL )
            {
                g_szInternalError = L"Cache _beginthread";
                g_dwLastError = 40000000 + errno; // Is 40 million area unused?
            }
        }
    }
    else
    {
        g_nCountOfCacheThreads ++;
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessCacheThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"195" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is not used.
    // size_t Index = ( size_t ) ThreadParam;

    // Cache differs from all the other threads.
    // There is no diversity of OneCache fruits.

    OnlyCache.m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:
    OnlyCache.pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyCache );

    // In case of early returns in loops below,
    // I will new and delete this member here:

    {
        pSolCacheToDo = new CSol( CSOL_SCALAR ); // to sort files by size...

        Www.AddCachePhaseOne( );
        Www.AddCachePhaseTwo( );

        delete pSolCacheToDo;
        pSolCacheToDo = NULL;
    }


    OnlyCache.pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyCache );

    // Thread is ending, so inform application.
    WPARAM MyWParam = ENUM_ADDCACHE;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;

    // Spew( L"Sending Threadend msg." );
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );

    return 0;
}
#else
void __cdecl CIni::ProcessCacheThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"196" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is not used.
    // size_t Index = ( size_t ) ThreadParam;

    // Cache differs from all the other threads.
    // There is no diversity of OneCache fruits.

    OnlyCache.m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:
    OnlyCache.pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyCache );

    // In case of early returns in loops below,
    // I will new and delete this member here:

    {
        pSolCacheToDo = new CSol( CSOL_SCALAR ); // to sort files by size...

        Www.AddCachePhaseOne( );
        Www.AddCachePhaseTwo( );

        delete pSolCacheToDo;
        pSolCacheToDo = NULL;
    }

    OnlyCache.pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyCache );

    // Thread is ending, so inform application.
    WPARAM MyWParam = ENUM_ADDCACHE;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );
}
#endif


void CIni::InitiateFolder( size_t Index )
{
    #if DO_DEBUG_CALLS
        Routine( L"197" );
    #endif
    g_bStopAllThreads = 0;

    // Get the CSolUserFolders instance controlling this unique CSol expression.
    COneFolder * pFolder = ( COneFolder * ) CSolUserFolders.GetUserpVoid( Index );
    if( pFolder == NULL )
    {
        ProgramError( L"InitiateFolder pFolder NULL" );
        return;
    }

    // If a thread for this is currently running, I should do nothing.
    if( pFolder->m_WorkInProgress != 0 )
    {
        return;
    }

    // It was scary to think of unlinking a Wsb from its Bud,
    // and replacing it with a new one. Reset is easy answer.
    pFolder->pWsbResultText->Reset( );
    pFolder->pIdxResultIndex->Reset( );

    // Get the original directory path.
    // GetFullKey returns a malloc, I must free.
    wchar_t * pMalFolderPath = CSolUserFolders.GetFullKey( Index );
    if( pMalFolderPath == NULL )
    {
        ProgramError( L"InitiateFolder pPath NULL" );
        return;
    }

    pFolder->m_WorkInProgress = 1;

    // This looks dangerous. Just add to existing log page.
    //
    // if( pFolder->pWsbResultText->StrLen != 0 )
    // {
    //     // Discard a previous result text;
    //     delete pFolder->pWsbResultText;
    //     pFolder->pWsbResultText = NULL;
    //
    //     // Start anew.
    //     pFolder->pWsbResultText = new CWsb( );
    // }

    // I did this in w*.cpp
    if( pFolder->m_bAddRecursively )
        pFolder->pWsbResultText->Add( L"Add Directory, recursive: " );
    else
        pFolder->pWsbResultText->Add( L"Add Directory, flat: " );
    pFolder->pWsbResultText->Add( pMalFolderPath );
    pFolder->pWsbResultText->Add( L"\r\n" );
    MyFree( 879, zx, pMalFolderPath );
    pMalFolderPath = NULL;

    // obs: Top.UpdateViewIfOnScreen( pFolder );

    HANDLE hFolderThread;

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) Index;
    hFolderThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessFolderThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hFolderThread == NULL )
    {
        // CeateThread returns NULL on an error
        ProgramError( L"InitiateFolder" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hFolderThread );
        g_nCountOfFolderThreads ++;
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = Index;
    hFolderThread = ( HANDLE ) _beginthread( ProcessFolderThread, 0, ( void * ) ThreadParam );
    if( hFolderThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        // Also errno is set to EAGAIN if there are too many threads
        if( errno == EAGAIN )
        {
            SaveThreadParamsPending( ( WPARAM ) ENUM_ADDDIRECTORY, ( LPARAM ) ThreadParam );
        }
        else
        {
            // Preserve only the first error.
            if( g_szInternalError == NULL )
            {
                g_szInternalError = L"Folder _beginthread";
                g_dwLastError = 40000000 + errno; // Is 40 million area unused?
            }
        }
    }
    else
    {
        g_nCountOfFolderThreads ++;
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessFolderThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"198" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is an index into CSol CSolUserFolders;
    size_t FolderIndex = ( size_t ) ThreadParam;

    // Get the OneFolder Fruit from CSolUserFolders to control and see this thread.
    COneFolder * pFolder = ( COneFolder * ) CSolUserFolders.GetUserpVoid( FolderIndex );
    if( pFolder == NULL )
    {
        ProgramError( L"ProcessFolderThread pFolder NULL" );
        return 0;
    }
    pFolder->m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:
    pFolder->pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFolder );

    if( pFolder->pSolFolderToDo != NULL )
    {
        CSol * Temp = pFolder->pSolFolderToDo; // copy before delete
        pFolder->pSolFolderToDo = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    if( Fio.AddFolderPhaseOne( FolderIndex ) )
        Fio.AddFolderPhaseTwo( FolderIndex );

    // I do not need to keep this list of filenames after the work.
    if( pFolder->pSolFolderToDo != NULL )
    {
        CSol * Temp = pFolder->pSolFolderToDo; // copy before delete
        pFolder->pSolFolderToDo = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }

    pFolder->pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFolder );

    // Thread is ending, so inform application.
    WPARAM MyWParam = ENUM_ADDDIRECTORY;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );

    return 0;
}
#else
void __cdecl CIni::ProcessFolderThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"199" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is an index into CSol CSolUserFolders;
    size_t FolderIndex = ( size_t ) ThreadParam;

    // Get the OneFolder Fruit from CSolUserFolders to control and see this thread.
    COneFolder * pFolder = ( COneFolder * ) CSolUserFolders.GetUserpVoid( FolderIndex );
    if( pFolder == NULL )
    {
        ProgramError( L"ProcessFolderThread pFolder NULL" );
        return;
    }

    pFolder->m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:

    pFolder->pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFolder );

    if( Fio.AddFolderPhaseOne( FolderIndex ) )
        Fio.AddFolderPhaseTwo( FolderIndex );

    pFolder->pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFolder );

    // Thread is ending, so inform application.
    WPARAM MyWParam = ENUM_ADDDIRECTORY;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );
}
#endif


void CIni::InitiatePhrases( )
{
    #if DO_DEBUG_CALLS
        Routine( L"200" );
    #endif
    g_bStopAllThreads = 0;

    // This is okay, in main thread, before starting worker thread:
    Top.Add( & OnlyPhrases );

    // If a thread for this is currently running, I should do nothing.
    if( OnlyPhrases.m_WorkInProgress != 0 )
    {
        #if DO_DEBUG_PHRASE
            ; Spew( L"Initiate AddPhrases: No, Found WorkInProgress." );
        #endif
        return;
    }

    OnlyPhrases.m_WorkInProgress = 1;

    #if DO_DEBUG_PHRASE
        ; Spew( L"Initiate AddPhrases: Set WorkInProgress." );
    #endif

    // It was scary to think of unlinking a Wsb from its Bud,
    // and replacing it with a new one. Reset is easy answer.
    OnlyPhrases.pWsbResultText->Reset( );
    OnlyPhrases.pIdxResultIndex->Reset( );

    OnlyPhrases.pWsbResultText->Add( L"Add Phrases:\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyPhrases );

    HANDLE hPhrasesThread;

#ifdef _WIN32_WCE
    // This is the WinCE version using CreateThread.
    void * ThreadParam = ( void * ) 0;
    hPhrasesThread = CreateThread(
        NULL,               // LPSECURITY_ATTRIBUTES - Ignored. Must be NULL.
        0,                  // DWORD dwStackSize - Ignored. linker setting /STACK.
        ProcessPhrasesThread, // LPTHREAD_START_ROUTINE lpStartAddress
        ThreadParam,        // LPVOID lpParameter
        0,                  // DWORD dwCreationFlags - 0 = thread runs immediately
        NULL                // LPDWORD lpThreadId - NULL = not wanted
    );
    if( hPhrasesThread == NULL )
    {
        // CeateThread returns NULL on an error
        ProgramError( L"InitiatePhrases" );
    }
    else
    {
        // Must CloseHandle after each CreateThread.
        CloseHandle( hPhrasesThread );
        g_nCountOfPhrasesThreads ++;
    }
#else
    // This is the Win32 version using _beginthread.
    size_t ThreadParam = 0;
    hPhrasesThread = ( HANDLE ) _beginthread( ProcessPhrasesThread, 0, ( void * ) ThreadParam );
    if( hPhrasesThread == ( HANDLE ) ( -1 ) )
    {
        // _beginthread returns -1 on an error.
        // Also errno is set to EAGAIN if there are too many threads
        if( errno == EAGAIN )
        {
            // Do not postpone a Phrases thread:
            Say( L"Cannot start Phrases thread. Try again later." );
            // SaveThreadParamsPending( ( WPARAM ) ENUM_ADDPHRASES, ( LPARAM ) ThreadParam );
        }
        else
        {
            // Preserve only the first error.
            if( g_szInternalError == NULL )
            {
                g_szInternalError = L"Phrases _beginthread";
                g_dwLastError = 40000000 + errno; // Is 40 million area unused?
            }
        }
    }
    else
    {
        g_nCountOfPhrasesThreads ++;
    }
#endif
}

#ifdef _WIN32_WCE
DWORD __cdecl CIni::ProcessPhrasesThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"201" );
    #endif
    // This is the WinCE version using CreateThread.
    // The ThreadParam is not used.
    // size_t Index = ( size_t ) ThreadParam;

    // Phrases differs from all the other threads.
    // There is no diversity of OnePhrases fruits.

    OnlyPhrases.m_StopThisThread = 0; // in case stopped, now restarting

    // Just keep adding to this progress log:
    OnlyPhrases.pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyPhrases );

    {
        CFwd * Find = new CFwd( 0 ); // No KWIC information here
        Find->RunPhrasesThread( );
        // Nobody else holds Find pointer, so no need to copy before delete.
        delete Find;
        Find = NULL;
    }

    OnlyPhrases.pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyPhrases );

    // Thread is ending, so inform application.
    WPARAM MyWParam = ENUM_ADDPHRASES;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;

    // Spew( L"Sending Threadend msg." );
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );

    return 0;
}
#else
void __cdecl CIni::ProcessPhrasesThread( void * ThreadParam )
{
    #if DO_DEBUG_CALLS
        Routine( L"202" );
    #endif
    // This is the Win32 version using _beginthread.
    // The ThreadParam is not used.
    // size_t Index = ( size_t ) ThreadParam;

    // Phrases differs from all the other threads.
    // There is no diversity of OnePhrases fruits.

    OnlyPhrases.m_StopThisThread = 0; // in case stopped, now restarting

    #if DO_DEBUG_PHRASE
        ; Spew( L"ProcessPhrasesThread: Starting." );
    #endif

    // Just keep adding to this progress log:
    OnlyPhrases.pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyPhrases );

    {
        CFwd * Find = new CFwd( 0 ); // No KWIC information here
        Find->RunPhrasesThread( );
        // Nobody else holds Find pointer, so no need to copy before delete.
        delete Find;
        Find = NULL;
    }

    OnlyPhrases.pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( & OnlyPhrases );

    #if DO_DEBUG_PHRASE
        ; Spew( L"ProcessPhrasesThread: Ending." );
    #endif

    // Thread is ending, so inform application.
    WPARAM MyWParam = ENUM_ADDPHRASES;
    LPARAM MyLParam = ( LPARAM ) ThreadParam;
    PostMessage( g_hWnd, WM_THREADENDING, MyWParam, MyLParam );
}
#endif
