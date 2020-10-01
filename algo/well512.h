#pragma once
/*
Chris Lomont, www.lomont.org
http://lomont.org/papers/2008/Lomont_PRNG_2008.pdf

very  small programing improvements (c) 2020 by dbj.org
*/

#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WELLRNG512_args {
/* initialize state to random bits */
unsigned long state[16];
/* init should also reset this to 0 */
unsigned int index ;
} WELLRNG512_args ;

inline WELLRNG512_args WELLRNG512_setup (void){
    WELLRNG512_args args_ = { {0}, 0 };
       srand( (unsigned)time( NULL ) );
    for ( unsigned K = 0; K < 16; ++K) 
    {
        args_.state[K] = (unsigned long)(clock() + rand() + K);
    }      
        return args_ ;
}
/* 
   return 32 bit random number 
   dbj: optional locking to be added
*/
inline unsigned long WELLRNG512(WELLRNG512_args args_ )
{
unsigned long a = 0, b = 0, c = 0, d = 0;
    a = args_.state[args_.index];
    c = args_.state[(args_.index+13)&15];
    b = a^c^(a<<16)^(c<<15);
    c = args_.state[(args_.index+9)&15];
    c ^= (c>>11);
    a = args_.state[args_.index] = b^c;
    d = a^((a<<5)&0xDA442D24UL);
    args_.index = (args_.index + 15)&15;
    a = args_.state[args_.index];
    args_.state[args_.index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
return args_.state[args_.index];
}

#ifdef __cplusplus
} // extern "C" 
#endif