// This is file: CWww.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals


// Even my T-Mobile Pocket PC Phone has a Wininet.dll.

HINSTANCE               g_hWinInet;     // Loaded WININET.DLL

FIND_FIRST_URL_ETC pFindFirstUrlCacheEntry; // Pointer to DLL routine
FIND_NEXT_URL_ETC pFindNextUrlCacheEntry; // Pointer to DLL routine

wchar_t * szSimpleBlurb = {
    L"This thread will send your search request to many search engines"
    L" and will receive back each search engine's query result page."
    L" It will find the desirable hit links on each query result page,"
    L" and will download each of those hit addresses from the Internet."
    L" It will write a summary of each page in the progress text below."
    L"\r\n\r\n"
    L"At any time, you may click on a summary below to begin reading the"
    L" text of that web page. Then use the Delete command to return here."
    L"\r\n\r\n"
};

CWww::CWww( )
{
    #if DO_DEBUG_CALLS
        Routine( L"289" );
    #endif
    m_hInternet = NULL;
    m_FirstTimeCalled = 0;

    m_TotalCacheFiles = 0;  // in CWww
    m_GoodCacheFiles = 0;   // in CWww
    m_GoodCacheBytes = 0;   // in CWww

}

CWww::~CWww( )
{
    #if DO_DEBUG_CALLS
        Routine( L"290" );
    #endif
    if( m_hInternet != NULL )
        if( ! InternetCloseHandle( m_hInternet ) )
            ProgramError( L"InternetCloseHandle m_hInternet" );
}

int CWww::LoadWinInetDll( )
{
    #if DO_DEBUG_CALLS
        Routine( L"291" );
    #endif
    g_hWinInet = LoadLibrary( L"wininet.dll" );
    if( ! g_hWinInet )
    {
        ProgramError( L"LoadLibrary wininet.dll" );
        return 0; // failure
    }

#ifdef _WIN32_WCE
    pFindFirstUrlCacheEntry = ( FIND_FIRST_URL_ETC ) GetProcAddress(
        ( HMODULE ) g_hWinInet, L"FindFirstUrlCacheEntryW" );
    pFindNextUrlCacheEntry = ( FIND_NEXT_URL_ETC ) GetProcAddress(
        ( HMODULE ) g_hWinInet, L"FindNextUrlCacheEntryW" );
#else
    // It is curious that on Win32, the W form is not supported.
    pFindFirstUrlCacheEntry = ( FIND_FIRST_URL_ETC ) GetProcAddress(
        ( HMODULE ) g_hWinInet, "FindFirstUrlCacheEntryW" );
    pFindNextUrlCacheEntry = ( FIND_NEXT_URL_ETC ) GetProcAddress(
        ( HMODULE ) g_hWinInet, "FindNextUrlCacheEntryW" );
#endif

    if( pFindFirstUrlCacheEntry == NULL )
    {
        ProgramError( L"GetProcAddress FindFirstUrlCacheEntry" );
    }

    if( pFindNextUrlCacheEntry == NULL )
    {
        ProgramError( L"GetProcAddress FindNextUrlCacheEntry" );
    }

    if( pFindFirstUrlCacheEntry == NULL
    || pFindNextUrlCacheEntry == NULL )
    {
        FreeLibrary( g_hWinInet );
        g_hWinInet = NULL;
        pFindFirstUrlCacheEntry = NULL;
        pFindNextUrlCacheEntry = NULL;
        return 0; // failure
    }

    return 1; // success
}

void CWww::UserUrlError( wchar_t * szSystemCommand )
{
    #if DO_DEBUG_CALLS
        Routine( L"292" );
    #endif
    // Cloned right out of ProgramErrorMsgBox routine, no spews.

    size_t dwLastError = GetLastError( );

    HMODULE hModule = NULL; // default to system source
    DWORD dwBufferLength = 0;
    DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM;

    // In the case of WinInet numbers, point to that source.
    if( dwLastError >= 12001
    && dwLastError <= 12156 ) // probably higher - find a symbol
    {
        hModule = g_hWinInet;
        if( hModule != NULL )
            dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
    }

    wchar_t * MessageBuffer = NULL; // receives the LocalAlloc.

    dwBufferLength = FormatMessage(
        dwFormatFlags,
        hModule,
        dwLastError,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
        ( wchar_t * ) & MessageBuffer, // type was mis-cast on purpose
        0,
        NULL );

    // Handle the case that FormatMessage failed.	
    if( dwBufferLength == 0 )
    {
        wchar_t wk[80];
        wsprintf( wk, L"Error No. %d", dwLastError );
        MessageBox( g_hWnd, wk, szSystemCommand, MB_OK );
        return;
    }

    // Normal case after FormatMessage succeeded.
    MessageBox( g_hWnd, MessageBuffer, szSystemCommand, MB_OK );

    if( MessageBuffer != NULL )
        LocalFree( MessageBuffer );
    MessageBuffer = NULL;

    return;
}

int CWww::FixUpUserUrl( wchar_t * * NewMallocPtr, wchar_t * MallocPtr )
{
    #if DO_DEBUG_CALLS
        Routine( L"293" );
    #endif
    // returns 1=success, 0=program failure, -1=URL fail but stay in dialog.

    // This must be an absoute URL, including a method.
    // Add-WebPage prefixed an http:// method if none.

    * NewMallocPtr = NULL; // just in case

    if( NewMallocPtr == NULL )
    {
        ProgramError( L"Pointer NULL - FixUpUserUrl" );
        return 0; // failure
    }

    if( MallocPtr == NULL )
    {
        ProgramError( L"Input NULL - FixUpUserUrl" );
        return 0; // failure
    }

    if( g_hWinInet == NULL )
    {
        ProgramError( L"WinInet NULL - FixUpUserUrl" );
        return 0; // failure
    }

    size_t nInputLen = wcslen( MallocPtr );

    size_t nMallocLen = ( nInputLen + 1 ) * 3; // worst case % expansion?
    wchar_t * LocalPtr = ( wchar_t * ) MyMalloc( 100, nMallocLen * sizeof( wchar_t ) );

    DWORD dwLength = nMallocLen;
    if( ! InternetCanonicalizeUrl( MallocPtr, LocalPtr, & dwLength,
        ICU_BROWSER_MODE | ICU_DECODE ) )
    {
        MyFree( 100, nMallocLen * sizeof( wchar_t ), LocalPtr );
        LocalPtr = NULL;
        UserUrlError( L"InternetCanonicalizeUrl" );
        return -1; // failure, but do not dismiss dialog
    }

    // Since this helped while parsing an Anchor, do it in FixUpUserUrl too:
    {
        // Solve this Problem, not the same issue as ICU_BROWSER_MODE:
        // So run a loop that will concatenate out any spaces after ?.
        // Top URL: http://www.waste.org/mail/?list=pynchon-l&keywords=dickinson
        // Anchor: <a href="?list=pynchon-l&month=9510 &msg=3079 &keywords=dickinson">
        // Joined: http://www.waste.org/mail/?list=pynchon-l&month=9705 &msg=13950 &keywords=dickinson
        // CatOut: http://www.waste.org/mail/?list=pynchon-l&month=9510&msg=3079&keywords=dickinson
        wchar_t * from = MallocPtr;
        wchar_t * into = from;
        int SeenQuestionMark = 0;
        for( ;; )
        {
            wchar_t c = *from++;
            *into++ = c;

            if( c == '?' )
                SeenQuestionMark = 1;

            if( c == ' ' // remove spaces from post-'?' queries
            && SeenQuestionMark )
                into--;

            if( c == NULL )
                break;
        }
    }

    URL_COMPONENTS UrlComponents;
    memset( & UrlComponents, 0, sizeof( URL_COMPONENTS ) );
    UrlComponents.dwStructSize = sizeof( URL_COMPONENTS );

    // Leave pointers NULL and sizes non-zero to get pointers into my URL.
    UrlComponents.dwSchemeLength = nMallocLen;
    UrlComponents.dwHostNameLength = nMallocLen;
    UrlComponents.dwUrlPathLength = nMallocLen;
    UrlComponents.dwExtraInfoLength = nMallocLen;

    if( ! InternetCrackUrl( LocalPtr, 0, 0, & UrlComponents ) )
    {
        MyFree( 146, nMallocLen * sizeof( wchar_t ), LocalPtr );
        LocalPtr = NULL;
        UserUrlError( L"InternetCrackUrl" );
        return -1; // failure, but do not dismiss dialog
    }

    // Because some FTP fetches hang up WordsEx, I had excluded FTP here:
    // This looks like the determinative location to exclude FTP urls:
    // Although, perhaps it only prevents the user from entering them.
    // if( UrlComponents.nScheme != INTERNET_SCHEME_FTP... && ...

    if( UrlComponents.nScheme != INTERNET_SCHEME_HTTP
#if OKAY_TO_GET_FPT_URL
    && UrlComponents.nScheme != INTERNET_SCHEME_FTP
#endif // OKAY_TO_GET_FPT_URL
    && UrlComponents.nScheme != INTERNET_SCHEME_HTTPS )
    {
        MyFree( 156, nMallocLen * sizeof( wchar_t ), LocalPtr );
        LocalPtr = NULL;
        UserUrlError( L"InternetCrackUrl" );
        return -1; // failure, but do not dismiss dialog
    }

    if( UrlComponents.lpszExtraInfo != NULL )
    {
        // The extra parts may be a "? query... # fragment...."
        // I do not want to let fragments distinguish same URL.
        // I need to keep query to distinguish, e.g., searches.

        wchar_t * scan = UrlComponents.lpszExtraInfo;

        for( ;; )
        {
            if( *scan == NULL
            ||  *scan == L'#' )
            {
                // Since Windows help says that ICU_BROWSER_MODE
                // does not remove trailing white space after "?",
                // I will back up over any space characters at end.
                while( scan[-1] == L' ' ) // remove spaces at end of URL
                    scan --; // determinate, because of '?' atop field.
                *scan = NULL; // overwrite any '#' to cancel fragment.
                break;
            }
            scan ++;
        }
    }


    // {
    //     // before humiliation of system error, see the parts...
    //     wchar_t wk[500];
    //     wchar_t * into = wk;
    //
    //     if( UrlComponents.lpszScheme != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszScheme, UrlComponents.dwSchemeLength );
    //         into += UrlComponents.dwSchemeLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     if( UrlComponents.lpszHostName != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszHostName, UrlComponents.dwHostNameLength );
    //         into += UrlComponents.dwHostNameLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     if( UrlComponents.lpszUrlPath != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszUrlPath, UrlComponents.dwUrlPathLength );
    //         into += UrlComponents.dwUrlPathLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     if( UrlComponents.lpszExtraInfo != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszExtraInfo, UrlComponents.dwExtraInfoLength );
    //         into += UrlComponents.dwExtraInfoLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     *into ++ = NULL;
    //     Say( wk );
    // }

    dwLength = nMallocLen; // use same sloppy length as above
    wchar_t * SecondPtr = ( wchar_t * ) MyMalloc( 237, dwLength * sizeof( wchar_t ) );
    if( ! InternetCreateUrl( & UrlComponents, 0, SecondPtr, & dwLength ) )
    {
        MyFree( 229, zx, SecondPtr );
        SecondPtr = NULL;
        MyFree( 231, nMallocLen * sizeof( wchar_t ), LocalPtr );
        LocalPtr = NULL;
        UserUrlError( L"InternetCreateUrl" );
        return -1; // failure, but do not dismiss dialog
    }

    MyFree( 237, nMallocLen * sizeof( wchar_t ), LocalPtr );
    LocalPtr = NULL;
    * NewMallocPtr = SecondPtr;
    return 1; // success
}

int CWww::FirstTimeSetupInternet( )
{
    #if DO_DEBUG_CALLS
        Routine( L"294" );
    #endif
    if( m_FirstTimeCalled )
    {
        return m_ConnectedOkay;
    }
    m_FirstTimeCalled = 1;

    // The following entry points were not found in Pocket PC's WinInet.DLL.
    // So I will reverse the logic, and assume connected if nothing to call.
    // Or, for that matter, just forget the "seamless user experience"...
    // if( pInternetAttemptConnect != NULL )
    // {
    //     // Returns ERROR_SUCCESS if successful, or a Microsoft 'Win32' error value.
    //     if( pInternetAttemptConnect( 0 ) == ERROR_SUCCESS )
    //     {
    //         Say( L"pInternetAttemptConnect = TRUE" );
    //         m_ConnectedOkay = 1;
    //     }
    //     else
    //     {
    //         Say( L"pInternetAttemptConnect = FALSE" );
    //     }
    // }
    // if( pInternetGetConnectedState != NULL )
    // {
    //     // Returns TRUE if there is an Internet connection, or FALSE otherwise.
    //     // INTERNET_CONNECTION_CONFIGURED  Local system has a valid connection to the Internet, but it may or may not be currently connected.
    //     // INTERNET_CONNECTION_LAN  Local system uses a local area network to connect to the Internet.
    //     // INTERNET_CONNECTION_MODEM  Local system uses a modem to connect to the Internet.
    //     // INTERNET_CONNECTION_MODEM_BUSY  No longer used.
    //     // INTERNET_CONNECTION_OFFLINE  Local system is in offline mode.
    //     // INTERNET_CONNECTION_PROXY  Local system uses a proxy server to connect to the Internet.
    //     // INTERNET_RAS_INSTALLED  Local system has RAS installed.
    //     DWORD dwFlags;
    //     if( pInternetGetConnectedState( & dwFlags, 0 ) )
    //     {
    //         Say( L"InternetGetConnectedState = TRUE" );
    //         m_ConnectedOkay = 1;
    //     }
    //     else
    //     {
    //         Say( L"InternetGetConnectedState = FALSE" );
    //     }
    // }
    // if( pInternetGoOnline != NULL )
    // {
    //     // Returns TRUE if successful, or FALSE otherwise.
    //     if( pInternetGoOnline( L"http://Very.Many.Urls", g_hWnd, 0 ) )
    //     {
    //         Say( L"InternetGoOnline = TRUE" );
    //         m_ConnectedOkay = 1;
    //     }
    //     else
    //     {
    //         Say( L"InternetGoOnline = FALSE" );
    //     }
    // }

    m_ConnectedOkay = 1;

    m_hInternet = InternetOpen( L"WordsEx", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
    if( m_hInternet == NULL )
    {
        ProgramError( L"InternetOpen" );
        m_ConnectedOkay = 0;
        return 0;
    }

    AdjustInternetOptions( );

    return m_ConnectedOkay;
}

void CWww::AdjustInternetOptions( )
{
    #if DO_DEBUG_CALLS
        Routine( L"295" );
    #endif
    // Let's find out what some of these settings are:
    // I placed my values found in the comments below.
    // 300,000 and 3,600,000 ms are just way too long!
    // INTERNET_OPTION_CONNECT_TIMEOUT
    // INTERNET_OPTION_CONNECT_RETRIES
    // INTERNET_OPTION_CONNECT_BACKOFF
    // INTERNET_OPTION_CONTROL_SEND_TIMEOUT
    // INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT
    // INTERNET_OPTION_DATA_SEND_TIMEOUT
    // INTERNET_OPTION_DATA_RECEIVE_TIMEOUT
    // INTERNET_OPTION_READ_BUFFER_SIZE
    // INTERNET_OPTION_WRITE_BUFFER_SIZE
    // INTERNET_OPTION_USER_AGENT
    // INTERNET_OPTION_USERNAME
    // INTERNET_OPTION_CONNECTED_STATE

    DWORD dwBuffer;
    DWORD dwBufferLength;

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_CONNECT_TIMEOUT = 300,000
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_CONNECT_TIMEOUT" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_CONNECT_TIMEOUT = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_CONNECT_RETRIES = 5
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_CONNECT_RETRIES, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_CONNECT_RETRIES" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_CONNECT_RETRIES = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_CONNECT_BACKOFF = 0 ( but on PPC, 1000 )
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_CONNECT_BACKOFF, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_CONNECT_BACKOFF" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_CONNECT_BACKOFF = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_CONTROL_SEND_TIMEOUT = 300,000
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_CONTROL_SEND_TIMEOUT" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_CONTROL_SEND_TIMEOUT = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT = 3,600,000 ( but on PPC 300,000 )
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_DATA_SEND_TIMEOUT = 300,000
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_DATA_SEND_TIMEOUT, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_DATA_SEND_TIMEOUT" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_DATA_SEND_TIMEOUT = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_DATA_RECEIVE_TIMEOUT = 3,600,000 ( but on PPC 300,000 )
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_DATA_RECEIVE_TIMEOUT" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_DATA_RECEIVE_TIMEOUT = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_READ_BUFFER_SIZE = Error: Wrong type of handle.
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_READ_BUFFER_SIZE, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_READ_BUFFER_SIZE" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_READ_BUFFER_SIZE = %d.", dwBuffer );
    //     Say( wk );
    // }

    // dwBuffer = -1;
    // dwBufferLength = sizeof( dwBuffer );
    // // INTERNET_OPTION_WRITE_BUFFER_SIZE =
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_WRITE_BUFFER_SIZE, & dwBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_WRITE_BUFFER_SIZE" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_WRITE_BUFFER_SIZE = %d.", dwBuffer );
    //     Say( wk );
    // }

    // Those were DWORDs, THESE are STRINGS.
    // wchar_t szBuffer[100];

    // szBuffer[0] = NULL;
    // dwBufferLength = sizeof( szBuffer );
    // // INTERNET_OPTION_USER_AGENT = "WordsEx"
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_USER_AGENT, & szBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_USER_AGENT" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_USER_AGENT= %s.", szBuffer );
    //     Say( wk );
    // }

    // szBuffer[0] = NULL;
    // dwBufferLength = sizeof( szBuffer );
    // // INTERNET_OPTION_USERNAME = Error: Wrong type of handle.
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_USERNAME, & szBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_USERNAME" );
    // }
    // else
    // {
    //     wchar_t wk[100];
    //     wsprintf( wk, L"INTERNET_OPTION_USERNAME = %s.", szBuffer );
    //     Say( wk );
    // }

    // And this one returns a structure.
    // INTERNET_CONNECTED_INFO icBuffer;
    // icBuffer.dwConnectedState = 0;
    // icBuffer.dwFlags = 0;

    // INTERNET_CONNECTED_INFO
    // INTERNET_OPTION_CONNECTED_STATE
    // typedef struct {
    //         DWORD  dwConnectedState;
    //         DWORD  dwFlags;
    // } INTERNET_CONNECTED_INFO, * LPINTERNET_CONNECTED_INFO;
    // INTERNET_STATE_BUSY
    // Network requests are being made by the Microsoft® Win32® Internet functions.
    // INTERNET_STATE_CONNECTED
    // Connected to network. Replaces INTERNET_STATE_ONLINE.
    // INTERNET_STATE_DISCONNECTED
    // Disconnected from network. Replaces INTERNET_STATE_OFFLINE.
    // INTERNET_STATE_DISCONNECTED_BY_USER
    // Disconnected by user request. Replaces INTERNET_STATE_OFFLINE_USER.
    // INTERNET_STATE_IDLE
    // No network requests are being made by Microsoft Win32 Internet functions.

    // dwBufferLength = sizeof( icBuffer );
    // // Reported: Yes: INTERNET_STATE_CONNECTED
    // // even on PPC, although PPC is not dialed up!
    //
    // if( ! InternetQueryOption( m_hInternet, INTERNET_OPTION_CONNECTED_STATE, & icBuffer, & dwBufferLength ) )
    // {
    //     ProgramError( L"InternetQueryOption INTERNET_OPTION_CONNECTED_STATE" );
    // }
    // else
    // {
    //     if( icBuffer.dwConnectedState & INTERNET_STATE_BUSY )
    //         Say( L"Yes: INTERNET_STATE_BUSY" );
    //     if( icBuffer.dwConnectedState & INTERNET_STATE_CONNECTED )
    //         Say( L"Yes: INTERNET_STATE_CONNECTED" );
    //     if( icBuffer.dwConnectedState & INTERNET_STATE_DISCONNECTED )
    //         Say( L"Yes: INTERNET_STATE_DISCONNECTED" );
    //     if( icBuffer.dwConnectedState & INTERNET_STATE_DISCONNECTED_BY_USER )
    //         Say( L"Yes: INTERNET_STATE_DISCONNECTED_BY_USER" );
    //     if( icBuffer.dwConnectedState & INTERNET_STATE_IDLE )
    //         Say( L"Yes: INTERNET_STATE_IDLE" );
    // }

    // Change EVERYTHING down to 10 seconds or less...
    // Hmmm. That might be why I only get short pages.
    // try 30000 for all...

    // INTERNET_OPTION_CONNECT_TIMEOUT
    // INTERNET_OPTION_CONNECT_RETRIES
    // INTERNET_OPTION_CONNECT_BACKOFF
    // INTERNET_OPTION_CONTROL_SEND_TIMEOUT
    // INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT
    // INTERNET_OPTION_DATA_SEND_TIMEOUT
    // INTERNET_OPTION_DATA_RECEIVE_TIMEOUT
    // INTERNET_OPTION_READ_BUFFER_SIZE
    // INTERNET_OPTION_WRITE_BUFFER_SIZE
    // INTERNET_OPTION_USER_AGENT
    // INTERNET_OPTION_USERNAME
    // INTERNET_OPTION_CONNECTED_STATE

    dwBuffer = 30000;
    dwBufferLength = sizeof( dwBuffer );
    if( ! InternetSetOption( m_hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, & dwBuffer, dwBufferLength ) )
        ProgramError( L"InternetSetOption 1" );

    dwBuffer = 30000;
    dwBufferLength = sizeof( dwBuffer );
    if( ! InternetSetOption( m_hInternet, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, & dwBuffer, dwBufferLength ) )
        ProgramError( L"InternetSetOption 2" );

    dwBuffer = 30000;
    dwBufferLength = sizeof( dwBuffer );
    if( ! InternetSetOption( m_hInternet, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, & dwBuffer, dwBufferLength ) )
        ProgramError( L"InternetSetOption 3" );

    dwBuffer = 30000;
    dwBufferLength = sizeof( dwBuffer );
    if( ! InternetSetOption( m_hInternet, INTERNET_OPTION_DATA_SEND_TIMEOUT, & dwBuffer, dwBufferLength ) )
        ProgramError( L"InternetSetOption 4" );

    dwBuffer = 30000;
    dwBufferLength = sizeof( dwBuffer );
    if( ! InternetSetOption( m_hInternet, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, & dwBuffer, dwBufferLength ) )
        ProgramError( L"InternetSetOption 5" );
}

