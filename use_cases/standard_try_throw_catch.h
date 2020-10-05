#pragma once
#include "use_cases_macros.h"
#include "../nanoclib/nanoclib.h"

#ifndef TRY_MS_STL_KERNEL_COMBO
#error __FILE__ " requires TRY_MS_STL_KERNEL_COMBO to be defined"
#endif // TRY_MS_STL_KERNEL_COMBO

// MS STL should/does raise SE in /kernel builds
#include <vector>
#include <stdexcept>

inline void standard_try_throw_catch ()
{
    try
    {
        std::vector<const char *> bv_{"true", "true", "true"};
        auto never = bv_.at(22);
    }
    catch (std::out_of_range const &x_)
    {
        DBJ_ERROR(" std::out_of_range: %s ", x_.what());
    }
    catch (std::exception const &x_)
    {
        DBJ_ERROR("std::exception: %s ", x_.what());
    }
    catch (...) {
       DBJ_ERROR("Unknown C++ exception caught!");
    }
}

