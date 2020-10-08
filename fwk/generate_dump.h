#pragma once

#include "meta.h"

/*

Taken from: https://docs.microsoft.com/en-us/windows/win32/dxtecharts/crash-dump-analysis

Extension (c) 2020 by dbj.org -- LICENSE_DBJ

NOTE: SEH routines are intrinsic to the cl.exe
Usage: 

inline void SomeFunction()
{
    __try
    {
        int *pBadPtr = NULL;
        *pBadPtr = 0;
    }
    __except(GenerateDump(GetExceptionInformation()))
    {
    }
}
*/

#include <windows.h>

#include <assert.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <strsafe.h>

//    StringCchPrintfA( szFileName, MAX_PATH, "%s%s", szPath, szAppName );
#undef  win32_sprintf
#define win32_sprintf StringCchPrintfA

#pragma comment(lib, "Dbghelp.lib")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct generate_dump_last_run {
    char  dump_folder_name[MAX_PATH] ; 
    char  dump_file_name[MAX_PATH] ; 
    BOOL finished_ok ;
} generate_dump_last_run;

static generate_dump_last_run dump_last_run ;
static generate_dump_last_run dump_last_run_clean = { {0}, {0}, FALSE } ;

/*
DBJ: warning, currently this is a blocking call to an expensive function
*/
static inline int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
    dump_last_run = dump_last_run_clean  ;

    char  szPath[MAX_PATH] = {0}; 
    char  szFileName[MAX_PATH] = {0}; 
    const char * szAppName = DBJ_APP_NAME;
    const char * szVersion = DBJ_APP_VERSION;
    DWORD dwBufferSize = MAX_PATH;
    HANDLE hDumpFile = 0 ;
    SYSTEMTIME stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;

    GetLocalTime( &stLocalTime );
    GetTempPathA( dwBufferSize, szPath );

    win32_sprintf( szFileName, MAX_PATH, "%s%s", szPath, szAppName );
    CreateDirectoryA( szFileName, NULL );

    win32_sprintf( dump_last_run.dump_folder_name,  MAX_PATH, "%s", szFileName  );

    win32_sprintf( szFileName, MAX_PATH, "%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
               szPath, szAppName, szVersion, 
               stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
               stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
               GetCurrentProcessId(), GetCurrentThreadId());

    hDumpFile = CreateFileA(szFileName, GENERIC_READ|GENERIC_WRITE, 
                FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

    win32_sprintf( dump_last_run.dump_file_name ,  MAX_PATH, "%s", szFileName  );

    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = pExceptionPointers;
    ExpParam.ClientPointers = TRUE;

    BOOL bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
                    hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

    dump_last_run.finished_ok = bMiniDumpSuccessful;
    return EXCEPTION_EXECUTE_HANDLER;
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

