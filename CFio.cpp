// This is file: CFio.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

CFio::CFio( )
{
    #if DO_DEBUG_CALLS
        Routine( L"126" );
    #endif
}
CFio::~CFio( )
{
    #if DO_DEBUG_CALLS
        Routine( L"127" );
    #endif
}

int CFio::MakeFoldersPaths( wchar_t * szFolderPath )
{

    #if DO_DEBUG_CALLS
        Routine( L"129" );
    #endif

    // This serves Save Directory, or Use Directory,
    // so it will create all the path parts.

    // First, clone here the path string format verification:
    // I will put up a UserError message box if not acceptable.

    // Verify that the user string does not contain metacharacters ( *,? ).
    // Also note whether path has final \, ensure has one if is at root.
    // In fact, let's copy it into a MAX_PATH buffer, all my api can use.

    // user string might be relative or absolute.
    // Test string for the following formats:
    // rel:     file...
    // UNC:     \\server\file...
    // PPC      \My Documents\file...
    // Full:    x:\file...

    // Windows CE does not allow the following characters in a file or directory name:
    // '*' , '\\', '/','?', '>', '<', ':', '"', '|', and any character less than 32.

    #if DO_DEBUG_PATH
        SpewTwo( L"Examining", szFolderPath );
    #endif

    wchar_t AutoPath [ MAX_PATH ];
    int InvalidPath = 0;
    int UNCPath = 0;

    wchar_t * from = szFolderPath;
    wchar_t * into = AutoPath;
    wchar_t * PastDrive = AutoPath;
    wchar_t * LastPathSep = NULL;
    wchar_t * RootPathSep = NULL;

    for( ;; )
    {
        wchar_t wc = *from++;
        if( wc == NULL )
        {
            #if DO_DEBUG_PATH
                Spew( L"At EOS" );
            #endif
            *into = NULL;
            break;
        }
        if( wc < 32 || wc > 255 )
        {
            #if DO_DEBUG_PATH
                Spew( L"wc < 32 || wc > 255" );
            #endif
            InvalidPath = 1;
            break;
        }
        // Only allow \ and : from invalid chars list.

        if( wc == '*'
        ||  wc == '/'
        ||  wc == '>'
        ||  wc == '<'
        ||  wc == '"'
        ||  wc == '|'
        ||  wc == '?' )
        {
            #if DO_DEBUG_PATH
                Spew( L"wc == filechar disallow" );
            #endif
            InvalidPath = 1;
            break;
        }

        if( wc == ':' )
        {
            #if DO_DEBUG_PATH
                Spew( L"wc == colon" );
            #endif

            #ifdef _WIN32_WCE
                // Disallow all colons on Pocket PC
                InvalidPath = 1;
                break;
            #else // not _WIN32_WCE
                // Allow only one colon at [1] offset: "X:\..."
                // And not after '\\'

                if( into - AutoPath != 1
                || AutoPath[0] == '\\' )
                {
                    InvalidPath = 1;
                    break;
                }
                PastDrive = AutoPath + 2;
            #endif // _WIN32_WCE

        }

        if( wc == '\\' )
        {
            #if DO_DEBUG_PATH
                Spew( L"wc == backslash" );
            #endif

            // What shall we ask about path separators?
            if( into == PastDrive )
            {
                #if DO_DEBUG_PATH
                    Spew( L"backslash at PastDrive" );
                #endif
                // e.g., \path, x:\path, \My Doc...
                // but also on first char of \\...
                RootPathSep = into;
            }

            if( UNCPath
            && RootPathSep == NULL )
            {
                #if DO_DEBUG_PATH
                    Spew( L"backslash 3 of UNC" );
                #endif
                RootPathSep = into; // set it further into UNC path
            }

            if( into == AutoPath + 1
            && RootPathSep != NULL )
            {
                #if DO_DEBUG_PATH
                    Spew( L"backslash 2 of UNC" );
                #endif
                UNCPath = 1; // e.g., \\server\path...
                RootPathSep = NULL; // available to set again above
            }
            LastPathSep = into;
        }

        *into = wc;
        if( ++into == AutoPath + sizeof( AutoPath )/sizeof( *AutoPath ) )
        {
            #if DO_DEBUG_PATH
                Spew( L"Past MAX_PATH" );
            #endif
            InvalidPath = 1;
            break;
        }

    }

    // Test for directory passes with or without final backslash.
    // Nobody sees my strip, but it affects empty path decision.

    #if DO_DEBUG_PATH
        SpewTwo( L"Prestrip", AutoPath );
    #endif

    // In fact, since FindFirstFile will fail on root path,
    // let's recognize that now to disallow it explicitly.

    // This test did not strip final \\ on UNC root path,
    // ( or did it? ) nor did it disallow a UNC root folder,
    // e.g., \\File_Server\Drive_P\ or w/o final backslash
    // and it should have... Ehhh, small fry.

    if( LastPathSep == into - 1 )
    {
        if( LastPathSep == RootPathSep )
        {
            UserError( L"The root folder is not allowed." );
            return 0;
        }
        else
        {
            #if DO_DEBUG_PATH
                Spew( L"strip final backslash" );
            #endif

            // The final char was \\. Do what?
            // Let's strip it, unless at root.
            --into;
            *into = NULL;
        }
    }

    #if DO_DEBUG_PATH
        SpewTwo( L"Poststrip", AutoPath );
    #endif

    if( into == PastDrive )
    {
        #if DO_DEBUG_PATH
            Spew( L"empty path" );
        #endif
        InvalidPath = 1; // empty path ( maybe after X: )
    }

    if( InvalidPath )
    {
        UserError( L"The path format is invalid." );
        return 0;
    }


    // hook new smarts into old scan code:

    wchar_t * scan = AutoPath;
    if( RootPathSep != NULL )
        scan = RootPathSep + 1;

    // scan is positioned to find any next path separator or null.

    // Later, I could improve this in CFio::MakeFoldersPaths:
    // I expect failure on first pathpart of UNC names, which IS root.

    for( ;; )
    {
        if( scan[0] == NULL )
        {
            // Try to create the final folder.
            if( ! MakeFoldersHelper( AutoPath ) )
                return 0; // failure
            break;
        }
        if( scan[0] == '/'
        || scan[0] == '\\' )
        {
            wchar_t hold = scan[0];
            scan[0] = NULL;
            if( ! MakeFoldersHelper( AutoPath ) )
                return 0; // failure
            scan[0] = hold;

            // In case of a final path separator, we're done.
            if( scan[1] == NULL )
                break;

        }
        scan ++;
    }
    return 1; // success
}

int CFio::MakeFoldersHelper( wchar_t * szFolderPath )
{
    #if DO_DEBUG_CALLS
        Routine( L"128" );
    #endif


    // to finish at some time - in CFio::MakeFoldersHelper:
    // When doing Add Folder, I should not do any mkdirs.
    // So be willing to only test for preexising dirs.


    // to fix at some time - in CFio::MakeFoldersHelper:
    // let the generic add folder paths supply a MAX_PATH test.


    // Try to create the folder: nonzero means okay; for zero test reason.
    if( CreateDirectory( szFolderPath, NULL ) != 0
    || GetLastError( ) == ERROR_ALREADY_EXISTS )
    {
        // Yes, but maybe it exists as a FILE instead of DIR!
        //
        // Now I see what steps elicit the "CreateFile 4" error:
        // I saved the log as \77 ( and did not add an extension )
        // I tried to save all as \77
        // The extant FILE probably evinces ERROR_ALREADY_EXISTS
        // So here I need to retest if it exists, is it a DIR?
        // Also ensure MKDIR failure case goes back to dialog.
        // Okay. Add a msg to failure.

        WIN32_FIND_DATA FindFileData;
        HANDLE hFindFile = FindFirstFile( szFolderPath, & FindFileData );
        if( hFindFile == INVALID_HANDLE_VALUE )
        {
            ProgramError( L"FindFirstFile 1" );
            return 0;
        }
        DWORD saved = FindFileData . dwFileAttributes;
        if( ! FindClose( hFindFile ) )
        {
            ProgramError( L"FindClose 2" );
        }
        if ( saved & FILE_ATTRIBUTE_DIRECTORY )
        {
            // that's ok. It exists, but it is a directory.
            return 1; // success
        }

        // Not ok. It exists, but it is NOT a directory.
        Say( L"Cannot create a folder. Path exists as a file." );
        return 0; // failure
    }
    else
    {
        // Show the error, but do not exit program.
        DWORD dwLastError = GetLastError( );

        HMODULE hModule = NULL; // default to system source
        DWORD dwBufferLength = 0;
        DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_FROM_SYSTEM;
        wchar_t * MessageBuffer = NULL; // receives the LocalAlloc.

        dwBufferLength = FormatMessage(
            dwFormatFlags,
            hModule,
            dwLastError,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
            ( wchar_t * ) & MessageBuffer, // type was mis-cast on purpose
            0,
            NULL );

        wchar_t wk[500];
        wchar_t * M2 = ( MessageBuffer == NULL ? L"FormatMessageFailed" : MessageBuffer );
        wsprintf( wk, L"CreateDirectory <%.256s>: %.200s", szFolderPath, M2 );
        UserError( wk );

        // Okay, I've seen this message box, while doing a Save All,
        // ( in fact, that's why I added it ) in sublanguage folder PT
        // that I guess was full on Windows Vista, with 13000 files.
        //
        // GLE just says "The directory or file cannot be created."

        if( MessageBuffer != NULL )
            LocalFree( MessageBuffer );

        return 0; // failure
    }
}

