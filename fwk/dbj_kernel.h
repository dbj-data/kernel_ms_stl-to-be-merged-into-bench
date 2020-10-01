#pragma once
/*
whatever /kernel build needs or might need
*/
#include "macros.h"
#include "generate_dump.h"
#include "vt100win10.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <new>

// --------------------------------------------------------------------------
/*
Is this for C++ exceptions, for SEH or for both?

<vcruntime.h> #100

#ifndef _HAS_EXCEPTIONS // Predefine as 0 to disable exceptions
    #ifdef _KERNEL_MODE
        #define _HAS_EXCEPTIONS 0
    #else
        #define _HAS_EXCEPTIONS 1
    #endif // _KERNEL_MODE 
#endif // _HAS_EXCEPTIONS 

*/
static_assert( _HAS_EXCEPTIONS == 0 ,
 __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : _HAS_EXCEPTIONS should be 0 as _KERNEL_MODE should be defined?" 
 ) ;

// --------------------------------------------------------------------------
// optional
void my_handler() noexcept 
{
    puts("Memory allocation failed, terminating");
    exit( EXIT_FAILURE);
}
// --------------------------------------------------------------------------
// optional
// #ifdef __cpp_aligned_new
namespace my
{
    enum class align_val_t : size_t {};
}
// #endif // __cpp_aligned_new
// --------------------------------------------------------------------------
// optional
inline void* operator new  ( size_t count ){  return calloc(1, count) ;}
inline void* operator new  ( size_t count, my::align_val_t al ) { return calloc(1, count) ; }
inline void operator delete  ( void* ptr ) noexcept { free(ptr); }
inline void operator delete  ( void* ptr, my::align_val_t al ) noexcept {free(ptr); }

// --------------------------------------------------------------------------
extern "C" int program (int argc , char ** argv ) ;
// --------------------------------------------------------------------------
/*
Policy is very simple: 

use /kernel switch
do not use any /EH switch
have only one top level function as bellow to catch SE
generate minidump if SE is raised / caught

ps: can somebody decipher these remarks bellow, fully?
https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=vs-2019#default-exception-handling-behavior
*/
extern "C" static int dbj_main (int argc, char ** argv) 
{
     __try 
    { 
        __try {
        std::set_new_handler(my_handler);
        errno = 0 ;
        system(" "); // hack for WIN10 cmd.exe VT100 switch on
            DBJ_PROMPT( DBJ_APP_NAME " " DBJ_APP_VERSION );
                program(argc,argv);
        }
        __finally {
            DBJ_PROMPT( DBJ_APP_NAME ":  __finally block visited");
        }
    }
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
        // MS STL "throws" are caught here
        // as now they are SEH
         DBJ_PROMPT( DBJ_APP_NAME ": SEH Exception caught");
        
        DBJ_PROMPT( dump_last_run.finished_ok == TRUE 
        ? "minidump creation succeeded" 
        : "minidump creation failed"  
        );

        if ( dump_last_run.finished_ok ) {
            DBJ_PROMPT( dump_last_run.dump_folder_name);
            DBJ_PROMPT( dump_last_run.dump_file_name);
        }
         DBJ_PROMPT( "Open the minidump in Visual Studio...");

    } 
        DBJ_PROMPT( DBJ_APP_NAME ": Leaving...");
        system("@pause");
        return 42;
} // main
