#pragma once

// these macros depend on the kind of a build
// and they do control what is compiled and what is not
// depending on the kind of a build

#if _HAS_EXCEPTIONS
#define TRY_MS_STL_KERNEL_COMBO
#else // ! _HAS_EXCEPTIONS
#undef   TRY_MS_STL_KERNEL_COMBO
#endif // ! _HAS_EXCEPTIONS

#define SAMPLE_COMDEF_KERNEL_COMBO