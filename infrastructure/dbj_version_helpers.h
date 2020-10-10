#pragma once
#ifndef dbj_version_helpers_inc_
#define dbj_version_helpers_inc_
/*

 https://docs.microsoft.com/en-us/windows/win32/api/versionhelpers/nf-versionhelpers-iswindowsxporgreater

These APIs are defined by versionhelpers.h, which is included in the Windows 8.1 software development kit (SDK). 
This file can be used with other Microsoft Visual Studio releases to implement the same functionality 
for Windows versions prior to Windows 8.1.

 */
/******************************************************************
*                                                                 *
*  VersionHelpers.h -- This module defines helper functions to    *
*                      promote version check with proper          *
*                      comparisons.                               *
*                                                                 *
*  Copyright (c) Microsoft Corp.  All rights reserved.            *
*                                                                 *
******************************************************************/

#include <winapifamily.h>
#include <specstrings.h>    // for _In_, etc.

// #ifndef WINAPI_FAMILY_PARTITION( WINAPI_PARTITION_DESKTOP )
// #error only for  WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
// #endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */


#if defined(__midl) && defined(SORTPP_PASS)
#error MIDL usage is not supported
#endif // defined(__midl)

#if (NTDDI_VERSION < NTDDI_WINXP)
#error Versions prior to XP are not supported
#endif // NTDDI_VERSION


#ifdef __cplusplus
#define VERSIONHELPERAPI inline bool
#else  // ! __cplusplus
#define VERSIONHELPERAPI FORCEINLINE BOOL
#endif // ! __cplusplus

VERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(
        VerSetConditionMask(
        VerSetConditionMask(
            0, VER_MAJORVERSION, VER_GREATER_EQUAL),
               VER_MINORVERSION, VER_GREATER_EQUAL),
               VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = wMajorVersion;
    osvi.dwMinorVersion = wMinorVersion;
    osvi.wServicePackMajor = wServicePackMajor;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

VERSIONHELPERAPI
IsWindowsXPOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}

VERSIONHELPERAPI
IsWindowsXPSP1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
}

VERSIONHELPERAPI
IsWindowsXPSP2OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
}

VERSIONHELPERAPI
IsWindowsXPSP3OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

VERSIONHELPERAPI
IsWindowsVistaOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

VERSIONHELPERAPI
IsWindowsVistaSP1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
}

VERSIONHELPERAPI
IsWindowsVistaSP2OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
}

VERSIONHELPERAPI
IsWindows7OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}

VERSIONHELPERAPI
IsWindows7SP1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

VERSIONHELPERAPI
IsWindows8OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

VERSIONHELPERAPI
IsWindows8Point1OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
}

VERSIONHELPERAPI
IsWindowsThresholdOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINTHRESHOLD), LOBYTE(_WIN32_WINNT_WINTHRESHOLD), 0);
}

VERSIONHELPERAPI
IsWindows10OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINTHRESHOLD), LOBYTE(_WIN32_WINNT_WINTHRESHOLD), 0);
}

VERSIONHELPERAPI
IsWindowsServer()
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0, 0, VER_NT_WORKSTATION };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask( 0, VER_PRODUCT_TYPE, VER_EQUAL );

    return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}

VERSIONHELPERAPI
IsActiveSessionCountLimited()
{
    BOOL fActiveSessionCountLimited = FALSE;
    DWORDLONG dwlConditionMask = 0;
    OSVERSIONINFOEX VersionInfo = { 0 };

    VER_SET_CONDITION(dwlConditionMask, VER_SUITENAME, VER_AND);
    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    VersionInfo.wSuiteMask = VER_SUITE_TERMINAL;
    BOOL fSuiteTerminal = VerifyVersionInfo( &VersionInfo, VER_SUITENAME, dwlConditionMask );

    VersionInfo.wSuiteMask = VER_SUITE_SINGLEUSERTS;
    BOOL fSuiteSingleUserTS = VerifyVersionInfo( &VersionInfo, VER_SUITENAME, dwlConditionMask );
		
    fActiveSessionCountLimited  = !(fSuiteTerminal & !fSuiteSingleUserTS);

    return fActiveSessionCountLimited;
}

#endif // dbj_version_helpers_inc_






