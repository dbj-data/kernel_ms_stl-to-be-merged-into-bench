#pragma once
#include "use_cases_macros.h"
#include "../nanoclib/nanoclib.h"

#if _HAS_EXCEPTIONS
#error this is for sampling SEH MS STL combo only
#endif // ! _HAS_EXCEPTIONS

// MS STL should/does raise SE in /kernel builds
#include <vector>
#include <stdexcept>

inline void seh_ms_stl ()
{
    // error C2980: C++ exception handling is not supported with /kernel
    // try / throw / catch are not part of a language any more
        std::vector<const char *> bv_{"true", "true", "true"};
        auto never = bv_.at(22);
}

