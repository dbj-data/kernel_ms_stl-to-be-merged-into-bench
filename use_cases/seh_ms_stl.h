#pragma once
#include "use_cases_macros.h"
#include "../nanoclib/nanoclib.h"

#if _HAS_EXCEPTIONS
#error this is for sampling SEH MS STL combo only
#endif // ! _HAS_EXCEPTIONS

// MS STL should/does raise SE in /kernel builds
#include <vector>
#include <stdexcept>

// error C2980: C++ exception handling is not supported with /kernel
// try / throw / catch are not part of a language any more
inline void seh_ms_stl ()
{
// problem: this instance dtor is not visited
// it is in the same scope from where SE is raised
volatile canary in_the_abadon(__FILE__, __LINE__ ) ;

auto * p = new char[0xFF];

delete [] p ;

    std::vector<const char *> bv_{"true", "true", "true"};
    // raise the SE
    auto never = bv_.at(22);


}