void CWww::ParseHttpHeader( COnePaper * pOnePaper, size_t ClaimedUrlIndex, wchar_t * pMallocBuf, size_t nMallocBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"296" );
    #endif
    // Scan passed HTTP header buffer, but caller owns, frees it.
    // Set members of pOnePaper for later examination, annotation.

    #if DO_DEBUG_HTTPHDRS
        ; Spewn( pMallocBuf, nMallocBuf );
    #endif

    // Here is a collection of other lines from Add Web Page.
    // Notice that DATE: gives only the moment when fetched.
    //
    //     1 Status: 200 OK     -- Ok. This was redundant to HTTP line.
    //     8 HTTP/1.1 302 Found
    //     1 Vary: Accept-Encoding, Cookie
    //    13 Accept-Ranges: bytes
    //    11 Transfer-Encoding: chunked
    //    20 Expires: Thu, 19 Nov 1981 08:52:00 GMT
    //    20 Connection: Keep-Alive
    //    20 Pragma: no-cache
    //    55 Connection: close
    //    86 HTTP/1.1 200 OK
    //     4 Content-Type: video/mp4 -- Yeah, I need binaries ( Do as phase 2! ).
    //     1 Last-Modified: Sat, 08 Oct 2005 17:50:58 GMT
    //     1 Last-Modified: Mon, 29 Mar 2004 17:24:20 GMT
    //     1 Last-Modified: Mon, 18 Dec 2006 23:46:03 GMT
    //     1 Last-Modified: Mon, 18 Dec 2006 01:44:39 GMT
    //
    //     I think I once understood the difference in these L names...
    //     1 Content-Location: http://www.ufodc.com/index.htm
    //     1 Location: /asp/default.asp?t=webshop
    //
    //     Here are some dates alone on a line:
    //     -- oh, that was my spew line above!
    //     1 Sat, 08 Oct 2005 21:14:27 GMT
    //     1 Sat, 08 Oct 2005 17:50:58 GMT
    //     1 Mon, 29 Mar 2004 17:24:20 GMT
    //     And the current time IS:
    //     1 Wed, 28 Mar 2007 03:10:32 GMT
    //     1 Wed, 28 Mar 2007 03:10:29 GMT

    // Here is a collection of header lines from Add Cache.
    // Notice that reported whole header has NO DATES!
    // -- So, I should get dates from info structure
    //
    //  394 HTTP/1.1 200 OK
    //  149 Content-Type: text/html
    //  147 Content-Type: image/gif
    //   54 Transfer-Encoding: chunked
    //   48 Keep-Alive: timeout=15, max=100
    //   40 HTTP/1.0 200 OK
    //   20 Content-Type: image/jpeg
    //   19 Content-Type: application/x-javascript
    //   18 Content-Type: text/html; charset=utf-8
    //   15 Content-Type: text/html; charset=UTF-8
    //   14 Content-Type: text/javascript
    //   13 Keep-Alive: timeout=5, max=100
    //   13 Content-Type: image/png
    //   12 Content-Type: text/css
    //    9 Content-Type: text/html; charset=ISO-8859-1
    //    7 Content-Language: en
    //    6 Content-Type: text/html; charset=iso-8859-1
    //    5 Content-type: text/html
    //    5 Content-Type: text/html;charset=ISO-8859-1
    //    3 Content-Length: 85
    //    3 Content-Language: en-US
    //    3 Allow: GET
    //    2 WWW-Authenticate: Basic realm="Passworded Area"
    //    2 Page-Completion-Status: Normal
    //    2 P3P: policyref="http://www.tacoda.com/w3c/p3p.xml", CP="NON DSP COR NID CURa ADMo DEVo TAIo PSAo PSDo OUR DELa IND PHY ONL UNI COM NAV DEM"
    //    2 Content-Type: text/css; charset=utf-8
    //    1 Transfer-Encoding:  chunked
    //    1 Pragma:
    //    1 P3P: policyref="http://www.aad.org/w3c/p3p.xml", CP="NOI NID ADMa OUR BUS UNI"
    //    1 MIME-version: 1.0
    //    1 HTTP/1.1 200 Sending data and caching
    //    1 HTTP/1.1 200
    //    1 Etag: "177c-3e9c6237"
    //    1 ETag: u-1g3s18hn-6gop-p9tyii4dj-1pfe0bzd6e0
    //    1 ETag: W/"991-1055539313000"
    //    1 ETag: "a5-442f8760"
    //    1 Content-transfer-encoding: 8bit
    //    1 Content-Type: text/plain
    //    1 Content-Type: text/html;charset=iso-8859-1
    //    1 Content-Location: http://www....
    //    1 Content-Length: 997
    //    1 Content-Language: en-AU





    pOnePaper->HttpHeaderContentType = CONTENT_OTHER; // in case none seen
    pOnePaper->HttpHeaderContentLanguage = LANGUAGE_UNKNOWN; // in case none seen
    pOnePaper->LanguageGroup = LANGUAGE_GROUP_UNKNOWN; // in case none solved
    pOnePaper->HttpHeaderContentEncoding = ENCODING_NONE; // in case none seen

    wchar_t * scan = pMallocBuf;
    wchar_t * stop = scan + nMallocBuf;
    for( ;; )
    {

        // Find first non-space character

        for( ;; )
        {
            if( scan >= stop )
                break;

            if( *scan > ' ' ) // skip any CR, LF, SPACE, garbage atop http header line
                break;

            scan++;
        }

        if( scan >= stop )
            break;

        switch( *scan )
        {
            // Treat header lines starting with C for Content-type,
            // Treat header lines starting with C for Content-language,
            // The upper/lower case of these lines is all uncontrolled.
        case 'c':
        case 'C':
            if( ( scan[1] | ' ' ) == 'o'
            &&  ( scan[2] | ' ' ) == 'n'
            &&  ( scan[3] | ' ' ) == 't'
            &&  ( scan[4] | ' ' ) == 'e'
            &&  ( scan[5] | ' ' ) == 'n'
            &&  ( scan[6] | ' ' ) == 't'
            &&  scan[7] == '-' )
            {

                #if DO_DEBUG_CONTENT
                {
                    // Extract this line to spew
                    wchar_t * tend = scan + 7;
                    for( ;; )
                    {
                        if( tend >= stop )
                            break;

                        if( *tend < ' ' ) // find CR, LF, NULL
                            break;

                        tend++;
                    }
                    // Add temporary null at newline
                    wchar_t save = *tend;
                    *tend = NULL;
                    ; Spew( scan ); // show all Content- ... lines
                    *tend = save;
                }
                #endif

                // Try to match Content-Type:
                if( ( scan[8] | ' ' ) == 't'
                &&  ( scan[9] | ' ' ) == 'y'
                &&  ( scan[10] | ' ' ) == 'p'
                &&  ( scan[11] | ' ' ) == 'e'
                &&  scan[12] == ':' )
                {
                    // Process Content-Type:

                    // e.g., Content-Type: text/css;charset=ISO-8859-1

                    scan += 13; // after colon
                    for( ;; )
                    {
                        if( *scan != ' ' )
                            break;
                        scan++; // skip over any spaces
                    }

                    switch( *scan )
                    {
                        case 't':
                        case 'T':
                            if( ( scan[1] | ' ' ) == 'e'
                            &&  ( scan[2] | ' ' ) == 'x'
                            &&  ( scan[3] | ' ' ) == 't' )
                            {
                                scan += 4; // after "text"
                                pOnePaper->HttpHeaderContentType = CONTENT_TEXT;

                                if( scan[0] == '/'
                                &&  ( scan[1] | ' ' ) == 'h'
                                &&  ( scan[2] | ' ' ) == 't'
                                &&  ( scan[3] | ' ' ) == 'm'
                                &&  ( scan[4] | ' ' ) == 'l' )
                                {
                                    scan += 5; // after "/html"
                                    pOnePaper->HttpHeaderContentType = CONTENT_HTMLTEXT;
                                }
                                else if( scan[0] == '/'
                                &&  ( scan[1] | ' ' ) == 'p'
                                &&  ( scan[2] | ' ' ) == 'l'
                                &&  ( scan[3] | ' ' ) == 'a'
                                &&  ( scan[4] | ' ' ) == 'i'
                                &&  ( scan[5] | ' ' ) == 'n' )
                                {
                                    scan += 6; // after "/plain"
                                    pOnePaper->HttpHeaderContentType = CONTENT_PLAINTEXT;
                                }
                                else if( scan[0] == '/'
                                &&  ( scan[1] | ' ' ) == 'r'
                                &&  ( scan[2] | ' ' ) == 't'
                                &&  ( scan[3] | ' ' ) == 'f' )
                                {
                                    scan += 4; // after "/rtf"
                                    // Later, accept RTF, add an RTF parser.
                                    pOnePaper->HttpHeaderContentType = CONTENT_RTFTEXT;
                                }
                                else if( scan[4] == '/' )
                                {
                                    pOnePaper->HttpHeaderContentType = CONTENT_OTHERTEXT;
                                }

                                if( pOnePaper->HttpHeaderContentType != CONTENT_OTHERTEXT )
                                {
                                    // so, for text, text/plain, text/html text/rtf:
                                    // parse charset to choose font for showing page:
                                    // Just scrunch together all alpha+digit
                                    // No, pass verbatim string to new routine.
                                    // 929 Content-Type: text/html; charset=utf-8
                                    // 823 Content-Type: text/html; charset=ISO-8859-1
                                    // 547 Content-Type: text/html; charset=UTF-8
                                    // 373 Content-Type: text/html; charset=iso-8859-1
                                    //   9 Content-Type: text/html; charset=Windows-1252
                                    //   2 Content-type: text/html;charset=8859_1
                                    //   1 Content-Type: text/html;charset=utf-8
                                    //   1 Content-Type: text/html; charset=windows-1251
                                    //   1 Content-Type: text/html; charset=ISO8859_1
                                    //   1 Content-Type: text/html; Charset=utf-8

                                    // scan should be on semicolon now.
                                    if( *scan == ';' )
                                        scan++; // skip over semicolon
                                    for( ;; )
                                    {
                                        if( *scan != ' ' )
                                            break;
                                        scan++; // skip over any spaces
                                    }
                                    if( ( scan[0] | ' ' ) == 'c'
                                    &&  ( scan[1] | ' ' ) == 'h'
                                    &&  ( scan[2] | ' ' ) == 'a'
                                    &&  ( scan[3] | ' ' ) == 'r'
                                    &&  ( scan[4] | ' ' ) == 's'
                                    &&  ( scan[5] | ' ' ) == 'e'
                                    &&  ( scan[6] | ' ' ) == 't' )
                                    {
                                        scan += 7; // after "charset"
                                        for( ;; )
                                        {
                                            if( *scan != ' '
                                            &&  *scan != '=' )
                                                break;
                                            scan++; // skip any spaces,equals
                                        }
                                        // Accept only alphas and digits until end.
                                        // Many names include punctuation, ignored.
                                        // lowercase it. Save in CSol CSolCharsets.
                                        // No, pass verbatim string to new routine.
                                        wchar_t copied[60];
                                        size_t i = 0;
                                        for( ;; )
                                        {
                                            wchar_t c = *scan++ & 0x7f;
                                            if( c <= ' ' ) // stop on space cr lf null
                                                break;
                                            copied[i] = c; // verbatim now
                                            if( ++i == 59 )
                                                break;
                                        }
                                        if( i > 0 )
                                        {
                                            // some non-empty thing is present
                                            copied[i] = NULL;
                                            pOnePaper->HttpHeaderCharset = BestIndexforCharsetString( copied, NULL );
                                            #if DO_DEBUG_CONTENT
                                                ; SpewValue( copied, pOnePaper->HttpHeaderCharset );
                                            #endif
                                        }
                                        break;
                                    }
                                }
                            }
                            break;

                        case 'a':
                        case 'A':
                            // Application...
                            break;

                        case 'i':
                        case 'I':
                            // Image...
                            break;
                    }
                    goto FindNextNewline;
                    // End of Content-Type:
                }

                // Try to match Content-Language:
                if( ( scan[8] | ' ' ) == 'l'
                &&  ( scan[9] | ' ' ) == 'a'
                &&  ( scan[10] | ' ' ) == 'n'
                &&  ( scan[11] | ' ' ) == 'g'
                &&  ( scan[12] | ' ' ) == 'u'
                &&  ( scan[13] | ' ' ) == 'a'
                &&  ( scan[14] | ' ' ) == 'g'
                &&  ( scan[15] | ' ' ) == 'e'
                &&  scan[16] == ':' )
                {
                    // Process Content-Language:
                    scan += 17; // after colon
                    for( ;; )
                    {
                        if( *scan != ' ' )
                            break;
                        scan++; // skip any spaces
                    }
                    // Pass verbatim string to new routine.
                    wchar_t copied[60];
                    size_t i = 0;
                    for( ;; )
                    {
                        wchar_t c = *scan++ & 0x7f;
                        if( c < ' ' ) // stop on cr lf null
                            break;
                        copied[i] = c; // verbatim now
                        if( ++i == 59 )
                            break;
                    }
                    if( i > 0 )
                    {
                        // some non-empty thing is present
                        copied[i] = NULL;
                        pOnePaper->HttpHeaderContentLanguage = BestIndexforLanguageString( copied, NULL );
                        pOnePaper->LanguageGroup = GroupIndexForLanguageIndex( pOnePaper->HttpHeaderContentLanguage );
                        #if DO_DEBUG_LANGUAGE
                            ; Spew( L"ParseHttpHeader got this table index for Content-language:" );
                            ; SpewValue( copied, pOnePaper->HttpHeaderContentLanguage );
                        #endif
                    }

                    goto FindNextNewline;
                    // End of Content-Language:
                }

                // Try to match Content-Encoding:
                if( ( scan[8] | ' ' ) == 'e'
                &&  ( scan[9] | ' ' ) == 'n'
                &&  ( scan[10] | ' ' ) == 'c'
                &&  ( scan[11] | ' ' ) == 'o'
                &&  ( scan[12] | ' ' ) == 'd'
                &&  ( scan[13] | ' ' ) == 'i'
                &&  ( scan[14] | ' ' ) == 'n'
                &&  ( scan[15] | ' ' ) == 'g'
                &&  scan[16] == ':' )
                {
                    // Process Content-Encoding:

                    // Any Content-Encoding makes file inedible.
                    // because an identity type is never stated.

                    pOnePaper->HttpHeaderContentEncoding = ENCODING_USED;
                    goto FindNextNewline;
                    // End of Content-Encoding:
                }

                // Try to match Content-Length:
                if( ( scan[8] | ' ' ) == 'l'
                &&  ( scan[9] | ' ' ) == 'e'
                &&  ( scan[10] | ' ' ) == 'n'
                &&  ( scan[11] | ' ' ) == 'g'
                &&  ( scan[12] | ' ' ) == 't'
                &&  ( scan[13] | ' ' ) == 'h'
                &&  scan[14] == ':' )
                {
                    // Process Content-Length:
                    scan += 15; // after colon
                    for( ;; )
                    {
                        if( *scan != ' ' )
                            break;
                        scan++; // skip over any spaces
                    }
                    int accu = 0;
                    for( ;; )
                    {
                        int c = *scan & 127;
                        if( iswdigit( c ) )
                        {
                            accu *= 10;
                            accu += c - '0';
                        }
                        else
                        {
                            break;
                        }
                        scan++; // skip over URL
                    }
                    pOnePaper->HttpHeaderContentLength = accu;
                    goto FindNextNewline;
                    // End of Content-Length:
                }

                // End of Content-...:
            }

            // end of case c or C.
            break;


            // Treat header lines starting with L for Location,
            // Treat header lines starting with L for Last-modified.
            // The upper/lower case of these lines is all uncontrolled.
        case 'l':
        case 'L':
            if( ( scan[1] | ' ' ) == 'a'
            &&  ( scan[2] | ' ' ) == 's'
            &&  ( scan[3] | ' ' ) == 't'
            &&    scan[4]  == '-'
            &&  ( scan[5] | ' ' ) == 'm'
            &&  ( scan[6] | ' ' ) == 'o'
            &&  ( scan[7] | ' ' ) == 'd'
            &&  ( scan[8] | ' ' ) == 'i'
            &&  ( scan[9] | ' ' ) == 'f'
            &&  ( scan[10] | ' ' ) == 'i'
            &&  ( scan[11] | ' ' ) == 'e'
            &&  ( scan[12] | ' ' ) == 'd'
            &&    scan[13] == ':' )
            {
                // Process Last-Modified:
                // e.g., Last-Modified: Sat, 24 Mar 2007 20:02:13 GMT
                //     1 Last-Modified: Mon, 29 Mar 2004 17:24:20 GMT
                scan += 14; // after colon

                for( ;; )
                {
                    if( *scan != ' ' )
                        break;
                    scan++; // skip over any spaces
                }
                #if DO_DEBUG_HTTPDATE
                {
                    // Extract this line to spew
                    wchar_t * tend = scan;
                    for( ;; )
                    {
                        if( tend >= stop )
                            break;

                        if( *tend < ' ' ) // find CR, LF, NULL
                            break;

                        tend++;
                    }
                    // Add temporary null at newline
                    wchar_t save = *tend;
                    *tend = NULL;
                    ; Spew( scan ); // show all Last-Modified: ... lines
                    *tend = save;
                }
                #endif

                // Scan is sitting on this first letter:
                // Mon, 29 Mar 2004 17:24:20 GMT
                // 0123456789 123456789 12345678
                // Use such a fixed format, or ignore it.

                // Take 8 decimal digits directly into 4-bits each ( BCD ).
                // Naw, BCD is really stupid.
                // Later, format MSB first, like this: 2001-09-11

                if( scan[3] == ','
                && scan[7] == ' '
                && scan[11] == ' '
                && scan[16] == ' '
                && scan[19] == ':'
                && scan[22] == ':' )
                {
                    // Mon, 29 Mar 2004 17:24:20 GMT
                    // 0123456789 123456789 12345678
                    int mo = 0;
                    int L1 = ( scan[8] | ' ' );
                    int L2 = ( scan[9] | ' ' );
                    int L3 = ( scan[10] | ' ' );
                    switch( L1 + L2 + L3 )
                    {
                    case ( 'j'+'a'+'n' ): mo =  1; break; // Jan
                    case ( 'f'+'e'+'b' ): mo =  2; break; // Feb
                    case ( 'm'+'a'+'r' ): mo =  3; break; // Mar
                    case ( 'a'+'p'+'r' ): mo =  4; break; // Apr
                    case ( 'm'+'a'+'y' ): mo =  5; break; // May
                    case ( 'j'+'u'+'n' ): mo =  6; break; // Jun
                    case ( 'j'+'u'+'l' ): mo =  7; break; // Jul
                    case ( 'a'+'u'+'g' ): mo =  8; break; // Aug
                    case ( 's'+'e'+'p' ): mo =  9; break; // Sep
                    case ( 'o'+'c'+'t' ): mo = 10; break; // Oct
                    case ( 'n'+'o'+'v' ): mo = 11; break; // Nov
                    case ( 'd'+'e'+'c' ): mo = 12; break; // Dec
                    }
                    if( mo != 0 )
                    {
                        // Mon, 29 Mar 2004 17:24:20 GMT
                        // 0123456789 123456789 12345678
                        int yr = 0;
                        yr += ( scan[12] - '0' ) * 1000;
                        yr += ( scan[13] - '0' ) * 100;
                        yr += ( scan[14] - '0' ) * 10;
                        yr += ( scan[15] - '0' ) * 1;

                        int dy = 0;
                        dy += ( scan[5] - '0' ) * 10;
                        dy += ( scan[6] - '0' ) * 1;

                        // A decimal multiply now allows simple
                        // itoa to recover 8 digits to reformat.
                        int yyyymmdd = ( yr * 10000 ) + ( mo * 100 ) + dy;

                        pOnePaper->HttpHeaderDateyyyymmdd = yyyymmdd;
                    }
                }



                goto FindNextNewline;
                // end of Last-Modified:
            }

            // Continue with the case 'L' as in Location....

            if( ( scan[1] | ' ' ) == 'o'
            &&  ( scan[2] | ' ' ) == 'c'
            &&  ( scan[3] | ' ' ) == 'a'
            &&  ( scan[4] | ' ' ) == 't'
            &&  ( scan[5] | ' ' ) == 'i'
            &&  ( scan[6] | ' ' ) == 'o'
            &&  ( scan[7] | ' ' ) == 'n'
            &&  scan[8] == ':' )
            {
                // Process Location:
                scan += 9; // after colon
                for( ;; )
                {
                    if( *scan != ' ' )
                        break;
                    scan++; // skip over any spaces
                }
                wchar_t * atop = scan;
                for( ;; )
                {
                    if( *scan <= ' ' ) // stop on CR, LF, NULL, space, garbage
                        break;
                    scan++; // skip over URL
                }

                // I need to can'ize it, hang it on tree, remember it.
                // Having an HTTP header, I have this URL in the tree.
                // During file load, I might not have a URL, get zero.
                int NewUrlIndex = 0;

                // The Cache does not seem to keep 3xx pages,
                // so I don't need new URL when passing NULL.
                int nBase = ClaimedUrlIndex;
                if( nBase != 0 )
                {
                    // Better for me to fetch the base url text,
                    // because HTML parser will call Hang often.
                    wchar_t * pMalBase = ( wchar_t * ) CSolAllUrls.GetFullKey( nBase );
                    if( pMalBase != NULL )
                    {
                        wchar_t save_c = *scan;
                        *scan = NULL;
                        NewUrlIndex = CombineAndHangUrl( pMalBase, atop, 0 );
                        *scan = save_c;
                        MyFree( 2131, zx, pMalBase );
                        pMalBase = NULL;
                    }
                    pOnePaper->NewLocationIndex = NewUrlIndex; // Tell fetcher
                }

                if( NewUrlIndex > 1 )
                    pOnePaper->NewLocationIndex = NewUrlIndex; // forget any prior

                // Fetch etc. will follow up on pOnePaper->NewLocationIndex;

                goto FindNextNewline;
                // end of Location:
            }

            // end of case l or L.
            break;


            // Treat header lines starting with H for HTTP
        case 'h':
        case 'H':
            if( ( scan[1] | ' ' ) == 't'
            &&  ( scan[2] | ' ' ) == 't'
            &&  ( scan[3] | ' ' ) == 'p'
            &&  ( scan[4] | ' ' ) == '/'
            &&  ( scan[5] | ' ' ) == '1'
            &&  ( scan[6] | ' ' ) == '.'
            && ( ( scan[7] | ' ' ) == '0' || ( scan[7] | ' ' ) == '1' )
            &&  scan[8] == ' ' )
            {
                // Process HTTP/1.1 200 OK
                scan += 9; // after space
                for( ;; )
                {
                    if( *scan != ' ' )
                        break;
                    scan++; // skip over any spaces
                }
                int accu = 0;
                for( ;; )
                {
                    int c = *scan & 127;
                    if( iswdigit( c ) )
                    {
                        accu *= 10;
                        accu += c - '0';
                    }
                    else
                    {
                        break;
                    }
                    scan++; // skip over 200 300 400 etc.
                }
                // I need a coarser value in pOnePaper->HttpHeaderStatus.
                // There can be several 2XX values, but I only want 200.
                if( accu == 200 )
                    pOnePaper->HttpHeaderStatus = FETCH_STATUS_200_SUCCESS;
                else if( accu >= 300 && accu < 400 )
                    pOnePaper->HttpHeaderStatus = FETCH_STATUS_300_REDIRECT;
                else
                    pOnePaper->HttpHeaderStatus = FETCH_STATUS_400_FAILURE;
                goto FindNextNewline;
                // end of Location:
            }

            // end of case h or H.
            break;


        }

    FindNextNewline: ;

        for( ;; )
        {
            if( scan >= stop )
                break;

            if( *scan < ' ' ) // Match CR, LF, NULL, garbage until next http header line
                break;

            scan++;
        }
    }
}

