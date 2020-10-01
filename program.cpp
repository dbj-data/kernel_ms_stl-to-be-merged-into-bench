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

// --------------------------------------------------------------------------
extern "C" int program (int argc , char ** argv ) 
{

     return 0;
}

