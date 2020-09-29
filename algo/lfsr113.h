#pragma once
/* transpiled to C by dbj@dbj.org */
#ifndef LFSR_INC
#define LFSR_INC

#include <time.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
[L’Ecuyer99] combined LFSR Tausworthe generators LFSR113 and
LFSR258 designed specially for 32-bit and 64-bit computers, respectively, with periods
of approximately 2^113 and 2^258, respectively.
*/
/* 
 NOTE: the seed MUST satisfy
 z1 > 1, z2 > 7, z3 > 15, and z4 > 127 
*/
typedef struct lfsr113seeds {
unsigned long z1, z2, z3, z4; /* the state */
} lfsr113seeds ;

inline unsigned long lfsr113(lfsr113seeds seeds )
 { /* Generates random 32 bit numbers. */
 unsigned long b;
 b = (((seeds.z1 << 6) ^ seeds.z1) >> 13);
 seeds.z1 = (((seeds.z1 & 4294967294) << 18) ^ b);
 b = (((seeds.z2 << 2) ^ seeds.z2) >> 27);
 seeds.z2 = (((seeds.z2 & 4294967288) << 2) ^ b);
 b = (((seeds.z3 << 13) ^ seeds.z3) >> 21);
 seeds.z3 = (((seeds.z3 & 4294967280) << 7) ^ b);
 b = (((seeds.z4 << 3) ^ seeds.z4) >> 12);
 seeds.z4 = (((seeds.z4 & 4294967168) << 13) ^ b);
 return (seeds.z1 ^ seeds.z2 ^ seeds.z3 ^ seeds.z4);
 }

/* 
 NOTE: the seed MUST satisfy
 z1 > 1, z2 > 7, z3 > 15, and z4 > 127 
 return 0 on arguments bad 
 */
inline unsigned long lfsr113_checked_call(lfsr113seeds seeds )
 {
     if ( !(seeds.z1 > 1) && !(seeds.z2 > 7) && !(seeds.z3 > 15) && !(seeds.z4 > 127) )
     {
         return 0UL;
     }
     return lfsr113( seeds ) ;
 }

 inline lfsr113seeds lfsr113_seeds ( void )
 {
    srand( (unsigned)time( NULL ) );
    lfsr113seeds seeds = { 
        (unsigned long)clock() + rand(), 
        (unsigned long)clock() + rand(), 
        (unsigned long)clock() + rand(), 
        (unsigned long)clock() + rand() 
        };
    return seeds ;
 }

#ifdef __cplusplus
} // extern "C" 
#endif

#endif // LFSR_INC
