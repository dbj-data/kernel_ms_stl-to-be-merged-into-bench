/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

Kernel switch, what is that?

*/

#include "nanoclib/nanoclib.h"
#include "use_cases/canary.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// user code start here
// this is called from framework
// framework is where SE handling is implemented
extern "C" int program (int argc , char ** argv ) 
{
// problem: this instance dtor is not visited
volatile canary in_the_abadon(__FILE__, __LINE__ ) ;

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

// this instance dtor is always visited
static canary in_the_bottomles_pit(__FILE__, __LINE__ ) ;