int CWww::CombineAndHangUrl( wchar_t * szBaseUrl, wchar_t * szLinkUrl, int ForForm )
{
    #if DO_DEBUG_CALLS
        // Routine( L"297" );
    #endif
    // The szBaseUrl has been canonicalized, or NULL.
    // Or after load file, it may contain "file:...."

    // The szLinkUrl must be canonicalized, cracked,
    // ( that dropping any passwords, fragments, etc. )
    // joined to the szBaseUrl, and if the resultant
    // URL is valid & absolute, add to CSol CSolAllUrls,
    // returning the index therein; Or, return zero.

    // 1. I have determined that it is invalid to immediately
    // try to canonicalize the Link URL, which may be relative.

    // 2. On Win2k at home, I have a "refresh" page making some
    // URL X, then XX, then XXXX, etc., until all fetches fail.
    // I think that is because of changes to fix realization #1.

    // So:
    // Do a pre-test on link for absolute, including all ftp/http( s ).
    // N.b. Do that after skipping any initial spaces in parameter.
    // If absolute, base is n/a, just canonicalize the Link URL.
    // If relative, join to base, which I think should canonicalize.
    // After all that, do the crack, and then omit ftp if desired.

    #if DO_DEBUG_FETCH
        ; Spew( L"CombineAndHang input ( base, link ):" );
        if( szBaseUrl == NULL )
            { Spew( L"Base URL is null ( which is okay )." ); }
        else
            { Spew( szBaseUrl ); }
        if( szLinkUrl == NULL )
            { Spew( L"LINK URL is null ( which is NOT okay )." ); }
        else
            { Spew( szLinkUrl ); }
    #endif

    if( szLinkUrl == NULL )
    {
        ProgramError( L"CombineAndHangUrl: szLinkUrl == NULL" );
        return 0; // failure. Ignore all invalid URLs.
    }

    // Canonicalize crashed on home cpu while parsing fetched URL:
    // http://images.google.com/images?hl=en&lr=&sa=N&tab=wi&q=Internet
    // Which crash tried to join up:
    // CombineAndHang input ( base, link ):
    // http://images.google.com/images?hl=en&lr=&sa=N&tab=wi&q=Internet
    //  /imgres?imgurl=http://imgs.xkcd.com/comics/bored_with_the_internet.jpg&imgrefurl=http://xkcd.com/c77.html&start=10&h=798&w=640&sz=95&tbnid=Ut7z42bGuFN8qM:&tbnh=143&tbnw=115&hl=en&prev=/images%3Fq%3DInternet%26svnum%3D10%26hl%3Den%26lr%3D%26ie%3DUTF-8%26sa%3DN
    // I notice this heap error in the debug window too:
    // HEAP[wordsex.exe]: Heap block at 0018FA80 modified at 0018FC90 past requested size of 208
    // try-catch did not help. Maybe if I raise *3 to *4?
    // Maybe the malloc length s/b in bytes, not wchar_t?
    // That didn't help. Same heap 208 error. !!!!
    // On work Win2000Pro, no exception, but wierd outputs in spew.txt.
    // Oh, that's cuz LocalPtr is into Garbage.


// |    // DEC 07 2007 -
// |    // I'm seeing this bug chronically forever on my home Win2KPro,
// |    // Even after building on VC .net, which I though made it okay.
// |    // And I'm tired of this shit. I will write my own if I have to.
// |
// |    // But wait... I see the length of my malloc, despite doubling it
// |    // for wchar_t, and only passing 1/2 the char count into windows;
// |    // I notice my length is only of one of the two input wcslen's!
// |    // So try adding both together, and then still goose it way up.
// |    // Oh, no. This isn't the join call, just a 1-input-url-parameter
// |    // canonicalize call. Add Great Spew-everythings before the call.
// |
// |    // I see today's heap dump has the same relationship as above;
// |    // Now notice, this heap address is not related to my memory:
// |    // HEAP[wordsex.exe]: Heap block at 0019D680 modified at 0019D890 past requested size of 208
// |
// |    // Parameters Passed into:
// |    // ... InternetCanonicalizeUrl( szLinkUrl, LocalPtr, & dwLength, ...
// |    // from
// |    // ... Www.CombineAndHangUrl( m_pMalBase, szValue, 0 );
// |
// |    // Ohhhh! Now, here's a thought: It is not right to ask Windows
// |    // to canonicalize a relative url, which, as in the example below,
// |    // may have all sorts of bizarre stuff that confuses canonicalize!
// |    // So let's not canonicalize the szLinkUrl; Just pass it to Join.
// |
// |    // Next-morning insight: Both Link URLs above and below start with
// |    // a space character! -- That's probably true of all URLs coming
// |    // from a tag attr, as I prefix a space; I should probably +1 it.
// |
// |szLinkUrl:
// |0151E9B4  20 00 2F 00 52 00 65 00 64 00 69 00 72 00 65 00 63 00 74 00   ./.R.e.d.i.r.e.c.t.
// |0151E9C8  6F 00 72 00 2E 00 61 00 73 00 70 00 78 00 3F 00 62 00 73 00  o.r...a.s.p.x.?.b.s.
// |0151E9DC  74 00 61 00 74 00 3D 00 74 00 73 00 30 00 31 00 26 00 74 00  t.a.t.=.t.s.0.1.&.t.
// |0151E9F0  79 00 70 00 65 00 3D 00 43 00 4B 00 26 00 73 00 6F 00 75 00  y.p.e.=.C.K.&.s.o.u.
// |0151EA04  72 00 63 00 65 00 3D 00 62 00 75 00 73 00 63 00 61 00 64 00  r.c.e.=.b.u.s.c.a.d.
// |0151EA18  6F 00 72 00 2E 00 74 00 65 00 72 00 72 00 61 00 2E 00 65 00  o.r...t.e.r.r.a...e.
// |0151EA2C  73 00 26 00 69 00 64 00 3D 00 73 00 70 00 6F 00 6E 00 73 00  s.&.i.d.=.s.p.o.n.s.
// |0151EA40  6F 00 72 00 65 00 64 00 5F 00 73 00 68 00 6F 00 70 00 70 00  o.r.e.d._.s.h.o.p.p.
// |0151EA54  69 00 6E 00 67 00 26 00 70 00 61 00 72 00 74 00 6E 00 65 00  i.n.g.&.p.a.r.t.n.e.
// |0151EA68  72 00 3D 00 73 00 68 00 6F 00 70 00 70 00 69 00 6E 00 67 00  r.=.s.h.o.p.p.i.n.g.
// |0151EA7C  26 00 71 00 75 00 65 00 72 00 79 00 3D 00 64 00 69 00 72 00  &.q.u.e.r.y.=.d.i.r.
// |0151EA90  65 00 63 00 74 00 76 00 2B 00 74 00 72 00 61 00 6E 00 73 00  e.c.t.v.+.t.r.a.n.s.
// |0151EAA4  70 00 6F 00 6E 00 64 00 65 00 72 00 2B 00 6D 00 75 00 6C 00  p.o.n.d.e.r.+.m.u.l.
// |0151EAB8  74 00 69 00 73 00 77 00 69 00 74 00 63 00 68 00 2B 00 6F 00  t.i.s.w.i.t.c.h.+.o.
// |0151EACC  70 00 65 00 72 00 61 00 74 00 69 00 6F 00 6E 00 26 00 70 00  p.e.r.a.t.i.o.n.&.p.
// |0151EAE0  6F 00 73 00 69 00 74 00 69 00 6F 00 6E 00 3D 00 33 00 26 00  o.s.i.t.i.o.n.=.3.&.
// |0151EAF4  74 00 61 00 72 00 67 00 65 00 74 00 3D 00 68 00 74 00 74 00  t.a.r.g.e.t.=.h.t.t.
// |0151EB08  70 00 25 00 33 00 61 00 25 00 32 00 66 00 25 00 32 00 66 00  p.%.3.a.%.2.f.%.2.f.
// |0151EB1C  74 00 69 00 65 00 6E 00 64 00 61 00 2E 00 74 00 65 00 72 00  t.i.e.n.d.a...t.e.r.
// |0151EB30  72 00 61 00 2E 00 65 00 73 00 25 00 32 00 66 00 70 00 72 00  r.a...e.s.%.2.f.p.r.
// |0151EB44  25 00 32 00 66 00 31 00 33 00 37 00 32 00 34 00 32 00 31 00  %.2.f.1.3.7.2.4.2.1.
// |0151EB58  25 00 32 00 66 00 50 00 25 00 32 00 66 00 30 00 31 00 33 00  %.2.f.P.%.2.f.0.1.3.
// |0151EB6C  25 00 32 00 66 00 31 00 31 00 32 00 39 00 39 00 25 00 32 00  %.2.f.1.1.2.9.9.%.2.
// |0151EB80  66 00 43 00 68 00 6F 00 63 00 6F 00 74 00 65 00 6C 00 65 00  f.C.h.o.c.o.t.e.l.e.
// |0151EB94  67 00 72 00 61 00 6D 00 2E 00 68 00 74 00 6D 00 6C 00 26 00  g.r.a.m...h.t.m.l.&.
// |0151EBA8  65 00 73 00 74 00 61 00 74 00 3D 00 74 00 73 00 30 00 31 00  e.s.t.a.t.=.t.s.0.1.
// |0151EBBC  20 00 00 00 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC   ...ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ
// |
// |LocalPtr:
// |016498D0  00 00 AD BA 0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 0D F0 AD BA 0D  ..­º.ð­º.ð­º.ð­º.ð­º.
// |
// |dwLength: 786
// |
// |m_pMalBase:
// |02A96068  68 00 74 00 74 00 70 00 3A 00 2F 00 2F 00 62 00 75 00 73 00 63  h.t.t.p.:././.b.u.s.c
// |02A9607D  00 61 00 64 00 6F 00 72 00 2E 00 74 00 65 00 72 00 72 00 61 00  .a.d.o.r...t.e.r.r.a.
// |02A96092  2E 00 65 00 73 00 2F 00 44 00 65 00 66 00 61 00 75 00 6C 00 74  ..e.s./.D.e.f.a.u.l.t
// |02A960A7  00 2E 00 61 00 73 00 70 00 78 00 3F 00 6C 00 6F 00 63 00 3D 00  ...a.s.p.x.?.l.o.c.=.
// |02A960BC  73 00 65 00 61 00 72 00 63 00 68 00 62 00 6F 00 78 00 26 00 63  s.e.a.r.c.h.b.o.x.&.c
// |02A960D1  00 61 00 3D 00 63 00 26 00 71 00 75 00 65 00 72 00 79 00 3D 00  .a.=.c.&.q.u.e.r.y.=.
// |02A960E6  64 00 69 00 72 00 65 00 63 00 74 00 76 00 25 00 32 00 30 00 74  d.i.r.e.c.t.v.%.2.0.t
// |02A960FB  00 72 00 61 00 6E 00 73 00 70 00 6F 00 6E 00 64 00 65 00 72 00  .r.a.n.s.p.o.n.d.e.r.
// |02A96110  25 00 32 00 30 00 6D 00 75 00 6C 00 74 00 69 00 73 00 77 00 69  %.2.0.m.u.l.t.i.s.w.i
// |02A96125  00 74 00 63 00 68 00 25 00 32 00 30 00 6F 00 70 00 65 00 72 00  .t.c.h.%.2.0.o.p.e.r.
// |02A9613A  61 00 74 00 69 00 6F 00 6E 00 26 00 73 00 6F 00 75 00 72 00 63  a.t.i.o.n.&.s.o.u.r.c
// |02A9614F  00 65 00 3D 00 53 00 65 00 61 00 72 00 63 00 68 00 00 00 7A 37  .e.=.S.e.a.r.c.h...z7
// |
// |szValue:
// |0151EA32  64 00 3D 00 73 00 70 00 6F 00 6E 00 73 00 6F 00 72 00 65 00 64  d.=.s.p.o.n.s.o.r.e.d
// |0151EA47  00 5F 00 73 00 68 00 6F 00 70 00 70 00 69 00 6E 00 67 00 26 00  ._.s.h.o.p.p.i.n.g.&.
// |0151EA5C  70 00 61 00 72 00 74 00 6E 00 65 00 72 00 3D 00 73 00 68 00 6F  p.a.r.t.n.e.r.=.s.h.o
// |0151EA71  00 70 00 70 00 69 00 6E 00 67 00 26 00 71 00 75 00 65 00 72 00  .p.p.i.n.g.&.q.u.e.r.
// |0151EA86  79 00 3D 00 64 00 69 00 72 00 65 00 63 00 74 00 76 00 2B 00 74  y.=.d.i.r.e.c.t.v.+.t
// |0151EA9B  00 72 00 61 00 6E 00 73 00 70 00 6F 00 6E 00 64 00 65 00 72 00  .r.a.n.s.p.o.n.d.e.r.
// |0151EAB0  2B 00 6D 00 75 00 6C 00 74 00 69 00 73 00 77 00 69 00 74 00 63  +.m.u.l.t.i.s.w.i.t.c
// |0151EAC5  00 68 00 2B 00 6F 00 70 00 65 00 72 00 61 00 74 00 69 00 6F 00  .h.+.o.p.e.r.a.t.i.o.
// |0151EADA  6E 00 26 00 70 00 6F 00 73 00 69 00 74 00 69 00 6F 00 6E 00 3D  n.&.p.o.s.i.t.i.o.n.=
// |0151EAEF  00 33 00 26 00 74 00 61 00 72 00 67 00 65 00 74 00 3D 00 68 00  .3.&.t.a.r.g.e.t.=.h.
// |0151EB04  74 00 74 00 70 00 25 00 33 00 61 00 25 00 32 00 66 00 25 00 32  t.t.p.%.3.a.%.2.f.%.2
// |0151EB19  00 66 00 74 00 69 00 65 00 6E 00 64 00 61 00 2E 00 74 00 65 00  .f.t.i.e.n.d.a...t.e.
// |0151EB2E  72 00 72 00 61 00 2E 00 65 00 73 00 25 00 32 00 66 00 70 00 72  r.r.a...e.s.%.2.f.p.r
// |0151EB43  00 25 00 32 00 66 00 31 00 33 00 37 00 32 00 34 00 32 00 31 00  .%.2.f.1.3.7.2.4.2.1.
// |0151EB58  25 00 32 00 66 00 50 00 25 00 32 00 66 00 30 00 31 00 33 00 25  %.2.f.P.%.2.f.0.1.3.%
// |0151EB6D  00 32 00 66 00 31 00 31 00 32 00 39 00 39 00 25 00 32 00 66 00  .2.f.1.1.2.9.9.%.2.f.
// |0151EB82  43 00 68 00 6F 00 63 00 6F 00 74 00 65 00 6C 00 65 00 67 00 72  C.h.o.c.o.t.e.l.e.g.r
// |0151EB97  00 61 00 6D 00 2E 00 68 00 74 00 6D 00 6C 00 26 00 65 00 73 00  .a.m...h.t.m.l.&.e.s.
// |0151EBAC  74 00 61 00 74 00 3D 00 74 00 73 00 30 00 31 00 20 00 00 00 CC  t.a.t.=.t.s.0.1. ...Ì
// |

//sicko    size_t nInputLen = wcslen( szLinkUrl );
//sicko    size_t nMallocLen = ( nInputLen + 1 ) * 6; // 3 * = worst case % expansion?
//sicko    wchar_t * LocalPtr = ( wchar_t * ) MyMalloc( 2706, nMallocLen * sizeof( wchar_t ) );
//sicko    DWORD dwLength = nMallocLen / 2;
//sicko    LocalPtr[0] = NULL;
//sicko    #if DO_DEBUG_CANON
//sicko        ; Spew( L"About to call InternetCanonicalizeUrl( szLinkUrl, LocalPtr, & dwLength, ICU_BROWSER_MODE | ICU_DECODE ) on inputs:" );
//sicko
//sicko        ; SpewValue( L"& szLinkUrl", & szLinkUrl );
//sicko        ; SpewValue( L"wcslen( szLinkUrl )", wcslen( szLinkUrl ) );
//sicko        ; SpewTwo( L"szLinkUrl", szLinkUrl );
//sicko
//sicko        ; SpewValue( L"& LocalPtr", & LocalPtr );
//sicko        ; SpewValue( L"wcslen( LocalPtr )", wcslen( LocalPtr ) );
//sicko        ; SpewTwo( L"LocalPtr", LocalPtr );
//sicko
//sicko        ; SpewValue( L"& dwLength", & dwLength );
//sicko        ; SpewValue( L"dwLength", dwLength );
//sicko
//sicko        ; Spew( L"Going in..." );
//sicko    #endif
//sicko    int res = 0;
//sicko
//sicko    // try-catch not allowed on Win_CE: Use structured exception handling.
//sicko    // Anyway, they didn't catch the heap exception...
//sicko
//sicko#ifndef _WIN32_WCE
//sicko    try
//sicko#endif
//sicko    {
//sicko        res = InternetCanonicalizeUrl( szLinkUrl, LocalPtr, & dwLength, ICU_BROWSER_MODE | ICU_DECODE );
//sicko    }
//sicko#ifndef _WIN32_WCE
//sicko    catch( ... )
//sicko    {
//sicko        #if DO_DEBUG_CANON
//sicko            ; Spew( L"EXCEPTION DURING InternetCanonicalizeUrl" );
//sicko        #endif
//sicko        res = 0;
//sicko    }
//sicko#endif
//sicko
//sicko    #if DO_DEBUG_CANON
//sicko        ; Spew( L"Returned from InternetCanonicalizeUrl on inputs:" );
//sicko        ; SpewValue( L"szLinkUrl, LocalPtr, dwLength", dwLength );
//sicko        ; SpewValue( szLinkUrl, wcslen( szLinkUrl ) );
//sicko        ; SpewValue( LocalPtr, wcslen( LocalPtr ) );
//sicko        ; Spew( L"Done" );
//sicko    #endif
//sicko    if( ! res )
//sicko    {
//sicko        MyFree( 2349, zx, LocalPtr );
//sicko        LocalPtr = NULL;
//sicko        return 0; // failure. Ignore all invalid URLs.
//sicko    }
//sicko
//sicko    // Now my malloc LocalPtr holds can'ized szLinkUrl.
//sicko

    // I'll have to "combine" before "crack" URL.
    // Otherwise relative urls contain no method.

    // Bring malloc out of conditional scope for a final optional free:
    wchar_t * LocalJoin = NULL;

    wchar_t * szContinueWithThisUrl = NULL;

    size_t nBaseLen = 0;
    if( szBaseUrl != NULL )
    {
        nBaseLen = wcslen( szBaseUrl );
    }

    // This 1,2 is because my HTML parser surrounds attr values with space:

    // 1.
    if( szLinkUrl[0] == ' ' )
        szLinkUrl++; // skip one leading space

    size_t nLinkLen = wcslen( szLinkUrl );

    // 2.
    if( nLinkLen > 0
    && szLinkUrl[nLinkLen - 1] == ' ' )
    {
        szLinkUrl[nLinkLen - 1] = NULL; // trim one trailing space
        nLinkLen --;
    }

    size_t nCombined = ( nBaseLen + nLinkLen + 10 ) * 3; // +1/ +1eos; *3 worst case % expansion?

    // double here, in case Windows wants count in bytes.
    // IN FACT YES: Input IS in bytes; output is in chars.
    nCombined *= sizeof( wchar_t );

    // New way: test first for absolute Link Url.
    // test Link URL for http://, https://, or ftp://, then is absolute.
    // I doubt that case has been lowered yet.

    int AbsoluteLinkUrl = 0;

    if( ( szLinkUrl [0] | ' ' ) == L'h'
    &&  ( szLinkUrl [1] | ' ' ) == L't'
    &&  ( szLinkUrl [2] | ' ' ) == L't'
    &&  ( szLinkUrl [3] | ' ' ) == L'p' )
    {
        if( szLinkUrl [4] == L':'
        && szLinkUrl [5] == L'/'
        && szLinkUrl [6] == L'/' )
        {
            AbsoluteLinkUrl = 1;
        }
        else if( ( szLinkUrl [4] | ' ' ) == L's'
        && szLinkUrl [5] == L':'
        && szLinkUrl [6] == L'/'
        && szLinkUrl [7] == L'/' )
        {
            AbsoluteLinkUrl = 1;
        }
    }
    else if( ( szLinkUrl [0] | ' ' ) == L'f'
    && ( szLinkUrl [1] | ' ' ) == L't'
    && ( szLinkUrl [2] | ' ' ) == L'p'
    && szLinkUrl [3] == L':'
    && szLinkUrl [4] == L'/'
    && szLinkUrl [5] == L'/' )
    {
        AbsoluteLinkUrl = 1;
    }

    if( AbsoluteLinkUrl )
    {
        szContinueWithThisUrl = szLinkUrl;
    }
    else
    {
        // See then if we have a suitable method on the Base URL.
        // Exclude especially the case of file://... in base URL.

        if( szBaseUrl == NULL )
        {
            return 0; // No good base URL to fix relative link URL.
        }

        int GoodBaseUrl = 0;

        if( ( szBaseUrl [0] | ' ' ) == L'h'
        &&  ( szBaseUrl [1] | ' ' ) == L't'
        &&  ( szBaseUrl [2] | ' ' ) == L't'
        &&  ( szBaseUrl [3] | ' ' ) == L'p' )
        {
            if( szBaseUrl [4] == L':'
            && szBaseUrl [5] == L'/'
            && szBaseUrl [6] == L'/' )
            {
                GoodBaseUrl = 1;
            }
            else if( ( szBaseUrl [4] | ' ' ) == L's'
            && szBaseUrl [5] == L':'
            && szBaseUrl [6] == L'/'
            && szBaseUrl [7] == L'/' )
            {
                GoodBaseUrl = 1;
            }
        }
        else if( ( szBaseUrl [0] | ' ' ) == L'f'
        && ( szBaseUrl [1] | ' ' ) == L't'
        && ( szBaseUrl [2] | ' ' ) == L'p'
        && szBaseUrl [3] == L':'
        && szBaseUrl [4] == L'/'
        && szBaseUrl [5] == L'/' )
        {
            GoodBaseUrl = 1;
        }

        if( ! GoodBaseUrl )
        {
            return 0; // No good base URL to fix relative link URL.
        }

        // Make a new malloc called LocalJoin while here.
        // Actually, declare outside this scope, to free.

        // double quietly here, in case Windows really thought count was chars.
        LocalJoin = ( wchar_t * ) MyMalloc( 2738, nCombined * sizeof( wchar_t ) );

        // For the record, per HELP -- combineurls's lpdwBufferLength :
        // Long pointer to the size, in bytes, of the lpszBuffer buffer.
        // If the function succeeds, this parameter receives the length,
        // in characters, of the resultant combined URL

        DWORD PassedSize = nCombined;

        if( ! InternetCombineUrl(
            szBaseUrl, // LPCTSTR lpszBaseUrl
            szLinkUrl, // LPCTSTR lpszRelativeUrl
            LocalJoin, // LPTSTR lpszBuffer
            & PassedSize, // LPDWORD lpdwBufferLength
            ICU_BROWSER_MODE | ICU_DECODE ) ) // DWORD dwFlags
        {
            MyFree( 2380, zx, LocalJoin );
            LocalJoin = NULL;
            return 0; // failure. Ignore all invalid URLs.
        }

        szContinueWithThisUrl = LocalJoin;
    }

    // Whether we are now continuing with the Link or the Joined URL,
    // go ahead and canonicalize it, possibly redundant if is Joined.

    // double quietly here, in case Windows really thought count was chars.
    wchar_t * szCanizedPtr = ( wchar_t * ) MyMalloc( 1751, nCombined * sizeof( wchar_t ) );
    DWORD Passed2Can = nCombined;

    if( ! InternetCanonicalizeUrl( szContinueWithThisUrl, szCanizedPtr, & Passed2Can, ICU_BROWSER_MODE | ICU_DECODE ) )
    {
        MyFree( 1705, zx, szCanizedPtr );
        szCanizedPtr = NULL;

        if( LocalJoin != NULL )
        {
            MyFree( 1705, zx, LocalJoin );
            LocalJoin = NULL;
        }

        return 0; // failure. Ignore all invalid URLs.
    }

    URL_COMPONENTS UrlComponents;
    memset( & UrlComponents, 0, sizeof( URL_COMPONENTS ) );
    UrlComponents.dwStructSize = sizeof( URL_COMPONENTS );

    // I'm not sure it matters, but some code may be written to
    // the assumption that URLS will not exceed MAX_MARKUP_TEXT.

    if( wcslen( szCanizedPtr ) >= MAX_MARKUP_TEXT )
    {
        MyFree( 1781, zx, szCanizedPtr );
        szCanizedPtr = NULL;

        if( LocalJoin != NULL )
        {
            MyFree( 1705, zx, LocalJoin );
            LocalJoin = NULL;
        }

        return 0; // failure. Ignore all invalid URLs.
    }

    // Leave pointers NULL and sizes non-zero to get pointers into my URL.
    // I don't know what non-zero to put. Do these actually affect length?
    // I was using full length. Try 1, as a proof that they do not matter.
    UrlComponents.dwSchemeLength = 1; // nCombined;
    UrlComponents.dwHostNameLength = 1; // nCombined;
    UrlComponents.dwUrlPathLength = 1; // nCombined;
    UrlComponents.dwExtraInfoLength = 1; // nCombined;

    if( ! InternetCrackUrl( szCanizedPtr, 0, 0, & UrlComponents ) )
    {
        MyFree( 1781, zx, szCanizedPtr );
        szCanizedPtr = NULL;

        if( LocalJoin != NULL )
        {
            MyFree( 1705, zx, LocalJoin );
            LocalJoin = NULL;
        }

        return 0; // failure. Ignore all invalid URLs.
    }

    // I had excluded FTP here to prevent hanging on fetch. That is overkill,
    // because here, it will also prevent listing all FTP urls at all.
    // With so many new fixes in code above, I will resume allowing FTP...

    if( UrlComponents.nScheme != INTERNET_SCHEME_HTTP
    && UrlComponents.nScheme != INTERNET_SCHEME_FTP
    && UrlComponents.nScheme != INTERNET_SCHEME_HTTPS )
    {
        MyFree( 1781, zx, szCanizedPtr );
        szCanizedPtr = NULL;

        if( LocalJoin != NULL )
        {
            MyFree( 1729, zx, LocalJoin );
            LocalJoin = NULL;
        }

        return 0; // failure. Ignore all invalid URLs.
    }

    if( UrlComponents.lpszExtraInfo != NULL )
    {
        // The extra parts may be a "? query... # fragment...."
        // I do not want to let fragments distinguish same URL.
        // I need to keep query to distinguish, e.g., searches.
        // So, remove fragments, but keep query parts of extra.

        wchar_t * scan = UrlComponents.lpszExtraInfo;

        // Spew( scan );
        // Spew( L"\r\n" );

        for( ;; )
        {
            if( *scan == NULL
            ||  *scan == L'#' )
            {
                // Since Windows help says that ICU_BROWSER_MODE
                // does not remove trailing white space after "?",
                // I will back up over any space characters at end.
                while( scan[-1] == L' ' ) // remove spaces at end of URL
                    scan --; // determinate, because of '?' atop field.
                *scan = NULL; // overwrite any '#' to cancel fragment.
                break;
            }
            scan ++;
        }
    }

    // {
    //     // before humiliation of system error, see the parts...
    //     // Speaking of humiliation, this bugger is too small:
    //     // If this were permanent code, I'd change to strncpy.
    //     wchar_t wk[MAX_MARKUP_TEXT];
    //     wchar_t * into = wk;
    //
    //     if( UrlComponents.lpszScheme != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszScheme, UrlComponents.dwSchemeLength );
    //         into += UrlComponents.dwSchemeLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     if( UrlComponents.lpszHostName != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszHostName, UrlComponents.dwHostNameLength );
    //         into += UrlComponents.dwHostNameLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     if( UrlComponents.lpszUrlPath != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszUrlPath, UrlComponents.dwUrlPathLength );
    //         into += UrlComponents.dwUrlPathLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     if( UrlComponents.lpszExtraInfo != NULL )
    //     {
    //         wcsncpy( into, UrlComponents.lpszExtraInfo, UrlComponents.dwExtraInfoLength );
    //         into += UrlComponents.dwExtraInfoLength;
    //         *into ++ = L'\r';
    //         *into ++ = L'\n';
    //     }
    //     *into ++ = NULL;
    //     // Spew( wk );
    // }

    // double quietly here, in case Windows really thought count was chars.
    wchar_t * SecondPtr = ( wchar_t * ) MyMalloc( 1815, nCombined * sizeof( wchar_t ) );

    DWORD PassedSize = nCombined;
    if( ! InternetCreateUrl( & UrlComponents, 0, SecondPtr, & PassedSize ) )
    {
        MyFree( 2519, zx, SecondPtr );
        SecondPtr = NULL;

        MyFree( 1893, zx, szCanizedPtr );
        szCanizedPtr = NULL;

        if( LocalJoin != NULL )
        {
            MyFree( 1705, zx, LocalJoin );
            LocalJoin = NULL;
        }

        #if DO_DEBUG_FETCH
            ; Spew( L"! InternetCreatUrl" );
        #endif

        return 0; // failure. Ignore all invalid URLs.
    }

    {
        // Solve this Problem, noticed at WASTE url,
        // not the same issue as ICU_BROWSER_MODE:
        //
        // Top URL: http://www.waste.org/mail/?list=pynchon-l&keywords=dickinson
        // Anchor: <a href="?list=pynchon-l&month=9510 &msg=3079 &keywords=dickinson">
        // Joined: http://www.waste.org/mail/?list=pynchon-l&month=9705 &msg=13950 &keywords=dickinson
        // CatOut: http://www.waste.org/mail/?list=pynchon-l&month=9510&msg=3079&keywords=dickinson
        //
        // So run a loop that will concatenate out any spaces after ?.

        wchar_t * from = SecondPtr;
        wchar_t * into = from;
        int SeenQuestionMark = 0;
        for( ;; )
        {
            wchar_t c = *from++;
            *into++ = c;

            if( c == '?' )
                SeenQuestionMark = 1;

            if( c == ' ' // to remove any post-'?' spaces in queries.
            && SeenQuestionMark )
                into--;

            if( c == NULL )
                break; // after having already copied null terminator
        }
    }

    int UrlIndex = 0;
    if( ForForm )
    {
        // In this case for FORM ACTION URLS,
        // remove any query or fragment parts.
        // Also any 'parameter' on the path, often holds a session ID.
        // Just drop a NULL over any '?' or '#' or ';'

        wchar_t * scan = SecondPtr;
        for( ;; )
        {
            if( *scan == NULL )
                break;
            if( *scan == '?'
            ||  *scan == '#'
            ||  *scan == ';' )
            {
                *scan = NULL;
                break;
            }
            scan ++;
        }

        UrlIndex = CSolFormUrls.AddKey( SecondPtr );
        #if DO_DEBUG_ADDFIND
            if( UrlIndex == 1 )
                { Spew( L"AddFind 1 at cwww 1597" ); }
        #endif
        #if DO_DEBUG_FORM1
            ; SpewValue( L"ACTION URL, #", UrlIndex );
        #endif
        #if DO_DEBUG_FORM0
            ; Spew( L"ACTION URL:" );
            ; Spew( SecondPtr );
        #endif
    }
    else
    {
        UrlIndex = CSolAllUrls.AddKey( SecondPtr );
        #if DO_DEBUG_ADDFIND
            if( UrlIndex == 1 )
                { Spew( L"AddFind 1 at cwww 2896" ); }
        #endif
        #if DO_DEBUG_FETCH
            ; SpewValue( L"Hung URL, #", UrlIndex );
            ; Spew( SecondPtr );
        #endif
    }

    if( LocalJoin != NULL )
    {
        MyFree( 1864, zx, LocalJoin );
        LocalJoin = NULL;
    }

    MyFree( 1995, zx, szCanizedPtr );
    szCanizedPtr = NULL;

    MyFree( 2564, zx, SecondPtr );
    SecondPtr = NULL;

    return UrlIndex;
}

void CWww::AddCachePhaseOne( )
{
    #if DO_DEBUG_CALLS
        Routine( L"298" );
    #endif
    // Phase One just iterates the cache,
    // parses the HTTP headers, and lists
    // which items to do during phase two.

    // Vital reading:
    //
    // typedef struct _INTERNET_CACHE_ENTRY_INFO {
    //     DWORD dwStructSize;
    //     LPTSTR lpszSourceUrlName;
    //     LPTSTR lpszLocalFileName;
    //     DWORD CacheEntryType;
    //     DWORD dwUseCount;
    //     DWORD dwHitRate;
    //     DWORD dwSizeLow;
    //     DWORD dwSizeHigh;
    //     FILETIME LastModifiedTime;
    //     FILETIME ExpireTime;
    //     FILETIME LastAccessTime;
    //     FILETIME LastSyncTime;
    //     LPBYTE lpHeaderInfo;
    //     DWORD dwHeaderInfoSize;
    //     LPTSTR lpszFileExtension;
    //     union {
    //         DWORD dwReserved;
    //         DWORD dwExemptDelta;
    //     };
    // } INTERNET_CACHE_ENTRY_INFO, *LPINTERNET_CACHE_ENTRY_INFO;

    m_TotalCacheFiles = 0;  // in AddCachePhaseOne
    m_GoodCacheFiles = 0;   // in AddCachePhaseOne
    m_GoodCacheBytes = 0;   // in AddCachePhaseOne

    LPINTERNET_CACHE_ENTRY_INFO InfoBufferPtr = NULL;
    DWORD InfoBufferSize = 0;
    DWORD InfoMallocSize = 0; // I need to re-init before each call.
    HANDLE hFind = NULL;

    OnlyCache.pWsbResultText->Add( L"Phase 1: Vetting headers, sorting files by size.\r\n\r\n" );

    #if DO_DEBUG_CACHE
        ; Spew( L"AddCachePhaseOne entered" );
    #endif

    for( ;; )
    {
        // first iteration just discovers buffer size.
        // second iteration processes the FIRST item.
        InfoBufferSize = InfoMallocSize;
        hFind = FindFirstUrlCacheEntryEx(
            NULL, // LPCWSTR lpszUrlSearchPattern, // NULL=*.*
            NULL, // DWORD dwFlags,
            NORMAL_CACHE_ENTRY | STICKY_CACHE_ENTRY, // DWORD dwFilter
            NULL, // GROUPID GroupId,
            InfoBufferPtr, // LPINTERNET_CACHE_ENTRY_INFO lpFirstCacheEntryInfo,
            & InfoBufferSize, // LPDWORD lpdwFirstCacheEntryInfoBufferSize,
            NULL, // LPVOID lpGroupAttributes,
            NULL, // LPDWORD pcbGroupAttributes,
            NULL ); // LPVOID lpReserved );

        if( hFind == NULL )
        {
            DWORD gle = GetLastError( );
            if( gle == ERROR_INSUFFICIENT_BUFFER )
            {
                if( InfoBufferPtr != NULL )
                {
                    MyFree( 1401, zx, InfoBufferPtr );
                    InfoBufferPtr = NULL;
                }
                InfoMallocSize = InfoBufferSize + 100; // stop creeping growth
                InfoBufferPtr = ( LPINTERNET_CACHE_ENTRY_INFO ) MyMalloc( 1651, InfoMallocSize );
                continue;
            }
            // For any other errors...
            if( InfoBufferPtr != NULL )
            {
                MyFree( 1411, zx, InfoBufferPtr );
                InfoBufferPtr = NULL;
            }
            if( gle == ERROR_NO_MORE_ITEMS )
            {
                #if DO_DEBUG_CACHE
                    ; Spew( L"AddCachePhaseOne return - no more items" );
                #endif
                return; // up here, no handle to close
            }
            ProgramError( L"FindFirstUrlCacheEntryEx" );
            return;
        }
        else
        {
            LoadInternetCacheItem( InfoBufferPtr ); // never null here
            break; // from first loop; enter second loop
        }
    }

    for( ;; )
    {
        // I think I'd best call *Ex version of this too.
        // A first iteration may re-discover buffer size.
        // First or second iteration processes each item.
        InfoBufferSize = InfoMallocSize;
        if( FindNextUrlCacheEntryEx (
            hFind, // HANDLE hEnumHandle,
            InfoBufferPtr, // LPINTERNET_CACHE_ENTRY_INFO lpNextCacheEntryInfo,
            & InfoBufferSize, // LPDWORD lpdwNextCacheEntryInfoBufferSize
            NULL, // LPVOID lpGroupAttributes,
            NULL, // LPDWORD pcbGroupAttributes,
            NULL ) ) // LPVOID lpReserved
        {
            LoadInternetCacheItem( InfoBufferPtr ); // never null here
        }
        else
        {
            DWORD gle = GetLastError( );
            if( gle == ERROR_INSUFFICIENT_BUFFER )
            {
                if( InfoBufferPtr != NULL )
                {
                    MyFree( 1451, zx, InfoBufferPtr );
                    InfoBufferPtr = NULL;
                }
                InfoMallocSize = InfoBufferSize + 100; // stop creeping growth
                InfoBufferPtr = ( LPINTERNET_CACHE_ENTRY_INFO ) MyMalloc( 1706, InfoMallocSize );
                continue;
            }
            // For any other errors...

            // In second loop must close the find handle.

            if( InfoBufferPtr != NULL )
            {
                MyFree( 1464, zx, InfoBufferPtr );
                InfoBufferPtr = NULL;
            }

            if( gle != ERROR_NO_MORE_ITEMS )
            {
                ProgramError( L"FindNextUrlCacheEntry" );
            }

            if( ! FindCloseUrlCache( hFind ) )
            {
                // On the Pocket PC, this gives an error: handle invalid.
                // However, I would rather suffer error than not call it.
#ifndef _WIN32_WCE
                ProgramError( L"FindCloseUrlCache" );
#endif // not _WIN32_WCE
            }

            // Web buzz says: Always explicitly set FindHandle
            // to INVALID_HANDLE_VALUE after calling FindClose.
            hFind = INVALID_HANDLE_VALUE; // gratuitous

            #if DO_DEBUG_CACHE
                ; Spew( L"AddCachePhaseOne return - no more items" );
            #endif

            return;
        }

        // I notice that AddCachePhaseOne did not have a thread stopper.
        // It runs so fast for me, it doesn't seem necessary, but do it.

        if( OnlyCache.m_StopThisThread )
        {
            #if DO_DEBUG_CACHE
            ; Spew( L"cache phase 1: OnlyCache.m_StopThisThread" );
            #endif
            break;
        }
        if( gbMallocLimitExceeded )
        {
            #if DO_DEBUG_CACHE
            ; Spew( L"cache phase 1: gbMallocLimitExceeded" );
            #endif
            break;
        }
        if( g_bStopAllThreads )
        {
            #if DO_DEBUG_CACHE
            ; Spew( L"cache phase 1: g_bStopAllThreads" );
            #endif
            break;
        }


    }
}

void CWww::LoadInternetCacheItem( LPINTERNET_CACHE_ENTRY_INFO InfoBufferPtr )
{
    #if DO_DEBUG_CALLS
        Routine( L"299" );
    #endif
    // This serves AddCachePhaseOne
    // Despite VC++ 6.0 thinks this member is a ( char * ),
    // InfoBufferPtr->lpszSourceUrlName is a ( TCHAR * ),
    // Otherwise my URL is only 1 letter. Force the cast.

    #if DO_DEBUG_CACHE
        ; Spew( L"LoadInternetCacheItem started" );
    #endif

    // Some stuff in the cache ( help files? ) have no httpheader.

    m_TotalCacheFiles ++;

    if( InfoBufferPtr->lpszSourceUrlName == NULL
    || InfoBufferPtr->lpHeaderInfo == NULL
    || InfoBufferPtr->dwHeaderInfoSize == 0 )
    {
        #if DO_DEBUG_CACHE
            ; Spew( L"LoadInternetCacheItem return - 1,2,3 NULL tests" );
        #endif
        return;
    }

    // Also now I have seen zero length files that I cannot open.

    DWORD ItemSize = InfoBufferPtr->dwSizeLow;
    if( InfoBufferPtr->dwSizeHigh != 0 )
        ItemSize = 0xffffffffL; // This is my max size
    if( ItemSize == 0 )
    {
        #if DO_DEBUG_CACHE
            ; Spew( L"LoadInternetCacheItem return - ItemSize == 0" );
        #endif
        return;
    }

    // Here is my chance to reject .css, .js, .vbs, and others.
    // Get the extension part of the url: First, see some:
    //   127 htm
    //    72 html
    //     7 css
    //     1 xlb
    //     1 vbs
    //     1 py
    //     1 php
    //     1 js
    //     1 cpp
    //     1 aspx

    // lpszFileExtension
    // Pointer to a string that contains the file extension
    // used to retrieve the data as a file. The string occupies
    // the memory area at the end of this structure.

    // Help me obtain file to zero in on other bugs.
    // No help there. it was all NULL. Use the URLs.
    // #if WHAT_CACHE_FILENAME
    //     ; Spew( ( InfoBufferPtr->lpszFileExtension==NULL ) ? L".-null-" : InfoBufferPtr->lpszFileExtension );
    // #endif

    //     LPTSTR lpszLocalFileName;

    // It turns out the same tests should be applied to the URL,
    // and then to the filename, which I guess has intelligence.

    if( IsBinaryExtension( ( wchar_t * ) InfoBufferPtr->lpszLocalFileName ) )
        return;  // infer CONTENT_OTHER per IsBinaryExtension on filename in LoadInternetCacheItem

    if( IsBinaryExtension( ( wchar_t * ) InfoBufferPtr->lpszSourceUrlName ) )
        return;  // infer CONTENT_OTHER per IsBinaryExtension on URL in LoadInternetCacheItem

    // This first phase will have to claim all the URLs it finds,
    // in order to hang a pOnePaper to receive HTTP header parse.

    size_t UrlIndexToClaim = CSolAllUrls.AddKey( ( wchar_t * ) InfoBufferPtr->lpszSourceUrlName );
    #if DO_DEBUG_ADDFIND
        if( UrlIndexToClaim == 1 )
            { Spew( L"AddFind 1 at cwww 1776" ); }
    #endif

    #if DO_DEBUG_CACHE
        ; Spew( ( wchar_t * ) InfoBufferPtr->lpszSourceUrlName );
        {
            wchar_t wk [60];
            wsprintf( wk, L"UrlIndexToClaim: %d", UrlIndexToClaim );
            ; Spew( wk );
        }
    #endif

    // this one is for cache phase one:
    if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_CLAIMING ) )
    {
        #if DO_DEBUG_CACHE
            ; Spew( L"LoadInternetCacheItem return - Cannot CLAIM url" );
        #endif
        return;
    }

    // I successfully claimed this URL as one I am updating.

    // This new COnePaper action is for cache phase one:
    // However, I cannot hang pOnePaper in CSolAllUrls yet,
    // For I must put a PVOID_CLAIMING there until all done.
    // I need this pOnePaper to call the http header parse,
    // but after using it, will save members, and delete it.

    // It seems to me now, that because I am creating pOnePaper
    // just to call ParseHttpHeader, and then deleting pOnePaper,
    // that I should rather pass a structure into ParseHttpHeader
    // to receive its observations. I guess the reason this is
    // not worth cleaning up is: That was the natural way to do
    // URL fetches, and I rarely do Add Cache, so just suffer it.

    COnePaper * pOnePaper = new COnePaper( UrlIndexToClaim ); // this one is temporary, in cache phase 1.
    // Every making of a new COnePaper must create its pWsbAnnotation.
    pOnePaper->pWsbAnnotation = new CWsb;

    // The pointer is to TCHARS. The size is in TCHARS.
    // InfoBufferPtr->lpHeaderInfo
    // InfoBufferPtr->dwHeaderInfoSize
    // this one is for cache phase 1...
    ParseHttpHeader( pOnePaper, UrlIndexToClaim, ( wchar_t * ) InfoBufferPtr->lpHeaderInfo, InfoBufferPtr->dwHeaderInfoSize );

    {
        CWsb WsbCritique; // just one to pass in, then discard.
        int BinaryOkay = 0; // not during cache
        if( ! TestPaperHttpHeaderValuesAcceptable( pOnePaper, & WsbCritique, BinaryOkay ) )
        {
            // This is only interesting when debugging:
            // Not even then always, saying only, e.g.,
            // Not okay: ContentType = non-text
            // Not okay: ContentEncoding != ENCODING_NONE
            // #if DO_DEBUG_CONTENT
            //     OnlyCache.pWsbResultText->AddWsb( & WsbCritique );
            //     // obs: Top.UpdateViewIfOnScreen( & OnlyCache );
            // #endif

            delete pOnePaper;
            pOnePaper = NULL;

            if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_UNDESIRABLE ) )
            {
                ProgramError( L"LoadInternetCacheItem: ! CSolAllUrls.ClaimUserpVoid" );
            }
            #if DO_DEBUG_CACHE
                ; Spew( L"LoadInternetCacheItem return - ! Content Was Not Okay" );
            #endif
            return;
        }
    }

    // Append item ( UrlIndex and filename ) to a CSol for phase 2.

    // Since I am not hanging pOnePaper in CSolAllUrls until sure is good,
    // I need to format http header information into phase 2 control keys.
    // I already had the size, to serve as a resort key, and the URL text.
    // These are the items I may consult during HTM parse or presentation:
    // pOnePaper->HttpHeaderContentType;
    // pOnePaper->HttpHeaderContentLanguage;
    // pOnePaper->HttpHeaderCharset;

    // Various Windows Help info suggests some paths may exceed MAX_PATH,
    // also that MAX_PATH chars might be held in strNcpy non-sz buffers.
    // So rectify anything gotten from os before copying to fixed arrays.
    // In this case, I will take %s out of the wsprintf, and do wcsncpy.
    wchar_t PhaseTwoKey[ MAX_PATH + 50 ]; // fifty is fat for 24 atop, 1 null.
    // Should I also be limiting my internal variables? I think they are okay.

    // The Add Cache had not been working, getting no files, a long time now.
    // You must have had a bad counting day when rephrasing this. 22, not 24:
    wsprintf( PhaseTwoKey, L"%09d %03d %03d %03d ", // is 22 chars + null.
        ItemSize,
        pOnePaper->HttpHeaderContentType,       // format in cache phase1
        pOnePaper->HttpHeaderContentLanguage,   // format in cache phase1
        pOnePaper->HttpHeaderCharset );         // format in cache phase1
    wcsncpy( PhaseTwoKey + 22, InfoBufferPtr->lpszLocalFileName, MAX_PATH ); // format in cache phase1
    PhaseTwoKey [ 22 + MAX_PATH ] = NULL; // Always CYA after using wcsncpy

    #if DO_DEBUG_CONTENT
        ; Spew( PhaseTwoKey );
    #endif

    size_t Index = pSolCacheToDo->AddKey( PhaseTwoKey );
    #if DO_DEBUG_ADDFIND
        if( Index == 1 )
            { Spew( L"AddFind 1 at cwww 1825" ); }
    #endif
    pSolCacheToDo->SetUserValue( Index, UrlIndexToClaim ); // Phase 1 done!

    m_GoodCacheFiles ++;
    m_GoodCacheBytes += ItemSize;

    #if DO_DEBUG_CACHE
        ; Spew( L"LoadInternetCacheItem return - Added Url to list to do" );
    #endif

    // This pOnePaper was only temporary during cache phase 1:
    delete pOnePaper;
    pOnePaper = NULL;
}

