/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ
Kkernel switch, what is that?
*/

/*
#include "fwk/vt100win10.h"
#include "fwk/macros.h"
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef _KERNEL_MODE
#error This must be compiled with the /kernel switch
#endif // ! _KERNEL_MODE

#if _HAS_EXCEPTIONS
#error _HAS_EXCEPTIONS must be 0 in this build
#endif //  _HAS_EXCEPTIONS

// WARNING! as of 2020OCT01 comdef.h + /kernel works by accident
//
#include <comdef.h>

// --------------------------------------------------------------------------
extern "C" int program (int argc , char ** argv ) 
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
  } catch ( ... ) {
       printf("C++ exception caught!");
  }
#endif // _HAS_EXCEPTIONS
          return 0;
}

