/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

Kernel switch, what is that?

*/

#include "nanoclib/nanoclib.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if _HAS_EXCEPTIONS
#define DBJ_CPP_EXCEPTIONS
#else // ! _HAS_EXCEPTIONS
#undef   DBJ_CPP_EXCEPTIONS
#endif _HAS_EXCEPTIONS

#undef TRY_COMDEF_KERNEL_COMBO
#ifdef TRY_COMDEF_KERNEL_COMBO
// WARNING! as of 2020OCT01 comdef.h + /kernel, seemingly works by accident
#include <comdef.h>
#endif // TRY_COMDEF_KERNEL_COMBO

#define TRY_MS_STL_KERNEL_COMBO
#ifdef TRY_MS_STL_KERNEL_COMBO
// MS STL should/does raise SE in /kernel builds
#include <vector>
#include <stdexcept>
#endif // TRY_MS_STL_KERNEL_COMBO

// we use this canary to test 
// if constructors and destructors 
// are called in SEH builds
// hint: they are
struct canary final 
{
  const char * file_ ;
  const long   line_ ;

 canary ( const char * f_, const long l_) 
 : file_(f_), line_(l_)
 {
     dbj_nanoc_win_vt100_initor_();
DBJ_WARN ("canary constructor %lu", line_ );
 }
~canary () {
DBJ_WARN ("canary destructor %lu", line_ );
}

};

canary in_the_coal_mine(__FILE__, __LINE__ ) ;

// --------------------------------------------------------------------------
extern "C" int program (int argc , char ** argv ) 
{

#ifdef DBJ_CPP_EXCEPTIONS
  try {
#endif // DBJ_CPP_EXCEPTIONS

#ifdef TRY_COMDEF_KERNEL_COMBO
// REPEATED WARNING! as of 2020OCT01 comdef.h + /kernel works by accident
// in that accidental modus operandi SE with this ID is raised, after that
// main (in here) is catching it in the __except block and produces a minidump
// one can open that minidump in VStudio to exactly see what was thrown and
// where was it thrown from, being it C++ o or C
// it all just works, and I see no reason to check that and make that official behaviour?
          _com_raise_error (CO_E_SERVER_START_TIMEOUT);
/*
Above call goes imediately into:
Unable to open 'comraise.cpp': 
Unable to read file 'd:\agent\_work\9\s\src\vctools\Compiler\CxxFE\SL\vccom\comraise.cpp' 
(Error: Unable to resolve non-existing file 'd:\agent\_work\9\s\src\vctools\Compiler\CxxFE\SL\vccom\comraise.cpp').

NOTE: compiling this goes by:
cl	/DNDEBUG /kernel /Fe: .\kerneldbj.exe program.cpp  fwk\main.cpp  pseudo_random\well.c

Thus there is no /HE beside compiler intrinsic /HE
*/          
#endif // TRY_COMDEF_KERNEL_COMBO

#ifdef TRY_MS_STL_KERNEL_COMBO
/*
        std::vector<bool> bv_{ true, true, true } ;
         auto never = bv_.at(22);
that boils down to   
https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp
line# 25       
*/
#ifdef DBJ_CPP_EXCEPTIONS
// MS STL macro _THROW(x) is defined here as throw x
// _THROW(out_of_range("invalid vector<bool> subscript"));
// from https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp # 24
// this is thrown as
 throw std::out_of_range("invalid vector<bool> subscript") ;
#else // ! DBJ_CPP_EXCEPTIONS
// MS STL macro _THROW(x) is defined here as throw x._Raise()
// _THROW(out_of_range("invalid vector<bool> subscript"));
// from https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp # 24
// this is thrown as
std::out_of_range("invalid vector<bool> subscript")._Raise() ;
#endif // ! DBJ_CPP_EXCEPTIONS

#endif // TRY_MS_STL_KERNEL_COMBO

#ifdef DBJ_CPP_EXCEPTIONS
// NOTE: this is not allowed to compile in case /kernel switch is used
// whatever is  the /EHx combination
  } catch ( std::out_of_range const & x_ ) {
       DBJ_ERROR("%s ", x_.what() );
  }
  catch ( _com_error const & comer_ ) { 
    const TCHAR * wm_ = comer_.ErrorMessage() ;
    DBJ_ERROR( " _com_error , message: %TS" , wm_);
  }
  catch ( ... ) {
       DBJ_ERROR("C++ exception caught!");
  }
#endif // DBJ_CPP_EXCEPTIONS

// NOTE! in the /kernel builds or if there are no std exceptions
// this line is not visited
canary in_the_abadon(__FILE__, __LINE__ ) ;

   return 0;
}
//--------------------------------------------------------------------------------------
#ifdef _KERNEL_MODE
#pragma message("_KERNEL_MODE defined")
#else // ! _KERNEL_MODE
#pragma message("_KERNEL_MODE NOT defined")
#endif // !_KERNEL_MODE


#if _HAS_EXCEPTIONS
#pragma message("_HAS_EXCEPTIONS == 1")
#else
#pragma message("_HAS_EXCEPTIONS == 0")
#endif // _HAS_EXCEPTIONS

#if _CPPRTTI 
#pragma message("_CPPRTTI == 1")
#else
#pragma message("_CPPRTTI == 0")
#endif // ! _CPPRTTI

#if _CPPUNWIND 
#pragma message("_CPPUNWIND == 1")
#else // ! _CPPUNWIND 
#pragma message("_CPPUNWIND == 0")
#endif //! _CPPUNWIND 

static canary in_the_bottomles_pit(__FILE__, __LINE__ ) ;