void CWww::AddCachePhaseTwo( )
{
    #if DO_DEBUG_CALLS
        Routine( L"300" );
    #endif
    // Phase Two runs through the list from Phase One,
    // opening each file, parsing and hanging sources,
    // adding result log text and indices to OnlyCache.

    #if DO_DEBUG_CACHE
        ; Spew( L"AddCachePhaseTwo started" );
    #endif

    {
        // For byte counts running into the Gigabytes, insert commas:
        // I know some library function might have done this for me.
        wchar_t come[20];
        wchar_t * bkwds = come + 19;
        *bkwds = NULL;
        int n = m_GoodCacheBytes;
        int mod3 = 0;
        for( ;; )
        {
            *--bkwds = '0' + n % 10;
            n /= 10;
            if( n == 0 )
                break;
            if( ++mod3 == 3 )
            {
                mod3 = 0;
                *--bkwds = ',';
            }
        }
        wchar_t wk[100];
        wsprintf( wk, L"The cache holds %d total files, %d good, %s bytes.\r\n\r\n",
            m_TotalCacheFiles,
            m_GoodCacheFiles,
            bkwds );
        OnlyCache.pWsbResultText->Add( wk );
        // obs: Top.UpdateViewIfOnScreen( & OnlyCache );
    }

    OnlyCache.pWsbResultText->Add( L"Phase 2: Adding files.\r\n\r\n" );

    #if DO_DEBUG_CACHE
    ; SpewValue( L"cache phase 2: pSolCacheToDo->nList", pSolCacheToDo->nList );
    #endif

    // Oct 4 2007 - have a bug, that 1. add cache. 2. stop it. 3 add cache,
    // says starting phase 2, and then immediately, thread ended.
    // It appears that I CLAIMED all the urls on first try, and never released:
    // Whether I got to and added file, or broke early, allow a way around it:
    //         try 1--
    // LoadInternetCacheItem return - Added Url to list to do
    //         try 2--
    // LoadInternetCacheItem return - Cannot CLAIM url
    // |
    // Solution:
    // Phase 2, upon abort, must spin out the rest of loops,
    // but simply revert the claimed URLs to NULL pointers.
    // Phase 1 COULD also display the paper summary in case
    // it cannot claim URL, and is because already has it.

    int DoingAbort = 0;

    CoIt * pMalVector = pSolCacheToDo->GetSortedVector( CSOL_BACKWARD );
    if( pMalVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
        {
            #if DO_DEBUG_CACHE
            ; Spew( L"cache phase 2: pCoIt->IsSentinel" );
            #endif
            break;
        }

        size_t UrlIndexToClaim = pCoIt->User.Value;

        #if DO_DEBUG_CACHE
            {
                wchar_t wk [60];
                wsprintf( wk, L"UrlIndexToClaim: %d", UrlIndexToClaim );
                ; Spew( wk );
            }
        #endif

        if( UrlIndexToClaim < 2 )
        {
            ProgramError( L"AddCachePhaseTwo UrlIndexToClaim < 2" );
            break; // error: cease traversal
        }

        // During Phase 1, all listed items were successfully claimed.

        // Phase One successfully claimed this URL as one I am updating.
        // NO: Phase One also hung the paper in AllUrls, and did http parse.
        // This is not claiming, just a test of the value:
        // No, rather, a getting of the pOnePaper pointer.
        // pVoid cannot hold a pointer AND PVOID_CLAIMING!
        // So, Phase 1 cannot do the new COnePaper for me.
        // Therefore, I coded HTTP header data in the key.

        if( CSolAllUrls.GetUserpVoid( UrlIndexToClaim ) != PVOID_CLAIMING )
        {
            ProgramError( L"AddCachePhaseTwo UrlIndexToClaim != PVOID_CLAIMING" );
            break; // error: cease traversal
        }


        if( DoingAbort )
        {
            // This call must work, and finishes my internal tasks.
            if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, NULL ) )
            {
                ProgramError( L"AddCachePhaseTwo: ! CSolAllUrls.ClaimUserpVoid, by NULL" );
            }
        }
        else
        {

            wchar_t * pMalPhaseTwoKey = CoItFullKey( pCoIt );

            // This part gets the source paper from the cache.
            CAsb * pAsbOutput = new CAsb( );

            // Lately, Add Cache stopped finding anything, saying empty/binary.
            // That means pWBuf is null, either from Read or Convert step.
            // That was fixed when I found the +24 instead of +22 in keystring.

            ReadInternetCacheItem( pAsbOutput, pMalPhaseTwoKey );

            #if DO_DEBUG_CACHE
                ; SpewValue( L"pAsbOutput->StrLen", pAsbOutput->StrLen );
            #endif

            COnePaper * pOnePaper = new COnePaper( 0 ); // this one is in cache phase 2
            // Every making of a new COnePaper must create its pWsbAnnotation.
            pOnePaper->pWsbAnnotation = new CWsb;

            // For various reasons, pAsbOutput may be empty.
            size_t nMalSource = 0;
            BYTE * pMalSource = NULL;
            size_t nWBuf = 0;
            wchar_t * pWBuf = NULL;
            if( pAsbOutput->StrLen > 0 )
            {
                pMalSource = pAsbOutput->GetBuffer( & nMalSource );
                // This call is for AddCachePhaseTwo:
                pWBuf = Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection( & nWBuf, pMalSource, nMalSource );

                #if DO_DEBUG_CACHE
                    ; SpewValue( L"nMalSource", nMalSource );
                #endif

                MyFree( 1951, ( nMalSource + 1 ) * sizeof( BYTE ), pMalSource );
                pMalSource = NULL;
            }
            delete pAsbOutput;
            pAsbOutput = NULL;
            // leaving only pWBuf to use, free...

            // PhaseTwoKey that I am receiving is like this:
            // wsprintf( PhaseTwoKey, L"%09d %03d %03d %03d %s",
            //     ItemSize,
            //     pOnePaper->HttpHeaderContentType,
            //     pOnePaper->HttpHeaderContentLanguage,
            //     pOnePaper->HttpHeaderCharset,
            //     InfoBufferPtr->lpszLocalFileName );

            #if DO_DEBUG_CONTENT
                ; Spew( pMalPhaseTwoKey );
            #endif

            // Recover the http header information from phase one:
            {
                // see wsprintf format above
                wchar_t * from = pMalPhaseTwoKey + 10; // after 9d sp
                size_t n = 0;
                n += ( from[0] - '0' ) * 100;
                n += ( from[1] - '0' ) * 10;
                n += ( from[2] - '0' );
                pOnePaper->HttpHeaderContentType = n; // recover in cache phase2

                from = pMalPhaseTwoKey + 14; // after 9d sp 3d sp
                n = 0;
                n += ( from[0] - '0' ) * 100;
                n += ( from[1] - '0' ) * 10;
                n += ( from[2] - '0' );
                pOnePaper->HttpHeaderContentLanguage = n; // recover in cache phase2

                // And a new convenience correlate:
                pOnePaper->LanguageGroup = GroupIndexForLanguageIndex( pOnePaper->HttpHeaderContentLanguage );

                from = pMalPhaseTwoKey + 18; // after 9d sp 3d sp 3d sp
                n = 0;
                n += ( from[0] - '0' ) * 100;
                n += ( from[1] - '0' ) * 10;
                n += ( from[2] - '0' );
                pOnePaper->HttpHeaderCharset = n; // recover in cache phase2
            }

            #if DO_DEBUG_CONTENT
            {
                // Trying to plumb those numbers...
                wchar_t wk[60];
                wsprintf( wk, L"Phase2: Type=%d, Lang=%d, CSet=%d",
                pOnePaper->HttpHeaderContentType,     // debug in cache phase2
                pOnePaper->HttpHeaderContentLanguage, // debug in cache phase2
                pOnePaper->HttpHeaderCharset );       // debug in cache phase2
                ; Spew( wk );
            }
            #endif

            // This part parses, analyzes, the source paper.
            // 1 of 4 similar sequences. This one is for cache phase 2.
            if( pWBuf != NULL )
            {
                int AsText = 0;
                if( pOnePaper->HttpHeaderContentType == CONTENT_PLAINTEXT )
                    AsText = 1;

                #if DO_FORMS_DUMP
                    g_CHtmFoundAForm = 0; // to trigger debugs
                #endif

                // I see no value in revising BASE url for add cache.

                // SED_PILE1
                // from CWww AddCachePhaseTwo
                // This page processing call is for files from Cache
                Pag.ProcessPaper(
                    UrlIndexToClaim,        // size_t UrlIndex
                    pOnePaper,              // COnePaper * pOnePaper
                    pWBuf,                  // wchar_t * pInputBuffer
                    nWBuf,                  // size_t nInputSize
                    AsText,                 // int AsText
                    NO_FRAMES,              // CSol * pSolFrames
                    NO_MORES,               // CSol * pSolMores
                    NO_HITS,                // CSol * pSolHits
                    NULL );                 // COneSurl * pOneSurl
                // SED_PILE2

                #if DO_FORMS_DUMP
                    if( g_CHtmFoundAForm )
                    {
                        // Give me access to that filename to get for study.
                        ; SpewValue( L"CHTM found a form", g_CHtmFoundAForm );
                        if( UrlIndexToClaim != 0 )
                        {
                            wchar_t * pMal = CSolAllUrls.GetFullKey( UrlIndexToClaim );
                            if( pMal != NULL )
                            {
                                ; Spew( pMal ); // cache filename - no, the URL.
                                MyFree( 2223, UNPREDICTABLE, pMal );
                            }
                        }
                        ; Spew( pMalPhaseTwoKey + 22 ); // cache filename
                    }
                #endif

                int ShowAsAQrp = 0; // during cache work
                Pag.SecondPartOfProcessForText( UrlIndexToClaim, ShowAsAQrp, pOnePaper );

                // Now insert all annotations and second newline, atop paper content.
                {
                    size_t nMalNewTop = 0;
                    wchar_t * pMalNewTop = pOnePaper->pWsbAnnotation->GetBuffer( & nMalNewTop );
                    pOnePaper->pWsbResultText->Insert( pMalNewTop, 1 ); // +1 newline
                    int SizeChange = nMalNewTop + 2; // wchars: 1 for CR, 1 for LF
                    pOnePaper->pIdxResultIndex->IncreaseOffsets( SizeChange );
                    pOnePaper->pIdxTextBlocks->IncreaseOffsets( SizeChange );
                    pOnePaper->pIdxSentences->IncreaseOffsets( SizeChange );
                    MyFree( 1995, UNPREDICTABLE, pMalNewTop );
                    pMalNewTop = NULL;
                }

                #if DO_DEBUG_CACHE
                    ; Spew( L"Ready to hang paper" );
                #endif

                // I am about to have CSol swap in my paper pointer for the marker.
                // Before doing that, paper need to contain the valid index number.

                pOnePaper->m_CSolIndex = UrlIndexToClaim;

                // This call must work, and finishes my internal tasks.
                if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, pOnePaper ) )
                {
                    ProgramError( L"AddCachePhaseTwo: ! CSolAllUrls.ClaimUserpVoid" );
                }

                #if DO_DEBUG_CACHE
                    ; Spew( L"Hung paper" );
                #endif

                // Annotate log with the URL and other notes from ProcessPaper.
                // I decided again that I would like to see the cache filename.
                {
                    size_t StartOffset = OnlyCache.pWsbResultText->StrLen;
                    wchar_t * CacheFileName = pMalPhaseTwoKey + 22; // see wsprintf above
                    OnlyCache.pWsbResultText->Add( CacheFileName );
                    OnlyCache.pWsbResultText->Add( L"\r\n" );
                    OnlyCache.pWsbResultText->AddWsb( pOnePaper->pWsbAnnotation );
                    size_t FinalOffset = OnlyCache.pWsbResultText->StrLen;
                    OnlyCache.pIdxResultIndex->AddIdx( StartOffset, FinalOffset, UrlIndexToClaim, 0 );
                    OnlyCache.pWsbResultText->Add( L"\r\n" );
                    // obs: Top.UpdateViewIfOnScreen( & OnlyCache );
                }
                // Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection made a sloppy large buffer.
                MyFree( 2151, UNPREDICTABLE, pWBuf );
            }
            else
            {
                // Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection returned NULL if it determined binary data.
                delete pOnePaper;
                pOnePaper = NULL;

                if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_UNDESIRABLE ) )
                {
                    ProgramError( L"AddCachePhaseTwo: ! CSolAllUrls.ClaimUserpVoid" );
                }

                #if DO_DEBUG_CACHE
                    ; Spew( L"AddCachePhaseTwo - body was empty or binary" );
                #endif
            }

            MyFree( 1581, UNPREDICTABLE, pMalPhaseTwoKey );
            pMalPhaseTwoKey = NULL;

            // While still within the: if( ! DoingAbort ) clause:

            if( OnlyCache.m_StopThisThread )
            {
                #if DO_DEBUG_CACHE
                ; Spew( L"cache phase 2: OnlyCache.m_StopThisThread" );
                #endif
                DoingAbort = 1; // break;
            }
            if( gbMallocLimitExceeded )
            {
                #if DO_DEBUG_CACHE
                ; Spew( L"cache phase 2: gbMallocLimitExceeded" );
                #endif
                DoingAbort = 1; // break;
            }
            if( g_bStopAllThreads )
            {
                #if DO_DEBUG_CACHE
                ; Spew( L"cache phase 2: g_bStopAllThreads" );
                #endif
                DoingAbort = 1; // break;
            }
        }
    }

    MyFree( 1589, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    #if DO_DEBUG_CACHE
        ; Spew( L"AddCachePhaseTwo return" );
    #endif

}

void CWww::ReadInternetCacheItem( CAsb * pAsbOutput, wchar_t * PhaseTwoKey )
{
    #if DO_DEBUG_CALLS
        Routine( L"301" );
    #endif
    // This serves AddCachePhaseTwo. It completes routine just above.

    // It looks like my Add Cache failure is getting an empty filename:
    // <empty line>
    // hFile == INVALID_HANDLE_VALUE
    // pAsbOutput->StrLen: 0


    // PhaseTwoKey is like this:
    // wsprintf( PhaseTwoKey, L"%09d %03d %03d %03d %s",
    //     ItemSize,
    //     pOnePaper->HttpHeaderContentType,
    //     pOnePaper->HttpHeaderContentLanguage,
    //     pOnePaper->HttpHeaderCharset,
    //     InfoBufferPtr->lpszLocalFileName );

    wchar_t * PassedFileName = PhaseTwoKey + 22; // see wsprintf above

    // Only show cache filename when I am debugging
    #if DO_DEBUG_CACHE
        ; Spew( PassedFileName );
        OnlyCache.pWsbResultText->Add( PassedFileName );
        OnlyCache.pWsbResultText->Add( L"\r\n" );
        // obs: Top.UpdateViewIfOnScreen( & OnlyCache );
    #endif

    // Help me obtain file to zero in on other bugs.
    #if WHAT_CACHE_FILENAME
        ; Spew( PassedFileName );
    #endif

    // Just drop the content bytes in the passed pAsbOutput bucket.

    // use memory-mapped file i/o to finish the operation.
    // PassedFileName

    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = INVALID_HANDLE_VALUE;

#ifdef _WIN32_WCE
    // N.B. On the PPC, where CreateFileForMapping replaces CreateFile,
    // as soon as that handle gets passed to CreateFileMapping, whether
    // it pass or fail, that handle is invalid, so do not CloseFile it.
    hFile = CreateFileForMapping( // Lookalikes... READING
        PassedFileName, // LPCTSTR lpFileName,
        GENERIC_READ, // DWORD dwDesiredAccess,
        FILE_SHARE_READ, // DWORD dwShareMode,
        NULL, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        OPEN_EXISTING, // DWORD dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, // DWORD dwFlagsAndAttributes,
        NULL // HANDLE hTemplateFile
   );
#else // not _WIN32_WCE
    hFile = CreateFile( // Lookalikes... READING - in ReadInternetCacheItem
        PassedFileName, // LPCTSTR lpFileName,
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
        // The cache is a slippery place. Ignore failures...
        // ProgramError( L"CreateFile 2" );
        #if DO_DEBUG_CACHE
            ; Spew( L"hFile == INVALID_HANDLE_VALUE" );
        #endif
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
                ProgramError( L"CloseHandle hFile 8" );
        #endif // not _WIN32_WCE
        //naw... ProgramError( L"GetFileSize 1" );
        #if DO_DEBUG_CACHE
            ; Spew( L"dwSize == 0xFFFFFFFF" );
        #endif
        return; // failure
    }
    if ( dwSize == 0 )
    {
        // Silently return and not try to load any zero byte file.
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 9" );
        #endif // not _WIN32_WCE
        #if DO_DEBUG_CACHE
            ; Spew( L"dwSize == 0" );
        #endif
        return; // slight failure
    }

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
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 10" );
        #endif // not _WIN32_WCE
        ProgramError( L"CreateFileMapping2 " );
        #if DO_DEBUG_CACHE
            ; Spew( L"hMap == INVALID_HANDLE_VALUE" );
        #endif
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
                ProgramError( L"CloseHandle hFile 11" );
        #endif // not _WIN32_WCE
        #if DO_DEBUG_CACHE
            ; Spew( L"bBuffer == NULL" );
        #endif
        return; // failure
    }

    // This bBuffer contains dwSize bytes.
    // You cannot write sentinels into it.
    // here, copy out ( bBuffer, dwSize );

    // Just drop the bytes in the passed pAsbOutput bucket.
    pAsbOutput->Addn( bBuffer, dwSize );

    if( ! UnmapViewOfFile( bBuffer ) )
    {
        ProgramError( L"UnmapViewOfFile 2" );
        if( ! CloseHandle( hMap ) )
            ProgramError( L"CloseHandle hMap 13" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 12" );
        #endif // not _WIN32_WCE
        #if DO_DEBUG_CACHE
            ; Spew( L"! UnmapViewOfFile( bBuffer )" );
        #endif
        return; // failure
    }
    if( ! CloseHandle( hMap ) )
    {
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 13" );
        #endif // not _WIN32_WCE
        ProgramError( L"CloseHandle hMap 15" );
        #if DO_DEBUG_CACHE
            ; Spew( L"! CloseHandle( hMap )" );
        #endif
        return; // failure
    }

    // After doing CreateFilemapping, system will do the CloseHandle( hFile )
    // and this call will return false.--Only applies to CE, not the desktop:

    #ifndef _WIN32_WCE
        if( ! CloseHandle( hFile ) ) // Never on PPC
            ProgramError( L"CloseHandle hFile 14" );
    #endif // not _WIN32_WCE

    #if DO_DEBUG_CACHE
        ; Spew( L"RICI: Final return" );
    #endif
    return;
}


