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

#include "algo/well.h"
#include "algo/lfsr113.h"
#include "algo/well512.h"

// #include "fwk/dbj_kernel.h"
#include "fwk/vt100win10.h"
#include "fwk/macros.h"
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
    unsigned int seed[624]{0};
      srand( (unsigned)time( NULL ) );
    for (size_t K = 0; K < 624 ; ++K )
   {
     seed[K] = (unsigned)(rand() + clock() + K );
   }    
    InitWELLRNG19937( seed );

    DBJ_PROMPT( "Pseudo RND WELLRNG19937" );
    for (size_t K = 0; K < 0xF; ++K )
    {
        DBJ_PRNT( VT100_COLOR_SWAP, "%-59d" , WELLRNG19937() ) ;
    }

    DBJ_PROMPT("Pseudo RND lfsr113");
    for (size_t K = 0; K < 0xF; ++K )
    {
        DBJ_PRNT( VT100_COLOR_SWAP, "%-59lu \t" ,  lfsr113( lfsr113_seeds() ) ) ;
    }

    DBJ_PROMPT("Pseudo RND WELLRNG512");
    for (size_t K = 0; K < 0xF; ++K )
    {
        DBJ_PRNT( VT100_COLOR_SWAP, "%-59lu \t" ,  WELLRNG512( WELLRNG512_setup() ) ) ;
    }
     return 0;
}

