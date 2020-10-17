
#pragma once
#include "use_cases_macros.h"
#include "../fwk/nanoclib.h"

#ifndef SAMPLE_COMDEF_KERNEL_COMBO
#error __FILE__ " required SAMPLE_COMDEF_KERNEL_COMBO to be defined"
#endif // ! SAMPLE_COMDEF_KERNEL_COMBO

// WARNING! as of 2020OCT01 comdef.h + /kernel, seemingly works by accident
#include <comdef.h>

inline void comdef_kernel_combo()
{
#if _HAS_EXCEPTIONS
  try {
#endif // _HAS_EXCEPTIONS

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
#if _HAS_EXCEPTIONS
  }
  catch ( _com_error const & comer_ ) { 
    const TCHAR * wm_ = comer_.ErrorMessage() ;
    DBJ_ERROR( " _com_error , message: %TS" , wm_);
  }
  catch ( ... ) {
       DBJ_ERROR("Unknown C++ exception caught!");
  }
#endif // _HAS_EXCEPTIONS
}