void CWww::ObtainOneURLContent( COnePaper * pOnePaper, CAsb * pAsbOutput, size_t ClaimedUrlIndex, CBud * pBudLog, int BinaryOkay )
{
    #if DO_DEBUG_CALLS
        Routine( L"304" );
    #endif

    #if DO_DEBUG_FETCH
        ; SpewValue( L"ObtainOneURLContent was passed ClaimedUrlIndex", ClaimedUrlIndex );
    #endif

    // I had return values, but it allowed schizophrenic code paths.
    // Now, see pOnePaper->HttpHeaderStatus for result of this call.
    //
    // I had to troubleshoot a new "Internet opertion timed out" error:
    // GetterHelper complained of a paper's fetch status NOT ATTEMPTED
    // in the switch downstream from calling me, ObtainOneURLContent.
    // I see the same thing happened before near InternetReadFile and
    // I plugged that error return path, so I need to do that again on
    // some prior internet operation too, within this routine.
    // Since http status should have been set during ParseHttpHeader,
    // ( and assuming no header would lack that line? ) the Inet timeout
    // error and my return must have been prior to doing header parse.
    //
    // Init to 400 atop routine in case of Internet failures,
    // or in case it is possible for header parse to miss it:

    pOnePaper->HttpHeaderStatus = FETCH_STATUS_400_FAILURE;

    // My callers created the new COnePaper before calling me.
    // I will only affect http statuses, no text, in pOnePaper.
    // My callers will finish and hang pOnePaper in CSolAllUrls.

    // Passed pAsbOutput is where I am to deposit whole web page content.

    // Passed ClaimedUrlIndex is into CSolAllUrls.
    // My caller already "claimed" the URL for me.

    // Passed pBudLog is a COneQuery or COneFetch.

    // Verify caller's claim:

    if( CSolAllUrls.GetUserpVoid( ClaimedUrlIndex ) != PVOID_CLAIMING )
    {
        ProgramError( L"ObtainOneURLContent: != PVOID_CLAIMING" );
        return; // leaving 400 failure status
    }

    // Proceed to do the internet fetch operation.

    wchar_t * pMalUrlKey = CSolAllUrls.GetFullKey( ClaimedUrlIndex );

    #if DO_DEBUG_FETCH
        ; Spew( L"ObtainOneURLContent will fetch URL: " );
        ; Spew( pMalUrlKey );
        pBudLog->pWsbResultText->Add( L"ObtainOneURLContent: " );
        pBudLog->pWsbResultText->Add( pMalUrlKey );
        pBudLog->pWsbResultText->Add( L"\r\n" );
        // obs: Top.UpdateViewIfOnScreen( pBudLog );
    #endif

    DWORD dwFlags =
    ( INTERNET_FLAG_IGNORE_CERT_CN_INVALID
    | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
    | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP
    | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
    | INTERNET_FLAG_NO_AUTH
    | INTERNET_FLAG_NO_AUTO_REDIRECT
    | INTERNET_FLAG_NO_UI
    | INTERNET_FLAG_NO_CACHE_WRITE
    | INTERNET_FLAG_TRANSFER_BINARY
    | INTERNET_FLAG_KEEP_CONNECTION
    | INTERNET_FLAG_EXISTING_CONNECT
    | INTERNET_FLAG_NO_COOKIES
   );

    // Surf4me set these lpszHeaders, dwHeadersLength:
    // L"Accept: text/*\r\nUser-Agent: Surf4Me\r\n",
    // sizeof( "Accept: text/*\r\nUser-Agent: Surf4Me\r\n" ) - 1,
    // But WordsEx has been doing great without these.
    // Especially with nontext binary file capability.

    HINTERNET hIRequest = InternetOpenUrl(
        Www.m_hInternet, // HINTERNET hInternet,
        pMalUrlKey, // LPCTSTR lpszUrl,
        NULL, // LPCTSTR lpszHeaders,
        0, // DWORD dwHeadersLength,
        dwFlags, // DWORD dwFlags,
        0 // DWORD_PTR dwContext
   );

    if( hIRequest == NULL )
    {
        // An INTERNET error is not a Program Error.

        #if DO_DEBUG_FETCH
            ; Spew( L"ObtainOneURLContent: InternetOpenUrl: hIRequest == NULL" );
        #endif

        AddInternetErrorToLog( pBudLog, pMalUrlKey, L"InternetOpenUrl" );
        MyFree( 1395, UNPREDICTABLE, pMalUrlKey );
        pMalUrlKey = NULL;
        return; // leaving 400 failure status
    }

    // The internet handle was opened successfully.

    // Obtain the HttpHeader. Parse it.

    size_t nOutBuffer = 0;
    LPVOID lpOutBuffer = NULL;
    DWORD dwIndex = 0;
    DWORD dwSize = 0;
    for( ;; )
    {
        // The first call is to obtain buffer size.

        // The second call should fill that buffer.

        if( HttpQueryInfo(
            hIRequest, // HINTERNET hRequest,
            HTTP_QUERY_RAW_HEADERS_CRLF, // DWORD dwInfoLevel,
            lpOutBuffer, // LPVOID lpBuffer,
            & dwSize, // LPDWORD lpdwBufferLength,
            & dwIndex ) ) // LPDWORD lpdwIndex
        {
            // call outcome: buffer was filled.

            // This is the success path. Parse fetched HTTP header buffer.
            // Windows anomaly: Returns a byte count, but contains wchar_t.
            // ParseHttpHeader sets statuses in pOnePaper, but no text out.

            #if DEBUG_SHOW_HEADER
                Spew( ( wchar_t * ) lpOutBuffer );
            #endif

            size_t nLength = dwSize / sizeof( wchar_t );
            ParseHttpHeader( pOnePaper, ClaimedUrlIndex, ( wchar_t * ) lpOutBuffer, nLength );

            // Make some adjustments to the declared data contents:
            if( IsBinaryExtension( pMalUrlKey ) )
                pOnePaper->HttpHeaderContentType = CONTENT_OTHER; // per URL extension in ObtainOneURLContent


            break; // after second call is successful.
        }
        else
        {
            // call outcome: buffer was not filled.
            // ERROR_INSUFFICIENT_BUFFER is expected on first call.
            // lpOutBuffer == NULL marks this as my first call.
            // dwSize > 0 proves that he asked for some bytes.

            if ( GetLastError( ) == ERROR_INSUFFICIENT_BUFFER
            && lpOutBuffer == NULL
            && dwSize > 0 )
            {
                // Allocate the necessary buffer, size was in bytes.
                // Whatever possessed me to say: new char[ dwSize ]?
                // Change to use the now well-tuned MyMalloc/MyFree.

                // The first time, size included +2 for wide NULL.
                // The second time, size signified count w/o NULL.
                // Remember the original size for my picky MyFree.

                nOutBuffer = dwSize;
                lpOutBuffer = ( char * ) MyMalloc( 1458, nOutBuffer * sizeof( char ) );

                // continue to second iteration, and MyFree below.
            }
            else
            {
                // An INTERNET error is not a Program Error.
                AddInternetErrorToLog( pBudLog, pMalUrlKey, L"HttpQueryInfo" );
                MyFree( 1453, UNPREDICTABLE, pMalUrlKey );
                pMalUrlKey = NULL;
                // Since we do not know if closures will fail too
                // after an error, try close; Do not test result:
                InternetCloseHandle( hIRequest ); // no test result
                hIRequest = NULL;
                if( lpOutBuffer != NULL )
                {
                    MyFree( 1466, nOutBuffer * sizeof( char ), lpOutBuffer );
                    lpOutBuffer = NULL;
                }

                #if DO_DEBUG_FETCH
                    ; Spew( L"ObtainOneURLContent: HttpQueryInfo failed ( not insufficient buffer )" );
                #endif

                return; // leaving 400 failure status
            }
        }
    }

    // Just in case of logic errors above, check to free http header buffer
    if( lpOutBuffer != NULL )
    {
        MyFree( 1474, nOutBuffer * sizeof( char ), lpOutBuffer );
        lpOutBuffer = NULL;
    }

    // Having reached here, we already parsed the http HEADER
    // and set any statuses due to that parse into pOnePaper.

    #if DO_DEBUG_FETCH
        Spew( L"HttpQueryInfo was successful." );
        pBudLog->pWsbResultText->Add( L"HttpQueryInfo was successful.\r\n" );
        // obs: Top.UpdateViewIfOnScreen( pBudLog );
    #endif

    // Determine if we want to fetch this item.
    // Nevertheless, the http connection is on.
    {
        CWsb WsbCritique;

        if( ! TestPaperHttpHeaderValuesAcceptable( pOnePaper, & WsbCritique, BinaryOkay ) )
        {
            #if DO_DEBUG_FETCH
                ; Spew( L"ObtainOneURLContent: ! TestPaperHttpHeaderValuesAcceptable" );
            #endif

            if( pOnePaper->HttpHeaderStatus >= 400 )
            {
                // Here is an interesting case. This URL gives me 4XX status,
                // but when fetched in IE, it saves a file ( full of jscript ).
                // Oh, that may be because my fetch header says accept text.
                // Nevertheless, http header said: Content-Type: text/html
                // http://video.google.com/video/js/688808920-videojs.js
                // No, my fetch header does NOT say accept text.
                // But my fetch header does NOT HTTP 1.1 either. Maybe as 1.0?

                AddRejectionToLog( pBudLog, pMalUrlKey, L"Http status 4XX = bad URL request." );
            }
            else if( pOnePaper->HttpHeaderStatus >= 300 )
            {
                AddRejectionToLog( pBudLog, pMalUrlKey, L"Http status 3XX = redirection." );
            }
            else
            {
                AddRejectionToLog( pBudLog, pMalUrlKey, L"Http status 200, but content type unsuitable." );
            }

            if( ! InternetCloseHandle( hIRequest ) )
                ProgramError( L"InternetCloseHandle hIRequest" );
            hIRequest = NULL;

            MyFree( 1511, UNPREDICTABLE, pMalUrlKey );
            pMalUrlKey = NULL;

            return; // leaving one of 400, 300, 250 header failure statuses
        }
    }

    // Proceed to do some internet reads on opened handle.
    // Later, add a timer that can put 15 sec, 30 sec, etc.
    // Later, might assess quality or rate, and stop fetch.

    // Let's optimize the buffering by asking system's size.
    // I don't know if this is constant of system or of URL.
    DWORD OptimumSize = 4096;
    {
        // Avoid refering to confusing option buffer/length names.
        DWORD dwBuffer = -1;
        DWORD dwBufferLength = sizeof( dwBuffer );
        // INTERNET_OPTION_READ_BUFFER_SIZE = 4096 on Desktop; Also on PPC.
        if( ! InternetQueryOption( hIRequest, INTERNET_OPTION_READ_BUFFER_SIZE, & dwBuffer, & dwBufferLength ) )
        {
            // In case this can ever happen benignly, do not make a program error.
            // ProgramError( L"InternetQueryOption INTERNET_OPTION_READ_BUFFER_SIZE" );

            #if DO_DEBUG_FETCH
                ; Spew( L"ObtainOneURLContent: ! InternetQueryOption" );
            #endif

            // An INTERNET error is not a Program Error.
            AddInternetErrorToLog( pBudLog, pMalUrlKey, L"InternetQueryOption INTERNET_OPTION_READ_BUFFER_SIZE" );
            MyFree( 1533, UNPREDICTABLE, pMalUrlKey );
            pMalUrlKey = NULL;
            // Since we do not know if closures will fail too
            // after an error, try close; Do not test result:
            InternetCloseHandle( hIRequest ); // no test result
            hIRequest = NULL;

            pOnePaper->HttpHeaderStatus = FETCH_STATUS_400_FAILURE;
            return; // having changed pOnePaper to 400 failure status
        }
        else
        {
            OptimumSize = dwBuffer; // copy out - rename
        }
    }

    BYTE * ReadBuffer = ( BYTE * ) MyMalloc( 1475, OptimumSize * sizeof( BYTE ) );

    // BOOL InternetReadFile(
    //     HINTERNET hFile,
    //     LPVOID lpBuffer,
    //     DWORD dwNumberOfBytesToRead,
    //     LPDWORD lpdwNumberOfBytesRead
    // );
    //
    // hFile
    // [in] Valid HINTERNET handle returned from a previous call
    // to InternetOpenUrl, FtpOpenFile, GopherOpenFile, or HttpOpenRequest.
    //
    // lpBuffer
    // [in] Pointer to a buffer that receives the data to read.
    //
    // dwNumberOfBytesToRead
    // [in] Unsigned long integer value that contains the number of bytes to read.
    //
    // lpdwNumberOfBytesRead
    // [out] Pointer to an unsigned long integer variable that receives
    // the number of bytes read. InternetReadFile sets this value to zero
    // before doing any work or error checking.
    //
    // Returns TRUE if successful, or FALSE otherwise.

    // DEC 11 2007:
    // Here is one of those problem URLs.
    // After the last read shown below,
    // WordsEx is hung, awaiting a read.
    //
    // ObtainOneURLContent will fetch URL:
    // http://www.thinklocal.com/searchw2.asp?q=trumpets+thunders+lightnings+voices
    // HttpQueryInfo was successful.
    // Begin Reading.
    // Read  4016 bytes.
    // Read  2992 bytes.
    // Read  4096 bytes.
    // Read  3204 bytes.
    // Read  1460 bytes.
    // Read  4096 bytes.
    // Read  3204 bytes.
    // Read  4096 bytes.
    // Read  3760 bytes.
    // Read   904 bytes.
    // Read  4096 bytes.
    // Read  3204 bytes.
    // Read  4096 bytes.
    // Read  2261 bytes.
    //
    //
    // Without using asynchronous mode,
    // maybe I should use expected byte count
    // to stop looping on the read operation.
    //
    // There was no size available in the header:
    //
    // HTTP/1.1 200 OK
    // Via: 1.1 MRP-ISA
    // Connection:
    // Keep-Alive
    // Proxy-Connection:
    // Keep-Alive
    // Transfer-Encoding: chunked
    // Date:
    // Tue, 11 Dec 2007 13:47:58 GMT
    // Content-Type:
    // text/html
    // Server:
    // Microsoft-IIS/6.0
    // X-Powered-By:
    // ASP.NET
    // X-AspNet-Version:
    // 2.0.50727
    // Set-Cookie:
    // ASP.NET_SessionId=ukoupr450nxlea55k3xkde45; path=/;
    // HttpOnly
    // Cache-Control: private

    // Perhaps my initial setting of this timeout was too early.
    // Set it again, no, on this final handle to use in reading.
    // That is, on hIRequest, not m_hInternet.
    // No, this did not change the hung symptom
    //
    // DWORD dwBuffer = 30000;
    // DWORD dwBufferLength = sizeof( dwBuffer );
    // if( ! InternetSetOption( hIRequest, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, & dwBuffer, dwBufferLength ) )
    //     ProgramError( L"InternetSetOption 15" );

    // Problem in limbo:
    //
    // http://www.thinklocal.com/searchw2.asp?q=trumpets+thunders+lightnings+voices
    // HUNG symptom was noticed with Win2K VC 6 build.
    // What about trying it on the vista VC.net build?
    // AHA! FETCH COMPLETED OKAY. JUST USE vc.net build.
    // What about that build on my Win2000pro at work?
    // Damn! That URL still hangs up after last read!
    // Perhaps there is an IE upgrade I could download,
    // and then add to usage text comment about hangup.


    #if DO_DEBUG_FETCH
        Spew( L"Begin Reading." );
        pBudLog->pWsbResultText->Add( L"Begin Reading.\r\n" );
        // obs: Top.UpdateViewIfOnScreen( pBudLog );
    #endif

    for( ;; )
    {
        // I fetched a big page, and stopped threads,
        // and got error below: #6 = handle invalid.
        // So, I will not stop the reading loop here.

        // When I had 30 fetch threads running, one of them
        // stopped on InternetReadFile, with the status msg:
        // "The operation timed out". Per Windows Help, this
        // may occur after 5 seconds waiting on server data.
        // Do not let internet errors become program errors.
        // That error ( #12002 ) was handled okay by WordsEx's
        // GetLastError and FormatMessage in WordsEx.cpp.
        // Make a special version of those to annotate log.

        DWORD dwNumberOfBytesRead;

        if( ! InternetReadFile( hIRequest, ReadBuffer,
            OptimumSize, & dwNumberOfBytesRead ) )
        {
            // An INTERNET error is not a Program Error.

            #if DO_DEBUG_FETCH
                ; Spew( L"ObtainOneURLContent: ! InternetReadFile" );
            #endif

            AddInternetErrorToLog( pBudLog, pMalUrlKey, L"InternetReadFile" );
            MyFree( 1601, UNPREDICTABLE, pMalUrlKey );
            pMalUrlKey = NULL;
            // Since we do not know if closures will fail too
            // after an error, try close; Do not test result:
            InternetCloseHandle( hIRequest ); // no test result
            hIRequest = NULL;
            MyFree( 1607, OptimumSize * sizeof( BYTE ), ReadBuffer );
            ReadBuffer = NULL;
            pOnePaper->HttpHeaderStatus = FETCH_STATUS_400_FAILURE;
            return; // having changed pOnePaper to 400 failure status
        }

        if( dwNumberOfBytesRead > 0 )
        {
            // Some more data has been deposited.
            #if DO_DEBUG_FETCH
                wchar_t wk[ 40 ];
                wsprintf( wk, L"Read %5d bytes.", dwNumberOfBytesRead );
                pBudLog->pWsbResultText->Add( wk );
                pBudLog->pWsbResultText->Add( L"\r\n" );
                // obs: Top.UpdateViewIfOnScreen( pBudLog );
                ; Spew( wk );
            #endif

            // Store content into Caller's CAsb.
            pAsbOutput->Addn( ReadBuffer, dwNumberOfBytesRead );
        }

        // I may be wrong about ( dwNumberOfBytesRead < OptimumSize )
        // as eof criterion. I am only getting partial page downloads.
        // Try testing instead for a zero byte result. Yes, good now!

        if( dwNumberOfBytesRead == 0 )
        {
            // Read has no early return options.
            // This must be the end of the data.
            break;
        }
    }

    // I am finished doing internet read operations.
    // Close the opened handle to this URL request.

    #if DO_DEBUG_FETCH
        Spew( L"Finished Reading." );
        pBudLog->pWsbResultText->Add( L"Finished Reading.\r\n\r\n" );
        // obs: Top.UpdateViewIfOnScreen( pBudLog );
    #endif

    MyFree( 1607, OptimumSize * sizeof( BYTE ), ReadBuffer );
    ReadBuffer = NULL;

    // I fetched a big page, and stopped threads,
    // and got error here: #6 = handle invalid.
    // So, I do not stop the reading loop above.
    // Also, do not let a mere failure to close
    // a handle bring down the whole program.

    if( ! InternetCloseHandle( hIRequest ) )
    {
        #if DO_DEBUG_FETCH
            ; Spew( L"ObtainOneURLContent: ! InternetCloseHandle" );
        #endif

        // An INTERNET error is not a Program Error.
        AddInternetErrorToLog( pBudLog, pMalUrlKey, L"InternetCloseHandle" );
        // Fall out into the 200 path... return FETCH_STATUS_400_FAILURE;
        pOnePaper->HttpHeaderStatus = FETCH_STATUS_400_FAILURE;
        return; // having changed pOnePaper to 400 failure status
    }

    MyFree( 1660, UNPREDICTABLE, pMalUrlKey );
    pMalUrlKey = NULL;

    #if DO_DEBUG_FETCH
        ; Spew( L"ObtainOneURLContent: Successful completion" );
    #endif

    hIRequest = NULL; // gratuitous
    return; // having changed pOnePaper to 200 success status
}

void CWww::AddRejectionToLog( CBud * pBudLog, wchar_t * szUrl, wchar_t * szReason )
{
    #if DO_DEBUG_CALLS
        Routine( L"305" );
    #endif

    // As a conenience to many callers, I will add the URL, newline,
    // before the reason message, and then two newlines to pBudLog.
    // Otherwise, the URL attempted loses visibility in result log.

    // During DO_DEBUG_FETCH, some calls to me are not for rejection.

    pBudLog->pWsbResultText->Add( szUrl );
    pBudLog->pWsbResultText->Add( L"\r\n" );
    pBudLog->pWsbResultText->Add( szReason );
    pBudLog->pWsbResultText->Add( L"\r\n\r\n" );

    #if DO_DEBUG_FETCH
        ; Spew( szUrl );
        ; Spew( szReason );
    #endif

}

void CWww::AddInternetErrorToLog( CBud * pBudLog, wchar_t * szUrl, wchar_t * szOperation )
{
    #if DO_DEBUG_CALLS
        Routine( L"306" );
    #endif
    // As a conenience to many callers, I will add the URL, newline,
    // before the error message, and then two newlines to pBudLog.
    // Otherwise, the URL attempted loses visibility in the log.

    DWORD dwLastError = GetLastError( );

    // Do it simply, ignoring internet "extended" errors.
    HMODULE hModule = NULL; // default to system source
    DWORD dwBufferLength = 0;
    DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM;

    // In the case of WinInet numbers, point to that source.
    if( dwLastError >= 12001
    && dwLastError <= 12156 ) // probably higher - find a symbol
    {
        hModule = g_hWinInet;
        if( hModule != NULL )
            dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
    }

    wchar_t * MessageBuffer = NULL; // receives the LocalAlloc.

    dwBufferLength = FormatMessage(
        dwFormatFlags,
        hModule,
        dwLastError,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
        ( wchar_t * ) & MessageBuffer, // type was mis-cast on purpose
        0,
        NULL );

    if( dwBufferLength == 0 )
    {
        // Handle the case that FormatMessage failed.
        // I saw an unknown case of 12057 on vista.

        pBudLog->pWsbResultText->Add( szUrl );
        pBudLog->pWsbResultText->Add( L"\r\n" );
        pBudLog->pWsbResultText->Add( szOperation );
        pBudLog->pWsbResultText->Add( L": " );
        wchar_t wk[80];
        wsprintf( wk, L"Error No. %d", dwLastError );
        pBudLog->pWsbResultText->Add( wk );
        pBudLog->pWsbResultText->Add( L"\r\n\r\n" );
    }
    else
    {
        // Normal case after FormatMessage succeeded.

        pBudLog->pWsbResultText->Add( szUrl );
        pBudLog->pWsbResultText->Add( L"\r\n" );
        pBudLog->pWsbResultText->Add( szOperation );
        pBudLog->pWsbResultText->Add( L": " );
        pBudLog->pWsbResultText->Add( MessageBuffer );
        pBudLog->pWsbResultText->Add( L"\r\n\r\n" );

    }
    if( MessageBuffer != NULL )
        LocalFree( MessageBuffer );
    MessageBuffer = NULL;
}


void CWww::RunOneFetchThread( size_t FetchIndex )
{
    #if DO_DEBUG_CALLS
        Routine( L"303" );
    #endif

    // New: Add Links is a synonym of the FetchDlgProc work.
    // So Add Links and Add Web Page are both serviced here.

    // RunOneFetchThread is servicing just one of possibly
    // many concurrent threads going in the same Cwww class.
    // I must pass my thread-controlling pFetch pointer into
    // any helper subroutines, and use it to access log, etc.
    // In fact, I must prefix pFetch to anything used myself.

    // ( RunFindThread had it easy, because it's class creator
    // received the parameters and held them as its members; )

    // Therefore, RunOneFetchThread has received a FetchIndex.
    // FetchIndex is into CSolUserFetchs ( not CSolAllUrls ).

    // The Key in CSolUserFetchs is a well-canonicalized URL
    // from FixUpUserUrl, which the FetchDlgProc has hung as
    // the Key in CSolUserFetchs, but not in CSolAllUrls.

    // That Key in CSolUserFetchs finds a pVoid which points
    // to a COneFetch * pFetch instance, to control my work.

    // FetchDlgProc has init these members of the COneFetch:
    // pFetch->m_FetchAllLinks = g_FetchAllLinks;

    // FetchDlgProc also added that new Fetch to the display.


    // ===================================================
    // This very routine is tasked with figuring out how to
    // choose new URLs to fetch, according to user settings.
    //
    // Even the plan I had set up is way too complicated.
    // New plan. Allow:
    // 1. to fetch a single URL.
    // 2. to fetch a single URL and all links from it.
    // testing pFetch->m_FetchAllLinks = g_FetchAllLinks;
    //
    // Actually, now it is simple: While user is reading a
    // web page of interest, he can do Add Links, and I'll
    // fetch all the anchors from that web page, tout suite.
    // ===================================================


    // Previous work could not re-examine a URL already done.
    // So when re-fetching old redirect URLS, it had to stop.
    // Therefore, I have a global CSolRedirects( CSOL_SCALAR ),
    // Wherein the key will be atoi of some original URLIndex
    // in CSolAllUrls and User.Value its redirected URLIndex.

    // Some new helper routine will, for every URL to fetch,
    // make a new COnePaper, call ObtainOneURLContent, then
    // call ProcessPaper, then finish hanging the new paper.

    // Such helper should be able to assist OneQuery later.
    // Therefore, it should remain at arm's length from me.
    // It should be given a UrlIndex, and settings such as
    // whether to accept all, or texts only in http header,
    // and a binary path, and a CSol to list new URLIndices.

    // What else might it need for queries? Return a what?
    // It needs a pBudLog, base class of pFetch or pQuery.


    // Get my thread's own pFetch hanging in CSolUserFetchs.
    COneFetch * pFetch = ( COneFetch * ) CSolUserFetchs.GetUserpVoid( FetchIndex );
    if( pFetch == NULL )
    {
        ProgramError( L"RunOneFetchThread: pFetch == NULL" );
        return;
    }

    #if DO_DEBUG_FETCH
        ; SpewValue( L"Starting on FetchIndex", FetchIndex );
    #endif

    // Get my thread's starting URL Key string.
    // Get UrlIndex for starting URL Key string.
    size_t UrlIndex = NULL;
    {
        wchar_t * pMalUrl = CSolUserFetchs.GetFullKey( FetchIndex );
        if( pMalUrl == NULL )
        {
            ProgramError( L"RunOneFetchThread: pMalUrl == NULL" );
            return;
        }

        #if DO_DEBUG_FETCH
            ; Spew( pMalUrl );
        #endif

        UrlIndex = CSolAllUrls.AddKey( pMalUrl );
        #if DO_DEBUG_ADDFIND
            if( UrlIndex == 1 )
                { Spew( L"AddFind 1 at cwww 1088" ); }
        #endif
        MyFree( 2595, zx, pMalUrl );

        #if DO_DEBUG_FETCH
            ; SpewValue( L"Starting on UrlIndex", UrlIndex );
        #endif

    }

    pFetch->pWsbResultText->Add( L"Thread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFetch );

    // I should like to organize this as a simple loop here,
    // not as spaghetti linear sequences of similar actions.

    // For now, just do one URL well:
    // Oh, that worked out very good.

    // CommonGetter automatically follows redirections.
    // CommonGetter automatically saves non-text files.

    // As it stands, RunFet receives back a final URL after redirections,
    // which is the only web page that it then processes to follow links.
    //
    // However, my recursive frame experiments shows that such are valid.
    // Now, RunFet must pass a pSolFrames down to receive a list of URLs.
    //
    // RunFet Step 1:
    //     RunFet calls CmnGet once for top url.
    //     CmnGet loops to follow a single sequence of redirections.
    //     GetHpr calls the CHtm parse, receives pSolFrames to pass up.
    //
    // RunFet Step 2:
    //     RunFet calls CmnGet while iterating URLs of final first page.
    //     Again, CmnGet loops to follows any of their redirections.
    //     GetHpr calls the CHtm parse, receives pSolFrames to pass up.
    //
    // I want:
    //     1. All framesets of the Step 1 top url.
    //     2. All framesets of the Step 2 linked urls.
    //     3. Recursive framesets if that concept is applicable. ( study )
    //
    // I don't want:
    //     1. Self-stimulating recursion in any routine. ( unavoidable? )
    //     2. Loop to do frames lower than one url/idx pBudLog annotation.
    //     3. I probably will not want to do framesets during query work.
    //
    // So:
    //     Put a new routine above CmnGet, which creates a new pSolFrame
    //     on the stack and calls CmnGet, then -Calls itself recursivly-
    //     to iterate over any frames found. It will return to RunFet a
    //     list of all successful UrlIndexes fetched, including top one.
    //
    // This is FANTASTIC! According to spews, everything went okay.
    // but the log page does not show the fetched urls... See why.
    //

    #if DO_DEBUG_BINARIES
        ; Spew( L"RunOneFetchThread...pFetch->m_NonTextPath:" );
        ; Spew( ( pFetch->m_NonTextPath == NULL ) ? L"-null-" : pFetch->m_NonTextPath );
    #endif

    #if DO_DEBUG_FRAME
        ; SpewValue( L"CWww: RunOneFetchThread: Does Step 1 UrlIndex", UrlIndex );
    #endif

    CSol * pSolFrames = new CSol( CSOL_SCALAR );
    // SED_PILE1
    // from CWww RunOneFetchThread
    // This call is for some very top URL that a user typed in, or clicked:
    RecursiveGetter(
        ZEROETH,                    // int Recursions
        UrlIndex,                   // size_t UrlIndex
        pFetch,                     // CBud * pBudLog
        pFetch->m_NonTextPath,      // wchar_t * szBinaryPath
        pSolFrames,                 // CSol * pSolFrames
        NO_MORES,                   // CSol * pSolMores
        NO_HITS,                    // CSol * pSolHits
        ZERO_ORDINAL_FOR_NON_QRP,   // int FromEngineNo
        ZERO_ORDINAL_FOR_NON_QRP,   // int PageIsAQrp
        NO_QRURL,                   // wchar_t * szQueryResultUrl
        YES_TOP,                    // int bTopUrl
        NULL );                     // COneSurl * pOneSurl
    // SED_PILE2

    #if DO_DEBUG_FRAME
        ; SpewValue( L"CWww: RunOneFetchThread: No. of frames after Step 1", pSolFrames->nList - 2 );
    #endif

    // Now, if they want all links, All that I have to do
    // is iterate page's links IDX, calling RecursiveGetter.
    // What paper? I never touched a paper. Get one now.

    if( pFetch->m_FetchAllLinks
    && pSolFrames->nList - 2 > 0 ) // -2 for head,tail
    {
        #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
            ; SpewValue( L"About to iterate the anchors of top+frames", pSolFrames->nList - 2 );
        #endif

        // This is the outer loop to iterate urls successfully fetched.
        CoIt * pMalVector = pSolFrames->GetSortedVector( CSOL_FORWARD );
        if( pMalVector != NULL )
        {
            size_t take = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector + take++;
                if( pCoIt->IsSentinel )
                    break;

                // One UrlIndex is held as itoa in the pCoIt's key value.

                wchar_t * FullKey = CoItFullKey( pCoIt );
                UrlIndex = _wtoi( FullKey );
                MyFree( 1388, zx, FullKey );
                FullKey = NULL;
                #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
                    ; SpewValue( L"CWww: RunOneFetchThread: Does Step 2 UrlIndex", UrlIndex );
                #endif

                COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
                // This is just a CYA test. For the paper was already fetched.
                if( pOnePaper != NULL
                &&  pOnePaper < PVOID_VALID_BELOW )
                {
                    {
                        // I only need the full key to annotate log
                        wchar_t * pMalUrlKey = CSolAllUrls.GetFullKey( UrlIndex );
                        if( pMalUrlKey == NULL )
                        {
                            ProgramError( L"RunOneFetchThread: pMalUrlKey == NULL" );
                            return; // error
                        }
                        pFetch->pWsbResultText->Add( L"Fetching all links from " );
                        pFetch->pWsbResultText->Add( pMalUrlKey );
                        pFetch->pWsbResultText->Add( L"\r\n\r\n" );
                        // obs: Top.UpdateViewIfOnScreen( pFetch );
                        MyFree( 2595, UNPREDICTABLE, pMalUrlKey );
                        pMalUrlKey = NULL;
                    }

                    // Now that papers have a CIdx of cleartext URLs,
                    // I will double down this piece of looping code:

                    // This is the inner loop to iterate anchors within one page:
                    int i = 0;
                    for( ;; )
                    {
                        size_t nSourceIndex;
                        size_t nAtopText;
                        size_t nPastText;

                        if( pOnePaper->pIdxResultIndex->ReturnIdxSlotRange( i, & nSourceIndex, & nAtopText, & nPastText ) )
                        {
                            // That successful result means the four passed variables were filled.
                            // Four? Fourth one is offset in target URL. Not used by Slot callers.

                            #if DO_DEBUG_LINKIDX
                            {
                                // While here, let me debug the list of IDX. Pretty.
                                wchar_t wk[80];
                                wsprintf( wk, L"FOLLOW LINK in text from %7d to %7d -> URL #%7d.",
                                    nAtopText, nPastText, nSourceIndex );
                                ; Spew( wk );
                                pFetch->pWsbResultText->Add( wk );
                                pFetch->pWsbResultText->Add( L"\r\n" );
                                // obs: Top.UpdateViewIfOnScreen( pFetch );
                            }
                            #endif

                            // This is all I have to do to hook it up!
                            CSol * pSolIgnore = new CSol( CSOL_SCALAR );
                            // SED_PILE1
                            // from CWww RunOneFetchThread
                            // This call is fetching ALL LINKS found immediately under some top url user typed in or clicked.
                            RecursiveGetter(
                                ZEROETH,                    // int Recursions
                                nSourceIndex,               // size_t UrlIndex
                                pFetch,                     // CBud * pBudLog
                                pFetch->m_NonTextPath,      // wchar_t * szBinaryPath
                                pSolIgnore,                 // CSol * pSolFrames
                                NO_MORES,                   // CSol * pSolMores
                                NO_HITS,                    // CSol * pSolHits
                                ZERO_ORDINAL_FOR_NON_QRP,   // int FromEngineNo
                                ZERO_ORDINAL_FOR_NON_QRP,   // int PageIsAQrp
                                NO_QRURL,                   // wchar_t * szQueryResultUrl
                                NOT_TOP,                    // int bTopUrl
                                NULL );                     // COneSurl * pOneSurl
                            // SED_PILE2
                            delete pSolIgnore;
                            pSolIgnore = NULL;

                            // Oh, and thread-stop bools.

                            int stopping = 0; // hold this thought until present facts

                            if( pFetch->m_StopThisThread )
                            {
                                #if DO_DEBUG_FETCH
                                    ; Spew( L"RunOneFetchThread: pFetch->m_StopThisThread" );
                                #endif
                                stopping = 1;
                            }
                            if( gbMallocLimitExceeded )
                            {
                                #if DO_DEBUG_CACHE
                                ; Spew( L"RunOneFetchThread: gbMallocLimitExceeded" );
                                #endif
                                stopping = 1;
                            }
                            if( g_bStopAllThreads )
                            {
                                #if DO_DEBUG_FETCH
                                    ; Spew( L"RunOneFetchThread: g_bStopAllThreads" );
                                #endif
                                stopping = 1;
                            }

                            if( stopping )
                                break;
                        }
                        else
                        {
                            break;
                        }
                        i ++;
                    }

                    // Check before run the second copy of such a loop:

                    if ( g_bStopAllThreads
                    || gbMallocLimitExceeded
                    || pFetch->m_StopThisThread )
                        break;

                    // This second inner loop iterates cleartext urls.
                    if( pOnePaper->pIdxClearTextUrls != NULL )
                    {

                        int i = 0;
                        for( ;; )
                        {
                            size_t nSourceIndex;
                            size_t nAtopText;
                            size_t nPastText;

                            if( pOnePaper->pIdxClearTextUrls->ReturnIdxSlotRange( i, & nSourceIndex, & nAtopText, & nPastText ) )
                            {
                                // That successful result means the four passed variables were filled.
                                // Four? Fourth one is offset in target URL. Not used by Slot callers.

                                #if DO_DEBUG_LINKIDX
                                {
                                    // While here, let me debug the list of IDX. Pretty.
                                    wchar_t wk[80];
                                    wsprintf( wk, L"FOLLOW LINK in text from %7d to %7d -> URL #%7d.",
                                        nAtopText, nPastText, nSourceIndex );
                                    ; Spew( wk );
                                    pFetch->pWsbResultText->Add( wk );
                                    pFetch->pWsbResultText->Add( L"\r\n" );
                                    // obs: Top.UpdateViewIfOnScreen( pFetch );
                                }
                                #endif

                                // This is all I have to do to hook it up!
                                CSol * pSolIgnore = new CSol( CSOL_SCALAR );
                                // SED_PILE1
                                // from CWww RunOneFetchThread
                                // This call is fetching ALL CLEARTEXT LINKS found immediately under some top url user typed in or clicked.
                                RecursiveGetter(
                                    ZEROETH,                    // int Recursions
                                    nSourceIndex,               // size_t UrlIndex
                                    pFetch,                     // CBud * pBudLog
                                    pFetch->m_NonTextPath,      // wchar_t * szBinaryPath
                                    pSolIgnore,                 // CSol * pSolFrames
                                    NO_MORES,                   // CSol * pSolMores
                                    NO_HITS,                    // CSol * pSolHits
                                    ZERO_ORDINAL_FOR_NON_QRP,   // int FromEngineNo
                                    ZERO_ORDINAL_FOR_NON_QRP,   // int PageIsAQrp
                                    NO_QRURL,                   // wchar_t * szQueryResultUrl
                                    NOT_TOP,                    // int bTopUrl
                                    NULL );                     // COneSurl * pOneSurl
                                // SED_PILE2
                                delete pSolIgnore;
                                pSolIgnore = NULL;

                                // Oh, and thread-stop bools.

                                int stopping = 0; // hold this thought until present facts

                                if( pFetch->m_StopThisThread )
                                {
                                    #if DO_DEBUG_FETCH
                                        ; Spew( L"RunOneFetchThread: pFetch->m_StopThisThread" );
                                    #endif
                                    stopping = 1;
                                }
                                if( gbMallocLimitExceeded )
                                {
                                    #if DO_DEBUG_FETCH
                                        ; Spew( L"RunOneFetchThread: gbMallocLimitExceeded" );
                                    #endif
                                    stopping = 1;
                                }
                                if( g_bStopAllThreads )
                                {
                                    #if DO_DEBUG_FETCH
                                        ; Spew( L"RunOneFetchThread: g_bStopAllThreads" );
                                    #endif
                                    stopping = 1;
                                }

                                if( stopping )
                                    break;
                            }
                            else
                            {
                                break;
                            }
                            i ++;
                        }
                    }

                }

                if ( g_bStopAllThreads
                || gbMallocLimitExceeded
                || pFetch->m_StopThisThread )
                    break;
            }

            MyFree( 1395, UNPREDICTABLE, pMalVector );
            pMalVector = NULL;
        }
    }

    // On the other hand, if they DONT want to follow links,
    // and the fetching only returned one single top paper,
    // then put that new paper in the ring as current view.

    if( ! pFetch->m_FetchAllLinks
    && pSolFrames->nList - 2 == 1 ) // -2 for head,tail
    {
        // If exactly 1 in Csol, it is at index [0=head] [1=tail], 2.
        // One UrlIndex is held as itoa in the pCoIt's key value.

        wchar_t * FullKey = pSolFrames->GetFullKey( 2 );
        UrlIndex = _wtoi( FullKey );
        MyFree( 1388, zx, FullKey );
        FullKey = NULL;
        #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
            ; SpewValue( L"CWww: RunOneFetchThread: Single UrlIndex fetched", UrlIndex );
        #endif

        COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
        // This is just a CYA test. For the paper was already fetched.
        if( pOnePaper != NULL
        &&  pOnePaper < PVOID_VALID_BELOW )
        {
            // This would NOT be okay. Worker threads cannot touch view,
            // but must set variables for main thread monitoring to act.
            // Top.Add( pOnePaper ); // Put fruit on display
            g_MainShallTopAddThisPaper = pOnePaper;
        }
    }

    delete pSolFrames;
    pSolFrames = NULL;
    pFetch->pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pFetch );
}

