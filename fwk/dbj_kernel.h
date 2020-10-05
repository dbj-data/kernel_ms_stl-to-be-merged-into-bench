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

   constexpr inline static int max_arg_count{0xFF};
  int argc_{0} ;
  char * args[max_arg_count]{0};
  char * prog_path{} ;
  char * prog_basename {0};
    
    command_line() = delete ;

    // stay on the safe side,  copy from ** argv
    explicit command_line( int argc , char * const * argv ) noexcept :
       argc_( argc - 1) ,
       prog_path( _strdup(argv[0]) ),
          prog_basename( _strdup( strrchr( prog_path, '\\') + 1) )     
    {
        DBJ_ASSERT(max_arg_count > argc);
         int k = 0;
        while ( k < argc_ ){
           args[k] = _strdup(argv[k] );
           k++ ;
        }
    }

    ~command_line() {
         int k = 0;
        while ( k < argc_ ){
           free( args[k] ); 
           args[k] = nullptr ;
           k++ ;
        }
        free(prog_path);
        free(prog_basename);
    }
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
#undef DBJ_FOLLOWS_KERNEL_ADVICE
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
            command_line cli(argc,argv); // currently not in use, does SEGV on exit
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
