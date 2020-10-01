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

#include "fwk/vt100win10.h"
#include "fwk/macros.h"

#include "pseudo_random/prnd_tests.h"

// #include "fwk/dbj_kernel.h"
/*
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <string.h>
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// --------------------------------------------------------------------------
extern "C" int program (int argc , char ** argv ) 
{
  pseudo_random_tests(argc,argv);
     return 0;
}