void CWww::RecursiveGetter( int Recursions, size_t UrlIndex, CBud * pBudLog, wchar_t * szBinaryPath, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int FromEngineNo, int PageIsAQrp, wchar_t * szQueryResultUrl, int bTopUrl, COneSurl * pOneSurl )
{
    #if DO_DEBUG_CALLS
        Routine( L"312" );
    #endif

    #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
        ; SpewValue( L"CWww: RecursiveGetter: Received this UrlIndex from caller", UrlIndex );
    #endif


    #if DO_DEBUG_FOLLOW
        ; SpewTwo( L"\r\n======= Recursive Getter: szQueryResultUrl =======", szQueryResultUrl );
    #endif


    #if DO_DEBUG_REJECTS
        if( szQueryResultUrl != NULL )
        {
            ; SpewTwo( L"=== szQueryResultUrl ===", szQueryResultUrl );
        }
    #endif

    CSol * pSolFramesThisLevel = new CSol( CSOL_SCALAR );
    // SED_PILE1
    // from CWww RecursiveGetter
    // This call does the work that downloads a single URL
    size_t FinalUrl = CommonGetter(
        UrlIndex,
        pBudLog,
        szBinaryPath,
        pSolFramesThisLevel,
        pSolMores,
        pSolHits,
        FromEngineNo,
        PageIsAQrp,
        szQueryResultUrl,
        bTopUrl,
        pOneSurl );
    // SED_PILE2

    // Originally, CommonGetter returned final url to RunOneFetchThread.
    // In that case, we do not achieve to follow all links out of frames.

    // Rather, I must return a list of top and all frame URLs to caller.
    // Then the top caller of RecursiveGetter can iterate the URL list,
    // and iterate their IDX for all links arising in all the recursive
    // framesets. This RecursiveGetter will not iterate to follow links.

    // During RunOneFetchThread's Step 1, the passed-in pSolFrame will
    // hold a list of the top and all frame UrlIndex successful fetches.

    // During RunOneFetchThread's Step 2 loop, it still has to pass me
    // a pSolFrame for syntax, but can ignore any URLs returned in it.

    // So, now, at any given level of recursion, I will iterate the list
    // of frames generated freshly within this level, calling myself, and
    // append each of those url indexes to the caller's past pSolFollow.

    if( FinalUrl != 0 )
    {
        wchar_t wk[20];
        _itow( FinalUrl, wk, 10 );
        size_t index = pSolFrames->AddKey( wk ); // add to caller's list
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cwww 3032" ); }
        #endif

        #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
            ; SpewValue( L"CWww: RecursiveGetter: Listing gotten Url for caller", FinalUrl );
        #endif

    }
    if( FinalUrl != 0
    && pSolFramesThisLevel->nList - 2 > 0 ) // -2 for head,tail
    {
        #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
            ; SpewValue( L"About to iterate frames", pSolFramesThisLevel->nList - 2 );
        #endif

        // pBudLog->pWsbResultText->Add( L"Getting frameset...\r\n\r\n" );

        // This loop recursively iterates frames AT THIS LEVEL from last Url.
        CoIt * pMalVector = pSolFramesThisLevel->GetSortedVector( CSOL_FORWARD );
        if( pMalVector != NULL )
        {
            size_t take = 0;
            for( ;; )
            {
                CoIt * pCoIt = pMalVector + take++;
                if( pCoIt->IsSentinel )
                    break;

                // One UrlIndex is held as itoa in the pCoIt's key value.

                wchar_t * FullKey = CoItFullKey( pCoIt );
                UrlIndex = _wtoi( FullKey );
                #if DO_DEBUG_FRAME
                    ; SpewValue( FullKey, UrlIndex );
                #endif
                MyFree( 3047, UNPREDICTABLE, FullKey );
                FullKey = NULL;
                #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
                    ; SpewValue( L"CWww: RecursiveGetter: Does Step 3 UrlIndex", UrlIndex );
                #endif

                #if DO_DEBUG_FRAME
                    ; SpewValue( L"CWww: RunOneFetchThread: Total No. of frames before Step 3", pSolFrames->nList - 2 );
                #endif

                // Add Links did not obey delete nor stop all,
                // and I think it was because some .cz web site
                // was continually making up new URLs ( in frames? )
                // mentioning log out.

                // So for one thing, put the stop bools ahead of
                // the recursive call. For another, add a depth
                // member, and stop at some maximum depth ( 10? ).

                if ( g_bStopAllThreads
                || gbMallocLimitExceeded
                || pBudLog->m_StopThisThread )
                    break;

                // I don't think I should have to get framesets for queries.
                // Otherwise, I'd have to test frames with RejectableHitUrl.
                if( PageIsAQrp == ZERO_ORDINAL_FOR_NON_QRP )
                {
                    if( Recursions > 10 )
                    {
                        pBudLog->pWsbResultText->Add( L"Frameset recursion reached 10 deep: URL ignored.\r\n" );
                    }
                    else
                    {
                        // SED_PILE1
                        // from CWww RecursiveGetter
                        // This call does the recursive fetch of frames in one frameset
                        // However, not called for query result pages ( QRPOrdinal != 0 ).
                        RecursiveGetter(
                            Recursions + 1, // deeper and deeper...
                            UrlIndex,
                            pBudLog,
                            szBinaryPath,
                            pSolFrames,
                            pSolMores,
                            pSolHits,
                            FromEngineNo,
                            PageIsAQrp,
                            szQueryResultUrl,
                            bTopUrl,
                            pOneSurl );
                        // SED_PILE2
                    }
                }

                #if DO_DEBUG_FRAME
                    ; SpewValue( L"CWww: RunOneFetchThread: Total No. of frames after Step 3", pSolFrames->nList - 2 );
                #endif

                // Let's think... At any given level of recursion,
                // this RecursiveGetter only iterates the frames
                // returned by CommonGetter, not by deeper calls
                // to RecursiveGetter. So I can now copy all the
                // succesful UrlIndexes of frames returned from
                // below me, into my caller's list of frame URLs,
                // and I will not be causing infinite recursions.

                // Rather than make a new pSolFrames for each call,
                // I can just pass in the pSolFrames that I received.
                // That was easy! Can it really be that easy?

                // Almost. The recursive fetch of frames works good.
                // But the passed-in, passed-on pSolFrames is empty.
                // All I need to do is, if FinalUrl from CommonGetter
                // is not zero, add that to my caller's pSolFrames
                // using _itow, like in CHtm at FRAME SRC attribute.
            }

            MyFree( 1395, UNPREDICTABLE, pMalVector );
            pMalVector = NULL;
        }
    }

    delete pSolFramesThisLevel;
    pSolFramesThisLevel = NULL;
}

size_t CWww::CommonGetter( size_t UrlIndex, CBud * pBudLog, wchar_t * szBinaryPath, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int FromEngineNo, int PageIsAQrp, wchar_t * szQueryResultUrl, int bTopUrl, COneSurl * pOneSurl )
{
    #if DO_DEBUG_CALLS
        Routine( L"313" );
    #endif

    #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
        ; SpewValue( L"CWww: CommonGetter: Received this UrlIndex from caller", UrlIndex );
    #endif

    // This new helper routine will, for every URL to fetch,
    // make a new COnePaper, call ObtainOneURLContent, then
    // call ProcessPaper, then finish hanging the new paper.
    // I will automatically follow any redirections in here.

    // CommonGetter automatically follows redirections.
    // CommonGetter automatically saves non-text files.
    // CommonGetter automatically fetches any frameset.

    // Return original or redirection UrlIndex for success;
    // Return 0 to stop caller from fetching paper's links.
    size_t FinalUrlIndex = 0;

    // This helper should be able to assist OneFetch first.
    // It should remain at arm's length from Fetch or Query.
    // It should be given a UrlIndex, and settings such as
    // whether to accept all, or texts only in http header,
    // and a binary path, and a CSol to list new URLIndices.

    // This helper should be able to assist OneQuery later.
    // What else might this need for queries? Return what?
    // I need the pBudLog, base class of pFetch or pQuery.

    // If passed path is non-NULL, I will save binary files.
    // Shape new work by saving all files prior to analysis.

    // First loop does original URL, later loops do redirections.
    int Redirections = 0;
    size_t UrlIndexToClaim = UrlIndex; // to start first loop
    size_t NextIterationUrlIndex = NULL;
    for( ;; )
    {
        if( UrlIndexToClaim < 2 )
        {
            ProgramError( L"CommonGetter UrlIndexToClaim < 2" );
            return 0; // error
        }

        // I'll need this string for annotations if nothing else
        wchar_t * pMalUrlKey = CSolAllUrls.GetFullKey( UrlIndexToClaim );
        if( pMalUrlKey == NULL )
        {
            ProgramError( L"CommonGetter: pMalUrlKey == NULL" );
            return 0; // error
        }

        #if DO_DEBUG_FOLLOW
            ; SpewTwo( L"\r\n======= Common Getter: Doing pMalUrlKey =======", pMalUrlKey );
        #endif

        void * pVoidToTest = CSolAllUrls.GetUserpVoid( UrlIndexToClaim );
        #if DO_DEBUG_FETCH
            ; SpewValue( L"pVoidToTest", ( size_t ) pVoidToTest );
        #endif

        // My original code only did URLs matching PVOID_UNTRIED.
        // This new attempt will also re-evaluate present papers.
        // This new attempt will also re-follow any redirections.

        // PVOID_UNTRIED       NULL
        // PVOID_CLAIMING      ( ( void* )( -1L ) )
        // PVOID_NOTFOUNDETC   ( ( void* )( -2L ) )
        // PVOID_REDIRECTION   ( ( void* )( -3L ) )
        // PVOID_UNDESIRABLE   ( ( void* )( -4L ) )
        // PVOID_QUERYREJECT   ( ( void* )( -5L ) )
        // PVOID_VALID_BELOW   ( ( void* )( -6L ) )

        // Should I be verbose to the log?
        // User needs feedback on re-follows. ( Re-get annotations. )
        // User needs feedback on re-redirect, bad URLs ( one line ).
        // So be prepared to annotate fetch log for all cases:

        size_t StartOffset = pBudLog->pWsbResultText->StrLen;

        switch( ( long ) pVoidToTest )
        {

        case ( long ) PVOID_UNTRIED:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was UNTRIED =======" );
            #endif
            #if DO_DEBUG_FETCH
                AddRejectionToLog( pBudLog, pMalUrlKey, L"Incoming: PVOID_UNTRIED..." );
            #endif
            // So TRY it!
            // Can I do that right in here, in a few lines?
            // this one is for fetching:
            if( CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_CLAIMING ) )
            {
                #if DO_DEBUG_FETCH
                    AddRejectionToLog( pBudLog, pMalUrlKey, L"ClaimUserpVoid Succeeded. Working on URL..." );
                #endif
                // If this routine returns a non-zero, it is a redirection URL to process.
                // Otherwise, I can recover outcome by studying UrlIndex's pVoid and paper.
                // He finishes all the rest of get URL processing and releasing this claim.

                #if DO_DEBUG_BINARIES
                    ; Spew( L"ComnGetr...BinPath:" );
                    ; Spew( ( szBinaryPath == NULL ) ? L"-null-" : szBinaryPath );
                #endif

                // SED_PILE1
                // from CWww CommonGetter
                // This call helps in the work that downloads a single URL
                NextIterationUrlIndex = GetterHelper(
                    UrlIndexToClaim,
                    pBudLog,
                    szBinaryPath,
                    pSolFrames,
                    pSolMores,
                    pSolHits,
                    FromEngineNo,
                    PageIsAQrp,
                    bTopUrl,
                    pOneSurl );
                // SED_PILE2
                pVoidToTest = CSolAllUrls.GetUserpVoid( UrlIndexToClaim );
                #if DO_DEBUG_FETCH
                    ; SpewValue( L"After GetterHelper, pVoidToTest", ( size_t ) pVoidToTest );
                #endif
                // This needs no NULL test, cuz I have NON-NULLed this URL.
                if( pVoidToTest < PVOID_VALID_BELOW )
                {
                    FinalUrlIndex = UrlIndexToClaim;
                }
            }
            else
            {
                #if DO_DEBUG_FETCH
                    AddRejectionToLog( pBudLog, pMalUrlKey, L"ClaimUserpVoid Failed. Postpone URL." );
                #endif
                // So I should wait in a sleepy loop until other process finishes.
                // Better yet, I should make list of such URL to defer processing.
                // For later...
            }
            break;

        case ( long ) PVOID_CLAIMING:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was CLAIMED =======" );
            #endif
            #if DO_DEBUG_FETCH
                AddRejectionToLog( pBudLog, pMalUrlKey, L"Incoming: PVOID_CLAIMING..." );
            #endif
            // So I should wait in a sleepy loop until other process finishes.
            // Better yet, I should make list of such URL to defer processing.

            // For later...
            break;

        case ( long ) PVOID_NOTFOUNDETC:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was 4xx =======" );
            #endif
            AddRejectionToLog( pBudLog, pMalUrlKey, L"Previously attempted. Was a bad URL request." );
            break;

        case ( long ) PVOID_REDIRECTION:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was 3xx =======" );
            #endif
            AddRejectionToLog( pBudLog, pMalUrlKey, L"Previously attempted. Was a redirection..." );
            // So re-follow that redirection.
            // All I have to do is lookup url's new index,
            // Use the value to set NextIterationUrlIndex.
            // Being in CSolRedirects does not mean fetched.
            {
                wchar_t wk[20];
                // key %d is sufficient for uniqueness.
                wsprintf( wk, L"%d", UrlIndexToClaim );
                size_t index = CSolRedirects.Find( wk );
                #if DO_DEBUG_ADDFIND
                    if( index == 1 )
                        { Spew( L"AddFind 1 at cwww 2893" ); }
                #endif
                if( index != 0 )
                    NextIterationUrlIndex = CSolRedirects.GetUserValue( index );
                #if DO_DEBUG_FETCH
                    ; SpewValue( wk, NextIterationUrlIndex );
                #endif
            }
            break;

        case ( long ) PVOID_UNDESIRABLE:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was 250 =======" );
            #endif
            AddRejectionToLog( pBudLog, pMalUrlKey, L"Previously attempted. Was unsuitable content." );
            break;

        case ( long ) PVOID_QUERYREJECT:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was query reject =======" );
            #endif
            AddRejectionToLog( pBudLog, pMalUrlKey, L"Previously attempted. Query no-fetch-rule reject." );
            break;

        default:
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Url was already fetched =======" );
            #endif
            // This URL was a prior-success with hanging paper.
            #if DO_DEBUG_FETCH
                AddRejectionToLog( pBudLog, pMalUrlKey, L"Incoming: prior valid paper..." );
            #endif

            // So re-evaluate that successful paper.
            // Get original header of to put in log.
            // wchar_t * pNotes = Pag.GetWordsExHeader( UrlIndexToClaim );
            // if( pNotes != NULL )
            // {
            //     pBudLog->pWsbResultText->Add( pNotes );
            //     MyFree( 2791, zx, pNotes );
            //     pNotes = NULL;
            // }

            // No, rather, just say URL was already fetched.
            AddRejectionToLog( pBudLog, pMalUrlKey, L"Previously fetched." );

            // I still need to return this, so caller may
            // follow links, if it had not done so before.
            FinalUrlIndex = UrlIndexToClaim;
            break;
        }
        size_t FinalOffset = pBudLog->pWsbResultText->StrLen;

        // I may have entered loop, finding PVOID_UNTRIED.
        // I may have entered loop, finding a valid paper.
        // Otherwise, I made non-linkable rejection notes.
        // Loop has handled and changed any PVOID_UNTRIED.

        // So if anything valid now, make a link to it.
        if( pVoidToTest < PVOID_VALID_BELOW )
        {
            #if DO_DEBUG_FETCH
                ; Spew( L"CommonGetter: outcome is a valid paper" );
            #endif
            pBudLog->pIdxResultIndex->AddIdx( StartOffset, FinalOffset, UrlIndexToClaim, 0 );
            pBudLog->pWsbResultText->Add( L"\r\n" );
        }

        int stopping = 0; // hold this thought until present facts

        if( NextIterationUrlIndex == NULL ) // no redirection
        {
            #if DO_DEBUG_FETCH
                ; Spew( L"CommonGetter: NextIterationUrlIndex == NULL" );
            #endif
            stopping = 1;
            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: No redirect" );
            #endif
        }
        else
        {
            // New work to cross-reference links:
            size_t iAnchorText = CSolAnchorText.AddKey( L"Redirection" );
            #if DO_DEBUG_ADDFIND
            if( iAnchorText <= 1 )
                { Spew( L"AddFind <= 1 at cwww 4002" ); }
            #endif
            AddAnchorHeadTailText( UrlIndexToClaim, NextIterationUrlIndex, iAnchorText );

            #if DO_DEBUG_FOLLOW
                ; Spew( L"Common Getter: Redirection" );
            #endif
            // I probably should add a rule to catch cyclic redirections,
            // like this and others alike, that went for 10 redirections:
            // http://taylorandfrancis.metapress.com/index/MLX3RYW11XWE77N2.pdf
            // Http status 3XX = redirection.
            // http://www.informaworld.com/ampp/siteindex?request=%2Findex%2FMLX3RYW11XWE77N2%2Epdf&host=http%3A%2F%2Ftaylorandfrancis%2Emetapress%2Ecom
            // Http status 3XX = redirection.
            // http://www.informaworld.com/smpp/ftinterface?content=a713689354&rt=0&format=pdf
            // Http status 3XX = redirection.
            // http://www.informaworld.com/smpp/ftinterface?content=a713689354&rt=0&format=pdf&cktry=1825251388-68317180_3923854332512
            // Http status 3XX = redirection.
            // http://www.informaworld.com/smpp/ftinterface?content=a713689354&rt=0&format=pdf
            // Previously attempted. Was a redirection...

            if( ++ Redirections == 4 ) // arbitrary following limit
            {
                pBudLog->pWsbResultText->Add( L"Ignoring 4-th consecutive redirection.\r\n\r\n" );
                // obs: Top.UpdateViewIfOnScreen( pBudLog );

                #if DO_DEBUG_FETCH
                    ; Spew( L"CommonGetter: ++ Redirections == 4" );
                #endif
                stopping = 1;
            }

            // furthermore, when working for query ( not fetch ),
            // I need to test the redirection URL against the
            // URL rejection rules per the current result URL.
            // Which means another argument must be passed in.
            //
            // This security... was not rejected, although listed in table.
            // https://adcenter.looksmart.com/?sid=lsl100867
            // Http status 3XX = redirection.
            // https://adcenter.looksmart.com/security/login
            //
            // This empty path part is listed to reject, but it came via a 3xx:
            // http://www.altavista.com/trans.go?urltext=http%3A%2F%2Fwww.mp3.com%2F+&language=en
            // Http status 3XX = redirection.
            // http://www.altavista.com/

            // This was the wrong bool to test here:
            // if( QRPOrdinal )

            if( szQueryResultUrl != NULL )
            {
                wchar_t * pMalNextUrlKey = CSolAllUrls.GetFullKey( NextIterationUrlIndex );
                #if DO_DEBUG_FOLLOW
                    ; SpewTwo( L"\r\n======= Common Getter: Examining pMalNextUrlKey =======", pMalNextUrlKey );
                #endif
                if( pMalNextUrlKey == NULL )
                {
                    ProgramError( L"CommonGetter: pMalNextUrlKey == NULL" );
                    return 0; // error
                }

                if( RejectableQueryUrl( szQueryResultUrl, pMalNextUrlKey ) )
                {
                    // Ok, I accept that I cannot just enable following
                    // because of bTopUrl, in order to have total power
                    // to reject; But I need to see rejected URL in log:

                    AddRejectionToLog( pBudLog, pMalNextUrlKey, L"Rejected. In NOT list." );

                    MarkAsRejectedUrl( NextIterationUrlIndex );
                    #if DO_DEBUG_REJECTS
                        ; SpewTwo( L"Rejected result redirection URL", pMalNextUrlKey );
                    #endif
                    stopping = 1;
                }
                MyFree( 3726, UNPREDICTABLE, pMalNextUrlKey );
            }
            else
            {
                #if DO_DEBUG_FOLLOW
                    ; Spew( L"Common Getter: Not in a query. Not reject-examining pMalNextUrlKey" );
                #endif
            }
        }

        if( pBudLog->m_StopThisThread )
        {
            #if DO_DEBUG_FETCH
                ; Spew( L"CommonGetter: pFetch->m_StopThisThread" );
            #endif
            stopping = 1;
        }
        if( gbMallocLimitExceeded )
        {
            #if DO_DEBUG_FETCH
                ; Spew( L"CommonGetter: gbMallocLimitExceeded" );
            #endif
            stopping = 1;
        }
        if( g_bStopAllThreads )
        {
            #if DO_DEBUG_FETCH
                ; Spew( L"CommonGetter: g_bStopAllThreads" );
            #endif
            stopping = 1;
        }

        MyFree( 2595, UNPREDICTABLE, pMalUrlKey );
        pMalUrlKey = NULL;
        // obs: Top.UpdateViewIfOnScreen( pBudLog );

        if( stopping )
            break;

        UrlIndexToClaim = NextIterationUrlIndex;
        NextIterationUrlIndex = NULL;
    }

    #if DO_DEBUG_FETCH || DO_DEBUG_FRAME
        ; SpewValue( L"CWww: CommonGetter: Returning this FinalUrlIndex to caller", FinalUrlIndex );
    #endif
    return FinalUrlIndex; // non-zero if successful
}

