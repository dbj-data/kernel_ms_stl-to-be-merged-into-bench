/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/*                 A modified "maximally equidistributed" implementation         */
/*                 by Shin Harase, Hiroshima University.                         */
/* ***************************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif

void InitWELLRNG19937(unsigned int *init);

// see bello on how to get to this
extern unsigned int (*WELLRNG19937)(void);

#ifdef __cplusplus
} // extern "C" 
#endif

/*
static int program (int argc , char ** argv ) 
{
    // technicaly 624
    unsigned int seed[1024]{0};
    
    for (size_t K = 0; K < 1024 ; ++K )
   {
     seed[K] = clock() ;
     ::Sleep(0);
   }    
    InitWELLRNG19937( seed );
    for (size_t K = 0; K < 0xFFFF; ++K )
    {
        printf( "%d \t" , WELLRNG19937() ) ;
    }

   return 0;
}

*/