void CFio::SaveFolder( wchar_t * szFolderPath )
{
    #if DO_DEBUG_CALLS
        Routine( L"130" );
    #endif
    size_t TotalLength = 0;

    // When SaveFolder is called, wordex.cpp has already had a successful
    // return from calling MakeFoldersPaths, and szFolderPath now exists.

    // fix any time - test that SaveFolder path is way less than MAX_PATH.

    // Implementing a new idea to make subfolders based on paper language.

    size_t StrLenPath = wcslen( szFolderPath );
    if( StrLenPath > 200 ) // later, allow some math fraction of MAX_PATH
    {
        UserError( L"Folder path exceeds 200 chars." );
        return;
    }

    size_t nUsable = StrLenPath;

    if( szFolderPath[ StrLenPath - 1 ] == '/'
    || szFolderPath[ StrLenPath - 1 ] == '\\' )
    {
        nUsable = StrLenPath - 1;
    }

    wchar_t szTryFolderPath[ MAX_PATH + 1 ];
    memcpy( szTryFolderPath, szFolderPath, nUsable * sizeof( wchar_t ) );
    szTryFolderPath[ nUsable++ ] = '\\';
    szTryFolderPath[ nUsable ] = NULL; // Here I may append language acronym

    // In first loop, count the bytes of URLs with present content.
    {
        // Is this still fair to do?
        // After all my work on CoIt?
        // I don't need vector sorted.
        size_t Index = 2; // after 0=Head, 1=Tail.
        size_t Past = CSolAllUrls.nList;
        for( ;; )
        {
            if( Index >= Past )
                break;
            COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( Index );
            if( pOnePaper != NULL
            &&  pOnePaper < PVOID_VALID_BELOW )
            {
                TotalLength += pOnePaper->pWsbResultText->StrLen;
            }
            Index ++;
        }
    }

    if( TotalLength == 0 )
        return;

    // The Progress bar lower and upper value limit is 0 to 65K.
    size_t ScaleFactor = ( TotalLength + 0xffff ) / 0xffff; // 1 or more

    if( g_hWndSaveProgressBar != NULL )
    {
        size_t Upper = TotalLength / ScaleFactor;
        // If I do PostMessage, nothing happens, because no msg loop calls.
        SendMessage( g_hWndSaveProgressBar, PBM_SETRANGE, 0, ( LPARAM ) MAKELONG( 0, Upper ) );
    }

    size_t TotalDone = 0;

    // In second loop, do the work.
    {
        // If CSolAllUrls grew since first loop,
        // it will only affect progress bar.
        size_t Index = 2; // after 0=Head, 1=Tail.
        size_t Past = CSolAllUrls.nList;
        for( ;; )
        {
            if( Index >= Past )
                break;
            COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( Index );
            if( pOnePaper != NULL
            &&  pOnePaper < PVOID_VALID_BELOW )
            {
                if( pOnePaper->FetchedButNotViewed ) // first view of a "significant" page
                {
                    pOnePaper->FetchedButNotViewed = 0; // save counts for view
                    g_nPapersFetchedButNotViewed --;    // save counts for view
                }

                int SaveAsAQrp = ZERO_ORDINAL_FOR_NON_QRP;
                int FromEngineNo = ZERO_ORDINAL_FOR_NON_QRP;
                int BoolsEngineNo = pOnePaper->PageIsAQrpThisIsItsOrdinal; // two bools and ordinal

                if( BoolsEngineNo != ZERO_ORDINAL_FOR_NON_QRP )
                {
                    SaveAsAQrp = 1;
                    FromEngineNo = BoolsEngineNo & ~ BIT_IN_ORDINAL_TO_KEEP;
                }
                if( BoolsEngineNo & BIT_IN_ORDINAL_TO_KEEP )
                {
                    SaveAsAQrp = 0;
                }

                size_t Length = pOnePaper->pWsbResultText->StrLen;
                if( Length != 0 )
                {
                    wchar_t * pUrlKey = CSolAllUrls.GetFullKey( Index ); // a malloc, user frees
                    if( pUrlKey != NULL )
                    {
                        // To assort pages into subfolders by language,
                        // or save QRP into '_' folder, this is the place:
                        // Append a language abbreviation to szFolderPath,
                        // make folder,
                        // and pass that longer path to FullFileNameForUrl.

                        // Just do a best effort, else relent
                        int bRelent = 0;

                        if( ! g_bCreateSubfolders ) // user prefers not to.
                        {
                            bRelent = 1;
                        }
                        else
                        {
                            wchar_t wk[20];
                            wchar_t * into = wk;
                            wchar_t * from = L"_"; // folder for any PageIsAQrpThisIsItsOrdinal
                            if( ! SaveAsAQrp )
                            {
                                // plan WAS:
                                // from = BestLanguageAbbreviationforIndex( pOnePaper->HttpHeaderContentLanguage );
                                // plan IS:
                                from = GroupAbbreviationforGroupIndex( pOnePaper->LanguageGroup );
                            }
                            // copy language acronym, lowercase it, stop at hyphen.
                            for( ;; )
                            {
                                if( into == wk + 18 )
                                {
                                    bRelent = 1;
                                    break;
                                }
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
                                if( isupper( c ) ) // only usacii in my acronyms
                                {
                                    *into++ = c - 'A' + 'a';
                                }
                                else
                                {
                                    // All my acronyms are isalpha + hyphen,
                                    // but I passed in an underscore above.
                                    *into++ = c;
                                }
                            }
                            if( into == wk )
                                bRelent = 1;

                            // nUsable is indexing just past \ at end.
                            // append language acronym and a new null.
                            wcscpy( szTryFolderPath + nUsable, wk ); // trim, safe

                            // make a feeble mkdir attempt, else relent.
                            // DO check failure due to preexisting file.
                            // feeble attempts make basackwards code...
                            // In fact, for loop, first check dir exist.

                            {
                                WIN32_FIND_DATA FindFileData;
                                HANDLE hFindFile = FindFirstFile( szTryFolderPath, & FindFileData );
                                if( hFindFile == INVALID_HANDLE_VALUE )
                                {
                                    // DIR/FILE does not preexist ( or other error? )
                                    if( ! CreateDirectory( szTryFolderPath, NULL ) )
                                        bRelent = 1;
                                }
                                else
                                {
                                    // DIR/FILE does preexist
                                    DWORD saved = FindFileData . dwFileAttributes;
                                    if( ! FindClose( hFindFile ) )
                                    {
                                        ProgramError( L"FindClose 2x" );
                                    }
                                    if( ! ( saved & FILE_ATTRIBUTE_DIRECTORY ) )
                                    {
                                        // It exists, but it is NOT a directory.
                                        bRelent = 1;
                                    }
                                }
                            }
                        }

                        wchar_t * szPassFolderPath = szTryFolderPath;
                        if( bRelent )
                            szPassFolderPath = szFolderPath;

                        // SED_PILE1
                        wchar_t * pFileName = FullFileNameForUrl( // in SaveFolder
                            szPassFolderPath,
                            pUrlKey,
                            0,           // 0 = .htm
                            SaveAsAQrp,
                            FromEngineNo );
                        // SED_PILE2

                        if( pFileName != NULL )
                        {
                            SaveUrlToFile( pFileName, Index );
                            MyFree( 195, zx, pFileName );
                            pFileName = NULL;
                        }
                        MyFree( 198, zx, pUrlKey );
                        pUrlKey = NULL;

                        TotalDone += Length;
                        if( g_hWndSaveProgressBar != NULL )
                        {
                            size_t nPosition = TotalDone / ScaleFactor;
                            // If I do PostMessage, nothing happens, because no msg loop calls.
                            SendMessage( g_hWndSaveProgressBar, PBM_SETPOS, ( WPARAM ) MAKELONG( ( short ) nPosition, 0 ), 0 );
                        }
                    }
                }
            }
            Index ++;
        }
    }
}