size_t CWww::GetterHelper( size_t UrlIndexToClaim, CBud * pBudLog, wchar_t * szBinaryPath, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, int FromEngineNo, int PageIsAQrp, int bTopUrl, COneSurl * pOneSurl )
{
    // This part of Getter is too big for readable code above.
    // My caller has successfully claimed the URL. That's all.
    // I must create a new paper, call the http fetch routine,
    // call the CHtm parse routine, and return redirect index.
    // I must release the claim and change status on URLIndex.


    #if DO_DEBUG_FACTTEST
        Spew( L"GetterHelper - Setting up parameters..." );
        SpewValue( L"UrlIndexToClaim", UrlIndexToClaim );
        SpewTwo( L"szBinaryPath", szBinaryPath );
        SpewValue( L"FromEngineNo", FromEngineNo );
        SpewValue( L"PageIsAQrp", PageIsAQrp );
        SpewValue( L"bTopUrl", bTopUrl );
        if( pOneSurl == NULL )
        {
            Spew( L"pOneSurl == NULL" );
        }
        else
        {
            Spew( L"pOneSurl != NULL. Details..." );
            SpewTwo( L"pOneSurl->szDomainName", pOneSurl->szDomainName );
            SpewValue( L"pOneSurl->GetOrdinal", pOneSurl->GetOrdinal );
            SpewValue( L"pOneSurl->nRuleVector", pOneSurl->nRuleVector );
            SpewValue( L"pOneSurl->ContainsAnyRuleNoneGoodUntil", pOneSurl->ContainsAnyRuleNoneGoodUntil );
            SpewValue( L"pOneSurl->KeepQueryResultPageAsGoodText", pOneSurl->KeepQueryResultPageAsGoodText );
            SpewValue( L"pOneSurl->AnnotateFactsForStudy", pOneSurl->AnnotateFactsForStudy );
        }
    #endif



    #if DO_DEBUG_BINARIES
        ; Spew( L"GetHelpr...BinPath:" );
        ; Spew( ( szBinaryPath == NULL ) ? L"-null-" : szBinaryPath );
    #endif

    // This new paper is for fetching a URL in CommonGetter:
    COnePaper * pOnePaper = new COnePaper( 0 ); // this one is in common getter
    // Every making of a new COnePaper must create its pWsbAnnotation.
    pOnePaper->pWsbAnnotation = new CWsb;

    // This was a weak provision:
    // Oh, it marks page as BEING a search engine's Query Result Page.
    // This pOnePaper variable is terribly overloaded.
    if( PageIsAQrp != ZERO_ORDINAL_FOR_NON_QRP )
    {
        // pOneSurl is not NULL.
        // pOneSurl->KeepQueryResultPageAsGoodText
        // FromeEngineNo == pOneSurl->GetOrdinal

        int BoolsEngineNo = FromEngineNo;
        if( pOneSurl->KeepQueryResultPageAsGoodText )
            BoolsEngineNo |= BIT_IN_ORDINAL_TO_KEEP;
        pOnePaper->PageIsAQrpThisIsItsOrdinal = BoolsEngineNo; // two bools and ordinal
    }

    // This is a strong provision: It credits URL to a search engine ordinal.
    if( FromEngineNo != 0 )
    {
        // Now I want to add to list on the hit URL at nSourceIndex,
        // that it was found by search engine pOneSurl->GetOrdinal.
        //
        // Rather than hold an empty pIntFromEngine on each URL,
        // constructor left pIntFromEngine = NULL; I create JIT.
        if( pOnePaper->pIntFromEngine == NULL )
        {
            pOnePaper->pIntFromEngine = new CInt( );
        }
        pOnePaper->pIntFromEngine->Add( FromEngineNo );
    }

    size_t nWBuf = 0;       // Strlen. malloc is larger.
    wchar_t * pWBuf = NULL; // May stay NULL at times.

    // ObtainOneURLContent gets the source paper from the web.
    // ObtainOneURLContent adds any non-success status to log.
    // ObtainOneURLContent returns original URL bytes in CAsb.
    {
        CAsb * pAsbOutput = new CAsb( );
        // Examine pOnePaper->HttpHeaderStatus for result of this call:

        int BinaryOkay = 0;
        int PromptToSave = 0;

        if( szBinaryPath != NULL ) // due to a fetch with non-text checked.
        {
            BinaryOkay = 1;
            PromptToSave = 0;
        }
        // This ELSE gives priority to having checked box: [x] non-text too.
        else
        if( bTopUrl
        && ! PageIsAQrp )
        {
            // special case for fetch w/o [ ] non-text, will prompt: Save As?
            BinaryOkay = 1;
            PromptToSave = 1;
        }

        #if DO_DEBUG_BINARIES
            ; SpewValue( L"bTopUrl", bTopUrl );
            // undef? ; SpewValue( L"QRPOrdinal", QRPOrdinal );
            ; SpewValue( L"BinaryOkay", BinaryOkay );
            ; SpewValue( L"PromptToSave", PromptToSave );
        #endif

        ObtainOneURLContent( pOnePaper, pAsbOutput, UrlIndexToClaim, pBudLog, BinaryOkay );

        // For various reasons, pAsbOutput may be empty.
        // Hence, leaving here, pWBuf might remain NULL.
        if( pAsbOutput->StrLen > 0 )
        {
            #if DO_DEBUG_BINARIES
                ; SpewValue( L"GH: Size>0:", pAsbOutput->StrLen );
            #endif
            size_t nMalSource = 0;
            BYTE * pMalSource = pAsbOutput->GetBuffer( & nMalSource );

            // Start the non-text capability by saving all files.
            // A non-null folder path also serves as binary bool.

            // That worked, now stop saving files that will make
            // acceptable text papers to display in the program.

            // The header often does not reflect the truth on type.
            // Recognize other common non-text filename extensions.
            // Move that IsBinary... call to where ParseHttpHeader.

            int doit = 1;
            switch( pOnePaper->HttpHeaderContentType )
            {
            case CONTENT_TEXT:      doit = 0; break;
            case CONTENT_HTMLTEXT:  doit = 0; break;
            case CONTENT_PLAINTEXT: doit = 0; break;
            }

            #if DO_DEBUG_SAVEHTML
                doit = 1; // Save html files too
            #endif

            if( doit
            && BinaryOkay )
            {
                wchar_t * pUrlKey = CSolAllUrls.GetFullKey( UrlIndexToClaim ); // a malloc, user frees
                if( pUrlKey == NULL )
                {
                    ProgramError( L"GetterHelper: pUrlKey NULL" );
                    return 0;
                }

                #if DO_DEBUG_BINARIES || DO_DEBUG_SAVEHTML
                    ; Spew( L"GH: Url:" );
                    ; Spew( pUrlKey );
                #endif

                wchar_t * pFileName = NULL;
                if( PromptToSave )
                {
                    // Put up a Save As dialog.
                    // I should localize that nastiness to cfio.
                    // Return me a pointer to a malloc, to free:

                    // Try to suggest a most appropriate file extension:
                    // look for the last '.', cancel upon '/', stop at '?'.
                    wchar_t * scan = pUrlKey;
                    wchar_t * pslash = NULL;
                    wchar_t * pdot = NULL;
                    for( ;; )
                    {
                        if( *scan == NULL )
                            break;
                        if( *scan == '?' )
                            break;
                        if( *scan == '/' )
                        {
                            pslash = scan;
                            pdot = NULL;
                            break;
                        }
                        if( *scan == '.' )
                        {
                            pdot = scan;
                            break;
                        }

                        scan++;
                    }

                    wchar_t Suggestion[100];
                    int FillSuggestion = 0;

                    if( pslash != NULL )
                    {
                        // accept a limited range of filenaming characters.
                        for( ;; )
                        {
                            if( *++pslash == NULL ) // pre-increment off '/'
                                break;

                            if( *pslash < 127 && iswalnum( *pslash ) )
                            {

                                Suggestion[ FillSuggestion ] = *pslash;
                                if( ++ FillSuggestion == sizeof( Suggestion ) / sizeof( *Suggestion ) - 1 )
                                    break;

                            }
                        }
                    }

                    Suggestion[ FillSuggestion ] = NULL; // even if empty


                    wchar_t UrlExtension[15];
                    int FillExtension = 0;

                    if( pdot != NULL )
                    {
                        // accept a limited range of filenaming characters.
                        for( ;; )
                        {
                            if( *++pdot == NULL ) // pre-increment off '.'
                                break;

                            if( *pdot < 127 && iswalnum( *pdot ) )
                            {

                                UrlExtension[ FillExtension ] = *pdot;
                                if( ++ FillExtension == sizeof( UrlExtension ) / sizeof( *UrlExtension ) - 1 )
                                    break;

                            }
                        }
                    }

                    if( FillExtension > 0 )
                    {
                        UrlExtension[ FillExtension ] = NULL;
                    }
                    else
                    {
                        wcscpy( UrlExtension, L"bin" );
                    }

                    pFileName = Fio.PromptForSaveAsAnyFileName( Suggestion, UrlExtension );
                }
                else
                {
                    // Return me a pointer to a malloc, to free:

                    // SED_PILE1
                    int SaveAsAQrp = 0;
                    if( PageIsAQrp
                    && ! pOneSurl->KeepQueryResultPageAsGoodText )
                        SaveAsAQrp = 1;

                    pFileName = FullFileNameForUrl( // in GetterHelper
                        szBinaryPath,
                        pUrlKey,
                        1,           // 1 = .bin or per url ( not .htm )
                        SaveAsAQrp,
                        FromEngineNo );
                    // SED_PILE2
                }

                if( pFileName != NULL )
                {
                    #if DO_DEBUG_BINARIES || DO_DEBUG_SAVEHTML
                        ; Spew( L"GH: Non-null filename:" );
                        ; Spew( pFileName );
                    #endif
                    pBudLog->pWsbResultText->Add( L"Saving " );
                    pBudLog->pWsbResultText->Add( pFileName );
                    pBudLog->pWsbResultText->Add( L"\r\n" );
                    // obs: Top.UpdateViewIfOnScreen( pBudLog );

                    // Here is where I saved any binary downloaded file:
                    // In WordsEx Version 1.1, also create a text file
                    // next to it, containing the source URL downloaded:
                    {
                        // I need 3 pieces as determined in the Fio. call...
                        // 1, a filename;  2, content(url);  3, contentsize
                        int nFileName2 = wcslen( pFileName ) + 5; // for .txt, EOS
                        wchar_t * pFileName2 = ( wchar_t * ) MyMalloc( 4913, nFileName2 * sizeof( wchar_t ) );
                        if( pFileName2 != NULL )
                        {
                            wcscpy( pFileName2, pFileName );
                            wcscat( pFileName2, L".txt" );

                            // If I don't decorate with newlines, etc,
                            // then I need not malloc -- Oh, but these
                            // are wide characters!
                            // I should convert to an mbs buffer.
                            int nUrlKey = wcslen( pUrlKey ) * sizeof( wchar_t );
                            Fio.CommonFileSave( pFileName2, (unsigned char *) pUrlKey, nUrlKey );

                            MyFree( 3252, zx, pFileName2 );
                            pFileName2 = NULL;
                        }
                    }
                    Fio.CommonFileSave( pFileName, pMalSource, nMalSource );
                    MyFree( 3252, zx, pFileName );
                    pFileName = NULL;
                }
                MyFree( 3255, zx, pUrlKey );
                pUrlKey = NULL;
            }

            // Convert ascii, 2-byte UCS or UTF-8 into safe Unicode chars.
            // CategorizeAndConvertInputBytesToWideWithBinaryRejection returns NULL if it determines binary data.
            // This call is after fetching a URL in CommonGetter:
            pWBuf = Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection( & nWBuf, pMalSource, nMalSource );
            MyFree( 735, ( nMalSource + 1 ) * sizeof( BYTE ), pMalSource );
            pMalSource = NULL;

            // But will a live fetch still save binary after dislike content?
            // It looks like no. Fix that right about here...

            #if DO_DEBUG_FETCH
                ; SpewValue( L"GetterHelper original byte count", nMalSource );
            #endif
        }
        delete pAsbOutput;
        pAsbOutput = NULL;
    }
    #if DO_DEBUG_FETCH
        ; SpewValue( L"GetterHelper unicode character count", nWBuf );
    #endif

    // After "Internet opertion timed out", I reached pgm err below:
    // pOnePaper->HttpHeaderStatus == FETCH_STATUS_NOT_ATTEMPTED:
    // Init to 400 above before calling Obtain in case of failures.


    // I think I turned off all the AddRejection calls,
    // but bad when fetch just one URL and get nothing:
    // Add Web Page:
    // http://slate.msn.com/
    //
    // Thread started.
    //
    //
    // Thread ended.
    //   -- Add Web Page:
    // So test new bool, bTopUrl, to restore messages:


    switch( pOnePaper->HttpHeaderStatus )
    {
    case FETCH_STATUS_NOT_ATTEMPTED:
        ProgramError( L"CommonGetter: FETCH_STATUS_NOT_ATTEMPTED" );
        return 0;
        break;

    case FETCH_STATUS_200_SUCCESS:
        if( pWBuf != NULL )
            break; // finish the non-empty success case after switch

        // fall into undesirable case:

    case FETCH_STATUS_200_BUT_CONTENT_UNDESIRABLE:
        if( bTopUrl )
        {
            pBudLog->pWsbResultText->Add( L"Was unsuitable content.\r\n" );
        }
        if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_UNDESIRABLE ) )
            ProgramError( L"CommonGetter: ! Release claim" );
        if( pWBuf != NULL )
        {
            MyFree( 2879, UNPREDICTABLE, pWBuf );
            pWBuf = NULL;
        }
        delete pOnePaper;
        pOnePaper = NULL;
        return 0;
        break;

    case FETCH_STATUS_300_REDIRECT:
        size_t RedirectionUrlIndex;
        RedirectionUrlIndex = pOnePaper->NewLocationIndex;
        if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_REDIRECTION ) )
            ProgramError( L"CommonGetter: ! Release claim" );
        if( pWBuf != NULL )
        {
            MyFree( 2893, UNPREDICTABLE, pWBuf );
            pWBuf = NULL;
        }
        delete pOnePaper;
        pOnePaper = NULL;

        // I have the old UrlIndex and the new UrlIndex.
        // So I'll add the pair to global CSolRedirects.
        // Being in CSolRedirects does not mean fetched.

        {
            wchar_t wk[20];
            // key %d is sufficient for uniqueness.
            wsprintf( wk, L"%d", UrlIndexToClaim );
            size_t index = CSolRedirects.AddKey( wk );
            #if DO_DEBUG_ADDFIND
                if( index == 1 )
                    { Spew( L"AddFind 1 at cwww 2893" ); }
            #endif
            CSolRedirects.SetUserValue( index, RedirectionUrlIndex );
            #if DO_DEBUG_FETCH
                ; SpewValue( wk, RedirectionUrlIndex );
            #endif
        }
        return RedirectionUrlIndex;
        break;

    case FETCH_STATUS_400_FAILURE:
        if( bTopUrl )
        {
            pBudLog->pWsbResultText->Add( L"Was a bad URL request.\r\n" );
        }

        if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_NOTFOUNDETC ) )
            ProgramError( L"CommonGetter: ! Release claim" );
        if( pWBuf != NULL )
        {
            MyFree( 2925, UNPREDICTABLE, pWBuf );
            pWBuf = NULL;
        }
        delete pOnePaper;
        pOnePaper = NULL;
        return 0;
        break;

    default:
        ProgramError( L"CommonGetter: FetchStatus" );
        // This is the universe of possibilities. What did I omit?
        // FETCH_STATUS_NOT_ATTEMPTED = 0,
        // FETCH_STATUS_200_SUCCESS = 200,
        // FETCH_STATUS_200_BUT_CONTENT_UNDESIRABLE = 250,
        // FETCH_STATUS_300_REDIRECT = 300,
        // FETCH_STATUS_400_FAILURE = 400,
        return 0;
        break;
    }

    // Continue with the status 200 non-empty and desirable success path.
    // Desirable means TEXT only. Any binary file work was handled above.

    // I got a memory error with pOnePaper == NULL below, on
    // http://video.google.com/video/js/688808920-videojs.js
    // because some case deleted above, but failed to return.
    if( pOnePaper == NULL )
    {
        ProgramError( L"CommonGetter: pOnePaper == NULL" );
    }

    #if DO_DEBUG_FETCH
        ; Spew( L"Success path. Do Process part one." );
    #endif

    // This part parses, analyzes, the source paper.
    // 1 of 4 similar sequences. This one is after HTTP download.
    int AsText = 0;
    if( pOnePaper->HttpHeaderContentType == CONTENT_PLAINTEXT )
        AsText = 1;

    // I see no value in revising BASE url during a live fetch.
    // Therefore I'll ignore the return value from ProcessPaper.

    // The first half runs CHtm parser ( unless AsText has bit 0x01 ).
    // The first half transfers CHtm variables into paper variables.

    // SED_PILE1
    // from CWww GetterHelper
    // This page processing call is for downloaded files.
    Pag.ProcessPaper(
        UrlIndexToClaim,        // size_t UrlIndex
        pOnePaper,              // COnePaper * pOnePaper
        pWBuf,                  // wchar_t * pInputBuffer
        nWBuf,                  // size_t nInputSize
        AsText,                 // int AsText
        pSolFrames,             // CSol * pSolFrames
        pSolMores,              // CSol * pSolMores
        pSolHits,               // CSol * pSolHits
        pOneSurl );             // COneSurl * pOneSurl
    // SED_PILE2

    MyFree( 2966, UNPREDICTABLE, pWBuf );
    pWBuf = NULL;

    #if DO_DEBUG_FETCH
        ; Spew( L"Success path. Do Process part two." );
    #endif

    // The second half applies any charset to the text buffer.
    // The second half does output text scans and annotations.

    int ShowAsAQrp = 0; // complex idea to me... tied me up for hours.
    if( PageIsAQrp )
    {
        ShowAsAQrp = 1; // I planned an alternate annotation for QRPs.
        // But wait, there's more...!
        // I can trust pOneSurl is not null in here.
        if( pOneSurl->KeepQueryResultPageAsGoodText )
            ShowAsAQrp = 0; // Then, even though a QRP, parse richly.
    }
    Pag.SecondPartOfProcessForText( UrlIndexToClaim, ShowAsAQrp, pOnePaper );

    // Now insert all annotations and second newline, atop paper content.
    {
        size_t nMalNewTop = 0;
        wchar_t * pMalNewTop = pOnePaper->pWsbAnnotation->GetBuffer( & nMalNewTop );
        pOnePaper->pWsbResultText->Insert( pMalNewTop, 1 ); // +1 newline
        int SizeChange = nMalNewTop + 2; // wchars: 1 for CR, 1 for LF
        pOnePaper->pIdxResultIndex->IncreaseOffsets( SizeChange );
        pOnePaper->pIdxTextBlocks->IncreaseOffsets( SizeChange );
        pOnePaper->pIdxSentences->IncreaseOffsets( SizeChange );
        #if DO_DEBUG_FETCH
            ; Spew( pMalNewTop );
        #endif
        MyFree( 728, UNPREDICTABLE, pMalNewTop );
        pMalNewTop = NULL;
    }

    if( pOnePaper->pIdxSentences->nSlots >= 10 ) // "significant" pages fetched
    {
        pOnePaper->FetchedButNotViewed = 1; // "significant" pages fetched
        g_nPapersFetchedButNotViewed ++;
    }

    // I am about to ask CSol to swap my paper pointer for claim marker.
    // Before doing that, pOnePaper must contain the valid index number.

    pOnePaper->m_CSolIndex = UrlIndexToClaim;

    // This call must work, and finishes my internal tasks.
    if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, pOnePaper ) )
        ProgramError( L"CommonGetter: ! Swap paper for claim" );

    // Annotate log with the URL and other notes from ProcessPaper.
    {
        // This IDX would be redundant, overlap one in Getter itself:
        // But I still need the AddWsb, just not the AddIdx...

        // size_t StartOffset = pBudLog->pWsbResultText->StrLen;

        pBudLog->pWsbResultText->AddWsb( pOnePaper->pWsbAnnotation );

        // size_t FinalOffset = pBudLog->pWsbResultText->StrLen;
        // pBudLog->pIdxResultIndex->AddIdx( StartOffset, FinalOffset, UrlIndexToClaim, 0 );

        pBudLog->pWsbResultText->Add( L"\r\n" );
        // obs: Top.UpdateViewIfOnScreen( pBudLog );
    }

    #if DO_DEBUG_FETCH
        ; Spew( L"Success path. Hung finished paper." );
    #endif

    return 0; // success, no redirection UrlIndex
}


