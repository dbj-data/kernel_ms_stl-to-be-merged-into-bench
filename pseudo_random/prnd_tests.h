#pragma once
/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ

Just some shenanigans to provide for some rudimentary performance 
testing on the future ROADMAP
*/

#include "well.h"
#include "lfsr113.h"
#include "well512.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

inline int pseudo_random_tests (int argc , char ** argv ) 
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

#ifdef __cplusplus
} // extern "C" 
#endif