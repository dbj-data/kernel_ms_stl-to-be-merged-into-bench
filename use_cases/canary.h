#pragma once
#include "../fwk/nanoclib.h"
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

// we use this canary to test 
// if constructors and destructors 
// are called in SEH builds
// hint: they are, but in one case 
struct canary final 
{
  const char * file_ {};
  const long   line_ {};

  char basename_[0xFF]{0};

 canary ( const char * f_, const long l_) 
 : file_(f_), line_(l_)
 { 
    (void)strncpy( basename_ , strrchr( f_, '\\') + 1, 0xFF );
    dbj_nanoc_win_vt100_initor_();
    DBJ_WARN ("canary %s[%4lu] constructor called", basename_, line_ );
 }
~canary () {
DBJ_WARN ("canary %s[%4lu] destructor called", basename_, line_ );
}

}; // canary

canary in_the_coal_mine(__FILE__, __LINE__ ) ;
