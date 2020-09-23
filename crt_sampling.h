#pragma once
/* (c) 2020 by dbj.org -- license: LICENCE_DBJ */
#include <string.h>

namespace {
    inline void crt_sample_one ( int argc, char ** argv )
    {
        // UCRT throws SE too
        int rez = strcmp( nullptr, nullptr) ;

    }
} 