void CWww::RunOneQueryThread( size_t QueryIndex )
{
    #if DO_DEBUG_CALLS
        Routine( L"479" );
    #endif

    // RunOneQueryThread is similar to RunOneFetchThread,
    // but instead of receiving one URL, will receive one
    // query string, attach it to all listed SearchUrls,
    // fetch and analyse each result page, and may fetch
    // all links listed therein, or have some heuristics
    // to distinguish and fetch only the good hit links.

    // After using each SearchUrl, there should be some
    // scoring applied for its success, speed and size,
    // and for quantity, quality and redundancy of hits,
    // to adjust some relative SearchUrl scores so that
    // best engines will be used first, and to annotate
    // during the search form dump ( and reload ) process.

    // Query work should also use the Fetch provision
    // that obtains all framesets for any one hit URL.
    // I don't know if that is needed for a query URL.
    // No, seach engine results are right in the page.

    // Query work will never fetch binaries like Fetch.
    // Actually, I plan to change that. Go add the bool.

    // RunOneQueryThread is servicing just one of possibly
    // many concurrent threads going in the same Cwww class.
    // I must pass my thread-controlling pQuery pointer into
    // any helper subroutines, and use it to access log, etc.
    // In fact, I must prefix pQuery to anything used myself.

    // Therefore, RunOneQueryThread has received a QueryIndex.
    // QueryIndex is into CSolUserQuerys ( not CSolAllUrls ).

    // The Key in CSolUserQuerys is user's query expression.

    // That Key in CSolUserQuerys finds a pVoid which points
    // to a COneQuery * pQuery instance, to control my work.

    // QueryDlgProc has init these members of the COneQuery:
    // pFruit->m_QueryDiligence = g_QueryDiligence;
    // pFruit->m_NonTextLinks = g_NonTextLinks;

    // QueryDlgProc also added that new Query to the display.

    // Get my thread's own pQuery hanging in CSolUserQuerys.
    COneQuery * pQuery = ( COneQuery * ) CSolUserQuerys.GetUserpVoid( QueryIndex );
    if( pQuery == NULL )
    {
        ProgramError( L"RunOneQueryThread: pQuery == NULL" );
        return;
    }

    #if DO_DEBUG_QUERY
        ; SpewValue( L"Starting on QueryIndex", QueryIndex );
    #endif

    // Get my thread's query expression Key string,
    // adjust its format, save into pWsbQueryValue.
    // 0. Don't bother trimming unlikely whitespace.
    // 1. apply UTF-8 to chars over 0x7f? or over 0xff?
    // 2. convert spaces and unsafe chars to %xx form.

    // Actually, between 0x80 and 0xff may depend on code page.
    // No, Any of those were changed into UCS during HTM parse.

    {
        wchar_t * pMalKey = CSolUserQuerys.GetFullKey( QueryIndex );
        if( pMalKey == NULL )
        {
            ProgramError( L"RunOneQueryThread: pMalKey == NULL" );
            return;
        }

        #if DO_DEBUG_QUERY
            ; Spew( L"Received this query string to process:" );
            ; Spew( pMalKey );
        #endif

        // adjust its format, save into pWsbQueryValue.
        pQuery->pWsbQueryValue->Reset( ); // In case thread was restarted

        size_t nScan = 0;
        for( ;; )
        {
            // cloned from CPag::Interleave loop.
            // Although I am converting to <= 7f byte range,
            // I still need to handle the bytes as wchar_t.
            wchar_t convert[4];
            wchar_t percent[3];
            percent[0] = '%';
            wchar_t wc = pMalKey [ nScan ];
            if( wc == NULL )
                break;
            if( wc <= 0x007f )
            {
                //  0x00000000 - 0x0000007F: 0xxxxxxx
                if( wc < ' ' )
                    wc = ' ';   // Not expected, but prevent control chars
                convert[0] = ( BYTE ) wc;
                // Not all USASCII values are safe URL bytes.
                // Convert unsafe URL bytes to the %xx form.

                // Surf4me had a truely stunning analysis:
                // I can't think of any truly general qualifications
                // for possible query topics. Just turn whatever the
                // user gave me into a well-escaped string. Only the
                // UNRESERVED = ( ALPHANUM|MARK ) can I pass through
                // because RESERVED, ESCAPED, SPACE, etc. mean stuff.
                // The slick unreserved macro expects a char pointer.

                if( unreserved( convert ) )
                {
                    pQuery->pWsbQueryValue->Addn( convert, 1 );
                }
                else
                {
                    percent[1] = ToHex[ ( convert[0] >> 4 ) & 0x0f ];
                    percent[2] = ToHex[   convert[0] & 0x0f ];
                    pQuery->pWsbQueryValue->Addn( percent, 3 );
                }
            }
            else if( wc <= 0x07ff )
            {
                //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
                convert[0] = ( BYTE ) ( ( ( wc >> 6 ) & 0x1f ) | 0xc0 );
                convert[1] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                // These bytes are always above 0x7f, so use %xx form:
                percent[1] = ToHex[ ( convert[0] >> 4 ) & 0x0f ];
                percent[2] = ToHex[   convert[0] & 0x0f ];
                pQuery->pWsbQueryValue->Addn( percent, 3 );
                percent[1] = ToHex[ ( convert[1] >> 4 ) & 0x0f ];
                percent[2] = ToHex[   convert[1] & 0x0f ];
                pQuery->pWsbQueryValue->Addn( percent, 3 );
            }
            else
            {
                //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
                convert[0] = ( BYTE ) ( ( ( wc >> 12 ) & 0x0f ) | 0xe0 );
                convert[1] = ( BYTE ) ( ( ( wc >> 6 ) & 0x3f ) | 0x80 );
                convert[2] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                // These bytes are always above 0x7f, so use %xx form:
                percent[1] = ToHex[ ( convert[0] >> 4 ) & 0x0f ];
                percent[2] = ToHex[   convert[0] & 0x0f ];
                pQuery->pWsbQueryValue->Addn( percent, 3 );
                percent[1] = ToHex[ ( convert[1] >> 4 ) & 0x0f ];
                percent[2] = ToHex[   convert[1] & 0x0f ];
                pQuery->pWsbQueryValue->Addn( percent, 3 );
                percent[1] = ToHex[ ( convert[2] >> 4 ) & 0x0f ];
                percent[2] = ToHex[   convert[2] & 0x0f ];
                pQuery->pWsbQueryValue->Addn( percent, 3 );
            }

            nScan ++;
        }
        MyFree( 4240, zx, pMalKey );
    }

    size_t nMalUserQry = 0;
    wchar_t * pMalUserQry = pQuery->pWsbQueryValue->GetBuffer( & nMalUserQry ); // a malloc, user frees
    if( pMalUserQry == NULL )
    {
        ProgramError( L"RunOneQueryThread: pMalUserQry == NULL" );
        return;
    }

    // 0. Let's do that trim of whitespace after all.
    wchar_t * szTrimmed = pMalUserQry;
    for( ;; )
    {
        if( *szTrimmed != ' '
        && *szTrimmed != '\t'
        && *szTrimmed != '\r'
        && *szTrimmed != '\n' )
            break;
        szTrimmed++;
    }
    int nLen = wcslen( szTrimmed );
    wchar_t * pPast = szTrimmed + nLen;
    for( ;; )
    {
        if( pPast == szTrimmed )
            break;
        if( pPast[-1] != ' '
        && pPast[-1] != '\t'
        && pPast[-1] != '\r'
        && pPast[-1] != '\n' )
            break;
        pPast--;
    }
    *pPast = NULL;

    #if DO_DEBUG_QUERY
        ; Spew( L"Adjusted query string:" );
        ; Spew( szTrimmed );
    #endif

    if( pPast == szTrimmed )
    {
        pQuery->pWsbResultText->Add( L"Query string was empty. Nothing to do.\r\n" );
        Top.UpdateViewIfOnScreen( pQuery );
        return;
    }

    #if DO_DEBUG_QUERY
        ; SpewValue( L"CSolSearchUrls.nList", CSolSearchUrls.nList );
    #endif

    if( CSolSearchUrls.nList <= 2 ) // deduct 2 counts for head and tail
    {
        pQuery->pWsbResultText->Add( L"No search engine query URLs listed. Nothing to do.\r\n" );
        Top.UpdateViewIfOnScreen( pQuery );
        return;
    }

    pQuery->pWsbResultText->Add( szSimpleBlurb );

    pQuery->pWsbResultText->Add( L"\r\nThread started.\r\n\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pQuery );

    #if DO_DEBUG_BINARIES
        ; Spew( L"RunOneFetchThread...pQuery->m_NonTextPath:" );
        ; Spew( ( pQuery->m_NonTextPath == NULL ) ? L"-null-" : pQuery->m_NonTextPath );
    #endif

    // I should like to organize this as a simple loop here,
    // not as spaghetti linear sequences of similar actions.

    // Starting the new way:
    // build a vector of *pSolMore, one per search engine url.
    // build a vector of *pSolHits, one per search engine url.
    // init vector of *pSolMore with 1 each search engine url.
    // run an outer loop to iterate up the vector, all s.e.
    // run an inner loop to process ( just one ) unfetched MORE,
    // and then to fetch all HITS added by the one MORE fetch.
    // I need a third pOneSurl * vector to keep each pOneSurl.

    // If it is not 0, it was already init, do not re-init.
    if( pQuery->nEngineVectors == 0 )
    {
        // Then all four vectors must be malloc'ed and
        // both filled with newly created CSol objects.

        pQuery->nEngineVectors = CSolSearchUrls.nList - 2;

        pQuery->ppSolMore = ( CSol * * ) MyMalloc( 5030, pQuery->nEngineVectors * sizeof( CSol * ) );
        pQuery->ppSolHits = ( CSol * * ) MyMalloc( 5031, pQuery->nEngineVectors * sizeof( CSol * ) );
        pQuery->ppSurls = ( COneSurl * * ) MyMalloc( 5032, pQuery->nEngineVectors * sizeof( COneSurl * ) );
        if( pQuery->ppSolMore == NULL
        ||  pQuery->ppSolHits == NULL
        ||  pQuery->ppSurls == NULL )
        {
            ProgramError( L"RunOneQueryThread: ppSolMore/ppSolHits == NULL" );
            return;
        }

        size_t i = 0;
        for( ;; )
        {
            if( i == pQuery->nEngineVectors )
                break;
            pQuery->ppSolMore[i] = new CSol( CSOL_SCALAR );
            pQuery->ppSolHits[i] = new CSol( CSOL_SCALAR );
            if( pQuery->ppSolMore[i] == NULL
            ||  pQuery->ppSolHits[i] == NULL )
            {
                ProgramError( L"RunOneQueryThread: ppSolMore[i]/ppSolHits[i] == NULL" );
                return;
            }

            // Oops, I left the ppSurls vector unfilled!
            // Oh, no, I will do that in traverse below.

            // This loop has created identical new objects,
            // so there is no issue of orders not matching.

            i ++;
        }
    }

    // Now change the original traverse from doing queries,
    // to adding each query URL into one slot of ppSolMore.
    // This loop iterates list of SEARCH ( not FORM ) urls.
    // The ordinal is found in ascii prefixing the FullKey.
    // CUrl has made each entry's User.pVoid point to Surl.

    // For some reason, the first query URL tried has pSurl
    // pointing to last search engine ( 295, bigblog ) while
    // using the expected URL of first engine ( 101, google ).
    // Aha! I didn't increment iFillGinVect during the loop.
    // Empty slots above #0 would also explain later errors.

    CoIt * pMalVector = CSolSearchUrls.GetSortedVector( CSOL_FORWARD );
    if( pMalVector != NULL )
    {
        size_t iFillGinVect = 0;
        size_t take = 0;
        for( ;; )
        {
            CoIt * pCoIt = pMalVector + take++;
            if( pCoIt->IsSentinel )
                break;

            wchar_t * FullKey = CoItFullKey( pCoIt );
            if( FullKey != NULL )
            {
                // Extract and skip 3 digits and space atop key.
                // No need, it is in each pOneSurl->GetOrdinal.

                pQuery->ppSurls[iFillGinVect] = ( COneSurl * ) pCoIt->User.pVoid;
                if( pQuery->ppSurls[iFillGinVect] == NULL )
                {
                    ProgramError( L"pQuery->ppSurls[iFillGinVect] == NULL" );
                    return;
                }

                CWsb Joined;
                Joined.Add( FullKey + 4 ); // Query URL after the ordinal
                Joined.Add( szTrimmed );
                size_t nMalUrl = 0;
                wchar_t * pMalUrl = Joined.GetBuffer( & nMalUrl ); // a malloc, user frees
                if( pMalUrl == NULL )
                {
                    ProgramError( L"Joined.GetBuffer( & nMalUrl ) == NULL" );
                    return;
                }

                // For HITS, I plan to hang each URL, and format
                // an index for holding in the ppSolHits[n], but
                // for MORE, let's just toss the first, suffixed,
                // read-to-use query URL string into ppSolMore[n].
                // It will be all alone during first loop to fetch.

                int index = pQuery->ppSolMore[iFillGinVect]->AddKey( pMalUrl );
                #if DO_DEBUG_ADDFIND
                    if( index <= 1 )
                        { Spew( L"AddFind 1 at cwww 5101" ); }
                #endif
                #if DO_DEBUG_NEWQUERY
                    SpewValue( L"RunOneQueryThread initginvects QueryIndex", QueryIndex );
                    SpewValue( pMalUrl, index );
                    SpewTwo( L"pQuery->ppSurls[iFillGinVect]->szDomainName", pQuery->ppSurls[iFillGinVect]->szDomainName );
                    SpewValue( L"pQuery->ppSurls[iFillGinVect]->GetOrdinal", pQuery->ppSurls[iFillGinVect]->GetOrdinal );
                #endif

                MyFree( 5154, zx, pMalUrl );
                pMalUrl = NULL;

                MyFree( 5156, UNPREDICTABLE, FullKey );
                FullKey = NULL;
            }
            iFillGinVect ++;
        }
        MyFree( 4322, UNPREDICTABLE, pMalVector );
        pMalVector = NULL;
    }
    // Having prepared all first URLs, I all done with the query string.
    MyFree( 2595, zx, pMalUserQry );
    pMalUserQry = NULL;
    szTrimmed = NULL; // not a malloc

    // I thought to provide some kind of bools to skip over
    // levels that already ran to completion, but just let
    // them iterate, which will fetch no URLs again, and
    // first loops never omit to fill URLs into Mores/Hits.

    pQuery->pWsbResultText->Add( L"\r\nPass one: Initial results from each search engine.\r\n\r\n" );

    {
        size_t iSEV = 0;
        for( ;; )
        {
            if( iSEV == pQuery->nEngineVectors )
                break;

            if( pQuery->m_StopThisThread
            || gbMallocLimitExceeded
            || g_bStopAllThreads )
                break;

            wchar_t * pMalUrlOne = pQuery->ppSolMore[iSEV]->GetFullKey( 2 ); // First URL = #2
            if( pMalUrlOne == NULL )
            {
                ProgramError( L"RunOneQueryThread: pMalUrlOne == NULL" );
                return;
            }

            #if DO_DEBUG_NEWQUERY
                SpewValue( L"RunOneQueryThread PassOne QueryIndex", QueryIndex );
                SpewTwo( L"pMalUrlOne", pMalUrlOne );
                SpewTwo( L"pQuery->ppSurls[iSEV]->szDomainName", pQuery->ppSurls[iSEV]->szDomainName );
                SpewValue( L"pQuery->ppSurls[iSEV]->GetOrdinal", pQuery->ppSurls[iSEV]->GetOrdinal );
            #endif

            PerformOneQuery( 1, pQuery, pMalUrlOne, pQuery->ppSurls[iSEV], pQuery->ppSolMore[iSEV], pQuery->ppSolHits[iSEV] );

            MyFree( 5228, UNPREDICTABLE, pMalUrlOne );
            iSEV ++;
        }
    }

    // Next, perform Diligence 2 and higher loops.
    // This time, traverse all of the mores.

    size_t QRPNo = 1; // All Page No. 1 were already done.

    for( ;; )
    {
        if( pQuery->m_StopThisThread
        || gbMallocLimitExceeded
        || g_bStopAllThreads )
            break;

        if( ++QRPNo > pQuery->m_QueryDiligence )
            break;
        pQuery->pWsbResultText->Add( L"\r\n\r\nRequesting another round of query result pages.\r\n\r\n" );

        #if DO_DEBUG_NEWQUERY
            SpewValue( L"RunOneQueryThread: Starting on QRPage", QRPNo );
        #endif

        int FoundAnyToDo = 0;
        size_t iSEV = 0;
        for( ;; )
        {
            if( iSEV == pQuery->nEngineVectors )
                break;

            if( pQuery->m_StopThisThread
            || gbMallocLimitExceeded
            || g_bStopAllThreads )
                break;

            // Usually, continuation URLs would sort forward as I wish,
            // having incrementing parameters like 1,2,3, or 10,20,30.

            // This traverse loop will go through them all in order,
            // but skip over any already processed ( non-NULL in AllUrls ),
            // and do the first one not yet done ( NULL in AllUrls ),
            // then break, and go around all other s.e. before resuming.

            #if DO_DEBUG_NEWQUERY
                SpewValue( L"RunOneQueryThread: Doing SEVectorIndex", iSEV );
            #endif

            CoIt * pMalVector2 = pQuery->ppSolMore[iSEV]->GetSortedVector( CSOL_FORWARD );
            if( pMalVector2 != NULL )
            {
                size_t take2 = 0;
                for( ;; )
                {
                    if( pQuery->m_StopThisThread
                    || gbMallocLimitExceeded
                    || g_bStopAllThreads )
                        break;

                    CoIt * pCoIt2 = pMalVector2 + take2++;
                    if( pCoIt2->IsSentinel )
                        break;
                    wchar_t * pMalUrlOthers = CoItFullKey( pCoIt2 );

                    if( pMalUrlOthers == NULL )
                    {
                        ProgramError( L"RunOneQueryThread: pMalUrlOthers == NULL" );
                        return;
                    }

                    int skipUrl = 0; // not continue, misses a free

                    int MoreUrlIndex = CSolAllUrls.Find( pMalUrlOthers );
                    // I may get back 1==TAIL, meaning not found. Never 0=HEAD.
                    #if DO_DEBUG_ADDFIND
                        if( MoreUrlIndex < 1 )
                            { Spew( L"AddFind 0 at cwww 5303" ); }
                    #endif
                    if( MoreUrlIndex > 1 )
                    {
                        // This URL is listed. See if it was processed.
                        void * pVoid = CSolAllUrls.GetUserpVoid( MoreUrlIndex );
                        if( pVoid != NULL )
                        {
                            // This URL was processed.
                            skipUrl = 1; // not continue, misses a free
                        }
                    }
                    if( ! skipUrl )
                    {
                        // Do it! Do it now!

                        #if DO_DEBUG_NEWQUERY
                            SpewValue( L"RunOneQueryThread PassTwo QueryIndex", QueryIndex );
                            SpewTwo( L"pMalUrlOthers", pMalUrlOthers );
                            SpewTwo( L"pQuery->ppSurls[iSEV]->szDomainName", pQuery->ppSurls[iSEV]->szDomainName );
                            SpewValue( L"pQuery->ppSurls[iSEV]->GetOrdinal", pQuery->ppSurls[iSEV]->GetOrdinal );
                        #endif

                        PerformOneQuery( QRPNo, pQuery, pMalUrlOthers, pQuery->ppSurls[iSEV], pQuery->ppSolMore[iSEV], pQuery->ppSolHits[iSEV] );
                        FoundAnyToDo = 1;
                    }

                    MyFree( 5265, UNPREDICTABLE, pMalUrlOthers );
                    pMalUrlOthers = NULL;

                    if( ! skipUrl )
                    {
                        // We did one. Go around to others first.
                        break;
                    }
                }
                MyFree( 4057, UNPREDICTABLE, pMalVector2 );
                pMalVector2 = NULL;
            }
            iSEV ++;
        }
        if( ! FoundAnyToDo )
            break;
    }

    pQuery->pWsbResultText->Add( L"\r\nThread ended.\r\n" );
    // obs: Top.UpdateViewIfOnScreen( pQuery );

    // After the search thread ends, formulate a revision
    // of the whole result log that will list results for
    // this search by some kind of priority, perhaps with
    // an embedded KWIC display of the hit lines, scores
    // for all the search engines, etc...


}

void CWww::PerformOneQuery( int QRPNo, COneQuery * pQuery, wchar_t * szUrl, COneSurl * pOneSurl, CSol * pSolMore, CSol * pSolHits )
{
    // It is my caller that will do any iteration over continuation urls.
    // However, I follow any links out of each current query result page.

    #if DO_DEBUG_QUERY || DO_DEBUG_REJECTS
        ; SpewTwo( L"PerformOneQuery using", szUrl );
        ; SpewValue( L"PerformOneQuery pOneSurl->GetOrdinal", pOneSurl->GetOrdinal );
    #endif

    #if DO_DEBUG_FOLLOW
        ; SpewTwo( L"\r\n======= PerformOneQuery: szUrl =======", szUrl );
    #endif

    #ifdef _WIN32_WCE
        int t1 = GetTickCount( ) / 1000;
    #else // not _WIN32_WCE
        time_t t1 = time( NULL );
    #endif // _WIN32_WCE

    {
        wchar_t wk[200];
        wsprintf( wk, L"\r\nFetching query result page %2d from search engine %03d (%s).\r\n\r\n",
            QRPNo,
            pOneSurl->GetOrdinal,
            pOneSurl->szDomainName );
        pQuery->pWsbResultText->Add( wk );
    }

    size_t UrlIndex = CSolAllUrls.AddKey( szUrl );
    #if DO_DEBUG_ADDFIND
        if( UrlIndex == 1 )
            { Spew( L"AddFind 1 at cwww 1088" ); }
    #endif

    #if DO_DEBUG_QUERY
        ; SpewValue( L"Starting on Query UrlIndex", UrlIndex );
    #endif

    CSol * pSolFrames = new CSol( CSOL_SCALAR );

    int PreFram = pSolFrames -> nList - 2;
    int PreMore = pSolMore   -> nList - 2;
    int PreHits = pSolHits   -> nList - 2;

    // SED_PILE1
    // from CWww PerformOneQuery
    // This call fetches each assembled query URL from some search engine:
    // RecursiveGetter will not fetch framesets, because QRPOrdinal != 0.
    RecursiveGetter(
        ZEROETH,                            // int Recursions
        UrlIndex,                           // size_t UrlIndex
        pQuery,                             // CBud * pBudLog
        pQuery->m_NonTextPath,              // wchar_t * szBinaryPath
        pSolFrames,                         // CSol * pSolFrames
        pSolMore,                           // CSol * pSolMores
        pSolHits,                           // CSol * pSolHits
        pOneSurl->GetOrdinal,               // int FromEngineNo
        YES_PAGE_IS_A_QRP,                  // int PageIsAQrp
        szUrl,                              // wchar_t * szQueryResultUrl
        YES_TOP,                            // int bTopUrl
        pOneSurl );                         // COneSurl * pOneSurl
    // SED_PILE2

    #ifdef _WIN32_WCE
        int t2 = GetTickCount( ) / 1000;
    #else // not _WIN32_WCE
        time_t t2 = time( NULL );
    #endif // _WIN32_WCE

    int PostFram = pSolFrames -> nList - 2;
    int PostMore = pSolMore   -> nList - 2;
    int PostHits = pSolHits   -> nList - 2;

    {
        int elapsed = ( int ) ( t2 - t1 );
        wchar_t wk[400];
        wsprintf( wk, L"\r\nQuery result page %2d from search engine %03d (%s) produced %2d hit URLs, %d continuation URLs in %d seconds.\r\n\r\n",
            QRPNo,
            pOneSurl->GetOrdinal,
            pOneSurl->szDomainName,
            PostHits - PreHits,
            PostMore - PreMore,
            elapsed );
        pQuery->pWsbResultText->Add( wk );
    }

    if( PostHits - PreHits > 0 )
    {
        pQuery->pWsbResultText->Add( L"\r\nNow fetching the selected web pages.\r\n\r\n" );

        CoIt * pMalVector2 = pSolHits->GetSortedVector( CSOL_FORWARD );
        if( pMalVector2 != NULL )
        {
            size_t take2 = 0;
            for( ;; )
            {
                CoIt * pCoIt2 = pMalVector2 + take2++;
                if( pCoIt2->IsSentinel )
                    break;

                if( pQuery->m_StopThisThread
                || gbMallocLimitExceeded
                || g_bStopAllThreads )
                    break;

                wchar_t * FullKey2 = CoItFullKey( pCoIt2 );
                // This key was an itoa of an URL index.
                // Recover URL, see if novel, fetch it ( with frames ).

                int HitUrlIndex = _wtoi( FullKey2 );

                void * pVoid = CSolAllUrls.GetUserpVoid( HitUrlIndex );
                if( pVoid == NULL )
                {
                    // This URL has not yet been processed.

                    CSol * pSolHitFrames = new CSol( CSOL_SCALAR );
                    // SED_PILE1
                    // from CWww RunOneFetchThread
                    // This call is for some Hit URL found during a search.
                    // The ordinal is passed to cross ref links "FROM s.e."
                    RecursiveGetter(
                        ZEROETH,                    // int Recursions
                        HitUrlIndex,                // size_t UrlIndex
                        pQuery,                     // CBud * pBudLog
                        pQuery->m_NonTextPath,      // wchar_t * szBinaryPath
                        pSolHitFrames,              // CSol * pSolFrames
                        NO_MORES,                   // CSol * pSolMores
                        NO_HITS,                    // CSol * pSolHits
                        pOneSurl->GetOrdinal,       // int FromEngineNo
                        ZERO_ORDINAL_FOR_NON_QRP,   // int PageIsAQrp
                        NO_QRURL,                   // wchar_t * szQueryResultUrl
                        NOT_TOP,                    // int bTopUrl
                        NO_SURL );                  // COneSurl * pOneSurl
                    // SED_PILE2

                    delete pSolHitFrames;
                    pSolHitFrames = NULL;

                    MyFree( 5491, zx, FullKey2 );
                    FullKey2 = NULL;
                }
                else
                {
                    // This URL has already been processed.
                    // HOWSOMEVER!
                    // I want my search engine to get credit for finding it too.
                    // If it has a paper hanging, I can add it to list.
                    // If it has no paper hanging, I'm S.O.L.
                    if( pVoid < PVOID_VALID_BELOW )
                    {
                        COnePaper * pOnePaper = ( COnePaper * ) pVoid;
                        if( pOnePaper->pIntFromEngine == NULL )
                        {
                            pOnePaper->pIntFromEngine = new CInt( );
                        }
                        pOnePaper->pIntFromEngine->Add( pOneSurl->GetOrdinal );
                    }
                }
            }
            MyFree( 5494, UNPREDICTABLE, pMalVector2 );
            pMalVector2 = NULL;
        }
    }

    delete pSolFrames;
    pSolFrames = NULL;
}

int CWww::RejectableQueryUrl( wchar_t * szPageUrl, wchar_t * szHitUrl )
{
    // The idea here is to reject all the listed search engine's
    // internal pages, or cache pages, etc, such as the following:
    // http://www.altavista.com/image/default
    // http://72.14.253.104/search?q=cache:

    // caller warrants:
    // szPageUrl != NULL
    // szHitUrl != NULL

    // szPageUrl will be tested against a list of search result domains,
    // such as altavista.com, which find partial match as by strstr.
    // Subsorted within each result domain URL is whole URL strings,
    // e.g., under altavista.com, maybe altavista.com/image/default.

    // It must be a partial domain match, due to variable host cpus.
    // E.g., http://s16.us.ixquick.com/do/metasearch.pl...

    // I need a way to specify a match to any value of IP domain:
    // http://72.14.253.104/search?q=cache:
    // Let a domain part of L"#" match any purely numeric IP domain.

    // I need a way to specify reject ALL urls under some domains,
    // for example, if a no-matches found comes to different pages.
    // Let a second string part of L"*" skip the second match test.
    // In that case, it should be the only part2 for such a part1.
    //
    // Actually, "*" in part 1, and matching domain in part 2 works.
    // So I do not need to look for "*" in part 2 as stated above.
    // However, I see both "*" in part 1 or part 2 are implemented.

    // I need a way to specify reject an empty path part too:
    // http://www.altavista.com/
    // However valid hits also may have empty path parts.
    // Let a domain part of L"/" apply for all empty path part urls.
    // So I will need to add a "/" entry for all search engines,
    // and for other nuisance domains that appear with empty path.

    // Because of "*", "#", and "/" keys, traverse must ignore them.

    // This is growing unplanned like weeds. Pray for more insight.

    #if DO_DEBUG_REJECTS || DO_DEBUG_FOLLOW
        ; Spew( L"RejectableQueryUrl: test this HitUrl from this Query PageUrl:" );
        ; SpewTwo( L"szPageUrl", szPageUrl );
        ; SpewTwo( L"szHitUrl", szHitUrl );
    #endif

    // Step 1: parse szPageUrl for authority. Notice if purely numeric.

    // Is it fair to assume any valid domain is under 100 chars?
    // Yea. Case insensitive. Allow for http: or https: or any:.
    int AlphaInPageDomain = 0;
    {
        wchar_t Domain[100];
        wchar_t * into = Domain;
        wchar_t * from = szPageUrl;

        // find "://"
        for( ;; )
        {
            if( *from == NULL )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA1" );
                #endif
                return 1; // cya
            }
            if( *from == ':' )
                break;
            from++;
        }
        if( from[1] != '/'
        ||  from[2] != '/' )
        {
            #if DO_DEBUG_REJECTS
                ; Spew( L"CYA2" );
            #endif
            return 1; // cya
        }
        from += 3;
        // copy until before / or null
        for( ;; )
        {
            wchar_t wc = *from++;
            if( wc > 0x7f )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA3" );
                #endif
                return 1; // cya
            }
            if( wc == NULL )
                break;
            if( wc == '/' )
                break;
            if( isalpha( wc ) )
            {
                AlphaInPageDomain = 1;
                if( isupper( wc ) )
                    wc |= 0x20; // ASCII lowercase
            }
            *into++ = wc;
            if( into == Domain + 100 - 2 )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA4" );
                #endif
                return 1; // cya
            }
        }
        *into = NULL;

        #if DO_DEBUG_REJECTS
            ; Spew( L" - - - - - " );
            if( AlphaInPageDomain )
            {
                ; SpewTwo( L"Found alphas in Query Page Domain", Domain );
            }
            else
            {
                ; SpewTwo( L"Purely Numeric IP Query Page Domain", Domain );
            }
        #endif
    }

    // Step 2: parse szHitUrl. Notice if purely numeric, or if pathless.

    int AlphaInHitDomain = 0;
    int PathPartIsEmpty = 0;
    {
        // Is it fair to assume valid HIT domain is under 200 chars?
        // Yea. Case insensitive. Allow for http: or https: or any:.
        wchar_t Domain[200];
        wchar_t * into = Domain;
        wchar_t * from = szHitUrl;

        // find "://"
        for( ;; )
        {
            if( *from == NULL )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA5" );
                #endif
                return 1; // cya
            }
            if( *from == ':' )
                break;
            from++;
        }
        if( from[1] != '/'
        ||  from[2] != '/' )
        {
            #if DO_DEBUG_REJECTS
                ; Spew( L"CYA7" );
            #endif
            return 1; // cya
        }
        from += 3;
        // copy until before / or null
        for( ;; )
        {
            wchar_t wc = *from++;
            if( wc > 0x7f )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA8" );
                #endif
                return 1; // cya
            }
            if( wc == NULL )
            {
                PathPartIsEmpty = 1;
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA9" );
                #endif
                return 1; // cya
            }
            if( wc == '/' )
            {
                if( *from == NULL )
                    PathPartIsEmpty = 1;
                break;
            }
            if( isalpha( wc ) )
            {
                AlphaInHitDomain = 1;
                if( isupper( wc ) )
                    wc |= 0x20; // ASCII lowercase
            }
            *into++ = wc;
            if( into == Domain + 200 - 2 )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA10" );
                #endif
                return 1; // cya
            }
        }
        *into = NULL;

        #if DO_DEBUG_REJECTS
            if( PathPartIsEmpty
            || ! AlphaInHitDomain )
            {
                if( PathPartIsEmpty )
                {
                    ; SpewTwo( L"Path part is empty in Hit Url", Domain );
                }
                if( ! AlphaInHitDomain )
                {
                    ; SpewTwo( L"Purely Numeric IP in Hit Url", Domain );
                }
            }
            else
            {
                ; SpewTwo( L"Normal alpha+path in Hit Url", Domain );
            }
        #endif
    }

    // Step 3: for empty hit URL path part, test to special "/" list.

    if( PathPartIsEmpty )
    {
        #if DO_DEBUG_REJECTS
            ; Spew( L"Hit Url path part is empty, so test to / list" );
        #endif

        // Have "/" left key to apply for empty hit URL path part.

        size_t index = CSolRejectUrls.Find( L"/" );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cwww 4810" ); }
        #endif
        if( index != 0 )
        {
            // A "/" entry was in CSolRejectUrls. It is for empty pathparts.
            // See if this pathpartless hit URL domain is listed therein
            CSol * pSol = ( CSol * ) CSolRejectUrls.GetUserpVoid( index );
            if( pSol == NULL )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA21" );
                #endif
                return 1; // cya
            }

            #if DO_DEBUG_REJECTS
                ; Spew( L"Test URL to / domain entry." );
            #endif

            if( RejectableHitUrl( pSol, szHitUrl, 1 ) ) // 1=domain must match to any dot
                return 1;
        }
        // If not listed, no more tests; pathpartless hit url is acceptable.
        #if DO_DEBUG_REJECTS
            ; Spew( L"pathpartless hit url is acceptable." );
        #endif
        // But don't skip later tests! ... return 0;
    }

    // Step 4: for numeric IP addr, see if hit URL is in special "#" list.
    // Let "#" list do double duty for either page or hit numeric domain.

    if( ! AlphaInHitDomain
    || ! AlphaInPageDomain )
    {
        #if DO_DEBUG_REJECTS
            ; Spew( L"Query page or Hit URL was numeric IP addr, so test to # list" );
        #endif

        // Have "#" left key to apply for numeric page or hit URL domain.

        size_t index = CSolRejectUrls.Find( L"#" );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cwww 4660" ); }
        #endif

        if( index == 0 )
        {
            #if DO_DEBUG_REJECTS
                ; Spew( L"No # domain entry in lists" );
            #endif
            // But don't skip later tests! ... Just ELSE it...
            // return 0; // no "#" domain entry was defined. Ok url.
        }
        else
        {

            // A "#" entry was in CSolRejectUrls. It matches any IP addr.
            CSol * pSol = ( CSol * ) CSolRejectUrls.GetUserpVoid( index );
            if( pSol == NULL )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA11" );
                #endif
                return 1; // cya
            }

            #if DO_DEBUG_REJECTS
                ; Spew( L"Test URL to # domain entry." );
            #endif

            if( RejectableHitUrl( pSol, szHitUrl, 0 ) )
                return 1;

            // If not listed, no more tests; numeric IP hit url is acceptable.
            #if DO_DEBUG_REJECTS
                ; Spew( L"pathpartless hit url is acceptable." );
            #endif
            // But don't skip later tests! ... return 0;
        }
    }

    // Step 5: from any page URL, see if hit URL is in special "*" list.

    {
        #if DO_DEBUG_REJECTS
            ; Spew( L"Test all Hit URLs against * list" );
        #endif

        size_t index = CSolRejectUrls.Find( L"*" );
        #if DO_DEBUG_ADDFIND
            if( index == 1 )
                { Spew( L"AddFind 1 at cwww 4860" ); }
        #endif
        if( index != 0 )
        {
            // A "*" entry was in CSolRejectUrls. It matches any domain.
            // See if this hit URL domain is listed therein
            CSol * pSol = ( CSol * ) CSolRejectUrls.GetUserpVoid( index );
            if( pSol == NULL )
            {
                #if DO_DEBUG_REJECTS
                    ; Spew( L"CYA21" );
                #endif
                return 1; // cya
            }

            #if DO_DEBUG_REJECTS
                ; Spew( L"Test URL to * domain entry." );
            #endif

            if( RejectableHitUrl( pSol, szHitUrl, 1 ) ) // 1=domain must match to any dot
                return 1;
        }
        // If not listed, there are still more tests to do...
    }

    // Step 6: finally, the double traversal of all top, all sub CSols.

    {
        // szPageUrl may match than one domain? Not likely. Allow for it.

        #if DO_DEBUG_REJECTS
            ; Spew( L"Test all Hit URLs against double-level lists" );
        #endif

        int rejected = 0;

        CoIt * pMalVector = CSolRejectUrls.GetSortedVector( CSOL_FORWARD );
        if( pMalVector == NULL )
        {
            #if DO_DEBUG_REJECTS
                ; Spew( L"No reject entries2. Ok url." );
            #endif
            return 0; // conceivably, no reject entries were defined. Ok url.
        }
        size_t take = 0;
        for( ;; )
        {
            CoIt * pCoIt = pMalVector + take++;
            if( pCoIt->IsSentinel )
                break;
            wchar_t * pMalTest = CoItFullKey( pCoIt );
            int matched = 0;
            if( pMalTest != NULL )
            {
                // Because of the special keys, skip any key of 1 char.
                if( pMalTest[1] != NULL )
                {
                    #if DO_DEBUG_REJECTS
                        ; SpewValue( pMalTest, 5555 );
                    #endif
                    matched = ( int ) wcsstr( szHitUrl, pMalTest ); // 0 if no Test in Hit
                }
                MyFree( 4691, zx, pMalTest );
                pMalTest = NULL;
            }
            if( matched )
            {
                // match of domain is not sufficient.
                // test for match to any test string.
                // I cannot return RejectableHitUrl until after my frees.

                #if DO_DEBUG_REJECTS
                    ; Spew( L"domain matched" );
                #endif

                CSol * pSol = ( CSol * ) pCoIt->User.pVoid;
                if( pSol == NULL )
                {
                    #if DO_DEBUG_REJECTS
                        ; Spew( L"CYA12" );
                    #endif
                    return 1; // cya
                }
                rejected = RejectableHitUrl( pSol, szHitUrl, 0 );
                if( rejected )
                {
                    break;
                }
            }
        }

        MyFree( 4695, UNPREDICTABLE, pMalVector );
        pMalVector = NULL;
        if( rejected )
        {
            #if DO_DEBUG_REJECTS
                ; Spew( L"1st & 2nd strstr matched, reject URL" );
            #endif
            return 1; // 1st & 2nd strstr matched, reject URL
        }
    }

    return 0;
}

int CWww::RejectableHitUrl( CSol * pSol, wchar_t * szHitUrl, int ToAnyDot )
{
    // Passed pSol was hanging from matching item in CSolRejectUrls.
    // Return 1 if pSol contains a full key of "*".
    // Return 1 if any full key in pSol matches ( strstr ) szHitUrl.

    #if DO_DEBUG_REJECTS
        ; SpewTwo( L"Examine szHitUrl", szHitUrl );
    #endif

    size_t index = pSol->Find( L"*" ); // This is a RIGHT-HAND "*" key
    #if DO_DEBUG_ADDFIND
        if( index == 1 )
            { Spew( L"AddFind 1 at cwww 4753" ); }
    #endif

    if( index != 0 )
    {
        #if DO_DEBUG_REJECTS
            ; Spew( L"This domain lists *, so reject all." );
        #endif
        return 1;
    }

    int rejected = 0;

    CoIt * pMalVector = pSol->GetSortedVector( CSOL_FORWARD );
    if( pMalVector == NULL )
    {
        #if DO_DEBUG_REJECTS
            ; Spew( L"No reject entries3. Ok url." );
        #endif
        return 0; // I'll never get here with a top and no sub item.
    }
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;
        wchar_t * pMalTest = CoItFullKey( pCoIt );
        int matched = 0;
        if( pMalTest != NULL )
        {
            // Because of the special keys, skip any key of 1 char.
            if( pMalTest[1] != NULL )
            {
                // After did right-hand "*", skipping 1-letter right-hands,
                // If the "right-hand" string is found anywhere in the URL,
                // the URL is rejectable.

                // But for the left-hand "/" case, ToAnyDot=1, expect that
                // the match must start right after "://", or after any ".".

                wchar_t * pMatch = wcsstr( szHitUrl, pMalTest ); // 0 if no Test in Hit
                if( pMatch != NULL )
                {
                    if( ToAnyDot )
                    {
                        // So pMatch is at the first character of match.
                        // CYA to test back of pointer as far as [-2]

                        #if DO_DEBUG_TOANYDOT
                            ; Spew( L"To Any Dot:" );
                            ; SpewTwo( L"pMalTest", pMalTest );
                            ; SpewTwo( L"szHitUrl", szHitUrl );
                        #endif

                        if( pMatch < szHitUrl + 3 )
                        {
                            // test not.
                        }
                        else if( pMatch [-1] == '/'
                        && pMatch [-2] == '/'
                        && pMatch [-3] == ':' )
                        {
                            #if DO_DEBUG_TOANYDOT
                                ; Spew( L"Matched after ://" );
                            #endif
                            matched = 1;
                        }
                        else if( pMatch [-1] == '.' )
                        {
                            #if DO_DEBUG_TOANYDOT
                                ; Spew( L"Matched after dot" );
                            #endif
                            matched = 1;
                        }
                        else
                        {
                            #if DO_DEBUG_TOANYDOT
                                ; Spew( L"match imperfect" );
                            #endif
                        }
                    }
                    else
                    {
                        matched = 1;
                    }

                }



            }
            #if DO_DEBUG_REJECTS
                ; SpewValue( pMalTest, ( matched ? 7777777 : 0 ) );
            #endif
            MyFree( 4691, zx, pMalTest );
            pMalTest = NULL;
            if( matched )
            {
                rejected = 1;
                break; // not return, until after frees
            }
        }
    }
    MyFree( 4695, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;
    return rejected; // if 2nd strstr matched, reject URL
}

void CWww::MarkAsRejectedUrl( size_t UrlIndexToClaim )
{
    // 1. Must first "claim" the URL, else do nothing.
    // 2. Reclaim URL with the new pVoid marker value.

    if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_CLAIMING ) )
        return;
    if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_QUERYREJECT ) )
        ProgramError( L"MarkAsRejectedUrl: ! Release claim" );
}

int CWww::TestPaperHttpHeaderValuesAcceptable( COnePaper * pOnePaper, CWsb * pWsbCritique, int BinaryOkay )
{
    // Examine members of pOnePaper set during ParseHttpHeader.
    // Tell Add Cache or Add Fetch if this page should be okay.

    // Called from LoadInternetCacheItem.
    // Called from ObtainOneURLContent for both fetch and query.

    int Okay = 1;

    if( pOnePaper->HttpHeaderContentEncoding != ENCODING_NONE )
    {
        // This content is not okay. ( e.g., gzip encoding...? )
        #if DO_DEBUG_CACHE
            ; Spew( L"Not okay: ContentEncoding != ENCODING_NONE" );
        #endif
        pWsbCritique->Add( L"Not okay: ContentEncoding != ENCODING_NONE\r\n" );
        Okay = 0;
    }

    if( pOnePaper->HttpHeaderStatus != 200 )
    {
        // This content is not okay. Caller may follow 3xx redirects.
        #if DO_DEBUG_CACHE
            ; Spew( L"Not okay: Status != 200" );
        #endif
        wchar_t msg[] = L"Not okay: Status = xxx\r\n";
        msg[ 19 ] = pOnePaper->HttpHeaderStatus / 100 % 10 + '0';
        msg[ 20 ] = pOnePaper->HttpHeaderStatus / 10 % 10 + '0';
        msg[ 21 ] = pOnePaper->HttpHeaderStatus % 10 + '0';
        pWsbCritique->Add( msg );
        Okay = 0;
    }

    if( pOnePaper->NewLocationIndex != 0 )
    {
        wchar_t * pNewUrl = CSolAllUrls.GetFullKey( pOnePaper->NewLocationIndex ); // a malloc, user frees
        #if DO_DEBUG_CACHE
            ; Spew( L"Redirection:" );
            ; Spew( L"pNewUrl" );
        #endif
        pWsbCritique->Add( L"Location: " );
        pWsbCritique->Add( pNewUrl );
        pWsbCritique->Add( L"\r\n" );
        MyFree( 2255, zx, pNewUrl );
        pNewUrl = NULL;
    }

    switch( pOnePaper->HttpHeaderContentType )
    {
        case CONTENT_UNKNOWN:
        case CONTENT_OTHER:
            // This content is not okay.
            #if DO_DEBUG_CACHE
                ; Spew( L"Not okay: ContentType = non-text" );
            #endif
            if( BinaryOkay )
            {
                pWsbCritique->Add( L"ContentType = non-text. Saving file.\r\n" );
            }
            else
            {
                pWsbCritique->Add( L"Not okay: ContentType = non-text\r\n" );
                Okay = 0;
            }
            break;
        case CONTENT_RTFTEXT:
        case CONTENT_OTHERTEXT:
            // This content is not okay.
            #if DO_DEBUG_CACHE
                ; Spew( L"Not okay: ContentType = other-text" );
            #endif
            if( BinaryOkay )
            {
                pWsbCritique->Add( L"ContentType = other-text. Saving file.\r\n" );
            }
            else
            {
                pWsbCritique->Add( L"Not okay: ContentType = other-text\r\n" );
                Okay = 0;
            }
            Okay = 0;
            break;

        case CONTENT_TEXT:
        case CONTENT_HTMLTEXT:
        case CONTENT_PLAINTEXT:
            #if DO_DEBUG_CACHE
                ; Spew( L"It is text, html or plain" );
            #endif
            // This content is okay.
            break;
    }
    return Okay; // 1 = content is okay.
}

