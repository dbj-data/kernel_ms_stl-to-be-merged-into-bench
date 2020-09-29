/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ

kernel switch, what is that?

https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019

NOTE: this is MSVC only

This (as it is) does not build with _DEBUG defined. 
Why not? Because the linker has this problem?

main.obj : error LNK2019: unresolved external symbol _CrtDbgReport referenced in function 
"void * __cdecl std::_Allocate_manually_vector_aligned<struct std::_Default_allocate_traits>(unsigned __int64)" (??$_Allocate_manually_vector_aligned@U_Default_allocate_traits@std@@@std@@YAPEAX_K@Z)
*/
#include "top.h"
#include "show_predefined_cl_macros.h"

#include "crt_sampling.h"
#include "generate_dump.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <new.h>
#include <comdef.h>

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
 __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : _HAS_EXCEPTIONS should be 0 as _KERNEL_MODE is defined?" 
 ) ;

#ifdef _CPPUNWIND
#error __FILE__ " _CPPUNWIND should be undefined as _KERNEL_MODE is defined?" 
#endif

// --------------------------------------------------------------------------
// optional
void my_handler() noexcept 
{
    puts("Memory allocation failed, terminating");
    std::terminate();
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
void* operator new  ( std::size_t count ){  return calloc(1, count) ;}
void* operator new  ( std::size_t count, my::align_val_t al ) { return calloc(1, count) ; }
void operator delete  ( void* ptr ) noexcept { free(ptr); }
void operator delete  ( void* ptr, my::align_val_t al ) noexcept {free(ptr); }
// --------------------------------------------------------------------------
static int program (int argc , char ** argv ) 
{
// error C2980: C++ exception handling is not supported with /kernel
// try {
#ifdef _KERNEL_MODE
   // this throws SE in da _KERNEL_MODE
   // and C++ exception otherwise
   _com_raise_error( S_OK ) ;
#endif
//  }
//        catch (std::exception const & sex) {
//          dbg(sex.what());
//  }


}
// --------------------------------------------------------------------------
// destructors also work in /kernel builds

#ifdef _KERNEL_MODE

struct canary final 
{
 canary () {
     dbg("canary constructor in /kernel mode");
 }
 
 ~canary () {
     dbg("canary destructor in /kernel mode");
 }

}; // canary

canary in_a_coal_mine_ ;

#endif // _KERNEL_MODE

// --------------------------------------------------------------------------
int main (int argc, char ** argv) 
{
     __try 
    { 
        __try {
        std::set_new_handler(my_handler);
        errno = 0 ;
            program(argc, argv);
        }
        __finally {
            puts("\n");
            puts(DBJ_APP_NAME ":  __finally block visited");
        }
    }
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
        // MS STL "throws" are caught here
        // as now they are SEH
        puts("\n");
        puts( DBJ_APP_NAME ": SEH Exception caught");
        
        puts( dump_last_run.finished_ok == TRUE 
        ? "minidump creation succeeded" 
        : "minidump creation failed"  
        );

        if ( dump_last_run.finished_ok ) {
            puts(dump_last_run.dump_folder_name);
            puts(dump_last_run.dump_file_name );
        }
        puts("\n");
        puts("Open the minidump in Visual Studio...");

    } 
        puts( "\n");
        puts( DBJ_APP_NAME ": Leaving...");
        return 42;
} // main