void CFio::SaveFile( )
{
    #if DO_DEBUG_CALLS
        Routine( L"131" );
    #endif
    if( Top.CurrentView == NULL )
        return;

    wchar_t szFile[ MAX_PATH + 1 ]; // Win Help says call will add null
    szFile[0] = NULL; // Initial null means do not recommend a filename

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrTitle = L"Save As Text or Html (with links)";
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0Html Files (*.htm;*.html)\0*.htm;*.html\0All Files (*.*)\0*.*\0";
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

    int AsText = 0; // Save File, interleave, treat as HTM

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
                wcscpy( into, L".htm" );
                break;
            }
        }
    }

    {
        // Do it again, sort of, to see if we all decided on .txt

        int n = wcslen( szFile );
        wchar_t * scan = szFile + n;
        // Scan is at the terminating null of the filename.
        if( ( scan[ -4 ] | ' ' ) == '.'
        &&  ( scan[ -3 ] | ' ' ) == 't'
        &&  ( scan[ -2 ] | ' ' ) == 'x'
        &&  ( scan[ -1 ] | ' ' ) == 't' )
            AsText = 1; // Save File, interleave, treat as TXT
    }

    // Now that all fruit types have the same CBud member pWsbResultText,
    // I can have a single line replace a switch of text format handlers:
    // The name Interleave is because click offsets will become <A> </A>.
    // Interleave will also optionally convert Unicode to MSB or USASCII.

    size_t nMallocBuf = 0;
    // tested above that Top.CurrentView != NULL:
    BYTE * pMallocBuf = Pag.Interleave( Top.CurrentView->pFruit, & nMallocBuf, AsText );

    if( pMallocBuf == NULL )
        return;

    if( nMallocBuf == 0 )
    {
        MyFree( 284, zx, pMallocBuf );
        pMallocBuf = NULL;
        return;
    }

    CommonFileSave( szFile, pMallocBuf, nMallocBuf ); // for current view

    MyFree( 291, zx, pMallocBuf );
    pMallocBuf = NULL;
}

wchar_t * CFio::PromptForSaveAsAnyFileName( wchar_t * szSuggestion, wchar_t * szExtension )
{
    // This is a clone of SaveFile's prompt for a filename,
    // but with a return of a malloc'ed copy of a filename.

    // The purpose is to save a fetched binary file.

    // Caller passes suggestion, e.g., L"bin"

    #if DO_DEBUG_CALLS
        Routine( L"499" );
    #endif

    wchar_t szFile[ MAX_PATH + 1 ]; // Win Help says call will add null
    szFile[0] = NULL; // Initial null means do not recommend a filename

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrTitle = L"Save Binary Data";

    // If suggestion would be empty, my caller made it L"bin".

    wchar_t strFilter[60];
    wsprintf( strFilter, L".%s Files\0*.%s\0All\0*.*\0", szExtension, szExtension );
    ofn.lpstrFilter = strFilter; // made from suggested URL extension

    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST;

    wchar_t strDefExt[20]; // 14 is max len passed to me
    wsprintf( strFilter, L".%s", szExtension );
    ofn.lpstrDefExt = strDefExt; // let system add single default ( . )whatever

    if ( ! GetSaveFileName( & ofn ) )
    {
#ifndef _WIN32_WCE
        if( CommDlgExtendedError( ) ) // zero if user canceled
            ProgramError( L"GetSaveFileName" );
#endif // not _WIN32_WCE

        return NULL; // no filename. User should not save file, not free.
    }

    // Win CE returns a bizarre string containing the desired filename
    // followed by a semicolon, and the filter string; Fix that string:
    // I see now. Win CE appended my whole string from ofn.lpstrFilter.
    // That is no problem here, as I do not have a *.htm;*.html option.

    // Make a malloc copy of my auto array to return name.

    size_t wcount = wcslen( szFile );
    size_t nMallocLen = wcount + 1;
    wchar_t * FullFileName = ( wchar_t * ) MyMalloc( 465, nMallocLen * sizeof( wchar_t ) );

    if( FullFileName != NULL )
    {
        wcscpy( FullFileName, szFile ); // known to be null-terminated
    }
    return FullFileName;
}


void CFio::SaveUrlToFile( wchar_t * szFileName, size_t UrlIndex )
{
    #if DO_DEBUG_CALLS
        Routine( L"134" );
    #endif
    // This only serves Save All, so the output filenames will be .htm.
    // However I could examine the text/plain attribute to save as txt.
    int AsText = 0; // SaveUrlToFile, interleave, treat as HTM

    COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
    if( pOnePaper == NULL
    ||  pOnePaper >= PVOID_VALID_BELOW )
        return;

    size_t nMallocBuf = 0;
    BYTE * pMallocBuf = Pag.Interleave( pOnePaper, & nMallocBuf, AsText );

    if( pMallocBuf == NULL )
        return;

    if( nMallocBuf > 0 )
        CommonFileSave( szFileName, pMallocBuf, nMallocBuf );

    MyFree( 637, zx, pMallocBuf );
    pMallocBuf = NULL;
}

void CFio::CommonFileSave( wchar_t * szFileName, BYTE * pMallocBuf, size_t nMallocBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"135" );
    #endif

    // Now that all the BYTEs to write have been gathered,
    // use memory-mapped file i/o to finish the operation.

    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = INVALID_HANDLE_VALUE;

#ifdef _WIN32_WCE
    // N.B. On the PPC, where CreateFileForMapping replaces CreateFile,
    // as soon as that handle gets passed to CreateFileMapping, whether
    // it pass or fail, that handle is invalid, so do not CloseFile it.
    hFile = CreateFileForMapping( // Lookalikes... WRITING
        szFileName, // LPCTSTR lpFileName,
        GENERIC_READ | GENERIC_WRITE, // DWORD dwDesiredAccess,
        FILE_SHARE_READ, // DWORD dwShareMode,
        NULL, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        CREATE_ALWAYS, // DWORD dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, // DWORD dwFlagsAndAttributes,
        NULL // HANDLE hTemplateFile
    );
#else // not _WIN32_WCE
    hFile = CreateFile( // Lookalikes... WRITING - in CommonFileSave
        szFileName, // LPCTSTR lpFileName,
        GENERIC_READ | GENERIC_WRITE, // DWORD dwDesiredAccess,
        FILE_SHARE_READ, // DWORD dwShareMode,
        NULL, // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        CREATE_ALWAYS, // DWORD dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, // DWORD dwFlagsAndAttributes,
        NULL // HANDLE hTemplateFile
    );
