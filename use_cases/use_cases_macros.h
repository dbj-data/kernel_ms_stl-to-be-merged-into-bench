#pragma once
#ifdef __clang__
#pragma clang system_header

// #pragma clang diagnostic push
// #pragma clang diagnostic note "-W#pragma-messages"

#endif // __clang__

// these macros depend on the kind of a build
// and they do control what is compiled and what is not
// depending on the kind of a build

// NOTE! alas clang-cl does not understand the /kernel switch
// thus use /D_HAS_EXCEPTIONS=0 on the CL command line


#if _HAS_EXCEPTIONS
#define TRY_MS_STL_KERNEL_COMBO
#else // ! _HAS_EXCEPTIONS
#undef   TRY_MS_STL_KERNEL_COMBO
#endif // ! _HAS_EXCEPTIONS

#if __cpp_exceptions
#pragma message("__cpp_exceptions is defined")
#else
#pragma message("__cpp_exceptions is NOT defined")
#endif

#define SAMPLE_COMDEF_KERNEL_COMBO

#ifdef __clang__
// #pragma clang diagnostic pop
#endif // __clang__
