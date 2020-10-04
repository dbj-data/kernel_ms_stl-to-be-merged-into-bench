#pragma once
/*
whatever /kernel build needs or might need
*/
#include "meta.h"
#include "generate_dump.h"
#include "../nanoclib/vt100.h"
#include "../nanoclib/nanoclib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <new>

// --------------------------------------------------------------------------
struct command_line final {
    
    command_line() = delete ;

    explicit command_line( int argc , char ** argv ) noexcept :
       argc_{argc} ,
       args( argv + 1 ) , 
       prog_path( argv[0])       
    {
        strcpy( prog_basename, strrchr( prog_path, '\\') + 1 );
    }
 
  const int argc_{0} ;
  char ** args{0};
  const char * prog_path ;
  char prog_basename[0xFF]{0};
}; // command_line

// --------------------------------------------------------------------------
/*
<vcruntime.h> #100

#ifndef _HAS_EXCEPTIONS // Predefine as 0 to disable exceptions
    #ifdef _KERNEL_MODE
        #define _HAS_EXCEPTIONS 0
    #else
        #define _HAS_EXCEPTIONS 1
    #endif // _KERNEL_MODE 
#endif // _HAS_EXCEPTIONS 

*/
// --------------------------------------------------------------------------
// optional
void my_handler() noexcept 
{
    puts("Memory allocation failed, terminating");
    exit( EXIT_FAILURE);
}
// --------------------------------------------------------------------------
// https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019
// optional?
// somehow /kernel switch is confusing me
// it mentions the new and delete operator  
// so they appear as not optional?
// but. it seems CL will always compile and linker will link
// with or without these in any kind of C++ builds
#ifdef DBJ_FOLLOWS_KERNEL_ADVICE
// optional
#ifdef __cpp_aligned_new
namespace my {  enum class align_val_t : size_t {}; }
#endif // __cpp_aligned_new
inline void* operator new  ( size_t count ){  return calloc(1, count) ;}
#ifdef __cpp_aligned_new
inline void* operator new  ( size_t count, my::align_val_t al ) { return calloc(1, count) ; }
#endif
inline void operator delete  ( void* ptr ) noexcept { free(ptr); }
#ifdef __cpp_aligned_new
inline void operator delete  ( void* ptr, my::align_val_t al ) noexcept {free(ptr); }
#endif

#endif // DBJ_FOLLOWS_KERNEL_ADVICE

// --------------------------------------------------------------------------
// user code start point
extern "C" int program (int argc , char ** argv ) ;
// --------------------------------------------------------------------------
/*
Policy is very simple: 

use /kernel switch or simply do not use any /EH switch
have only one top level function as bellow to catch SE
then generate minidump if SE is raised.

Although this function bellow works always. With or without
SEH buils. As SEH is intrinsic to Windows.

*/
extern "C" static int dbj_main (int argc, char ** argv) 
{
     __try 
    { 
        __try {
        std::set_new_handler(my_handler);
        errno = 0 ;
            DBJ_INFO( DBJ_APP_NAME ": " DBJ_APP_VERSION );
            command_line cli(argc,argv);
                program(argc,argv);
        }
        __finally {
            DBJ_INFO( DBJ_APP_NAME ":  __finally block visited");
        }
    }
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
        // MS STL "throws" are raised SE's under /kernel builds
        // caught here and nowhere else in the app
         DBJ_ERROR( DBJ_APP_NAME ": SEH Exception caught");
        
        DBJ_WARN( DBJ_APP_NAME ": %s", 
( dump_last_run.finished_ok == TRUE ? "minidump creation succeeded" : "minidump creation failed" ) 
        );

        if ( dump_last_run.finished_ok ) {
            DBJ_INFO( DBJ_APP_NAME ": Dump folder: %s", dump_last_run.dump_folder_name );
            DBJ_INFO( DBJ_APP_NAME ": Dump file  : %s", dump_last_run.dump_file_name);
        }
         DBJ_INFO( DBJ_APP_NAME ": Open that minidump in Visual Studio...");

    } 
        DBJ_INFO( DBJ_APP_NAME ": Leaving...");
        return 42;
} // main