#endif // _WIN32_WCE

    if( hFile == INVALID_HANDLE_VALUE )
    {
        // This error occurred while I was saving all web pages too.
        // Presumably, a web page filename was equal to a dir name.
        // Rather than let it kill program, silently ignore it.
        // Or perhaps, may a less violent message box.
        // ProgramError( L"CreateFile 4" );

        // Show the error, but do not exit program.
        DWORD dwLastError = GetLastError( );

        HMODULE hModule = NULL; // default to system source
        DWORD dwBufferLength = 0;
        DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_FROM_SYSTEM;
        wchar_t * MessageBuffer = NULL; // receives the LocalAlloc.

        dwBufferLength = FormatMessage(
            dwFormatFlags,
            hModule,
            dwLastError,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // default language
            ( wchar_t * ) & MessageBuffer, // type was mis-cast on purpose
            0,
            NULL );

        wchar_t wk[500];
        wchar_t * M2 = ( MessageBuffer == NULL ? L"Unknown reason." : MessageBuffer );
        wsprintf( wk, L"CreateFile <%.256s>: %.200s", szFileName, M2 );
        UserError( wk );

        if( MessageBuffer != NULL )
            LocalFree( MessageBuffer );

        return; // failure
    }

    // Help warns not to CreateFileMapping if size zero.
    // However, data assembly is larger than zero bytes.

    // So writing has only 6, not 7 CloseHandle hFile calls.

    hMap = CreateFileMapping( // Lookalikes... WRITING
        hFile, // HANDLE hFile,
        NULL, // LPSECURITY_ATTRIBUTES lpAttributes,
        PAGE_READWRITE, // DWORD flProtect,
        0, // DWORD dwMaximumSizeHigh,
        nMallocBuf, // DWORD dwMaximumSizeLow,
        NULL // LPCTSTR lpName
    );

    if( hMap == INVALID_HANDLE_VALUE )
    {
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 15" );
        #endif // not _WIN32_WCE
        ProgramError( L"CreateFileMapping 4" );
        return; // failure
    }

    BYTE * bBuffer = ( BYTE* ) MapViewOfFile( // Lookalikes... WRITING
        hMap, // HANDLE hFileMappingObject,
        FILE_MAP_WRITE, // DWORD dwDesiredAccess,
        0, // DWORD dwFileOffsetHigh,
        0, // DWORD dwFileOffsetLow,
        nMallocBuf // SIZE_T dwNumberOfBytesToMap
    );

    if( bBuffer == NULL )
    {
        ProgramError( L"MapViewOfFile 4" );
        if( ! CloseHandle( hMap ) )
            ProgramError( L"CloseHandle hMap 5" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 16" );
        #endif // not _WIN32_WCE
        return; // failure
    }

    // This bBuffer can hold nMallocBuf bytes.

    memcpy( bBuffer, pMallocBuf, nMallocBuf );

    if( ! UnmapViewOfFile( bBuffer ) )
    {
        ProgramError( L"UnmapViewOfFile 4" );
        if( ! CloseHandle( hMap ) )
            ProgramError( L"CloseHandle hMap 4" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 17" );
        #endif // not _WIN32_WCE
        return; // failure
    }
    if( ! CloseHandle( hMap ) )
    {
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 18" );
        #endif // not _WIN32_WCE
        ProgramError( L"CloseHandle hMap 6" );
        return; // failure
    }

    // After doing CreateFilemapping, system will do the CloseHandle( hFile )
    // and this call will return false.--Only applies to CE, not the desktop.

    #ifndef _WIN32_WCE
        if( ! CloseHandle( hFile ) ) // Never on PPC
            ProgramError( L"CloseHandle hFile 19" );
    #endif // not _WIN32_WCE

}

void CFio::CommonPaperFileInput( wchar_t * szFileName, CBud * pBudLog )
{
    #if DO_DEBUG_CALLS
        Routine( L"136" );
    #endif
    // All we know so far is a file name, which includes the path.
    // pBudLog will be NULL for Add File.
    // Open and read whole file contents.
    // Test, convert any MBS or Unicode to wchar_t before analysis.
    // If WordsEx annotated the file, use that to hand an UrlIndex.

	// 2016-05-09 If PIPE.exe annotated file, use that format instead. (somewhere up under here!...)

    // Otherwise, we will finally create the fake "file://..." URL.
    // I will probably have to supply the filename to ProcessPaper.

    // Since I started to format the PageIsAQrpThisIsItsOrdinal in filenames,
    // recover it from filename to set pOnePaper->PageIsAQrpThisIsItsOrdinal.

    // I will not try to recover the secondary bool, but treat as a non-QRP.

    int QRPOrdinal = 0; // predict not a query result page / ordinal.
    int AsText = 0; // predict CommonPaperFileInput will treat as HTM input
    {
        wchar_t * scan = szFileName;
        wchar_t * pPastSlash = scan;
        for( ;; )
        {
            if( * scan == NULL )
                break;
            if( * scan == '/'
            ||  * scan == '\\' )
                pPastSlash = scan + 1;
            scan ++;
        }

        // Scan is at the terminating null of the filename.
        // Compare the file extension to ".txt"
        if( ( scan[ -4 ] | ' ' ) == '.'
        &&  ( scan[ -3 ] | ' ' ) == 't'
        &&  ( scan[ -2 ] | ' ' ) == 'x'
        &&  ( scan[ -1 ] | ' ' ) == 't' )
            AsText = 1; // for .txt, CommonPaperFileInput will treat as TXT input

        // Now analyze the final part of filename,
        // at pPastSlash, which I pointed validly.
        // This IF is safe if NULL stops at [any].
        if( pPastSlash[0] == '_' )
        {
            if( iswdigit( pPastSlash[1] )
            &&  iswdigit( pPastSlash[2] )
            &&  iswdigit( pPastSlash[3] ) )
            {
                QRPOrdinal = ( pPastSlash[1] - '0' ) * 100
                    + ( pPastSlash[2] - '0' ) * 10
                    + ( pPastSlash[3] - '0' );
                if( QRPOrdinal < 1
                || QRPOrdinal > 999 )
                    QRPOrdinal = 1; // CYA, replace with a boolean
            }
        }
    }

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
    hFile = CreateFile( // Lookalikes... READING - in CommonPaperFileInput
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
        // When I attempted several concurrent Add Folders,
        // I got a failure here. Rather than stop program,
        // let's absorb this one, but make note to log.
        // ProgramError( L"CreateFile 1" );
        if( pBudLog != NULL )
        {
            // Add Folder failure
            // Annotate log with the filename and rejection message.
            pBudLog->pWsbResultText->Add( szFileName );
            pBudLog->pWsbResultText->Add( L"\r\n--CreateFile GENERIC_READ failed.\r\n\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }
        else
        {
            // Add File failure. I really need to show the filename.
            wchar_t wk[500];
            wsprintf( wk, L"CreateFile <%.256s> for GENERIC_READ failed.", szFileName );
            UserError( wk );
        }

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
                ProgramError( L"CloseHandle hFile 21" );
        #endif // not _WIN32_WCE
        ProgramError( L"GetFileSize 1" );
        return; // failure
    }
    if ( dwSize == 0 )
    {
        // Silently return and not try to load any zero byte file.
        // Not silently any more...
        // Add some mention of this unusable file to the log.
        // If doing a single file, make a message box.

        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 22" );
        #endif // not _WIN32_WCE

        if( pBudLog != NULL )
        {
            // Add Folder failure
            // Annotate log with the filename and rejection message.
            pBudLog->pWsbResultText->Add( szFileName );
            pBudLog->pWsbResultText->Add( L"\r\n--File size is zero. Ignoring file.\r\n\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }
        else
        {
            // Add File failure
            Say( L"File size is zero. Ignoring file." );
        }
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
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 23" );
        #endif // not _WIN32_WCE
        ProgramError( L"CreateFileMapping2 " );
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
                ProgramError( L"CloseHandle hFile 24" );
        #endif // not _WIN32_WCE
        return; // failure
    }

    // When I tried to input a 400MB file, I got a malloc
    // returned 0 error ( over at ID#1918 ), so put a limit
    // on file size. Don't return; Just take part of file.

    if ( dwSize > 10000000 )
    {
        dwSize = 10000000; // Ten MB big enuf for you?

        if( pBudLog != NULL )
        {
            // Add Folder failure
            // Annotate log with the filename and rejection message.
            pBudLog->pWsbResultText->Add( szFileName );
            pBudLog->pWsbResultText->Add( L"\r\n--Exceeds 10 MB. Taking only 10 MB.\r\n\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }
        else
        {
            // Add File failure
            Say( L"Exceeds 10 MB. Taking only 10 MB." );
        }
    }

    // This bBuffer contains dwSize bytes.
    // You cannot write sentinels into it.


	// 2016-05-09 If PIPE.exe annotated file, use that format instead. (must be about here:)
	// The Pag.CategorizeAndConvert... looks at BOM etc, so I must rid my PIP header now:
	// PIPE wrote UTF8, so this top of file reads as strictly ASCII here:

	//Request-Url: https://www.jetbrains.com/clion/
	//Status: OK; OK
	//Content-Type: text/html;charset=UTF-8
	//Date: Wed, 06 Apr 2016 23:06:00 GMT
	//Server: nginx
	//Depth: 2
	//
	// ... Visual Studio did corrupt on paste, but this header stopper is exactly first ...CRLFCRLF...
	//
	//
	//<!DOCTYPE html>...

	// So this new code will:
	// 1. recognize such header
	// 2. advance past such header
	// 3. Still need to inform somebody of that base URL! -- somewhere below, near variable UrlIndexToClaim
	// 4. Also, still need to cause parse of HTML, despite ending in .txt.
	BYTE * bBuffer2 = bBuffer;
	DWORD dwSize2 = dwSize;

	if (dwSize > 20
		&& bBuffer[0] == 'R'
		&& bBuffer[1] == 'e'
		&& bBuffer[2] == 'q'
		&& bBuffer[3] == 'u'
		&& bBuffer[4] == 'e'
		&& bBuffer[5] == 's'
		&& bBuffer[6] == 't'
		&& bBuffer[7] == '-'
		&& bBuffer[8] == 'U'
		&& bBuffer[9] == 'r'
		&& bBuffer[10] == 'l'
		&& bBuffer[11] == ':'
		&& bBuffer[12] == ' '
		)
	{
		// Find first CRLFCRLF < dwSize2,
		BYTE * bpSearch = bBuffer2;
		BYTE * eoSearch = bBuffer2 + dwSize2 - 4;
		while (bpSearch < eoSearch)
		{
			bpSearch++;
			if (bpSearch[0] == '\r'
				&& bpSearch[1] == '\n'
				&& bpSearch[2] == '\r'
				&& bpSearch[3] == '\n'
				)
			{
				// advance bBuffer2
				// diminish dwSize2
				dwSize2 -= bpSearch + 4 - bBuffer2;
				bBuffer2 = bpSearch + 4;
			}
		}
		// 2016 wipwipwiop

	}
    // This call is for CommonPaperFileInput:
    size_t nWBuf;
    wchar_t * pWBuf = Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection( & nWBuf, bBuffer2, dwSize2 );

    // Now I can close off the file i/o part before continuing.
    if( ! UnmapViewOfFile( bBuffer ) )
    {
        ProgramError( L"UnmapViewOfFile 2" );
        if( ! CloseHandle( hMap ) )
            ProgramError( L"CloseHandle hMap 13" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 25" );
        #endif // not _WIN32_WCE
        if( pWBuf != NULL )
            MyFree( 886, UNPREDICTABLE, pWBuf );
        pWBuf = NULL;
        return; // failure
    }
    if( ! CloseHandle( hMap ) )
    {
        ProgramError( L"CloseHandle hMap 15" );
        #ifndef _WIN32_WCE
            if( ! CloseHandle( hFile ) ) // Never on PPC
                ProgramError( L"CloseHandle hFile 26" );
        #endif // not _WIN32_WCE
        if( pWBuf != NULL )
            MyFree( 891, UNPREDICTABLE, pWBuf );
        pWBuf = NULL;
        return; // failure
    }

    // After doing CreateFilemapping, system will do the CloseHandle( hFile )
    // and this call will return false.--Only applies to CE, not the desktop.

    #ifndef _WIN32_WCE
        if( ! CloseHandle( hFile ) ) // Never on PPC
            ProgramError( L"CloseHandle hFile 27" );
    #endif // not _WIN32_WCE

    // Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection returned NULL if it determined binary data.
    if( pWBuf == NULL )
    {
        // Add some mention of this unusable file to the log.
        // If doing a single file, make a message box.

        if( pBudLog != NULL )
        {
            // Add Folder failure
            // Annotate log with the filename and rejection message.
            pBudLog->pWsbResultText->Add( szFileName );
            pBudLog->pWsbResultText->Add( L"\r\n--File characterized and refused.\r\n\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }
        else
        {
            // Add File failure
            Say( L"File characterized and refused." );
        }
        return;
    }

    // If WordsEx annotated the file, use that to hand an UrlIndex.
    // Otherwise, we will finally create the fake "file://..." URL.
    // I will probably have to supply the filename to ProcessPaper.

    // I have not claimed any URL yet and will pass UrlIndex of 0.
    // No, go ahead and claim a File: URL, although it may change.
    // Well, maybe the recognition of top-line URL can solve this.


    // Suffer me to have a pFruit not hanging from any tree for a moment...
    COnePaper * pOnePaper = new COnePaper( 0 );
    // Every making of a new COnePaper must create its pWsbAnnotation.
    pOnePaper->pWsbAnnotation = new CWsb;

    // pOnePaper->m_CSolIndex = UrlIndexToClaim;

    // Since I started to format the PageIsAQrpThisIsItsOrdinal in filenames,
    // recover it from filename to set pOnePaper->PageIsAQrpThisIsItsOrdinal.

    pOnePaper->PageIsAQrpThisIsItsOrdinal = QRPOrdinal;

    #if DO_DEBUG_ORDINALS
        ; SpewTwo( L"CommonPaperFileInput", szFileName );
        ; SpewValue( L"QRPOrdinal", QRPOrdinal );
    #endif

    // Notice that Pag.Process... doesn't care about QRPOrdinal.
    int ShowAsAQrp = QRPOrdinal; // bool synonym: show meaning better.

    size_t UrlIndexToClaim = 0;
    size_t nDate = 0;
    size_t nLang = 0;
    size_t nCSet = 0;

    size_t Skip = Pag.SkipWordsExHeader( & UrlIndexToClaim, & nDate, & nLang, & nCSet, pWBuf, nWBuf, pOnePaper );

    if( Skip > 0 )
    {
        // Note that my idiomatic header check come after CategorizeAndConvertInputBytesToWideWithBinaryRejection.
        // It WAS my idiomatic header. Input as text.
        // Not so fast. If filename was .htm, it has links to parse out.
        // Do not set bit 1 of astext, but set bit 2, honor newlines.
        AsText |= 2; // CommonPaperFileInput doing WordsEx files: honor indent
    }

    if( UrlIndexToClaim == 0 )
    {
        // formulate a tentative FILE:// url to claim.
        #if DO_DEBUG_INPUT
            ; Spew( L"Making a File:// URL." );
        #endif
        wchar_t FileUrl[ 7 + MAX_PATH + 1 ]; // 7 for L"file://", 1 extra
        wcscpy( FileUrl, L"file://" );
        wcscpy( FileUrl + 7 , szFileName );
        UrlIndexToClaim = CSolAllUrls.AddKey( FileUrl );
        #if DO_DEBUG_ADDFIND
            if( UrlIndexToClaim == 1 )
                { Spew( L"AddFind 1 at cfio 1088" ); }
        #endif
    }

    if( CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_CLAIMING ) )
    {
        #if DO_DEBUG_INPUT
            ; Spew( L"ClaimUserpVoid Succeeded." );
        #endif

        size_t OriginallyClaimed = UrlIndexToClaim; // in case of a change.

        // This part parses, analyzes, the source paper.
        // 1 of 4 similar sequences. This one is for input file.

        // Gating this off serves the re-import of directories
        // as LANGUAGE_UNKNOWN during the making of word lists.

        #if OKAY_TO_ASSIGN_LANG
            pOnePaper->HttpHeaderContentLanguage = nLang; // from SkipWordsExHeader
        #endif

        pOnePaper->LanguageGroup = GroupIndexForLanguageIndex( pOnePaper->HttpHeaderContentLanguage );
        pOnePaper->HttpHeaderCharset         = nCSet; // from SkipWordsExHeader
        pOnePaper->HttpHeaderDateyyyymmdd    = nDate; // from SkipWordsExHeader
        // SED_PILE1
        // from CFio CommonPaperFileInput
        // This page processing call is for File input.
        UrlIndexToClaim = Pag.ProcessPaper(
            UrlIndexToClaim,        // size_t UrlIndex
            pOnePaper,              // COnePaper * pOnePaper
            pWBuf+Skip,             // wchar_t * pInputBuffer
            nWBuf-Skip,             // size_t nInputSize
            AsText,                 // int AsText
            NO_FRAMES,              // CSol * pSolFrames
            NO_MORES,               // CSol * pSolMores
            NO_HITS,                // CSol * pSolHits
            NULL );                 // COneSurl * pOneSurl
        // SED_PILE2

        // Revise now in case changed, so that annotations are correct:
        pOnePaper->m_CSolIndex = UrlIndexToClaim;

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
            MyFree( 728, UNPREDICTABLE, pMalNewTop );
            pMalNewTop = NULL;
        }

        // Now I must hang the paper.

        if( OriginallyClaimed != UrlIndexToClaim )
        {
            // Abandon the tentative claim,
            // make new claim for UrlIndex.

            #if DO_DEBUG_BASE
                ; Spew( L"CFio: OriginallyClaimed != UrlIndexToClaim" );
            #endif

            // This call must work
            if( ! CSolAllUrls.ClaimUserpVoid( OriginallyClaimed, PVOID_NOTFOUNDETC ) )
            {
                ProgramError( L"CommonPaperFileInput: ! CSolAllUrls.ClaimUserpVoid" );
            }

            // This one might not, if some URL is already hanging.

            if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, PVOID_CLAIMING ) )
            {
                #if DO_DEBUG_BASE
                    ; Spew( L"CFio: The attempt to claim the revised base URL failed." );
                #endif
                UrlIndexToClaim = 0; // to stop work below
            }
        }
        else
        {
            #if DO_DEBUG_BASE
                ; Spew( L"CFio: ProcessPaper did not change OriginallyClaimed UrlIndex" );
            #endif
        }
    }
    else
    {
        UrlIndexToClaim = 0; // to stop work below
    }

    if( UrlIndexToClaim != 0 )
    {
        // I am about to have CSol swap in my paper pointer for the marker.
        // Before doing that, paper need to contain the valid index number.

        pOnePaper->m_CSolIndex = UrlIndexToClaim;

        // This call must work, and finishes my internal tasks.
        if( ! CSolAllUrls.ClaimUserpVoid( UrlIndexToClaim, pOnePaper ) )
        {
            ProgramError( L"CommonPaperFileInput: ! CSolAllUrls.ClaimUserpVoid" );
        }

        // Annotate log with the URL and other notes from ProcessPaper.
        if( pBudLog != NULL )
        {
            // Add folder, passing a log, so coming here,
            // does not need to put each file on display.
            size_t StartOffset = pBudLog->pWsbResultText->StrLen;
            pBudLog->pWsbResultText->AddWsb( pOnePaper->pWsbAnnotation );
            size_t FinalOffset = pBudLog->pWsbResultText->StrLen;
            pBudLog->pIdxResultIndex->AddIdx( StartOffset, FinalOffset, UrlIndexToClaim, 0 );
            pBudLog->pWsbResultText->Add( L"\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }
        else
        {
            // File Open needs me to put this paper on display.
            // Serving File Open, this is okay, in main thread:
            Top.Add( pOnePaper ); // Put fruit on display
        }
    }
    else
    {
        // I failed to claim the desired URL for updating.

        // This is an orphan paper. I have to destroy it.
        delete pOnePaper;
        pOnePaper = NULL;

        // Add some mention of this situation to the log.
        // If doing a single file, make a message box.

        if( pBudLog != NULL )
        {
            // Add Folder failure
            // Annotate log with the filename and rejection message.
            pBudLog->pWsbResultText->Add( szFileName );
            pBudLog->pWsbResultText->Add( L"\r\n--Attempt to claim this URL failed ( previously processed ).\r\n\r\n" );
            // obs: Top.UpdateViewIfOnScreen( pBudLog );
        }
        else
        {
            // Add File failure
            Say( L"Attempt to claim this URL failed ( previously processed )." );
        }
    }

    // any other mallocs etc to detroy?

    // Pag.CategorizeAndConvertInputBytesToWideWithBinaryRejection made a sloppy large buffer.
    MyFree( 1262, UNPREDICTABLE, pWBuf );
    pWBuf = NULL;

    return;
}

void CFio::AddFile( )
{
    #if DO_DEBUG_CALLS
        Routine( L"137" );
    #endif
    wchar_t szFile[ MAX_PATH + 1 ]; // Win Help says call will add null
    szFile[0] = NULL; // Initial null means do not recommend a filename

    // During file open, if I index 1 ( .txt ) and click on a bare file,
    // the attempt to append a .txt makes createfile fail. So use 3.
    // All the worse because the error message doesn't show filename.

    OPENFILENAME ofn;
    memset( & ofn, 0, sizeof( OPENFILENAME ) );
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof( szFile )/sizeof( *szFile );
    ofn.lpstrTitle = L"Add Text or Html File";
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0Html Files (*.htm;*.html)\0*.htm;*.html\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 3; // point to the *.* choice for easiest clicking.
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = NULL; // I will handle N default extensions myself

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
                wcscpy( into, L".htm" );
                break;
            }
        }
    }

    // This add file will never have a progress log.
    CommonPaperFileInput( szFile, NULL );
}

int CFio::FolderPathIsValid( wchar_t * szFolderPath )
{
    // Whereas Save Directory, or Use Directory, will create all paths;
    // For Add Directory, folder path should pre-exist. Do not make it.

    // I will put up a UserError message box if not acceptable.

    // Verify that the user string does not contain metacharacters ( *,? ).
    // Also note whether path has final \, ensure has one if is at root.
    // In fact, let's copy it into a MAX_PATH buffer, all my api can use.

    // user string might be relative or absolute.
    // Test string for the following formats:
    // rel:     file...
    // UNC:     \\server\file...
    // PPC      \My Documents\file...
    // Full:    x:\file...

    // Windows CE does not allow the following characters in a file or directory name:
    // '*' , '\\', '/','?', '>', '<', ':', '"', '|', and any character less than 32.

    #if DO_DEBUG_PATH
        SpewTwo( L"Examining", szFolderPath );
    #endif

    wchar_t AutoPath [ MAX_PATH ];
    int InvalidPath = 0;
    int UNCPath = 0;
    {
        wchar_t * from = szFolderPath;
        wchar_t * into = AutoPath;
        wchar_t * PastDrive = AutoPath;
        wchar_t * LastPathSep = NULL;
        wchar_t * RootPathSep = NULL;

        for( ;; )
        {
            wchar_t wc = *from++;
            if( wc == NULL )
            {
                #if DO_DEBUG_PATH
                    Spew( L"At EOS" );
                #endif
                *into = NULL;
                break;
            }
            if( wc < 32 || wc > 255 )
            {
                #if DO_DEBUG_PATH
                    Spew( L"wc < 32 || wc > 255" );
                #endif
                InvalidPath = 1;
                break;
            }
            // Only allow \ and : from invalid chars list.

            if( wc == '*'
            ||  wc == '/'
            ||  wc == '>'
            ||  wc == '<'
            ||  wc == '"'
            ||  wc == '|'
            ||  wc == '?' )
            {
                #if DO_DEBUG_PATH
                    Spew( L"wc == filechar disallow" );
                #endif
                InvalidPath = 1;
                break;
            }

            if( wc == ':' )
            {
                #if DO_DEBUG_PATH
                    Spew( L"wc == colon" );
                #endif

                #ifdef _WIN32_WCE
                    // Disallow all colons on Pocket PC
                    InvalidPath = 1;
                    break;
                #else // not _WIN32_WCE
                    // Allow only one colon at [1] offset: "X:\..."
                    // And not after '\\'

                    if( into - AutoPath != 1
                    || AutoPath[0] == '\\' )
                    {
                        InvalidPath = 1;
                        break;
                    }
                    PastDrive = AutoPath + 2;
                #endif // _WIN32_WCE

            }

            if( wc == '\\' )
            {
                #if DO_DEBUG_PATH
                    Spew( L"wc == backslash" );
                #endif

                // What shall we ask about path separators?
                if( into == PastDrive )
                {
                    #if DO_DEBUG_PATH
                        Spew( L"backslash at PastDrive" );
                    #endif
                    // e.g., \path, x:\path, \My Doc...
                    // but also on first char of \\...
                    RootPathSep = into;
                }

                if( UNCPath
                && RootPathSep == NULL )
                {
                    #if DO_DEBUG_PATH
                        Spew( L"backslash 3 of UNC" );
                    #endif
                    RootPathSep = into; // set it further into UNC path
                }

                if( into == AutoPath + 1
                && RootPathSep != NULL )
                {
                    #if DO_DEBUG_PATH
                        Spew( L"backslash 2 of UNC" );
                    #endif
                    UNCPath = 1; // e.g., \\server\path...
                    RootPathSep = NULL; // available to set again above
                }
                LastPathSep = into;
            }

            *into = wc;
            if( ++into == AutoPath + sizeof( AutoPath )/sizeof( *AutoPath ) )
            {
                #if DO_DEBUG_PATH
                    Spew( L"Past MAX_PATH" );
                #endif
                InvalidPath = 1;
                break;
            }

        }

        // Test for directory passes with or without final backslash.
        // Nobody sees my strip, but it affects empty path decision.

        #if DO_DEBUG_PATH
            SpewTwo( L"Prestrip", AutoPath );
        #endif

        // In fact, since FindFirstFile will fail on root path,
        // let's recognize that now to disallow it explicitly.

        // This test did not strip final \\ on UNC root path,
        // ( or did it? ) nor did it disallow a UNC root folder,
        // e.g., \\File_Server\Drive_P\ or w/o final backslash
        // and it should have... Ehhh, small fry.

        if( LastPathSep == into - 1 )
        {
            if( LastPathSep == RootPathSep )
            {
                UserError( L"The root folder is not allowed." );
                return 0;
            }
            else
            {
                #if DO_DEBUG_PATH
                    Spew( L"strip final backslash" );
                #endif

                // The final char was \\. Do what?
                // Let's strip it, unless at root.
                --into;
                *into = NULL;
            }
        }

        #if DO_DEBUG_PATH
            SpewTwo( L"Poststrip", AutoPath );
        #endif

        if( into == PastDrive )
        {
            #if DO_DEBUG_PATH
                Spew( L"empty path" );
            #endif
            InvalidPath = 1; // empty path ( maybe after X: )
        }
    }

    if( InvalidPath )
    {
        UserError( L"The path format is invalid." );
        return 0;
    }

    // Test that such path name exists, and is a directory.
    // Note that local root directories may fail this test!

    WIN32_FIND_DATA FindFileData;
    HANDLE hFindFile = FindFirstFile( AutoPath, & FindFileData );
    if( hFindFile == INVALID_HANDLE_VALUE )
    {
        // DIR/FILE does not preexist ( or other error? )
        // Do not call CreateDirectory in this case.

        UserError( L"The path does not pre-exist." );

        return 0; // no such directory
    }
    else
    {
        // DIR/FILE does preexist
        DWORD saved = FindFileData . dwFileAttributes;
        if( ! FindClose( hFindFile ) )
        {
            ProgramError( L"FindClose 2x" );
        }
        if( ! ( saved & FILE_ATTRIBUTE_DIRECTORY ) )
        {
            UserError( L"The path exists, but as a file." );
            return 0; // no such directory
        }
    }

    return 1; // success: directory pre-existed

}

void CFio::AddFolderP1RecursivePart( COneFolder * pFolder, wchar_t * szCurFolder )
{
    // szCurFolder IS a folder, < MAX_PATH, and has no final backslash.

    // pFolder->pWsbResultText->Add( L"Helper called for [" );
    // pFolder->pWsbResultText->Add( szCurFolder );
    // pFolder->pWsbResultText->Add( L"]\r\n" );

    // I have inherited no little confusion here.
    // A similar routine before failed to FIND extant x:\d\1 folder.
    // Finally, I believe that cFileName[MAX_PATH] includes a null.
    // However, I notice cFileName[MAX_PATH] is a pathless filename.

    int nPath = wcslen( szCurFolder );
    if( nPath > MAX_PATH - 6 ) // barely room to add \\*.*\0
        return; // without warning?

    wchar_t AutoFileName[ MAX_PATH * 2 ]; // CYA to cat path + filename
    wcscpy( AutoFileName, szCurFolder );

    // Although this has worked before, in say, x:\aq\q01,
    // it ceased working in say, x:\d\1 folder, returning
    // only the "." and ".." names. Change "*" to "*.*" ?

    AutoFileName[ nPath ] = L'\\';
    wchar_t * pStar = AutoFileName + nPath + 1; // where to append *.* or...

    // typedef struct _WIN32_FIND_DATA {
    // DWORD dwFileAttributes;
    // FILETIME ftCreationTime;
    // FILETIME ftLastAccessTime;
    // FILETIME ftLastWriteTime;
    // DWORD nFileSizeHigh;
    // DWORD nFileSizeLow;
    // DWORD dwOID;
    // TCHAR cFileName[MAX_PATH];
    // } WIN32_FIND_DATA;

    WIN32_FIND_DATA FindFileData;

    // In part 1 loop, do all the non-directory filenames.

    size_t FolderFiles = 0;
    size_t FolderBytes = 0;
    size_t FoundFirst = 1;

    pStar[ 0 ] = L'*';
    pStar[ 1 ] = L'.';
    pStar[ 2 ] = L'*';
    pStar[ 3 ] = NULL;
    HANDLE hFindFile = FindFirstFile( AutoFileName, & FindFileData );
    if( hFindFile == INVALID_HANDLE_VALUE )
    {
        int iError = GetLastError( );
        if( iError == ERROR_NO_MORE_FILES ) // I see it applies to First too.
        {
            FoundFirst = 0;
        }
        else
        {
            ProgramError( L"FindFirstFile 3" );
            return;
        }
    }

    if( FoundFirst )
    {
        for( ;; )
        {
            // Now add each filename, with size prefix, to a sorted list.

            if( FindFileData . nFileSizeHigh == 0 // No files over 2^32 size
            && FindFileData . nFileSizeLow != 0 // No files of zero size

            // I need to re-import queries to fully annotate All URLS list.
            // So do not exclude folders/filenames starting with underscore.
            // && FindFileData . cFileName[ 0 ] != '_' // first char of query filenames
            // No, I don't like them. User can import that subfolder explicitly.
            // Oh, don't exclude them here, this is the filename, not folder...

            && ! ( FindFileData . dwFileAttributes & FILE_ATTRIBUTE_ABNORMAL ) )
            {
                // FindFileData . cFileName is pathless.
                // Copy the final path piece over asterisk.
                wcsncpy( pStar, FindFileData . cFileName, MAX_PATH - 1 );
                pStar [ MAX_PATH - 1 ] = NULL; // Always CYA after using wcsncpy

                // Now, I must append this item ( full path filename ) to a list.
                // Nothing could be finer than a CSol, with a size to control sort
                // followed by the filename to open, as the key. I have no UrlIndex.

                wchar_t Entry[ MAX_PATH * 2 + 30 ]; // worst AutoFilename + 30 for 18 in wsprintf
                wsprintf( Entry, L"%9d bytes:\r\n%s", FindFileData . nFileSizeLow, AutoFileName );
                // wip - pFolder->pSolFolderToDo is null here
                size_t Index = pFolder->pSolFolderToDo->AddKey( Entry );
                #if DO_DEBUG_ADDFIND
                    if( Index == 1 )
                        { Spew( L"AddFind 1 at cfio 1335" ); }
                #endif

                FolderFiles ++;
                FolderBytes += FindFileData . nFileSizeLow;

                pFolder->m_TotalFolderFiles ++;
                pFolder->m_TotalFolderBytes += FindFileData . nFileSizeLow;
            }

            if( ! FindNextFile( hFindFile, & FindFileData ) )
            {
                int iError = GetLastError( );
                if( iError == ERROR_NO_MORE_FILES )
                {
                    break;
                }
                ProgramError( L"FindNextFile" );
                break;
            }
        }
        if( ! FindClose( hFindFile ) )
        {
            ProgramError( L"FindClose 1" );
        }
    }

    if( pFolder->m_bAddRecursively )
    {
        // Only make this per-folder annotation when recursive.
        wchar_t wk[80];
        pFolder->pWsbResultText->Add( szCurFolder );
        wsprintf( wk, L" %d files, %d bytes.\r\n", FolderFiles, FolderBytes );
        pFolder->pWsbResultText->Add( wk );
    }

    // In part 2 loop, if pFolder->m_bAddRecursively, do directories.
    if( ! pFolder->m_bAddRecursively )
        return; // success ( but no guarantee that any files were found )

    // Let this be a lesson!

    //  C.:.\.W.o.r.d.s.E.x.\.
    //  ..\...\...\...\...\...
    //  \...\...\...\...\...\.
    //  ..\...\...\...\...\...
    //  \...\...\...\...\...\.
    //  ..\...\...\...\...\...
    //  \...\...\...\...\...\.
    //  ..\...\...\...\...\...
    //  \...\...\...\...\...\.

    FoundFirst = 1;
    pStar[ 0 ] = L'*';
    pStar[ 1 ] = L'.';
    pStar[ 2 ] = L'*';
    pStar[ 3 ] = NULL;
    hFindFile = FindFirstFile( AutoFileName, & FindFileData );
    if( hFindFile == INVALID_HANDLE_VALUE )
    {
        int iError = GetLastError( );
        if( iError == ERROR_NO_MORE_FILES ) // I see it applies to First too.
        {
            FoundFirst = 0;
        }
        else
        {
            ProgramError( L"FindFirstFile 3" );
            return;
        }
    }

    if( FoundFirst )
    {
        for( ;; )
        {
            // Now just do subdirectories.
            // Ignore my "_" subdirectory.

            if( ( FindFileData . dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0
            // I need to re-import queries to fully annotate All URLS list.
            // So do not exclude folders/filenames starting with underscore.
            // && FindFileData . cFileName[ 0 ] != '_'
            // No, I don't like them. User can import that subfolder explicitly.
            // Resume the excluding here. this is the folder, not filename...
            && FindFileData . cFileName[ 0 ] != '_'

            && FindFileData . cFileName[ 0 ] != '.' ) // eschew recursion up.
            {
                // FindFileData . cFileName is pathless.
                // Copy the final path piece over asterisk.
                wcsncpy( pStar, FindFileData . cFileName, MAX_PATH - 1 );
                pStar [ MAX_PATH - 1 ] = NULL; // Always CYA after using wcsncpy

                // Hoook it up!

                AddFolderP1RecursivePart( pFolder, AutoFileName );
            }

            if( ! FindNextFile( hFindFile, & FindFileData ) )
            {
                int iError = GetLastError( );
                if( iError == ERROR_NO_MORE_FILES )
                {
                    break;
                }
                ProgramError( L"FindNextFile" );
                break;
            }
        }
        if( ! FindClose( hFindFile ) )
        {
            ProgramError( L"FindClose 1" );
        }
    }

    return; // success ( but no guarantee that any files were found )
}

int CFio::AddFolderPhaseOne( size_t FolderIndex )
{
    #if DO_DEBUG_CALLS
        Routine( L"138" );
    #endif
    // Get at my thread's own CSolUserFolders pFruit to control process.
    COneFolder * pFolder = ( COneFolder * ) CSolUserFolders.GetUserpVoid( FolderIndex );
    if( pFolder == NULL )
    {
        ProgramError( L"AddFolderPhase1 pFolder NULL" );
        return 0;
    }

    pFolder->pWsbResultText->Add( L"Phase 1: Sorting files by size.\r\n\r\n" );

    // For _ADD_ ( not save, use ) path must preexist.
    // WordsEx already called Fio.FolderPathIsValid.

    // Easiest if I clone the path math above, rid msgs.

    wchar_t * pMalFolderPath = CSolUserFolders.GetFullKey( FolderIndex );
    if( pMalFolderPath == NULL )
    {
        ProgramError( L"pMalFolderPath == NULL" );
        return 0;
    }

    wchar_t AutoPath [ MAX_PATH ];
    int InvalidPath = 0;
    int UNCPath = 0;
    {
        wchar_t * from = pMalFolderPath;
        wchar_t * into = AutoPath;
        wchar_t * PastDrive = AutoPath;
        wchar_t * LastPathSep = NULL;
        wchar_t * RootPathSep = NULL;

        for( ;; )
        {
            wchar_t wc = *from++;
            if( wc == NULL )
            {
                *into = NULL;
                break;
            }
            if( wc == ':' )
            {
                // not _WIN32_WCE
                    PastDrive = AutoPath + 2;
                // _WIN32_WCE
            }

            if( wc == '\\' )
            {
                // What shall we ask about path separators?
                if( into == PastDrive )
                {
                    // e.g., \path, x:\path, \My Doc...
                    // but also on first char of \\...
                    RootPathSep = into;
                }

                if( UNCPath
                && RootPathSep == NULL )
                {
                    RootPathSep = into; // set it further into UNC path
                }

                if( into == AutoPath + 1
                && RootPathSep != NULL )
                {
                    UNCPath = 1; // e.g., \\server\path...
                    RootPathSep = NULL; // available to set again above
                }
                LastPathSep = into;
            }

            *into++ = wc;
        }

        if( LastPathSep == into - 1
        && LastPathSep != RootPathSep )
        {
            #if DO_DEBUG_PATH
                Spew( L"strip final backslash" );
            #endif

            // The final char was \\. Do what?
            // Let's strip it, unless at root.
            --into;
            *into = NULL;
        }

        #if DO_DEBUG_PATH
            SpewTwo( L"Poststrip", AutoPath );
        #endif
    }

    if( pFolder->pSolFolderToDo != NULL )
    {
        CSol * Temp = pFolder->pSolFolderToDo; // copy before delete
        pFolder->pSolFolderToDo = NULL;        // NULL before delete
        delete Temp;
        Temp = NULL; // obligatory rigor after delete
    }
    pFolder->pSolFolderToDo = new CSol( CSOL_SCALAR ); // to sort files by size...

    pFolder->m_TotalFolderFiles = 0;
    pFolder->m_TotalFolderBytes = 0;

    AddFolderP1RecursivePart( pFolder, AutoPath );

    if( pFolder->m_bAddRecursively )
        pFolder->pWsbResultText->Add( L"\r\n" ); // after block of dir blurbs

    if( pFolder->m_TotalFolderFiles == 0 )
    {
        pFolder->pWsbResultText->Add( L"Found no files.\r\n" );
        MyFree( 1843, zx, pMalFolderPath );
        return 0;
    }

    MyFree( 1847, zx, pMalFolderPath );
    return 1; // 1=success, okay do do phase 2.
}

void CFio::AddFolderPhaseTwo( size_t FolderIndex )
{
    #if DO_DEBUG_CALLS
        Routine( L"139" );
    #endif
    // Get at my thread's own CSolUserFolders pFruit to control process.
    COneFolder * pFolder = ( COneFolder * ) CSolUserFolders.GetUserpVoid( FolderIndex );
    if( pFolder == NULL )
    {
        ProgramError( L"AddFolderPhase2 pFolder NULL" );
        return;
    }

    pFolder->pWsbResultText->Add( L"Phase 2: Adding files.\r\n\r\n" );

    if( pFolder->pSolFolderToDo == NULL )
    {
        ProgramError( L"AddFolderPhase2 pSolFolderToDo NULL" );
        return;
    }

    // Phase Two runs through the list from Phase One,
    // opening each file, parsing and hanging sources,
    // adding result log text and indices to pFolder->

    {
        // For byte counts running into the Gigabytes, insert commas:
        // I know some library function might have done this for me.
        wchar_t come[20];
        wchar_t * bkwds = come + 19;
        *bkwds = NULL;
        int n = pFolder->m_TotalFolderBytes;
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
        wsprintf( wk, L"The directory holds %d total files, %s bytes.\r\n\r\n",
            pFolder->m_TotalFolderFiles,
            bkwds ); // pFolder->m_TotalFolderBytes );
        pFolder->pWsbResultText->Add( wk );
    }

    CoIt * pMalVector = pFolder->pSolFolderToDo->GetSortedVector( CSOL_BACKWARD );
    if( pMalVector == NULL )
        return;
    size_t take = 0;
    for( ;; )
    {
        CoIt * pCoIt = pMalVector + take++;
        if( pCoIt->IsSentinel )
            break;

        // The thing of interest to me is the file path,
        // which phase one added into CSol pSolFolderToDo,
        // which is held right in the pCoIt's key value.

        wchar_t * FullKey = CoItFullKey( pCoIt );
        wchar_t * PassedFileName = FullKey + 18; // above: "%9d bytes:\r\n%s"

        size_t StartOffset = pFolder->pWsbResultText->StrLen;

        // Showing filename should not be limited to debugging.
        // I need filename while perusing Add Directory results
        // to be able to delete files as I find anomalies, etc:

        pFolder->pWsbResultText->Add( PassedFileName );
        pFolder->pWsbResultText->Add( L"\r\n" );

        // This add folder call does always have and pass a progress log.
        CommonPaperFileInput( PassedFileName, pFolder );

        MyFree( 1388, zx, FullKey );
        FullKey = NULL;

        if ( g_bStopAllThreads || pFolder->m_StopThisThread )
            break;
    }

    MyFree( 1395, UNPREDICTABLE, pMalVector );
    pMalVector = NULL;

    delete pFolder->pSolFolderToDo;
    pFolder->pSolFolderToDo = NULL;

}
