#pragma once
#include "use_cases_macros.h"
#include "../nanoclib/nanoclib.h"

#if _HAS_EXCEPTIONS
#error this is for sampling SEH MS STL combo only
#endif // ! _HAS_EXCEPTIONS

// MS STL should/does raise SE in /kernel builds
#include <vector>
#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>

// error C2980: C++ exception handling is not supported with /kernel
// try / throw / catch are not part of a language any more
// but, it seems everything else works
inline void seh_ms_stl ()
{
    using namespace std::chrono_literals;
// problem/issue/fact: this instance dtor is not visited
// it is in the same scope from where SE is raised
volatile canary in_the_abadon(__FILE__, __LINE__ ) ;

auto start = std::chrono::high_resolution_clock::now();
auto * p = new char[0xFF];
std::this_thread::sleep_for(1s);
delete [] p ;
auto end = std::chrono::high_resolution_clock::now();
std::chrono::duration<double, std::milli> elapsed = end-start;
auto elapsed_ = elapsed.count() ;

    std::vector<std::string> bv_{"true", "true", "true"};
    // raise the SE
    auto never = bv_.at(22);
}

